/* 
 * Copyright (C) 2005 Jean-Yves Lefort <jylefort@brutele.be>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"
#include <stdarg.h>
#include <string.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <libbonobo.h>
#include <camel/camel-folder.h>

#ifdef HAVE_EVOLUTION_2_2
/* headers from the Evolution source tree */
#include "mail/em-event.h"
#include "mail/mail-tools.h"
#else
#include <mail/em-event.h>
#include <mail/mail-tools.h>
#endif

#include "mn-evolution.h"
#include "mn-evolution-glue.h"
#include "mn-evolution-folder-tree-control.h"

/*** cpp *********************************************************************/

#define FACTORY			"_Factory"

/*** variables ***************************************************************/

static GSList *glue_event_sources = NULL;

/*** functions ***************************************************************/

static void mn_evolution_plugin_error_dialog (const char *primary,
					      const char *format,
					      ...);
static BonoboObject *mn_evolution_plugin_glue_factory_cb (BonoboGenericFactory *factory,
							  const char *iid,
							  gpointer closure);
static gboolean mn_evolution_plugin_factory_create (const char *factory_iid,
						    BonoboFactoryCallback factory_cb,
						    gpointer user_data);

/*** implementation **********************************************************/

static void
mn_evolution_plugin_error_dialog (const char *primary, const char *format, ...)
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

static BonoboObject *
mn_evolution_plugin_glue_factory_cb (BonoboGenericFactory *factory,
				     const char *iid,
				     gpointer closure)
{
  BonoboObject *object;
  BonoboEventSource *event_source;

  if (strcmp(iid, MN_EVOLUTION_GLUE_IID) != 0)
    return NULL;

  object = BONOBO_OBJECT(mn_evolution_glue_new());
  event_source = bonobo_event_source_new();

  glue_event_sources = g_slist_append(glue_event_sources, event_source);
  bonobo_object_add_interface(object, BONOBO_OBJECT(event_source));

  return object;
}

static gboolean
mn_evolution_plugin_factory_create (const char *factory_iid,
				    BonoboFactoryCallback factory_cb,
				    gpointer user_data)
{
  BonoboGenericFactory *factory;
  GClosure *closure;
  Bonobo_RegistrationResult result;

  g_return_val_if_fail(factory_iid != NULL, FALSE);
  g_return_val_if_fail(factory_cb != NULL, FALSE);

  factory = g_object_new(bonobo_generic_factory_get_type(), NULL);
  closure = g_cclosure_new(G_CALLBACK(factory_cb), user_data, NULL);
  bonobo_generic_factory_construct_noreg(factory, factory_iid, closure);

  result = bonobo_activation_register_active_server(factory_iid, BONOBO_OBJREF(factory), NULL);
  switch (result)
    {
    case Bonobo_ACTIVATION_REG_ALREADY_ACTIVE:
    case Bonobo_ACTIVATION_REG_SUCCESS:
      return TRUE;

    case Bonobo_ACTIVATION_REG_NOT_LISTED:
      mn_evolution_plugin_error_dialog(dgettext(GETTEXT_PACKAGE, "Unable to activate the Mail Notification plugin"), dgettext(GETTEXT_PACKAGE, "Bonobo could not locate the %s server. Please check your Mail Notification installation."), factory_iid);
      return FALSE;

    case Bonobo_ACTIVATION_REG_ERROR:
      mn_evolution_plugin_error_dialog(dgettext(GETTEXT_PACKAGE, "Unable to activate the Mail Notification plugin"), dgettext(GETTEXT_PACKAGE, "Bonobo was unable to register the %s server. Please check your Mail Notification installation."), factory_iid);
      return FALSE;

    default:
      g_assert_not_reached();
      return FALSE;
    }
}

int
e_plugin_lib_enable (EPluginLib *ep, int enable)
{
  static gboolean enabled = FALSE;

  if (! enable || enabled)
    return 0;			/* success */

  enabled = TRUE;

  if (mn_evolution_plugin_factory_create(MN_EVOLUTION_GLUE_IID FACTORY, mn_evolution_plugin_glue_factory_cb, NULL)
      && mn_evolution_plugin_factory_create(MN_EVOLUTION_FOLDER_TREE_CONTROL_IID FACTORY, mn_evolution_folder_tree_control_factory_cb, NULL))
    return 0;			/* success */
  else
    {
      GSList *l;

      for (l = glue_event_sources; l != NULL; l = l->next)
	bonobo_object_release_unref(BONOBO_OBJREF(l->data), NULL);

      g_slist_free(glue_event_sources);
      glue_event_sources = NULL;

      return 1;			/* failure */
    }
}

void
org_gnome_mail_notification_folder_changed (EPlugin *plugin,
					    EMEventTargetFolder *folder)
{
  if (glue_event_sources)
    {
      BonoboArg *arg;
      GSList *l;

      arg = bonobo_arg_new(BONOBO_ARG_STRING);
      BONOBO_ARG_SET_STRING(arg, folder->uri);

      for (l = glue_event_sources; l != NULL; l = l->next)
	bonobo_event_source_notify_listeners_full(l->data,
						  MN_EVOLUTION_EVENT_PREFIX,
						  MN_EVOLUTION_EVENT_FOLDER_CHANGED,
						  NULL,
						  arg,
						  NULL);

      bonobo_arg_release(arg);
    }
}

void
org_gnome_mail_notification_message_reading (EPlugin *plugin,
					     EMEventTargetMessage *message)
{
  if (glue_event_sources)
    {
      BonoboArg *arg;
      char *url;
      GSList *l;

      arg = bonobo_arg_new(BONOBO_ARG_STRING);

      url = mail_tools_folder_to_url(message->folder);
      BONOBO_ARG_SET_STRING(arg, url);
      g_free(url);

      for (l = glue_event_sources; l != NULL; l = l->next)
	bonobo_event_source_notify_listeners_full(l->data,
						  MN_EVOLUTION_EVENT_PREFIX,
						  MN_EVOLUTION_EVENT_MESSAGE_READING,
						  NULL,
						  arg,
						  NULL);

      bonobo_arg_release(arg);
    }
}
