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
#include <libgnome/gnome-i18n.h>
#include "mn-unsupported-mailbox.h"

/*** functions ***************************************************************/

static void mn_unsupported_mailbox_class_init (MNUnsupportedMailboxClass *class);

/*** implementation **********************************************************/

GType
mn_unsupported_mailbox_get_type (void)
{
  static GType unsupported_mailbox_type = 0;
  
  if (! unsupported_mailbox_type)
    {
      static const GTypeInfo unsupported_mailbox_info = {
	sizeof(MNUnsupportedMailboxClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_unsupported_mailbox_class_init,
	NULL,
	NULL,
	sizeof(MNUnsupportedMailbox),
	0,
	NULL
      };
      
      unsupported_mailbox_type = g_type_register_static(MN_TYPE_MAILBOX,
							"MNUnsupportedMailbox",
							&unsupported_mailbox_info,
							0);
    }
  
  return unsupported_mailbox_type;
}

static void
mn_unsupported_mailbox_class_init (MNUnsupportedMailboxClass *class)
{
  MNMailboxClass *mailbox_class = MN_MAILBOX_CLASS(class);

  mailbox_class->format = _("unsupported");
  mailbox_class->is_remote = FALSE;
  mailbox_class->is = NULL;
}
