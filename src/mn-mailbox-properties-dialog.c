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
  GtkWidget		*type_label;
  GtkWidget		*type_combo;

  GtkWidget		*autodetect_vbox;
  GtkWidget		*location_label;
  GtkWidget		*location_entry;
  GtkWidget		*browse_button;

  GtkWidget		*pop3_vbox;
  GtkWidget		*pop3_hostname_label;
  GtkWidget		*pop3_hostname_entry;
  GtkWidget		*pop3_port_label;
  GtkWidget		*pop3_port_spin;
  GtkWidget		*pop3_username_label;
  GtkWidget		*pop3_username_entry;
  GtkWidget		*pop3_password_label;
  GtkWidget		*pop3_password_entry;

  GtkWidget		*gmail_vbox;
  GtkWidget		*gmail_username_label;
  GtkWidget		*gmail_username_entry;
  GtkWidget		*gmail_password_label;
  GtkWidget		*gmail_password_entry;

  GtkWidget		*unsupported_label;

  MNMailboxPropertiesDialogMode		mode;
  char					*uri;
  GtkWidget				*apply_button;
  GtkWidget				*accept_button;
  GtkListStore				*store;
} Private;

typedef enum {
  TYPE_AUTODETECT,
  TYPE_POP3,
  TYPE_GMAIL
} Type;
    
enum {
  COLUMN_TYPE,
  COLUMN_TEXT,
  COLUMN_STYLE,
  N_COLUMNS
};
    
/*** variables ***************************************************************/

static char *current_folder_uri = NULL;
static Type selected_type = TYPE_AUTODETECT;

/*** functions ***************************************************************/

static void mn_mailbox_properties_dialog_private_free (Private *private);
static gboolean mn_mailbox_properties_dialog_is_complete (MNMailboxPropertiesDialog *dialog);
static Type mn_mailbox_properties_dialog_get_active_type (MNMailboxPropertiesDialog *dialog);
static void mn_mailbox_properties_dialog_set_active_type (MNMailboxPropertiesDialog *dialog,
							  Type type);
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
  GtkTreeIter iter;
  GtkCellRenderer *renderer;

  private = g_new0(Private, 1);
  mn_create_interface("mailbox-properties",
		      "dialog", (GtkWidget **) &dialog,
		      "type_label", &private->type_label,
		      "type_combo", &private->type_combo,
		      "autodetect_vbox", &private->autodetect_vbox,
		      "location_label", &private->location_label,
		      "location_entry", &private->location_entry,
		      "browse_button", &private->browse_button,
		      "pop3_vbox", &private->pop3_vbox,
		      "pop3_hostname_label", &private->pop3_hostname_label,
		      "pop3_hostname_entry", &private->pop3_hostname_entry,
		      "pop3_port_label", &private->pop3_port_label,
		      "pop3_port_spin", &private->pop3_port_spin,
		      "pop3_username_label", &private->pop3_username_label,
		      "pop3_username_entry", &private->pop3_username_entry,
		      "pop3_password_label", &private->pop3_password_label,
		      "pop3_password_entry", &private->pop3_password_entry,
		      "gmail_vbox", &private->gmail_vbox,
		      "gmail_username_label", &private->gmail_username_label,
		      "gmail_username_entry", &private->gmail_username_entry,
		      "gmail_password_label", &private->gmail_password_label,
		      "gmail_password_entry", &private->gmail_password_entry,
		      "unsupported_label", &private->unsupported_label,
		      NULL);
  private->mode = mode;

  g_object_set_data_full(G_OBJECT(dialog),
			 MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE_KEY,
			 private,
			 (GDestroyNotify) mn_mailbox_properties_dialog_private_free);

  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget(size_group, private->type_label);
  gtk_size_group_add_widget(size_group, private->location_label);
  gtk_size_group_add_widget(size_group, private->pop3_hostname_label);
  gtk_size_group_add_widget(size_group, private->pop3_username_label);
  gtk_size_group_add_widget(size_group, private->pop3_password_label);
  gtk_size_group_add_widget(size_group, private->gmail_username_label);
  gtk_size_group_add_widget(size_group, private->gmail_password_label);
  g_object_unref(size_group);

  if (parent)
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

  gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_HELP, GTK_RESPONSE_HELP);
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
      
  /* finish the combo box */

  private->store = gtk_list_store_new(N_COLUMNS,
				      G_TYPE_INT,
				      G_TYPE_STRING,
				      PANGO_TYPE_STYLE);

  gtk_list_store_append(private->store, &iter);
  gtk_list_store_set(private->store, &iter,
		     COLUMN_TYPE, TYPE_AUTODETECT,
		     COLUMN_TEXT, _("autodetect"),
		     COLUMN_STYLE, PANGO_STYLE_ITALIC,
		     -1);

  gtk_list_store_append(private->store, &iter);
  gtk_list_store_set(private->store, &iter,
		     COLUMN_TYPE, TYPE_POP3,
		     COLUMN_TEXT, "POP3",
		     -1);

  gtk_list_store_append(private->store, &iter);
  gtk_list_store_set(private->store, &iter,
		     COLUMN_TYPE, TYPE_GMAIL,
		     COLUMN_TEXT, "Gmail",
		     -1);

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(private->type_combo), renderer, TRUE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(private->type_combo), renderer,
				 "text", COLUMN_TEXT,
				 "style", COLUMN_STYLE,
				 NULL);

  gtk_combo_box_set_model(GTK_COMBO_BOX(private->type_combo), GTK_TREE_MODEL(private->store));

  if (mode == MN_MAILBOX_PROPERTIES_DIALOG_MODE_ADD)
    mn_mailbox_properties_dialog_set_active_type(dialog, selected_type);
    
  return GTK_WIDGET(dialog);
}

static void
mn_mailbox_properties_dialog_private_free (Private *private)
{
  g_return_if_fail(private != NULL);

  g_free(private->uri);
  g_object_unref(private->store);
  g_free(private);
}

static gboolean
mn_mailbox_properties_dialog_is_complete (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  gboolean is_complete = FALSE;

  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog), FALSE);
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  switch (mn_mailbox_properties_dialog_get_active_type(dialog))
    {
    case TYPE_AUTODETECT:
      {
	const char *location;
	
	location = gtk_entry_get_text(GTK_ENTRY(private->location_entry));
	is_complete = *location != 0;
      }
      break;

    case TYPE_POP3:
#ifdef WITH_POP3
      {
	const char *hostname;
	const char *username;
	const char *password;
      
	hostname = gtk_entry_get_text(GTK_ENTRY(private->pop3_hostname_entry));
	username = gtk_entry_get_text(GTK_ENTRY(private->pop3_username_entry));
	password = gtk_entry_get_text(GTK_ENTRY(private->pop3_password_entry));

	is_complete = *hostname != 0 && *username != 0 && *password != 0;
      }
#endif /* WITH_POP3 */
      break;

    case TYPE_GMAIL:
#ifdef WITH_GMAIL
      {
	const char *username;
	const char *password;
	
	username = gtk_entry_get_text(GTK_ENTRY(private->gmail_username_entry));
	password = gtk_entry_get_text(GTK_ENTRY(private->gmail_password_entry));
      
	is_complete = *username != 0 && *password != 0;
      }
#endif /* WITH_GMAIL */
      break;

    default:
      g_return_val_if_reached(FALSE);
    }
  
  return is_complete;
}

static Type
mn_mailbox_properties_dialog_get_active_type (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  GtkTreeIter iter;
  Type type = TYPE_AUTODETECT;

  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog), 0);
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(private->type_combo), &iter))
    gtk_tree_model_get(GTK_TREE_MODEL(private->store), &iter, COLUMN_TYPE, &type, -1);

  return type;
}

static void
mn_mailbox_properties_dialog_set_active_type (MNMailboxPropertiesDialog *dialog,
					      Type type)
{
  Private *private;
  gboolean valid;
  GtkTreeIter iter;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(private->store), &iter);
  while (valid)
    {
      Type this_type;

      gtk_tree_model_get(GTK_TREE_MODEL(private->store), &iter, COLUMN_TYPE, &this_type, -1);
      if (this_type == type)
	{
	  gtk_combo_box_set_active_iter(GTK_COMBO_BOX(private->type_combo), &iter);
	  break;
	}

      valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(private->store), &iter);
    }
}

static void
mn_mailbox_properties_dialog_update_sensitivity (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  Type type;
  gboolean is_complete;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  type = mn_mailbox_properties_dialog_get_active_type(dialog);
  
  switch (mn_mailbox_properties_dialog_get_active_type(dialog))
    {
    case TYPE_AUTODETECT:
      gtk_widget_show(private->autodetect_vbox);
      gtk_widget_hide(private->unsupported_label);
      gtk_widget_hide(private->pop3_vbox);
      gtk_widget_hide(private->gmail_vbox);
      break;

    case TYPE_POP3:
#ifdef WITH_POP3
      gtk_widget_show(private->pop3_vbox);
      gtk_widget_hide(private->unsupported_label);
#else
      gtk_widget_show(private->unsupported_label);
      gtk_widget_hide(private->pop3_vbox);
#endif /* WITH_POP3 */
      gtk_widget_hide(private->autodetect_vbox);
      gtk_widget_hide(private->gmail_vbox);
      break;

    case TYPE_GMAIL:
#ifdef WITH_GMAIL
      gtk_widget_show(private->gmail_vbox);
      gtk_widget_hide(private->unsupported_label);
#else
      gtk_widget_show(private->unsupported_label);
      gtk_widget_hide(private->gmail_vbox);
#endif /* WITH_GMAIL */
      gtk_widget_hide(private->autodetect_vbox);
      gtk_widget_hide(private->pop3_vbox);
      break;
      
    default:
      g_return_if_reached();
    }
  
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
  Type type;
  char *hostname;
  int port;
  char *username;
  char *password;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);
  g_return_if_fail(private->uri != NULL);

  if (mn_uri_parse_pop(private->uri, &username, &password, &hostname, &port))
    {
      type = TYPE_POP3;

#ifdef WITH_POP3
      gtk_entry_set_text(GTK_ENTRY(private->pop3_hostname_entry), hostname);
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(private->pop3_port_spin), port);
      gtk_entry_set_text(GTK_ENTRY(private->pop3_username_entry), username);
      gtk_entry_set_text(GTK_ENTRY(private->pop3_password_entry), password);
#endif /* WITH_POP3 */

      g_free(hostname);
      g_free(username);
      g_free(password);
    }
  else if (mn_uri_parse_gmail(private->uri, &username, &password))
    {
      type = TYPE_GMAIL;

#ifdef WITH_GMAIL
      gtk_entry_set_text(GTK_ENTRY(private->gmail_username_entry), username);
      gtk_entry_set_text(GTK_ENTRY(private->gmail_password_entry), password);
#endif /* WITH_GMAIL */

      g_free(username);
      g_free(password);
    }
  else
    {
      type = TYPE_AUTODETECT;

      gtk_entry_set_text(GTK_ENTRY(private->location_entry), private->uri);
    }

  mn_mailbox_properties_dialog_set_active_type(dialog, type);
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

  switch (mn_mailbox_properties_dialog_get_active_type(dialog))
    {
    case TYPE_AUTODETECT:
      {
	const char *location;
	
	location = gtk_entry_get_text(GTK_ENTRY(private->location_entry));
	uri = g_strdup(location);
      }
      break;

#ifdef WITH_POP3
    case TYPE_POP3:
      {
	const char *hostname;
	int port;
	const char *username;
	const char *password;
	
	hostname = gtk_entry_get_text(GTK_ENTRY(private->pop3_hostname_entry));
	port = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(private->pop3_port_spin));
	username = gtk_entry_get_text(GTK_ENTRY(private->pop3_username_entry));
	password = gtk_entry_get_text(GTK_ENTRY(private->pop3_password_entry));
	
	uri = mn_uri_build_pop(username, password, hostname, port);
      }
      break;
#endif /* WITH_POP3 */

#ifdef WITH_GMAIL
    case TYPE_GMAIL:
      {
	const char *username;
	const char *password;

	username = gtk_entry_get_text(GTK_ENTRY(private->gmail_username_entry));
	password = gtk_entry_get_text(GTK_ENTRY(private->gmail_password_entry));
	
	uri = mn_uri_build_gmail(username, password);
      }
      break;
#endif /* TYPE_GMAIL */
      
    default:
      g_return_val_if_reached(NULL);
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
mn_mailbox_properties_dialog_type_changed_h (gpointer user_data,
					     GtkComboBox *combobox)
{
  MNMailboxPropertiesDialog *dialog = user_data;
  Private *private;

  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);
  if (private->mode == MN_MAILBOX_PROPERTIES_DIALOG_MODE_ADD)
    selected_type = mn_mailbox_properties_dialog_get_active_type(dialog);

  mn_mailbox_properties_dialog_update_sensitivity(dialog);
}

void
mn_mailbox_properties_dialog_entry_activate_h (gpointer user_data,
					       GtkEntry *entry)
{
  GtkWidget *next = user_data;
  GtkWidget *toplevel;

  toplevel = gtk_widget_get_toplevel(GTK_WIDGET(entry));
  if (GTK_WIDGET_TOPLEVEL(toplevel)
      && GTK_WINDOW(toplevel)->default_widget
      && GTK_WIDGET_IS_SENSITIVE(GTK_WINDOW(toplevel)->default_widget))
    gtk_window_activate_default(GTK_WINDOW(toplevel));
  else
    gtk_widget_grab_focus(next);
}

void
mn_mailbox_properties_dialog_entry_changed_h (gpointer user_data,
					      GtkEditable *editable)
{
  MNMailboxPropertiesDialog *dialog = user_data;

  mn_mailbox_properties_dialog_update_sensitivity(dialog);
}
