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

#ifndef _MN_POP3_MAILBOX_H
#define _MN_POP3_MAILBOX_H

#include <glib.h>
#include <glib-object.h>
#include "mn-mailbox.h"

#define MN_TYPE_POP3_MAILBOX		(mn_pop3_mailbox_get_type())
#define MN_POP3_MAILBOX(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), MN_TYPE_POP3_MAILBOX, MNPOP3Mailbox))
#define MN_POP3_MAILBOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), MN_TYPE_POP3_MAILBOX, MNPOP3MailboxClass))
#define MN_IS_POP3_MAILBOX(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), MN_TYPE_POP3_MAILBOX))
#define MN_IS_POP3_MAILBOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), MN_TYPE_POP3_MAILBOX))
#define MN_POP3_MAILBOX_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), MN_TYPE_POP3_MAILBOX, MNPOP3MailboxClass))

#define MN_POP3_MAILBOX_ERROR		(mn_pop3_mailbox_error_quark())

typedef enum
{
  MN_POP3_MAILBOX_ERROR_PARSE_LOCATOR,
  MN_POP3_MAILBOX_ERROR_RESOLVE,
  MN_POP3_MAILBOX_ERROR_CONNECT,
  MN_POP3_MAILBOX_ERROR_ACK,
  MN_POP3_MAILBOX_ERROR_SEND,
  MN_POP3_MAILBOX_ERROR_AUTH,
  MN_POP3_MAILBOX_ERROR_REPLY
} MNPOP3MailboxError;

typedef struct _MNPOP3MailboxPrivate MNPOP3MailboxPrivate;

typedef struct
{
  MNMailbox		mailbox;
  
  MNPOP3MailboxPrivate	*priv;
} MNPOP3Mailbox;

typedef struct
{
  MNMailboxClass	mailbox_class;
} MNPOP3MailboxClass;

GType	mn_pop3_mailbox_get_type	(void);
GQuark	mn_pop3_mailbox_error_quark	(void);

#endif /* _MN_POP3_MAILBOX_H */
