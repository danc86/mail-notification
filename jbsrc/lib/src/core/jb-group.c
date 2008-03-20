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
#include "jb-group.h"
#include "jb-feature.h"
#include "jb-variable.h"
#include "jb-util.h"
#include "jb-resource.h"
#include "jb-main.h"

GSList *jb_groups = NULL;

JBStringHashSet *jb_templates = NULL;

G_DEFINE_TYPE(JBGroup, jb_group, G_TYPE_OBJECT)

static void
jb_group_init (JBGroup *self)
{
}

static void
jb_group_class_init (JBGroupClass *class)
{
}

JBGroup *
jb_group_new (const char *name)
{
  JBGroup *self;
  char *cppflags;
  char *gob2flags;

  g_return_val_if_fail(name != NULL, NULL);

  self = g_object_new(JB_TYPE_GROUP, NULL);

  self->name = g_strdup(name);

  self->srcdir = g_strdup(name);
  self->builddir = g_strdup_printf("build/%s", self->srcdir);

  self->compile_options = jb_compile_options_new(name);

  /* srcdir for the C headers, builddir for the generated C headers */
  cppflags = g_strdup_printf("-I%s -I%s", self->srcdir, self->builddir);
  jb_compile_options_add_cppflags(self->compile_options, cppflags);
  g_free(cppflags);

  gob2flags = g_strdup_printf("--output-dir=%s", self->builddir);
  jb_compile_options_add_gob2flags(self->compile_options, gob2flags);
  g_free(gob2flags);

  return self;
}

void
jb_group_add (JBGroup *group)
{
  GSList *l;

  g_return_if_fail(JB_IS_GROUP(group));

  JB_LIST_FOREACH(l, group->resources)
    {
      JBResource *res = l->data;

      if (JB_IS_TEMPLATE(res))
	{
	  JBTemplate *template = JB_TEMPLATE(res);
	  char *filename;

	  filename = g_strdup_printf("%s/%s", group->srcdir, template->filename);

	  if (jb_templates == NULL)
	    jb_templates = jb_string_hash_set_new();

	  if (! jb_string_hash_set_add(jb_templates, filename))
	    g_error("template file \"%s\" specified multiple times", filename);

	  g_free(filename);
	}
    }

  jb_groups = g_slist_append(jb_groups, group);
}

JBGroup *
jb_group_get (const char *name)
{
  GSList *l;

  g_return_val_if_fail(name != NULL, NULL);

  JB_LIST_FOREACH(l, jb_groups)
    {
      JBGroup *group = l->data;

      if (! strcmp(group->name, name))
	return group;
    }

  return NULL;
}

void
jb_group_add_resource (JBGroup *self, JBGroupResource *res)
{
  g_return_if_fail(JB_IS_GROUP(self));
  g_return_if_fail(JB_IS_GROUP_RESOURCE(res));
  g_return_if_fail(res->group == NULL);

  res->group = self;

  self->resources = g_slist_append(self->resources, res);
}

void
jb_group_add_data_file (JBGroup *self,
			const char *file,
			const char *installdir)
{
  JBDataFile *data_file;

  g_return_if_fail(JB_IS_GROUP(self));
  g_return_if_fail(file != NULL);

  data_file = jb_data_file_new(file);

  jb_install_options_set_installdir(data_file->install_options, installdir);

  jb_group_add_resource(self, JB_GROUP_RESOURCE(data_file));
}

void
jb_group_add_data_files (JBGroup *self, const char *file, ...)
{
  va_list args;

  g_return_if_fail(JB_IS_GROUP(self));

  va_start(args, file);

  while (file != NULL)
    {
      const char *installdir;

      installdir = va_arg(args, const char *);
      g_assert(installdir != NULL);

      jb_group_add_data_file(self, file, installdir);

      file = va_arg(args, const char *);
    }

  va_end(args);
}

void
jb_group_add_data_files_list (JBGroup *self,
			      GSList *files,
			      const char *installdir)
{
  GSList *l;

  g_return_if_fail(JB_IS_GROUP(self));

  JB_LIST_FOREACH(l, files)
    {
      const char *file = l->data;

      jb_group_add_data_file(self, file, installdir);
    }
}

void
jb_group_add_data_files_pattern (JBGroup *self,
				 const char *pattern,
				 const char *installdir)
{
  GSList *files;

  g_return_if_fail(JB_IS_GROUP(self));
  g_return_if_fail(pattern != NULL);

  /*
   * We must chdir to srcdir for matching, because the file list must
   * be relative to srcdir.
   */
  jb_chdir(self->srcdir);
  files = jb_match_files(pattern);
  jb_chdir(jb_topsrcdir);

  jb_group_add_data_files_list(self, files, installdir);
  jb_g_slist_free_deep(files);
}

void
jb_group_add_dist_file (JBGroup *self, const char *file)
{
  g_return_if_fail(JB_IS_GROUP(self));
  g_return_if_fail(file != NULL);

  jb_group_add_data_file(self, file, NULL);
}

void
jb_group_add_dist_files (JBGroup *self, const char *file, ...)
{
  va_list args;

  g_return_if_fail(JB_IS_GROUP(self));

  va_start(args, file);

  while (file != NULL)
    {
      jb_group_add_dist_file(self, file);

      file = va_arg(args, const char *);
    }

  va_end(args);
}

static void
add_intltool_file (JBGroup *self,
		   const char *type,
		   const char *filename,
		   const char *merge_flags,
		   const char *installdir)
{
  JBIntltoolFile *file;

  file = jb_intltool_file_new(type, filename, merge_flags);

  jb_install_options_set_installdir(file->install_options, installdir);

  jb_group_add_resource(self, JB_GROUP_RESOURCE(file));
}

void
jb_group_add_desktop_file (JBGroup *self,
			   const char *filename,
			   const char *installdir)
{
  g_return_if_fail(JB_IS_GROUP(self));
  g_return_if_fail(filename != NULL);
  g_return_if_fail(installdir != NULL);

  add_intltool_file(self,
		    "desktop file",
		    filename,
		    "-d",
		    installdir);
}

void
jb_group_add_gconf_schemas (JBGroup *self, const char *filename)
{
  JBGConfSchemas *schemas;

  g_return_if_fail(JB_IS_GROUP(self));
  g_return_if_fail(filename != NULL);
  g_return_if_fail(jb_feature_is_enabled(&jb_gconf_feature));
  g_return_if_fail(jb_feature_is_enabled(&jb_intltool_feature));
  g_return_if_fail(jb_intltool_use_xml);

  schemas = jb_gconf_schemas_new(filename);

  jb_group_add_resource(self, JB_GROUP_RESOURCE(schemas));
}

void
jb_group_add_bonobo_server (JBGroup *self, const char *filename)
{
  g_return_if_fail(JB_IS_GROUP(self));
  g_return_if_fail(filename != NULL);
  g_return_if_fail(jb_feature_is_enabled(&jb_intltool_feature));
  g_return_if_fail(jb_intltool_use_xml);

  add_intltool_file(self,
		    "Bonobo server",
		    filename,
		    "-b",
		    "$libdir/bonobo/servers");
}

void
jb_group_add_translations (JBGroup *self, const char *languages)
{
  char **array;
  int i;

  g_return_if_fail(self != NULL);
  g_return_if_fail(jb_feature_is_enabled(&jb_gettext_feature));

  array = g_strsplit(languages, " ", 0);
  for (i = 0; array[i] != NULL; i++)
    jb_group_add_resource(self, JB_GROUP_RESOURCE(jb_translations_new(array[i])));
  g_strfreev(array);
}
