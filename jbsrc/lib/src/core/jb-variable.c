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
#include "jb-variable.h"
#include "jb-util.h"

static GHashTable *variable_types = NULL;
static GHashTable *variables_hash_table = NULL;

GSList *jb_variables = NULL;

GSList *jb_variable_groups = NULL;

JBVariableGroup *jb_variable_group_compiler_options = NULL;
JBVariableGroup *jb_variable_group_installation_options = NULL;
JBVariableGroup *jb_variable_group_external_programs = NULL;

JBVariableType *jb_variable_type_bool = NULL;
JBVariableType *jb_variable_type_string = NULL;
JBVariableType *jb_variable_type_mode = NULL;

static JBVariableType *
variable_type_new (const char *name,
		   GType gtype,
		   gboolean (*from_string) (const char *str, GValue *value, GError **err),
		   char * (*to_string) (const GValue *value))
{
  JBVariableType *self;

  self = g_new0(JBVariableType, 1);
  self->name = g_strdup(name);
  self->gtype = gtype;
  self->from_string = from_string;
  self->to_string = to_string;

  return self;
}

static JBVariableType *
variable_type_add (const char *name,
		   GType gtype,
		   gboolean (*from_string) (const char *str, GValue *value, GError **err),
		   char * (*to_string) (const GValue *value))
{
  JBVariableType *type;

  type = variable_type_new(name, gtype, from_string, to_string);

  g_hash_table_insert(variable_types, type->name, type);

  return type;
}

static gboolean
bool_from_string (const char *str, GValue *value, GError **err)
{
  if (! strcmp(str, "yes"))
    g_value_set_boolean(value, TRUE);
  else if (! strcmp(str, "no"))
    g_value_set_boolean(value, FALSE);
  else
    {
      g_set_error(err, 0, 0, "\"yes\" or \"no\" expected");
      return FALSE;
    }

  return TRUE;
}

static char *
bool_to_string (const GValue *value)
{
  if (g_value_get_boolean(value))
    return g_strdup("yes");
  else
    return g_strdup("no");
}

static gboolean
string_from_string (const char *str, GValue *value, GError **err)
{
  g_value_set_string(value, *str == '\0' ? NULL : str);

  return TRUE;
}

static char *
string_to_string (const GValue *value)
{
  char *str;

  str = g_value_dup_string(value);

  return str == NULL ? g_strdup("") : str;
}

static gboolean
mode_from_string (const char *str, GValue *value, GError **err)
{
  guint32 mode;

  if (! jb_parse_uint32(str, 8, &mode, err))
    return FALSE;

  if ((mode & ~07777) != 0)
    {
      g_set_error(err, 0, 0, "mode out of range");
      return FALSE;
    }

  g_value_set_uint(value, mode);
  return TRUE;
}

static char *
mode_to_string (const GValue *value)
{
  unsigned int mode;

  mode = g_value_get_uint(value);

  return g_strdup_printf(JB_MODE_FORMAT, mode);
}

void
jb_variable_init (void)
{
  variable_types = g_hash_table_new(g_str_hash, g_str_equal);
  variables_hash_table = g_hash_table_new(g_str_hash, g_str_equal);

  jb_variable_group_compiler_options = jb_variable_add_group("Compiler options");
  jb_variable_group_installation_options = jb_variable_add_group("Installation options");
  jb_variable_group_external_programs = jb_variable_add_group("External programs");

  jb_variable_type_bool = variable_type_add("bool",
					    G_TYPE_BOOLEAN,
					    bool_from_string,
					    bool_to_string);
  jb_variable_type_string = variable_type_add("string",
					      G_TYPE_STRING,
					      string_from_string,
					      string_to_string);
  jb_variable_type_mode = variable_type_add("mode",
					    G_TYPE_UINT,
					    mode_from_string,
					    mode_to_string);
}

JBVariableGroup *
jb_variable_add_group (const char *name)
{
  JBVariableGroup *group;

  group = g_new0(JBVariableGroup, 1);
  group->name = g_strdup(name);

  jb_variable_groups = g_slist_append(jb_variable_groups, group);

  return group;
}

JBVariableType *
jb_variable_get_type (const char *name)
{
  g_return_val_if_fail(name != NULL, NULL);

  return g_hash_table_lookup(variable_types, name);
}

static JBVariable *
variable_new (JBVariableType *type,
	      const char *name,
	      const char *description,
	      JBVariableGroup *group,
	      JBVariableFlags flags)
{
  JBVariable *variable;

  g_return_val_if_fail(type != NULL, NULL);
  g_return_val_if_fail(name != NULL, NULL);

  variable = g_new0(JBVariable, 1);
  variable->type = type;
  variable->name = g_strdup(name);
  variable->description = g_strdup(description);
  variable->group = group;
  variable->flags = flags;

  g_value_init(&variable->value, type->gtype);

  return variable;
}

static void
add_variable (JBVariable *variable)
{
  g_return_if_fail(variable != NULL);
  g_return_if_fail(g_hash_table_lookup(variables_hash_table, variable->name) == NULL);

  g_hash_table_insert(variables_hash_table, variable->name, variable);
  jb_variables = g_slist_append(jb_variables, variable);
}

JBVariable *
jb_variable_get_variable (const char *name)
{
  g_return_val_if_fail(name != NULL, NULL);

  return g_hash_table_lookup(variables_hash_table, name);
}

JBVariable *
jb_variable_get_variable_or_error (const char *name)
{
  JBVariable *variable;

  g_return_val_if_fail(name != NULL, NULL);

  variable = jb_variable_get_variable(name);
  if (variable == NULL)
    g_error("variable \"%s\" does not exist", name);

  return variable;
}

JBVariable *
ensure_variable (JBVariableType *type, const char *name)
{
  JBVariable *variable;

  g_return_val_if_fail(name != NULL, NULL);

  variable = jb_variable_get_variable(name);
  if (variable == NULL)
    {
      variable = variable_new(type, name, NULL, NULL, 0);
      add_variable(variable);
    }

  return variable;
}

JBVariable *
jb_variable_add (JBVariableType *type,
		 const char *name,
		 const char *description,
		 JBVariableGroup *group,
		 JBVariableFlags flags)
{
  JBVariable *variable;

  g_return_val_if_fail(type != NULL, NULL);
  g_return_val_if_fail(name != NULL, NULL);

  variable = variable_new(type, name, description, group, flags | JB_VARIABLE_USER_SETTABLE);

  add_variable(variable);

  return variable;
}

void
jb_variable_add_bool (const char *name,
		      const char *description,
		      JBVariableGroup *group,
		      JBVariableFlags flags,
		      gboolean default_value)
{
  JBVariable *variable;

  g_return_if_fail(name != NULL);

  variable = jb_variable_add(jb_variable_type_bool, name, description, group, flags);

  g_value_set_boolean(&variable->value, default_value);
}

void
jb_variable_add_string (const char *name,
			const char *description,
			JBVariableGroup *group,
			JBVariableFlags flags,
			const char *default_value)
{
  JBVariable *variable;

  g_return_if_fail(name != NULL);

  variable = jb_variable_add(jb_variable_type_string, name, description, group, flags);

  g_value_set_string(&variable->value, default_value);
}

void
jb_variable_add_mode (const char *name,
		      const char *description,
		      JBVariableGroup *group,
		      JBVariableFlags flags,
		      mode_t default_value)
{
  JBVariable *variable;

  g_return_if_fail(name != NULL);

  variable = jb_variable_add(jb_variable_type_mode, name, description, group, flags);

  g_value_set_uint(&variable->value, default_value);
}

gboolean
jb_variable_get_bool (const char *name)
{
  JBVariable *variable;

  g_return_val_if_fail(name != NULL, FALSE);

  variable = jb_variable_get_variable_or_error(name);
  g_return_val_if_fail(variable->type == jb_variable_type_bool, FALSE);

  return g_value_get_boolean(&variable->value);
}

void
jb_variable_set_bool (const char *name, gboolean value)
{
  JBVariable *variable;

  g_return_if_fail(name != NULL);

  variable = ensure_variable(jb_variable_type_bool, name);

  g_value_set_boolean(&variable->value, value);
}

const char *
jb_variable_get_string (const char *name)
{
  const char *value;

  g_return_val_if_fail(name != NULL, FALSE);

  value = jb_variable_get_string_or_null(name);

  return value != NULL ? value : "";
}

const char *
jb_variable_get_string_or_null (const char *name)
{
  JBVariable *variable;

  g_return_val_if_fail(name != NULL, FALSE);

  variable = jb_variable_get_variable_or_error(name);
  g_return_val_if_fail(variable->type == jb_variable_type_string, FALSE);

  return g_value_get_string(&variable->value);
}

void
jb_variable_set_string (const char *name, const char *value)
{
  JBVariable *variable;

  g_return_if_fail(name != NULL);

  variable = ensure_variable(jb_variable_type_string, name);

  g_value_set_string(&variable->value, value);
}

mode_t
jb_variable_get_mode (const char *name)
{
  JBVariable *variable;

  g_return_val_if_fail(name != NULL, FALSE);

  variable = jb_variable_get_variable_or_error(name);
  g_return_val_if_fail(variable->type == jb_variable_type_mode, FALSE);

  return g_value_get_uint(&variable->value);
}

void
jb_variable_set_mode (const char *name, mode_t value)
{
  JBVariable *variable;

  g_return_if_fail(name != NULL);

  variable = ensure_variable(jb_variable_type_mode, name);

  g_value_set_uint(&variable->value, value);
}

void
jb_variable_set_package_flags (const char *name,
			       const char *cflags,
			       const char *cppflags,
			       const char *ldflags,
			       const char *libs)
{
  char *cflags_var;
  char *cppflags_var;
  char *ldflags_var;
  char *libs_var;

  g_return_if_fail(name != NULL);

  cflags_var = g_strdup_printf("%s-cflags", name);
  jb_variable_set_string(cflags_var, cflags);
  g_free(cflags_var);

  cppflags_var = g_strdup_printf("%s-cppflags", name);
  jb_variable_set_string(cppflags_var, cppflags);
  g_free(cppflags_var);

  ldflags_var = g_strdup_printf("%s-ldflags", name);
  jb_variable_set_string(ldflags_var, ldflags);
  g_free(ldflags_var);

  libs_var = g_strdup_printf("%s-libs", name);
  jb_variable_set_string(libs_var, libs);
  g_free(libs_var);
}

gboolean
jb_variable_set_from_string (JBVariable *self, const char *value, GError **err)
{
  g_return_val_if_fail(self != NULL, FALSE);
  g_return_val_if_fail(value != NULL, FALSE);

  return self->type->from_string(value, &self->value, err);
}

char *
jb_variable_to_string (JBVariable *self)
{
  g_return_val_if_fail(self != NULL, NULL);

  return self->type->to_string(&self->value);
}

static char *
get_expansion (const char *varname, GHashTable *extra_variables)
{
  JBVariable *variable;

  if (extra_variables != NULL)
    {
      const char *extra_value;

      if (g_hash_table_lookup_extended(extra_variables, varname, NULL, (gpointer) &extra_value))
	return g_strdup(extra_value != NULL ? extra_value : "");
    }

  variable = jb_variable_get_variable(varname);
  if (variable != NULL)
    return jb_variable_evaluate(variable);

  jb_error("unknown variable \"%s\"", varname);
}

static char *
expand_real (const char *str, GHashTable *extra_variables)
{
  GString *result;
  const char *p;

  g_return_val_if_fail(str != NULL, NULL);

  result = g_string_new(NULL);

  for (p = str; *p != '\0';)
    {
      char c = *p;

      if (c == '$')
	{
	  char *name;
	  char *value;
	  char *expanded_value;

	  if (p[1] == '$')
	    {
	      g_string_append_c(result, '$');
	      p += 2;
	      continue;
	    }

	  if (p[1] == '{')
	    {
	      char *end;

	      p += 2;

	      end = strchr(p, '}');
	      if (end == NULL)
		jb_error("unterminated variable reference");

	      if (end == p + 1)
		jb_error("empty variable reference");

	      name = g_strndup(p, end - p);
	      p = end + 1;
	    }
	  else
	    {
	      const char *end;

	      p++;

	      end = p + 1;
	      while (g_ascii_isalnum(*end) || *end == '-')
		end++;

	      if (end == p)
		jb_error("empty variable reference");

	      name = g_strndup(p, end - p);
	      p = end;
	    }

	  value = get_expansion(name, extra_variables);

	  /* do not pass the extra variables to the recursive expansion */
	  expanded_value = expand_real(value, NULL);
	  g_free(value);

	  g_string_append(result, expanded_value);
	  g_free(expanded_value);
	}
      else
	{
	  g_string_append_c(result, c);
	  p++;
	}
    }

  return g_string_free(result, FALSE);
}

char *
jb_variable_evaluate (JBVariable *variable)
{
  static GSList *evaluation_stack = NULL;
  char *str;
  char *value;

  g_return_val_if_fail(variable != NULL, NULL);

  if (evaluation_stack != NULL && evaluation_stack->data == variable)
    jb_error("variable \"%s\" references itself", variable->name);

  if (g_slist_find(evaluation_stack, variable) != NULL)
    jb_error("variable \"%s\" indirectly references itself", variable->name);

  evaluation_stack = g_slist_prepend(evaluation_stack, variable);

  str = jb_variable_to_string(variable);
  value = expand_real(str, NULL);
  g_free(str);

  evaluation_stack = g_slist_delete_link(evaluation_stack, evaluation_stack);

  return value;
}

char *
jb_variable_expand (const char *str, ...)
{
  va_list args;
  char *expanded;

  va_start(args, str);
  expanded = jb_variable_expandv(str, args);
  va_end(args);

  return expanded;
}

char *
jb_variable_expandv (const char *str, va_list args)
{
  GHashTable *extra_variables;
  const char *extra_name;
  char *expanded;

  g_return_val_if_fail(str != NULL, NULL);

  extra_variables = g_hash_table_new(g_str_hash, g_str_equal);

  while ((extra_name = va_arg(args, const char *)) != NULL)
    {
      const char *extra_value;

      extra_value = va_arg(args, const char *);
      /*
       * For consistency, extra_value can be NULL, since variables can
       * be NULL as well.
       */

      if (g_hash_table_lookup(extra_variables, extra_name) != NULL)
	g_error("extra variable \"%s\" specified twice", extra_name);

      g_hash_table_insert(extra_variables, (gpointer) extra_name, (gpointer) extra_value);
    }

  expanded = expand_real(str, extra_variables);
  g_hash_table_destroy(extra_variables);

  return expanded;
}

const char *
jb_variable_get_type_name (JBVariable *self)
{
  g_return_val_if_fail(self != NULL, NULL);

  return self->type->name;
}
