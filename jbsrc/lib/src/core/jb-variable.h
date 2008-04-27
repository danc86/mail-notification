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

#ifndef _JB_VARIABLE_H
#define _JB_VARIABLE_H

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <glib-object.h>

typedef enum
{
  /* allow the user to set it on the command line */
  JB_VARIABLE_USER_SETTABLE	= 1 << 0,

  /* add -DWITH_name=0|1 (bool variables only) */
  JB_VARIABLE_C_DEFINE		= 1 << 1,

  /* omit it from the configure report */
  JB_VARIABLE_NO_REPORT		= 1 << 2
} JBVariableFlags;

typedef struct
{
  char		*name;
} JBVariableGroup;

typedef struct
{
  char		*name;

  GType		gtype;

  gboolean	(*from_string)	(const char *str, GValue *value, GError **err);
  char *	(*to_string)	(const GValue *value);
} JBVariableType;

typedef struct
{
  JBVariableType	*type;
  char			*name;

  /* can be NULL if the group is NULL */
  char			*description;

  /* if NULL, the variable is omitted from the help and report */
  JBVariableGroup	*group;

  JBVariableFlags	flags;

  GValue		value;

  int			user_set : 1;
} JBVariable;

extern GSList *jb_variables;

extern GSList *jb_variable_groups;

extern JBVariableGroup *jb_variable_group_compiler_options;
extern JBVariableGroup *jb_variable_group_installation_options;
extern JBVariableGroup *jb_variable_group_external_programs;

extern JBVariableType *jb_variable_type_bool;
extern JBVariableType *jb_variable_type_string;
extern JBVariableType *jb_variable_type_mode;

void jb_variable_init (void);

JBVariableGroup *jb_variable_add_group (const char *name);

JBVariableType *jb_variable_get_type (const char *name);

JBVariable *jb_variable_get_variable (const char *name);
JBVariable *jb_variable_get_variable_or_error (const char *name);

JBVariable *jb_variable_add (JBVariableType *type,
			     const char *name,
			     const char *description,
			     JBVariableGroup *group,
			     JBVariableFlags flags);

void jb_variable_add_bool (const char *name,
			   const char *description,
			   JBVariableGroup *group,
			   JBVariableFlags flags,
			   gboolean default_value);
void jb_variable_add_string (const char *name,
			     const char *description,
			     JBVariableGroup *group,
			     JBVariableFlags flags,
			     const char *default_value);
void jb_variable_add_mode (const char *name,
			   const char *description,
			   JBVariableGroup *group,
			   JBVariableFlags flags,
			   mode_t default_value);

gboolean jb_variable_get_bool (const char *name);
void jb_variable_set_bool (const char *name, gboolean value);

const char *jb_variable_get_string (const char *name);
const char *jb_variable_get_string_or_null (const char *name);
void jb_variable_set_string (const char *name, const char *value);

mode_t jb_variable_get_mode (const char *name);
void jb_variable_set_mode (const char *name, mode_t value);

void jb_variable_set_package_flags (const char *name,
				    const char *cflags,
				    const char *cppflags,
				    const char *ldflags,
				    const char *libs);

gboolean jb_variable_set_from_string (JBVariable *self,
				      const char *value,
				      GError **err);

char *jb_variable_to_string (JBVariable *self);

char *jb_variable_evaluate (JBVariable *variable);
char *jb_variable_expand (const char *str, ...) G_GNUC_NULL_TERMINATED;
char *jb_variable_expandv (const char *str, va_list args);

const char *jb_variable_get_type_name (JBVariable *self);

#endif /* _JB_VARIABLE_H */
