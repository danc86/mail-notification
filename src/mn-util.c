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
#include <string.h>
#include <stdarg.h>
#include <gnome.h>
#include <glade/glade.h>
#include "mn-util.h"
#include "mn-conf.h"
#include "mn-mailboxes.h"
#include "mn-dialog.h"

/*** types *******************************************************************/

enum {
  TARGET_URI_LIST,
  TARGET_MOZ_URL
};

/*** functions ***************************************************************/

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

/*
 * Free a singly linked list of heap pointers.
 */
void
mn_slist_free (GSList *list)
{
  GSList *l;

  MN_LIST_FOREACH(l, list)
    g_free(l->data);

  g_slist_free(list);
}

gboolean
mn_str_isnumeric (const char *str)
{
  int i;

  g_return_val_if_fail(str != NULL, FALSE);

  for (i = 0; str[i]; i++)
    if (! g_ascii_isdigit(str[i]))
      return FALSE;

  return TRUE;
}

GdkPixbuf *
mn_pixbuf_new (const char *filename)
{
  char *pathname;
  GdkPixbuf *pixbuf;
  GError *err = NULL;

  g_return_val_if_fail(filename != NULL, NULL);

  pathname = g_build_filename(UIDIR, filename, NULL);
  pixbuf = gdk_pixbuf_new_from_file(pathname, &err);
  g_free(pathname);

  if (! pixbuf)
    {
      g_warning(_("error loading image: %s"), err->message);
      g_error_free(err);
    }

  return pixbuf;
}

void
mn_create_interface (const char *name, ...)
{
  char *filename;
  char *pathname;
  GladeXML *xml;
  va_list args;
  const char *widget_name;

  g_return_if_fail(name != NULL);

  filename = g_strconcat(name, ".glade", NULL);
  pathname = g_build_filename(UIDIR, filename, NULL);
  g_free(filename);

  xml = glade_xml_new(pathname, NULL, NULL);
  g_free(pathname);

  g_return_if_fail(xml != NULL);
  glade_xml_signal_autoconnect(xml);

  va_start(args, name);
  while ((widget_name = va_arg(args, const char *)))
    {
      GtkWidget **widget;

      widget = va_arg(args, GtkWidget **);
      g_return_if_fail(widget != NULL);

      *widget = glade_xml_get_widget(xml, widget_name);
      if (! *widget)
	g_critical(_("widget %s not found in interface %s"), widget_name, name);
    }
  va_end(args);
  
  g_object_unref(xml);
}

/*
 * The GtkFileChooser API does not allow a chooser to pick a file
 * (GTK_FILE_CHOOSER_ACTION_OPEN) and select a folder
 * (GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER) at the same time.
 *
 * This function provides a workaround.
 */
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

  g_signal_connect(G_OBJECT(dialog),
		   "file-activated",
		   G_CALLBACK(mn_file_chooser_dialog_file_activated_h),
		   GINT_TO_POINTER(accept_id));
  g_signal_connect(G_OBJECT(dialog),
		   "response",
		   G_CALLBACK(mn_file_chooser_dialog_response_h),
		   GINT_TO_POINTER(accept_id));
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
  g_signal_connect(G_OBJECT(widget),
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
	  if (*uriv[i] && *uriv[i] != '#' && ! mn_mailboxes_find(uriv[i]))
	    new_mailboxes = g_slist_append(new_mailboxes, g_strdup(uriv[i]));

	g_strfreev(uriv);

	if (new_mailboxes)
	  {
	    GSList *gconf_mailboxes;
		  
	    gconf_mailboxes = eel_gconf_get_string_list(MN_CONF_MAILBOXES);
	    gconf_mailboxes = g_slist_concat(gconf_mailboxes, new_mailboxes);
	    eel_gconf_set_string_list(MN_CONF_MAILBOXES, gconf_mailboxes);
	    mn_slist_free(gconf_mailboxes);
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

	if (! mn_mailboxes_find(url->str))
	  {
	    GSList *gconf_mailboxes;
		  
	    gconf_mailboxes = eel_gconf_get_string_list(MN_CONF_MAILBOXES);
	    gconf_mailboxes = g_slist_append(gconf_mailboxes, g_strdup(url->str));
	    eel_gconf_set_string_list(MN_CONF_MAILBOXES, gconf_mailboxes);
	    mn_slist_free(gconf_mailboxes);
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
mn_display_help (const char *link_id)
{
  GError *err = NULL;

  if (! gnome_help_display("mail-notification.xml", link_id, &err))
    {
      mn_error_dialog(NULL, _("Unable to display help."), "%s", err->message);
      g_error_free(err);
    }
}
