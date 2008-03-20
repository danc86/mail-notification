/*
 * Mail Notification
 * Copyright (C) 2003-2008 Jean-Yves Lefort <jylefort@brutele.be>
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

#include <errno.h>
#include <glib.h>
#include <glib/gi18n.h>

typedef GMutex CRYPTO_dynlock_value;

#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include "mn-ssl.h"

static gboolean attempted = FALSE;
static SSL_CTX *ctx = NULL;
static char *init_error = NULL;

static int num_locks;
static GMutex **locks;

/* a general purpose global SSL lock */
G_LOCK_DEFINE(mn_ssl);

static void
locking_cb (int mode, int n, const char *file, int line)
{
  g_assert(n >= 0 && n < num_locks);

  if ((mode & CRYPTO_LOCK) != 0)
    g_mutex_lock(locks[n]);
  else
    g_mutex_unlock(locks[n]);
}

static unsigned long
id_cb (void)
{
  return (unsigned long) g_thread_self();
}

static struct CRYPTO_dynlock_value *
dynlock_create_cb (const char *file, int line)
{
  return (struct CRYPTO_dynlock_value *) g_mutex_new();
}

static void
dynlock_locking_cb (int mode,
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
dynlock_destroy_cb (struct CRYPTO_dynlock_value *lock,
		    const char *file,
		    int line)
{
  g_mutex_free((GMutex *) lock);
}

static void
init_threading (void)
{
  int i;

  num_locks = CRYPTO_num_locks();

  locks = g_new(GMutex *, num_locks);
  for (i = 0; i < num_locks; i++)
    locks[i] = g_mutex_new();

  CRYPTO_set_locking_callback(locking_cb);
  CRYPTO_set_id_callback(id_cb);

  CRYPTO_set_dynlock_create_callback(dynlock_create_cb);
  CRYPTO_set_dynlock_lock_callback(dynlock_locking_cb);
  CRYPTO_set_dynlock_destroy_callback(dynlock_destroy_cb);
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

      init_threading();

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
  static GStaticPrivate buf_key = G_STATIC_PRIVATE_INIT;
  char *buf;

  /*
   * We use a per-thread buffer so that the caller does not have to
   * free the returned string.
   */

  buf = g_static_private_get(&buf_key);
  if (! buf)
    {
      buf = g_new(char, 120);	/* the size is specified by the manpage */
      g_static_private_set(&buf_key, buf, g_free);
    }

  return ERR_error_string(ERR_get_error(), buf);
}

const char *
mn_ssl_get_io_error (const SSL *ssl, int ret)
{
  g_return_val_if_fail(ssl != NULL, NULL);

  switch (SSL_get_error(ssl, ret))
    {
    case SSL_ERROR_SYSCALL:
      if (ERR_peek_error() == 0)
	switch (ret)
	  {
	  case 0:
	    return "EOF";

	  case -1:
	    /*
	     * This assumes that a UNIX BIO is in use (it is always
	     * the case in MN).
	     */
	    return g_strerror(errno);
	  }
      else
	return mn_ssl_get_error();
      break;

    case SSL_ERROR_SSL:
      return mn_ssl_get_error();
    }

  return _("unknown SSL/TLS error");
}
