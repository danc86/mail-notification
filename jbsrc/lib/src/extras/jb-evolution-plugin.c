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

#include "jb-tests.h"
#include "jb-util.h"
#include "jb-variable.h"

gboolean
jb_evolution_plugin_check (const char *minversion)
{
  char *packages;
  gboolean result;
  char *plugindir;

  if (! minversion)
    minversion = "2.12";

  packages = g_strdup_printf("evolution-plugin >= %s", minversion);
  result = jb_check_packages("Evolution", "evolution-plugin", packages);
  g_free(packages);

  if (! result)
    return FALSE;

  jb_message_checking("for the Evolution plugin directory");
  plugindir = jb_get_package_variable("evolution-plugin", "plugindir");
  jb_message_result_string(plugindir ? plugindir : "not found");

  if (! plugindir)
    return FALSE;

  jb_variable_set_string("evolution-plugindir", plugindir);
  g_free(plugindir);

  return TRUE;
}
