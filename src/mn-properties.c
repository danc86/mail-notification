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
#include <gtk/gtk.h>
#include "mn-conf.h"
#include "mn-properties.h"
#include "mn-util.h"
#include "mn-mailbox-view.h"
#include "mn-shell.h"

/*** types *******************************************************************/

typedef struct
{
  GtkWidget		*dialog;

  GtkWidget		*delay_label;
  GtkWidget		*minutes_spin;
  GtkWidget		*seconds_spin;

  GtkWidget		*autostart_check;
  GtkWidget		*blink_check;

  GtkWidget		*scrolled;
  GtkWidget		*list;
  GtkWidget		*selected_label;
  GtkWidget		*remove;
  GtkWidget		*properties;

  GtkWidget		*command_new_mail_check;
  GtkWidget		*command_new_mail_entry;
  GtkWidget		*command_double_clicked_check;
  GtkWidget		*command_double_clicked_entry;
} PropertiesDialog;

/*** variables ***************************************************************/

static PropertiesDialog properties = { NULL };
  
/*** functions ***************************************************************/

static void mn_properties_update_selected_label (void);
static void mn_properties_update_sensitivity (void);

static void mn_properties_selection_changed_h (GtkTreeSelection *selection,
					       gpointer user_data);
static void mn_properties_mailboxes_list_changed_h (MNMailboxes *mailboxes,
						    gpointer user_data);

static void mn_properties_weak_notify_cb (gpointer data,
					  GObject *former_properties);

/*** implementation **********************************************************/

void
mn_properties_display (void)
{
  GtkSizeGroup *size_group;
  GtkTreeSelection *selection;

  g_return_if_fail(mn_shell != NULL);

  if (properties.dialog)
    {
      gtk_window_present(GTK_WINDOW(properties.dialog));
      return;
    }
  
  mn_create_interface("properties",
		      "dialog", &properties.dialog,
		      "delay_label", &properties.delay_label,
		      "minutes_spin", &properties.minutes_spin,
		      "seconds_spin", &properties.seconds_spin,
		      "autostart_check", &properties.autostart_check,
		      "blink_check", &properties.blink_check,
		      "scrolled", &properties.scrolled,
		      "selected_label", &properties.selected_label,
		      "remove", &properties.remove,
		      "properties", &properties.properties,
		      "command_new_mail_check", &properties.command_new_mail_check,
		      "command_new_mail_entry", &properties.command_new_mail_entry,
		      "command_double_clicked_check", &properties.command_double_clicked_check,
		      "command_double_clicked_entry", &properties.command_double_clicked_entry,
		      NULL);

  g_object_add_weak_pointer(G_OBJECT(properties.dialog), (gpointer *) &properties.dialog);

  properties.list = mn_mailbox_view_new();
  gtk_container_add(GTK_CONTAINER(properties.scrolled), properties.list);
  gtk_widget_show(properties.list);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(properties.list));
  g_signal_connect(G_OBJECT(selection), "changed", G_CALLBACK(mn_properties_selection_changed_h), NULL);
  
  mn_setup_dnd(properties.scrolled);

  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget(size_group, properties.delay_label);
  gtk_size_group_add_widget(size_group, properties.command_new_mail_check);
  gtk_size_group_add_widget(size_group, properties.command_double_clicked_check);
  g_object_unref(size_group);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(properties.autostart_check), mn_conf_get_autostart());

  mn_conf_link(properties.dialog, MN_CONF_PROPERTIES_DIALOG,
	       properties.minutes_spin, MN_CONF_DELAY_MINUTES,
	       properties.seconds_spin, MN_CONF_DELAY_SECONDS,
	       properties.blink_check, MN_CONF_BLINK_ON_ERRORS,
	       properties.command_new_mail_check, MN_CONF_COMMANDS_NEW_MAIL_ENABLED,
	       properties.command_new_mail_entry, MN_CONF_COMMANDS_NEW_MAIL_COMMAND,
	       properties.command_double_clicked_check, MN_CONF_COMMANDS_DOUBLE_CLICKED_ENABLED,
	       properties.command_double_clicked_entry, MN_CONF_COMMANDS_DOUBLE_CLICKED_COMMAND,
	       NULL);

  mn_properties_update_selected_label();
  mn_properties_update_sensitivity();

  g_signal_connect(G_OBJECT(mn_shell->mailboxes), "list-changed", G_CALLBACK(mn_properties_mailboxes_list_changed_h), NULL);
  g_object_weak_ref(G_OBJECT(properties.dialog), mn_properties_weak_notify_cb, NULL);

  gtk_widget_show(properties.dialog);
}

static void
mn_properties_update_selected_label (void)
{
  GtkTreeSelection *selection;
  int n_rows;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(properties.list));
  n_rows = gtk_tree_selection_count_selected_rows(selection);

  if (n_rows == 0)
    gtk_label_set_text(GTK_LABEL(properties.selected_label), _("No mailbox selected."));
  else
    {
      char *str;

      str = g_strdup_printf(ngettext("%i mailbox selected.",
				     "%i mailboxes selected.",
				     n_rows), n_rows);
      gtk_label_set_text(GTK_LABEL(properties.selected_label), str);
      g_free(str);
    }
}

static void
mn_properties_update_sensitivity (void)
{
  gboolean has_manual;
  GtkTreeSelection *selection;
  gboolean has_selection;
  gboolean command_new_mail_enabled;
  gboolean command_double_clicked_enabled;

  has_manual = mn_mailboxes_has_manual(mn_shell->mailboxes);
  gtk_widget_set_sensitive(properties.minutes_spin, has_manual);
  gtk_widget_set_sensitive(properties.seconds_spin, has_manual);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(properties.list));
  has_selection = gtk_tree_selection_count_selected_rows(selection) > 0;

  gtk_widget_set_sensitive(properties.remove, has_selection);
  gtk_widget_set_sensitive(properties.properties, has_selection);

  command_new_mail_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(properties.command_new_mail_check));
  command_double_clicked_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(properties.command_double_clicked_check));

  gtk_widget_set_sensitive(properties.command_new_mail_entry, command_new_mail_enabled);
  gtk_widget_set_sensitive(properties.command_double_clicked_entry, command_double_clicked_enabled);
}

static void
mn_properties_selection_changed_h (GtkTreeSelection *selection,
				    gpointer user_data)
{
  mn_properties_update_selected_label();
  mn_properties_update_sensitivity();
}

static void
mn_properties_mailboxes_list_changed_h (MNMailboxes *mailboxes,
					gpointer user_data)
{
  mn_properties_update_sensitivity();
}

static void
mn_properties_weak_notify_cb (gpointer data, GObject *former_properties)
{
  g_signal_handlers_disconnect_by_func(mn_shell->mailboxes, mn_properties_mailboxes_list_changed_h, NULL);
}

/* libglade callbacks */

void
mn_properties_autostart_toggled_h (GtkToggleButton *togglebutton,
				   gpointer user_data)
{
  mn_conf_set_autostart(gtk_toggle_button_get_active(togglebutton));
}

void
mn_properties_toggled_h (GtkToggleButton *togglebutton, gpointer user_data)
{
  mn_properties_update_sensitivity();
}

void
mn_properties_add_clicked_h (GtkButton *button, gpointer user_data)
{
  mn_mailbox_view_activate_add(MN_MAILBOX_VIEW(properties.list));
}

void
mn_properties_remove_clicked_h (GtkButton *button, gpointer user_data)
{
  mn_mailbox_view_activate_remove(MN_MAILBOX_VIEW(properties.list));
}

void
mn_properties_properties_clicked_h (GtkButton *button, gpointer user_data)
{
  mn_mailbox_view_activate_properties(MN_MAILBOX_VIEW(properties.list));
}

gboolean
mn_properties_scrolled_drag_motion_h (GtkWidget *widget,
				      GdkDragContext *drag_context,
				      int x,
				      int y,
				      unsigned int time,
				      gpointer user_data)
{
  GtkAdjustment *adjustment;
  
  adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(properties.scrolled));
  gtk_adjustment_set_value(adjustment, (double) y / (widget->allocation.height - 2) * (adjustment->upper - adjustment->page_size));

  return TRUE;
}

void
mn_properties_response_h (GtkDialog *dialog, int response, gpointer user_data)
{
  switch (response)
    {
    case GTK_RESPONSE_HELP:
      mn_display_help("properties");
      break;

    case GTK_RESPONSE_CLOSE:
      gtk_widget_destroy(properties.dialog);
      break;
    }
}
