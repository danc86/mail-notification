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

#include "config.h"
#include <glib.h>
#include <glib/gi18n.h>
#include <sasl/sasl.h>

/*** variables ***************************************************************/

static gboolean attempted = FALSE;
static gboolean initialized = FALSE;
static char *init_error = NULL;

G_LOCK_DEFINE_STATIC(init);

/*** implementation **********************************************************/

gboolean
mn_sasl_init (GError **err)
{
  /*
   * Bad things may happen if we call sasl_client_init() again after
   * having called sasl_done(), so we just keep SASL initialized for
   * the whole application lifetime.
   */

  G_LOCK(init);
  if (! attempted)
    {
      int status;

      status = sasl_client_init(NULL);
      if (status == SASL_OK)
	initialized = TRUE;
      else
	init_error = g_strdup(sasl_errstring(status, NULL, NULL));

      attempted = TRUE;
    }

  if (! initialized)
    {
      g_assert(init_error != NULL);
      g_set_error(err, 0, 0, "%s", init_error);
    }
  G_UNLOCK(init);

  return initialized;
}
