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

#include <glib.h>
#include <libgnomevfs/gnome-vfs.h>
#include "mn-locked-callback.h"
#include "mn-conf.h"
#include "mn-util.h"

/*
 * About MNLockedCallback:
 *
 * Consider the following code:
 *
 *   static gboolean idle_cb (gpointer data)
 *   {
 *     MyObject *object = data;
 *
 *     GDK_THREADS_ENTER();
 *     ...do something with object
 *     GDK_THREADS_LEAVE();
 *
 *     return TRUE;
 *   }
 *
 *   static void my_object_init (MyObject *object)
 *   {
 *     object->idle_id = g_idle_add(idle_cb, object);
 *   }
 *
 *   static void my_object_finalize (GObject *object)
 *   {
 *     g_source_remove(MY_OBJECT(object)->idle_id);
 *     G_OBJECT_CLASS(parent_class)->finalize(object);
 *   }
 *
 * Timeline of events:
 *
 *   1) in thread x: we grab the GDK lock
 *   2) in the main_thread: idle_cb() is executed and waits to acquire
 *      the lock
 *   3) in thread x: we do something which causes the object instance
 *      to be finalized, and we release the lock
 *   4) in the main thread: idle_cb() acquires the lock and crashes
 *      because the object instance has been destroyed while it was
 *      waiting for the lock
 *
 * This code is therefore unsafe if MyObject can be destroyed from
 * another thread than the thread running the GLib main loop.
 *
 * This observation can be generalized as follows: holding a lock in a
 * callback and unregistering the callback from another thread with
 * the same lock held is not thread-safe.
 *
 * As of version 2.12, GTK+ is no longer affected by this problem
 * since the gdk_threads_add_*() family of functions has been added
 * (see http://bugzilla.gnome.org/show_bug.cgi?id=321886).
 *
 * However, most GTK+-based libraries (libgnomeui, libeel, ...) are
 * still affected. MNLockedCallback provides solutions for the
 * components used by MN (GConf, GnomeVFS).
 *
 * Note that throughout Mail Notification, we always use our
 * MNLockedCallback variants (except of course for callbacks which do
 * not need to hold a lock), even in objects which cannot be destroyed
 * from a thread and are therefore not affected by the problem. It is
 * safer and more maintainable than having to follow the code path to
 * decide whether the stock library functions can safely be used or
 * not, and the MNLockedCallback overhead is neglectible.
 */

typedef struct
{
  gboolean	removed;
  gpointer	function;
  gpointer	data;
  gpointer	handle;
} LockedCallback;

static GHashTable *vfs_monitors = NULL;
G_LOCK_DEFINE_STATIC(vfs_monitors);

static void
gconf_notification_cb (GConfClient *client,
		       unsigned int cnxn_id,
		       GConfEntry *entry,
		       gpointer user_data)
{
  LockedCallback *callback = user_data;

  GDK_THREADS_ENTER();

  if (! callback->removed)
    ((GConfClientNotifyFunc) callback->function)(client, cnxn_id, entry, callback->data);

  GDK_THREADS_LEAVE();
}

static void
gconf_notification_weak_notify_cb (gpointer data, GObject *former_object)
{
  LockedCallback *callback = data;

  callback->removed = TRUE;
  mn_conf_notification_remove(GPOINTER_TO_UINT(callback->handle));
}

static void
locked_callback_free (LockedCallback *callback)
{
  g_return_if_fail(callback != NULL);

  g_free(callback);
}

/**
 * mn_g_object_gconf_notification_add_gdk_locked:
 * @object: a GObject-derived instance
 * @key: a GConf key or namespace section
 * @function: function to call when changes occur
 * @user_data: data to pass to @function
 *
 * Monitors the GConf key or namespace @key for changes. The @function
 * invocation will be wrapped with GDK_THREADS_ENTER() and
 * GDK_THREADS_LEAVE().  The notification will be removed when @object
 * is finalized. If after acquiring the GDK lock, the object has been
 * removed, @function will not be executed.
 **/
void
mn_g_object_gconf_notification_add_gdk_locked (gpointer object,
					       const char *key,
					       GConfClientNotifyFunc function,
					       gpointer user_data)
{
  LockedCallback *callback;

  g_return_if_fail(G_IS_OBJECT(object));
  g_return_if_fail(key != NULL);
  g_return_if_fail(function != NULL);

  callback = g_new0(LockedCallback, 1);
  callback->function = function;
  callback->data = user_data;
  callback->handle = GUINT_TO_POINTER(mn_conf_notification_add(key, gconf_notification_cb, callback, (GDestroyNotify) locked_callback_free));

  g_object_weak_ref(G_OBJECT(object), gconf_notification_weak_notify_cb, callback);
}

/**
 * mn_g_object_gconf_notifications_add_gdk_locked:
 * @object: a GObject-derived instance
 * @...: a %NULL-terminated list of key/function/user_data tuples
 *
 * Adds multiple GConf notifications with
 * mn_g_object_gconf_notification_add_gdk_locked().
 **/
void
mn_g_object_gconf_notifications_add_gdk_locked (gpointer object, ...)
{
  va_list args;
  const char *key;

  g_return_if_fail(G_IS_OBJECT(object));

  va_start(args, object);

  while ((key = va_arg(args, const char *)))
    {
      GConfClientNotifyFunc function;
      gpointer user_data;

      function = va_arg(args, GConfClientNotifyFunc);
      g_return_if_fail(function != NULL);

      user_data = va_arg(args, gpointer);

      mn_g_object_gconf_notification_add_gdk_locked(object, key, function, user_data);
    }

  va_end(args);
}

static void
vfs_monitor_cb (GnomeVFSMonitorHandle *handle,
		const char *monitor_uri,
		const char *info_uri,
		GnomeVFSMonitorEventType event_type,
		gpointer user_data)
{
  GnomeVFSMonitorCallback callback;

  G_LOCK(vfs_monitors);
  callback = g_hash_table_lookup(vfs_monitors, handle);
  if (callback)
    callback(handle, monitor_uri, info_uri, event_type, user_data);
  G_UNLOCK(vfs_monitors);
}

/**
 * mn_gnome_vfs_monitor_add_locked:
 * @handle: a location to return the monitor handle on success
 * @text_uri: URI to monitor
 * @monitor_type: monitor type
 * @callback: function to call
 * @user_data: data to pass to @function
 *
 * Monitors @text_uri and invokes @callback when a change occurs.
 * Callback invocations will be protected by an internal global lock,
 * and will not occur if the monitor has been removed.
 *
 * Return value: the status of the operation
 **/
GnomeVFSResult
mn_gnome_vfs_monitor_add_locked (GnomeVFSMonitorHandle **handle,
				 const char *text_uri,
				 GnomeVFSMonitorType monitor_type,
				 GnomeVFSMonitorCallback callback,
				 gpointer user_data)
{
  GnomeVFSMonitorHandle *_handle;
  GnomeVFSResult result;

  g_return_val_if_fail(handle != NULL, GNOME_VFS_ERROR_BAD_PARAMETERS);
  g_return_val_if_fail(text_uri != NULL, GNOME_VFS_ERROR_BAD_PARAMETERS);
  g_return_val_if_fail(callback != NULL, GNOME_VFS_ERROR_BAD_PARAMETERS);

  /*
   * We need a global monitor hash table because
   * gnome_vfs_monitor_add() has no destroy_data parameter.
   */

  G_LOCK(vfs_monitors);
  result = gnome_vfs_monitor_add(&_handle, text_uri, monitor_type, vfs_monitor_cb, user_data);
  if (result == GNOME_VFS_OK)
    {
      *handle = _handle;

      if (! vfs_monitors)
	vfs_monitors = g_hash_table_new(g_direct_hash, g_direct_equal);

      g_hash_table_insert(vfs_monitors, _handle, callback);
    }
  G_UNLOCK(vfs_monitors);

  return result;
}

/**
 * mn_gnome_vfs_monitor_cancel_locked:
 * @handle: handle of the monitor to cancel
 *
 * Cancels the monitor pointed to by @handle (which must have been
 * returned by a call to mn_gnome_vfs_monitor_add_locked()), using a
 * global lock to make sure that its callback function will no longer
 * be executed.
 *
 * Return value: the status of the operation
 **/
GnomeVFSResult
mn_gnome_vfs_monitor_cancel_locked (GnomeVFSMonitorHandle *handle)
{
  GnomeVFSResult result;

  g_return_val_if_fail(handle != NULL, GNOME_VFS_ERROR_BAD_PARAMETERS);

  G_LOCK(vfs_monitors);
  g_assert(g_hash_table_lookup(vfs_monitors, handle) != NULL);
  g_hash_table_remove(vfs_monitors, handle);
  result = gnome_vfs_monitor_cancel(handle);
  G_UNLOCK(vfs_monitors);

  return result;
}
