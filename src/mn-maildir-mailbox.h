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

#ifndef _MN_MAILDIR_MAILBOX_H
#define _MN_MAILDIR_MAILBOX_H

#include <glib.h>
#include <glib-object.h>
#include "mn-mailbox.h"

#define MN_TYPE_MAILDIR_MAILBOX			(mn_maildir_mailbox_get_type())
#define MN_MAILDIR_MAILBOX(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), MN_TYPE_MAILDIR_MAILBOX, MNMaildirMailbox))
#define MN_MAILDIR_MAILBOX_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), MN_TYPE_MAILDIR_MAILBOX, MNMaildirMailboxClass))
#define MN_IS_MAILDIR_MAILBOX(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), MN_TYPE_MAILDIR_MAILBOX))
#define MN_IS_MAILDIR_MAILBOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), MN_TYPE_MAILDIR_MAILBOX))
#define MN_MAILDIR_MAILBOX_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), MN_TYPE_MAILDIR_MAILBOX, MNMaildirMailboxClass))

#define MN_MAILDIR_MAILBOX_ERROR		(mn_maildir_mailbox_error_quark())

typedef enum
{
  MN_MAILDIR_MAILBOX_ERROR_OPEN_NEW
} MNMaildirMailboxError;

typedef struct
{
  MNMailbox		mailbox;
} MNMaildirMailbox;

typedef struct
{
  MNMailboxClass	mailbox_class;
} MNMaildirMailboxClass;

GType	mn_maildir_mailbox_get_type	(void);
GQuark	mn_maildir_mailbox_error_quark	(void);

#endif /* _MN_MAILDIR_MAILBOX_H */
