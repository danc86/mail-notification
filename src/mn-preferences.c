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
#include "mn-dialog.h"
#include "mn-mailboxes.h"
#include "mn-preferences.h"
#include "mn-util.h"
#include "mn-stock.h"
#include "mn-unsupported-mailbox.h"
#include "mn-pending-mailbox.h"
#include "mn-mailbox-properties-dialog.h"
#include "mn-uri.h"

/*** cpp *********************************************************************/

#define GNOME_COPIED_FILES		"x-special/gnome-copied-files"

/*** types *******************************************************************/

enum {
  COLUMN_URI,
  COLUMN_MAILBOX_ICON,
  COLUMN_MAILBOX,
  COLUMN_FORMAT,
  N_COLUMNS
};

typedef struct
{
  GtkWidget		*dialog;

  GtkWidget		*delay_label;
  GtkWidget		*minutes_spin;
  GtkWidget		*seconds_spin;

  GtkWidget		*scrolled;
  GtkWidget		*list;
  GtkWidget		*selected_label;
  GtkWidget		*remove;
  GtkWidget		*properties;

  GtkWidget		*command_new_mail_check;
  GtkWidget		*command_new_mail_entry;
  GtkWidget		*command_clicked_check;
  GtkWidget		*command_clicked_entry;

  GtkWidget		*mailbox_menu;
  GtkWidget		*remove_item;
  GtkWidget		*cut_item;
  GtkWidget		*copy_item;
  GtkWidget		*paste_item;
  GtkWidget		*properties_item;
  GtkListStore		*store;
} PreferencesDialog;

/*** variables ***************************************************************/

static GtkClipboard *clipboard = NULL;
static GdkAtom gnome_copied_files_atom;

static PreferencesDialog preferences = { NULL };
static GtkWidget *add_mailbox = NULL;
  
/*** functions ***************************************************************/

static void mn_preferences_update_iter (GtkTreeIter *iter, MNMailbox *mailbox);
static gboolean mn_preferences_has_mailbox (MNMailbox *mailbox);

static void mn_preferences_update_selected_label (void);
static void mn_preferences_update_sensitivity (void);

static void mn_preferences_add_mailbox (void);

static void mn_preferences_remove_mailbox (void);
static void mn_preferences_remove_mailbox_cb (GtkTreeModel *model,
					      GtkTreePath *path,
					      GtkTreeIter *iter,
					      gpointer data);

static void mn_preferences_cut_mailbox (void);

static void mn_preferences_copy_mailbox (void);
static void mn_preferences_copy_mailbox_cb (GtkTreeModel *model,
					    GtkTreePath *path,
					    GtkTreeIter *iter,
					    gpointer data);

static void mn_preferences_get_clipboard_cb (GtkClipboard *clipboard,
					     GtkSelectionData *selection_data,
					     unsigned int info,
					     gpointer user_data_or_owner);
static void mn_preferences_clear_clipboard_cb (GtkClipboard *clipboard,
					       gpointer user_data_or_owner);

static void mn_preferences_paste_mailbox (void);
static void mn_preferences_paste_mailbox_receive_cb (GtkClipboard *clipboard,
						     GtkSelectionData *selection_data,
						     gpointer data);

static void mn_preferences_edit_mailbox (void);
static void mn_preferences_edit_mailbox_cb (GtkTreeModel *model,
					    GtkTreePath *path,
					    GtkTreeIter *iter,
					    gpointer data);

static void mn_preferences_selection_changed_h (GtkTreeSelection *selection,
						gpointer user_data);
static void mn_preferences_mailbox_properties_response_h (GtkDialog *dialog,
							  int response,
							  gpointer user_data);

static void mn_preferences_popup_mailbox_menu (unsigned int button,
					       guint32 activate_time);
static void mn_preferences_popup_mailbox_menu_receive_cb (GtkClipboard *clipboard,
							  GtkSelectionData *selection_data,
							  gpointer data);

/*** implementation **********************************************************/

void
mn_preferences_display (void)
{
  GtkAccelGroup *accel_group;
  GtkSizeGroup *size_group;
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;
  GtkTreeSelection *selection;

  if (! clipboard)
    {
      clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      gnome_copied_files_atom = gdk_atom_intern(GNOME_COPIED_FILES, FALSE);
    }
  
  if (preferences.dialog)
    {
      gtk_window_present(GTK_WINDOW(preferences.dialog));
      return;
    }
  
  mn_create_interface("preferences",
		      "dialog", &preferences.dialog,
		      "delay_label", &preferences.delay_label,
		      "minutes_spin", &preferences.minutes_spin,
		      "seconds_spin", &preferences.seconds_spin,
		      "scrolled", &preferences.scrolled,
		      "list", &preferences.list,
		      "selected_label", &preferences.selected_label,
		      "remove", &preferences.remove,
		      "properties", &preferences.properties,
		      "command_new_mail_check", &preferences.command_new_mail_check,
		      "command_new_mail_entry", &preferences.command_new_mail_entry,
		      "command_clicked_check", &preferences.command_clicked_check,
		      "command_clicked_entry", &preferences.command_clicked_entry,
		      "mailbox_menu", &preferences.mailbox_menu,
		      "remove_item", &preferences.remove_item,
		      "cut_item", &preferences.cut_item,
		      "copy_item", &preferences.copy_item,
		      "paste_item", &preferences.paste_item,
		      "properties_item", &preferences.properties_item,
		      NULL);

  accel_group = gtk_menu_get_accel_group(GTK_MENU(preferences.mailbox_menu));
  gtk_window_add_accel_group(GTK_WINDOW(preferences.dialog), accel_group);

  /* finish the mailboxes list */

  preferences.store = gtk_list_store_new(N_COLUMNS,
					 G_TYPE_STRING,
					 G_TYPE_STRING,
					 G_TYPE_STRING,
					 G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(preferences.list), GTK_TREE_MODEL(preferences.store));

  column = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(column, _("Mailbox"));
  gtk_tree_view_column_set_resizable(column, TRUE);
  gtk_tree_view_column_set_sort_column_id(column, COLUMN_MAILBOX);

  renderer = gtk_cell_renderer_pixbuf_new();
  gtk_tree_view_column_pack_start(column, renderer, FALSE);
  gtk_tree_view_column_add_attribute(column, renderer, "stock-id", COLUMN_MAILBOX_ICON);
  
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(column, renderer, TRUE);
  gtk_tree_view_column_add_attribute(column, renderer, "text", COLUMN_MAILBOX);
  
  gtk_tree_view_append_column(GTK_TREE_VIEW(preferences.list), column);

  column = gtk_tree_view_column_new_with_attributes(_("Format"),
						    gtk_cell_renderer_text_new(),
						    "text", COLUMN_FORMAT,
						    NULL);

  gtk_tree_view_column_set_resizable(column, TRUE);
  gtk_tree_view_column_set_sort_column_id(column, COLUMN_FORMAT);
  gtk_tree_view_append_column(GTK_TREE_VIEW(preferences.list), column);

  gtk_tree_view_set_search_column(GTK_TREE_VIEW(preferences.list), COLUMN_MAILBOX);
  
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(preferences.list));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
  g_signal_connect(G_OBJECT(selection), "changed", G_CALLBACK(mn_preferences_selection_changed_h), NULL);
  
  mn_setup_dnd(preferences.scrolled);

  /* create the GtkSizeGroup */
  
  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget(size_group, preferences.delay_label);
  gtk_size_group_add_widget(size_group, preferences.command_new_mail_check);
  gtk_size_group_add_widget(size_group, preferences.command_clicked_check);
  g_object_unref(size_group);

  mn_conf_link(preferences.dialog, MN_CONF_PREFERENCES_DIALOG,
	       preferences.minutes_spin, MN_CONF_DELAY_MINUTES,
	       preferences.seconds_spin, MN_CONF_DELAY_SECONDS,
	       preferences.command_new_mail_check, MN_CONF_COMMANDS_NEW_MAIL_ENABLED,
	       preferences.command_new_mail_entry, MN_CONF_COMMANDS_NEW_MAIL_COMMAND,
	       preferences.command_clicked_check, MN_CONF_COMMANDS_CLICKED_ENABLED,
	       preferences.command_clicked_entry, MN_CONF_COMMANDS_CLICKED_COMMAND,
	       NULL);

  mn_preferences_update_list();
  mn_preferences_update_selected_label();
  mn_preferences_update_sensitivity();

  gtk_widget_show(preferences.dialog);
}

void
mn_preferences_update_list (void)
{
  if (preferences.dialog)
    {
      GtkTreeIter iter;
      gboolean valid;
      MNMailbox *mailbox;
      GSList *l;
      
      /* first step: remove or update old mailboxes */

      valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(preferences.store), &iter);
      while (valid)
	{
	  char *uri;

	  gtk_tree_model_get(GTK_TREE_MODEL(preferences.store), &iter, COLUMN_URI, &uri, -1);
	  mailbox = mn_mailboxes_find(uri);
	  g_free(uri);

	  if (mailbox)
	    {
	      mn_preferences_update_iter(&iter, mailbox);
	      valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(preferences.store), &iter);
	    }
	  else
	    valid = gtk_list_store_remove(preferences.store, &iter);
	}
      
      /* second step: add new mailboxes */

      MN_LIST_FOREACH(l, mn_mailboxes_get())
        {
	  mailbox = l->data;

	  if (! mn_preferences_has_mailbox(mailbox))
	    {
	      gtk_list_store_append(preferences.store, &iter);
	      mn_preferences_update_iter(&iter, mailbox);
	    }
	}
    }
}

static void
mn_preferences_update_iter (GtkTreeIter *iter, MNMailbox *mailbox)
{
  MNMailboxClass *class;
  const char *uri;
  const char *stock_id;

  g_return_if_fail(iter != NULL);
  g_return_if_fail(MN_IS_MAILBOX(mailbox));

  class = MN_MAILBOX_GET_CLASS(mailbox);
  uri = mn_mailbox_get_uri(mailbox);

  if (class->stock_id)
    stock_id = class->stock_id;
  else
    stock_id = mn_uri_is_local(uri) ? MN_STOCK_LOCAL : MN_STOCK_REMOTE;

  gtk_list_store_set(preferences.store, iter,
		     COLUMN_URI, uri,
		     COLUMN_MAILBOX_ICON, stock_id,
		     COLUMN_MAILBOX, mn_mailbox_get_name(mailbox),
		     COLUMN_FORMAT, class->format,
		     -1);
}

static gboolean
mn_preferences_has_mailbox (MNMailbox *mailbox)
{
  const char *uri;
  GtkTreeIter iter;
  gboolean valid;
  gboolean has = FALSE;

  g_return_val_if_fail(MN_IS_MAILBOX(mailbox), FALSE);

  uri = mn_mailbox_get_uri(mailbox);

  valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(preferences.store), &iter);
  while (valid)
    {
      char *this_uri;

      gtk_tree_model_get(GTK_TREE_MODEL(preferences.store), &iter, COLUMN_URI, &this_uri, -1);
      has = ! mn_uri_cmp(this_uri, uri);
      g_free(this_uri);

      if (has)
	break;

      valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(preferences.store), &iter);
    }

  return has;
}

static void
mn_preferences_update_selected_label (void)
{
  GtkTreeSelection *selection;
  int n_rows;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(preferences.list));
  n_rows = gtk_tree_selection_count_selected_rows(selection);

  if (n_rows == 0)
    gtk_label_set_text(GTK_LABEL(preferences.selected_label), _("No mailbox selected."));
  else
    {
      char *str;

      str = g_strdup_printf(ngettext("%i mailbox selected.",
				     "%i mailboxes selected.",
				     n_rows), n_rows);
      gtk_label_set_text(GTK_LABEL(preferences.selected_label), str);
      g_free(str);
    }
}

static void
mn_preferences_update_sensitivity (void)
{
  GtkTreeSelection *selection;
  gboolean has_selection;
  gboolean command_new_mail_enabled;
  gboolean command_clicked_enabled;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(preferences.list));
  has_selection = gtk_tree_selection_count_selected_rows(selection) > 0;

  gtk_widget_set_sensitive(preferences.remove, has_selection);
  gtk_widget_set_sensitive(preferences.properties, has_selection);
  gtk_widget_set_sensitive(preferences.remove_item, has_selection);
  gtk_widget_set_sensitive(preferences.cut_item, has_selection);
  gtk_widget_set_sensitive(preferences.copy_item, has_selection);
  gtk_widget_set_sensitive(preferences.properties_item, has_selection);

  command_new_mail_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(preferences.command_new_mail_check));
  command_clicked_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(preferences.command_clicked_check));

  gtk_widget_set_sensitive(preferences.command_new_mail_entry, command_new_mail_enabled);
  gtk_widget_set_sensitive(preferences.command_clicked_entry, command_clicked_enabled);
}

static void
mn_preferences_add_mailbox (void)
{
  add_mailbox = mn_mailbox_properties_dialog_new(GTK_WINDOW(preferences.dialog), MN_MAILBOX_PROPERTIES_DIALOG_MODE_ADD);

 run:
  switch (gtk_dialog_run(GTK_DIALOG(add_mailbox)))
    {
    case GTK_RESPONSE_HELP:
      mn_display_help("mailbox-properties");
      goto run;

    case GTK_RESPONSE_ACCEPT:
      {
	char *uri;
	
	uri = mn_mailbox_properties_dialog_get_uri(MN_MAILBOX_PROPERTIES_DIALOG(add_mailbox));
	g_return_if_fail(uri != NULL);
      
	if (! mn_mailboxes_find(uri))
	  {
	    GSList *gconf_mailboxes;
	    
	    gconf_mailboxes = eel_gconf_get_string_list(MN_CONF_MAILBOXES);
	    gconf_mailboxes = g_slist_append(gconf_mailboxes, uri);
	    eel_gconf_set_string_list(MN_CONF_MAILBOXES, gconf_mailboxes);
	    mn_slist_free(gconf_mailboxes);
	  }
	else
	  {
	    mn_error_dialog(NULL, _("Unable to add mailbox."), _("The mailbox is already in the list."));
	    g_free(uri);
	    goto run;
	  }
      }
      break;
    }

  gtk_widget_destroy(add_mailbox);
}

static void
mn_preferences_remove_mailbox (void)
{
  GtkTreeSelection *selection;
  GSList *gconf_mailboxes;

  gconf_mailboxes = eel_gconf_get_string_list(MN_CONF_MAILBOXES);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(preferences.list));
  gtk_tree_selection_selected_foreach(selection, mn_preferences_remove_mailbox_cb, &gconf_mailboxes);
  eel_gconf_set_string_list(MN_CONF_MAILBOXES, gconf_mailboxes);
  mn_slist_free(gconf_mailboxes);
}

static void
mn_preferences_remove_mailbox_cb (GtkTreeModel *model,
				  GtkTreePath *path,
				  GtkTreeIter *iter,
				  gpointer data)
{
  GSList **gconf_mailboxes = data;
  char *uri;
  GSList *elem;
  
  gtk_tree_model_get(model, iter, COLUMN_URI, &uri, -1);

  /* remove all the mailboxes with that URI */
  while ((elem = g_slist_find_custom(*gconf_mailboxes, uri, (GCompareFunc) mn_uri_cmp)))
    {
      *gconf_mailboxes = g_slist_remove_link(*gconf_mailboxes, elem);
      g_free(elem->data);
      g_slist_free(elem);
    }

  g_free(uri);
}

static void
mn_preferences_cut_mailbox (void)
{
  mn_preferences_copy_mailbox();
  mn_preferences_remove_mailbox();
}

static void
mn_preferences_copy_mailbox (void)
{
  GtkTreeSelection *selection;
  GSList *uri_list = NULL;
  char *gnome_copied_files;
  const GtkTargetEntry target = { GNOME_COPIED_FILES, 0, 0 };
  gboolean status;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(preferences.list));
  gtk_tree_selection_selected_foreach(selection, mn_preferences_copy_mailbox_cb, &uri_list);

  gnome_copied_files = mn_build_gnome_copied_files(MN_GNOME_COPIED_FILES_COPY, uri_list);
  mn_slist_free(uri_list);

  status = gtk_clipboard_set_with_data(clipboard,
				       &target,
				       1,
				       mn_preferences_get_clipboard_cb,
				       mn_preferences_clear_clipboard_cb,
				       gnome_copied_files);
  g_return_if_fail(status == TRUE);
}

static void
mn_preferences_copy_mailbox_cb (GtkTreeModel *model,
				GtkTreePath *path,
				GtkTreeIter *iter,
				gpointer data)
{
  GSList **uri_list = data;
  char *uri;

  gtk_tree_model_get(model, iter, COLUMN_URI, &uri, -1);
  *uri_list = g_slist_append(*uri_list, uri);
  /* uri is now owned by uri_list, do not free */
}

static void
mn_preferences_get_clipboard_cb (GtkClipboard *clipboard,
				 GtkSelectionData *selection_data,
				 unsigned int info,
				 gpointer user_data_or_owner)
{
  const char *gnome_copied_files = user_data_or_owner;
  gtk_selection_data_set(selection_data, gnome_copied_files_atom, 8, gnome_copied_files, strlen(gnome_copied_files));
}

static void
mn_preferences_clear_clipboard_cb (GtkClipboard *clipboard,
				   gpointer user_data_or_owner)
{
  char *gnome_copied_files = user_data_or_owner;
  g_free(gnome_copied_files);
}

static void
mn_preferences_paste_mailbox (void)
{
  gtk_clipboard_request_contents(clipboard,
				 gnome_copied_files_atom,
				 mn_preferences_paste_mailbox_receive_cb,
				 NULL);
}

static void
mn_preferences_paste_mailbox_receive_cb (GtkClipboard *clipboard,
					 GtkSelectionData *selection_data,
					 gpointer data)
{
  if (selection_data->type == gnome_copied_files_atom
      && selection_data->format == 8
      && selection_data->length > 0)
    {
      char *gnome_copied_files;
      gboolean status;
      MNGnomeCopiedFilesType type;
      GSList *uri_list;

      gnome_copied_files = g_strndup(selection_data->data, selection_data->length);
      status = mn_parse_gnome_copied_files(gnome_copied_files, &type, &uri_list);
      g_free(gnome_copied_files);

      if (status)
	{
	  if (type == MN_GNOME_COPIED_FILES_COPY)
	    {
	      GSList *new_mailboxes = NULL;
	      GSList *l;

	      MN_LIST_FOREACH(l, uri_list)
	        {
		  const char *uri = l->data;

		  if (! mn_mailboxes_find(uri))
		    new_mailboxes = g_slist_append(new_mailboxes, g_strdup(uri));
		}

	      if (new_mailboxes)
		{
		  GSList *gconf_mailboxes;
		  
		  gconf_mailboxes = eel_gconf_get_string_list(MN_CONF_MAILBOXES);
		  gconf_mailboxes = g_slist_concat(gconf_mailboxes, new_mailboxes);
		  eel_gconf_set_string_list(MN_CONF_MAILBOXES, gconf_mailboxes);
		  mn_slist_free(gconf_mailboxes);
		}
	    }

	  mn_slist_free(uri_list);
	}
    }
}

static void
mn_preferences_edit_mailbox (void)
{
  GtkTreeSelection *selection;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(preferences.list));
  gtk_tree_selection_selected_foreach(selection, mn_preferences_edit_mailbox_cb, NULL);
}

static void
mn_preferences_edit_mailbox_cb (GtkTreeModel *model,
				GtkTreePath *path,
				GtkTreeIter *iter,
				gpointer data)
{
  GtkWidget *dialog;
  char *uri;

  dialog = mn_mailbox_properties_dialog_new(GTK_WINDOW(preferences.dialog), MN_MAILBOX_PROPERTIES_DIALOG_MODE_EDIT);

  gtk_tree_model_get(model, iter, COLUMN_URI, &uri, -1);
  mn_mailbox_properties_dialog_set_uri(MN_MAILBOX_PROPERTIES_DIALOG(dialog), uri);
  g_free(uri);

  g_signal_connect(G_OBJECT(dialog),
		   "response",
		   G_CALLBACK(mn_preferences_mailbox_properties_response_h),
		   NULL);
  
  gtk_widget_show(dialog);
}

static void
mn_preferences_selection_changed_h (GtkTreeSelection *selection,
				    gpointer user_data)
{
  mn_preferences_update_selected_label();
  mn_preferences_update_sensitivity();
}

static void
mn_preferences_mailbox_properties_response_h (GtkDialog *dialog,
					      int response,
					      gpointer user_data)
{
  if (response == GTK_RESPONSE_HELP)
    mn_display_help("mailbox-properties");
  else
    {
      if (response == GTK_RESPONSE_APPLY || response == GTK_RESPONSE_OK)
	mn_mailbox_properties_dialog_apply(MN_MAILBOX_PROPERTIES_DIALOG(dialog));
      if (response == GTK_RESPONSE_CANCEL || response == GTK_RESPONSE_OK)
	gtk_widget_destroy(GTK_WIDGET(dialog));
    }
}

static void
mn_preferences_popup_mailbox_menu (unsigned int button, guint32 activate_time)
{
  gtk_clipboard_request_contents(clipboard,
				 gnome_copied_files_atom,
				 mn_preferences_popup_mailbox_menu_receive_cb,
				 NULL);
  gtk_menu_popup(GTK_MENU(preferences.mailbox_menu), NULL, NULL, NULL, NULL, button, activate_time);
}

static void
mn_preferences_popup_mailbox_menu_receive_cb (GtkClipboard *clipboard,
					      GtkSelectionData *selection_data,
					      gpointer data)
{
  gboolean can_paste = FALSE;

  if (selection_data->type == gnome_copied_files_atom
      && selection_data->format == 8
      && selection_data->length > 0)
    {
      char *gnome_copied_files;
      gboolean status;
      MNGnomeCopiedFilesType type;
      GSList *uri_list;
      
      gnome_copied_files = g_strndup(selection_data->data, selection_data->length);
      status = mn_parse_gnome_copied_files(gnome_copied_files, &type, &uri_list);
      g_free(gnome_copied_files);

      if (status)
	{
	  if (type == MN_GNOME_COPIED_FILES_COPY && uri_list)
	    can_paste = TRUE;
	  mn_slist_free(uri_list);
	}
    }

  gtk_widget_set_sensitive(preferences.paste_item, can_paste);
}

/* libglade callbacks */

void
mn_preferences_toggled_h (GtkToggleButton *togglebutton, gpointer user_data)
{
  mn_preferences_update_sensitivity();
}

void
mn_preferences_add_clicked_h (GtkButton *button, gpointer user_data)
{
  mn_preferences_add_mailbox();
}

void
mn_preferences_remove_clicked_h (GtkButton *button, gpointer user_data)
{
  mn_preferences_remove_mailbox();
}

void
mn_preferences_properties_clicked_h (GtkButton *button, gpointer user_data)
{
  mn_preferences_edit_mailbox();
}

void
mn_preferences_add_item_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  mn_preferences_add_mailbox();
}

void
mn_preferences_remove_item_activate_h (GtkMenuItem *menuitem,
				       gpointer user_data)
{
  mn_preferences_remove_mailbox();
}

void
mn_preferences_cut_item_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  mn_preferences_cut_mailbox();
}

void
mn_preferences_copy_item_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  mn_preferences_copy_mailbox();
}

void
mn_preferences_paste_item_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  mn_preferences_paste_mailbox();
}

void
mn_preferences_properties_item_activate_h (GtkMenuItem *menuitem,
					   gpointer user_data)
{
  mn_preferences_edit_mailbox();
}

gboolean
mn_preferences_list_popup_menu_h (GtkWidget *widget, gpointer user_data)
{
  mn_preferences_popup_mailbox_menu(0, gtk_get_current_event_time());

  return TRUE;			/* a menu was activated */
}

gboolean
mn_preferences_list_button_press_event_h (GtkWidget *widget,
					  GdkEventButton *event,
					  gpointer user_data)
{
  if (event->button == 3)
    mn_preferences_popup_mailbox_menu(event->button, event->time);

  return FALSE;			/* propagate event */
}

void
mn_preferences_list_row_activated_h (GtkTreeView *treeview,
				     GtkTreePath *path,
				     GtkTreeViewColumn *column,
				     gpointer user_data)
{
  mn_preferences_edit_mailbox();
}

gboolean
mn_preferences_scrolled_drag_motion_h (GtkWidget *widget,
				       GdkDragContext *drag_context,
				       int x,
				       int y,
				       unsigned int time,
				       gpointer user_data)
{
  GtkAdjustment *adjustment;
  
  adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(preferences.scrolled));
  gtk_adjustment_set_value(adjustment, (double) y / (widget->allocation.height - 2) * (adjustment->upper - adjustment->page_size));

  return TRUE;
}

void
mn_preferences_response_h (GtkDialog *dialog, int response, gpointer user_data)
{
  switch (response)
    {
    case GTK_RESPONSE_HELP:
      mn_display_help("preferences");
      break;

    case GTK_RESPONSE_CLOSE:
      gtk_widget_destroy(preferences.dialog);
      break;
    }
}

void
mn_preferences_destroy_h (GtkObject *object, gpointer user_data)
{
  gtk_widget_destroy(preferences.mailbox_menu);
  g_object_unref(preferences.store);
  preferences.dialog = NULL;
}
