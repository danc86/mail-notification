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

#include <stdio.h>
#include "jb-tests.h"
#include "jb-variable.h"
#include "jb-util.h"

#define JB_SASL2_CFLAGS		NULL
#define JB_SASL2_CPPFLAGS	NULL
#define JB_SASL2_LDFLAGS	NULL
#define JB_SASL2_LIBS		"-lsasl2"

gboolean
jb_sasl2_check (const char *minversion)
{
  gboolean result;
  char *cppflags;
  int major;
  int minor;
  int step = 0;
  int patch = 0;

  if (! minversion)
    minversion = "2.0";

  jb_message_checking("for Cyrus SASL >= %s", minversion);

  if (sscanf(minversion, "%d.%d.%d.%d", &major, &minor, &step, &patch) < 2)
    g_error("invalid version string `%s'", minversion);

  cppflags = g_strdup_printf("-DJB_SASL2_MAJOR=%i -DJB_SASL2_MINOR=%i -DJB_SASL2_STEP=%i -DJB_SASL2_PATCH=%i",
			     major, minor, step, patch);

  result = jb_test_run_string("#include <sasl/sasl.h>\n"
			      "int main () {\n"
			      "  int major, minor, step, patch;\n"
			      "  sasl_version_info(0, 0, &major, &minor, &step, &patch);\n"
			      "  if ((major << 24) + (minor << 16) + (step << 8) + patch\n"
			      "      < (JB_SASL2_MAJOR << 24) + (JB_SASL2_MINOR << 16) + (JB_SASL2_STEP << 8) + JB_SASL2_PATCH)\n"
			      "    exit(2); /* version too old */\n"
			      "  exit(0);\n"
			      "}\n",
			      JB_SASL2_CFLAGS,
			      cppflags,
			      JB_SASL2_LDFLAGS,
			      JB_SASL2_LIBS);

  g_free(cppflags);

  jb_message_result_bool(result);

  if (result)
    jb_variable_set_package_flags("sasl2",
				  JB_SASL2_CFLAGS,
				  JB_SASL2_CPPFLAGS,
				  JB_SASL2_LDFLAGS,
				  JB_SASL2_LIBS);

  return result;
}
