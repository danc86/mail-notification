/* 
 * Copyright (c) 2004 Jean-Yves Lefort <jylefort@brutele.be>
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
#include <limits.h>
#include <glib.h>
#include "mn-sgml-entities.h"

/*** functions ***************************************************************/

static gunichar mn_sgml_ref_get_unichar (const char *ref);

/*** implementation **********************************************************/

static gunichar
mn_sgml_ref_get_unichar (const char *ref)
{
  gunichar c = 0;		/* 0 means "invalid reference" */

  g_return_val_if_fail(ref != NULL, 0);

  if (*ref == '#')
    {				/* numeric reference */
      const char *nptr;
      int base;

      if (*(ref + 1) == 'x' || *(ref + 1) == 'X')
	{			/* hexadecimal number */
	  nptr = ref + 2;
	  base = 16;
	}
      else
	{			/* decimal number */
	  nptr = ref + 1;
	  base = 10;
	}

      if (*nptr)
	{
	  char *end;
	  unsigned long code;
	  
	  code = strtoul(nptr, &end, base);
	  if (*end == 0)	/* could convert */
	    c = code;
	}
    }
  else
    {				/* entity reference */
      int i;

      for (i = 0; i < G_N_ELEMENTS(entities); i++)
	if (! strcmp(ref, entities[i].name))
	  {
	    c = entities[i].character;
	    break;
	  }
    }
  
  return c;
}

/**
 * mn_sgml_ref_expand:
 * @string: a string to expand
 *
 * Parses @str, expanding every SGML character reference to its
 * Unicode character.
 *
 * Return value: a newly-allocated UTF-8 string.
 **/
char *
mn_sgml_ref_expand (const char *str)
{
  GString *unescaped;
  char *ampersand;
  char *start;

  g_return_val_if_fail(str != NULL, NULL);

  unescaped = g_string_new(NULL);
  start = (char *) str;

  while ((ampersand = strchr(start, '&')))
    {
      char *semicolon = strchr(ampersand, ';');

      if (semicolon)
	{
	  char *ref;
	  gunichar c;

	  ref = g_strndup(ampersand + 1, semicolon - ampersand - 1);
	  c = mn_sgml_ref_get_unichar(ref);
	  g_free(ref);

	  g_string_append_len(unescaped, start, ampersand - start);
	  if (c)
	    g_string_append_unichar(unescaped, c);
	  else			/* invalid reference, append it raw */
	    g_string_append_len(unescaped, ampersand, semicolon - ampersand + 1);

	  start = semicolon + 1;
	}
      else
	break;
    }

  g_string_append(unescaped, start);

  return g_string_free(unescaped, FALSE);
}
