/* 
 * Copyright (c) 2003 Jean-Yves Lefort <jylefort@brutele.be>
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
#include <stdarg.h>
#include <stdlib.h>
#include "mn-util.h"

/*** implementation **********************************************************/

void
mn_notice (const char *format, ...)
{
  va_list args;
  char *message;

  va_start(args, format);
  message = g_strdup_vprintf(format, args);
  va_end(args);

  g_printerr(PACKAGE ": %s\n", message);
}

void
mn_fatal (const char *format, ...)
{
  va_list args;
  char *message;

  va_start(args, format);
  message = g_strdup_vprintf(format, args);
  va_end(args);

  g_printerr(_("%s: FATAL ERROR: %s\n"), PACKAGE, message);
  exit(1);
}

/*
 * Displays an HIG-compliant modal dialog.
 *
 * Actually, it won't fully comply to the HIG because of
 * http://bugzilla.gnome.org/show_bug.cgi?id=98779.
 */
void
mn_error_dialog (const char *primary,
		 const char *format,
		 ...)
{
  GladeXML *xml;
  GtkWidget *dialog;
  GtkWidget *label;
  char *secondary = NULL;
  GString *message;

  xml = mn_glade_xml_new("dialog");

  dialog = glade_xml_get_widget(xml, "dialog");
  label = glade_xml_get_widget(xml, "label");
  
  gtk_window_set_title(GTK_WINDOW(dialog), "");
  gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);

  message = g_string_new(NULL);

  if (format)
    {
      va_list args;
      
      va_start(args, format);
      secondary = g_strdup_vprintf(format, args);
      va_end(args);
    }

  if (primary)
    g_string_printf(message,
		    "<span weight=\"bold\" size=\"larger\">%s</span>",
		    primary);

  if (secondary)
    {
      if (primary)
	g_string_append(message, "\n\n");
      
      g_string_append(message, secondary);
    }
  
  g_free(secondary);

  gtk_label_set_markup(GTK_LABEL(label), message->str);
  g_string_free(message, TRUE);
  
  gtk_dialog_run(GTK_DIALOG(dialog));

  gtk_widget_destroy(dialog);
  g_object_unref(xml);
}
