/*
 * JB, the Jean-Yves Lefort's Build System
 * Copyright (C) 2008 Jean-Yves Lefort <jylefort@brutele.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <string.h>
#include "jb-resource.h"
#include "jb-group.h"
#include "jb-variable.h"
#include "jb-util.h"
#include "jb-feature.h"
#include "jb-action.h"

#define INTLTOOL_MERGE_IN	"jbsrc/tools/intltool-merge.in"
#define INTLTOOL_MERGE_OUT	"build/jbsrc/tools/intltool-merge"
#define INTLTOOL_MERGE_CACHE	"build/jbsrc/tools/intltool-merge-cache"

static gboolean
has_template (const char *filename)
{
  char *template_input;
  gboolean result;

  if (jb_templates == NULL)
    return FALSE;

  template_input = g_strdup_printf("%s.in", filename);
  result = jb_string_hash_set_contains(jb_templates, template_input);
  g_free(template_input);

  return result;
}

static char *
get_input_file (const char *filename)
{
  if (has_template(filename))
    return g_strdup_printf("build/%s", filename);
  else
    return g_strdup(filename);
}

static char *
get_output_file (const char *filename)
{
  char *tmp;
  char *result;

  g_return_val_if_fail(filename != NULL, NULL);
  g_return_val_if_fail(g_str_has_suffix(filename, ".in"), NULL);

  tmp = jb_strip_extension(filename);

  /*
   * If the input file is already in build/, do not put the output
   * file in build/build/.
   */
  if (g_str_has_prefix(tmp, "build/"))
    result = g_strdup(tmp);
  else
    result = g_strdup_printf("build/%s", tmp);

  g_free(tmp);

  return result;
}

G_DEFINE_ABSTRACT_TYPE(JBResource, jb_resource, G_TYPE_OBJECT)

char *
jb_resource_to_string (JBResource *self)
{
  JBResourceClass *class;

  g_return_val_if_fail(JB_IS_RESOURCE(self), NULL);

  class = JB_RESOURCE_GET_CLASS(self);

  if (class->to_string != NULL)
    return class->to_string(self);
  else
    {
      g_error("to_string() not implemented for resource type %s", G_OBJECT_TYPE_NAME(self));
      return NULL;
    }
}

void
jb_resource_pre_build (JBResource *self)
{
  JBResourceClass *class;

  g_return_if_fail(JB_IS_RESOURCE(self));

  class = JB_RESOURCE_GET_CLASS(self);

  if (class->pre_build != NULL)
    class->pre_build(self);
}

void
jb_resource_build (JBResource *self)
{
  JBResourceClass *class;

  g_return_if_fail(JB_IS_RESOURCE(self));

  class = JB_RESOURCE_GET_CLASS(self);

  if (class->build != NULL)
    class->build(self);
}

void
jb_resource_install (JBResource *self)
{
  JBResourceClass *class;

  g_return_if_fail(JB_IS_RESOURCE(self));

  class = JB_RESOURCE_GET_CLASS(self);

  if (class->install != NULL)
    class->install(self);
}

void
jb_resource_makedist (JBResource *self)
{
  JBResourceClass *class;

  g_return_if_fail(JB_IS_RESOURCE(self));

  class = JB_RESOURCE_GET_CLASS(self);

  if (class->makedist != NULL)
    class->makedist(self);
}

void
jb_resource_clean (JBResource *self)
{
  JBResourceClass *class;

  g_return_if_fail(JB_IS_RESOURCE(self));

  class = JB_RESOURCE_GET_CLASS(self);

  if (class->clean != NULL)
    class->clean(self);
}

void
jb_resource_distclean (JBResource *self)
{
  JBResourceClass *class;

  g_return_if_fail(JB_IS_RESOURCE(self));

  class = JB_RESOURCE_GET_CLASS(self);

  if (class->distclean != NULL)
    class->distclean(self);
}

void
jb_resource_maintainerclean (JBResource *self)
{
  JBResourceClass *class;

  g_return_if_fail(JB_IS_RESOURCE(self));

  class = JB_RESOURCE_GET_CLASS(self);

  if (class->maintainerclean != NULL)
    class->maintainerclean(self);
}

static void
resource_action_message (JBResource *self, const char *action)
{
  char *repr;

  g_return_if_fail(JB_IS_RESOURCE(self));
  g_return_if_fail(action != NULL);

  repr = jb_resource_to_string(self);
  jb_message("%s %s", action, repr);
  g_free(repr);
}

void
jb_resource_message_building (JBResource *self)
{
  g_return_if_fail(JB_IS_RESOURCE(self));

  resource_action_message(self, "building");
}

static void
jb_resource_init (JBResource *self)
{
}

static void
jb_resource_class_init (JBResourceClass *class)
{
}

G_DEFINE_ABSTRACT_TYPE(JBGroupResource, jb_group_resource, JB_TYPE_RESOURCE)

static void
jb_group_resource_init (JBGroupResource *self)
{
}

static void
jb_group_resource_class_init (JBGroupResourceClass *class)
{
}

G_DEFINE_ABSTRACT_TYPE(JBObjectResource, jb_object_resource, JB_TYPE_RESOURCE)

static void
jb_object_resource_init (JBObjectResource *self)
{
}

static void
jb_object_resource_class_init (JBObjectResourceClass *class)
{
}

static char *
object_resource_get_object_file (JBObjectResource *self)
{
  JBObjectResourceClass *class;

  g_return_val_if_fail(JB_IS_OBJECT_RESOURCE(self), NULL);

  class = JB_OBJECT_RESOURCE_GET_CLASS(self);

  if (class->get_object_file != NULL)
    return class->get_object_file(self);
  else
    {
      g_error("get_object_file() not implemented for object resource type %s", G_OBJECT_TYPE_NAME(self));
      return NULL;
    }
}

G_DEFINE_TYPE(JBTemplate, jb_template, JB_TYPE_GROUP_RESOURCE)

JBTemplate *
jb_template_new (const char *filename)
{
  JBTemplate *self;

  g_return_val_if_fail(filename != NULL, NULL);
  g_return_val_if_fail(g_str_has_suffix(filename, ".in"), NULL);

  self = g_object_new(JB_TYPE_TEMPLATE, NULL);
  self->filename = g_strdup(filename);

  return self;
}

static char *
template_to_string (JBResource *res)
{
  JBTemplate *self = JB_TEMPLATE(res);

  return g_strdup_printf("template %s", self->filename);
}

static void
template_get_files (JBTemplate *self, char **infile, char **outfile)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(self);

  if (infile != NULL)
    *infile = g_strdup_printf("%s/%s", gres->group->srcdir, self->filename);
  if (outfile != NULL)
    {
      char *tmp;

      tmp = jb_strip_extension(self->filename);
      *outfile = g_strdup_printf("%s/%s", gres->group->builddir, tmp);
      g_free(tmp);
    }
}

static void
template_build (JBResource *res)
{
  JBTemplate *self = JB_TEMPLATE(res);
  char *infile;
  char *outfile;

  template_get_files(self, &infile, &outfile);

  if (! jb_is_uptodate(outfile, infile))
    {
      static GHashTable *variables = NULL;

      jb_resource_message_building(res);

      if (variables == NULL)
	{
	  GSList *l;

	  variables = g_hash_table_new(g_str_hash, g_str_equal);

	  JB_LIST_FOREACH(l, jb_variables)
	    {
	      JBVariable *variable = l->data;

	      g_hash_table_insert(variables, variable->name, jb_variable_evaluate(variable));
	    }
	}

      jb_mkdir_of_file(outfile);

      jb_subst(infile, outfile, variables);
    }

  g_free(infile);
  g_free(outfile);
}

static void
template_makedist (JBResource *res)
{
  JBTemplate *self = JB_TEMPLATE(res);
  char *infile;

  template_get_files(self, &infile, NULL);

  jb_action_add_to_dist(infile);

  g_free(infile);
}

static void
template_clean (JBResource *res)
{
  JBTemplate *self = JB_TEMPLATE(res);
  char *outfile;

  template_get_files(self, NULL, &outfile);

  jb_action_rm(outfile);

  g_free(outfile);
}

static void
jb_template_init (JBTemplate *self)
{
}

static void
jb_template_class_init (JBTemplateClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);

  rclass->to_string = template_to_string;
  rclass->build = template_build;
  rclass->makedist = template_makedist;
  rclass->clean = template_clean;
}

G_DEFINE_TYPE(JBDataFile, jb_data_file, JB_TYPE_GROUP_RESOURCE)

JBDataFile *
jb_data_file_new (const char *filename)
{
  JBDataFile *self;

  g_return_val_if_fail(filename != NULL, NULL);

  self = g_object_new(JB_TYPE_DATA_FILE, NULL);
  self->filename = g_strdup(filename);

  return self;
}

static char *
data_file_to_string (JBResource *res)
{
  JBDataFile *self = JB_DATA_FILE(res);

  return g_strdup_printf("data file %s", self->filename);
}

static void
data_file_install (JBResource *res)
{
  JBDataFile *self = JB_DATA_FILE(res);
  JBGroupResource *gres = JB_GROUP_RESOURCE(res);
  char *srcfile;
  char *real_srcfile;
  char *dstfile;

  srcfile = g_strdup_printf("%s/%s", gres->group->srcdir, self->filename);
  real_srcfile = get_input_file(srcfile);
  dstfile = g_path_get_basename(self->filename);

  jb_install_options_install_data(self->install_options, real_srcfile, dstfile);

  g_free(srcfile);
  g_free(real_srcfile);
  g_free(dstfile);
}

static void
data_file_makedist (JBResource *res)
{
  JBDataFile *self = JB_DATA_FILE(res);
  JBGroupResource *gres = JB_GROUP_RESOURCE(res);
  char *file;

  file = g_strdup_printf("%s/%s", gres->group->srcdir, self->filename);

  if (! has_template(file))
    jb_action_add_to_dist(file);

  g_free(file);
}

static void
jb_data_file_init (JBDataFile *self)
{
  self->install_options = jb_install_options_new();
}

static void
jb_data_file_class_init (JBDataFileClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);

  rclass->to_string = data_file_to_string;
  rclass->install = data_file_install;
  rclass->makedist = data_file_makedist;
}

static GSList *
parse_depsfile (const char *depsfile)
{
  char *contents;
  char **lines;
  GSList *deps = NULL;
  int i;

  contents = jb_read_file(depsfile, NULL);
  if (contents == NULL)
    return NULL;		/* depsfile is optional */

  lines = g_strsplit(contents, "\n", 0);
  g_free(contents);

  for (i = 0; lines[i] != NULL; i++)
    {
      char *p;
      char **files;
      int j;

      p = strchr(lines[i], ':');
      if (p != NULL)
	p++;
      else
	p = lines[i];

      files = g_strsplit(p, " ", 0);

      for (j = 0; files[j] != NULL; j++)
	{
	  char *file;

	  file = g_strdup(files[j]);
	  file = g_strstrip(file);

	  if (*file != '\0' && *file != '\\')
	    deps = g_slist_prepend(deps, g_strdup(file));

	  g_free(file);
	}

      g_strfreev(files);
    }

  return deps;
}

static char *
object_file_get_depsfile (const char *ofile)
{
  return g_strdup_printf("%s.deps", ofile);
}

static gboolean
object_file_is_uptodate (const char *ofile, const char *cfile)
{
  char *depsfile;
  GSList *deps;

  if (! jb_variable_get_bool("cc-dependency-tracking"))
    return FALSE;

  if (! jb_is_uptodate(ofile, cfile))
    return FALSE;

  depsfile = object_file_get_depsfile(ofile);
  deps = parse_depsfile(depsfile);
  g_free(depsfile);

  if (deps != NULL)
    {
      gboolean is_uptodate;

      is_uptodate = jb_is_uptodate_list(ofile, deps);

      jb_g_slist_free_deep(deps);

      return is_uptodate;
    }
  else
    return TRUE;
}

static void
object_file_build (JBObject *object, const char *ofile, const char *cfile)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(object);
  JBObjectClass *object_class;
  char *depsfile;
  char *depsflags = NULL;

  object_class = JB_OBJECT_GET_CLASS(object);

  depsfile = object_file_get_depsfile(ofile);

  if (jb_variable_get_bool("cc-dependency-tracking"))
    depsflags = jb_variable_expand(" -MT $ofile -MD -MP -MF $depsfile",
				   "ofile", ofile,
				   "depsfile", depsfile,
				   NULL);

  jb_mkdir_of_file(ofile);

  jb_action_exec("$cc $package-cflags $group-cflags $object-cflags $stock-cflags $cflags"
		 " $package-cppflags $group-cppflags $object-cppflags $stock-cppflags $cppflags"
		 " $depsflags -c -o $ofile $cfile",
		 "package-cflags", jb_compile_options_get_cflags(jb_compile_options),
		 "group-cflags", jb_compile_options_get_cflags(gres->group->compile_options),
		 "object-cflags", jb_compile_options_get_cflags(object->compile_options),
		 "stock-cflags", object_class->stock_cflags,
		 "package-cppflags", jb_compile_options_get_cppflags(jb_compile_options),
		 "group-cppflags", jb_compile_options_get_cppflags(gres->group->compile_options),
		 "object-cppflags", jb_compile_options_get_cppflags(object->compile_options),
		 "stock-cppflags", object_class->stock_cppflags,
		 "depsflags", depsflags,
		 "ofile", ofile,
		 "cfile", cfile,
		 NULL);

  g_free(depsfile);
  g_free(depsflags);
}

static void
object_file_clean (const char *ofile)
{
  char *depsfile;

  depsfile = object_file_get_depsfile(ofile);

  jb_action_rm(ofile);
  jb_action_rm(depsfile);

  g_free(depsfile);
}

G_DEFINE_TYPE(JBSource, jb_source, JB_TYPE_OBJECT_RESOURCE)

JBSource *
jb_source_new (const char *filename)
{
  JBSource *self;

  g_return_val_if_fail(filename != NULL, NULL);

  self = g_object_new(JB_TYPE_SOURCE, NULL);
  self->filename = g_strdup(filename);

  return self;
}

static char *
source_to_string (JBResource *res)
{
  JBSource *self = JB_SOURCE(res);
  char *base_filename;
  char *str;

  base_filename = g_path_get_basename(self->filename);
  str = g_strdup_printf("C source %s", base_filename);
  g_free(base_filename);

  return str;
}

static void
source_get_files (JBSource *self, char **ofile, char **cfile)
{
  JBObjectResource *ores = JB_OBJECT_RESOURCE(self);
  JBGroupResource *gres = JB_GROUP_RESOURCE(ores->object);
  char *base_filename;
  char *base_prefix;

  base_filename = g_path_get_basename(self->filename);
  base_prefix = jb_strip_extension(base_filename);

  if (ofile != NULL)
    *ofile = g_strdup_printf("%s/%s-%s.o",
			     gres->group->builddir,
			     ores->object->name,
			     base_prefix);
  if (cfile != NULL)
    *cfile = g_strdup_printf("%s/%s", gres->group->srcdir, self->filename);

  g_free(base_filename);
  g_free(base_prefix);
}

static void
source_build (JBResource *res)
{
  JBObjectResource *ores = JB_OBJECT_RESOURCE(res);
  JBSource *self = JB_SOURCE(res);
  char *ofile;
  char *cfile;

  source_get_files(self, &ofile, &cfile);

  if (! object_file_is_uptodate(ofile, cfile))
    {
      jb_resource_message_building(res);
      object_file_build(ores->object, ofile, cfile);
    }

  g_free(ofile);
  g_free(cfile);
}

static void
source_makedist (JBResource *res)
{
  JBSource *self = JB_SOURCE(res);
  char *cfile;

  source_get_files(self, NULL, &cfile);

  jb_action_add_to_dist(cfile);

  g_free(cfile);
}

static void
source_clean (JBResource *res)
{
  JBSource *self = JB_SOURCE(res);
  char *ofile;

  source_get_files(self, &ofile, NULL);

  object_file_clean(ofile);

  g_free(ofile);
}

static char *
source_get_object_file (JBObjectResource *res)
{
  JBSource *self = JB_SOURCE(res);
  char *ofile;

  source_get_files(self, &ofile, NULL);

  return ofile;
}

static void
jb_source_init (JBSource *self)
{
}

static void
jb_source_class_init (JBSourceClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);
  JBObjectResourceClass *orclass = JB_OBJECT_RESOURCE_CLASS(class);

  rclass->to_string = source_to_string;
  rclass->build = source_build;
  rclass->makedist = source_makedist;
  rclass->clean = source_clean;

  orclass->get_object_file = source_get_object_file;
}

G_DEFINE_TYPE(JBGobClass, jb_gob_class, JB_TYPE_OBJECT_RESOURCE)

JBGobClass *
jb_gob_class_new (const char *name)
{
  JBGobClass *self;

  g_return_val_if_fail(name != NULL, NULL);
  g_return_val_if_fail(jb_feature_is_enabled(&jb_gob2_feature), NULL);

  self = g_object_new(JB_TYPE_GOB_CLASS, NULL);
  self->name = g_strdup(name);

  return self;
}

static void
gob_class_get_gob_files (JBGobClass *self, char **gobfile, char **stamp)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(JB_OBJECT_RESOURCE(self)->object);
  char *prefix;

  prefix = g_strdelimit(g_ascii_strdown(self->name, -1), ":", '-');
  if (gobfile != NULL)
    *gobfile = g_strdup_printf("%s/%s.gob", gres->group->srcdir, prefix);
  if (stamp != NULL)
    *stamp = g_strdup_printf("%s/%s.gob.stamp", gres->group->builddir, prefix);
  g_free(prefix);
}

static void
gob_class_pre_build (JBResource *res)
{
  JBGobClass *self = JB_GOB_CLASS(res);
  char *gobfile;
  char *stamp;

  gob_class_get_gob_files(self, &gobfile, &stamp);

  if (! jb_is_uptodate(stamp, gobfile))
    {
      JBObjectResource *ores = JB_OBJECT_RESOURCE(res);
      JBGroupResource *gres = JB_GROUP_RESOURCE(ores->object);
      const char *error;

      error = jb_variable_get_string_or_null("gob2-error");
      if (error != NULL)
	jb_error("%s", error);

      jb_message("building GOB class %s", self->name);

      jb_mkdir(gres->group->builddir);

      jb_action_exec("$gob2 $package-gob2flags $group-gob2flags $object-gob2flags $gobfile",
		     "package-gob2flags", jb_compile_options_get_gob2flags(jb_compile_options),
		     "group-gob2flags", jb_compile_options_get_gob2flags(gres->group->compile_options),
		     "object-gob2flags", jb_compile_options_get_gob2flags(ores->object->compile_options),
		     "gobfile", gobfile,
		     NULL);

      jb_action_exec("touch $stamp",
		     "stamp", stamp,
		     NULL);
    }

  g_free(gobfile);
  g_free(stamp);
}

static void
gob_class_get_c_files (JBGobClass *self,
		       char **ofile,
		       char **cfile,
		       char **hfile,
		       char **phfile)
{
  JBObjectResource *ores = JB_OBJECT_RESOURCE(self);
  JBGroupResource *gres = JB_GROUP_RESOURCE(ores->object);
  char *prefix;

  prefix = g_strdelimit(g_ascii_strdown(self->name, -1), ":", '-');

  if (ofile != NULL)
    *ofile = g_strdup_printf("%s/%s-%s.o",
			     gres->group->builddir,
			     ores->object->name,
			     prefix);
  if (cfile != NULL)
    *cfile = g_strdup_printf("%s/%s.c", gres->group->builddir, prefix);
  if (hfile != NULL)
    *hfile = g_strdup_printf("%s/%s.h", gres->group->builddir, prefix);
  if (phfile != NULL)
    *phfile = g_strdup_printf("%s/%s-private.h", gres->group->builddir, prefix);

  g_free(prefix);
}

static void
gob_class_build (JBResource *res)
{
  JBGobClass *self = JB_GOB_CLASS(res);
  char *ofile;
  char *cfile;

  gob_class_get_c_files(self, &ofile, &cfile, NULL, NULL);

  if (! object_file_is_uptodate(ofile, cfile))
    {
      JBObjectResource *ores = JB_OBJECT_RESOURCE(res);
      char *class_name;

      class_name = jb_strip_chars(self->name, ":");
      jb_message("building class %s", class_name);
      g_free(class_name);

      object_file_build(ores->object, ofile, cfile);
    }

  g_free(ofile);
  g_free(cfile);
}

static void
gob_class_makedist (JBResource *res)
{
  JBGobClass *self = JB_GOB_CLASS(res);
  char *gobfile;
  char *stamp;
  char *cfile;
  char *hfile;
  char *phfile;

  gob_class_get_gob_files(self, &gobfile, &stamp);
  gob_class_get_c_files(self, NULL, &cfile, &hfile, &phfile);

  jb_action_add_to_dist(gobfile);
  jb_action_add_to_dist(stamp);
  jb_action_add_to_dist(cfile);
  jb_action_add_to_dist(hfile);
  jb_action_add_to_dist(phfile);

  g_free(gobfile);
  g_free(stamp);
  g_free(cfile);
  g_free(hfile);
  g_free(phfile);
}

static void
gob_class_clean (JBResource *res)
{
  JBGobClass *self = JB_GOB_CLASS(res);
  char *ofile;

  gob_class_get_c_files(self, &ofile, NULL, NULL, NULL);

  object_file_clean(ofile);

  g_free(ofile);
}

static void
gob_class_maintainerclean (JBResource *res)
{
  JBGobClass *self = JB_GOB_CLASS(res);
  char *stamp;
  char *cfile;
  char *hfile;
  char *phfile;

  gob_class_get_gob_files(self, NULL, &stamp);
  gob_class_get_c_files(self, NULL, &cfile, &hfile, &phfile);

  jb_action_rm(stamp);
  jb_action_rm(cfile);
  jb_action_rm(hfile);
  jb_action_rm(phfile);

  g_free(stamp);
  g_free(cfile);
  g_free(hfile);
  g_free(phfile);
}

static char *
gob_class_get_object_file (JBObjectResource *res)
{
  JBGobClass *self = JB_GOB_CLASS(res);
  char *ofile;

  gob_class_get_c_files(self, &ofile, NULL, NULL, NULL);

  return ofile;
}

static void
jb_gob_class_init (JBGobClass *self)
{
}

static void
jb_gob_class_class_init (JBGobClassClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);
  JBObjectResourceClass *orclass = JB_OBJECT_RESOURCE_CLASS(class);

  rclass->pre_build = gob_class_pre_build;
  rclass->build = gob_class_build;
  rclass->makedist = gob_class_makedist;
  rclass->clean = gob_class_clean;
  rclass->maintainerclean = gob_class_maintainerclean;

  orclass->get_object_file = gob_class_get_object_file;
}

G_DEFINE_TYPE(JBIntltoolFile, jb_intltool_file, JB_TYPE_GROUP_RESOURCE)

JBIntltoolFile *
jb_intltool_file_new (const char *type,
		      const char *filename,
		      const char *merge_flags)
{
  JBIntltoolFile *self;

  g_return_val_if_fail(type != NULL, NULL);
  g_return_val_if_fail(filename != NULL, NULL);
  g_return_val_if_fail(g_str_has_suffix(filename, ".in"), NULL);
  g_return_val_if_fail(merge_flags != NULL, NULL);

  self = g_object_new(JB_TYPE_INTLTOOL_FILE, NULL);
  self->type = g_strdup(type);
  self->filename = g_strdup(filename);
  self->merge_flags = g_strdup(merge_flags);

  return self;
}

static char *
intltool_file_to_string (JBResource *res)
{
  JBIntltoolFile *self = JB_INTLTOOL_FILE(res);

  return g_strdup_printf("%s %s", self->type, self->filename);
}

static void
intltool_file_get_files (JBIntltoolFile *self, char **infile, char **outfile)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(self);
  char *filename;

  filename = g_strdup_printf("%s/%s", gres->group->srcdir, self->filename);

  if (infile != NULL)
    *infile = get_input_file(filename);
  if (outfile != NULL)
    *outfile = get_output_file(filename);

  g_free(filename);
}

static void
intltool_file_build (JBResource *res)
{
  JBIntltoolFile *self = JB_INTLTOOL_FILE(res);
  char *infile;
  char *outfile;
  GSList *deps = NULL;

  if (! jb_is_uptodate(INTLTOOL_MERGE_OUT, INTLTOOL_MERGE_IN))
    {
      GHashTable *variables;

      jb_message("building intltool-merge");

      jb_mkdir_of_file(INTLTOOL_MERGE_OUT);

      variables = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, g_free);
      g_hash_table_insert(variables, "INTLTOOL_PERL", jb_variable_expand("$perl", NULL));
      g_hash_table_insert(variables, "INTLTOOL_LIBDIR", jb_variable_expand("$perl", NULL));

      jb_subst(INTLTOOL_MERGE_IN, INTLTOOL_MERGE_OUT, variables);

      g_hash_table_destroy(variables);

      jb_chmod(INTLTOOL_MERGE_OUT, 0755);
    }

  intltool_file_get_files(self, &infile, &outfile);

  deps = g_slist_append(deps, g_strdup(infile));
  deps = g_slist_append(deps, g_strdup(INTLTOOL_MERGE_OUT));
  deps = g_slist_concat(deps, jb_match_files("po/*.po"));

  if (! jb_is_uptodate_list(outfile, deps))
    {
      jb_resource_message_building(res);

      jb_mkdir_of_file(outfile);
      jb_mkdir_of_file(INTLTOOL_MERGE_CACHE);

      jb_action_exec("LC_ALL=C " INTLTOOL_MERGE_OUT " $merge-flags -u -c " INTLTOOL_MERGE_CACHE " po $infile $outfile",
		     "merge-flags", self->merge_flags,
		     "infile", infile,
		     "outfile", outfile,
		     NULL);
    }

  g_free(infile);
  g_free(outfile);
  jb_g_slist_free_deep(deps);
}

static void
intltool_file_install (JBResource *res)
{
  JBIntltoolFile *self = JB_INTLTOOL_FILE(res);
  char *outfile;

  intltool_file_get_files(self, NULL, &outfile);

  jb_install_options_install_data(self->install_options, outfile, NULL);

  g_free(outfile);
}

static void
intltool_file_makedist (JBResource *res)
{
  JBIntltoolFile *self = JB_INTLTOOL_FILE(res);
  JBGroupResource *gres = JB_GROUP_RESOURCE(res);
  char *filename;

  filename = g_strdup_printf("%s/%s", gres->group->srcdir, self->filename);

  jb_action_add_to_dist(INTLTOOL_MERGE_IN);

  if (! has_template(filename))
    jb_action_add_to_dist(filename);

  g_free(filename);
}

static void
intltool_file_clean (JBResource *res)
{
  JBIntltoolFile *self = JB_INTLTOOL_FILE(res);
  char *outfile;

  intltool_file_get_files(self, NULL, &outfile);

  jb_action_rm(INTLTOOL_MERGE_OUT);
  jb_action_rm(INTLTOOL_MERGE_CACHE);
  jb_action_rm(outfile);

  g_free(outfile);
}

static void
intltool_file_maintainerclean (JBResource *res)
{
  jb_action_rm(INTLTOOL_MERGE_IN);
}

static void
jb_intltool_file_init (JBIntltoolFile *self)
{
  self->install_options = jb_install_options_new();
}

static void
jb_intltool_file_class_init (JBIntltoolFileClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);

  rclass->to_string = intltool_file_to_string;
  rclass->build = intltool_file_build;
  rclass->install = intltool_file_install;
  rclass->makedist = intltool_file_makedist;
  rclass->clean = intltool_file_clean;
  rclass->maintainerclean = intltool_file_maintainerclean;
}

G_DEFINE_TYPE(JBGnomeHelp, jb_gnome_help, JB_TYPE_GROUP_RESOURCE)

JBGnomeHelp *
jb_gnome_help_new (const char *lang, const char *entities)
{
  JBGnomeHelp *self;

  g_return_val_if_fail(lang != NULL, NULL);
  g_return_val_if_fail(jb_feature_is_enabled(&jb_gnome_help_feature), NULL);

  self = g_object_new(JB_TYPE_GNOME_HELP, NULL);
  self->name = g_strdup(jb_variable_get_string("package"));
  self->lang = g_strdup(lang);
  self->entities = g_strsplit(entities != NULL ? entities : "", " ", 0);

  return self;
}

static char *
gnome_help_to_string (JBResource *res)
{
  JBGnomeHelp *self = JB_GNOME_HELP(res);

  return g_strdup_printf("GNOME help %s [%s]", self->name, self->lang);
}

static void
gnome_help_get_files (JBGnomeHelp *self,
		      char **inomf,
		      char **outomf,
		      char **helpdir,
		      char **xmlfile,
		      GSList **entities,
		      GSList **figures)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(self);
  char *_inomf;

  _inomf = g_strdup_printf("%s/%s/%s-%s.omf.in",
			   gres->group->srcdir,
			   self->lang,
			   self->name,
			   self->lang);

  if (inomf != NULL)
    *inomf = g_strdup(_inomf);
  if (outomf != NULL)
    *outomf = get_output_file(_inomf);
  if (helpdir != NULL)
    *helpdir = jb_variable_expand("$help-dir/$name/$lang",
				  "name", self->name,
				  "lang", self->lang,
				  NULL);
  if (xmlfile != NULL)
    *xmlfile = g_strdup_printf("%s/%s/%s.xml",
			       gres->group->srcdir,
			       self->lang,
			       self->name);
  if (entities != NULL)
    {
      int i;
      GSList *list = NULL;

      for (i = 0; self->entities[i] != NULL; i++)
	list = g_slist_append(list, g_strdup_printf("%s/%s/%s",
						    gres->group->srcdir,
						    self->lang,
						    self->entities[i]));

      *entities = list;
    }
  if (figures != NULL)
    {
      char *pattern;

      pattern = g_strdup_printf("%s/%s/figures/*.png",
				gres->group->srcdir,
				self->lang);
      *figures = jb_match_files(pattern);
      g_free(pattern);
    }

  g_free(_inomf);
}

static void
gnome_help_build (JBResource *res)
{
  JBGnomeHelp *self = JB_GNOME_HELP(res);
  char *inomf;
  char *outomf;
  char *helpdir;

  gnome_help_get_files(self, &inomf, &outomf, &helpdir, NULL, NULL, NULL);

  if (! jb_is_uptodate(outomf, inomf))
    {
      jb_resource_message_building(res);

      jb_mkdir_of_file(outomf);

      jb_action_exec("$scrollkeeper-preinstall \"$helpdir/$helpname.xml\" $inomf $outomf",
		     "helpdir", helpdir,
		     "helpname", self->name,
		     "inomf", inomf,
		     "outomf", outomf,
		     NULL);
    }

  g_free(inomf);
  g_free(outomf);
  g_free(helpdir);
}

static void
gnome_help_install (JBResource *res)
{
  JBGnomeHelp *self = JB_GNOME_HELP(res);
  char *outomf;
  char *helpdir;
  char *omfdir;
  char *xmlfile;
  GSList *entities;
  GSList *figures;
  char *figdir;

  gnome_help_get_files(self, NULL, &outomf, &helpdir, &xmlfile, &entities, &figures);

  omfdir = g_strdup_printf("$omf-dir/%s", self->name);

  figdir = g_strdup_printf("%s/figures", helpdir);

  jb_action_install_data(xmlfile, helpdir);

  jb_action_install_data_list(entities, helpdir);

  jb_action_install_data_list(figures, figdir);

  jb_action_install_data(outomf, omfdir);

  jb_action_exec("-$scrollkeeper-update -p \"$destdir$scrollkeeper-dir\" -o \"$destdir$omfdir\"",
		 "omfdir", omfdir,
		 NULL);

  g_free(outomf);
  g_free(helpdir);
  g_free(omfdir);
  g_free(xmlfile);
  jb_g_slist_free_deep(entities);
  jb_g_slist_free_deep(figures);
  g_free(figdir);
}

static void
gnome_help_makedist (JBResource *res)
{
  JBGnomeHelp *self = JB_GNOME_HELP(res);
  char *inomf;
  char *xmlfile;
  GSList *entities;
  GSList *figures;

  gnome_help_get_files(self, &inomf, NULL, NULL, &xmlfile, &entities, &figures);

  jb_action_add_to_dist(inomf);
  jb_action_add_to_dist(xmlfile);
  jb_action_add_to_dist_list(entities);
  jb_action_add_to_dist_list(figures);

  g_free(inomf);
  g_free(xmlfile);
  jb_g_slist_free_deep(entities);
  jb_g_slist_free_deep(figures);
}

static void
gnome_help_clean (JBResource *res)
{
  JBGnomeHelp *self = JB_GNOME_HELP(res);
  char *outomf;

  gnome_help_get_files(self, NULL, &outomf, NULL, NULL, NULL, NULL);

  jb_action_rm(outomf);

  g_free(outomf);
}

static void
jb_gnome_help_init (JBGnomeHelp *self)
{
}

static void
jb_gnome_help_class_init (JBGnomeHelpClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);

  rclass->to_string = gnome_help_to_string;
  rclass->build = gnome_help_build;
  rclass->install = gnome_help_install;
  rclass->makedist = gnome_help_makedist;
  rclass->clean = gnome_help_clean;
}

G_DEFINE_TYPE(JBTranslations, jb_translations, JB_TYPE_GROUP_RESOURCE)

static void
translations_get_files (JBTranslations *self, char **infile, char **outfile)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(self);

  if (infile != NULL)
    *infile = g_strdup_printf("%s/%s.po", gres->group->srcdir, self->lang);
  if (outfile != NULL)
    *outfile = g_strdup_printf("%s/%s.mo", gres->group->builddir, self->lang);
}

JBTranslations *
jb_translations_new (const char *lang)
{
  JBTranslations *self;

  g_return_val_if_fail(lang != NULL, NULL);
  g_return_val_if_fail(jb_feature_is_enabled(&jb_gettext_feature), NULL);

  self = g_object_new(JB_TYPE_TRANSLATIONS, NULL);
  self->lang = g_strdup(lang);

  return self;
}

static char *
translations_to_string (JBResource *res)
{
  JBTranslations *self = JB_TRANSLATIONS(res);

  return g_strdup_printf("%s translations", self->lang);
}

static void
translations_build (JBResource *res)
{
  JBTranslations *self = JB_TRANSLATIONS(res);
  char *infile;
  char *outfile;

  translations_get_files(self, &infile, &outfile);

  if (! jb_is_uptodate(outfile, infile))
    {
      jb_resource_message_building(res);

      jb_mkdir_of_file(outfile);

      jb_action_exec("$msgfmt -o $outfile.tmp $infile && mv -f $outfile.tmp $outfile",
		     "outfile", outfile,
		     "infile", infile,
		     NULL);
    }

  g_free(infile);
  g_free(outfile);
}

static void
translations_install (JBResource *res)
{
  JBTranslations *self = JB_TRANSLATIONS(res);
  char *outfile;
  char *installfile;

  translations_get_files(self, NULL, &outfile);

  installfile = g_strdup_printf("$prefix/share/locale/%s/LC_MESSAGES/$package.mo", self->lang);

  jb_action_install_data_to_file(outfile, installfile);

  g_free(outfile);
  g_free(installfile);
}

static void
translations_makedist (JBResource *res)
{
  JBTranslations *self = JB_TRANSLATIONS(res);
  char *infile;

  translations_get_files(self, &infile, NULL);

  jb_action_add_to_dist(infile);

  g_free(infile);
}

static void
translations_clean (JBResource *res)
{
  JBTranslations *self = JB_TRANSLATIONS(res);
  char *outfile;

  translations_get_files(self, NULL, &outfile);

  jb_action_rm(outfile);

  g_free(outfile);
}

static void
jb_translations_init (JBTranslations *self)
{
}

static void
jb_translations_class_init (JBTranslationsClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);

  rclass->to_string = translations_to_string;
  rclass->build = translations_build;
  rclass->install = translations_install;
  rclass->makedist = translations_makedist;
  rclass->clean = translations_clean;
}

G_DEFINE_TYPE(JBRule, jb_rule, JB_TYPE_GROUP_RESOURCE)

JBRule *
jb_rule_new (void)
{
  return g_object_new(JB_TYPE_RULE, NULL);
}

/*
 * This could be inferred by mapping input files to the resources that
 * produce them, but it would require a more complex JBResource
 * design. For now this is good enough.
 */
void
jb_rule_add_dependency (JBRule *self, JBGroupResource *res)
{
  g_return_if_fail(JB_IS_RULE(self));
  g_return_if_fail(JB_IS_GROUP_RESOURCE(res));

  self->dependencies = g_slist_append(self->dependencies, g_object_ref(res));
}

void
jb_rule_add_input_file (JBRule *self, const char *format, ...)
{
  char *file;

  g_return_if_fail(JB_IS_RULE(self));
  g_return_if_fail(format != NULL);

  JB_STRDUP_VPRINTF(file, format);
  self->input_files = g_slist_append(self->input_files, file);
}

void
jb_rule_add_output_file (JBRule *self, const char *format, ...)
{
  char *file;

  g_return_if_fail(JB_IS_RULE(self));
  g_return_if_fail(format != NULL);

  JB_STRDUP_VPRINTF(file, format);
  self->output_files = g_slist_append(self->output_files, file);
}

void
jb_rule_set_build_message (JBRule *self, const char *format, ...)
{
  g_return_if_fail(JB_IS_RULE(self));
  g_return_if_fail(format != NULL);

  g_free(self->build_message);
  JB_STRDUP_VPRINTF(self->build_message, format);
}

void
jb_rule_add_build_command (JBRule *self, const char *format, ...)
{
  char *command;

  g_return_if_fail(JB_IS_RULE(self));
  g_return_if_fail(format != NULL);

  JB_STRDUP_VPRINTF(command, format);
  self->build_commands = g_slist_append(self->build_commands, command);
}

void
jb_rule_set_install_message (JBRule *self, const char *format, ...)
{
  g_return_if_fail(JB_IS_RULE(self));
  g_return_if_fail(format != NULL);

  g_free(self->install_message);
  JB_STRDUP_VPRINTF(self->install_message, format);
}

void
jb_rule_add_install_command (JBRule *self, const char *format, ...)
{
  char *command;

  g_return_if_fail(JB_IS_RULE(self));
  g_return_if_fail(format != NULL);

  JB_STRDUP_VPRINTF(command, format);
  self->install_commands = g_slist_append(self->install_commands, command);
}

static GSList *
rule_expand_variables (JBRule *self, GSList *strings)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(self);
  GSList *result = NULL;
  GSList *l;

  JB_LIST_FOREACH(l, strings)
    {
      const char *str = l->data;
      char *expanded;

      expanded = jb_variable_expand(str,
				    "srcdir", gres->group->srcdir,
				    "builddir", gres->group->builddir,
				    NULL);

      result = g_slist_append(result, expanded);
    }

  return result;
}

static void
rule_perform (JBRule *self, const char *message, GSList *commands)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(self);
  GSList *l;

  if (commands == NULL)
    return;

  jb_message("%s", message);

  JB_LIST_FOREACH(l, commands)
    {
      const char *command = l->data;

      jb_action_exec(command,
		     "srcdir", gres->group->srcdir,
		     "builddir", gres->group->builddir,
		     NULL);
    }
}

static void
rule_build (JBResource *res)
{
  JBRule *self = JB_RULE(res);
  GSList *l;

  JB_LIST_FOREACH(l, self->dependencies)
    {
      JBResource *dep = l->data;
      jb_resource_build(dep);
    }

  if (self->output_files != NULL)
    {
      GSList *input_files;
      GSList *output_files;
      gboolean is_uptodate;

      input_files = rule_expand_variables(self, self->input_files);
      output_files = rule_expand_variables(self, self->output_files);

      is_uptodate = jb_is_uptodate_list_list(output_files, input_files);

      jb_g_slist_free_deep(input_files);
      jb_g_slist_free_deep(output_files);

      if (is_uptodate)
	return;
    }

  rule_perform(self, self->build_message, self->build_commands);
}

static void
rule_install (JBResource *res)
{
  JBRule *self = JB_RULE(res);

  rule_perform(self, self->install_message, self->install_commands);
}

static void
rule_clean (JBResource *res)
{
  JBRule *self = JB_RULE(res);
  GSList *output_files;

  output_files = rule_expand_variables(self, self->output_files);

  jb_action_rm_list(output_files);

  jb_g_slist_free_deep(output_files);
}

static void
jb_rule_init (JBRule *self)
{
}

static void
jb_rule_class_init (JBRuleClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);

  rclass->build = rule_build;
  rclass->install = rule_install;
  rclass->clean = rule_clean;
}

G_DEFINE_ABSTRACT_TYPE(JBObject, jb_object, JB_TYPE_GROUP_RESOURCE)

static void
object_construct (JBObject *self, const char *name)
{
  self->name = g_strdup(name);

  self->compile_options = jb_compile_options_new(name);
  self->install_options = jb_install_options_new();
}

static char *
object_to_string (JBResource *res)
{
  JBObject *self = JB_OBJECT(res);
  JBObjectClass *class = JB_OBJECT_GET_CLASS(self);

  return g_strdup_printf("%s %s", class->type, self->name);
}

static char *
object_get_output_file (JBObject *self)
{
  JBObjectClass *class;

  g_return_val_if_fail(JB_IS_OBJECT(self), NULL);

  class = JB_OBJECT_GET_CLASS(self);

  if (class->get_output_file != NULL)
    return class->get_output_file(self);
  else
    {
      g_error("get_output_file() not implemented for object type %s", G_OBJECT_TYPE_NAME(self));
      return NULL;
    }
}

static GSList *
object_get_object_files (JBObject *self)
{
  GSList *list = NULL;
  GSList *l;

  JB_LIST_FOREACH(l, self->resources)
    list = g_slist_append(list, object_resource_get_object_file(l->data));

  return list;
}

static GSList *
object_get_headers (JBObject *self)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(self);
  GSList *list = NULL;
  GSList *l;

  JB_LIST_FOREACH(l, self->headers)
    {
      const char *header = l->data;

      list = g_slist_append(list, g_strdup_printf("%s/%s",
						  gres->group->srcdir,
						  header));
    }

  return list;
}

static void
object_pre_build (JBResource *res)
{
  JBObject *self = JB_OBJECT(res);
  GSList *l;

  JB_LIST_FOREACH(l, self->resources)
    jb_resource_pre_build(l->data);
}

static void
object_build (JBResource *res)
{
  JBObject *self = JB_OBJECT(res);
  char *outfile;
  GSList *object_files_list;
  GSList *l;

  JB_LIST_FOREACH(l, self->resources)
    jb_resource_build(l->data);

  outfile = object_get_output_file(self);
  object_files_list = object_get_object_files(self);

  if (! jb_is_uptodate_list(outfile, object_files_list))
    {
      JBGroupResource *gres = JB_GROUP_RESOURCE(self);
      JBObjectClass *object_class;
      char *object_files;

      jb_resource_message_building(res);

      object_class = JB_OBJECT_GET_CLASS(self);

      object_files = jb_string_list_join(object_files_list, " ");

      jb_mkdir(gres->group->builddir);

      jb_action_exec("$cc $package-cflags $group-cflags $object-cflags $cflags"
		     " $package-ldflags $group-ldflags $object-ldflags $stock-ldflags $ldflags"
		     " -o $outfile $object-files",
		     "package-cflags", jb_compile_options_get_cflags(jb_compile_options),
		     "group-cflags", jb_compile_options_get_cflags(gres->group->compile_options),
		     "object-cflags", jb_compile_options_get_cflags(self->compile_options),
		     "package-ldflags", jb_compile_options_get_ldflags(jb_compile_options),
		     "group-ldflags", jb_compile_options_get_ldflags(gres->group->compile_options),
		     "object-ldflags", jb_compile_options_get_ldflags(self->compile_options),
		     "stock-ldflags", object_class->stock_ldflags,
		     "outfile", outfile,
		     "object-files", object_files,
		     NULL);

      g_free(object_files);
    }

  g_free(outfile);
  jb_g_slist_free_deep(object_files_list);
}

static void
object_makedist (JBResource *res)
{
  JBObject *self = JB_OBJECT(res);
  GSList *l;
  GSList *headers;

  JB_LIST_FOREACH(l, self->resources)
    jb_resource_makedist(l->data);

  headers = object_get_headers(self);
  jb_action_add_to_dist_list(headers);
  jb_g_slist_free_deep(headers);
}

static void
object_clean (JBResource *res)
{
  JBObject *self = JB_OBJECT(res);
  GSList *l;
  char *outfile;

  JB_LIST_FOREACH(l, self->resources)
    jb_resource_clean(l->data);

  outfile = object_get_output_file(self);

  jb_action_rm(outfile);

  g_free(outfile);
}

static void
object_distclean (JBResource *res)
{
  JBObject *self = JB_OBJECT(res);
  GSList *l;

  JB_LIST_FOREACH(l, self->resources)
    jb_resource_distclean(l->data);
}

static void
object_maintainerclean (JBResource *res)
{
  JBObject *self = JB_OBJECT(res);
  GSList *l;

  JB_LIST_FOREACH(l, self->resources)
    jb_resource_maintainerclean(l->data);
}

static void
object_add_resource (JBObject *self, JBObjectResource *res)
{
  g_return_if_fail(JB_IS_OBJECT(self));
  g_return_if_fail(JB_IS_OBJECT_RESOURCE(res));
  g_return_if_fail(res->object == NULL);

  res->object = self;

  self->resources = g_slist_append(self->resources, res);
}

static void
object_add_header (JBObject *self, const char *filename)
{
  g_return_if_fail(JB_IS_OBJECT(self));
  g_return_if_fail(filename != NULL);

  self->headers = g_slist_append(self->headers, g_strdup(filename));
}

void
jb_object_add_source (JBObject *self, const char *name)
{
  g_return_if_fail(JB_IS_OBJECT(self));
  g_return_if_fail(name != NULL);

  if (strchr(name, ':') != NULL)
    object_add_resource(self, JB_OBJECT_RESOURCE(jb_gob_class_new(name)));
  else if (strchr(name, '.') == NULL)
    {
      char *cfile;
      char *hfile;

      cfile = g_strdup_printf("%s.c", name);
      hfile = g_strdup_printf("%s.h", name);

      object_add_resource(self, JB_OBJECT_RESOURCE(jb_source_new(cfile)));
      object_add_header(self, hfile);

      g_free(cfile);
      g_free(hfile);
    }
  else
    {
      if (g_str_has_suffix(name, ".c"))
	object_add_resource(self, JB_OBJECT_RESOURCE(jb_source_new(name)));
      else if (g_str_has_suffix(name, ".h"))
	object_add_header(self, name);
      else
	g_error("the extension of source file \"%s\" is not .c or .h", name);
    }
}

void
jb_object_add_sources (JBObject *self, const char *name, ...)
{
  va_list args;

  g_return_if_fail(JB_IS_OBJECT(self));

  va_start(args, name);

  while (name != NULL)
    {
      jb_object_add_source(self, name);
      name = va_arg(args, const char *);
    }

  va_end(args);
}

static void
jb_object_init (JBObject *self)
{
}

static void
jb_object_class_init (JBObjectClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);

  rclass->to_string = object_to_string;
  rclass->pre_build = object_pre_build;
  rclass->build = object_build;
  rclass->makedist = object_makedist;
  rclass->clean = object_clean;
  rclass->distclean = object_distclean;
  rclass->maintainerclean = object_maintainerclean;
}

G_DEFINE_TYPE(JBProgram, jb_program, JB_TYPE_OBJECT)

JBProgram *
jb_program_new (const char *name)
{
  JBProgram *self;
  JBObject *object;

  g_return_val_if_fail(name != NULL, NULL);

  self = g_object_new(JB_TYPE_PROGRAM, NULL);
  object = JB_OBJECT(self);

  object_construct(object, name);
  jb_install_options_set_installdir(object->install_options, "$bindir");

  return self;
}

static void
program_install (JBResource *res)
{
  JBObject *object = JB_OBJECT(res);
  char *outfile;

  outfile = object_get_output_file(object);

  jb_install_options_install_program(object->install_options, outfile, NULL);

  g_free(outfile);
}

static char *
program_get_output_file (JBObject *object)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(object);

  return g_strdup_printf("%s/%s", gres->group->builddir, object->name);
}

static void
jb_program_init (JBProgram *self)
{
}

static void
jb_program_class_init (JBProgramClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);
  JBObjectClass *oclass = JB_OBJECT_CLASS(class);

  rclass->install = program_install;

  oclass->type = "program";
  oclass->get_output_file = program_get_output_file;
}

G_DEFINE_TYPE(JBModule, jb_module, JB_TYPE_OBJECT)

JBModule *
jb_module_new (const char *name)
{
  JBModule *self;
  JBObject *object;

  g_return_val_if_fail(name != NULL, NULL);

  self = g_object_new(JB_TYPE_MODULE, NULL);
  object = JB_OBJECT(self);

  object_construct(object, name);
  jb_install_options_set_installdir(object->install_options, "$pkglibdir");

  return self;
}

static void
module_install (JBResource *res)
{
  JBObject *object = JB_OBJECT(res);
  char *outfile;

  outfile = object_get_output_file(object);

  jb_install_options_install_library(object->install_options, outfile, NULL);

  g_free(outfile);
}

static char *
module_get_output_file (JBObject *object)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(object);

  return g_strdup_printf("%s/%s.so", gres->group->builddir, object->name);
}

static void
jb_module_init (JBModule *self)
{
}

static void
jb_module_class_init (JBModuleClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);
  JBObjectClass *oclass = JB_OBJECT_CLASS(class);

  rclass->install = module_install;

  oclass->type = "module";
  oclass->stock_cflags = "-fPIC";
  oclass->stock_cppflags = "-DPIC";
  oclass->stock_ldflags = "-shared -fPIC";
  oclass->get_output_file = module_get_output_file;
}
