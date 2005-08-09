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
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "mn-ssl.h"

/*** variables ***************************************************************/

static gboolean attempted = FALSE;
static SSL_CTX *ctx = NULL;
static char *init_error = NULL;

G_LOCK_DEFINE_STATIC(init);

/*** implementation **********************************************************/

SSL_CTX *
mn_ssl_init (GError **err)
{
  /*
   * SSL_CTX_new() will fail the second time it is called, so we just
   * keep the same context for the whole application lifetime.
   */

  G_LOCK(init);
  if (! attempted)
    {
      SSL_library_init();
      SSL_load_error_strings();

      ctx = SSL_CTX_new(SSLv23_client_method());
      if (ctx)
	SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
      else
	init_error = g_strdup(mn_ssl_get_error());

      attempted = TRUE;
    }

  if (! ctx)
    {
      g_assert(init_error != NULL);
      g_set_error(err, 0, 0, "%s", init_error);
    }
  G_UNLOCK(init);

  return ctx;
}

const char *
mn_ssl_get_error (void)
{
  const char *error;

  error = ERR_reason_error_string(ERR_get_error());
  if (! error)
    error = _("unknown SSL/TLS error");

  return error;
}
