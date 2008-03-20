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

#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <glib/gi18n.h>
#include <gnome.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libnotify/notify.h>
#include <dbus/dbus-glib-lowlevel.h>
#if WITH_GMIME
#include <gmime/gmime.h>
#include "mn-gmime-stream-vfs.h"
#endif
#if WITH_MBOX || WITH_MOZILLA || WITH_MH || WITH_MAILDIR || WITH_SYLPHEED
#include "mn-vfs-mailbox.h"
#endif
#if WITH_MAILDIR
#include "mn-maildir-message.h"
#endif
#if WITH_SYLPHEED
#include "mn-sylpheed-message.h"
#endif
#if WITH_EVOLUTION
#include "mn-evolution-message.h"
#endif
#include "mn-conf.h"
#include "mn-util.h"
#include "mn-stock.h"
#include "mn-shell.h"
#include "mn-message.h"
#include "mn-client-dbus.h"
#include "mn-server.h"

#define CALL_SERVER(call)		\
  {					\
    if (! (call))			\
      handle_server_call_error(err);	\
  }

typedef struct
{
  const char	*name;
  gboolean	enabled;
} Component;

static gboolean arg_enable_info = FALSE;

static void
print_components (const Component *components, int n_components)
{
  int i;

  for (i = 0; i < n_components; i++)
    g_print("  %-30s %s\n", components[i].name, components[i].enabled ? _("yes") : _("no"));
}

static void
print_version (void)
{
  /*
   * Here and everywhere else, backends and features are sorted
   * alphabetically.
   */

  static const Component mailbox_backends[] = {
    { "Evolution",		WITH_EVOLUTION		},
    { "Gmail",			WITH_GMAIL		},
    { "IMAP",			WITH_IMAP		},
    { "Maildir",		WITH_MAILDIR		},
    { "mbox",			WITH_MBOX		},
    { "MH",			WITH_MH			},
    { "Mozilla products",	WITH_MOZILLA		},
    { "POP3",			WITH_POP3		},
    { "Sylpheed",		WITH_SYLPHEED		},
    { "Windows Live Hotmail",	WITH_HOTMAIL		},
    { "Yahoo! Mail",		WITH_YAHOO		}
  };

  static const Component pi_features[] = {
    { "IPv6",			WITH_IPV6		},
    { "SASL",			WITH_SASL		},
    { "SSL/TLS",		WITH_SSL		}
  };

  g_print(_("%s version %s\n"), _("Mail Notification"), VERSION);
  g_print("Copyright (C) 2003-2008 Jean-Yves Lefort.\n");

  g_print("\n");

  g_print(_("Mailbox backends:\n"));
  print_components(mailbox_backends, G_N_ELEMENTS(mailbox_backends));

  g_print("\n");

  g_print(_("IMAP and POP3 features:\n"));
  print_components(pi_features, G_N_ELEMENTS(pi_features));
}

static void
info_log_cb (const char *log_domain,
	     GLogLevelFlags log_level,
	     const char *message,
	     gpointer user_data)
{
  if (arg_enable_info)
    g_log_default_handler(log_domain, log_level, message, user_data);
}

static void
init_classes (void)
{
  int i;

#if WITH_GMIME
  g_type_class_ref(MN_TYPE_GMIME_STREAM_VFS);
  g_type_class_ref(GMIME_TYPE_PARSER);
  g_type_class_ref(GMIME_TYPE_STREAM_MEM);
  g_type_class_ref(GMIME_TYPE_MESSAGE);
#endif /* WITH_GMIME */
#if WITH_MBOX || WITH_MOZILLA || WITH_MH || WITH_MAILDIR || WITH_SYLPHEED
  for (i = 0; mn_vfs_mailbox_backend_types[i]; i++)
    g_type_class_ref(mn_vfs_mailbox_backend_types[i]);
#endif
#if WITH_MAILDIR
  g_type_class_ref(MN_TYPE_MAILDIR_MESSAGE);
#endif
#if WITH_SYLPHEED
  g_type_class_ref(MN_TYPE_SYLPHEED_MESSAGE);
#endif
  g_type_class_ref(MN_TYPE_MESSAGE);
}

#if WITH_GMIME
static void
init_gmime (void)
{
  GPtrArray *array;
  GSList *fallback_charsets;
  GSList *l;

  g_mime_init(0);

  array = g_ptr_array_new();

  fallback_charsets = mn_conf_get_string_list(MN_CONF_FALLBACK_CHARSETS);

  MN_LIST_FOREACH(l, fallback_charsets)
    {
      const char *charset = l->data;

      if (! strcmp(charset, "user"))
	{
	  const char *user_charset;

	  g_get_charset(&user_charset);
	  g_ptr_array_add(array, g_strdup(user_charset));
	}
      else
	g_ptr_array_add(array, g_strdup(charset));
    }

  mn_g_slist_free_deep(fallback_charsets);

  g_ptr_array_add(array, NULL);	/* canary */

  g_mime_set_user_charsets((const char **) array->pdata);

  /*
   * Note that because of
   * http://bugzilla.gnome.org/show_bug.cgi?id=509434, we do not free
   * the strings of the array (since GMime does not copy them).
   */
  g_ptr_array_free(array, TRUE);
}
#endif /* WITH_GMIME */

static void
handle_server_call_error (GError *err)
{
  g_return_if_fail(err != NULL);

  mn_show_fatal_error_dialog(NULL, _("Unable to contact the running Mail Notification instance: %s."), err->message);
}

static gboolean
has_icon_path (const char *path)
{
  char **paths;
  int i;
  gboolean has = FALSE;

  gtk_icon_theme_get_search_path(gtk_icon_theme_get_default(), &paths, NULL);
  for (i = 0; paths[i]; i++)
    if (! strcmp(paths[i], path))
      {
	has = TRUE;
	break;
      }
  g_strfreev(paths);

  return has;
}

/*
 * This is needed when MN is not installed in the standard prefix (as
 * is the case for my test builds).
 */
static void
ensure_icon_path (void)
{
  static const char *icon_path = DATADIR G_DIR_SEPARATOR_S "icons";

  if (! has_icon_path(icon_path))
    gtk_icon_theme_prepend_search_path(gtk_icon_theme_get_default(), icon_path);
}

static void
report_option_ignored (const char *option_name)
{
  g_return_if_fail(option_name != NULL);

  g_message(_("%s option ignored since Mail Notification is not already running"), option_name);
}

static DBusHandlerResult
session_bus_filter_cb (DBusConnection *conn, DBusMessage *message, void *user_data)
{
  if (dbus_message_is_signal(message, DBUS_INTERFACE_LOCAL, "Disconnected"))
    {
      GDK_THREADS_ENTER();

      mn_show_fatal_error_dialog(NULL, _("The connection to the D-Bus session bus was lost."));

      GDK_THREADS_LEAVE();

      return DBUS_HANDLER_RESULT_HANDLED;
    }

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static DBusGConnection *
connect_to_session_bus (void)
{
  GError *err = NULL;
  DBusGConnection *bus;
  DBusConnection *raw_bus;

  bus = dbus_g_bus_get(DBUS_BUS_SESSION, &err);
  if (! bus)
    {
      mn_show_fatal_error_dialog(NULL, _("Unable to connect to the D-Bus session bus: %s."), err->message);
      g_error_free(err);
    }

  raw_bus = dbus_g_connection_get_connection(bus);

  dbus_connection_set_exit_on_disconnect(raw_bus, FALSE);

  if (! dbus_connection_add_filter(raw_bus, session_bus_filter_cb, NULL, NULL))
    /* too unlikely to be worth a translation */
    mn_show_fatal_error_dialog(NULL, "Unable to add a D-Bus filter: not enough memory.");

  return bus;
}

/*
 * A memory management bug in DBusGProxy
 * (https://bugs.freedesktop.org/show_bug.cgi?id=14030) prevents us
 * from unreferencing the proxy, so provide an eternal singleton
 * proxy.
 */
static DBusGProxy *
get_bus_proxy (DBusGConnection *bus)
{
  DBusGProxy *proxy;

  g_return_val_if_fail(bus != NULL, NULL);

  proxy = dbus_g_proxy_new_for_name(bus,
				    DBUS_SERVICE_DBUS,
				    DBUS_PATH_DBUS,
				    DBUS_INTERFACE_DBUS);

#if WITH_EVOLUTION
  /* needed by MNEvolutionClient */
  dbus_g_proxy_add_signal(proxy,
			  "NameOwnerChanged",
			  G_TYPE_STRING, /* service_name */
			  G_TYPE_STRING, /* old_owner */
			  G_TYPE_STRING, /* new_owner */
			  G_TYPE_INVALID);
#endif

  return proxy;
}

int
main (int argc, char **argv)
{
  gboolean arg_version = FALSE;
  gboolean arg_display_properties = FALSE;
  gboolean arg_display_about = FALSE;
  gboolean arg_consider_new_mail_as_read = FALSE;
  gboolean arg_update = FALSE;
  gboolean arg_print_summary = FALSE;
  gboolean arg_unset_obsolete_configuration = FALSE;
  gboolean arg_quit = FALSE;
  const GOptionEntry options[] = {
    {
      "version",
      'v',
      0,
      G_OPTION_ARG_NONE,
      &arg_version,
      N_("Show version information"),
      NULL
    },
    {
      "enable-info",
      'i',
      0,
      G_OPTION_ARG_NONE,
      &arg_enable_info,
      N_("Enable informational output"),
      NULL
    },
    {
      "display-properties",
      'p',
      0,
      G_OPTION_ARG_NONE,
      &arg_display_properties,
      N_("Display the properties dialog"),
      NULL
    },
    {
      "display-about",
      'a',
      0,
      G_OPTION_ARG_NONE,
      &arg_display_about,
      N_("Display the about dialog"),
      NULL
    },
    {
      "consider-new-mail-as-read",
      'r',
      0,
      G_OPTION_ARG_NONE,
      &arg_consider_new_mail_as_read,
      N_("Consider new mail as read"),
      NULL
    },
    {
      "update",
      'u',
      0,
      G_OPTION_ARG_NONE,
      &arg_update,
      N_("Update the mail status"),
      NULL
    },
    {
      "print-summary",
      's',
      0,
      G_OPTION_ARG_NONE,
      &arg_print_summary,
      N_("Print a XML mail summary"),
      NULL
    },
    {
      "unset-obsolete-configuration",
      '\0',
      0,
      G_OPTION_ARG_NONE,
      &arg_unset_obsolete_configuration,
      N_("Unset obsolete GConf configuration"),
      NULL
    },
    {
      "quit",
      'q',
      0,
      G_OPTION_ARG_NONE,
      &arg_quit,
      N_("Quit Mail Notification"),
      NULL
    },
    { NULL }
  };
  GOptionContext *option_context;
  DBusGConnection *bus;
  DBusGProxy *bus_proxy;

  g_log_set_fatal_mask(NULL, G_LOG_LEVEL_CRITICAL);
  g_log_set_handler(NULL, G_LOG_LEVEL_INFO, info_log_cb, NULL);

#ifdef ENABLE_NLS
  bindtextdomain(GETTEXT_PACKAGE, GNOMELOCALEDIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);
#endif

  g_set_application_name(_("Mail Notification"));

  g_thread_init(NULL);
  if (! g_thread_supported())
    /*
     * We cannot use mn_fatal_error_dialog() because gtk_init() has
     * not been called yet.
     */
    g_critical(_("multi-threading is not available"));
  gdk_threads_init();

  GDK_THREADS_ENTER();

  option_context = g_option_context_new(NULL);
  g_option_context_add_main_entries(option_context, options, GETTEXT_PACKAGE);

  gnome_program_init(PACKAGE,
		     VERSION,
		     LIBGNOMEUI_MODULE,
		     argc,
		     argv,
		     GNOME_PARAM_HUMAN_READABLE_NAME, _("Mail Notification"),
		     GNOME_PROGRAM_STANDARD_PROPERTIES,
		     GNOME_PARAM_GOPTION_CONTEXT, option_context,
		     NULL);

  if (arg_version)
    {
      print_version();
      goto end;
    }

  if (arg_unset_obsolete_configuration)
    {
      mn_conf_unset_obsolete();
      goto end;
    }

  ensure_icon_path();
  gtk_window_set_default_icon_name("mail-notification");

  mn_stock_init();

  bus = connect_to_session_bus();
  bus_proxy = get_bus_proxy(bus);

  if (mn_server_start(bus, bus_proxy)) /* not already running */
    {
      if (arg_quit)
	g_message(_("Mail Notification is not running"));
      else
	{
	  mn_mailbox_init_types();
#if WITH_MBOX || WITH_MOZILLA || WITH_MH || WITH_MAILDIR || WITH_SYLPHEED
	  mn_vfs_mailbox_init_types();
#endif

	  /* mn-client-session uses sockets, we don't want to die on SIGPIPE */
	  signal(SIGPIPE, SIG_IGN);

	  if (! gnome_vfs_init())
	    mn_show_fatal_error_dialog(NULL, _("Unable to initialize the GnomeVFS library."));

	  gnome_authentication_manager_init();

	  /* must be called before init_gmime() */
	  mn_conf_init();

#if WITH_GMIME
	  init_gmime();
#endif

	  if (! notify_init(_("Mail Notification")))
	    mn_show_error_dialog(NULL,
				 _("An initialization error has occurred in Mail Notification"),
				 _("Unable to initialize the notification library. Message popups will not be displayed."));

	  /*
	   * Work around
	   * http://bugzilla.gnome.org/show_bug.cgi?id=64764:
	   * initialize the classes we will be using concurrently
	   * before any thread is created.
	   */
	  init_classes();

	  mn_shell_new(bus, bus_proxy);

	  /* also display the properties dialog if there are no mailboxes */
	  if (! mn_shell->mailboxes->list)
	    arg_display_properties = TRUE;

	  if (arg_display_properties)
	    mn_shell_show_properties_dialog(mn_shell, 0);
	  if (arg_display_about)
	    mn_shell_show_about_dialog(mn_shell, 0);
	  if (arg_consider_new_mail_as_read)
	    report_option_ignored("--consider-new-mail-as-read");
	  if (arg_update)
	    report_option_ignored("--update");
	  if (arg_print_summary)
	    report_option_ignored("--print-summary");

	  /* in case no window has been displayed */
	  gdk_notify_startup_complete();

	  gtk_main();
	}
    }
  else				/* already running */
    {
      DBusGProxy *proxy;
      GError *err = NULL;

      proxy = dbus_g_proxy_new_for_name(bus,
					MN_SERVER_SERVICE,
					MN_SERVER_PATH,
					MN_SERVER_INTERFACE);

      if (arg_quit)
	{
	  g_message(_("quitting Mail Notification"));
	  CALL_SERVER(org_gnome_MailNotification_quit(proxy, &err));
	}
      else
	{
	  /* also display the properties dialog if there are no mailboxes */
	  if (! arg_display_properties)
	    {
	      gboolean has;
	      CALL_SERVER(org_gnome_MailNotification_has_mailboxes(proxy, &has, &err));
	      arg_display_properties = ! has;
	    }

	  if (arg_display_properties)
	    CALL_SERVER(org_gnome_MailNotification_display_properties(proxy, &err));
	  if (arg_display_about)
	    CALL_SERVER(org_gnome_MailNotification_display_about(proxy, &err));
	  if (arg_consider_new_mail_as_read)
	    {
	      g_message(_("considering new mail as read"));
	      CALL_SERVER(org_gnome_MailNotification_consider_new_mail_as_read(proxy, &err));
	    }
	  if (arg_update)
	    {
	      g_message(_("updating the mail status"));
	      CALL_SERVER(org_gnome_MailNotification_update(proxy, &err));
	    }
	  if (arg_print_summary)
	    {
	      char *summary;

	      CALL_SERVER(org_gnome_MailNotification_get_summary(proxy, &summary, &err));
	      g_print("%s", summary);
	      g_free(summary);
	    }

	  if (! (arg_display_properties
		 || arg_display_about
		 || arg_consider_new_mail_as_read
		 || arg_update
		 || arg_print_summary))
	    g_message(_("Mail Notification is already running"));
	}

      /*
       * Do not unref the proxy, since it might break when the
       * DBusGProxy memory management issue is fixed
       * (https://bugs.freedesktop.org/show_bug.cgi?id=14030).
       */

      /* no window has been displayed by this process */
      gdk_notify_startup_complete();
    }

 end:
  GDK_THREADS_LEAVE();

  return 0;
}
