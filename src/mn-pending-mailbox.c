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
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "mn-pending-mailbox.h"

/*** functions ***************************************************************/

static void mn_pending_mailbox_class_init (MNPendingMailboxClass *class);

/*** implementation **********************************************************/

GType
mn_pending_mailbox_get_type (void)
{
  static GType pending_mailbox_type = 0;
  
  if (! pending_mailbox_type)
    {
      static const GTypeInfo pending_mailbox_info = {
	sizeof(MNPendingMailboxClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_pending_mailbox_class_init,
	NULL,
	NULL,
	sizeof(MNPendingMailbox),
	0,
	NULL
      };
      
      pending_mailbox_type = g_type_register_static(MN_TYPE_MAILBOX,
						    "MNPendingMailbox",
						    &pending_mailbox_info,
						    0);
    }
  
  return pending_mailbox_type;
}

static void
mn_pending_mailbox_class_init (MNPendingMailboxClass *class)
{
  MNMailboxClass *mailbox_class = MN_MAILBOX_CLASS(class);

  mailbox_class->stock_id = GTK_STOCK_EXECUTE;
  mailbox_class->format = _("detecting...");
}

MNMailbox *
mn_pending_mailbox_new (const char *uri)
{
  g_return_val_if_fail(uri != NULL, NULL);

  return g_object_new(MN_TYPE_PENDING_MAILBOX,
		      "uri", uri,
		      NULL);
}
