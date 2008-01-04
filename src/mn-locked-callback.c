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
#include <eel/eel.h>
#include "mn-locked-callback.h"
#include "mn-conf.h"
#include "mn-util.h"

/*
 * About MNLockedCallback:
 *
 * Consider the following construct:
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
 * The construct is therefore unsafe if MyObject can be destroyed from
 * another thread than the thread running the GLib main
 * loop. MNLockedCallback provides a solution.
 *
 * GTK+ and most GTK+-based libraries (libgnomeui, libeel, ...) are
 * also affected by the problem. Bugzilla entry for GTK+:
 *
 *   http://bugzilla.gnome.org/show_bug.cgi?id=321886
 *
 * Note that throughout Mail Notification, we always use our
 * _gdk_locked variants (except of course for callbacks which do not
 * need to hold the GDK lock), even in objects which cannot be
 * destroyed from a thread and are therefore not affected by the
 * problem. It is safer and more maintainable than having to follow
 * the code path to decide whether the GLib functions can safely be
 * used or not, and the _gdk_locked overhead is neglectible.
 */

/*** types *******************************************************************/

typedef enum
{
  CALLBACK_TYPE_G_SOURCE,
  CALLBACK_TYPE_GCONF_NOTIFICATION,
  CALLBACK_TYPE_BONOBO_LISTENER
} CallbackType;

typedef struct _MNLockedCallback MNLockedCallback;

struct _MNLockedCallback
{
  CallbackType	type;
  gboolean	removed;
  gpointer	function;
  gpointer	data;
  MNLockFunc	lock;
  MNUnlockFunc	unlock;
  gpointer	lock_data;
  gpointer	handle;
};

/*** variables ***************************************************************/

static GHashTable *vfs_monitors = NULL;
G_LOCK_DEFINE_STATIC(vfs_monitors);

/*** functions ***************************************************************/

static gboolean mn_locked_g_source_cb (gpointer data);

static void mn_gdk_locked_g_object_gconf_notification_cb (GConfClient *client,
							  unsigned int cnxn_id,
							  GConfEntry *entry,
							  gpointer user_data);
static void mn_gdk_locked_g_object_gconf_notification_weak_notify_cb (gpointer data,
								      GObject *former_object);

static void mn_locked_callback_free (MNLockedCallback *callback);

static void mn_locked_gnome_vfs_monitor_cb (GnomeVFSMonitorHandle *handle,
					    const char *monitor_uri,
					    const char *info_uri,
					    GnomeVFSMonitorEventType event_type,
					    gpointer user_data);

static void mn_locked_bonobo_listener_cb (BonoboListener *listener,
					  const char *event_name,
					  const CORBA_any *any,
					  CORBA_Environment *ev,
					  gpointer user_data);

/*** implementation **********************************************************/

void
mn_locked_callback_init (void)
{
  vfs_monitors = g_hash_table_new(g_direct_hash, g_direct_equal);
}

/**
 * mn_g_idle_add_locked:
 * @function: function to call
 * @data: data to pass to @function
 * @lock_func: lock function
 * @unlock_func: unlock function
 * @lock_data: data to pass to @lock_func and @unlock_func
 *
 * Adds a locked idle callback. The @function invocation will be
 * wrapped with @lock_func and @unlock_func invocations. If after
 * invoking @lock_func, the source has been removed, @function will
 * not be executed.
 *
 * Return value: an opaque event source handle
 **/
MNLockedGSource *
mn_g_idle_add_locked (GSourceFunc function,
		      gpointer data,
		      MNLockFunc lock_func,
		      MNUnlockFunc unlock_func,
		      gpointer lock_data)
{
  MNLockedCallback *callback;

  g_return_val_if_fail(function != NULL, NULL);
  g_return_val_if_fail(lock_func != NULL, NULL);
  g_return_val_if_fail(unlock_func != NULL, NULL);

  callback = g_new0(MNLockedCallback, 1);
  callback->type = CALLBACK_TYPE_G_SOURCE;
  callback->function = function;
  callback->data = data;
  callback->lock = lock_func;
  callback->unlock = unlock_func;
  callback->lock_data = lock_data;
  callback->handle = GUINT_TO_POINTER(g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, mn_locked_g_source_cb, callback, (GDestroyNotify) mn_locked_callback_free));

  return callback;
}

/**
 * mn_g_idle_add_gdk_locked:
 * @function: function to call
 * @data: data to pass to @function
 *
 * Adds a GDK-locked idle callback. The @function invocation will be
 * wrapped with GDK_THREADS_ENTER() and GDK_THREADS_LEAVE(). If after
 * acquiring the GDK lock, the source has been removed, @function will
 * not be executed.
 *
 * Return value: an opaque event source handle
 **/
MNLockedGSource *
mn_g_idle_add_gdk_locked (GSourceFunc function, gpointer data)
{
  g_return_val_if_fail(function != NULL, NULL);

  return mn_g_idle_add_locked(function, data,
			      (MNLockFunc) mn_gdk_threads_enter,
			      (MNUnlockFunc) mn_gdk_threads_leave,
			      NULL);
}

/**
 * mn_g_timeout_add_locked:
 * @interval: the time between calls to the function, in milliseconds
 * @function: function to call
 * @data: data to pass to @function
 * @lock_func: lock function
 * @unlock_func: unlock function
 * @lock_data: data to pass to @lock_func and @unlock_func
 *
 * Adds a locked timeout callback. The @function invocation will be
 * wrapped with @lock_func and @unlock_func invocations. If after
 * invoking @lock_func, the source has been removed, @function will
 * not be executed.
 *
 * Return value: an opaque event source handle
 **/
MNLockedGSource *
mn_g_timeout_add_locked (unsigned int interval,
			 GSourceFunc function,
			 gpointer data,
			 MNLockFunc lock_func,
			 MNUnlockFunc unlock_func,
			 gpointer lock_data)
{
  MNLockedCallback *callback;

  g_return_val_if_fail(function != NULL, NULL);
  g_return_val_if_fail(lock_func != NULL, NULL);
  g_return_val_if_fail(unlock_func != NULL, NULL);

  callback = g_new0(MNLockedCallback, 1);
  callback->type = CALLBACK_TYPE_G_SOURCE;
  callback->function = function;
  callback->data = data;
  callback->lock = lock_func;
  callback->unlock = unlock_func;
  callback->lock_data = lock_data;
  callback->handle = GUINT_TO_POINTER(g_timeout_add_full(G_PRIORITY_DEFAULT, interval, mn_locked_g_source_cb, callback, (GDestroyNotify) mn_locked_callback_free));

  return callback;
}

/**
 * mn_g_timeout_add_gdk_locked:
 * @interval: the time between calls to the function, in milliseconds
 * @function: function to call
 * @data: data to pass to @function
 *
 * Adds a GDK-locked timeout callback. The @function invocation will
 * be wrapped with GDK_THREADS_ENTER() and GDK_THREADS_LEAVE(). If
 * after acquiring the GDK lock, the source has been removed,
 * @function will not be executed.
 *
 * Return value: an opaque event source handle
 **/
MNLockedGSource *
mn_g_timeout_add_gdk_locked (unsigned int interval,
			     GSourceFunc function,
			     gpointer data)
{
  g_return_val_if_fail(function != NULL, NULL);

  return mn_g_timeout_add_locked(interval, function, data,
				 (MNLockFunc) mn_gdk_threads_enter,
				 (MNUnlockFunc) mn_gdk_threads_leave,
				 NULL);
}

static gboolean
mn_locked_g_source_cb (gpointer data)
{
  MNLockedCallback *callback = data;
  gboolean result;

  callback->lock(callback->lock_data);

  result = callback->removed
    ? FALSE			/* the returned value does not matter */
    : ((GSourceFunc) callback->function)(callback->data);

  callback->unlock(callback->lock_data);

  return result;
}

/**
 * mn_locked_g_source_remove:
 * @source: an event source handle
 *
 * Removes @source from the default main context. The callback
 * function will no longer be executed. The source lock must be held
 * while calling this function.
 **/
void
mn_locked_g_source_remove (MNLockedGSource *source)
{
  g_return_if_fail(source != NULL);
  g_return_if_fail(source->type == CALLBACK_TYPE_G_SOURCE);
  g_return_if_fail(source->removed == FALSE);

  source->removed = TRUE;
  g_source_remove(GPOINTER_TO_UINT(source->handle));
}

/**
 * mn_locked_g_source_clear:
 * @source: a pointer to an event source handle
 *
 * If *@source is not %NULL, removes it using
 * mn_locked_g_source_remove(), and sets it to %NULL.
 **/
void
mn_locked_g_source_clear (MNLockedGSource **source)
{
  g_return_if_fail(source != NULL);

  if (*source)
    {
      mn_locked_g_source_remove(*source);
      *source = NULL;
    }
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
  MNLockedCallback *callback;

  g_return_if_fail(G_IS_OBJECT(object));
  g_return_if_fail(key != NULL);
  g_return_if_fail(function != NULL);

  callback = g_new0(MNLockedCallback, 1);
  callback->type = CALLBACK_TYPE_GCONF_NOTIFICATION;
  callback->function = function;
  callback->data = user_data;
  callback->handle = GUINT_TO_POINTER(mn_conf_notification_add_full(key, mn_gdk_locked_g_object_gconf_notification_cb, callback, (GDestroyNotify) mn_locked_callback_free));

  g_object_weak_ref(G_OBJECT(object), mn_gdk_locked_g_object_gconf_notification_weak_notify_cb, callback);
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
mn_gdk_locked_g_object_gconf_notification_cb (GConfClient *client,
					      unsigned int cnxn_id,
					      GConfEntry *entry,
					      gpointer user_data)
{
  MNLockedCallback *callback = user_data;

  GDK_THREADS_ENTER();

  if (! callback->removed)
    ((GConfClientNotifyFunc) callback->function)(client, cnxn_id, entry, callback->data);

  GDK_THREADS_LEAVE();
}

static void
mn_gdk_locked_g_object_gconf_notification_weak_notify_cb (gpointer data, GObject *former_object)
{
  MNLockedCallback *callback = data;

  callback->removed = TRUE;
  eel_gconf_notification_remove(GPOINTER_TO_UINT(callback->handle));
}

static void
mn_locked_callback_free (MNLockedCallback *callback)
{
  g_return_if_fail(callback != NULL);

  g_free(callback);
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
  result = gnome_vfs_monitor_add(&_handle, text_uri, monitor_type, mn_locked_gnome_vfs_monitor_cb, user_data);
  if (result == GNOME_VFS_OK)
    {
      *handle = _handle;
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

static void
mn_locked_gnome_vfs_monitor_cb (GnomeVFSMonitorHandle *handle,
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
 * mn_bonobo_event_source_client_add_listener_full_locked:
 * @object: an object implementing IDL:Bonobo/EventSource:1.0
 * @function: function to call
 * @opt_mask: optional event mask
 * @opt_ev: optional CORBA environment
 * @user_data: data to pass to @function
 * @lock_func: lock function
 * @unlock_func: unlock function
 * @lock_data: data to pass to @lock_func and @unlock_func
 *
 * Adds a listener to the event source of @object. The @function
 * invocation will be wrapped with @lock_func and @unlock_func
 * invocations. If after invoking @lock_func, the listener has been
 * removed, @function will not be executed.
 *
 * Return value: an opaque listener handle
 **/
MNLockedBonoboListener *
mn_bonobo_event_source_client_add_listener_full_locked (Bonobo_Unknown object,
							BonoboListenerCallbackFn function,
							const char *opt_mask,
							CORBA_Environment *opt_ev,
							gpointer user_data,
							MNLockFunc lock_func,
							MNUnlockFunc unlock_func,
							gpointer lock_data)
{
  MNLockedCallback *callback;

  g_return_val_if_fail(object != CORBA_OBJECT_NIL, NULL);
  g_return_val_if_fail(function != NULL, NULL);
  g_return_val_if_fail(lock_func != NULL, NULL);
  g_return_val_if_fail(unlock_func != NULL, NULL);

  callback = g_new0(MNLockedCallback, 1);
  callback->type = CALLBACK_TYPE_BONOBO_LISTENER;
  callback->function = function;
  callback->data = user_data;
  callback->lock = lock_func;
  callback->unlock = unlock_func;
  callback->lock_data = lock_data;
  callback->handle = bonobo_event_source_client_add_listener_full(object, g_cclosure_new(G_CALLBACK(mn_locked_bonobo_listener_cb), callback, (GClosureNotify) mn_locked_callback_free), opt_mask, opt_ev);

  if (callback->handle == CORBA_OBJECT_NIL)
    {
      mn_locked_callback_free(callback);
      callback = NULL;
    }

  return callback;
}

static void
mn_locked_bonobo_listener_cb (BonoboListener *listener,
			      const char *event_name,
			      const CORBA_any *any,
			      CORBA_Environment *ev,
			      gpointer user_data)
{
  MNLockedCallback *callback = user_data;

  callback->lock(callback->lock_data);

  if (! callback->removed)
    ((BonoboListenerCallbackFn) callback->function)(listener, event_name, any, ev, callback->data);

  callback->unlock(callback->lock_data);
}

/**
 * mn_bonobo_event_source_client_remove_listener_locked:
 * @object: the object holding @listener, or %CORBA_OBJECT_NIL
 * @listener: a listener handle
 * @opt_ev: optional CORBA environment
 *
 * Removes @listener from the @object event source (if @object is not
 * %CORBA_OBJECT_NIL), then destroys @listener. The callback function
 * will no longer be executed. If @object is not %CORBA_OBJECT_NIL,
 * then the listener lock must be held while calling this function.
 **/
void
mn_bonobo_event_source_client_remove_listener_locked (Bonobo_Unknown object,
						      MNLockedBonoboListener *listener,
						      CORBA_Environment *opt_ev)
{
  Bonobo_Listener handle;

  g_return_if_fail(listener != NULL);
  g_return_if_fail(listener->type == CALLBACK_TYPE_BONOBO_LISTENER);
  g_return_if_fail(listener->removed == FALSE);

  handle = listener->handle;

  if (object != CORBA_OBJECT_NIL)
    {
      listener->removed = TRUE;
      bonobo_event_source_client_remove_listener(object, handle, opt_ev);
      /* listener has been freed in the closure destroy callback */
    }

  bonobo_object_release_unref(handle, NULL);
}
