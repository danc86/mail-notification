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

#include <stdlib.h>
#include "jb-config.h"
#include "jb-variable.h"
#include "jb-util.h"

void
jb_config_save (void)
{
  GString *config;
  GSList *l;

  config = g_string_new(NULL);

  JB_LIST_FOREACH(l, jb_variables)
    {
      JBVariable *variable = l->data;
      char *value;
      char *escaped;

      value = jb_variable_to_string(variable);
      escaped = g_strescape(value, NULL);
      g_free(value);

      g_string_append_printf(config,
			     "%s|%s|%i|%s\n",
			     variable->name,
			     jb_variable_get_type_name(variable),
			     variable->flags,
			     escaped);
      g_free(escaped);
    }

  jb_write_file_or_exit(JB_CONFIG_FILE, config->str);
  g_string_free(config, TRUE);
}

void
jb_config_load (void)
{
  char *config;
  char **lines;
  int i;

  config = jb_read_file_or_exit(JB_CONFIG_FILE);
  lines = g_strsplit(config, "\n", 0);
  g_free(config);

  for (i = 0; lines[i] != NULL; i++)
    {
      int lineno = i + 1;
      char **fields;
      JBVariable *variable;
      JBVariableType *type;
      char *unescaped;
      GError *err = NULL;

      if (*lines[i] == '\0')
	continue;

      fields = g_strsplit(lines[i], "|", 4);
      if (g_strv_length(fields) != 4)
	jb_error("%s:%i: cannot parse line", JB_CONFIG_FILE, lineno);

      type = jb_variable_get_type(fields[1]);
      if (type == NULL)
	jb_error("%s:%i: invalid type \"%s\"", JB_CONFIG_FILE, lineno, fields[1]);

      variable = jb_variable_get_variable(fields[0]);
      if (variable == NULL)
	variable = jb_variable_add(type, fields[0], NULL, NULL, atoi(fields[2]));
      else
	{
	  if (type != variable->type)
	    jb_error("%s:%i: inconsistent type \"%s\" (type \"%s\" expected)",
		     JB_CONFIG_FILE,
		     lineno,
		     type->name,
		     variable->type->name);
	}

      unescaped = g_strcompress(fields[3]);

      if (! jb_variable_set_from_string(variable, unescaped, &err))
	jb_error("%s:%i: invalid value for type \"%s\": %s",
		 JB_CONFIG_FILE,
		 lineno,
		 type->name,
		 err->message);

      g_free(unescaped);
    }

  g_strfreev(lines);
}
