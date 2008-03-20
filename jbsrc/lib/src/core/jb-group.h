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

#ifndef _JB_GROUP_H
#define _JB_GROUP_H

#include <stdarg.h>
#include <glib-object.h>
#include "jb-decls.h"
#include "jb-util.h"
#include "jb-compile-options.h"

#define JB_TYPE_GROUP		(jb_group_get_type())
#define JB_GROUP(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_GROUP, JBGroup))
#define JB_IS_GROUP(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_GROUP))

struct JBGroup
{
  GObject		parent;

  char			*name;

  char			*srcdir;
  char			*builddir;

  JBCompileOptions	*compile_options;

  GSList		*resources;
};

typedef struct
{
  GObjectClass		parent_class;
} JBGroupClass;

extern GSList *jb_groups;

extern JBStringHashSet *jb_templates;

GType jb_group_get_type (void);

JBGroup *jb_group_new (const char *name);

void jb_group_add (JBGroup *group);

JBGroup *jb_group_get (const char *name);

void jb_group_add_resource (JBGroup *self, JBGroupResource *res);

void jb_group_add_data_file (JBGroup *self,
			     const char *file,
			     const char *installdir);
void jb_group_add_data_files (JBGroup *self, const char *file, ...) G_GNUC_NULL_TERMINATED;
void jb_group_add_data_files_list (JBGroup *self,
				   GSList *files,
				   const char *installdir);
void jb_group_add_data_files_pattern (JBGroup *self,
				      const char *pattern,
				      const char *installdir);

void jb_group_add_dist_file (JBGroup *self, const char *file);
void jb_group_add_dist_files (JBGroup *self, const char *file, ...) G_GNUC_NULL_TERMINATED;

void jb_group_add_desktop_file (JBGroup *self,
				const char *filename,
				const char *installdir);

void jb_group_add_gconf_schemas (JBGroup *self, const char *filename);

void jb_group_add_bonobo_server (JBGroup *self, const char *filename);

void jb_group_add_translations (JBGroup *self, const char *languages);

#endif /* _JB_GROUP_H */
