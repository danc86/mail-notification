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
#include "mn-mailbox-properties-dialog.h"
#include "mn-mailbox-properties.h"
#include "mn-autodetect-mailbox-properties.h"
#include "mn-system-mailbox-properties.h"
#ifdef WITH_POP3
#include "mn-pop3-mailbox-properties.h"
#endif
#ifdef WITH_IMAP
#include "mn-imap-mailbox-properties.h"
#endif
#ifdef WITH_GMAIL
#include "mn-gmail-mailbox-properties.h"
#endif
#include "mn-conf.h"
#include "mn-util.h"
#include "mn-stock.h"
#include "mn-mailboxes.h"

/*** types *******************************************************************/

typedef struct
{
  GtkWidget				*mailbox_type_label;
  GtkWidget				*mailbox_type_combo;
  GtkWidget				*properties_event_box;
  
  MNURI					*uri;
  GtkWidget				*apply_button;
  GtkWidget				*accept_button;
  GtkListStore				*store;
} Private;

enum {
  COLUMN_PROPERTIES,
  COLUMN_TEXT,
  N_COLUMNS
};
    
/*** variables ***************************************************************/

static GType selected_type = 0;

/*** functions ***************************************************************/

static void mn_mailbox_properties_dialog_private_free (Private *private);

static void mn_mailbox_properties_dialog_add_type (MNMailboxPropertiesDialog *dialog,
						   GType properties_type);

static void mn_mailbox_properties_dialog_select_properties (MNMailboxPropertiesDialog *dialog);
static void mn_mailbox_properties_dialog_set_active_properties (MNMailboxPropertiesDialog *dialog, MNMailboxProperties *properties);
static MNMailboxProperties *mn_mailbox_properties_dialog_get_active_properties (MNMailboxPropertiesDialog *dialog);
static MNMailboxProperties *mn_mailbox_properties_dialog_get_properties_by_type (MNMailboxPropertiesDialog *dialog, GType type);

static void mn_mailbox_properties_dialog_set_uri (MNMailboxPropertiesDialog *dialog, MNURI *uri);
static void mn_mailbox_properties_dialog_set_contents (MNMailboxPropertiesDialog *dialog);

static void mn_mailbox_properties_dialog_update_sensitivity (MNMailboxPropertiesDialog *dialog);

/*** implementation **********************************************************/

GtkWidget *
mn_mailbox_properties_dialog_new (GtkWindow *parent, MNURI *uri)
{
  MNMailboxPropertiesDialog *dialog;
  Private *private;
  GtkCellRenderer *renderer;

  private = g_new0(Private, 1);
  mn_create_interface(MN_INTERFACE_FILE("mailbox-properties.glade"),
		      "dialog", &dialog,
		      "mailbox_type_label", &private->mailbox_type_label,
		      "mailbox_type_combo", &private->mailbox_type_combo,
		      "properties_event_box", &private->properties_event_box,
		      NULL);

  g_object_set_data_full(G_OBJECT(dialog),
			 MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE_KEY,
			 private,
			 (GDestroyNotify) mn_mailbox_properties_dialog_private_free);

  if (parent)
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

  /* finish the type combo box */

  private->store = gtk_list_store_new(N_COLUMNS,
				      GTK_TYPE_WIDGET,
				      G_TYPE_STRING);

  mn_mailbox_properties_dialog_add_type(dialog, MN_TYPE_AUTODETECT_MAILBOX_PROPERTIES);
  mn_mailbox_properties_dialog_add_type(dialog, MN_TYPE_SYSTEM_MAILBOX_PROPERTIES);
#ifdef WITH_POP3
  mn_mailbox_properties_dialog_add_type(dialog, MN_TYPE_POP3_MAILBOX_PROPERTIES);
#endif
#ifdef WITH_IMAP
  mn_mailbox_properties_dialog_add_type(dialog, MN_TYPE_IMAP_MAILBOX_PROPERTIES);
#endif
#ifdef WITH_GMAIL
  mn_mailbox_properties_dialog_add_type(dialog, MN_TYPE_GMAIL_MAILBOX_PROPERTIES);
#endif

  renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(private->mailbox_type_combo), renderer, TRUE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(private->mailbox_type_combo), renderer,
				 "markup", COLUMN_TEXT,
				 NULL);

  gtk_combo_box_set_model(GTK_COMBO_BOX(private->mailbox_type_combo), GTK_TREE_MODEL(private->store));

  /* setup the dialog depending on the mode (edit or add) */

  gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_HELP, GTK_RESPONSE_HELP);
  if (uri)
    {
      private->apply_button = gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_APPLY, GTK_RESPONSE_APPLY);
      gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
      private->accept_button = gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

      mn_mailbox_properties_dialog_set_uri(dialog, uri);
      mn_mailbox_properties_dialog_set_contents(dialog);
    }
  else
    {
      MNMailboxProperties *properties;

      gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
      private->accept_button = gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_ADD, GTK_RESPONSE_ACCEPT);

      if (! selected_type)
	selected_type = MN_TYPE_AUTODETECT_MAILBOX_PROPERTIES;

      properties = mn_mailbox_properties_dialog_get_properties_by_type(dialog, selected_type);
      mn_mailbox_properties_dialog_set_active_properties(dialog, properties);

      gtk_window_set_title(GTK_WINDOW(dialog), _("Add a Mailbox"));
    }
  gtk_widget_grab_default(private->accept_button);

  return GTK_WIDGET(dialog);
}

static void
mn_mailbox_properties_dialog_private_free (Private *private)
{
  g_return_if_fail(private != NULL);

  if (private->uri)
    g_object_unref(private->uri);
  g_object_unref(private->store);
  g_free(private);
}

static void
mn_mailbox_properties_dialog_add_type (MNMailboxPropertiesDialog *dialog,
				       GType properties_type)
{
  Private *private;
  GtkSizeGroup *size_group;
  MNMailboxProperties *properties;
  char *label;
  GtkTreeIter iter;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
  properties = g_object_new(properties_type, "size-group", size_group, NULL);
  g_object_unref(size_group);

  mn_gtk_object_ref_and_sink(GTK_OBJECT(properties));

  gtk_list_store_append(private->store, &iter);

  label = mn_mailbox_properties_get_label(properties);
  gtk_list_store_set(private->store, &iter,
		     COLUMN_PROPERTIES, properties,
		     COLUMN_TEXT, label,
		     -1);
  g_free(label);

  gtk_widget_show(GTK_WIDGET(properties));
  g_signal_connect_swapped(properties, "notify::complete", G_CALLBACK(mn_mailbox_properties_dialog_update_sensitivity), dialog);

  g_object_unref(properties);	/* now it belongs to the store */
}

static void
mn_mailbox_properties_dialog_select_properties (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  MNMailboxProperties *properties;
  GtkSizeGroup *size_group;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  properties = (MNMailboxProperties *) GTK_BIN(private->properties_event_box)->child;
  if (properties)
    {
      size_group = mn_mailbox_properties_get_size_group(properties);
      gtk_size_group_remove_widget(size_group, private->mailbox_type_label);
      gtk_container_remove(GTK_CONTAINER(private->properties_event_box), GTK_WIDGET(properties));
    }
  
  properties = mn_mailbox_properties_dialog_get_active_properties(dialog);
  gtk_container_add(GTK_CONTAINER(private->properties_event_box), GTK_WIDGET(properties));
  size_group = mn_mailbox_properties_get_size_group(properties);
  gtk_size_group_add_widget(size_group, private->mailbox_type_label);
}

static void
mn_mailbox_properties_dialog_set_active_properties (MNMailboxPropertiesDialog *dialog,
						    MNMailboxProperties *properties)
{
  Private *private;
  gboolean valid;
  GtkTreeIter iter;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES(properties));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(private->store), &iter);
  while (valid)
    {
      MNMailboxProperties *these_properties;

      gtk_tree_model_get(GTK_TREE_MODEL(private->store), &iter, COLUMN_PROPERTIES, &these_properties, -1);
      g_object_unref(these_properties);

      if (these_properties == properties)
	{
	  gtk_combo_box_set_active_iter(GTK_COMBO_BOX(private->mailbox_type_combo), &iter);
	  break;
	}

      valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(private->store), &iter);
    }
}

static MNMailboxProperties *
mn_mailbox_properties_dialog_get_active_properties (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  GtkTreeIter iter;
  MNMailboxProperties *properties = NULL;

  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog), 0);
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(private->mailbox_type_combo), &iter))
    {
      gtk_tree_model_get(GTK_TREE_MODEL(private->store), &iter, COLUMN_PROPERTIES, &properties, -1);
      g_object_unref(properties);
    }

  return properties;
}

static MNMailboxProperties *
mn_mailbox_properties_dialog_get_properties_by_type (MNMailboxPropertiesDialog *dialog,
						     GType type)
{
  Private *private;
  gboolean valid;
  GtkTreeIter iter;

  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog), NULL);
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(private->store), &iter);
  while (valid)
    {
      MNMailboxProperties *properties;

      gtk_tree_model_get(GTK_TREE_MODEL(private->store), &iter, COLUMN_PROPERTIES, &properties, -1);
      g_object_unref(properties);

      if (G_TYPE_CHECK_INSTANCE_TYPE(properties, type))
	return properties;

      valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(private->store), &iter);
    }

  return NULL;
}

static void
mn_mailbox_properties_dialog_set_uri (MNMailboxPropertiesDialog *dialog,
				      MNURI *uri)
{
  Private *private;
  char *title;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  g_return_if_fail(MN_IS_URI(uri));

  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  if (private->uri)
    g_object_unref(private->uri);
  private->uri = g_object_ref(uri);

  title = g_strdup_printf(_("%s Properties"), uri->human_readable);
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  g_free(title);
}

static void
mn_mailbox_properties_dialog_set_contents (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  gboolean valid;
  GtkTreeIter iter;
  MNMailboxProperties *properties;
  gboolean found = FALSE;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));

  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);
  g_return_if_fail(private->uri != NULL);

  valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(private->store), &iter);
  while (valid)
    {
      gtk_tree_model_get(GTK_TREE_MODEL(private->store), &iter, COLUMN_PROPERTIES, &properties, -1);
      g_object_unref(properties);

      if (! MN_IS_AUTODETECT_MAILBOX_PROPERTIES(properties)
	  && mn_mailbox_properties_set_uri(properties, private->uri))
	{
	  found = TRUE;
	  break;
	}

      valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(private->store), &iter);
    }

  if (! found)
    {
      properties = mn_mailbox_properties_dialog_get_properties_by_type(dialog, MN_TYPE_AUTODETECT_MAILBOX_PROPERTIES);
      mn_mailbox_properties_set_uri(properties, private->uri);
    }
  mn_mailbox_properties_dialog_set_active_properties(dialog, properties);
}

MNURI *
mn_mailbox_properties_dialog_get_uri (MNMailboxPropertiesDialog *dialog)
{
  Private *private;

  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog), NULL);
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  return private->uri;
}

MNURI *
mn_mailbox_properties_dialog_get_current_uri (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  MNMailboxProperties *properties;

  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog), NULL);
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  properties = mn_mailbox_properties_dialog_get_active_properties(dialog);
  return mn_mailbox_properties_get_uri(properties);
}

void
mn_mailbox_properties_dialog_apply (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  MNURI *new_uri;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  new_uri = mn_mailbox_properties_dialog_get_current_uri(dialog);
  g_return_if_fail(MN_IS_URI(new_uri));

  if (strcmp(new_uri->text, private->uri->text))
    {
      GSList *gconf_mailboxes;
      GSList *elem;

      gconf_mailboxes = eel_gconf_get_string_list(MN_CONF_MAILBOXES);

      elem = mn_mailboxes_conf_find_uri(gconf_mailboxes, private->uri);
      if (elem)
	{
	  g_free(elem->data);
	  elem->data = g_strdup(new_uri->text);
	}
      
      eel_gconf_set_string_list(MN_CONF_MAILBOXES, gconf_mailboxes);
      eel_g_slist_free_deep(gconf_mailboxes);
      
      mn_mailbox_properties_dialog_set_uri(dialog, new_uri);
    }
  g_object_unref(new_uri);
}

static void
mn_mailbox_properties_dialog_update_sensitivity (MNMailboxPropertiesDialog *dialog)
{
  Private *private;
  MNMailboxProperties *properties;

  g_return_if_fail(MN_IS_MAILBOX_PROPERTIES_DIALOG(dialog));
  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  properties = mn_mailbox_properties_dialog_get_active_properties(dialog);
  if (properties)
    {
      gboolean complete;

      g_object_get(G_OBJECT(properties), "complete", &complete, NULL);

      if (private->apply_button)
	gtk_widget_set_sensitive(private->apply_button, complete);
      gtk_widget_set_sensitive(private->accept_button, complete);
    }
}

/* libglade callbacks */

void
mn_mailbox_properties_dialog_mailbox_type_changed_h (gpointer user_data,
						     GtkComboBox *combobox)
{
  MNMailboxPropertiesDialog *dialog = user_data;
  Private *private;

  private = MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(dialog);

  mn_mailbox_properties_dialog_select_properties(dialog);
  if (! private->uri)		/* mode is add */
    {
      MNMailboxProperties *properties;
      
      properties = mn_mailbox_properties_dialog_get_active_properties(dialog);
      selected_type = G_TYPE_FROM_INSTANCE(properties);
    }

  mn_mailbox_properties_dialog_update_sensitivity(dialog);
}
