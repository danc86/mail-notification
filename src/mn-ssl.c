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

#include <glib.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

/*** variables ***************************************************************/

static gboolean initialized = FALSE;
G_LOCK_DEFINE_STATIC(initialized);

static SSL_CTX *global_ctx;
static char *init_error;

/*** implementation **********************************************************/

SSL_CTX *
mn_ssl_init (GError **err)
{
  SSL_CTX *ctx;

  /*
   * SSL_CTX_new() will fail the second time it is called, so we just
   * keep the same context for the whole application lifetime.
   */

  G_LOCK(initialized);
  if (! initialized)
    {
      SSL_library_init();
      SSL_load_error_strings();

      global_ctx = SSL_CTX_new(SSLv23_client_method());
      if (! global_ctx)
	init_error = g_strdup(ERR_reason_error_string(ERR_get_error()));

      initialized = TRUE;
    }

  ctx = global_ctx;
  if (! ctx)
    {
      g_return_val_if_fail(init_error != NULL, NULL);
      g_set_error(err, 0, 0, "%s", init_error);
    }
  G_UNLOCK(initialized);

  return ctx;
}
