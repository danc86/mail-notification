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
#include <stdarg.h>
#include <stdlib.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "mn-util.h"

/*** implementation **********************************************************/

/*
 * Displays an HIG-compliant modal dialog.
 *
 * Actually, it won't fully comply to the HIG because of
 * http://bugzilla.gnome.org/show_bug.cgi?id=98779.
 */
void
mn_error_dialog (const char *help_link_id,
		 const char *primary,
		 const char *format,
		 ...)
{
  GtkWidget *dialog;
  GtkWidget *label;
  char *secondary = NULL;
  GString *message;
  char *escaped;

  mn_create_interface("dialog",
		      "dialog", &dialog,
		      "label", &label,
		      NULL);

  gtk_window_set_title(GTK_WINDOW(dialog), "");
  
  if (help_link_id != NULL)
    gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_HELP, GTK_RESPONSE_HELP);
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
    {
      escaped = g_markup_printf_escaped("<span weight=\"bold\" size=\"larger\">%s</span>", primary);
      g_string_append(message, escaped);
      g_free(escaped);
    }

  if (secondary)
    {
      if (primary)
	g_string_append(message, "\n\n");
      
      escaped = g_markup_escape_text(secondary, -1);
      g_free(secondary);

      g_string_append(message, escaped);
      g_free(escaped);
    }
  
  gtk_label_set_markup(GTK_LABEL(label), message->str);
  g_string_free(message, TRUE);
  
  while (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_HELP)
    mn_display_help(help_link_id);

  gtk_widget_destroy(dialog);
}
