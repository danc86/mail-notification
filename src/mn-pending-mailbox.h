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

#ifndef _MN_PENDING_MAILBOX_H
#define _MN_PENDING_MAILBOX_H

#include <glib-object.h>
#include "mn-mailbox.h"

#define MN_TYPE_PENDING_MAILBOX			(mn_pending_mailbox_get_type())
#define MN_PENDING_MAILBOX(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), MN_TYPE_PENDING_MAILBOX, MNPendingMailbox))
#define MN_PENDING_MAILBOX_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), MN_TYPE_PENDING_MAILBOX, MNPendingMailboxClass))
#define MN_IS_PENDING_MAILBOX(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), MN_TYPE_PENDING_MAILBOX))
#define MN_IS_PENDING_MAILBOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), MN_TYPE_PENDING_MAILBOX))
#define MN_PENDING_MAILBOX_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), MN_TYPE_PENDING_MAILBOX, MNPendingMailboxClass))

typedef struct
{
  MNMailbox			mailbox;
} MNPendingMailbox;

typedef struct
{
  MNMailboxClass		mailbox_class;
} MNPendingMailboxClass;

GType mn_pending_mailbox_get_type (void);
MNMailbox *mn_pending_mailbox_new (const char *uri);

#endif /* _MN_PENDING_MAILBOX_H */
