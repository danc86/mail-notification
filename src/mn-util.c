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
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <gnome.h>
#include <glade/glade.h>
#include <eel/eel.h>
#include <eel/eel-alert-dialog.h>
#include "mn-util.h"
#include "mn-conf.h"
#include "mn-mailboxes.h"
#include "mn-shell.h"

/*** types *******************************************************************/

enum {
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
} RunNonmodalInfo;
  
/*** functions ***************************************************************/

static int mn_g_str_slist_compare_func (gconstpointer a, gconstpointer b);

static void mn_file_chooser_dialog_file_activated_h (GtkFileChooser *chooser,
						     gpointer user_data);
static void mn_file_chooser_dialog_response_h (GtkDialog *dialog,
					       int response_id,
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
				  gboolean blocking,
				  const char *not_again_key,
				  const char *help_link_id,
				  const char *primary,
				  const char *secondary);
static void mn_error_dialog_real_response_h (GtkDialog *dialog,
					     int response,
					     gpointer user_data);

static void mn_g_object_connect_weak_notify_cb (gpointer data,
						GObject *former_object);

static void mn_dialog_run_nonmodal_unmap_h (GtkWidget *widget,
					    gpointer user_data);
static void mn_dialog_run_nonmodal_response_h (GtkDialog *dialog,
					       int response,
					       gpointer user_data);
static gboolean mn_dialog_run_nonmodal_delete_event_h (GtkWidget *widget,
						       GdkEvent *event,
						       gpointer user_data);
static void mn_dialog_run_nonmodal_shutdown_loop (RunNonmodalInfo *info);

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

/**
 * mn_g_slist_delete_link_deep:
 * @list: a #GSList of g_free'able objects
 * @link_: an element in the #GSList
 *
 * Equivalent of g_slist_delete_link() for a list of g_free'able
 * objects.
 *
 * Return value: new head of @list.
 **/
GSList *
mn_g_slist_delete_link_deep (GSList *list, GSList *link_)
{
  return mn_g_slist_delete_link_deep_custom(list, link_, (GFunc) g_free, NULL);
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

/**
 * mn_g_object_slist_delete_link:
 * @list: a #GSList of #GObject instances
 * @link_: an element in the #GSList
 *
 * Equivalent of g_slist_delete_link() for a list of GObject
 * instances.
 *
 * Return value: new head of @list.
 **/
GSList *
mn_g_object_slist_delete_link (GSList *list, GSList *link_)
{
  return mn_g_slist_delete_link_deep_custom(list, link_, (GFunc) g_object_unref, NULL);
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

  if (! *str)
    return FALSE;

  for (i = 0; str[i]; i++)
    if (! g_ascii_isdigit(str[i]))
      return FALSE;

  return TRUE;
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
      g_warning(_("error loading image: %s"), err->message);
      g_error_free(err);
    }

  return pixbuf;
}

void
mn_create_interface (const char *filename, ...)
{
  GladeXML *xml;
  va_list args;
  const char *widget_name;

  g_return_if_fail(filename != NULL);

  xml = glade_xml_new(filename, NULL, NULL);
  g_return_if_fail(xml != NULL);

  glade_xml_signal_autoconnect(xml);

  va_start(args, filename);
  while ((widget_name = va_arg(args, const char *)))
    {
      GtkWidget **widget;

      widget = va_arg(args, GtkWidget **);
      g_return_if_fail(widget != NULL);

      *widget = glade_xml_get_widget(xml, widget_name);
      if (! *widget)
	g_critical(_("widget \"%s\" not found in interface \"%s\""), widget_name, filename);
    }
  va_end(args);
  
  g_object_unref(xml);
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
  const GtkTargetEntry targets[] = {
    { "text/uri-list",	0, TARGET_URI_LIST },
    { "text/x-moz-url",	0, TARGET_MOZ_URL }
  };
    
  g_return_if_fail(GTK_IS_WIDGET(widget));

  gtk_drag_dest_set(widget,
		    GTK_DEST_DEFAULT_ALL,
		    targets,
		    G_N_ELEMENTS(targets),
		    GDK_ACTION_COPY);
  g_signal_connect(widget,
		   "drag-data-received",
		   G_CALLBACK(mn_drag_data_received_h),
		   NULL);
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
	char *str;
	char **uriv;
	int i;
	GSList *new_mailboxes = NULL;

	/* complies to RFC 2483, section 5 */

	if (selection_data->format != 8 || selection_data->length <= 0)
	  {
	    g_warning(_("received an invalid URI list"));
	    return;
	  }

	str = g_strndup(selection_data->data, selection_data->length);
	uriv = g_strsplit(str, "\r\n", 0);
	g_free(str);
	
	for (i = 0; uriv[i]; i++)
	  if (*uriv[i] && *uriv[i] != '#' && ! mn_mailboxes_find(mn_shell->mailboxes, uriv[i]))
	    new_mailboxes = g_slist_append(new_mailboxes, g_strdup(uriv[i]));

	g_strfreev(uriv);

	if (new_mailboxes)
	  {
	    GSList *gconf_mailboxes;
		  
	    gconf_mailboxes = eel_gconf_get_string_list(MN_CONF_MAILBOXES);
	    gconf_mailboxes = g_slist_concat(gconf_mailboxes, new_mailboxes);
	    eel_gconf_set_string_list(MN_CONF_MAILBOXES, gconf_mailboxes);
	    eel_g_slist_free_deep(gconf_mailboxes);
	  }
      }
      break;

    case TARGET_MOZ_URL:
      {
	GString *url;
	const guint16 *char_data;
	int char_len;
	int i;

	/* text/x-moz-url is encoded in UCS-2 but in format 8: broken */
	if (selection_data->format != 8 || selection_data->length <= 0 || (selection_data->length % 2) != 0)
	  {
	    g_warning(_("received an invalid Mozilla URL"));
	    return;
	  }

	char_data = (const guint16 *) selection_data->data;
	char_len = selection_data->length / 2;
	
	url = g_string_new(NULL);
	for (i = 0; i < char_len && char_data[i] != '\n'; i++)
	  g_string_append_unichar(url, char_data[i]);

	g_return_if_fail(mn_shell != NULL);
	if (! mn_mailboxes_find(mn_shell->mailboxes, url->str))
	  {
	    GSList *gconf_mailboxes;
		  
	    gconf_mailboxes = eel_gconf_get_string_list(MN_CONF_MAILBOXES);
	    gconf_mailboxes = g_slist_append(gconf_mailboxes, g_strdup(url->str));
	    eel_gconf_set_string_list(MN_CONF_MAILBOXES, gconf_mailboxes);
	    eel_g_slist_free_deep(gconf_mailboxes);
	  }
	
	g_string_free(url, TRUE);
      }
      break;
    }
}

char *
mn_build_gnome_copied_files (MNGnomeCopiedFilesType type, GSList *uri_list)
{
  GString *string;
  GSList *l;

  string = g_string_new(type == MN_GNOME_COPIED_FILES_CUT ? "cut" : "copy");
  MN_LIST_FOREACH(l, uri_list)
    {
      const char *uri = l->data;

      g_string_append_c(string, '\n');
      g_string_append(string, uri);
    }

  return g_string_free(string, FALSE);
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
      mn_error_dialog(parent, NULL, NULL, _("Unable to display help"), "%s", err->message);
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
		      gboolean blocking,
		      const char *not_again_key,
		      const char *help_link_id,
		      const char *primary,
		      const char *secondary)
{
  GtkWidget *dialog;

  dialog = eel_alert_dialog_new(parent,
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_NONE,
				primary,
				secondary,
				NULL);

  if (not_again_key)
    {
      GtkWidget *alignment;
      GtkWidget *check;

      alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
      check = gtk_check_button_new_with_mnemonic(_("_Do not show this message again"));

      gtk_container_add(GTK_CONTAINER(alignment), check);
      gtk_widget_show_all(alignment);

      gtk_box_pack_end(GTK_BOX(GTK_DIALOG(dialog)->vbox), alignment, FALSE, FALSE, 0);

      mn_conf_link(check, not_again_key, "active", NULL);
    }
  
  if (help_link_id != NULL)
    gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_HELP, GTK_RESPONSE_HELP);
  gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

  gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

  if (blocking)
    {
      while (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_HELP)
	mn_display_help(GTK_WINDOW(dialog), help_link_id);
      gtk_widget_destroy(dialog);
    }
  else
    {
      g_signal_connect_data(dialog,
			    "response",
			    G_CALLBACK(mn_error_dialog_real_response_h),
			    g_strdup(help_link_id),
			    (GClosureNotify) g_free,
			    0);
      gtk_widget_show(dialog);
    }
}

static void
mn_error_dialog_real_response_h (GtkDialog *dialog,
				 int response,
				 gpointer user_data)
{
  char *help_link_id = user_data;

  if (response == GTK_RESPONSE_HELP)
    mn_display_help(GTK_WINDOW(dialog), help_link_id);
  else
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void
mn_error_dialog (GtkWindow *parent,
		 const char *not_again_key,
		 const char *help_link_id,
		 const char *primary,
		 const char *format,
		 ...)
{
  char *secondary;

  if (format)
    {
      va_list args;
      
      va_start(args, format);
      secondary = g_strdup_vprintf(format, args);
      va_end(args);
    }
  else
    secondary = NULL;

  mn_error_dialog_real(parent, FALSE, not_again_key, help_link_id, primary, secondary);
  g_free(secondary);
}

void
mn_fatal_error_dialog (GtkWindow *parent, const char *format, ...)
{
  va_list args;
  char *secondary;

  g_assert(format != NULL);

  va_start(args, format);
  secondary = g_strdup_vprintf(format, args);
  va_end(args);

  mn_error_dialog_real(parent, TRUE, NULL, NULL, _("A fatal error has occurred in Mail Notification"), secondary);
  g_free(secondary);

  exit(1);  
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
	g_critical(_("invalid signal specification \"%s\""), signal_spec);

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
mn_execute_command (const char *conf_key)
{
  char *command;

  g_return_if_fail(conf_key != NULL);

  command = eel_gconf_get_string(conf_key);
  g_return_if_fail(command != NULL && *command != 0);

  if (gnome_execute_shell(NULL, command) < 0)
    mn_error_dialog(NULL,
		    NULL,
		    NULL,
		    _("A command error has occurred in Mail Notification"),
		    _("Unable to execute \"%s\": %s."),
		    command,
		    g_strerror(errno));
  g_free(command);
}

void
mn_gtk_object_ref_and_sink (GtkObject *object)
{
  g_return_if_fail(GTK_IS_OBJECT(object));
  
  g_object_ref(object);
  gtk_object_sink(object);
}

int
mn_utf8_strcasecoll (const char *s1, const char *s2)
{
  char *folded_s1;
  char *folded_s2;
  int coll;

  g_return_val_if_fail(s1 != NULL, 0);
  g_return_val_if_fail(s2 != NULL, 0);

  folded_s1 = g_utf8_casefold(s1, -1);
  folded_s2 = g_utf8_casefold(s2, -1);

  coll = g_utf8_collate(folded_s1, folded_s2);

  g_free(folded_s1);
  g_free(folded_s2);

  return coll;
}

int
mn_dialog_run_nonmodal (GtkDialog *dialog)
{
  RunNonmodalInfo info = { NULL, GTK_RESPONSE_NONE };
  
  g_return_val_if_fail(GTK_IS_DIALOG(dialog), -1);

  g_object_ref(dialog);

  if (! GTK_WIDGET_VISIBLE(dialog))
    gtk_widget_show(GTK_WIDGET(dialog));

  g_object_connect(dialog,
		   "signal::unmap", mn_dialog_run_nonmodal_unmap_h, &info,
		   "signal::response", mn_dialog_run_nonmodal_response_h, &info,
		   "signal::delete-event", mn_dialog_run_nonmodal_delete_event_h, &info,
		   NULL);

  info.loop = g_main_loop_new(NULL, FALSE);

  GDK_THREADS_LEAVE();
  g_main_loop_run(info.loop);
  GDK_THREADS_ENTER();

  g_main_loop_unref(info.loop);

  g_object_disconnect(dialog,
		      "any-signal", mn_dialog_run_nonmodal_unmap_h, &info,
		      "any-signal", mn_dialog_run_nonmodal_response_h, &info,
		      "any-signal", mn_dialog_run_nonmodal_delete_event_h, &info,
		      NULL);

  g_object_unref(dialog);

  return info.response;
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

gboolean
mn_ascii_validate (const char *str)
{
  g_return_val_if_fail(str != NULL, FALSE);

  for (; *str; str++)
    if ((unsigned char) *str > 127)
      return FALSE;

  return TRUE;
}

void
mn_source_remove (unsigned int *tag)
{
  g_return_if_fail(tag != NULL);

  if (*tag)
    {
      g_source_remove(*tag);
      *tag = 0;
    }
}

unsigned int
mn_timeout_add (const char *minutes_key,
		const char *seconds_key,
		GSourceFunc function,
		gpointer data)
{
  int minutes;
  int seconds;

  g_return_val_if_fail(minutes_key != NULL, 0);
  g_return_val_if_fail(seconds_key != NULL, 0);

  minutes = eel_gconf_get_integer(minutes_key);
  seconds = eel_gconf_get_integer(seconds_key);

  return minutes != 0 || seconds != 0
    ? g_timeout_add(((minutes * 60) + seconds) * 1000, function, data)
    : 0;
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
