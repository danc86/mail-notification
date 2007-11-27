/*
 * Mail Notification
 * Copyright (C) 2003-2007 Jean-Yves Lefort <jylefort@brutele.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <gmodule.h>
#include <glib/gi18n.h>
#include <gnome.h>
#include <glade/glade.h>
#include <eel/eel.h>
#include "mn-util.h"
#include "mn-mailboxes.h"
#include "mn-shell.h"
#include "mn-conf.h"

/*** types *******************************************************************/

typedef struct
{
  GtkContainer	*container;
  const char	*callback_prefix;
} ContainerCreateInterfaceConnectInfo;

enum
{
  TARGET_URI_LIST,
  TARGET_MOZ_URL
};

typedef struct
{
  gpointer		instance;
  unsigned long		id;
} SignalHandler;

typedef struct
{
  GMainLoop	*loop;
  int		response;
  gboolean	destroyed;
} RunNonmodalInfo;

/*** functions ***************************************************************/

static GladeXML *mn_glade_xml_new (const char *filename,
				   const char *root,
				   const char *domain);
static GtkWidget *mn_glade_xml_get_widget (GladeXML *xml,
					   const char *widget_name);

static void mn_container_create_interface_connect_cb (const char *handler_name,
						      GObject *object,
						      const char *signal_name,
						      const char *signal_data,
						      GObject *connect_object,
						      gboolean after,
						      gpointer user_data);

static int mn_g_str_slist_compare_func (gconstpointer a, gconstpointer b);

static void mn_file_chooser_dialog_file_activated_h (GtkFileChooser *chooser,
						     gpointer user_data);
static void mn_file_chooser_dialog_response_h (GtkDialog *dialog,
					       int response_id,
					       gpointer user_data);

static gboolean mn_scrolled_window_drag_motion_h (GtkWidget *widget,
						  GdkDragContext *drag_context,
						  int x,
						  int y,
						  unsigned int time,
						  gpointer user_data);
static void mn_drag_data_received_h (GtkWidget *widget,
				     GdkDragContext *drag_context,
				     int x,
				     int y,
				     GtkSelectionData *selection_data,
				     unsigned int info,
				     unsigned int time,
				     gpointer user_data);

static GtkWidget *mn_menu_item_new (const char *stock_id, const char *mnemonic);

static void mn_error_dialog_real (GtkWindow *parent,
				  MNDialogFlags flags,
				  const char *primary,
				  const char *format,
				  va_list args);

static void mn_g_object_connect_weak_notify_cb (gpointer data,
						GObject *former_object);

static void mn_dialog_run_nonmodal_destroy_h (GtkObject *object,
					      gpointer user_data);
static void mn_dialog_run_nonmodal_unmap_h (GtkWidget *widget,
					    gpointer user_data);
static void mn_dialog_run_nonmodal_response_h (GtkDialog *dialog,
					       int response,
					       gpointer user_data);
static gboolean mn_dialog_run_nonmodal_delete_event_h (GtkWidget *widget,
						       GdkEvent *event,
						       gpointer user_data);
static void mn_dialog_run_nonmodal_shutdown_loop (RunNonmodalInfo *info);

static void mn_handle_execute_result (int status, const char *command);

/*** implementation **********************************************************/

void
mn_info (const char *format, ...)
{
  va_list args;

  g_return_if_fail(format != NULL);

  va_start(args, format);
  g_logv(G_LOG_DOMAIN, G_LOG_LEVEL_INFO, format, args);
  va_end(args);
}

GSList *
mn_g_slist_append_elements (GSList *list, gpointer data, ...)
{
  va_list args;

  va_start(args, data);
  while (data)
    {
      list = g_slist_append(list, data);
      data = va_arg(args, gpointer);
    }
  va_end(args);

  return list;
}

void
mn_g_slist_clear (GSList **list)
{
  g_return_if_fail(list != NULL);

  g_slist_free(*list);
  *list = NULL;
}

void
mn_g_slist_clear_deep (GSList **list)
{
  g_return_if_fail(list != NULL);

  eel_g_slist_free_deep(*list);
  *list = NULL;
}

void
mn_g_slist_clear_deep_custom (GSList **list,
			      GFunc element_free_func,
			      gpointer user_data)
{
  g_return_if_fail(list != NULL);
  g_return_if_fail(element_free_func != NULL);

  eel_g_slist_free_deep_custom(*list, element_free_func, user_data);
  *list = NULL;
}

/**
 * mn_g_slist_delete_link_deep_custom:
 * @list: a #GSList of @element_free_func-freeable objects
 * @link_: an element in the #GSList
 * @element_free_func: a function to free @link_->data
 * @user_data: user data to pass to @element_free_func
 *
 * Equivalent of g_slist_delete_link() for a list of
 * @element_free_func-freeable objects.
 *
 * Return value: new head of @list.
 **/
GSList *
mn_g_slist_delete_link_deep_custom (GSList *list,
				    GSList *link_,
				    GFunc element_free_func,
				    gpointer user_data)
{
  g_return_val_if_fail(element_free_func != NULL, NULL);

  if (link_)
    element_free_func(link_->data, user_data);

  return g_slist_delete_link(list, link_);
}

GSList *
mn_g_str_slist_find (GSList *list, const char *str)
{
  g_return_val_if_fail(str != NULL, NULL);

  return g_slist_find_custom(list, str, mn_g_str_slist_compare_func);
}

static int
mn_g_str_slist_compare_func (gconstpointer a, gconstpointer b)
{
  return strcmp(a, b);
}

GSList *
mn_g_object_slist_ref (GSList *list)
{
  g_slist_foreach(list, (GFunc) g_object_ref, NULL);
  return list;
}

GSList *
mn_g_object_slist_copy (GSList *list)
{
  return g_slist_copy(mn_g_object_slist_ref(list));
}

/**
 * mn_g_object_slist_free:
 * @list: a #GSList of #GObject instances
 *
 * Equivalent of eel_g_object_list_free() for a singly-linked list.
 **/
void
mn_g_object_slist_free (GSList *list)
{
  eel_g_slist_free_deep_custom(list, (GFunc) g_object_unref, NULL);
}

void
mn_g_object_slist_clear (GSList **list)
{
  g_return_if_fail(list != NULL);

  mn_g_object_slist_free(*list);
  *list = NULL;
}

void
mn_g_queue_free_deep_custom (GQueue *queue,
			     GFunc element_free_func,
			     gpointer user_data)
{
  gpointer data;

  g_return_if_fail(queue != NULL);
  g_return_if_fail(element_free_func != NULL);

  while ((data = g_queue_pop_head(queue)))
    element_free_func(data, user_data);

  g_queue_free(queue);
}

/**
 * mn_str_isnumeric:
 * @str: the ASCII string to test
 *
 * Tests if the ASCII string @str is numeric. Implemented by calling
 * g_ascii_isdigit() on each character of @str.
 *
 * Return value: %TRUE if the ASCII string @str only consists of digits
 **/
gboolean
mn_str_isnumeric (const char *str)
{
  int i;

  g_return_val_if_fail(str != NULL, FALSE);

  for (i = 0; str[i]; i++)
    if (! g_ascii_isdigit(str[i]))
      return FALSE;

  return i > 0;
}

gboolean
mn_str_ishex (const char *str)
{
  int i;

  g_return_val_if_fail(str != NULL, FALSE);

  for (i = 0; str[i]; i++)
    if (! g_ascii_isxdigit(str[i]))
      return FALSE;

  return i > 0;
}

/**
 * mn_strstr_span:
 * @big: a string.
 * @little: a string to search for in @big.
 *
 * Locates the first occurrence of @little in @big.
 *
 * Return value: a pointer to the character following the first
 * occurrence of @little in @big, or %NULL if @little does not appear
 * in @big.
 **/
char *
mn_strstr_span (const char *big, const char *little)
{
  char *s;

  g_return_val_if_fail(big != NULL, NULL);
  g_return_val_if_fail(little != NULL, NULL);

  s = strstr(big, little);
  if (s)
    s += strlen(little);

  return s;
}

GdkPixbuf *
mn_pixbuf_new (const char *filename)
{
  GdkPixbuf *pixbuf;
  GError *err = NULL;

  g_return_val_if_fail(filename != NULL, NULL);

  pixbuf = gdk_pixbuf_new_from_file(filename, &err);
  if (! pixbuf)
    {
      mn_fatal_error_dialog(NULL, _("Unable to load image \"%s\" (%s). Please check your Mail Notification installation."), filename, err->message);
      g_error_free(err);
    }

  return pixbuf;
}

static GladeXML *
mn_glade_xml_new (const char *filename, const char *root, const char *domain)
{
  GladeXML *xml;

  g_return_val_if_fail(filename != NULL, NULL);

  xml = glade_xml_new(filename, root, domain);
  if (! xml)
    mn_fatal_error_dialog(NULL, _("Unable to load interface \"%s\". Please check your Mail Notification installation."), filename);

  return xml;
}

static GtkWidget *
mn_glade_xml_get_widget (GladeXML *xml, const char *widget_name)
{
  GtkWidget *widget;

  g_return_val_if_fail(GLADE_IS_XML(xml), NULL);
  g_return_val_if_fail(widget_name != NULL, NULL);

  widget = glade_xml_get_widget(xml, widget_name);
  if (! widget)
    mn_fatal_error_dialog(NULL, _("Widget \"%s\" not found in interface \"%s\". Please check your Mail Notification installation."), widget_name, xml->filename);

  return widget;
}

void
mn_container_create_interface (GtkContainer *container,
			       const char *filename,
			       const char *child_name,
			       const char *callback_prefix,
			       ...)
{
  GladeXML *xml;
  GtkWidget *child;
  ContainerCreateInterfaceConnectInfo info;
  va_list args;
  const char *widget_name;

  g_return_if_fail(GTK_IS_CONTAINER(container));
  g_return_if_fail(filename != NULL);
  g_return_if_fail(child_name != NULL);
  g_return_if_fail(callback_prefix != NULL);

  xml = mn_glade_xml_new(filename, child_name, NULL);
  child = mn_glade_xml_get_widget(xml, child_name);

  if (GTK_IS_DIALOG(container))
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(container)->vbox), child, TRUE, TRUE, 0);
  else
    gtk_container_add(container, child);

  info.container = container;
  info.callback_prefix = callback_prefix;
  glade_xml_signal_autoconnect_full(xml, mn_container_create_interface_connect_cb, &info);

  va_start(args, callback_prefix);
  while ((widget_name = va_arg(args, const char *)))
    {
      GtkWidget **widget;

      widget = va_arg(args, GtkWidget **);
      g_return_if_fail(widget != NULL);

      *widget = mn_glade_xml_get_widget(xml, widget_name);
    }
  va_end(args);

  g_object_unref(xml);
}

static void
mn_container_create_interface_connect_cb (const char *handler_name,
					  GObject *object,
					  const char *signal_name,
					  const char *signal_data,
					  GObject *connect_object,
					  gboolean after,
					  gpointer user_data)
{
  static GModule *module = NULL;
  ContainerCreateInterfaceConnectInfo *info = user_data;
  char *cb_name;
  GCallback cb;
  GConnectFlags flags;

  if (! module)
    {
      module = g_module_open(NULL, 0);
      if (! module)
	mn_fatal_error_dialog(NULL, _("Unable to open self as a module (%s)."), g_module_error());
    }

  cb_name = g_strconcat(info->callback_prefix, handler_name, NULL);
  if (! g_module_symbol(module, cb_name, (gpointer) &cb))
    mn_fatal_error_dialog(NULL, _("Signal handler \"%s\" not found. Please check your Mail Notification installation."), cb_name);
  g_free(cb_name);

  flags = G_CONNECT_SWAPPED;
  if (after)
    flags |= G_CONNECT_AFTER;

  g_signal_connect_data(object, signal_name, cb, info->container, NULL, flags);
}

GtkWindow *
mn_widget_get_parent_window (GtkWidget *widget)
{
  GtkWidget *toplevel;

  g_return_val_if_fail(GTK_IS_WIDGET(widget), NULL);

  toplevel = gtk_widget_get_toplevel(widget);

  return GTK_WIDGET_TOPLEVEL(toplevel) ? GTK_WINDOW(toplevel) : NULL;
}

/**
 * mn_file_chooser_dialog_allow_select_folder:
 * @dialog: a #GtkFileChooserDialog
 * @accept_id: the "accept" response ID (must not be
 *             GTK_RESPONSE_ACCEPT, GTK_RESPONSE_OK, GTK_RESPONSE_YES
 *             or GTK_RESPONSE_APPLY)
 *
 * Allows @dialog to pick a file (%GTK_FILE_CHOOSER_ACTION_OPEN) or
 * select a folder (%GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER) at the
 * same time.
 *
 * Can be removed after
 * http://bugzilla.gnome.org/show_bug.cgi?id=136294 is fixed.
 **/
void
mn_file_chooser_dialog_allow_select_folder (GtkFileChooserDialog *dialog,
					    int accept_id)
{
  g_return_if_fail(GTK_IS_FILE_CHOOSER_DIALOG(dialog));
  g_return_if_fail(gtk_file_chooser_get_action(GTK_FILE_CHOOSER(dialog)) == GTK_FILE_CHOOSER_ACTION_OPEN);
  g_return_if_fail(! (accept_id == GTK_RESPONSE_ACCEPT
		      || accept_id == GTK_RESPONSE_OK
		      || accept_id == GTK_RESPONSE_YES
		      || accept_id == GTK_RESPONSE_APPLY));

  g_object_connect(dialog,
		   "signal::file-activated", mn_file_chooser_dialog_file_activated_h, GINT_TO_POINTER(accept_id),
		   "signal::response", mn_file_chooser_dialog_response_h, GINT_TO_POINTER(accept_id),
		   NULL);
}

static void
mn_file_chooser_dialog_file_activated_h (GtkFileChooser *chooser,
					 gpointer user_data)
{
  int accept_id = GPOINTER_TO_INT(user_data);

  gtk_dialog_response(GTK_DIALOG(chooser), accept_id);
}

static void
mn_file_chooser_dialog_response_h (GtkDialog *dialog,
				   int response_id,
				   gpointer user_data)
{
  int accept_id = GPOINTER_TO_INT(user_data);

  if (response_id == accept_id)
    {
      char *uri;

      uri = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog));
      if (uri)
	g_free(uri);
      else
	g_signal_stop_emission_by_name(dialog, "response");
    }
}

/**
 * mn_setup_dnd:
 * @widget: a widget to setup mailbox drag-and-drop for
 *
 * Configures @widget so that when mailboxes are dropped on it, they
 * will be added to the Mail Notification mailbox list.
 **/
void
mn_setup_dnd (GtkWidget *widget)
{
  static const GtkTargetEntry targets[] = {
    { "text/uri-list",	0, TARGET_URI_LIST },
    { "text/x-moz-url",	0, TARGET_MOZ_URL }
  };

  g_return_if_fail(GTK_IS_WIDGET(widget));

  gtk_drag_dest_set(widget,
		    GTK_DEST_DEFAULT_ALL,
		    targets,
		    G_N_ELEMENTS(targets),
		    GDK_ACTION_COPY);

  if (GTK_IS_SCROLLED_WINDOW(widget))
    g_signal_connect(widget,
		     "drag-motion",
		     G_CALLBACK(mn_scrolled_window_drag_motion_h),
		     NULL);

  g_signal_connect(widget,
		   "drag-data-received",
		   G_CALLBACK(mn_drag_data_received_h),
		   NULL);
}

static gboolean
mn_scrolled_window_drag_motion_h (GtkWidget *widget,
				  GdkDragContext *drag_context,
				  int x,
				  int y,
				  unsigned int time,
				  gpointer user_data)
{
  GtkAdjustment *adjustment;

  adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(widget));
  gtk_adjustment_set_value(adjustment, (double) y / (widget->allocation.height - 2) * (adjustment->upper - adjustment->page_size));

  return TRUE;			/* we're forcibly in a drop zone */
}

static void
mn_drag_data_received_h (GtkWidget *widget,
			 GdkDragContext *drag_context,
			 int x,
			 int y,
			 GtkSelectionData *selection_data,
			 unsigned int info,
			 unsigned int time,
			 gpointer user_data)
{
  switch (info)
    {
    case TARGET_URI_LIST:
      {
	char **uriv;
	int i;
	GSList *invalid_uri_list = NULL;

	uriv = gtk_selection_data_get_uris(selection_data);
	if (! uriv)
	  {
	    mn_error_dialog(mn_widget_get_parent_window(widget),
			    _("A drag and drop error has occurred"),
			    _("An invalid location list has been received."));
	    return;
	  }

	for (i = 0; uriv[i]; i++)
	  if (*uriv[i])
	    {
	      MNMailbox *mailbox;

	      mailbox = mn_mailbox_new_from_uri(uriv[i]);
	      if (mailbox)
		{
		  mn_mailbox_seal(mailbox);
		  mn_mailboxes_queue_add(mn_shell->mailboxes, mailbox);
		  g_object_unref(mailbox);
		}
	      else
		invalid_uri_list = g_slist_append(invalid_uri_list, uriv[i]);
	    }

	if (invalid_uri_list)
	  {
	    mn_invalid_uri_list_dialog(mn_widget_get_parent_window(widget), _("A drag and drop error has occurred"), invalid_uri_list);
	    g_slist_free(invalid_uri_list);
	  }

	g_strfreev(uriv);
      }
      break;

    case TARGET_MOZ_URL:
      {
	GString *url;
	const guint16 *char_data;
	int char_len;
	int i;
	MNMailbox *mailbox;

	/* text/x-moz-url is encoded in UCS-2 but in format 8: broken */
	if (selection_data->format != 8 || selection_data->length <= 0 || (selection_data->length % 2) != 0)
	  {
	    mn_error_dialog(mn_widget_get_parent_window(widget),
			    _("A drag and drop error has occurred"),
			    _("An invalid Mozilla location has been received."));
	    return;
	  }

	char_data = (const guint16 *) selection_data->data;
	char_len = selection_data->length / 2;

	url = g_string_new(NULL);
	for (i = 0; i < char_len && char_data[i] != '\n'; i++)
	  g_string_append_unichar(url, char_data[i]);

	g_assert(mn_shell != NULL);

	mailbox = mn_mailbox_new_from_uri(url->str);
	if (mailbox)
	  {
	    mn_mailbox_seal(mailbox);
	    mn_mailboxes_queue_add(mn_shell->mailboxes, mailbox);
	    g_object_unref(mailbox);
	  }
	else
	  mn_invalid_uri_dialog(mn_widget_get_parent_window(widget), _("A drag and drop error has occurred"), url->str);

	g_string_free(url, TRUE);
      }
      break;
    }
}

gboolean
mn_parse_gnome_copied_files (const char *gnome_copied_files,
			     MNGnomeCopiedFilesType *type,
			     GSList **uri_list)
{
  char **strv;
  gboolean status = FALSE;

  g_return_val_if_fail(gnome_copied_files != NULL, FALSE);
  g_return_val_if_fail(type != NULL, FALSE);
  g_return_val_if_fail(uri_list != NULL, FALSE);

  strv = g_strsplit(gnome_copied_files, "\n", 0);
  if (strv[0])
    {
      int i;

      if (! strcmp(strv[0], "cut"))
	{
	  status = TRUE;
	  *type = MN_GNOME_COPIED_FILES_CUT;
	}
      else if (! strcmp(strv[0], "copy"))
	{
	  status = TRUE;
	  *type = MN_GNOME_COPIED_FILES_COPY;
	}

      if (status)
	{
	  *uri_list = NULL;
	  for (i = 1; strv[i]; i++)
	    *uri_list = g_slist_append(*uri_list, g_strdup(strv[i]));
	}
    }

  g_strfreev(strv);
  return status;
}

void
mn_display_help (GtkWindow *parent, const char *link_id)
{
  GError *err = NULL;

  if (! gnome_help_display("mail-notification.xml", link_id, &err))
    {
      mn_error_dialog(parent, _("Unable to display help"), "%s", err->message);
      g_error_free(err);
    }
}

void
mn_open_link (GtkWindow *parent, const char *url)
{
  GError *err = NULL;

  if (! gnome_url_show(url, &err))
    {
      mn_error_dialog(parent, _("Unable to open link"), "%s", err->message);
      g_error_free(err);
    }
}

void
mn_thread_create (GThreadFunc func, gpointer data)
{
  GError *err = NULL;

  g_return_if_fail(func != NULL);

  if (! g_thread_create(func, data, FALSE, &err))
    {
      mn_fatal_error_dialog(NULL, _("Unable to create a thread: %s."), err->message);
      g_error_free(err);
    }
}

/**
 * mn_menu_shell_append:
 * @shell: the #GtkMenuShell to append to
 * @stock_id: the stock ID of the item or %NULL
 * @mnemonic: the mnemonic of the item or %NULL
 *
 * Creates a new menu item, shows it and appends it to @shell.
 *
 * If both @stock_id and @mnemonic are provided, a #GtkImageMenuItem
 * will be created using the text of @mnemonic and the icon of
 * @stock_id.
 *
 * If only @stock_id is provided, a #GtkImageMenuitem will be created
 * using the text and icon of @stock_id.
 *
 * If only @mnemonic is provided, a #GtkMenuItem will be created using
 * the text of @mnemonic.
 *
 * If @stock_id and @mnemonic are both %NULL, a #GtkSeparatorMenuItem
 * will be created.
 *
 * Return value: the new menu item.
 **/
GtkWidget *
mn_menu_shell_append (GtkMenuShell *shell,
		      const char *stock_id,
		      const char *mnemonic)
{
  GtkWidget *item;

  g_return_val_if_fail(GTK_IS_MENU_SHELL(shell), NULL);

  item = mn_menu_item_new(stock_id, mnemonic);
  gtk_menu_shell_append(shell, item);
  gtk_widget_show(item);

  return item;
}

static GtkWidget *
mn_menu_item_new (const char *stock_id, const char *mnemonic)
{
  GtkWidget *item;

  if (stock_id && mnemonic)
    {
      GtkWidget *image;

      item = gtk_image_menu_item_new_with_mnemonic(mnemonic);

      image = gtk_image_new_from_stock(stock_id, GTK_ICON_SIZE_MENU);
      gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
      gtk_widget_show(image);
    }
  else if (stock_id)
    item = gtk_image_menu_item_new_from_stock(stock_id, NULL);
  else if (mnemonic)
    item = gtk_menu_item_new_with_mnemonic(mnemonic);
  else
    item = gtk_separator_menu_item_new();

  return item;
}

static void
mn_error_dialog_real (GtkWindow *parent,
		      MNDialogFlags flags,
		      const char *primary,
		      const char *format,
		      va_list args)
{
  char *secondary;
  GtkWidget *dialog;

  g_return_if_fail(primary != NULL);
  g_return_if_fail(format != NULL);

  secondary = g_strdup_vprintf(format, args);
  dialog = mn_alert_dialog_new(parent, GTK_MESSAGE_ERROR, flags, primary, secondary);
  g_free(secondary);

  gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

  gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

  if ((flags & MN_DIALOG_BLOCKING) != 0)
    {
      gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);
    }
  else
    {
      g_signal_connect_swapped(dialog,
			       "response",
			       G_CALLBACK(gtk_widget_destroy),
			       dialog);
      gtk_widget_show(dialog);
    }
}

void
mn_error_dialog (GtkWindow *parent,
		 const char *primary,
		 const char *format,
		 ...)
{
  va_list args;

  g_return_if_fail(primary != NULL);
  g_return_if_fail(format != NULL);

  va_start(args, format);
  mn_error_dialog_real(parent, 0, primary, format, args);
  va_end(args);
}

/* only the secondary text can have markup */
void
mn_error_dialog_with_markup (GtkWindow *parent,
			     const char *primary,
			     const char *format,
			     ...)
{
  va_list args;

  g_return_if_fail(primary != NULL);
  g_return_if_fail(format != NULL);

  va_start(args, format);
  mn_error_dialog_real(parent, MN_DIALOG_MARKUP, primary, format, args);
  va_end(args);
}

void
mn_invalid_uri_dialog (GtkWindow *parent,
		       const char *primary,
		       const char *invalid_uri)
{
  GSList *list = NULL;

  g_return_if_fail(primary != NULL);
  g_return_if_fail(invalid_uri != NULL);

  list = g_slist_append(list, (gpointer) invalid_uri);
  mn_invalid_uri_list_dialog(parent, primary, list);
  g_slist_free(list);
}

void
mn_invalid_uri_list_dialog (GtkWindow *parent,
			    const char *primary,
			    const GSList *invalid_uri_list)
{
  GString *string;
  const GSList *l;

  g_return_if_fail(primary != NULL);
  g_return_if_fail(invalid_uri_list != NULL);

  string = g_string_new(NULL);

  MN_LIST_FOREACH(l, invalid_uri_list)
    {
      const char *uri = l->data;

      g_string_append(string, uri);
      if (l->next)
	g_string_append_c(string, '\n');
    }

  mn_error_dialog(parent,
		  primary,
		  ngettext("The following location is invalid:\n\n%s",
			   "The following locations are invalid:\n\n%s",
			   g_slist_length((GSList *) invalid_uri_list)),
		  string->str);

  g_string_free(string, TRUE);
}

void
mn_fatal_error_dialog (GtkWindow *parent, const char *format, ...)
{
  va_list args;

  g_assert(format != NULL);

  va_start(args, format);
  mn_error_dialog_real(parent, MN_DIALOG_BLOCKING, _("A fatal error has occurred in Mail Notification"), format, args);
  va_end(args);

  exit(1);
}

GtkWidget *
mn_alert_dialog_new (GtkWindow *parent,
		     GtkMessageType type,
		     MNDialogFlags flags,
		     const char *primary,
		     const char *secondary)
{
  GtkWidget *dialog;

  g_return_val_if_fail(primary != NULL, NULL);
  g_return_val_if_fail(secondary != NULL, NULL);

  dialog = gtk_message_dialog_new(parent,
				  GTK_DIALOG_DESTROY_WITH_PARENT,
				  type,
				  GTK_BUTTONS_NONE,
				  "%s",
				  primary);

  if ((flags & MN_DIALOG_MARKUP) != 0)
    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dialog), "%s", secondary);
  else
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", secondary);

  gtk_window_set_title(GTK_WINDOW(dialog), ""); /* HIG */

  return dialog;
}

time_t
mn_time (void)
{
  time_t t;

  t = time(NULL);
  if (t < 0)
    {
      t = 0;
      g_warning(_("unable to get current time: %s"), g_strerror(errno));
    }

  return t;
}

void
mn_g_object_null_unref (gpointer object)
{
  if (object)
    g_object_unref(object);
}

/**
 * mn_g_object_connect:
 * @object: the object to associate the handlers with
 * @instance: the instance to connect to
 * @signal_spec: the spec for the first signal
 * @...: #GCallback for the first signal, followed by data for the
 *       first signal, followed optionally by more signal spec/callback/data
 *       triples, followed by NULL
 *
 * Connects to one or more signals of @instance, associating the
 * handlers with @object. The handlers will be disconnected whenever
 * @object is finalized.
 *
 * Note: this function is not thread-safe. If @object and @instance
 * are finalized concurrently, the behaviour is undefined.
 *
 * The signals specs must be in the same format than those passed to
 * g_object_connect(), except that object-signal,
 * swapped-object-signal, object-signal-after and
 * swapped-object-signal-after are not accepted.
 *
 * Return value: @object
 **/
gpointer
mn_g_object_connect (gpointer object,
		     gpointer instance,
		     const char *signal_spec,
		     ...)
{
  va_list args;

  g_return_val_if_fail(G_IS_OBJECT(object), NULL);
  g_return_val_if_fail(G_IS_OBJECT(instance), NULL);

  va_start(args, signal_spec);
  while (signal_spec)
    {
      GCallback callback = va_arg(args, GCallback);
      gpointer data = va_arg(args, gpointer);
      SignalHandler *handler;

      handler = g_new(SignalHandler, 1);
      handler->instance = instance;

      if (g_str_has_prefix(signal_spec, "signal::"))
	handler->id = g_signal_connect(instance, signal_spec + 8, callback, data);
      else if (g_str_has_prefix(signal_spec, "swapped_signal")
	       || g_str_has_prefix(signal_spec, "swapped-signal"))
	handler->id = g_signal_connect_swapped(instance, signal_spec + 16, callback, data);
      else if (g_str_has_prefix(signal_spec, "signal_after::")
	       || g_str_has_prefix(signal_spec, "signal-after::"))
	handler->id = g_signal_connect_after(instance, signal_spec + 14, callback, data);
      else if (g_str_has_prefix(signal_spec, "swapped_signal_after::")
	       || g_str_has_prefix(signal_spec, "swapped-signal-after::"))
	handler->id = g_signal_connect_data(instance, signal_spec + 22, callback, data, NULL, G_CONNECT_AFTER | G_CONNECT_SWAPPED);
      else
	g_critical("invalid signal specification \"%s\"", signal_spec);

      eel_add_weak_pointer(&handler->instance);
      g_object_weak_ref(object, mn_g_object_connect_weak_notify_cb, handler);

      signal_spec = va_arg(args, const char *);
    }
  va_end(args);

  return object;
}

static void
mn_g_object_connect_weak_notify_cb (gpointer data, GObject *former_object)
{
  SignalHandler *handler = data;

  if (handler->instance)
    {
      g_signal_handler_disconnect(handler->instance, handler->id);
      eel_remove_weak_pointer(&handler->instance);
    }
  g_free(handler);
}

void
mn_gtk_object_ref_and_sink (GtkObject *object)
{
  g_return_if_fail(GTK_IS_OBJECT(object));

  g_object_ref(object);
  gtk_object_sink(object);
}

int
mn_utf8_strcmp (const char *s1, const char *s2)
{
  char *normalized_s1;
  char *normalized_s2;
  int cmp;

  g_return_val_if_fail(s1 != NULL, 0);
  g_return_val_if_fail(s2 != NULL, 0);

  normalized_s1 = g_utf8_normalize(s1, -1, G_NORMALIZE_ALL);
  normalized_s2 = g_utf8_normalize(s2, -1, G_NORMALIZE_ALL);

  cmp = strcmp(normalized_s1, normalized_s2);

  g_free(normalized_s1);
  g_free(normalized_s2);

  return cmp;
}

int
mn_utf8_strcasecmp (const char *s1, const char *s2)
{
  char *normalized_s1;
  char *normalized_s2;
  char *folded_s1;
  char *folded_s2;
  int cmp;

  g_return_val_if_fail(s1 != NULL, 0);
  g_return_val_if_fail(s2 != NULL, 0);

  normalized_s1 = g_utf8_normalize(s1, -1, G_NORMALIZE_ALL);
  normalized_s2 = g_utf8_normalize(s2, -1, G_NORMALIZE_ALL);
  folded_s1 = g_utf8_casefold(normalized_s1, -1);
  folded_s2 = g_utf8_casefold(normalized_s2, -1);

  cmp = strcmp(folded_s1, folded_s2);

  g_free(normalized_s1);
  g_free(normalized_s2);
  g_free(folded_s1);
  g_free(folded_s2);

  return cmp;
}

gboolean
mn_utf8_str_case_has_suffix (const char *str, const char *suffix)
{
  char *normalized_str;
  char *normalized_suffix;
  char *folded_str;
  char *folded_suffix;
  int str_len;
  int suffix_len;
  gboolean has;

  g_return_val_if_fail(str != NULL, FALSE);
  g_return_val_if_fail(suffix != NULL, FALSE);

  normalized_str = g_utf8_normalize(str, -1, G_NORMALIZE_ALL);
  normalized_suffix = g_utf8_normalize(suffix, -1, G_NORMALIZE_ALL);
  folded_str = g_utf8_casefold(normalized_str, -1);
  folded_suffix = g_utf8_casefold(normalized_suffix, -1);

  str_len = strlen(folded_str);
  suffix_len = strlen(folded_suffix);

  has = str_len >= suffix_len
    && ! strcmp(folded_str + str_len - suffix_len, folded_suffix);

  g_free(normalized_str);
  g_free(normalized_suffix);
  g_free(folded_str);
  g_free(folded_suffix);

  return has;
}

char *
mn_utf8_escape (const char *str)
{
  GString *escaped;

  g_return_val_if_fail(str != NULL, NULL);

  escaped = g_string_new(NULL);

  while (*str)
    {
      gunichar c;

      c = g_utf8_get_char_validated(str, -1);
      if (c != (gunichar) -2 && c != (gunichar) -1)
	{
	  g_string_append_unichar(escaped, c);
	  str = g_utf8_next_char(str);
	}
      else
	{
	  g_string_append_printf(escaped, "\\x%02x", (unsigned int) (unsigned char) *str);
	  str++;
	}
    }

  return g_string_free(escaped, FALSE);
}

int
mn_dialog_run_nonmodal (GtkDialog *dialog)
{
  RunNonmodalInfo info = { NULL, GTK_RESPONSE_NONE, FALSE };

  g_return_val_if_fail(GTK_IS_DIALOG(dialog), -1);

  g_object_ref(dialog);

  if (! GTK_WIDGET_VISIBLE(dialog))
    gtk_widget_show(GTK_WIDGET(dialog));

  g_object_connect(dialog,
		   "signal::destroy", mn_dialog_run_nonmodal_destroy_h, &info,
		   "signal::unmap", mn_dialog_run_nonmodal_unmap_h, &info,
		   "signal::response", mn_dialog_run_nonmodal_response_h, &info,
		   "signal::delete-event", mn_dialog_run_nonmodal_delete_event_h, &info,
		   NULL);

  info.loop = g_main_loop_new(NULL, FALSE);

  GDK_THREADS_LEAVE();
  g_main_loop_run(info.loop);
  GDK_THREADS_ENTER();

  g_main_loop_unref(info.loop);

  if (! info.destroyed)
    g_object_disconnect(dialog,
			"any-signal", mn_dialog_run_nonmodal_destroy_h, &info,
			"any-signal", mn_dialog_run_nonmodal_unmap_h, &info,
			"any-signal", mn_dialog_run_nonmodal_response_h, &info,
			"any-signal", mn_dialog_run_nonmodal_delete_event_h, &info,
			NULL);

  g_object_unref(dialog);

  return info.response;
}

static void
mn_dialog_run_nonmodal_destroy_h (GtkObject *object, gpointer user_data)
{
  RunNonmodalInfo *info = user_data;

  info->destroyed = TRUE;

  /*
   * mn_dialog_run_nonmodal_shutdown_loop() will be called by
   * mn_dialog_run_nonmodal_unmap_h()
   */
}

static void
mn_dialog_run_nonmodal_unmap_h (GtkWidget *widget, gpointer user_data)
{
  RunNonmodalInfo *info = user_data;

  mn_dialog_run_nonmodal_shutdown_loop(info);
}

static void
mn_dialog_run_nonmodal_response_h (GtkDialog *dialog,
				   int response,
				   gpointer user_data)
{
  RunNonmodalInfo *info = user_data;

  info->response = response;

  mn_dialog_run_nonmodal_shutdown_loop(info);
}

static gboolean
mn_dialog_run_nonmodal_delete_event_h (GtkWidget *widget,
				       GdkEvent *event,
				       gpointer user_data)
{
  RunNonmodalInfo *info = user_data;

  mn_dialog_run_nonmodal_shutdown_loop(info);

  return TRUE;			/* do not destroy */
}

static void
mn_dialog_run_nonmodal_shutdown_loop (RunNonmodalInfo *info)
{
  g_return_if_fail(info != NULL);

  if (g_main_loop_is_running(info->loop))
    g_main_loop_quit(info->loop);
}

void
mn_source_clear (unsigned int *tag)
{
  g_return_if_fail(tag != NULL);

  if (*tag)
    {
      g_source_remove(*tag);
      *tag = 0;
    }
}

gboolean
mn_ascii_str_case_has_prefix (const char *str, const char *prefix)
{
  int str_len;
  int prefix_len;

  g_return_val_if_fail(str != NULL, FALSE);
  g_return_val_if_fail(prefix != NULL, FALSE);

  str_len = strlen(str);
  prefix_len = strlen(prefix);

  if (str_len < prefix_len)
    return FALSE;

  return g_ascii_strncasecmp(str, prefix, prefix_len) == 0;
}

char *
mn_ascii_strcasestr (const char *big, const char *little)
{
  char *lower_big;
  char *lower_little;
  char *s;

  g_return_val_if_fail(big != NULL, NULL);
  g_return_val_if_fail(little != NULL, NULL);

  lower_big = g_ascii_strdown(big, -1);
  lower_little = g_ascii_strdown(little, -1);

  s = strstr(lower_big, lower_little);

  g_free(lower_big);
  g_free(lower_little);

  return s ? (char *) big + (s - lower_big) : NULL;
}

char *
mn_format_past_time (time_t past_time, time_t now)
{
  time_t diff;

  g_return_val_if_fail(past_time > 0, NULL);

  diff = now - past_time;
  if (diff >= 0)
    {
      if (diff < 60)
	return g_strdup_printf(ngettext("%i second ago", "%i seconds ago", (int) diff), (int) diff);
      else if (diff < 60 * 60)
	{
	  int minutes = diff / 60;
	  return g_strdup_printf(ngettext("about %i minute ago", "about %i minutes ago", minutes), minutes);
	}
      else if (diff < 60 * 60 * 24)
	{
	  int hours = diff / (60 * 60);
	  return g_strdup_printf(ngettext("about %i hour ago", "about %i hours ago", hours), hours);
	}
      else if (diff < 60 * 60 * 24 * 7)
	{
	  int days = diff / (60 * 60 * 24);
	  return g_strdup_printf(ngettext("about %i day ago", "about %i days ago", days), days);
	}
      else
	{
	  int weeks = diff / (60 * 60 * 24 * 7);
	  return g_strdup_printf(ngettext("about %i week ago", "about %i weeks ago", weeks), weeks);
	}
    }
  else				/* future time: simply format it */
    {
      struct tm *tm;
      char *formatted;

      tm = localtime(&past_time);
      g_assert(tm != NULL);

      formatted = eel_strdup_strftime("%c", tm);
      if (! formatted)
	formatted = g_strdup(_("unknown date"));

      return formatted;
    }
}

void
mn_gdk_threads_enter (void)
{
  GDK_THREADS_ENTER();
}

void
mn_gdk_threads_leave (void)
{
  GDK_THREADS_LEAVE();
}

void
mn_g_static_mutex_lock (GStaticMutex *mutex)
{
  g_static_mutex_lock(mutex);
}

void
mn_g_static_mutex_unlock (GStaticMutex *mutex)
{
  g_static_mutex_unlock(mutex);
}

char *
mn_subst_command (const char *command,
		  MNSubstCommandFunction subst,
		  gpointer data,
		  GError **err)
{
  GString *result;
  const char *p;

  g_return_val_if_fail(command != NULL, NULL);
  g_return_val_if_fail(subst != NULL, NULL);

  result = g_string_new(NULL);

  for (p = command; *p;)
    if (*p == '%')
      {
	char *name = NULL;

	if (p[1] == '%')
	  {
	    g_string_append_c(result, '%');
	    p += 2;
	  }
	else if (p[1] == '{')
	  {
	    char *end;

	    end = strchr(p + 2, '}');
	    if (! end)
	      {
		g_set_error(err, 0, 0, _("unterminated substitution"));
		goto error;
	      }

	    name = g_strndup(p + 2, end - p - 2);
	    p = end + 1;
	  }
	else
	  {
	    const char *end = p + 1;

	    while (g_ascii_isalnum(*end) || *end == '-' || *end == '_')
	      end++;

	    name = g_strndup(p + 1, end - p - 1);
	    p = end;
	  }

	if (name)
	  {
	    gboolean ok = FALSE;

	    if (*name)
	      {
		char *value;

		if (subst(name, &value, data))
		  {
		    char *quoted;

		    quoted = mn_shell_quote_safe(value ? value : "");
		    g_free(value);

		    g_string_append(result, quoted);
		    g_free(quoted);

		    ok = TRUE;
		  }
		else
		  g_set_error(err, 0, 0, _("unknown substitution \"%s\""), name);
	      }
	    else
	      g_set_error(err, 0, 0, _("empty substitution"));

	    g_free(name);
	    if (! ok)
	      goto error;
	  }
      }
    else
      {
	g_string_append_c(result, *p);
	p++;
      }

  goto end;			/* success */

 error:
  g_string_free(result, TRUE);
  result = NULL;

 end:
  return result ? g_string_free(result, FALSE) : NULL;
}

static void
mn_handle_execute_result (int status, const char *command)
{
  if (status < 0)
    mn_error_dialog(NULL,
		    _("A command error has occurred in Mail Notification"),
		    _("Unable to execute \"%s\": %s."),
		    command,
		    g_strerror(errno));
}

void
mn_execute_command (const char *command)
{
  g_return_if_fail(command != NULL);

  mn_handle_execute_result(gnome_execute_shell(NULL, command), command);
}

void
mn_execute_command_in_terminal (const char *command)
{
  g_return_if_fail(command != NULL);

  mn_handle_execute_result(gnome_execute_terminal_shell(NULL, command), command);
}

/**
 * mn_shell_quote_safe:
 * @unquoted_string: a literal string
 *
 * Like g_shell_quote(), but guarantees that the string will be quoted
 * using single quotes, therefore making sure that backticks will not
 * be processed.
 *
 * Return value: the quoted string
 **/
char *
mn_shell_quote_safe (const char *unquoted_string)
{
  GString *result;
  int i;

  g_return_val_if_fail(unquoted_string != NULL, NULL);

  result = g_string_new("'");

  for (i = 0; unquoted_string[i]; i++)
    if (unquoted_string[i] == '\'')
      g_string_append(result, "'\\''");
    else
      g_string_append_c(result, unquoted_string[i]);

  g_string_append_c(result, '\'');

  return g_string_free(result, FALSE);
}

GtkWidget *
mn_hig_section_new (const char *title,
		    GtkWidget **label,
		    GtkWidget **alignment)
{
  GtkWidget *section;
  char *markup;
  GtkWidget *_label;
  GtkWidget *_alignment;

  g_return_val_if_fail(title != NULL, NULL);

  section = gtk_vbox_new(FALSE, 6);

  markup = g_markup_printf_escaped("<span weight=\"bold\">%s</span>", title);
  _label = gtk_label_new(markup);
  g_free(markup);

  gtk_misc_set_alignment(GTK_MISC(_label), 0.0, 0.5);
  gtk_label_set_use_markup(GTK_LABEL(_label), TRUE);

  gtk_box_pack_start(GTK_BOX(section), _label, FALSE, FALSE, 0);

  _alignment = gtk_alignment_new(0.5, 0.5, 1.0, 1.0);
  gtk_alignment_set_padding(GTK_ALIGNMENT(_alignment), 0, 0, 12, 0);

  gtk_box_pack_start(GTK_BOX(section), _alignment, TRUE, TRUE, 0);

  gtk_widget_show(_label);
  gtk_widget_show(_alignment);

  if (label)
    *label = _label;
  if (alignment)
    *alignment = _alignment;

  return section;
}

GtkWidget *
mn_hig_section_new_with_box (const char *title,
			     GtkWidget **label,
			     GtkWidget **vbox)
{
  GtkWidget *section;
  GtkWidget *alignment;
  GtkWidget *_vbox;

  g_return_val_if_fail(title != NULL, NULL);

  section = mn_hig_section_new(title, label, &alignment);

  _vbox = gtk_vbox_new(FALSE, 6);
  gtk_container_add(GTK_CONTAINER(alignment), _vbox);
  gtk_widget_show(_vbox);

  if (vbox)
    *vbox = _vbox;

  return section;
}

char *
mn_g_value_to_string (const GValue *value)
{
  char *str;

  g_return_val_if_fail(G_IS_VALUE(value), NULL);

  /*
   * We only handle types which we actually export (grep for
   * MN_MESSAGE_PARAM_EXPORT and MN_MAILBOX_PARAM.*SAVE).
   */

  if (G_VALUE_HOLDS_INT(value))
    str = g_strdup_printf("%i", g_value_get_int(value));
  else if (G_VALUE_HOLDS_ULONG(value))
    str = g_strdup_printf("%lu", g_value_get_ulong(value));
  else if (G_VALUE_HOLDS_STRING(value))
    str = g_value_dup_string(value);
  else if (G_VALUE_HOLDS_ENUM(value))
    {
      GEnumClass *enum_class;
      GEnumValue *enum_value;

      enum_class = g_type_class_ref(G_VALUE_TYPE(value));
      enum_value = g_enum_get_value(enum_class, g_value_get_enum(value));
      g_assert(enum_value != NULL);

      str = g_strdup(enum_value->value_nick);
      g_type_class_unref(enum_class);
    }
  else
    g_return_val_if_reached(NULL);

  return str;
}

gboolean
mn_g_value_from_string (GValue *value, const char *str)
{
  g_return_val_if_fail(G_IS_VALUE(value), FALSE);
  g_return_val_if_fail(str != NULL, FALSE);

  /*
   * We only handle types which we actually import (grep for
   * MN_MAILBOX_PARAM_LOAD).
   */

  if (G_VALUE_HOLDS_INT(value))
    {
      int n;
      char *endptr;

      n = strtol(str, &endptr, 10);
      if (*endptr == '\0')	/* successful conversion */
	g_value_set_int(value, n);
      else
	return FALSE;
    }
  else if (G_VALUE_HOLDS_STRING(value))
    g_value_set_string(value, str);
  else if (G_VALUE_HOLDS_ENUM(value))
    {
      GEnumClass *enum_class;
      GEnumValue *enum_value;
      gboolean found;

      enum_class = g_type_class_ref(G_VALUE_TYPE(value));
      enum_value = g_enum_get_value_by_nick(enum_class, str);

      if (enum_value)
	{
	  g_value_set_enum(value, enum_value->value);
	  found = TRUE;
	}
      else
	found = FALSE;

      g_type_class_unref(enum_class);

      if (! found)
	return FALSE;
    }
  else
    g_return_val_if_reached(FALSE);

  return TRUE;
}

void
mn_window_present_from_event (GtkWindow *window)
{
  g_return_if_fail(GTK_IS_WINDOW(window));

  gtk_window_present_with_time(window, gtk_get_current_event_time());
}
