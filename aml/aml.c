/* 
 * aml.c - generate Makefile.am from an Automake Markup Language file
 *
 * Copyright (C) 2005 Jean-Yves Lefort <jylefort@brutele.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <glib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

typedef void (*OutputFunc) (xmlNode *node);

static GHashTable *variables = NULL;
static GSList *conditionals = NULL;

static const char *current_filename = NULL;
static char *current_object = NULL;

static void aml_error (xmlNode *node, const char *format, ...);
static char *aml_get_mandatory_property (xmlNode *node, const char *name);

static void aml_append (const char *varname, const char *format, ...);
static void aml_vappend (const char *varname, const char *format, va_list args);

static void aml_object_append (const char *varname,
			       const char *format,
			       ...);
static void aml_object_append_prefix (const char *prefix,
				      const char *varname,
				      const char *format,
				      ...);
static void aml_object_vappend_prefix (const char *prefix,
				       const char *varname,
				       const char *format,
				       va_list args);

static void aml_output_tree (xmlNode *node);
static void aml_output_node (xmlNode *node);
static void aml_output_object (xmlNode *node,
			       const char *type,
			       const char *default_dir);
static void aml_output_program (xmlNode *node);
static void aml_output_library (xmlNode *node);
static void aml_output_variable (xmlNode *node);
static void aml_output_package (xmlNode *node);
static void aml_output_source_real (xmlNode *node, const char *format, ...);
static void aml_output_source (xmlNode *node);
static void aml_output_module (xmlNode *node);
static void aml_output_class (xmlNode *node);
static void aml_output_interface (xmlNode *node);
static void aml_output_if (xmlNode *node);

static void aml_process_file (const char *filename);

static void
aml_error (xmlNode *node, const char *format, ...)
{
  va_list args;
  char *message;

  g_return_if_fail(format != NULL);

  va_start(args, format);
  message = g_strdup_vprintf(format, args);
  va_end(args);

  if (node)
    g_printerr("%s:%i: %s\n", current_filename, node->line, message);
  else if (current_filename)
    g_printerr("%s: %s\n", current_filename, message);
  else
    g_printerr("%s\n", message);

  g_free(message);

  exit(1);
}

static char *
aml_get_mandatory_property (xmlNode *node, const char *name)
{
  char *value;

  g_return_val_if_fail(node != NULL, NULL);
  g_return_val_if_fail(name != NULL, NULL);

  value = xmlGetProp(node, name);
  if (! value)
    aml_error(node, "mandatory property \"%s\" not specified", name);

  return value;
}

static void
aml_append (const char *varname, const char *format, ...)
{
  va_list args;

  g_return_if_fail(varname != NULL);
  g_return_if_fail(format != NULL);

  va_start(args, format);
  aml_vappend(varname, format, args);
  va_end(args);
}

static void
aml_vappend (const char *varname, const char *format, va_list args)
{
  char *value;

  g_return_if_fail(varname != NULL);
  g_return_if_fail(format != NULL);

  value = g_strdup_vprintf(format, args);

  if (g_hash_table_lookup(variables, varname))
    g_print("%s += %s\n", varname, value);
  else
    {
      /*
       * Automake requires variables to be initialized before += can
       * be used on them.
       */

      g_hash_table_insert(variables, g_strdup(varname), GINT_TO_POINTER(TRUE));
      g_print("%s = %s\n", varname, value);
    }

  g_free(value);
}

static void
aml_object_append (const char *varname,
		   const char *format,
		   ...)
{
  va_list args;

  g_return_if_fail(varname != NULL);
  g_return_if_fail(format != NULL);

  va_start(args, format);
  aml_object_vappend_prefix(NULL, varname, format, args);
  va_end(args);
}

static void
aml_object_append_prefix (const char *prefix,
			  const char *varname,
			  const char *format,
			  ...)
{
  va_list args;

  g_return_if_fail(varname != NULL);
  g_return_if_fail(format != NULL);

  va_start(args, format);
  aml_object_vappend_prefix(prefix, varname, format, args);
  va_end(args);
}

static void
aml_object_vappend_prefix (const char *prefix,
			   const char *varname,
			   const char *format,
			   va_list args)
{
  char *full_varname;

  g_return_if_fail(varname != NULL);
  g_return_if_fail(format != NULL);

  if (prefix)
    full_varname = g_strdup_printf("%s_%s_%s", prefix, current_object, varname);
  else
    full_varname = g_strdup_printf("%s_%s", current_object, varname);

  aml_vappend(full_varname, format, args);
  g_free(full_varname);
}

static void
aml_output_tree (xmlNode *node)
{
  g_return_if_fail(node != NULL);

  for (node = node->children; node != NULL; node = node->next)
    if (node->type == XML_ELEMENT_NODE)
      aml_output_node(node);
}

static void
aml_output_node (xmlNode *node)
{
  static const struct
  {
    const char	*name;
    OutputFunc	func;
  } nodes[] = {
    { "program",	aml_output_program },
    { "library",	aml_output_library },
    { "cppflags",	aml_output_variable },
    { "ldflags",	aml_output_variable },
    { "package",	aml_output_package },
    { "source",		aml_output_source },
    { "module",		aml_output_module },
    { "class",		aml_output_class },
    { "interface",	aml_output_interface },
    { "if",		aml_output_if }
  };
  int i;

  g_return_if_fail(node != NULL);

  for (i = 0; i < G_N_ELEMENTS(nodes); i++)
    if (! strcmp(node->name, nodes[i].name))
      {
	nodes[i].func(node);
	return;
      }

  aml_error(node, "unknown element \"%s\"", node->name);
}

static void
aml_output_object (xmlNode *node, const char *type, const char *default_dir)
{
  char *name;
  char *dir;
  char *varname;

  g_return_if_fail(node != NULL);
  g_return_if_fail(type != NULL);
  g_return_if_fail(default_dir != NULL);

  if (current_object)
    aml_error(node, "toplevel objects cannot be nested");

  name = aml_get_mandatory_property(node, "name");

  dir = xmlGetProp(node, "dir");
  if (! dir)
    dir = g_strdup(default_dir);

  varname = g_strdup_printf("%s_%s", dir, type);
  g_free(dir);

  aml_append(varname, "%s", name);
  g_free(varname);

  current_object = g_strdelimit(name, "-.", '_');

  aml_output_tree(node);

  g_free(current_object);
  current_object = NULL;
}

static void
aml_output_program (xmlNode *node)
{
  g_return_if_fail(node != NULL);

  aml_output_object(node, "PROGRAMS", "bin");
}

static void
aml_output_library (xmlNode *node)
{
  g_return_if_fail(node != NULL);

  aml_output_object(node, "LTLIBRARIES", "lib");
}

static void
aml_output_variable (xmlNode *node)
{
  char *content;

  g_return_if_fail(node != NULL);

  content = xmlNodeGetContent(node);
  if (content)
    {
      char *varname;

      varname = g_ascii_strup(node->name, -1);

      aml_object_append(varname, content);

      g_free(varname);
      g_free(content);
    }
}

static void
aml_output_package (xmlNode *node)
{
  char *name;
  char *varname;

  g_return_if_fail(node != NULL);

  name = aml_get_mandatory_property(node, "name");
  varname = g_ascii_strup(g_strdelimit(name, "-", '_'), -1);
  g_free(name);

  aml_object_append("CPPFLAGS", "$(%s_CFLAGS)", varname);
  aml_object_append("LDFLAGS", "$(%s_LIBS)", varname);

  g_free(varname);
}

static void
aml_output_source_real (xmlNode *node, const char *format, ...)
{
  va_list args;
  char *dist;
  char *built;
  char *clean;

  g_return_if_fail(node != NULL);
  g_return_if_fail(format != NULL);

  va_start(args, format);

  dist = xmlGetProp(node, "dist");
  aml_object_vappend_prefix(dist && ! strcmp(dist, "no") ? "nodist" : NULL, "SOURCES", format, args);
  g_free(dist);

  built = xmlGetProp(node, "built");
  if (built)
    {
      if (! strcmp(built, "yes"))
	aml_vappend("BUILT_SOURCES", format, args);
      g_free(built);
    }
  
  clean = xmlGetProp(node, "clean");
  if (clean)
    {
      char *cleanvar;

      if (! strcmp(clean, "yes"))
	cleanvar = g_strdup("CLEANFILES");
      else
	{
	  char *str;

	  str = g_ascii_strup(clean, -1);
	  cleanvar = g_strdup_printf("%sCLEANFILES", str);
	  g_free(str);
	}
      g_free(clean);

      aml_vappend(cleanvar, format, args);
      g_free(cleanvar);
    }

  va_end(args);
}

static void
aml_output_source (xmlNode *node)
{
  char *name;

  g_return_if_fail(node != NULL);

  name = aml_get_mandatory_property(node, "name");
  aml_output_source_real(node, "%s", name);
  g_free(name);
}

static void
aml_output_module (xmlNode *node)
{
  char *name;

  g_return_if_fail(node != NULL);

  name = aml_get_mandatory_property(node, "name");
  aml_output_source_real(node, "%s.c", name);
  aml_output_source_real(node, "%s.h", name);
  g_free(name);
}

static void
aml_output_class (xmlNode *node)
{
  char *name;
  char *prefix;

  g_return_if_fail(node != NULL);

#define GOB_BUILT_SOURCES(prefix) \
  "%s-private.h %s.c %s.h", (prefix), (prefix), (prefix)

  name = aml_get_mandatory_property(node, "name");
  prefix = g_ascii_strdown(g_strdelimit(name, ":", '-'), -1);
  g_free(name);

  aml_object_append("SOURCES", "%s.gob %s.gob.stamp", prefix, prefix);
  aml_object_append("SOURCES", GOB_BUILT_SOURCES(prefix));
  aml_append("BUILT_SOURCES", "%s.gob.stamp", prefix);
  aml_append("MAINTAINERCLEANFILES", "%s.gob.stamp", prefix);
  aml_append("MAINTAINERCLEANFILES", GOB_BUILT_SOURCES(prefix));
  
  g_free(prefix);
}

static void
aml_output_interface (xmlNode *node)
{
  char *name;

  g_return_if_fail(node != NULL);

#define IDL_BUILT_SOURCES(prefix) \
  "%s-common.c %s-skels.c %s-stubs.c %s.h", (prefix), (prefix), (prefix), (prefix)

  name = g_strdelimit(aml_get_mandatory_property(node, "name"), ":", '_');

  aml_object_append("SOURCES", "%s.idl", name);
  aml_object_append_prefix("nodist", "SOURCES", IDL_BUILT_SOURCES(name));
  aml_append("BUILT_SOURCES", IDL_BUILT_SOURCES(name));
  aml_append("CLEANFILES", IDL_BUILT_SOURCES(name));

  g_free(name);
}

static void
aml_output_if (xmlNode *node)
{
  char *test;

  g_return_if_fail(node != NULL);

  test = aml_get_mandatory_property(node, "test");

  conditionals = g_slist_prepend(conditionals, test);
  g_print("if %s\n", test);

  aml_output_tree(node);

  conditionals = g_slist_delete_link(conditionals, conditionals);
  g_print("endif\n");
}

static void
aml_process_file (const char *filename)
{
  xmlDoc *doc;
  xmlNode *root;
  char *basename;

  g_return_if_fail(filename != NULL);

  current_filename = filename;

  doc = xmlParseFile(filename);
  if (! doc)
    exit(1);			/* error printed by libxml */

  root = xmlDocGetRootElement(doc);
  if (! root)
    aml_error(NULL, "root element missing");

  if (strcmp(root->name, "makefile"))
    aml_error(root, "root element must be \"makefile\", not \"%s\"", root->name);

  basename = g_path_get_basename(filename);
  g_print("# Generated from %s using Jean-Yves Lefort's aml.\n", basename);
  g_free(basename);

  g_print("include $(top_srcdir)/aml/aml.make\n");

  aml_output_tree(root);

  g_print("include $(srcdir)/aml.make\n");

  xmlFreeDoc(doc);
}

int
main (int argc, char **argv)
{
  if (argc != 2)
    aml_error(NULL, "Usage: %s INPUTFILE", argv[0]);

  variables = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
  aml_process_file(argv[1]);
  g_hash_table_destroy(variables);

  return 0;
}
