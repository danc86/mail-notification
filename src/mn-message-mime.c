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
#include <glib/gi18n.h>
#include <libgnomevfs/gnome-vfs.h>
#include "mn-message-mime.h"
#include "mn-gmime-stream-vfs.h"

/*** functions ***************************************************************/

static char *mn_message_mime_header_decode_text (const char *str);

/*** implementation **********************************************************/

static char *
mn_message_mime_header_decode_text (const char *str)
{
  char *decoded;

  g_return_val_if_fail(str != NULL, NULL);

  decoded = g_mime_utils_header_decode_text(str);
  if (g_utf8_validate(decoded, -1, NULL))
    return decoded;
  else
    {
      char *converted;

      /* assume it is ISO8859-1 */
      converted = g_convert(decoded, -1, "UTF-8", "ISO8859-1", NULL, NULL, NULL);
      g_free(decoded);

      return converted ? converted : g_strdup("");
    }
}

MNMessage *
mn_message_new_from_mime_message (MNURI *mailbox_uri,
				  GMimeMessage *mime_message)
{
  MNMessage *message;
  const char *message_id;
  time_t sent_time;
  const char *from;
  const char *subject;
  char *decoded_from;
  char *decoded_subject;

  g_return_val_if_fail(MN_IS_URI(mailbox_uri), NULL);
  g_return_val_if_fail(GMIME_IS_MESSAGE(mime_message), NULL);
  
  message_id = g_mime_message_get_message_id(mime_message);
  g_mime_message_get_date(mime_message, &sent_time, NULL);
  from = g_mime_message_get_sender(mime_message);
  subject = g_mime_message_get_subject(mime_message);

  decoded_from = from ? mn_message_mime_header_decode_text(from) : NULL;
  decoded_subject = subject ? mn_message_mime_header_decode_text(subject) : NULL;

  message = mn_message_new(mailbox_uri, NULL, sent_time, message_id, decoded_from, decoded_subject);

  g_free(decoded_from);
  g_free(decoded_subject);

  return message;
}

MNMessage *
mn_message_new_from_mime_stream (MNURI *mailbox_uri,
				 GMimeStream *mime_stream)
{
  GMimeParser *parser;
  GMimeMessage *mime_message;
  MNMessage *message;

  g_return_val_if_fail(MN_IS_URI(mailbox_uri), NULL);
  g_return_val_if_fail(GMIME_IS_STREAM(mime_stream), NULL);

  parser = g_mime_parser_new_with_stream(mime_stream);
  mime_message = g_mime_parser_construct_message(parser);
  g_object_unref(parser);

  if (mime_message)
    {
      message = mn_message_new_from_mime_message(mailbox_uri, mime_message);
      g_object_unref(mime_message);
    }
  else
    message = mn_message_new_from_error(mailbox_uri, _("unable to parse MIME message"));
    
  return message;
}

MNMessage *
mn_message_new_from_uri (MNURI *mailbox_uri,
			 GnomeVFSURI *uri)
{
  GnomeVFSResult result;
  GnomeVFSHandle *handle;
  MNMessage *message;

  g_return_val_if_fail(MN_IS_URI(mailbox_uri), NULL);
  g_return_val_if_fail(uri != NULL, NULL);

  result = gnome_vfs_open_uri(&handle, uri, GNOME_VFS_OPEN_READ | GNOME_VFS_OPEN_RANDOM);
  if (result == GNOME_VFS_OK)
    {
      GMimeStream *stream;
      char *text_uri;

      text_uri = gnome_vfs_uri_to_string(uri, GNOME_VFS_URI_HIDE_NONE);
      stream = mn_gmime_stream_vfs_new(handle, text_uri);
      g_free(text_uri);

      message = mn_message_new_from_mime_stream(mailbox_uri, stream);
      g_object_unref(stream);

      gnome_vfs_close(handle);
    }
  else
    message = mn_message_new_from_error(mailbox_uri, gnome_vfs_result_to_string(result));
    
  return message;
}

MNMessage *
mn_message_new_from_buffer (MNURI *mailbox_uri,
			    const char *buffer,
			    unsigned int len)
{
  GMimeStream *stream;
  MNMessage *message;

  g_return_val_if_fail(MN_IS_URI(mailbox_uri), NULL);
  g_return_val_if_fail(buffer != NULL, NULL);

  stream = g_mime_stream_mem_new_with_buffer(buffer, len);
  message = mn_message_new_from_mime_stream(mailbox_uri, stream);
  g_object_unref(stream);

  return message;
}
