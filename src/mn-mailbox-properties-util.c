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
#include <stdarg.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "mn-mailbox-properties-util.h"
#include "mn-auth-combo-box.h"

/*** functions ***************************************************************/

#if defined(WITH_POP3) || defined(WITH_IMAP) || defined(WITH_GMAIL)
static void mn_mailbox_properties_entry_activate_h (GtkEntry *entry, gpointer user_data);
#endif

/*** implementation **********************************************************/

#if defined(WITH_POP3) || defined(WITH_IMAP) || defined(WITH_GMAIL)
void
mn_mailbox_properties_field_new (GtkVBox *vbox,
				 const char *mnemonic,
				 GtkWidget **label,
				 GtkWidget **entry)
{
  GtkWidget *hbox;

  g_return_if_fail(GTK_IS_VBOX(vbox));
  g_return_if_fail(mnemonic != NULL);
  g_return_if_fail(label != NULL);
  g_return_if_fail(entry != NULL);

  *label = gtk_label_new_with_mnemonic(mnemonic);
  gtk_misc_set_alignment(GTK_MISC(*label), 0.0, 0.5);

  *entry = gtk_entry_new();
  gtk_label_set_mnemonic_widget(GTK_LABEL(*label), *entry);

  hbox = gtk_hbox_new(FALSE, 12);
  gtk_box_pack_start(GTK_BOX(hbox), *label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), *entry, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show_all(hbox);
}

void
mn_mailbox_properties_credentials_new (GtkVBox *vbox,
				       GtkWidget **username_label,
				       GtkWidget **username_entry,
				       GtkWidget **password_label,
				       GtkWidget **password_entry)
{
  const char *username;

  g_return_if_fail(GTK_IS_VBOX(vbox));
  g_return_if_fail(username_label != NULL);
  g_return_if_fail(username_entry != NULL);
  g_return_if_fail(password_label != NULL);
  g_return_if_fail(password_entry != NULL);

  mn_mailbox_properties_field_new(vbox,
				  _("_Username:"),
				  username_label,
				  username_entry);

  /* defaults to the login name */
  username = g_get_user_name();
  if (username)
    gtk_entry_set_text(GTK_ENTRY(*username_entry), username);

  mn_mailbox_properties_field_new(vbox,
				  _("_Password:"),
				  password_label,
				  password_entry);
  gtk_entry_set_visibility(GTK_ENTRY(*password_entry), FALSE);
}

void
mn_mailbox_properties_link_entries (GtkEntry *first, ...)
{
  va_list args;
  GtkEntry *entry = first;
  GtkEntry *next;

  g_return_if_fail(GTK_IS_ENTRY(first));

  va_start(args, first);
  do
    {
      next = va_arg(args, GtkEntry *);
      if (next)
	g_return_if_fail(GTK_IS_ENTRY(next));

      g_signal_connect(G_OBJECT(entry),
		       "activate",
		       G_CALLBACK(mn_mailbox_properties_entry_activate_h),
		       next ? next : first);
      entry = next;
    }
  while (next);
  va_end(args);
}

static void
mn_mailbox_properties_entry_activate_h (GtkEntry *entry, gpointer user_data)
{
  GtkWidget *next = user_data;
  GtkWidget *toplevel;

  toplevel = gtk_widget_get_toplevel(GTK_WIDGET(entry));
  if (GTK_WIDGET_TOPLEVEL(toplevel)
      && GTK_WINDOW(toplevel)->default_widget
      && GTK_WIDGET_IS_SENSITIVE(GTK_WINDOW(toplevel)->default_widget))
    gtk_window_activate_default(GTK_WINDOW(toplevel));
  else
    gtk_widget_grab_focus(next);
}
#endif /* WITH_POP3 || WITH_IMAP || WITH_GMAIL */

#if defined(WITH_POP3) || defined(WITH_IMAP)
void
mn_mailbox_properties_connection_type_new (GtkVBox *vbox,
					   const char *mnemonic,
					   int default_port,
					   GtkRadioButton *radio_group,
					   GtkWidget **label,
					   GtkWidget **radio,
					   GtkWidget **spin)
{
  GtkWidget *hbox;
  GtkWidget *port_label;
  
  g_return_if_fail(GTK_IS_VBOX(vbox));
  g_return_if_fail(mnemonic != NULL);
  g_return_if_fail(label != NULL);
  g_return_if_fail(radio != NULL);
  g_return_if_fail(spin != NULL);

  *label = gtk_label_new(radio_group ? NULL : _("Connection type:"));
  gtk_misc_set_alignment(GTK_MISC(*label), 0.0, 0.5);

  *radio = radio_group
    ? gtk_radio_button_new_with_mnemonic_from_widget(radio_group, mnemonic)
    : gtk_radio_button_new_with_mnemonic(NULL, mnemonic);

  port_label = gtk_label_new(_("Port:"));

  *spin = gtk_spin_button_new_with_range(0, 0xFFFF, 1);
  gtk_entry_set_activates_default(GTK_ENTRY(*spin), TRUE);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(*spin), default_port);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(*spin), TRUE);

  hbox = gtk_hbox_new(FALSE, 12);
  gtk_box_pack_start(GTK_BOX(hbox), *label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), *radio, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), port_label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), *spin, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show_all(hbox);
}

void
mn_mailbox_properties_authentication_new (GtkVBox *vbox,
					  GtkWidget **label,
					  GtkWidget **auth_combo)
{
  GtkWidget *hbox;

  g_return_if_fail(GTK_IS_VBOX(vbox));
  g_return_if_fail(label != NULL);
  g_return_if_fail(auth_combo != NULL);

  hbox = gtk_hbox_new(FALSE, 12);
  *label = gtk_label_new_with_mnemonic(_("_Authentication mechanism:"));
  gtk_misc_set_alignment(GTK_MISC(*label), 0.0, 0.5);

  *auth_combo = mn_auth_combo_box_new();
  gtk_label_set_mnemonic_widget(GTK_LABEL(*label), *auth_combo);

  gtk_box_pack_start(GTK_BOX(hbox), *label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), *auth_combo, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show_all(hbox);
}
#endif /* WITH_POP3 || WITH_IMAP */
