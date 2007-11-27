/*
 * Mail Notification
 * Copyright (C) 2003-2007 Jean-Yves Lefort <jylefort@brutele.be>
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

#include "config.h"
#include "mn-keyring.h"

/*** types *******************************************************************/

typedef struct
{
  GMutex	*mutex;
  GCond		*cond;
  gboolean	done;
} Request;

typedef struct
{
  Request			request;

  const char			*username;
  const char			*domain;
  const char			*server;
  const char			*protocol;
  const char			*authtype;
  int				port;

  MNKeyringRequestCallback	request_callback;
  gpointer			data;

  char				*password;
} GetPasswordInfo;

typedef struct
{
  Request			request;

  const char			*keyring;
  const char			*username;
  const char			*domain;
  const char			*server;
  const char			*protocol;
  const char			*authtype;
  int				port;
  const char			*password;

  MNKeyringRequestCallback	request_callback;
  gpointer			data;

  GnomeKeyringResult		result;
  guint32			item_id;
} SetPasswordInfo;

/*** implementation **********************************************************/

static void
request_perform (Request *request, GSourceFunc perform_callback, gpointer data)
{
  g_return_if_fail(request != NULL);
  g_return_if_fail(perform_callback != NULL);

  request->mutex = g_mutex_new();
  request->cond = g_cond_new();
  request->done = FALSE;

  /*
   * Work around http://bugzilla.gnome.org/show_bug.cgi?id=474695 by
   * calling the gnome-keyring async function from a main loop
   * callback.
   */
  g_idle_add(perform_callback, data);

  g_mutex_lock(request->mutex);

  if (! request->done)
    g_cond_wait(request->cond, request->mutex);

  g_mutex_unlock(request->mutex);

  g_mutex_free(request->mutex);
  g_cond_free(request->cond);
}

static void
request_signal (Request *request)
{
  g_return_if_fail(request != NULL);

  g_mutex_lock(request->mutex);

  request->done = TRUE;
  g_cond_signal(request->cond);

  g_mutex_unlock(request->mutex);
}

static void
get_password_sync_cb (GnomeKeyringResult result,
		      GList *list,
		      gpointer data)
{
  GetPasswordInfo *info = data;

  info->request_callback(NULL, info->data);

  if (result == GNOME_KEYRING_RESULT_OK && list)
    {
      GnomeKeyringNetworkPasswordData *data = list->data;
      info->password = g_strdup(data->password);
    }

  request_signal(&info->request);
}

static gboolean
get_password_sync_perform_cb (gpointer data)
{
  GetPasswordInfo *info = data;
  gpointer request;

  request = gnome_keyring_find_network_password(info->username,
						info->domain,
						info->server,
						NULL,
						info->protocol,
						info->authtype,
						info->port,
						get_password_sync_cb,
						info,
						NULL);

  info->request_callback(request, info->data);

  return FALSE;			/* remove source */
}

char *
mn_keyring_get_password_sync (const char *username,
			      const char *domain,
			      const char *server,
			      const char *protocol,
			      const char *authtype,
			      int port,
			      MNKeyringRequestCallback request_callback,
			      gpointer data)
{
  GetPasswordInfo info;

  g_return_val_if_fail(request_callback != NULL, NULL);

  info.username = username;
  info.domain = domain;
  info.server = server;
  info.protocol = protocol;
  info.authtype = authtype;
  info.port = port;

  info.request_callback = request_callback;
  info.data = data;

  info.password = NULL;

  request_perform(&info.request, get_password_sync_perform_cb, &info);

  return info.password;
}

static void
set_password_sync_cb (GnomeKeyringResult result,
		      guint32 item_id,
		      gpointer data)
{
  SetPasswordInfo *info = data;

  info->request_callback(NULL, info->data);

  info->result = result;
  info->item_id = item_id;

  request_signal(&info->request);
}

static gboolean
set_password_sync_perform_cb (gpointer data)
{
  SetPasswordInfo *info = data;
  gpointer request;

  request = gnome_keyring_set_network_password(info->keyring,
					       info->username,
					       info->domain,
					       info->server,
					       NULL,
					       info->protocol,
					       info->authtype,
					       info->port,
					       info->password,
					       set_password_sync_cb,
					       info,
					       NULL);

  info->request_callback(request, info->data);

  return FALSE;			/* remove source */
}

GnomeKeyringResult
mn_keyring_set_password_sync (const char *keyring,
			      const char *username,
			      const char *domain,
			      const char *server,
			      const char *protocol,
			      const char *authtype,
			      int port,
			      const char *password,
			      guint32 *item_id,
			      MNKeyringRequestCallback request_callback,
			      gpointer data)
{
  SetPasswordInfo info;

  g_return_val_if_fail(password != NULL, GNOME_KEYRING_RESULT_BAD_ARGUMENTS);
  g_return_val_if_fail(item_id != NULL, GNOME_KEYRING_RESULT_BAD_ARGUMENTS);
  g_return_val_if_fail(request_callback != NULL, GNOME_KEYRING_RESULT_BAD_ARGUMENTS);

  info.keyring = keyring;
  info.username = username;
  info.domain = domain;
  info.server = server;
  info.protocol = protocol;
  info.authtype = authtype;
  info.port = port;
  info.password = password;

  info.request_callback = request_callback;
  info.data = data;

  request_perform(&info.request, set_password_sync_perform_cb, &info);

  if (info.result == GNOME_KEYRING_RESULT_OK)
    *item_id = info.item_id;

  return info.result;
}
