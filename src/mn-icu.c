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

#include "config.h"
#include <string.h>
#include <glib.h>
#include <unicode/uclean.h>
#include <unicode/utypes.h>
#include <unicode/putil.h>
#include <unicode/uiter.h>
#include <unicode/ustring.h>

/*** variables ***************************************************************/

static int use_count = 0;
G_LOCK_DEFINE_STATIC(use_count);

/*** implementation **********************************************************/

gboolean
mn_icu_use (GError **err)
{
  gboolean success;
  UErrorCode status = U_ZERO_ERROR;

  G_LOCK(use_count);

  /* u_init() must be called in every thread, so we always call it */

  u_init(&status);
  if (U_SUCCESS(status))
    {
      use_count++;
      success = TRUE;
    }
  else
    {
      g_set_error(err, 0, 0, "%s", u_errorName(status));
      success = FALSE;
    }

  G_UNLOCK(use_count);

  return success;
}

void
mn_icu_unuse (void)
{
  G_LOCK(use_count);
  g_return_if_fail(use_count > 0);
  if (--use_count == 0)
    u_cleanup();
  G_UNLOCK(use_count);
}

UChar *
mn_icu_ustr_new (const char *str)
{
  UChar *ustr;

  g_return_val_if_fail(str != NULL, NULL);

  ustr = g_new(UChar, sizeof(UChar) * (strlen(str) + 1));
  u_uastrcpy(ustr, str);
  
  return ustr;
}
