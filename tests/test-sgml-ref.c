/* 
 * Copyright (C) 2005 Jean-Yves Lefort <jylefort@brutele.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include "mn-sgml-ref.h"

#define FAIL							\
  {								\
    g_printerr("failure at %s\n", G_STRLOC);			\
    exit(1);							\
  }

int
main (int argc, char **argv)
{
  if (strcmp(mn_sgml_ref_expand("&lt;tag&gt;"), "<tag>") != 0)
    FAIL;
  if (strcmp(mn_sgml_ref_expand("&#65;&#66;&#67;"), "ABC") != 0)
    FAIL;
  if (strcmp(mn_sgml_ref_expand("&#65&#66&#67"), "ABC") != 0)
    FAIL;
  if (strcmp(mn_sgml_ref_expand("&#x41;&#x42;&#x43;"), "ABC") != 0)
    FAIL;
  if (strcmp(mn_sgml_ref_expand("&#X41;&#X42;&#X43;"), "ABC") != 0)
    FAIL;
  if (strcmp(mn_sgml_ref_expand("&lt;tag&gt; &unknown;"), "<tag> &unknown;") != 0)
    FAIL;
  if (strcmp(mn_sgml_ref_expand("&lt;tag&gt; &unknown end"), "<tag> &unknown end") != 0)
    FAIL;

  g_print("+++ SGML-REF TEST PASSED\n");

  return 0;
}
