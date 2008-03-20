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

#ifndef _JB_RESOURCE_H
#define _JB_RESOURCE_H

#include <stdarg.h>
#include <glib-object.h>
#include "jb-decls.h"
#include "jb-compile-options.h"
#include "jb-install-options.h"

#define JB_TYPE_RESOURCE		(jb_resource_get_type())
#define JB_RESOURCE(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_RESOURCE, JBResource))
#define JB_RESOURCE_CLASS(class)	(G_TYPE_CHECK_CLASS_CAST((class), JB_TYPE_RESOURCE, JBResourceClass))
#define JB_IS_RESOURCE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_RESOURCE))
#define JB_RESOURCE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), JB_TYPE_RESOURCE, JBResourceClass))

typedef struct
{
  GObject	parent;
} JBResource;

typedef struct
{
  GObjectClass	parent;

  char *	(*to_string)			(JBResource *self);

  void		(*pre_build)			(JBResource *self);
  void		(*build)			(JBResource *self);
  void		(*install)			(JBResource *self);
  void		(*makedist)			(JBResource *self);
  void		(*clean)			(JBResource *self);
  void		(*distclean)			(JBResource *self);
  void		(*maintainerclean)		(JBResource *self);
} JBResourceClass;

GType jb_resource_get_type (void);

char *jb_resource_to_string (JBResource *self);

void jb_resource_pre_build (JBResource *self);
void jb_resource_build (JBResource *self);
void jb_resource_install (JBResource *self);
void jb_resource_makedist (JBResource *self);
void jb_resource_clean (JBResource *self);
void jb_resource_distclean (JBResource *self);
void jb_resource_maintainerclean (JBResource *self);

void jb_resource_message_building (JBResource *self);

#define JB_TYPE_GROUP_RESOURCE		(jb_group_resource_get_type())
#define JB_GROUP_RESOURCE(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_GROUP_RESOURCE, JBGroupResource))
#define JB_IS_GROUP_RESOURCE(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_GROUP_RESOURCE))

struct JBGroupResource
{
  JBResource	parent;

  JBGroup	*group;
};

typedef struct
{
  JBResourceClass	parent;
} JBGroupResourceClass;

GType jb_group_resource_get_type (void);

#define JB_TYPE_OBJECT_RESOURCE			(jb_object_resource_get_type())
#define JB_OBJECT_RESOURCE(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_OBJECT_RESOURCE, JBObjectResource))
#define JB_OBJECT_RESOURCE_CLASS(class)		(G_TYPE_CHECK_CLASS_CAST((class), JB_TYPE_OBJECT_RESOURCE, JBObjectResourceClass))
#define JB_IS_OBJECT_RESOURCE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_OBJECT_RESOURCE))
#define JB_OBJECT_RESOURCE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), JB_TYPE_OBJECT_RESOURCE, JBObjectResourceClass))

typedef struct
{
  JBResource	parent;

  JBObject	*object;
} JBObjectResource;

typedef struct
{
  JBResourceClass	parent;

  char *		(*get_object_file) (JBObjectResource *self);
} JBObjectResourceClass;

GType jb_object_resource_get_type (void);

#define JB_TYPE_TEMPLATE		(jb_template_get_type())
#define JB_TEMPLATE(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_TEMPLATE, JBTemplate))
#define JB_IS_TEMPLATE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_TEMPLATE))

typedef struct
{
  JBGroupResource	parent;

  char			*filename;
} JBTemplate;

typedef struct
{
  JBGroupResourceClass	parent;
} JBTemplateClass;

GType jb_template_get_type (void);

JBTemplate *jb_template_new (const char *filename);

#define JB_TYPE_DATA_FILE		(jb_data_file_get_type())
#define JB_DATA_FILE(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_DATA_FILE, JBDataFile))
#define JB_IS_DATA_FILE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_DATA_FILE))

typedef struct
{
  JBGroupResource	parent;

  char			*filename;

  JBInstallOptions	*install_options;
} JBDataFile;

typedef struct
{
  JBGroupResourceClass	parent;
} JBDataFileClass;

GType jb_data_file_get_type (void);

JBDataFile *jb_data_file_new (const char *filename);

#define JB_TYPE_SOURCE			(jb_source_get_type())
#define JB_SOURCE(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_SOURCE, JBSource))
#define JB_IS_SOURCE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_SOURCE))

typedef struct
{
  JBObjectResource	parent;

  char			*filename;
} JBSource;

typedef struct
{
  JBObjectResourceClass	parent;
} JBSourceClass;

GType jb_source_get_type (void);

JBSource *jb_source_new (const char *filename);

#define JB_TYPE_GOB_CLASS		(jb_gob_class_get_type())
#define JB_GOB_CLASS(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_GOB_CLASS, JBGobClass))
#define JB_IS_GOB_CLASS(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_GOB_CLASS))

typedef struct
{
  JBObjectResource	parent;

  char			*name;	/* Foo:Bar */
} JBGobClass;

typedef struct
{
  JBObjectResourceClass	parent;
} JBGobClassClass;

GType jb_gob_class_get_type (void);

JBGobClass *jb_gob_class_new (const char *name);

#define JB_TYPE_INTLTOOL_FILE		(jb_intltool_file_get_type())
#define JB_INTLTOOL_FILE(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_INTLTOOL_FILE, JBIntltoolFile))
#define JB_IS_INTLTOOL_FILE(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_INTLTOOL_FILE))

typedef struct
{
  JBGroupResource	parent;

  char			*type;
  char			*filename;
  char			*merge_flags;

  JBInstallOptions	*install_options;
} JBIntltoolFile;

typedef struct
{
  JBGroupResourceClass	parent;
} JBIntltoolFileClass;

GType jb_intltool_file_get_type (void);

JBIntltoolFile *jb_intltool_file_new (const char *type,
				      const char *filename,
				      const char *merge_flags);

#define JB_TYPE_GNOME_HELP		(jb_gnome_help_get_type())
#define JB_GNOME_HELP(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_GNOME_HELP, JBGnomeHelp))
#define JB_IS_GNOME_HELP(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_GNOME_HELP))

typedef struct
{
  JBGroupResource	parent;

  char			*name;
  char			*lang;
  char			**entities;
} JBGnomeHelp;

typedef struct
{
  JBGroupResourceClass	parent;
} JBGnomeHelpClass;

GType jb_gnome_help_get_type (void);

JBGnomeHelp *jb_gnome_help_new (const char *lang, const char *entities);

#define JB_TYPE_TRANSLATIONS		(jb_translations_get_type())
#define JB_TRANSLATIONS(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_TRANSLATIONS, JBTranslations))
#define JB_IS_TRANSLATIONS(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_TRANSLATIONS))

typedef struct
{
  JBGroupResource	parent;

  char			*lang;
} JBTranslations;

typedef struct
{
  JBGroupResourceClass	parent;
} JBTranslationsClass;

GType jb_translations_get_type (void);

JBTranslations *jb_translations_new (const char *lang);

#define JB_TYPE_RULE			(jb_rule_get_type())
#define JB_RULE(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_RULE, JBRule))
#define JB_IS_RULE(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_RULE))

typedef struct
{
  JBGroupResource	parent;

  GSList		*dependencies;
  GSList		*output_files;
  GSList		*input_files;

  char			*build_message;
  GSList		*build_commands;

  char			*install_message;
  GSList		*install_commands;
} JBRule;

typedef struct
{
  JBGroupResourceClass	parent;
} JBRuleClass;

GType jb_rule_get_type (void);

JBRule *jb_rule_new (void);

void jb_rule_add_dependency (JBRule *self, JBGroupResource *res);

void jb_rule_add_input_file (JBRule *self, const char *format, ...) G_GNUC_PRINTF(2, 3);
void jb_rule_add_output_file (JBRule *self, const char *format, ...) G_GNUC_PRINTF(2, 3);

void jb_rule_set_build_message (JBRule *self, const char *format, ...) G_GNUC_PRINTF(2, 3);
void jb_rule_add_build_command (JBRule *self, const char *format, ...) G_GNUC_PRINTF(2, 3);

void jb_rule_set_install_message (JBRule *self, const char *format, ...) G_GNUC_PRINTF(2, 3);
void jb_rule_add_install_command (JBRule *self, const char *format, ...) G_GNUC_PRINTF(2, 3);

#define JB_TYPE_OBJECT			(jb_object_get_type())
#define JB_OBJECT(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_OBJECT, JBObject))
#define JB_OBJECT_CLASS(class)		(G_TYPE_CHECK_CLASS_CAST((class), JB_TYPE_OBJECT, JBObjectClass))
#define JB_IS_OBJECT(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_OBJECT))
#define JB_OBJECT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), JB_TYPE_OBJECT, JBObjectClass))

struct JBObject
{
  JBGroupResource	parent;

  char			*name;

  JBCompileOptions	*compile_options;
  JBInstallOptions	*install_options;

  GSList		*resources;
  GSList		*headers;
};

typedef struct
{
  JBGroupResourceClass	parent;

  const char		*type;

  /* these fields can be NULL */
  const char		*stock_cflags;
  const char		*stock_cppflags;
  const char		*stock_ldflags;

  char *		(*get_output_file) (JBObject *self);
} JBObjectClass;

GType jb_object_get_type (void);

void jb_object_add_source (JBObject *self, const char *name);
void jb_object_add_sources (JBObject *self, const char *name, ...) G_GNUC_NULL_TERMINATED;

#define JB_TYPE_PROGRAM			(jb_program_get_type())
#define JB_PROGRAM(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_PROGRAM, JBProgram))
#define JB_IS_PROGRAM(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_PROGRAM))

typedef struct
{
  JBObject	parent;
} JBProgram;

typedef struct
{
  JBObjectClass	parent;
} JBProgramClass;

GType jb_program_get_type (void);

JBProgram *jb_program_new (const char *name);

#define JB_TYPE_MODULE			(jb_module_get_type())
#define JB_MODULE(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_MODULE, JBModule))
#define JB_IS_MODULE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_MODULE))

typedef struct
{
  JBObject	parent;
} JBModule;

typedef struct
{
  JBObjectClass	parent;
} JBModuleClass;

GType jb_module_get_type (void);

JBModule *jb_module_new (const char *name);

#endif /* _JB_RESOURCE_H */
