/* 
 * Copyright (C) 2003-2005 Jean-Yves Lefort <jylefort@brutele.be>
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
#include "mn-properties-dialog.h"
#include "mn-util.h"
#include "mn-mailbox-view.h"
#include "mn-shell.h"
#include "mn-main-window.h"

/*** types *******************************************************************/

typedef struct
{
  GtkWidget		*dialog;
  GtkWidget		*notebook;

  /* general tab */
  GtkWidget		*autostart_check;
  GtkWidget		*blink_check;
  GtkWidget		*summary_tooltip_check;
  GtkWidget		*command_mail_reader_check;
  GtkWidget		*command_mail_reader_entry;
  GtkWidget		*command_new_mail_check;
  GtkWidget		*command_new_mail_entry;
  GtkWidget		*command_mail_read_check;
  GtkWidget		*command_mail_read_entry;
  GtkWidget		*action_main_window_radio;
  GtkWidget		*action_mail_reader_radio;
  GtkWidget		*action_update_radio;

  /* mailboxes tab */
  GtkWidget		*delay_label;
  GtkWidget		*minutes_spin;
  GtkWidget		*minutes_label;
  GtkWidget		*seconds_spin;
  GtkWidget		*seconds_label;
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
  GtkWidget		*summary_only_recent_check;
  GtkWidget		*summary_position_label;
  GtkWidget		*summary_position_combo;
  GtkWidget		*summary_horizontal_offset_label;
  GtkWidget		*summary_horizontal_offset_spin;
  GtkWidget		*summary_horizontal_pixels_label;
  GtkWidget		*summary_vertical_offset_label;
  GtkWidget		*summary_vertical_offset_spin;
  GtkWidget		*summary_vertical_pixels_label;
  GtkWidget		*summary_fonts_from_theme_radio;
  GtkWidget		*summary_custom_fonts_radio;
  GtkWidget		*summary_title_font_alignment;
  GtkWidget		*summary_title_font_label;
  GtkWidget		*summary_title_font_button;
  GtkWidget		*summary_contents_font_alignment;
  GtkWidget		*summary_contents_font_label;
  GtkWidget		*summary_contents_font_button;

  GtkTooltips		*tooltips;
} PropertiesDialog;

enum {
  POSITION_COLUMN_NICK,
  POSITION_COLUMN_LABEL,
  POSITION_N_COLUMNS
};

/*** variables ***************************************************************/

static PropertiesDialog self = { NULL };
  
/*** functions ***************************************************************/

static void mn_properties_dialog_weak_notify_cb (gpointer data,
						 GObject *former_dialog);

static void mn_properties_dialog_add_position (GtkListStore *store,
					       MNPosition position,
					       const char *label);

static void mn_properties_dialog_update_selected_label (void);
static void mn_properties_dialog_update_sensitivity (void);

static void mn_properties_dialog_selection_changed_h (GtkTreeSelection *selection,
						      gpointer user_data);

/*** implementation **********************************************************/

void
mn_properties_dialog_display (void)
{
  GtkSizeGroup *size_group;
  GtkTreeSelection *selection;
  GtkListStore *position_store;
  GtkCellRenderer *renderer;

  g_return_if_fail(mn_shell != NULL);

  if (self.dialog)
    {
      gtk_window_present(GTK_WINDOW(self.dialog));
      return;
    }
  
  mn_create_interface(MN_INTERFACE_FILE("properties.glade"),
		      "dialog", &self.dialog,
		      "notebook", &self.notebook,
		      "autostart_check", &self.autostart_check,
		      "blink_check", &self.blink_check,
		      "summary_tooltip_check", &self.summary_tooltip_check,
		      "command_mail_reader_check", &self.command_mail_reader_check,
		      "command_mail_reader_entry", &self.command_mail_reader_entry,
		      "command_new_mail_check", &self.command_new_mail_check,
		      "command_new_mail_entry", &self.command_new_mail_entry,
		      "command_mail_read_check", &self.command_mail_read_check,
		      "command_mail_read_entry", &self.command_mail_read_entry,
		      "action_main_window_radio", &self.action_main_window_radio,
		      "action_mail_reader_radio", &self.action_mail_reader_radio,
		      "action_update_radio", &self.action_update_radio,
		      "delay_label", &self.delay_label,
		      "minutes_spin", &self.minutes_spin,
		      "minutes_label", &self.minutes_label,
		      "seconds_spin", &self.seconds_spin,
		      "seconds_label", &self.seconds_label,
		      "scrolled", &self.scrolled,
		      "selected_label", &self.selected_label,
		      "remove", &self.remove,
		      "properties", &self.properties,
		      "summary_enable_check", &self.summary_enable_check,
		      "summary_autoclose_check", &self.summary_autoclose_check,
		      "summary_minutes_spin", &self.summary_minutes_spin,
		      "summary_minutes_label", &self.summary_minutes_label,
		      "summary_seconds_spin", &self.summary_seconds_spin,
		      "summary_seconds_label", &self.summary_seconds_label,
		      "summary_only_recent_check", &self.summary_only_recent_check,
		      "summary_position_label", &self.summary_position_label,
		      "summary_position_combo", &self.summary_position_combo,
		      "summary_horizontal_offset_label", &self.summary_horizontal_offset_label,
		      "summary_horizontal_offset_spin", &self.summary_horizontal_offset_spin,
		      "summary_horizontal_pixels_label", &self.summary_horizontal_pixels_label,
		      "summary_vertical_offset_label", &self.summary_vertical_offset_label,
		      "summary_vertical_offset_spin", &self.summary_vertical_offset_spin,
		      "summary_vertical_pixels_label", &self.summary_vertical_pixels_label,
		      "summary_fonts_from_theme_radio", &self.summary_fonts_from_theme_radio,
		      "summary_custom_fonts_radio", &self.summary_custom_fonts_radio,
		      "summary_title_font_alignment", &self.summary_title_font_alignment,
		      "summary_title_font_label", &self.summary_title_font_label,
		      "summary_title_font_button", &self.summary_title_font_button,
		      "summary_contents_font_alignment", &self.summary_contents_font_alignment,
		      "summary_contents_font_label", &self.summary_contents_font_label,
		      "summary_contents_font_button", &self.summary_contents_font_button,
		      NULL);

  eel_add_weak_pointer(&self.dialog);
  g_object_weak_ref(G_OBJECT(self.dialog), mn_properties_dialog_weak_notify_cb, NULL);

  self.tooltips = gtk_tooltips_new();
  mn_gtk_object_ref_and_sink(GTK_OBJECT(self.tooltips));
  eel_add_weak_pointer(&self.tooltips);

  self.list = mn_mailbox_view_new();
  gtk_container_add(GTK_CONTAINER(self.scrolled), self.list);
  gtk_widget_show(self.list);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self.list));
  g_signal_connect(selection, "changed", G_CALLBACK(mn_properties_dialog_selection_changed_h), NULL);
  
  mn_setup_dnd(self.scrolled);

  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget(size_group, self.command_mail_reader_check);
  gtk_size_group_add_widget(size_group, self.command_new_mail_check);
  gtk_size_group_add_widget(size_group, self.command_mail_read_check);
  g_object_unref(size_group);
  
  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget(size_group, self.summary_autoclose_check);
  gtk_size_group_add_widget(size_group, self.summary_position_label);
  gtk_size_group_add_widget(size_group, self.summary_horizontal_offset_label);
  gtk_size_group_add_widget(size_group, self.summary_vertical_offset_label);
  gtk_size_group_add_widget(size_group, self.summary_title_font_alignment);
  gtk_size_group_add_widget(size_group, self.summary_contents_font_alignment);
  g_object_unref(size_group);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self.autostart_check), mn_conf_get_autostart());

  position_store = gtk_list_store_new(POSITION_N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);

  mn_properties_dialog_add_position(position_store, MN_POSITION_TOP_LEFT, _("top left"));
  mn_properties_dialog_add_position(position_store, MN_POSITION_TOP_RIGHT, _("top right"));
  mn_properties_dialog_add_position(position_store, MN_POSITION_BOTTOM_LEFT, _("bottom left"));
  mn_properties_dialog_add_position(position_store, MN_POSITION_BOTTOM_RIGHT, _("bottom right"));

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(self.summary_position_combo), renderer, TRUE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(self.summary_position_combo), renderer,
				 "text", POSITION_COLUMN_LABEL,
				 NULL);

  gtk_combo_box_set_model(GTK_COMBO_BOX(self.summary_position_combo), GTK_TREE_MODEL(position_store));
  g_object_unref(position_store);
  
  mn_conf_link(self.dialog, MN_CONF_PROPERTIES_DIALOG,
	       self.blink_check, MN_CONF_BLINK_ON_ERRORS, "active",
	       self.summary_tooltip_check, MN_CONF_SUMMARY_TOOLTIP, "active",
	       self.command_mail_reader_check, MN_CONF_COMMANDS_MAIL_READER_ENABLED, "active",
	       self.command_mail_reader_entry, MN_CONF_COMMANDS_MAIL_READER_COMMAND, "text",
	       self.command_new_mail_check, MN_CONF_COMMANDS_NEW_MAIL_ENABLED, "active",
	       self.command_new_mail_entry, MN_CONF_COMMANDS_NEW_MAIL_COMMAND, "text",
	       self.command_mail_read_check, MN_CONF_COMMANDS_MAIL_READ_ENABLED, "active",
	       self.command_mail_read_entry, MN_CONF_COMMANDS_MAIL_READ_COMMAND, "text",
	       self.minutes_spin, MN_CONF_DELAY_MINUTES,
	       self.seconds_spin, MN_CONF_DELAY_SECONDS,
	       self.summary_enable_check, MN_CONF_MAIL_SUMMARY_POPUP_ENABLE, "active",
	       self.summary_autoclose_check, MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE, "active",
	       self.summary_minutes_spin, MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_MINUTES,
	       self.summary_seconds_spin, MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_SECONDS,
	       self.summary_only_recent_check, MN_CONF_MAIL_SUMMARY_POPUP_ONLY_RECENT, "active",
	       self.summary_horizontal_offset_spin, MN_CONF_MAIL_SUMMARY_POPUP_HORIZONTAL_OFFSET,
	       self.summary_vertical_offset_spin, MN_CONF_MAIL_SUMMARY_POPUP_VERTICAL_OFFSET,
	       self.summary_title_font_button, MN_CONF_MAIL_SUMMARY_POPUP_FONTS_TITLE_FONT, "font-name",
	       self.summary_contents_font_button, MN_CONF_MAIL_SUMMARY_POPUP_FONTS_CONTENTS_FONT, "font-name",
	       NULL);
  mn_conf_link_radio_group_to_enum(MN_TYPE_ACTION,
				   MN_CONF_DOUBLE_CLICK_ACTION_2,
				   self.action_main_window_radio, MN_ACTION_DISPLAY_MAIN_WINDOW,
				   self.action_mail_reader_radio, MN_ACTION_LAUNCH_MAIL_READER,
				   self.action_update_radio, MN_ACTION_UPDATE_MAIL_STATUS,
				   NULL);
  mn_conf_link_radio_group_to_enum(MN_TYPE_ASPECT_SOURCE,
				   MN_CONF_MAIL_SUMMARY_POPUP_FONTS_ASPECT_SOURCE,
				   self.summary_fonts_from_theme_radio, MN_ASPECT_SOURCE_THEME,
				   self.summary_custom_fonts_radio, MN_ASPECT_SOURCE_CUSTOM,
				   NULL);
  mn_conf_link_combo_box_to_string(GTK_COMBO_BOX(self.summary_position_combo),
				   POSITION_COLUMN_NICK,
				   MN_CONF_MAIL_SUMMARY_POPUP_POSITION);

  mn_properties_dialog_update_selected_label();
  mn_properties_dialog_update_sensitivity();

  mn_g_object_connect(self.dialog,
		      mn_shell->mailboxes,
		      "signal::notify::must-poll", mn_properties_dialog_update_sensitivity, NULL,
		      NULL);

  mn_main_window_add_transient(GTK_WINDOW(self.dialog));
  gtk_widget_show(self.dialog);
}

static void
mn_properties_dialog_weak_notify_cb (gpointer data, GObject *former_dialog)
{
  g_object_unref(self.tooltips);
}

static void
mn_properties_dialog_add_position (GtkListStore *store,
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
mn_properties_dialog_update_selected_label (void)
{
  GtkTreeSelection *selection;
  int n_rows;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self.list));
  n_rows = gtk_tree_selection_count_selected_rows(selection);

  if (n_rows == 0)
    gtk_label_set_text(GTK_LABEL(self.selected_label), _("No mailbox selected."));
  else
    {
      char *str;

      str = g_strdup_printf(ngettext("%i mailbox selected.",
				     "%i mailboxes selected.",
				     n_rows), n_rows);
      gtk_label_set_text(GTK_LABEL(self.selected_label), str);
      g_free(str);
    }
}

static void
mn_properties_dialog_update_sensitivity (void)
{
  gboolean command_mail_reader_enabled;
  gboolean command_new_mail_enabled;
  gboolean command_mail_read_enabled;
  gboolean must_poll;
  GtkTreeSelection *selection;
  gboolean has_selection;
  gboolean summary_enabled;
  gboolean summary_autoclose_enabled;
  gboolean summary_custom_fonts_enabled;

  command_mail_reader_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self.command_mail_reader_check));
  command_new_mail_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self.command_new_mail_check));
  command_mail_read_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self.command_mail_read_check));

  gtk_widget_set_sensitive(self.command_mail_reader_entry, command_mail_reader_enabled);
  gtk_widget_set_sensitive(self.command_new_mail_entry, command_new_mail_enabled);
  gtk_widget_set_sensitive(self.command_mail_read_entry, command_mail_read_enabled);

  must_poll = mn_mailboxes_get_must_poll(mn_shell->mailboxes);
  gtk_widget_set_sensitive(self.delay_label, must_poll);
  gtk_widget_set_sensitive(self.minutes_spin, must_poll);
  gtk_widget_set_sensitive(self.minutes_label, must_poll);
  gtk_widget_set_sensitive(self.seconds_spin, must_poll);
  gtk_widget_set_sensitive(self.seconds_label, must_poll);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self.list));
  has_selection = gtk_tree_selection_count_selected_rows(selection) > 0;

  gtk_widget_set_sensitive(self.remove, has_selection);
  gtk_widget_set_sensitive(self.properties, has_selection);

  summary_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self.summary_enable_check));
  summary_autoclose_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self.summary_autoclose_check));
  summary_custom_fonts_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self.summary_custom_fonts_radio));

  gtk_widget_set_sensitive(self.summary_autoclose_check, summary_enabled);
  gtk_widget_set_sensitive(self.summary_minutes_spin, summary_enabled && summary_autoclose_enabled);
  gtk_widget_set_sensitive(self.summary_minutes_label, summary_enabled && summary_autoclose_enabled);
  gtk_widget_set_sensitive(self.summary_seconds_spin, summary_enabled && summary_autoclose_enabled);
  gtk_widget_set_sensitive(self.summary_seconds_label, summary_enabled && summary_autoclose_enabled);
  gtk_widget_set_sensitive(self.summary_only_recent_check, summary_enabled);
  gtk_widget_set_sensitive(self.summary_position_label, summary_enabled);
  gtk_widget_set_sensitive(self.summary_position_combo, summary_enabled);
  gtk_widget_set_sensitive(self.summary_horizontal_offset_label, summary_enabled);
  gtk_widget_set_sensitive(self.summary_horizontal_offset_spin, summary_enabled);
  gtk_widget_set_sensitive(self.summary_horizontal_pixels_label, summary_enabled);
  gtk_widget_set_sensitive(self.summary_vertical_offset_label, summary_enabled);
  gtk_widget_set_sensitive(self.summary_vertical_offset_spin, summary_enabled);
  gtk_widget_set_sensitive(self.summary_vertical_pixels_label, summary_enabled);
  gtk_widget_set_sensitive(self.summary_fonts_from_theme_radio, summary_enabled);
  gtk_widget_set_sensitive(self.summary_custom_fonts_radio, summary_enabled);
  gtk_widget_set_sensitive(self.summary_title_font_label, summary_enabled && summary_custom_fonts_enabled);
  gtk_widget_set_sensitive(self.summary_title_font_button, summary_enabled && summary_custom_fonts_enabled);
  gtk_widget_set_sensitive(self.summary_contents_font_label, summary_enabled && summary_custom_fonts_enabled);
  gtk_widget_set_sensitive(self.summary_contents_font_button, summary_enabled && summary_custom_fonts_enabled);
}

static void
mn_properties_dialog_selection_changed_h (GtkTreeSelection *selection,
					  gpointer user_data)
{
  mn_properties_dialog_update_selected_label();
  mn_properties_dialog_update_sensitivity();
}

void
mn_properties_dialog_set_tooltip (GtkWidget *widget, const char *tip)
{
  g_return_if_fail(self.tooltips != NULL);
  g_return_if_fail(GTK_IS_WIDGET(widget));

  gtk_tooltips_set_tip(self.tooltips, widget, tip, NULL);
}

void
mn_properties_dialog_set_tooltips (GtkWidget *widget, ...)
{
  va_list args;

  g_return_if_fail(self.tooltips != NULL);

  va_start(args, widget);
  while (widget)
    {
      const char *tip;

      g_return_if_fail(GTK_IS_WIDGET(widget));

      tip = va_arg(args, const char *);

      mn_properties_dialog_set_tooltip(widget, tip);

      widget = va_arg(args, GtkWidget *);
    }
  va_end(args);
}

/* libglade callbacks */

void
mn_properties_dialog_autostart_toggled_h (GtkToggleButton *togglebutton,
					  gpointer user_data)
{
  mn_conf_set_autostart(gtk_toggle_button_get_active(togglebutton));
}

void
mn_properties_dialog_toggled_h (GtkToggleButton *togglebutton,
				gpointer user_data)
{
  mn_properties_dialog_update_sensitivity();
}

void
mn_properties_dialog_add_clicked_h (GtkButton *button, gpointer user_data)
{
  mn_mailbox_view_activate_add(MN_MAILBOX_VIEW(self.list));
}

void
mn_properties_dialog_remove_clicked_h (GtkButton *button, gpointer user_data)
{
  mn_mailbox_view_activate_remove(MN_MAILBOX_VIEW(self.list));
}

void
mn_properties_dialog_properties_clicked_h (GtkButton *button,
					   gpointer user_data)
{
  mn_mailbox_view_activate_properties(MN_MAILBOX_VIEW(self.list));
}

gboolean
mn_properties_dialog_scrolled_drag_motion_h (GtkWidget *widget,
					     GdkDragContext *drag_context,
					     int x,
					     int y,
					     unsigned int time,
					     gpointer user_data)
{
  GtkAdjustment *adjustment;
  
  adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(self.scrolled));
  gtk_adjustment_set_value(adjustment, (double) y / (widget->allocation.height - 2) * (adjustment->upper - adjustment->page_size));

  return TRUE;
}

void
mn_properties_dialog_response_h (GtkDialog *dialog,
				 int response,
				 gpointer user_data)
{
  switch (response)
    {
    case GTK_RESPONSE_HELP:
      {
	int current_page;
	const char *sections[] = {
	  "properties-general",
	  "properties-mailboxes",
	  "properties-mail-summary-popup"
	};
	
	current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self.notebook));
	g_return_if_fail(current_page >= 0 && current_page < G_N_ELEMENTS(sections));

	mn_display_help(GTK_WINDOW(dialog), sections[current_page]);
	break;
      }
    
    case GTK_RESPONSE_CLOSE:
      gtk_widget_destroy(self.dialog);
      break;
    }
}
