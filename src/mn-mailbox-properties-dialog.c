/* 
 * Copyright (c) 2003, 2004 Jean-Yves Lefort <jylefort@brutele.be>
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
#include <glib/gi18n-lib.h>
#include "mn-mailbox-properties-dialog.h"
#include "mn-conf.h"
#include "mn-util.h"
#include "mn-uri.h"

/*** types *******************************************************************/

typedef struct
{
  GtkWidget		*file_radio;
  GtkWidget		*location_label;
  GtkWidget		*location_entry;
  GtkWidget		*browse;
  GtkWidget		*pop3_radio;
  GtkWidget		*hostname_label;
  GtkWidget		*hostname_entry;
  GtkWidget		*port_label;
  GtkWidget		*port_spin;
  GtkWidget		*username_label;
  GtkWidget		*username_entry;
  GtkWidget		*password_label;
  GtkWidget		*password_entry;

  MNMailboxPropertiesDialogMode		mode;
  char					*uri;
  GtkWidget				*apply_button;
  GtkWidget				*accept_button;
} Private;

typedef enum
{
  RADIO_FILE,
  RADIO_POP3
} Radio;

/*** variables ***************************************************************/

static Radio selected_radio = RADIO_FILE;
static char *current_folder_uri = NULL;

/*** functions ***************************************************************/

static void mn_mailbox_properties_dialog_private_free (Private *private);
static gboolean mn_mailbox_properties_dialog_is_complete (MNMailboxPropertiesDialog *dialog);
static void mn_mailbox_properties_dialog_update_sensitivity (MNMailboxPropertiesDialog *dialog);
static void mn_mailbox_properties_dialog_set_uri_internal (MNMailboxPropertiesDialog *dialog,
							   const char *uri);
static void mn_mailbox_properties_dialog_fill (MNMailboxPropertiesDialog *dialog);
static void mn_mailbox_properties_dialog_current_folder_changed_h (GtkFileChooser *chooser,
								   gpointer user_data);

/*** implementation **********************************************************/

GtkWidget *
mn_mailbox_properties_dialog_new (GtkWindow *parent,
				  MNMailboxPropertiesDialogMode mode)
{
  MNMailboxPropertiesDialog *dialog;
  Private *private;
  GtkSizeGroup *size_group;

  private = g_new0(Private, 1);
  mn_create_interface("mailbox-properties",
		      "dialog", (GtkWidget **) &dialog,
		      "file_radio", &private->file_radio,
		      "location_label", &private->location_label,
		      "location_entry", &private->location_entry,
		      "browse", &private->browse,
		      "pop3_radio", &private->pop3_radio,
		      "hostname_label", &private->hostname_label,
		      "hostname_entry", &private->hostname_entry,
		      "port_label", &private->port_label,
		      "port_spin", &private->port_spin,
		      "username_label", &private->username_label,
		      "username_entry", &private->username_entry,
		      "password_label", &private->password_label,
		      "password_entry", &private->password_entry,
		      NULL);
  private->mode = mode;

  g_object_set_data_full(G_OBJECT(dialog),
			 MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE_KEY,
			 private,
			 (GDestroyNotify) mn_mailbox_properties_dialog_private_free);

  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget(size_group, private->location_label);
  gtk_size_group_add_widget(size_group, private->hostname_label);
  gtk_size_group_add_widget(size_group, private->username_label);
  g_object_unref(size_group);

  if (parent)
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

  if (mode == MN_MAILBOX_PROPERTIES_DIALOG_MODE_ADD)
    {
      gtk_window_set_title(GTK_WINDOW(dialog), _("Add a Mailbox"));

      gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
      private->accept_button = gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_ADD, GTK_RESPONSE_ACCEPT);
    }
  else if (mode == MN_MAILBOX_PROPERTIES_DIALOG_MODE_EDIT)
    {
      /* title will be set in _set_uri_internal() */

      private->apply_button = gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_APPLY, GTK_RESPONSE_APPLY);
      gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
      private->accept_button = gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
    }
  else
    g_return_val_if_reached(NULL);
  gtk_widget_grab_default(private->accept_button);
      
#ifndef WITH_POP3
  gtk_widget_set_sensitive(private->pop3_radio, FALSE);
#endif

  if (mode == MN_MAILBOX_PROPERTIES_DIALOG_MODE_ADD)
    {
      GtkWidget *default_radio;
      
      switch (selected_radio)
	{
	case RADIO_FILE:	default_radio = private->file_radio; break;
	case RADIO_POP3:	default_radio = private->pop3_radio; break;
	default:		g_return_val_if_reached(NULL);
	}

      if (! GTK_WIDGET_IS_SENSITIVE(default_radio))
	default_radio = private->file_radio;

      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(default_radio), TRUE);
    }
    
  mn_mailbox_properties_dialog_update_sensitivity(dialog);

  return GTK_WIDGET(dialog);
}

static void
mn_mailbox_properties_dialog_private_free (Private *private)
{
  g_return_if_fail(private != NULL);

  g_free(private->uri);
  g_free(private);
}

static gboolean
mn_mailbox_properties_dialog_is_complete (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  gboolean is_complete = FALSE;

  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog), FALSE);
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(private->file_radio)))
    {
      const char *location;

      location = gtk_entry_get_text(GTK_ENTRY(private->location_entry));
      is_complete = *location != 0;
    }
  else if (GTK_WIDGET_IS_SENSITIVE(private->pop3_radio))
    {
      const char *hostname;
      const char *username;
      const char *password;
      
      hostname = gtk_entry_get_text(GTK_ENTRY(private->hostname_entry));
      username = gtk_entry_get_text(GTK_ENTRY(private->username_entry));
      password = gtk_entry_get_text(GTK_ENTRY(private->password_entry));

      is_complete = *hostname != 0 && *username != 0 && *password != 0;
    }

  return is_complete;
}

static void
mn_mailbox_properties_dialog_update_sensitivity (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  gboolean file_selected;
  gboolean pop3_selected;
  gboolean is_complete;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  file_selected = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(private->file_radio));
  pop3_selected = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(private->pop3_radio)) && GTK_WIDGET_IS_SENSITIVE(private->pop3_radio);

  gtk_widget_set_sensitive(private->location_label, file_selected);
  gtk_widget_set_sensitive(private->location_entry, file_selected);
  gtk_widget_set_sensitive(private->browse, file_selected);

  gtk_widget_set_sensitive(private->hostname_label, pop3_selected);
  gtk_widget_set_sensitive(private->hostname_entry, pop3_selected);
  gtk_widget_set_sensitive(private->port_label, pop3_selected);
  gtk_widget_set_sensitive(private->port_spin, pop3_selected);
  gtk_widget_set_sensitive(private->username_label, pop3_selected);
  gtk_widget_set_sensitive(private->username_entry, pop3_selected);
  gtk_widget_set_sensitive(private->password_label, pop3_selected);
  gtk_widget_set_sensitive(private->password_entry, pop3_selected);

  is_complete = mn_mailbox_properties_dialog_is_complete(dialog);
  if (private->apply_button)
    gtk_widget_set_sensitive(private->apply_button, is_complete);
  gtk_widget_set_sensitive(private->accept_button, is_complete);
}

static void
mn_mailbox_properties_dialog_set_uri_internal (MNMailboxPropertiesDialog *dialog,
					       const char *uri)
{
  Private *private;
  char *name;
  char *title;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  g_return_if_fail(uri != NULL);
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  g_free(private->uri);
  private->uri = g_strdup(uri);

  name = mn_uri_format_for_display(private->uri);
  title = g_strdup_printf(_("%s Properties"), name);
  g_free(name);

  gtk_window_set_title(GTK_WINDOW(dialog), title);
  g_free(title);
}

static void
mn_mailbox_properties_dialog_fill (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  char *hostname;
  int port;
  char *username;
  char *password;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);
  g_return_if_fail(private->uri != NULL);

  if (mn_uri_parse_pop(private->uri, &username, &password, &hostname, &port))
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(private->pop3_radio), TRUE);

      gtk_entry_set_text(GTK_ENTRY(private->hostname_entry), hostname);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(private->port_spin), port);
      gtk_entry_set_text(GTK_ENTRY(private->username_entry), username);
      gtk_entry_set_text(GTK_ENTRY(private->password_entry), password);

      g_free(hostname);
      g_free(username);
      g_free(password);
    }
  else
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(private->file_radio), TRUE);
      gtk_entry_set_text(GTK_ENTRY(private->location_entry), private->uri);
    }
}

void
mn_mailbox_properties_dialog_set_uri (MNMailboxPropertiesDialog *dialog,
				      const char *uri)
{
  Private *private;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);
  g_return_if_fail(private->uri == NULL);

  mn_mailbox_properties_dialog_set_uri_internal(dialog, uri);
  mn_mailbox_properties_dialog_fill(dialog);
}

char *
mn_mailbox_properties_dialog_get_uri (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  char *uri;

  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog), NULL);
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(private->file_radio)))
    {
      const char *location;

      location = gtk_entry_get_text(GTK_ENTRY(private->location_entry));
      uri = g_strdup(location);
    }
  else
    {
      const char *hostname;
      int port;
      const char *username;
      const char *password;

      hostname = gtk_entry_get_text(GTK_ENTRY(private->hostname_entry));
      port = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(private->port_spin));
      username = gtk_entry_get_text(GTK_ENTRY(private->username_entry));
      password = gtk_entry_get_text(GTK_ENTRY(private->password_entry));

      uri = mn_uri_build_pop(username, password, hostname, port);
    }

  return uri;
}

void
mn_mailbox_properties_dialog_apply (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  char *new_uri;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  new_uri = mn_mailbox_properties_dialog_get_uri(dialog);
  g_return_if_fail(new_uri != NULL);

  if (mn_uri_cmp(new_uri, private->uri))
    {
      GSList *gconf_mailboxes;
      GSList *elem;

      gconf_mailboxes = eel_gconf_get_string_list(MN_CONF_MAILBOXES);

      elem = g_slist_find_custom(gconf_mailboxes, private->uri, (GCompareFunc) mn_uri_cmp);
      if (elem)
	{
	  g_free(elem->data);
	  elem->data = g_strdup(new_uri);
	}
      
      eel_gconf_set_string_list(MN_CONF_MAILBOXES, gconf_mailboxes);
      mn_slist_free(gconf_mailboxes);
      
      mn_mailbox_properties_dialog_set_uri_internal(dialog, new_uri);
    }
  g_free(new_uri);
}

static void
mn_mailbox_properties_dialog_current_folder_changed_h (GtkFileChooser *chooser,
						       gpointer user_data)
{
  g_free(current_folder_uri);
  current_folder_uri = gtk_file_chooser_get_current_folder_uri(chooser);
}

/* libglade callbacks */

void
mn_mailbox_properties_dialog_browse_clicked_h (gpointer user_data,
					       GtkButton *button)
{
  MNMailboxPropertiesDialog *dialog = user_data;
  Private *private;
  GtkWidget *chooser;
  const char *location;

  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  chooser = gtk_file_chooser_dialog_new_with_backend(_("Select a File or Folder"),
						     GTK_WINDOW(dialog),
						     GTK_FILE_CHOOSER_ACTION_OPEN,
						     "gnome-vfs",
						     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						     GTK_STOCK_OPEN, 1,
						     NULL);

  gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(chooser), FALSE);
  mn_file_chooser_dialog_allow_select_folder(GTK_FILE_CHOOSER_DIALOG(chooser), 1);

  if (private->mode == MN_MAILBOX_PROPERTIES_DIALOG_MODE_ADD)
    {
      if (current_folder_uri)
	gtk_file_chooser_set_current_folder_uri(GTK_FILE_CHOOSER(chooser), current_folder_uri);
      g_signal_connect(G_OBJECT(chooser), "current-folder-changed", G_CALLBACK(mn_mailbox_properties_dialog_current_folder_changed_h), NULL);
    }

  location = gtk_entry_get_text(GTK_ENTRY(private->location_entry));
  if (*location)
    gtk_file_chooser_set_uri(GTK_FILE_CHOOSER(chooser), location);

  if (gtk_dialog_run(GTK_DIALOG(chooser)) == 1)
    {
      char *uri;
      
      uri = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(chooser));
      gtk_entry_set_text(GTK_ENTRY(private->location_entry), uri);
      g_free(uri);
    }

  gtk_widget_destroy(chooser);
}

void
mn_mailbox_properties_dialog_radio_toggled_h (gpointer user_data,
					      GtkWidget *widget)
{
  MNMailboxPropertiesDialog *dialog = user_data;
  Private *private;

  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);
  if (private->mode == MN_MAILBOX_PROPERTIES_DIALOG_MODE_ADD)
    {
      if (widget == private->file_radio)
	selected_radio = RADIO_FILE;
      else if (widget == private->pop3_radio)
	selected_radio = RADIO_POP3;
      else
	g_return_if_reached();
    }
  
  mn_mailbox_properties_dialog_update_sensitivity(dialog);
}

void
mn_mailbox_properties_dialog_pop3_entry_activate_h (gpointer user_data,
						    GtkWidget *widget)
{
  GtkWidget *next = user_data;
  GtkWidget *toplevel;

  toplevel = gtk_widget_get_toplevel(widget);
  if (GTK_WIDGET_TOPLEVEL(toplevel)
      && GTK_WINDOW(toplevel)->default_widget
      && GTK_WIDGET_IS_SENSITIVE(GTK_WINDOW(toplevel)->default_widget))
    gtk_window_activate_default(GTK_WINDOW(toplevel));
  else
    gtk_widget_grab_focus(next);
}

void
mn_mailbox_properties_dialog_entry_changed_h (gpointer user_data,
					      GtkWidget *widget)
{
  MNMailboxPropertiesDialog *dialog = user_data;

  mn_mailbox_properties_dialog_update_sensitivity(dialog);
}
