/* 
 * Copyright (C) 2004, 2005 Jean-Yves Lefort <jylefort@brutele.be>
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
	    return code;
	}
    }
  else
    {				/* entity reference */
      int i;

      for (i = 0; i < G_N_ELEMENTS(entities); i++)
	if (! strcmp(ref, entities[i].name))
	  return entities[i].character;
    }
  
  return 0;			/* invalid reference */
}

/**
 * mn_sgml_ref_expand:
 * @str: a nul-terminated string.
 *
 * Parses @str, expanding its SGML character references and XHTML
 * character entities into their Unicode character value.
 *
 * Numerical SGML character references as well as XHTML entities are
 * supported. Unsupported entities will be inserted verbatim into the
 * result.
 *
 * Return value: the expansion of str. The returned string should be
 * freed when no longer needed.
 **/
char *
mn_sgml_ref_expand (const char *str)
{
  GString *unescaped;
  const char *start;

  g_return_val_if_fail(str != NULL, NULL);

  unescaped = g_string_new(NULL);

  while ((start = strchr(str, '&')))
    {
      const char *end;
      gunichar c;

      end = strpbrk(start + 1, "; &\t\n");
      if (! end)
	end = strchr(start + 1, 0);
      
      {
	char ref[end - start];

	strncpy(ref, start + 1, end - start - 1);
	ref[end - start - 1] = 0;

	c = mn_sgml_ref_get_unichar(ref);
      }
	
      if (*end == ';')		/* semicolon is part of entity, skip it */
	end++;

      g_string_append_len(unescaped, str, start - str);
      if (c)
	g_string_append_unichar(unescaped, c);
      else			/* invalid reference, append it raw */
	g_string_append_len(unescaped, start, end - start);
	
      str = end;
    }

  g_string_append(unescaped, str);

  return g_string_free(unescaped, FALSE);
}
