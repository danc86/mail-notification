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

#include "config.h"
#include <libgnome/gnome-i18n.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "mn-mbox-mailbox.h"

/*** functions ***************************************************************/

static void	mn_mbox_mailbox_class_init (MNmboxMailboxClass	*class);
static void	mn_mbox_mailbox_init       (MNmboxMailbox	*mailbox);
static gboolean	mn_mbox_mailbox_is         (const char		*locator);
static gboolean	mn_mbox_mailbox_has_new    (MNMailbox		*mailbox,
					    GError		**err);

/*** implementation **********************************************************/

GType
mn_mbox_mailbox_get_type (void)
{
  static GType mbox_mailbox_type = 0;
  
  if (! mbox_mailbox_type)
    {
      static const GTypeInfo mbox_mailbox_info = {
	sizeof(MNmboxMailboxClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_mbox_mailbox_class_init,
	NULL,
	NULL,
	sizeof(MNmboxMailbox),
	0,
	(GInstanceInitFunc) mn_mbox_mailbox_init,
      };
      
      mbox_mailbox_type = g_type_register_static(MN_TYPE_MAILBOX,
						 "MNmboxMailbox",
						 &mbox_mailbox_info,
						 0);
    }
  
  return mbox_mailbox_type;
}

static void
mn_mbox_mailbox_class_init (MNmboxMailboxClass *class)
{
  MNMailboxClass *mailbox_class;

  mailbox_class = MN_MAILBOX_CLASS(class);
  mailbox_class->format = "mbox";
  mailbox_class->is_remote = FALSE;
  mailbox_class->is = mn_mbox_mailbox_is;
  mailbox_class->has_new = mn_mbox_mailbox_has_new;
}

static void
mn_mbox_mailbox_init (MNmboxMailbox *mailbox)
{
  mailbox->last_mtime = 0;
  mailbox->last_size = 0;
}

static gboolean
mn_mbox_mailbox_is (const char *locator)
{
  g_return_val_if_fail(locator != NULL, FALSE);

  return g_file_test(locator, G_FILE_TEST_IS_REGULAR);
}

static gboolean
mn_mbox_mailbox_has_new (MNMailbox *mailbox, GError **err)
{
  MNmboxMailbox *mbox_mailbox;
  struct stat sb;

  mbox_mailbox = MN_MBOX_MAILBOX(mailbox);

  if (stat(mailbox->locator, &sb) == -1)
    {
      g_set_error(err,
		  MN_MBOX_MAILBOX_ERROR,
		  MN_MBOX_MAILBOX_ERROR_STAT,
		  _("unable to stat %s: %s"),
		  mailbox->locator,
		  g_strerror(errno));
      return FALSE;
    }
  
  if (mbox_mailbox->last_mtime != sb.st_mtime || mbox_mailbox->last_size != sb.st_size)
    {
      GIOChannel *channel;
      GIOStatus status;
      GError *tmp_err = NULL;
      char *line;
      int total_count = 0;
      int seen_count = 0;
      gboolean in_header = FALSE;

      mbox_mailbox->last_mtime = sb.st_mtime;
      mbox_mailbox->last_size = sb.st_size;

      channel = g_io_channel_new_file(mailbox->locator, "r", &tmp_err);
      if (! channel)
	{
	  g_set_error(err,
		      MN_MBOX_MAILBOX_ERROR,
		      MN_MBOX_MAILBOX_ERROR_OPEN,
		      _("unable to open %s: %s"),
		      mailbox->locator,
		      tmp_err->message);
	  g_error_free(tmp_err);
	  return FALSE;
	}
      
      /* to not get a read error if some line is not valid UTF-8 */
      if (g_io_channel_set_encoding(channel, "ISO8859-1", &tmp_err) != G_IO_STATUS_NORMAL)
	{
	  g_set_error(err,
		      MN_MBOX_MAILBOX_ERROR,
		      MN_MBOX_MAILBOX_ERROR_ENCODING,
		      _("unable to set the encoding for %s: %s"),
		      mailbox->locator,
		      tmp_err->message);
	  g_error_free(tmp_err);
	  g_io_channel_shutdown(channel, TRUE, NULL);

	  return FALSE;
	}
      
      while ((status = g_io_channel_read_line(channel,
					      &line,
					      NULL,
					      NULL,
					      &tmp_err)) == G_IO_STATUS_NORMAL)
	{
	  if (line[0] == '\n')
	    in_header = FALSE;
	  else if (! strncmp(line, "From ", 5))
	    {
	      total_count++;
	      in_header = TRUE;
	    }
	  else if (in_header
		   && ! strncmp(line, "Status:", 7)
		   && (strchr(line, 'O') || strchr(line, 'R')))
	    seen_count++;

	  g_free(line);
	}

      if (status == G_IO_STATUS_ERROR)
	{
	  g_set_error(err,
		      MN_MBOX_MAILBOX_ERROR,
		      MN_MBOX_MAILBOX_ERROR_READ,
		      _("error while reading %s: %s"),
		      mailbox->locator,
		      tmp_err->message);
	  g_error_free(tmp_err);
	  g_io_channel_shutdown(channel, TRUE, NULL);

	  return FALSE;
	}
      
      g_io_channel_shutdown(channel, TRUE, NULL);

      return total_count != seen_count;
    }

  return FALSE;
}

GQuark
mn_mbox_mailbox_error_quark (void)
{
  static GQuark quark = 0;

  if (! quark)
    quark = g_quark_from_static_string("mn_mbox_mailbox_error");

  return quark;
}
