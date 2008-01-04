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

#ifndef _MN_KEYRING_H
#define _MN_KEYRING_H

#include <gnome-keyring.h>

/*
 * These functions augment the gnome-keyring sync API by taking a
 * callback parameter which is invoked when the request is started and
 * completed. This allows another thread to call
 * gnome_keyring_cancel_request() while the calling thread performs
 * the request. The provided callback is executed in the thread that
 * runs the GLib main loop, with the GDK lock not held.
 */

typedef void (*MNKeyringRequestCallback) (gpointer request, gpointer data);

/* returns NULL if not found */
char *mn_keyring_get_password_sync (const char *username,
				    const char *domain,
				    const char *server,
				    const char *protocol,
				    const char *authtype,
				    int port,
				    MNKeyringRequestCallback request_callback,
				    gpointer data);

GnomeKeyringResult mn_keyring_set_password_sync (const char *keyring,
						 const char *username,
						 const char *domain,
						 const char *server,
						 const char *protocol,
						 const char *authtype,
						 int port,
						 const char *password,
						 guint32 *item_id,
						 MNKeyringRequestCallback request_callback,
						 gpointer data);

#endif /* _MN_KEYRING_H */
