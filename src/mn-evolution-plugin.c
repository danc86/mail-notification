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

#include <stdarg.h>
#include <string.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib-bindings.h>
#include <camel/camel-folder.h>
#include <mail/em-event.h>
#include <mail/mail-tools.h>
#include "mn-evolution.h"
#include "mn-evolution-server.h"
#include "mn-evolution-plugin.h"

#define ENABLE_SUCCESS	0
#define ENABLE_FAILURE	1

static MNEvolutionServer *evo_server = NULL;
static DBusGConnection *session_bus = NULL;
static DBusGProxy *session_bus_proxy = NULL;

static void show_error_dialog (const char *primary, const char *format, ...) G_GNUC_PRINTF(2, 3);

static void
show_error_dialog (const char *primary, const char *format, ...)
{
  GtkWidget *dialog;
  va_list args;
  char *secondary;

  g_return_if_fail(primary != NULL);
  g_return_if_fail(format != NULL);

  dialog = gtk_message_dialog_new(NULL,
				  0,
				  GTK_MESSAGE_ERROR,
				  GTK_BUTTONS_OK,
				  "%s",
				  primary);

  va_start(args, format);
  secondary = g_strdup_vprintf(format, args);
  va_end(args);

  gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", secondary);
  g_free(secondary);

  gtk_window_set_title(GTK_WINDOW(dialog), ""); /* HIG */

  g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
  gtk_widget_show(dialog);
}

gboolean
mn_evolution_plugin_register_server (GObject *server,
				     const char *service,
				     const char *path,
				     GError **err)
{
  unsigned int name_reply;

  g_return_val_if_fail(G_IS_OBJECT(server), FALSE);
  g_return_val_if_fail(service != NULL, FALSE);
  g_return_val_if_fail(path != NULL, FALSE);

  dbus_g_connection_register_g_object(session_bus, path, server);

  if (! org_freedesktop_DBus_request_name(session_bus_proxy,
					  service,
					  DBUS_NAME_FLAG_DO_NOT_QUEUE,
					  &name_reply,
					  err))
    return FALSE;

  if (name_reply != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
    {
      /* unlikely to ever happen, not worth a translation */
      g_set_error(err, 0, 0, "cannot register name \"%s\"", service);
      return FALSE;
    }

  return TRUE;
}

gboolean
mn_evolution_plugin_unregister_server (const char *service, GError **err)
{
  unsigned int name_reply;

  g_return_val_if_fail(service != NULL, FALSE);

  if (! org_freedesktop_DBus_release_name(session_bus_proxy,
					  service,
					  &name_reply,
					  err))
    return FALSE;

  if (name_reply != DBUS_RELEASE_NAME_REPLY_RELEASED)
    {
      /* unlikely to ever happen, not worth a translation */
      g_set_error(err, 0, 0, "cannot unregister name \"%s\"", service);
      return FALSE;
    }

  return TRUE;
}

static void
disable_plugin (void)
{
  g_object_unref(evo_server);
  evo_server = NULL;

  /*
   * Do not unref session_bus_proxy, since it might break when the
   * DBusGProxy memory management issue is fixed
   * (https://bugs.freedesktop.org/show_bug.cgi?id=14030).
   */
  session_bus_proxy = NULL;

  dbus_g_connection_unref(session_bus);
  session_bus = NULL;
}

static DBusHandlerResult
session_bus_filter_cb (DBusConnection *conn, DBusMessage *message, void *user_data)
{
  if (dbus_message_is_signal(message, DBUS_INTERFACE_LOCAL, "Disconnected"))
    {
      GDK_THREADS_ENTER();

      show_error_dialog(dgettext(GETTEXT_PACKAGE, "A fatal error has occurred in the Evolution Mail Notification plugin"),
			dgettext(GETTEXT_PACKAGE, "The connection to the D-Bus session bus was lost."));

      disable_plugin();

      GDK_THREADS_LEAVE();

      return DBUS_HANDLER_RESULT_HANDLED;
    }

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static gboolean
connect_to_session_bus (void)
{
  DBusConnection *raw_bus;
  GError *err = NULL;

  g_return_val_if_fail(session_bus == NULL, FALSE);

  session_bus = dbus_g_bus_get(DBUS_BUS_SESSION, &err);
  if (! session_bus)
    {
      show_error_dialog(dgettext(GETTEXT_PACKAGE, "Unable to initialize the Mail Notification plugin"),
			dgettext(GETTEXT_PACKAGE, "Unable to connect to the D-Bus session bus: %s."),
			err->message);
      g_error_free(err);
      return FALSE;
    }

  raw_bus = dbus_g_connection_get_connection(session_bus);

  dbus_connection_set_exit_on_disconnect(raw_bus, FALSE);

  if (! dbus_connection_add_filter(raw_bus, session_bus_filter_cb, NULL, NULL))
    {
      show_error_dialog(dgettext(GETTEXT_PACKAGE, "Unable to initialize the Mail Notification plugin"),
			/* too unlikely to be worth a translation */
			"Unable to add a D-Bus filter: not enough memory.");

      dbus_g_connection_unref(session_bus);
      session_bus = NULL;

      return FALSE;
    }

  session_bus_proxy = dbus_g_proxy_new_for_name(session_bus,
						DBUS_SERVICE_DBUS,
						DBUS_PATH_DBUS,
						DBUS_INTERFACE_DBUS);

  return TRUE;
}

int
e_plugin_lib_enable (EPluginLib *ep, int enable)
{
  static gboolean enabled = FALSE;
  GError *err = NULL;

  if (! enable || enabled)
    return ENABLE_SUCCESS;

  enabled = TRUE;

  if (! connect_to_session_bus())
    return ENABLE_FAILURE;

  evo_server = mn_evolution_server_new();
  if (! mn_evolution_plugin_register_server(G_OBJECT(evo_server),
					    MN_EVOLUTION_SERVER_SERVICE,
					    MN_EVOLUTION_SERVER_PATH,
					    &err))
    {
      show_error_dialog(dgettext(GETTEXT_PACKAGE, "Unable to initialize the Mail Notification plugin"),
			dgettext(GETTEXT_PACKAGE, "Unable to register the Mail Notification Evolution D-Bus server: %s."),
			err->message);
      g_error_free(err);

      disable_plugin();
      return ENABLE_FAILURE;
    }

  return ENABLE_SUCCESS;
}

void
org_jylefort_mail_notification_folder_changed (EPlugin *plugin,
					       EMEventTargetFolder *folder)
{
  if (evo_server)
    mn_evolution_server_folder_changed(evo_server, folder->uri);
}

void
org_jylefort_mail_notification_message_reading (EPlugin *plugin,
						EMEventTargetMessage *message)
{
  if (evo_server)
    {
      char *url;

      url = mail_tools_folder_to_url(message->folder);
      mn_evolution_server_message_reading(evo_server, url);
      g_free(url);
    }
}
