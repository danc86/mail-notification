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

#ifndef _MN_LOCKED_CALLBACK_H
#define _MN_LOCKED_CALLBACK_H

#include <stdarg.h>
#include <glib.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include <libgnomevfs/gnome-vfs.h>
#include <bonobo/bonobo-listener.h>

typedef struct _MNLockedCallback MNLockedGSource;
typedef struct _MNLockedCallback MNLockedBonoboListener;

typedef void (*MNLockFunc) (gpointer data);
typedef void (*MNUnlockFunc) (gpointer data);

void mn_locked_callback_init (void);

MNLockedGSource *mn_g_idle_add_locked (GSourceFunc function,
				       gpointer data,
				       MNLockFunc lock_func,
				       MNUnlockFunc unlock_func,
				       gpointer lock_data);
MNLockedGSource *mn_g_idle_add_gdk_locked (GSourceFunc function,
					   gpointer data);

MNLockedGSource *mn_g_timeout_add_locked (unsigned int interval,
					  GSourceFunc function,
					  gpointer data,
					  MNLockFunc lock_func,
					  MNUnlockFunc unlock_func,
					  gpointer lock_data);
MNLockedGSource *mn_g_timeout_add_gdk_locked (unsigned int interval,
					      GSourceFunc function,
					      gpointer data);

void mn_locked_g_source_remove (MNLockedGSource *source);
void mn_locked_g_source_clear (MNLockedGSource **source);

void mn_g_object_gconf_notification_add_gdk_locked (gpointer object,
						    const char *key,
						    GConfClientNotifyFunc function,
						    gpointer user_data);
void mn_g_object_gconf_notifications_add_gdk_locked (gpointer object, ...);

GnomeVFSResult mn_gnome_vfs_monitor_add_locked (GnomeVFSMonitorHandle **handle,
						const char *text_uri,
						GnomeVFSMonitorType monitor_type,
						GnomeVFSMonitorCallback callback,
						gpointer user_data);
GnomeVFSResult mn_gnome_vfs_monitor_cancel_locked (GnomeVFSMonitorHandle *handle);

MNLockedBonoboListener *mn_bonobo_event_source_client_add_listener_full_locked (Bonobo_Unknown object,
										BonoboListenerCallbackFn function,
										const char *opt_mask,
										CORBA_Environment *opt_ev,
										gpointer user_data,
										MNLockFunc lock_func,
										MNUnlockFunc unlock_func,
										gpointer lock_data);
void mn_bonobo_event_source_client_remove_listener_locked (Bonobo_Unknown object,
							   MNLockedBonoboListener *listener,
							   CORBA_Environment *opt_ev);

#endif /* _MN_LOCKED_CALLBACK_H */
