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
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <eel/eel.h>
#include "mn-conf.h"
#include "mn-properties.h"
#include "mn-util.h"
#include "mn-mailbox-view.h"
#include "mn-shell.h"

/*** types *******************************************************************/

typedef struct
{
  GtkWidget		*dialog;
  GtkWidget		*notebook;

  /* general tab */
  GtkWidget		*autostart_check;
  GtkWidget		*blink_check;
  GtkWidget		*command_new_mail_check;
  GtkWidget		*command_new_mail_entry;
  GtkWidget		*command_mail_read_check;
  GtkWidget		*command_mail_read_entry;

  /* mailboxes tab */
  GtkWidget		*delay_label;
  GtkWidget		*minutes_spin;
  GtkWidget		*seconds_spin;
  GtkWidget		*scrolled;
  GtkWidget		*list;
  GtkWidget		*selected_label;
  GtkWidget		*remove;
  GtkWidget		*properties;

  /* mail summary popup tab */
  GtkWidget		*summary_enable_check;
  GtkWidget		*summary_autoclose_check;
  GtkWidget		*summary_minutes_spin;
  GtkWidget		*summary_minutes_label;
  GtkWidget		*summary_seconds_spin;
  GtkWidget		*summary_seconds_label;
  GtkWidget		*summary_position_label;
  GtkWidget		*summary_position_combo;
  GtkWidget		*summary_horizontal_offset_label;
  GtkWidget		*summary_horizontal_offset_spin;
  GtkWidget		*summary_horizontal_pixels_label;
  GtkWidget		*summary_vertical_offset_label;
  GtkWidget		*summary_vertical_offset_spin;
  GtkWidget		*summary_vertical_pixels_label;
} PropertiesDialog;

enum {
  POSITION_COLUMN_NICK,
  POSITION_COLUMN_LABEL,
  POSITION_N_COLUMNS
};

/*** variables ***************************************************************/

static PropertiesDialog properties = { NULL };
  
/*** functions ***************************************************************/

static void mn_properties_add_position (GtkListStore *store,
					MNPosition position,
					const char *label);

static void mn_properties_update_selected_label (void);
static void mn_properties_update_sensitivity (void);

static void mn_properties_selection_changed_h (GtkTreeSelection *selection,
					       gpointer user_data);

/*** implementation **********************************************************/

void
mn_properties_display (void)
{
  GtkSizeGroup *size_group;
  GtkTreeSelection *selection;
  GtkListStore *position_store;
  GtkCellRenderer *renderer;

  g_return_if_fail(mn_shell != NULL);

  if (properties.dialog)
    {
      gtk_window_present(GTK_WINDOW(properties.dialog));
      return;
    }
  
  mn_create_interface("properties",
		      "dialog", &properties.dialog,
		      "notebook", &properties.notebook,
		      "autostart_check", &properties.autostart_check,
		      "blink_check", &properties.blink_check,
		      "command_new_mail_check", &properties.command_new_mail_check,
		      "command_new_mail_entry", &properties.command_new_mail_entry,
		      "command_mail_read_check", &properties.command_mail_read_check,
		      "command_mail_read_entry", &properties.command_mail_read_entry,
		      "delay_label", &properties.delay_label,
		      "minutes_spin", &properties.minutes_spin,
		      "seconds_spin", &properties.seconds_spin,
		      "scrolled", &properties.scrolled,
		      "selected_label", &properties.selected_label,
		      "remove", &properties.remove,
		      "properties", &properties.properties,
		      "summary_enable_check", &properties.summary_enable_check,
		      "summary_autoclose_check", &properties.summary_autoclose_check,
		      "summary_minutes_spin", &properties.summary_minutes_spin,
		      "summary_minutes_label", &properties.summary_minutes_label,
		      "summary_seconds_spin", &properties.summary_seconds_spin,
		      "summary_seconds_label", &properties.summary_seconds_label,
		      "summary_position_label", &properties.summary_position_label,
		      "summary_position_combo", &properties.summary_position_combo,
		      "summary_horizontal_offset_label", &properties.summary_horizontal_offset_label,
		      "summary_horizontal_offset_spin", &properties.summary_horizontal_offset_spin,
		      "summary_horizontal_pixels_label", &properties.summary_horizontal_pixels_label,
		      "summary_vertical_offset_label", &properties.summary_vertical_offset_label,
		      "summary_vertical_offset_spin", &properties.summary_vertical_offset_spin,
		      "summary_vertical_pixels_label", &properties.summary_vertical_pixels_label,
		      NULL);

  eel_add_weak_pointer(&properties.dialog);

  properties.list = mn_mailbox_view_new();
  gtk_container_add(GTK_CONTAINER(properties.scrolled), properties.list);
  gtk_widget_show(properties.list);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(properties.list));
  g_signal_connect(selection, "changed", G_CALLBACK(mn_properties_selection_changed_h), NULL);
  
  mn_setup_dnd(properties.scrolled);

  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget(size_group, properties.command_new_mail_check);
  gtk_size_group_add_widget(size_group, properties.command_mail_read_check);
  g_object_unref(size_group);
  
  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget(size_group, properties.summary_autoclose_check);
  gtk_size_group_add_widget(size_group, properties.summary_position_label);
  gtk_size_group_add_widget(size_group, properties.summary_horizontal_offset_label);
  gtk_size_group_add_widget(size_group, properties.summary_vertical_offset_label);
  g_object_unref(size_group);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(properties.autostart_check), mn_conf_get_autostart());

  position_store = gtk_list_store_new(POSITION_N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);

  mn_properties_add_position(position_store, MN_POSITION_TOP_LEFT, _("top left"));
  mn_properties_add_position(position_store, MN_POSITION_TOP_RIGHT, _("top right"));
  mn_properties_add_position(position_store, MN_POSITION_BOTTOM_LEFT, _("bottom left"));
  mn_properties_add_position(position_store, MN_POSITION_BOTTOM_RIGHT, _("bottom right"));

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(properties.summary_position_combo), renderer, TRUE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(properties.summary_position_combo), renderer,
				 "text", POSITION_COLUMN_LABEL,
				 NULL);

  gtk_combo_box_set_model(GTK_COMBO_BOX(properties.summary_position_combo), GTK_TREE_MODEL(position_store));
  g_object_unref(position_store);
  
  mn_conf_link(properties.dialog, MN_CONF_PROPERTIES_DIALOG,
	       properties.blink_check, MN_CONF_BLINK_ON_ERRORS,
	       properties.command_new_mail_check, MN_CONF_COMMANDS_NEW_MAIL_ENABLED,
	       properties.command_new_mail_entry, MN_CONF_COMMANDS_NEW_MAIL_COMMAND,
	       properties.command_mail_read_check, MN_CONF_COMMANDS_MAIL_READ_ENABLED,
	       properties.command_mail_read_entry, MN_CONF_COMMANDS_MAIL_READ_COMMAND,
	       properties.minutes_spin, MN_CONF_DELAY_MINUTES,
	       properties.seconds_spin, MN_CONF_DELAY_SECONDS,
	       properties.summary_enable_check, MN_CONF_MAIL_SUMMARY_POPUP_ENABLE,
	       properties.summary_autoclose_check, MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE,
	       properties.summary_minutes_spin, MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_MINUTES,
	       properties.summary_seconds_spin, MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_SECONDS,
	       properties.summary_horizontal_offset_spin, MN_CONF_MAIL_SUMMARY_POPUP_HORIZONTAL_OFFSET,
	       properties.summary_vertical_offset_spin, MN_CONF_MAIL_SUMMARY_POPUP_VERTICAL_OFFSET,
	       NULL);
  mn_conf_link_combo_box_to_string(GTK_COMBO_BOX(properties.summary_position_combo),
				   POSITION_COLUMN_NICK,
				   MN_CONF_MAIL_SUMMARY_POPUP_POSITION);

  mn_properties_update_selected_label();
  mn_properties_update_sensitivity();

  mn_g_object_connect(properties.dialog,
		      mn_shell->mailboxes,
		      "signal::notify::must-poll", mn_properties_update_sensitivity, NULL,
		      NULL);

  gtk_widget_show(properties.dialog);
}

static void
mn_properties_add_position (GtkListStore *store,
			    MNPosition position,
			    const char *label)
{
  GEnumClass *enum_class;
  GEnumValue *enum_value;
  GtkTreeIter iter;

  g_return_if_fail(GTK_IS_LIST_STORE(store));
  g_return_if_fail(label != NULL);

  enum_class = g_type_class_ref(MN_TYPE_POSITION);
  g_return_if_fail(enum_class != NULL);

  enum_value = g_enum_get_value(enum_class, position);
  g_return_if_fail(enum_value != NULL);

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter,
		     POSITION_COLUMN_NICK, enum_value->value_nick,
		     POSITION_COLUMN_LABEL, label,
		     -1);

  g_type_class_unref(enum_class);
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
  gboolean command_new_mail_enabled;
  gboolean command_mail_read_enabled;
  gboolean must_poll;
  GtkTreeSelection *selection;
  gboolean has_selection;
  gboolean summary_enabled = FALSE;
  gboolean summary_autoclose_enabled = FALSE;

  command_new_mail_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(properties.command_new_mail_check));
  command_mail_read_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(properties.command_mail_read_check));

  gtk_widget_set_sensitive(properties.command_new_mail_entry, command_new_mail_enabled);
  gtk_widget_set_sensitive(properties.command_mail_read_entry, command_mail_read_enabled);

  must_poll = mn_mailboxes_get_must_poll(mn_shell->mailboxes);
  gtk_widget_set_sensitive(properties.minutes_spin, must_poll);
  gtk_widget_set_sensitive(properties.seconds_spin, must_poll);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(properties.list));
  has_selection = gtk_tree_selection_count_selected_rows(selection) > 0;

  gtk_widget_set_sensitive(properties.remove, has_selection);
  gtk_widget_set_sensitive(properties.properties, has_selection);

  summary_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(properties.summary_enable_check));
  summary_autoclose_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(properties.summary_autoclose_check));

  gtk_widget_set_sensitive(properties.summary_autoclose_check, summary_enabled);
  gtk_widget_set_sensitive(properties.summary_minutes_spin, summary_enabled && summary_autoclose_enabled);
  gtk_widget_set_sensitive(properties.summary_minutes_label, summary_enabled && summary_autoclose_enabled);
  gtk_widget_set_sensitive(properties.summary_seconds_spin, summary_enabled && summary_autoclose_enabled);
  gtk_widget_set_sensitive(properties.summary_seconds_label, summary_enabled && summary_autoclose_enabled);
  gtk_widget_set_sensitive(properties.summary_position_label, summary_enabled);
  gtk_widget_set_sensitive(properties.summary_position_combo, summary_enabled);
  gtk_widget_set_sensitive(properties.summary_horizontal_offset_label, summary_enabled);
  gtk_widget_set_sensitive(properties.summary_horizontal_offset_spin, summary_enabled);
  gtk_widget_set_sensitive(properties.summary_horizontal_pixels_label, summary_enabled);
  gtk_widget_set_sensitive(properties.summary_vertical_offset_label, summary_enabled);
  gtk_widget_set_sensitive(properties.summary_vertical_offset_spin, summary_enabled);
  gtk_widget_set_sensitive(properties.summary_vertical_pixels_label, summary_enabled);
}

static void
mn_properties_selection_changed_h (GtkTreeSelection *selection,
				    gpointer user_data)
{
  mn_properties_update_selected_label();
  mn_properties_update_sensitivity();
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
      {
	int current_page;
	const char *sections[] = { "properties-general", "properties-mailboxes", "properties-mail-summary-popup" };
	
	current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(properties.notebook));
	g_return_if_fail(current_page >= 0 && current_page < G_N_ELEMENTS(sections));

	mn_display_help(GTK_WINDOW(dialog), sections[current_page]);
	break;
      }
    
    case GTK_RESPONSE_CLOSE:
      gtk_widget_destroy(properties.dialog);
      break;
    }
}
