/* 
 * Copyright (c) 2004 Jean-Yves Lefort <jylefort@brutele.be>
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
#include <eel/eel.h>
#include "mn-util.h"
#include "mn-shell.h"
#include "mn-conf.h"
#include "mn-summary-popup.h"
#include "mn-summary.h"

/*** types *******************************************************************/

typedef struct
{
  GtkWidget	*dialog;

  GtkWidget	*vbox;
  GtkWidget	*mail_reader_button;
  GtkWidget	*update_button;
} SummaryDialog;

/*** variables ***************************************************************/

static SummaryDialog summary = { NULL };
  
/*** functions ***************************************************************/
    
static void mn_summary_dialog_notify_mail_reader_cb (GConfClient *client,
						     guint cnxn_id,
						     GConfEntry *entry,
						     gpointer user_data);

static void mn_summary_dialog_update (void);
static void mn_summary_dialog_update_sensitivity (void);

/*** implementation **********************************************************/

void
mn_summary_dialog_display (void)
{
  g_return_if_fail(mn_shell != NULL);

  /* we don't need the popup anymore */
  mn_summary_popup_destroy();

  if (summary.dialog)
    {
      gtk_window_present(GTK_WINDOW(summary.dialog));
      return;
    }
  
  mn_create_interface("summary-dialog",
		      "dialog", &summary.dialog,
		      "vbox", &summary.vbox,
		      "mail_reader_button", &summary.mail_reader_button,
		      "update_button", &summary.update_button,
		      NULL);

  eel_add_weak_pointer(&summary.dialog);

  mn_conf_link(summary.dialog, MN_CONF_SUMMARY_DIALOG, NULL);

  mn_summary_dialog_update();
  mn_summary_dialog_update_sensitivity();

  mn_conf_notification_add(summary.dialog,
			   MN_CONF_COMMANDS_MAIL_READER_NAMESPACE,
			   mn_summary_dialog_notify_mail_reader_cb,
			   NULL);
  mn_g_object_connect(summary.dialog,
		      mn_shell->mailboxes,
		      "signal::notify::must-poll", mn_summary_dialog_update_sensitivity, NULL,
		      "signal::messages-changed", mn_summary_dialog_update, NULL,
		      NULL);

  gtk_widget_show(summary.dialog);
}

static void
mn_summary_dialog_notify_mail_reader_cb (GConfClient *client,
					 guint cnxn_id,
					 GConfEntry *entry,
					 gpointer user_data)
{
  GDK_THREADS_ENTER();
  mn_summary_dialog_update_sensitivity();
  GDK_THREADS_LEAVE();
}

gboolean
mn_summary_dialog_is_displayed (void)
{
  return summary.dialog != NULL;
}

static void
mn_summary_dialog_update (void)
{
  GSList *messages;

  messages = mn_mailboxes_get_messages(mn_shell->mailboxes);
  mn_summary_update(GTK_VBOX(summary.vbox), messages, TRUE);
  mn_g_object_slist_free(messages);
}

static void
mn_summary_dialog_update_sensitivity (void)
{
  gboolean mail_reader_sensitive = FALSE;

  if (eel_gconf_get_boolean(MN_CONF_COMMANDS_MAIL_READER_ENABLED))
    {
      char *mail_reader;

      mail_reader = eel_gconf_get_string(MN_CONF_COMMANDS_MAIL_READER_COMMAND);
      if (mail_reader)
	{
	  mail_reader_sensitive = TRUE;
	  g_free(mail_reader);
	}
    }

  gtk_widget_set_sensitive(summary.mail_reader_button, mail_reader_sensitive);
  gtk_widget_set_sensitive(summary.update_button, mn_mailboxes_get_must_poll(mn_shell->mailboxes));
}

/* libglade callbacks */

void
mn_summary_dialog_response_h (GtkDialog *dialog,
			      int response,
			      gpointer user_data)
{
  switch (response)
    {
    case GTK_RESPONSE_HELP:
      mn_display_help(GTK_WINDOW(dialog), "mail-summary");
      break;

    case 1:			/* launch mail reader */
      if (eel_gconf_get_boolean(MN_CONF_COMMANDS_MAIL_READER_ENABLED))
	mn_execute_command(MN_CONF_COMMANDS_MAIL_READER_COMMAND);
      break;
      
    case 2:			/* update */
      mn_mailboxes_check(mn_shell->mailboxes);
      break;

    case GTK_RESPONSE_CLOSE:
      gtk_widget_destroy(summary.dialog);
      break;
    }
}
