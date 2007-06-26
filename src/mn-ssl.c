/*
 * Mail Notification
 * Copyright (C) 2003-2007 Jean-Yves Lefort <jylefort@brutele.be>
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"
#include <glib.h>
#include <glib/gi18n.h>

typedef GMutex CRYPTO_dynlock_value;

#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include "mn-ssl.h"

/*** variables ***************************************************************/

static gboolean attempted = FALSE;
static SSL_CTX *ctx = NULL;
static char *init_error = NULL;

static int num_locks;
static GMutex **locks;

/* a general purpose global SSL lock */
G_LOCK_DEFINE(mn_ssl);

/*** functions ***************************************************************/

static void mn_ssl_locking_cb (int mode, int n, const char *file, int line);
static unsigned long mn_ssl_id_cb (void);

static void mn_ssl_init_threading (void);

/*** implementation **********************************************************/

static void
mn_ssl_locking_cb (int mode, int n, const char *file, int line)
{
  g_assert(n >= 0 && n < num_locks);

  if ((mode & CRYPTO_LOCK) != 0)
    g_mutex_lock(locks[n]);
  else
    g_mutex_unlock(locks[n]);
}

static unsigned long
mn_ssl_id_cb (void)
{
  return (unsigned long) g_thread_self();
}

static struct CRYPTO_dynlock_value *
mn_ssl_dynlock_create_cb (const char *file, int line)
{
  return (struct CRYPTO_dynlock_value *) g_mutex_new();
}

static void
mn_ssl_dynlock_locking_cb (int mode,
			   struct CRYPTO_dynlock_value *lock,
			   const char *file,
			   int line)
{
  if ((mode & CRYPTO_LOCK) != 0)
    g_mutex_lock((GMutex *) lock);
  else
    g_mutex_unlock((GMutex *) lock);
}

static void
mn_ssl_dynlock_destroy_cb (struct CRYPTO_dynlock_value *lock,
			   const char *file,
			   int line)
{
  g_mutex_free((GMutex *) lock);
}

static void
mn_ssl_init_threading (void)
{
  int i;

  num_locks = CRYPTO_num_locks();

  locks = g_new(GMutex *, num_locks);
  for (i = 0; i < num_locks; i++)
    locks[i] = g_mutex_new();

  CRYPTO_set_locking_callback(mn_ssl_locking_cb);
  CRYPTO_set_id_callback(mn_ssl_id_cb);

  CRYPTO_set_dynlock_create_callback(mn_ssl_dynlock_create_cb);
  CRYPTO_set_dynlock_lock_callback(mn_ssl_dynlock_locking_cb);
  CRYPTO_set_dynlock_destroy_callback(mn_ssl_dynlock_destroy_cb);
}

SSL_CTX *
mn_ssl_init (GError **err)
{
  /*
   * SSL_CTX_new() will fail the second time it is called, so we just
   * keep the same context for the whole application lifetime.
   */

  G_LOCK(mn_ssl);
  if (! attempted)
    {
      SSL_library_init();
      SSL_load_error_strings();

      mn_ssl_init_threading();

      ctx = SSL_CTX_new(SSLv23_client_method());
      if (ctx)
	{
	  SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
	  SSL_CTX_set_default_verify_paths(ctx); /* #19578 */
	}
      else
	init_error = g_strdup(mn_ssl_get_error());

      attempted = TRUE;
    }

  if (! ctx)
    {
      g_assert(init_error != NULL);
      g_set_error(err, 0, 0, "%s", init_error);
    }
  G_UNLOCK(mn_ssl);

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
