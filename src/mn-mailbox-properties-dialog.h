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

#ifndef _MN_MAILBOX_PROPERTIES_DIALOG_H
#define _MN_MAILBOX_PROPERTIES_DIALOG_H

#include <gtk/gtk.h>
#include "mn-uri.h"

#define MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE_KEY	"mn-mailbox-properties-dialog-private"
#define MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(obj)	(g_object_get_data(G_OBJECT(obj), MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE_KEY))

#define MN_TYPE_MAILBOX_PROPERTIES_DIALOG		(GTK_TYPE_DIALOG)
#define MN_MAILBOX_PROPERTIES_DIALOG(obj)		(GTK_DIALOG(obj))
#define MN_MAILBOX_PROPERTIES_DIALOG_CLASS(klass)	(GTK_DIALOG_CLASS(klass))
#define MN_IS_MAILBOX_PROPERTIES_DIALOG(obj)		(GTK_IS_DIALOG(obj) && MN_MAILBOX_PROPERTIES_DIALOG_PRIVATE(obj))
#define MN_IS_MAILBOX_PROPERTIES_DIALOG_CLASS(klass)	(GTK_IS_DIALOG_CLASS(klass))
#define MN_MAILBOX_PROPERTIES_DIALOG_GET_CLASS(obj)	(GTK_DIALOG_GET_CLASS(obj))

typedef GtkDialog MNMailboxPropertiesDialog;
typedef GtkDialogClass MNMailboxPropertiesDialogClass;

typedef enum
{
  MN_MAILBOX_PROPERTIES_DIALOG_MODE_ADD,
  MN_MAILBOX_PROPERTIES_DIALOG_MODE_EDIT
} MNMailboxPropertiesDialogMode;

GtkWidget *mn_mailbox_properties_dialog_new (GtkWindow *parent,
					     MNMailboxPropertiesDialogMode mode);

void mn_mailbox_properties_dialog_set_uri (MNMailboxPropertiesDialog *dialog,
					   MNURI *uri);
MNURI *mn_mailbox_properties_dialog_get_uri (MNMailboxPropertiesDialog *dialog);

void mn_mailbox_properties_dialog_apply (MNMailboxPropertiesDialog *dialog);

#endif /* _MN_MAILBOX_PROPERTIES_DIALOG_H */
