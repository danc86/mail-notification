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

#ifndef _MN_MBOX_MAILBOX_H
#define _MN_MBOX_MAILBOX_H

#include <glib.h>
#include <glib-object.h>
#include "mn-mailbox.h"

#define MN_TYPE_MBOX_MAILBOX		(mn_mbox_mailbox_get_type())
#define MN_MBOX_MAILBOX(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), MN_TYPE_MBOX_MAILBOX, MNmboxMailbox))
#define MN_MBOX_MAILBOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), MN_TYPE_MBOX_MAILBOX, MNmboxMailboxClass))
#define MN_IS_MBOX_MAILBOX(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), MN_TYPE_MBOX_MAILBOX))
#define MN_IS_MBOX_MAILBOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), MN_TYPE_MBOX_MAILBOX))
#define MN_MBOX_MAILBOX_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), MN_TYPE_MBOX_MAILBOX, MNmboxMailboxClass))

#define MN_MBOX_MAILBOX_ERROR		(mn_mbox_mailbox_error_quark())

typedef enum
{
  MN_MBOX_MAILBOX_ERROR_STAT,
  MN_MBOX_MAILBOX_ERROR_OPEN,
  MN_MBOX_MAILBOX_ERROR_ENCODING,
  MN_MBOX_MAILBOX_ERROR_READ
} MNmboxMailboxError;

typedef struct _MNmboxMailboxPrivate MNmboxMailboxPrivate;

typedef struct
{
  MNMailbox		mailbox;

  MNmboxMailboxPrivate	*priv;
} MNmboxMailbox;

typedef struct
{
  MNMailboxClass	mailbox_class;
} MNmboxMailboxClass;

GType	mn_mbox_mailbox_get_type	(void);
GQuark	mn_mbox_mailbox_error_quark	(void);

#endif /* _MN_MBOX_MAILBOX_H */
