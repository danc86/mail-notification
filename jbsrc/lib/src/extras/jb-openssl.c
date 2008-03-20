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

#define JB_OPENSSL_CFLAGS	NULL
#define JB_OPENSSL_CPPFLAGS	NULL
#define JB_OPENSSL_LDFLAGS	"-lssl -lcrypto"

gboolean
jb_openssl_check (const char *minversion)
{
  gboolean result;
  char *cppflags;
  int major;
  int minor;
  int fix;
  int patch = 0;
  char cpatch;

  if (! minversion)
    /* the OPENSSL_VERSION_NUMBER format we use appeared in 0.9.5b */
    minversion = "0.9.5b";

  jb_message_checking("for OpenSSL >= %s", minversion);

  if (sscanf(minversion, "%d.%d.%d%c", &major, &minor, &fix, &cpatch) < 3)
    g_error("invalid version string `%s'", minversion);
  if (cpatch)
    patch = cpatch - 96; /* letter -> number */

  cppflags = g_strdup_printf("-DJB_OPENSSL_MAJOR=%i -DJB_OPENSSL_MINOR=%i -DJB_OPENSSL_FIX=%i -DJB_OPENSSL_PATCH=%i",
			     major, minor, fix, patch);

  result = jb_test_run_string("#include <openssl/opensslv.h>\n"
			      "int main () {\n"
			      "  if (OPENSSL_VERSION_NUMBER <\n"
			      "      (JB_OPENSSL_MAJOR << 28)\n"
			      "      + (JB_OPENSSL_MINOR << 20)\n"
			      "      + (JB_OPENSSL_FIX << 12)\n"
			      "      + (JB_OPENSSL_PATCH << 4))\n"
			      "    exit(1); /* version too old */\n"
			      "  exit(0); /* ok */\n"
			      "}\n",
			      JB_OPENSSL_CFLAGS,
			      cppflags,
			      JB_OPENSSL_LDFLAGS);

  g_free(cppflags);

  jb_message_result_bool(result);

  if (result)
    jb_variable_set_package_flags("openssl",
				  JB_OPENSSL_CFLAGS,
				  JB_OPENSSL_CPPFLAGS,
				  JB_OPENSSL_LDFLAGS);

  return result;
}

gboolean
jb_openssl_check_mt (void)
{
  gboolean result;

  jb_message_checking("whether OpenSSL supports multi-threading");

  result = jb_test_compile_string("#define OPENSSL_THREAD_DEFINES\n"
				  "#include <openssl/opensslconf.h>\n"
				  "int main () {\n"
				  "#ifndef OPENSSL_THREADS\n"
				  "#error \"no thread support\"\n"
				  "#endif\n"
				  "}\n",
				  JB_OPENSSL_CFLAGS,
				  JB_OPENSSL_CPPFLAGS);

  jb_message_result_bool(result);

  return result;
}
