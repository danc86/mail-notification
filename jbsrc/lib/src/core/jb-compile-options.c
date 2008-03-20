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

#include "jb-compile-options.h"
#include "jb-feature.h"
#include "jb-util.h"
#include "jb-variable.h"

JBCompileOptions *jb_compile_options = NULL;

/*
 * We store the data in variables so that it can be set in configure
 * (since variables are saved to the config file).
 */

static char *
get_variable_name (JBCompileOptions *self, const char *name)
{
  return g_strdup_printf("_compile-options-%s-%s", self->name, name);
}

static void
init_variable (JBCompileOptions *self, const char *name)
{
  char *varname;

  varname = get_variable_name(self, name);
  jb_variable_set_string(varname, NULL);
  g_free(varname);
}

static void
add_to_variable (JBCompileOptions *self, const char *name, const char *value)
{
  char *varname;
  const char *old_value;

  varname = get_variable_name(self, name);

  old_value = jb_variable_get_string_or_null(varname);
  if (old_value == NULL)
    jb_variable_set_string(varname, value);
  else
    {
      char *new_value;

      new_value = g_strdup_printf("%s %s", old_value, value);
      jb_variable_set_string(varname, new_value);
      g_free(new_value);
    }

  g_free(varname);
}

static const char *
get_variable (JBCompileOptions *self, const char *name)
{
  char *varname;
  const char *value;

  varname = get_variable_name(self, name);
  value = jb_variable_get_string_or_null(varname);
  g_free(varname);

  return value;
}

JBCompileOptions *
jb_compile_options_new (const char *name)
{
  JBCompileOptions *self;

  g_return_val_if_fail(name != NULL, NULL);

  self = g_new0(JBCompileOptions, 1);
  self->name = g_strdup(name);

  init_variable(self, "cflags");
  init_variable(self, "cppflags");
  init_variable(self, "ldflags");
  init_variable(self, "gob2flags");

  return self;
}

void
jb_compile_options_add_cflags (JBCompileOptions *self, const char *cflags)
{
  g_return_if_fail(self != NULL);
  g_return_if_fail(cflags != NULL);

  add_to_variable(self, "cflags", cflags);
}

const char *
jb_compile_options_get_cflags (JBCompileOptions *self)
{
  g_return_val_if_fail(self != NULL, NULL);

  return get_variable(self, "cflags");
}

void
jb_compile_options_add_cppflags (JBCompileOptions *self, const char *cppflags)
{
  g_return_if_fail(self != NULL);
  g_return_if_fail(cppflags != NULL);

  add_to_variable(self, "cppflags", cppflags);
}

const char *
jb_compile_options_get_cppflags (JBCompileOptions *self)
{
  GString *cppflags;
  GSList *l;

  g_return_val_if_fail(self != NULL, NULL);

  if (self->cppflags != NULL)
    return self->cppflags;

  cppflags = g_string_new(get_variable(self, "cppflags"));

  /* add variable defines if working on the global JBCompileOptions */

  if (self == jb_compile_options)
    JB_LIST_FOREACH(l, jb_variables)
      {
	JBVariable *variable = l->data;
	char *symbol;

	if ((variable->flags & JB_VARIABLE_C_DEFINE) == 0)
	  continue;

	g_assert(variable->type == jb_variable_type_bool);

	symbol = g_strdelimit(g_ascii_strup(variable->name, -1), "-", '_');
	g_string_append_printf(cppflags, " -DWITH_%s=%i",
			       symbol,
			       g_value_get_boolean(&variable->value) ? 1 : 0);
	g_free(symbol);
      }

  self->cppflags = g_string_free(cppflags, FALSE);

  return self->cppflags;
}

void
jb_compile_options_add_ldflags (JBCompileOptions *self, const char *ldflags)
{
  g_return_if_fail(self != NULL);
  g_return_if_fail(ldflags != NULL);

  add_to_variable(self, "ldflags", ldflags);
}

const char *
jb_compile_options_get_ldflags (JBCompileOptions *self)
{
  g_return_val_if_fail(self != NULL, NULL);

  return get_variable(self, "ldflags");
}

void
jb_compile_options_add_gob2flags (JBCompileOptions *self, const char *gob2flags)
{
  g_return_if_fail(self != NULL);
  g_return_if_fail(gob2flags != NULL);
  g_return_if_fail(jb_feature_is_enabled(&jb_gob2_feature));

  add_to_variable(self, "gob2flags", gob2flags);
}

const char *
jb_compile_options_get_gob2flags (JBCompileOptions *self)
{
  g_return_val_if_fail(self != NULL, NULL);

  return get_variable(self, "gob2flags");
}

void
jb_compile_options_add_package (JBCompileOptions *self, const char *name)
{
  char *value;

  g_return_if_fail(self != NULL);
  g_return_if_fail(name != NULL);

  value = g_strdup_printf("$%s-cflags", name);
  jb_compile_options_add_cflags(self, value);
  g_free(value);

  value = g_strdup_printf("$%s-cppflags", name);
  jb_compile_options_add_cppflags(self, value);
  g_free(value);

  value = g_strdup_printf("$%s-ldflags", name);
  jb_compile_options_add_ldflags(self, value);
  g_free(value);
}

/* evaluates the provided values immediately */
void
jb_compile_options_add_string_defines (JBCompileOptions *self,
				       const char *name,
				       ...)
{
  va_list args;

  g_return_if_fail(self != NULL);

  va_start(args, name);

  while (name != NULL)
    {
      const char *value;
      char *evaluated;
      char *c_quoted;
      char *shell_quoted;
      char *cppflags;

      value = va_arg(args, const char *);
      g_assert(value != NULL);

      /* expand variables since we need to quote the value */
      evaluated = jb_variable_expand(value, NULL);

      c_quoted = jb_c_quote(evaluated);
      g_free(evaluated);

      shell_quoted = g_shell_quote(c_quoted);
      g_free(c_quoted);

      cppflags = g_strdup_printf("-D%s=%s", name, shell_quoted);
      g_free(shell_quoted);

      jb_compile_options_add_cppflags(self, cppflags);
      g_free(cppflags);

      name = va_arg(args, const char *);
    }

  va_end(args);
}
