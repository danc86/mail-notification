/* 
 * Copyright (C) 2004 Jean-Yves Lefort <jylefort@brutele.be>
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

#ifndef _MN_MAILBOX_PROPERTIES_UTIL_H
#define _MN_MAILBOX_PROPERTIES_UTIL_H

#include "config.h"
#include <stdarg.h>
#include <gtk/gtk.h>

#if defined(WITH_POP3) || defined(WITH_IMAP) || defined(WITH_GMAIL)
void mn_mailbox_properties_field_new (GtkVBox *vbox,
				      const char *mnemonic,
				      GtkWidget **label,
				      GtkWidget **entry);
void mn_mailbox_properties_credentials_new (GtkVBox *vbox,
					    GtkWidget **username_label,
					    GtkWidget **username_entry,
					    GtkWidget **password_label,
					    GtkWidget **password_entry);
void mn_mailbox_properties_link_entries (GtkEntry *first, ...);
#endif /* WITH_POP3 || WITH_IMAP || WITH_GMAIL */

#if defined(WITH_POP3) || defined(WITH_IMAP)
void mn_mailbox_properties_connection_type_new (GtkVBox *vbox,
						const char *mnemonic,
						int default_port,
						GtkRadioButton *radio_group,
						GtkWidget **label,
						GtkWidget **radio,
						GtkWidget **spin);
void mn_mailbox_properties_authentication_new (GtkVBox *vbox,
					       GtkWidget **label,
					       GtkWidget **auth_combo);
#endif /* WITH_POP3 || WITH_IMAP */

#endif /* _MN_MAILBOX_PROPERTIES_UTIL_H */
