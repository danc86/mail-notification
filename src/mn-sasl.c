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

#include "config.h"
#include <glib.h>
#include <glib/gi18n.h>
#include <sasl/sasl.h>

/*** variables ***************************************************************/

static int use_count = 0;
G_LOCK_DEFINE_STATIC(use_count);

/*** implementation **********************************************************/

gboolean
mn_sasl_use (GError **err)
{
  gboolean success = TRUE;

  G_LOCK(use_count);
  if (++use_count == 1)
    {
      int status;

      status = sasl_client_init(NULL);
      if (status != SASL_OK)
	{
	  use_count--;
	  g_set_error(err, 0, 0, "%s", sasl_errstring(status, NULL, NULL));
	  success = FALSE;
	}
    }
  G_UNLOCK(use_count);

  return success;
}

void
mn_sasl_unuse (void)
{
  G_LOCK(use_count);
  g_return_if_fail(use_count > 0);
  if (--use_count == 0)
    sasl_done();
  G_UNLOCK(use_count);
}
