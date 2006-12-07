/*
 * Mail Notification
 * Copyright (C) 2003-2006 Jean-Yves Lefort <jylefort@brutele.be>
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
#include <stdlib.h>
#include <signal.h>
#include <gnome.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libnotify/notify.h>
#if WITH_MIME
#include <gmime/gmime.h>
#include "mn-gmime-stream-vfs.h"
#endif
#if WITH_MBOX || WITH_MOZILLA || WITH_MH || WITH_MAILDIR || WITH_SYLPHEED
#include "mn-vfs-mailbox.h"
#include "mn-vfs-message.h"
#endif
#if WITH_EVOLUTION
#include "mn-bonobo-unknown.h"
#include "mn-evolution-message.h"
#endif
#include "mn-locked-callback.h"
#include "mn-conf.h"
#include "mn-util.h"
#include "mn-stock.h"
#include "mn-automation.h"
#include "mn-shell.h"
#include "mn-message.h"

/*** cpp *********************************************************************/

#define AUTOMATION_IID			"OAFIID:GNOME_MailNotification_Automation"
#define AUTOMATION_FACTORY_IID		"OAFIID:GNOME_MailNotification_Automation_Factory"

#define AUTOMATION_METHOD(method) \
  GNOME_MailNotification_Automation_ ## method(automation, &env); \
  mn_main_handle_bonobo_exception(&env, "GNOME:MailNotification:Automation:" #method)

/*** types *******************************************************************/

typedef struct
{
  const char	*name;
  gboolean	enabled;
} Component;

/*** variables ***************************************************************/

static gboolean arg_enable_info = FALSE;

/*** functions ***************************************************************/

static BonoboObject *mn_main_automation_factory_cb (BonoboGenericFactory *factory,
						    const char *iid,
						    gpointer closure);
static void	mn_main_print_components (const Component *components,
					  int n_components);
static void	mn_main_print_version	(void);
static void	mn_main_info_log_cb	(const char	*log_domain,
					 GLogLevelFlags	log_level,
					 const char	*message,
					 gpointer	user_data);

static void	mn_main_init_classes	(void);
static void	mn_main_handle_bonobo_exception (CORBA_Environment *env,
						 const char        *method);

static gboolean	mn_main_has_icon_path		(const char *path);
static void	mn_main_ensure_icon_path	(void);

static void	mn_main_report_option_ignored	(const char *option_name);

/*** implementation **********************************************************/

static BonoboObject *
mn_main_automation_factory_cb (BonoboGenericFactory *factory,
			       const char *iid,
			       gpointer closure)
{
  if (! strcmp(iid, AUTOMATION_IID))
    return BONOBO_OBJECT(mn_automation_new());

  g_assert_not_reached();
  return NULL;
}

static void
mn_main_print_components (const Component *components, int n_components)
{
  int i;

  for (i = 0; i < n_components; i++)
    g_print("  %-30s %s\n", components[i].name, components[i].enabled ? _("yes") : _("no"));
}

static void
mn_main_print_version (void)
{
  /*
   * Here and everywhere else, we order the backends by descending
   * order of (believed) popularity.
   */

  static const Component mailbox_backends[] = {
    { "mbox",			WITH_MBOX		},
    { "mh",			WITH_MH			},
    { "Maildir",		WITH_MAILDIR		},
    { "POP3",			WITH_POP3		},
    { "IMAP",			WITH_IMAP		},
    { "Gmail",			WITH_GMAIL		},
    { "Evolution",		WITH_EVOLUTION		},
    { "Mozilla products",	WITH_MOZILLA		},
    { "Sylpheed",		WITH_SYLPHEED		}
  };

  static const Component pi_features[] = {
    { "SSL/TLS",		WITH_SSL		},
    { "SASL",			WITH_SASL		},
    { "IPv6",			WITH_IPV6		}
  };

  static const Component sylpheed_features[] = {
    { ".sylpheed_mark locking",	WITH_SYLPHEED_LOCKING	}
  };

  g_print(_("%s version %s\n"), _("Mail Notification"), VERSION);
  g_print("Copyright (C) 2003-2006 Jean-Yves Lefort.\n");

  g_print("\n");

  g_print(_("Mailbox backends:\n"));
  mn_main_print_components(mailbox_backends, G_N_ELEMENTS(mailbox_backends));

  g_print("\n");

  g_print(_("POP3 and IMAP features:\n"));
  mn_main_print_components(pi_features, G_N_ELEMENTS(pi_features));

  g_print("\n");

  g_print(_("Sylpheed features:\n"));
  mn_main_print_components(sylpheed_features, G_N_ELEMENTS(sylpheed_features));
}

static void
mn_main_info_log_cb (const char *log_domain,
		     GLogLevelFlags log_level,
		     const char *message,
		     gpointer user_data)
{
  if (arg_enable_info)
    g_log_default_handler(log_domain, log_level, message, user_data);
}

static void
mn_main_init_classes (void)
{
  int i;

#if WITH_MIME
  g_type_class_ref(MN_TYPE_GMIME_STREAM_VFS);
  g_type_class_ref(GMIME_TYPE_PARSER);
  g_type_class_ref(GMIME_TYPE_STREAM_MEM);
  g_type_class_ref(GMIME_TYPE_MESSAGE);
#endif /* WITH_MIME */
#if WITH_MBOX || WITH_MOZILLA || WITH_MH || WITH_MAILDIR || WITH_SYLPHEED
  for (i = 0; mn_vfs_mailbox_backend_types[i]; i++)
    g_type_class_ref(mn_vfs_mailbox_backend_types[i]);
  g_type_class_ref(MN_TYPE_VFS_MESSAGE);
#endif
#if WITH_EVOLUTION
  g_type_class_ref(MN_TYPE_BONOBO_UNKNOWN);
  g_type_class_ref(MN_TYPE_EVOLUTION_MESSAGE);
#endif
  g_type_class_ref(MN_TYPE_MESSAGE);
}

static void
mn_main_handle_bonobo_exception (CORBA_Environment *env, const char *method)
{
  g_return_if_fail(env != NULL);
  g_return_if_fail(method != NULL);

  if (BONOBO_EX(env))
    {
      char *errmsg;

      errmsg = bonobo_exception_get_text(env);
      mn_fatal_error_dialog(NULL, _("A Bonobo exception (%s) has occurred in %s()."), errmsg, method);
      g_free(errmsg);
    }
}

static gboolean
mn_main_has_icon_path (const char *path)
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
mn_main_ensure_icon_path (void)
{
#define icon_path DATADIR G_DIR_SEPARATOR_S "icons"
  if (! mn_main_has_icon_path(icon_path))
    gtk_icon_theme_prepend_search_path(gtk_icon_theme_get_default(), icon_path);
#undef icon_path
}

static void
mn_main_report_option_ignored (const char *option_name)
{
  g_return_if_fail(option_name != NULL);

  g_message(_("%s option ignored since Mail Notification is not already running"), option_name);
}

int
main (int argc, char **argv)
{
  gboolean arg_version = FALSE;
  gboolean arg_display_properties = FALSE;
  gboolean arg_display_about = FALSE;
  gboolean arg_update = FALSE;
  gboolean arg_print_summary = FALSE;
  gboolean arg_unset_obsolete_configuration = FALSE;
  gboolean arg_quit = FALSE;
  const struct poptOption popt_options[] = {
    {
      "version",
      'v',
      POPT_ARG_NONE,
      &arg_version,
      0,
      N_("Show version information"),
      NULL
    },
    {
      "enable-info",
      'i',
      POPT_ARG_NONE,
      &arg_enable_info,
      0,
      N_("Enable informational output"),
      NULL
    },
    {
      "display-properties",
      'p',
      POPT_ARG_NONE,
      &arg_display_properties,
      0,
      N_("Display the properties dialog"),
      NULL
    },
    {
      "display-about",
      'a',
      POPT_ARG_NONE,
      &arg_display_about,
      0,
      N_("Display the about dialog"),
      NULL
    },
    {
      "update",
      'u',
      POPT_ARG_NONE,
      &arg_update,
      0,
      N_("Update the mail status"),
      NULL
    },
    {
      "print-summary",
      's',
      POPT_ARG_NONE,
      &arg_print_summary,
      0,
      N_("Print a XML mail summary"),
      NULL
    },
    {
      "unset-obsolete-configuration",
      '\0',
      POPT_ARG_NONE,
      &arg_unset_obsolete_configuration,
      0,
      N_("Unset obsolete GConf configuration"),
      NULL
    },
    {
      "quit",
      'q',
      POPT_ARG_NONE,
      &arg_quit,
      0,
      N_("Quit Mail Notification"),
      NULL
    },
    POPT_TABLEEND
  };
  BonoboGenericFactory *automation_factory;
  GClosure *automation_factory_closure;
  Bonobo_RegistrationResult result;

  g_log_set_fatal_mask(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL);
  g_log_set_handler(G_LOG_DOMAIN, G_LOG_LEVEL_INFO, mn_main_info_log_cb, NULL);

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

  gnome_program_init(PACKAGE,
		     VERSION,
		     LIBGNOMEUI_MODULE,
		     argc,
		     argv,
		     GNOME_PARAM_HUMAN_READABLE_NAME, _("Mail Notification"),
		     GNOME_PROGRAM_STANDARD_PROPERTIES,
		     GNOME_PARAM_POPT_TABLE, popt_options,
		     NULL);

  if (arg_version)
    {
      mn_main_print_version();
      exit(0);
    }

  if (arg_unset_obsolete_configuration)
    {
      mn_conf_unset_obsolete();
      exit(0);
    }

  GDK_THREADS_ENTER();

  mn_main_ensure_icon_path();
  gtk_window_set_default_icon_name("mail-notification");

  mn_stock_init();
  bonobo_activate();

  automation_factory = g_object_new(bonobo_generic_factory_get_type(), NULL);
  automation_factory_closure = g_cclosure_new(G_CALLBACK(mn_main_automation_factory_cb), NULL, NULL);
  bonobo_generic_factory_construct_noreg(automation_factory, AUTOMATION_FACTORY_IID, automation_factory_closure);

  result = bonobo_activation_register_active_server(AUTOMATION_FACTORY_IID, BONOBO_OBJREF(automation_factory), NULL);
  switch (result)
    {
    case Bonobo_ACTIVATION_REG_ALREADY_ACTIVE:
    case Bonobo_ACTIVATION_REG_SUCCESS:
      {
	CORBA_Environment env;
	GNOME_MailNotification_Automation automation;
	gboolean display_properties;

	CORBA_exception_init(&env);

	automation = bonobo_activation_activate_from_id(AUTOMATION_IID, 0, NULL, &env);
	if (automation == CORBA_OBJECT_NIL)
	  mn_fatal_error_dialog(NULL, _("Bonobo could not locate the automation object. Please check your Mail Notification installation."));

	if (arg_quit)
	  {
	    if (result == Bonobo_ACTIVATION_REG_ALREADY_ACTIVE)
	      {
		g_message(_("quitting Mail Notification"));
		AUTOMATION_METHOD(quit);
	      }
	    else
	      g_message(_("Mail Notification is not running"));
	  }
	else
	  {
	    if (result != Bonobo_ACTIVATION_REG_ALREADY_ACTIVE)
	      {
		mn_mailbox_init_types();
#if WITH_MBOX || WITH_MOZILLA || WITH_MH || WITH_MAILDIR || WITH_SYLPHEED
		mn_vfs_mailbox_init_types();
#endif

		/* mn-client-session uses sockets, we don't want to die on SIGPIPE */
		signal(SIGPIPE, SIG_IGN);

		if (! gnome_vfs_init())
		  mn_fatal_error_dialog(NULL, _("Unable to initialize the GnomeVFS library."));

		gnome_authentication_manager_init();

#if WITH_MIME
		g_mime_init(0);
#endif

		if (! notify_init(_("Mail Notification")))
		  mn_error_dialog(NULL,
				  _("An initialization error has occurred in Mail Notification"),
				  _("Unable to initialize the notification library. Message popups will not be displayed."));

		mn_locked_callback_init();
		mn_conf_init();

		/*
		 * Work around
		 * http://bugzilla.gnome.org/show_bug.cgi?id=64764:
		 * initialize the classes we will be using
		 * concurrently before any thread is created.
		 */
		mn_main_init_classes();

		mn_shell_new();
	      }

	    if (arg_display_properties)
	      display_properties = TRUE;
	    else /* also display the properties dialog if there are no mailboxes */
	      {
		CORBA_boolean has;
		has = AUTOMATION_METHOD(hasMailboxes);
		display_properties = has == CORBA_FALSE;
	      }

	    if (display_properties)
	      {
		AUTOMATION_METHOD(displayProperties);
	      }
	    if (arg_display_about)
	      {
		AUTOMATION_METHOD(displayAbout);
	      }

	    if (result == Bonobo_ACTIVATION_REG_ALREADY_ACTIVE)
	      {
		if (arg_update)
		  {
		    g_message(_("updating the mail status"));
		    AUTOMATION_METHOD(update);
		  }
		if (arg_print_summary)
		  {
		    CORBA_string summary;

		    summary = AUTOMATION_METHOD(getSummary);
		    g_print("%s", summary);
		    CORBA_free(summary);
		  }

		if (! (display_properties
		       || arg_display_about
		       || arg_update
		       || arg_print_summary))
		  g_message(_("Mail Notification is already running"));
	      }
	    else
	      {
		if (arg_update)
		  mn_main_report_option_ignored("--update");
		if (arg_print_summary)
		  mn_main_report_option_ignored("--print-summary");
	      }
	  }

	bonobo_object_release_unref(automation, &env);
	CORBA_exception_free(&env);
      }
      break;

    case Bonobo_ACTIVATION_REG_NOT_LISTED:
      mn_fatal_error_dialog(NULL, _("Bonobo could not locate the %s file. Please check your Mail Notification installation."), "GNOME_MailNotification.server");
      break;

    case Bonobo_ACTIVATION_REG_ERROR:
      mn_fatal_error_dialog(NULL, _("Bonobo was unable to register the %s server. Please check your Mail Notification installation."), AUTOMATION_FACTORY_IID);
      break;

    default:
      g_assert_not_reached();
      return 1;
    }

  gdk_notify_startup_complete();

  if (result != Bonobo_ACTIVATION_REG_ALREADY_ACTIVE && ! arg_quit)
    gtk_main();

  GDK_THREADS_LEAVE();

  return 0;
}
