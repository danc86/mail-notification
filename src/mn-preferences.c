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
#include <libgnome/gnome-i18n.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <string.h>
#include "mn-conf.h"
#include "mn-dialog.h"
#include "mn-mailbox.h"
#include "mn-mailboxes.h"
#include "mn-preferences.h"
#include "mn-util.h"

/*** types *******************************************************************/

enum {
  COLUMN_OBJECT,
  COLUMN_MAILBOX,
  COLUMN_FORMAT,
  N_COLUMNS
};

/*** variables ***************************************************************/

static GladeXML		*preferences_xml = NULL;
static GtkWidget	*preferences;

static GtkWidget	*local_check;
static GtkWidget	*local_minutes_spin;
static GtkWidget	*local_minutes_label;
static GtkWidget	*local_seconds_spin;
static GtkWidget	*local_seconds_label;

static GtkWidget	*remote_check;
static GtkWidget	*remote_minutes_spin;
static GtkWidget	*remote_minutes_label;
static GtkWidget	*remote_seconds_spin;
static GtkWidget	*remote_seconds_label;

static GtkWidget	*list;
static GtkWidget	*add_remote;
static GtkWidget	*remove;

static GtkWidget	*command_new_mail_check;
static GtkWidget	*command_new_mail_entry;
static GtkWidget	*command_clicked_check;
static GtkWidget	*command_clicked_entry;

static GtkListStore	*store;

/*** functions ***************************************************************/

static void mn_preferences_add_column		(int              id,
						 const char       *label);
static void mn_preferences_bind_boolean		(GtkWidget        *widget,
						 gpointer         key);
static void mn_preferences_boolean_toggled	(GtkToggleButton  *button,
						 gpointer         user_data);
static void mn_preferences_bind_int		(GtkWidget        *widget,
						 gpointer         key);
static void mn_preferences_int_changed		(GtkSpinButton    *button,
						 gpointer         user_data);
static void mn_preferences_bind_string		(GtkWidget        *widget,
						 gpointer         key);
static void mn_preferences_string_changed	(GtkEditable      *editable,
						 gpointer         user_data);
static void mn_preferences_selection_changed	(GtkTreeSelection *selection,
						 gpointer         user_data);
static void mn_preferences_remove_mailbox_cb	(GtkTreeModel     *model,
						 GtkTreePath      *path,
						 GtkTreeIter      *iter,
						 gpointer         data);
static gboolean mn_preferences_search_equal_func (GtkTreeModel    *model,
						  int             column,
						  const char      *key,
						  GtkTreeIter     *iter,
						  gpointer        search_data);
static void mn_preferences_update_sensitivity	(void);
static void mn_preferences_add_local_mailbox	(void);
static void mn_preferences_add_remote_mailbox	(void);
static void mn_preferences_remove_mailbox	(void);

/*** implementation **********************************************************/

static void
mn_preferences_add_column (int id, const char *label)
{
  GtkTreeViewColumn *column;

  g_return_if_fail(label != NULL);

  column = gtk_tree_view_column_new_with_attributes(label,
						    gtk_cell_renderer_text_new(),
						    "text", id,
						    NULL);
  gtk_tree_view_column_set_sort_column_id(column, id);
  gtk_tree_view_column_set_resizable(column, TRUE);
  gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
}

void
mn_preferences_display (void)
{
  GtkSizeGroup *size_group;
  GtkTreeSelection *selection;

  if (preferences_xml)
    {
      gtk_window_present(GTK_WINDOW(preferences));
      return;
    }
  
  preferences_xml = mn_glade_xml_new("preferences");

  preferences = glade_xml_get_widget(preferences_xml, "dialog");
  local_check = glade_xml_get_widget(preferences_xml, "local_check");
  local_minutes_spin = glade_xml_get_widget(preferences_xml, "local_minutes_spin");
  local_minutes_label = glade_xml_get_widget(preferences_xml, "local_minutes_label");
  local_seconds_spin = glade_xml_get_widget(preferences_xml, "local_seconds_spin");
  local_seconds_label = glade_xml_get_widget(preferences_xml, "local_seconds_label");
  remote_check = glade_xml_get_widget(preferences_xml, "remote_check");
  remote_minutes_spin = glade_xml_get_widget(preferences_xml, "remote_minutes_spin");
  remote_minutes_label = glade_xml_get_widget(preferences_xml, "remote_minutes_label");
  remote_seconds_spin = glade_xml_get_widget(preferences_xml, "remote_seconds_spin");
  remote_seconds_label = glade_xml_get_widget(preferences_xml, "remote_seconds_label");
  list = glade_xml_get_widget(preferences_xml, "list");
  add_remote = glade_xml_get_widget(preferences_xml, "add_remote");
  remove = glade_xml_get_widget(preferences_xml, "remove");
  command_new_mail_check = glade_xml_get_widget(preferences_xml, "command_new_mail_check");
  command_new_mail_entry = glade_xml_get_widget(preferences_xml, "command_new_mail_entry");
  command_clicked_check = glade_xml_get_widget(preferences_xml, "command_clicked_check");
  command_clicked_entry = glade_xml_get_widget(preferences_xml, "command_clicked_entry");

#ifndef WITH_POP3
  gtk_widget_set_sensitive(add_remote, FALSE);
#endif

  /* finish the mailboxes list */

  store = gtk_list_store_new(N_COLUMNS,
			     MN_TYPE_MAILBOX,
			     G_TYPE_STRING,
			     G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));

  mn_preferences_add_column(COLUMN_MAILBOX, _("Mailbox"));
  mn_preferences_add_column(COLUMN_FORMAT, _("Format"));

  gtk_tree_view_set_enable_search(GTK_TREE_VIEW(list), TRUE);
  gtk_tree_view_set_search_equal_func(GTK_TREE_VIEW(list),
				      mn_preferences_search_equal_func,
				      NULL,
				      NULL);
  
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
  g_signal_connect(G_OBJECT(selection), "changed",
		   G_CALLBACK(mn_preferences_selection_changed), NULL);
  
  /* create the GtkSizeGroup */
  
  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget(size_group, local_check);
  gtk_size_group_add_widget(size_group, remote_check);
  gtk_size_group_add_widget(size_group, command_new_mail_check);
  gtk_size_group_add_widget(size_group, command_clicked_check);

  mn_preferences_update_values();
  mn_preferences_update_sensitivity();

  mn_preferences_bind_boolean(local_check, "/apps/mail-notification/local/enabled");
  mn_preferences_bind_int(local_minutes_spin, "/apps/mail-notification/local/delay/minutes");
  mn_preferences_bind_int(local_seconds_spin, "/apps/mail-notification/local/delay/seconds");
  mn_preferences_bind_boolean(remote_check, "/apps/mail-notification/remote/enabled");
  mn_preferences_bind_int(remote_minutes_spin, "/apps/mail-notification/remote/delay/minutes");
  mn_preferences_bind_int(remote_seconds_spin, "/apps/mail-notification/remote/delay/seconds");
  mn_preferences_bind_boolean(command_new_mail_check, "/apps/mail-notification/commands/new-mail/enabled");
  mn_preferences_bind_string(command_new_mail_entry, "/apps/mail-notification/commands/new-mail/command");
  mn_preferences_bind_boolean(command_clicked_check, "/apps/mail-notification/commands/clicked/enabled");
  mn_preferences_bind_string(command_clicked_entry, "/apps/mail-notification/commands/clicked/command");

  gtk_widget_show(GTK_WIDGET(preferences));
}

void
mn_preferences_update_values (void)
{
  if (preferences)
    {
      const char *command_new_mail;
      const char *command_clicked;
      GSList *mailboxes;
      GSList *l;
      
      command_new_mail = mn_conf_get_string("/apps/mail-notification/commands/new-mail/command");
      command_clicked = mn_conf_get_string("/apps/mail-notification/commands/clicked/command");
      
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(local_check),
				   mn_conf_get_bool("/apps/mail-notification/local/enabled"));
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(local_minutes_spin),
				mn_conf_get_int("/apps/mail-notification/local/delay/minutes"));
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(local_seconds_spin),
				mn_conf_get_int("/apps/mail-notification/local/delay/seconds"));
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(remote_check),
				   mn_conf_get_bool("/apps/mail-notification/remote/enabled"));
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(remote_minutes_spin),
				mn_conf_get_int("/apps/mail-notification/remote/delay/minutes"));
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(remote_seconds_spin),
				mn_conf_get_int("/apps/mail-notification/remote/delay/seconds"));
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(command_new_mail_check),
				   mn_conf_get_bool("/apps/mail-notification/commands/new-mail/enabled"));
      gtk_entry_set_text(GTK_ENTRY(command_new_mail_entry), command_new_mail ? command_new_mail : "");
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(command_clicked_check),
				   mn_conf_get_bool("/apps/mail-notification/commands/clicked/enabled"));
      gtk_entry_set_text(GTK_ENTRY(command_clicked_entry), command_clicked ? command_clicked : "");

      gtk_list_store_clear(store);

      mailboxes = mn_mailboxes_get();
      MN_LIST_FOREACH(l, mailboxes)
	{
	  MNMailbox *mailbox = l->data;
	  MNMailboxClass *class;
	  GtkTreeIter iter;
	  
	  class = g_type_class_peek(G_TYPE_FROM_INSTANCE(mailbox));

	  gtk_list_store_append(store, &iter);
	  gtk_list_store_set(store,
			     &iter,
			     COLUMN_OBJECT, mailbox,
			     COLUMN_MAILBOX, mailbox->name,
			     COLUMN_FORMAT, class->format,
			     -1);
	}
      mn_objects_free(mailboxes);
    }
}

static void
mn_preferences_update_sensitivity (void)
{
  gboolean local_check_enabled;
  gboolean remote_check_enabled;
  gboolean command_new_mail_enabled;
  gboolean command_clicked_enabled;
  GtkTreeSelection *selection;
  gboolean has_selection;

  local_check_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(local_check));

  gtk_widget_set_sensitive(local_minutes_spin, local_check_enabled);
  gtk_widget_set_sensitive(local_minutes_label, local_check_enabled);
  gtk_widget_set_sensitive(local_seconds_spin, local_check_enabled);
  gtk_widget_set_sensitive(local_seconds_label, local_check_enabled);

  remote_check_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(remote_check));

  gtk_widget_set_sensitive(remote_minutes_spin, remote_check_enabled);
  gtk_widget_set_sensitive(remote_minutes_label, remote_check_enabled);
  gtk_widget_set_sensitive(remote_seconds_spin, remote_check_enabled);
  gtk_widget_set_sensitive(remote_seconds_label, remote_check_enabled);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
  has_selection = gtk_tree_selection_count_selected_rows(selection) > 0;

  gtk_widget_set_sensitive(remove, has_selection);

  command_new_mail_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(command_new_mail_check));
  command_clicked_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(command_clicked_check));

  gtk_widget_set_sensitive(command_new_mail_entry, command_new_mail_enabled);
  gtk_widget_set_sensitive(command_clicked_entry, command_clicked_enabled);
}

static void
mn_preferences_bind_boolean (GtkWidget *widget, gpointer key)
{
  g_signal_connect(G_OBJECT(widget),
		   "toggled",
		   G_CALLBACK(mn_preferences_boolean_toggled),
		   key);
}

static void
mn_preferences_boolean_toggled (GtkToggleButton *button, gpointer user_data)
{
  const char *key = user_data;
  mn_conf_set_bool(key, gtk_toggle_button_get_active(button));
}

static void
mn_preferences_bind_int (GtkWidget *widget, gpointer key)
{
  g_signal_connect(G_OBJECT(widget),
		   "value-changed",
		   G_CALLBACK(mn_preferences_int_changed),
		   key);
}

static void
mn_preferences_int_changed (GtkSpinButton *button, gpointer user_data)
{
  const char *key = user_data;
  mn_conf_set_int(key, gtk_spin_button_get_value(button));
}

static void
mn_preferences_bind_string (GtkWidget *widget, gpointer key)
{
  g_signal_connect(G_OBJECT(widget),
		   "changed",
		   G_CALLBACK(mn_preferences_string_changed),
		   key);
}

static void
mn_preferences_string_changed (GtkEditable *editable, gpointer user_data)
{
  const char *key = user_data;
  const char *str;

  str = gtk_editable_get_chars(editable, 0, -1);
  if (*str)
    mn_conf_set_string(key, gtk_editable_get_chars(editable, 0, -1));
  else
    mn_conf_unset(key);
}

static void
mn_preferences_selection_changed (GtkTreeSelection *selection,
				  gpointer user_data)
{
  mn_preferences_update_sensitivity();
}

static void
mn_preferences_add_local_mailbox (void)
{
  GtkWidget *file_selection;
  static char *path = NULL;
  
  file_selection = gtk_file_selection_new(_("Select One or More Mailboxes"));
  gtk_window_set_transient_for(GTK_WINDOW(file_selection), GTK_WINDOW(preferences));

  if (path)
    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file_selection), path);

  gtk_file_selection_set_select_multiple(GTK_FILE_SELECTION(file_selection), TRUE);

  if (gtk_dialog_run(GTK_DIALOG(file_selection)) == GTK_RESPONSE_OK)
    {
      char **selections;
      GSList *gconf_mailboxes;
      int i;
      char *tmp_path;

      /* remember path for next time */
      tmp_path = g_path_get_dirname(gtk_file_selection_get_filename(GTK_FILE_SELECTION(file_selection)));
      g_free(path);
      path = g_strconcat(tmp_path, "/", NULL);
      g_free(tmp_path);

      selections = gtk_file_selection_get_selections(GTK_FILE_SELECTION(file_selection));
      gconf_mailboxes = mn_conf_get_list("/apps/mail-notification/mailboxes",
					 GCONF_VALUE_STRING);

      for (i = 0; selections[i]; i++)
	{
	  char *mailbox;

	  mailbox = g_filename_to_utf8(selections[i], -1, NULL, NULL, NULL);
	  gconf_mailboxes = g_slist_append(gconf_mailboxes, mailbox);
	}

      g_strfreev(selections);
      
      mn_conf_set_list("/apps/mail-notification/mailboxes",
		       GCONF_VALUE_STRING,
		       gconf_mailboxes);
      mn_slist_free(gconf_mailboxes);
    }
  
  gtk_widget_destroy(file_selection);
}

static void
mn_preferences_add_remote_mailbox (void)
{
  GladeXML *xml;
  GtkWidget *dialog;
  GtkSizeGroup *size_group;
  GtkWidget *hostname_label;
  GtkWidget *port_label;
  GtkWidget *username_label;
  GtkWidget *password_label;
  
  xml = mn_glade_xml_new("add-pop3");

  dialog = glade_xml_get_widget(xml, "dialog");
  hostname_label = glade_xml_get_widget(xml, "hostname_label");
  port_label = glade_xml_get_widget(xml, "port_label");
  username_label = glade_xml_get_widget(xml, "username_label");
  password_label = glade_xml_get_widget(xml, "password_label");

  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget(size_group, hostname_label);
  gtk_size_group_add_widget(size_group, port_label);
  gtk_size_group_add_widget(size_group, username_label);
  gtk_size_group_add_widget(size_group, password_label);

  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(preferences));

 run:
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
    {
      GtkWidget *hostname_entry;
      GtkWidget *port_spin;
      GtkWidget *username_entry;
      GtkWidget *password_entry;
      const char *hostname;
      int port;
      const char *username;
      const char *password;
      char *locator;
      GSList *gconf_mailboxes;

      hostname_entry = glade_xml_get_widget(xml, "hostname_entry");
      port_spin = glade_xml_get_widget(xml, "port_spin");
      username_entry = glade_xml_get_widget(xml, "username_entry");
      password_entry = glade_xml_get_widget(xml, "password_entry");

      hostname = gtk_editable_get_chars(GTK_EDITABLE(hostname_entry), 0, -1);
      port = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(port_spin));
      username = gtk_editable_get_chars(GTK_EDITABLE(username_entry), 0, -1);
      password = gtk_editable_get_chars(GTK_EDITABLE(password_entry), 0, -1);

      if (! *hostname)
	{
	  mn_error_dialog(_("Unable to add mailbox."),
			  _("The hostname field must be filled."));
	  goto run;
	}
      
      if (! *username)
	{
	  mn_error_dialog(_("Unable to add mailbox."),
			  _("The username field must be filled."));
	  goto run;
	}

      if (! *password)
	{
	  mn_error_dialog(_("Unable to add mailbox."),
			  _("The password field must be filled."));
	  goto run;
	}

      locator = g_strdup_printf("pop3:%s:%s@%s:%i", username, password, hostname, port);

      gconf_mailboxes = mn_conf_get_list("/apps/mail-notification/mailboxes",
					 GCONF_VALUE_STRING);
      gconf_mailboxes = g_slist_append(gconf_mailboxes, locator);
      mn_conf_set_list("/apps/mail-notification/mailboxes",
		       GCONF_VALUE_STRING,
		       gconf_mailboxes);
      mn_slist_free(gconf_mailboxes); /* this frees locator aswell */
    }
  
  gtk_widget_destroy(dialog);
  g_object_unref(xml);
}

static void
mn_preferences_remove_mailbox (void)
{
  GtkTreeSelection *selection;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
  gtk_tree_selection_selected_foreach(selection, mn_preferences_remove_mailbox_cb, NULL);
}

static void
mn_preferences_remove_mailbox_cb (GtkTreeModel *model,
				  GtkTreePath *path,
				  GtkTreeIter *iter,
				  gpointer data)
{
  MNMailbox *mailbox;

  gtk_tree_model_get(model, iter, COLUMN_OBJECT, &mailbox, -1);
  mn_conf_remove_mailbox(mailbox->locator);
  g_object_unref(mailbox);
}

static gboolean
mn_preferences_search_equal_func (GtkTreeModel *model,
				  int column,
				  const char *key,
				  GtkTreeIter *iter,
				  gpointer search_data)
{
  char *mailbox;
  char *format;
  gboolean equal = FALSE;

  gtk_tree_model_get(model, iter,
		     COLUMN_MAILBOX, &mailbox,
		     COLUMN_FORMAT, &format,
		     -1);

  equal = mn_utf8_strcasecontains(mailbox, key) || mn_utf8_strcasecontains(format, key);
  
  g_free(mailbox);
  g_free(format);

  return ! equal;
}

/* libglade callbacks */

void
mn_preferences_toggled_h (GtkToggleButton *togglebutton, gpointer user_data)
{
  mn_preferences_update_sensitivity();
}

void
mn_preferences_add_local_clicked_h (GtkButton *button, gpointer user_data)
{
  mn_preferences_add_local_mailbox();
}

void
mn_preferences_add_remote_clicked_h (GtkButton *button, gpointer user_data)
{
  mn_preferences_add_remote_mailbox();
}

void
mn_preferences_remove_clicked_h (GtkButton *button, gpointer user_data)
{
  mn_preferences_remove_mailbox();
}

void
mn_preferences_response_h (GtkDialog *dialog, int response, gpointer user_data)
{
  if (response == GTK_RESPONSE_CLOSE)
    gtk_widget_destroy(GTK_WIDGET(preferences));
}

void
mn_preferences_destroy_h (GtkWidget *widget, gpointer user_data)
{
  g_object_unref(preferences_xml);
  preferences_xml = NULL;
}
