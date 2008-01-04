/*
 * Mail Notification
 * Copyright (C) 2003-2008 Jean-Yves Lefort <jylefort@brutele.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _MN_MESSAGE_MIME_H
#define _MN_MESSAGE_MIME_H

#include <gmime/gmime.h>
#include "mn-message.h"

/*
 * The following functions return a NULL message if one of the
 * following conditions is true:
 *
 *   - handle_status is TRUE and the message is marked as read
 *   - the message is marked as spam
 */

MNMessage *mn_message_new_from_mime_message (MNMailbox *mailbox,
					     GMimeMessage *mime_message,
					     const char *uri,
					     MNMessageFlags flags,
					     gboolean handle_status);
MNMessage *mn_message_new_from_mime_message_full (GType type,
						  MNMailbox *mailbox,
						  GMimeMessage *mime_message,
						  const char *uri,
						  MNMessageFlags flags,
						  gboolean handle_status);
MNMessage *mn_message_new_from_mime_stream (MNMailbox *mailbox,
					    GMimeStream *mime_stream,
					    const char *uri,
					    MNMessageFlags flags,
					    gboolean handle_status);
MNMessage *mn_message_new_from_mime_stream_full (GType type,
						 MNMailbox *mailbox,
						 GMimeStream *mime_stream,
						 const char *uri,
						 MNMessageFlags flags,
						 gboolean handle_status);
MNMessage *mn_message_new_from_uri (MNMailbox *mailbox,
				    GnomeVFSURI *uri,
				    MNMessageFlags flags,
				    gboolean handle_status);
MNMessage *mn_message_new_from_uri_full (GType type,
					 MNMailbox *mailbox,
					 GnomeVFSURI *uri,
					 MNMessageFlags flags,
					 gboolean handle_status);
MNMessage *mn_message_new_from_buffer (MNMailbox *mailbox,
				       const char *buffer,
				       unsigned int len,
				       MNMessageFlags flags,
				       gboolean handle_status);

#endif /* _MN_MESSAGE_MIME_H */
