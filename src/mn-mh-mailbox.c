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
#include <stdio.h>
#include <string.h>
#include "mn-mh-mailbox.h"

/*** functions ***************************************************************/

static void	mn_mh_mailbox_class_init (MNMHMailboxClass	*class);
static gboolean	mn_mh_mailbox_is         (const char		*locator);
static gboolean	mn_mh_mailbox_has_new    (MNMailbox		*mailbox,
					  GError		**err);

/*** implementation **********************************************************/

GType
mn_mh_mailbox_get_type (void)
{
  static GType mh_mailbox_type = 0;
  
  if (! mh_mailbox_type)
    {
      static const GTypeInfo mh_mailbox_info = {
	sizeof(MNMHMailboxClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_mh_mailbox_class_init,
	NULL,
	NULL,
	sizeof(MNMHMailbox),
	0,
	NULL
      };
      
      mh_mailbox_type = g_type_register_static(MN_TYPE_MAILBOX,
					       "MNMHMailbox",
					       &mh_mailbox_info,
					       0);
    }
  
  return mh_mailbox_type;
}

static void
mn_mh_mailbox_class_init (MNMHMailboxClass *class)
{
  MNMailboxClass *mailbox_class = MN_MAILBOX_CLASS(class);

  mailbox_class->format = "MH";
  mailbox_class->is_remote = FALSE;
  mailbox_class->is = mn_mh_mailbox_is;
  mailbox_class->has_new = mn_mh_mailbox_has_new;
}

static gboolean
mn_mh_mailbox_is (const char *locator)
{
  char *sequences;
  gboolean is;

  sequences = g_build_filename(locator, ".mh_sequences", NULL);
  is = g_file_test(sequences, G_FILE_TEST_IS_REGULAR);
  g_free(sequences);

  return is;
}

static gboolean
mn_mh_mailbox_has_new (MNMailbox *mailbox, GError **err)
{
  char *sequences;
  gboolean has_new = FALSE;
  GIOChannel *channel;
  GIOStatus status;
  GError *tmp_err = NULL;
  char *line;

  sequences = g_build_filename(mailbox->locator, ".mh_sequences", NULL);

  channel = g_io_channel_new_file(sequences, "r", &tmp_err);
  if (! channel)
    {
      g_set_error(err, MN_MH_MAILBOX_ERROR, MN_MH_MAILBOX_ERROR_OPEN_SEQUENCES,
		  _("unable to open %s: %s"), sequences, tmp_err->message);
      g_error_free(tmp_err);
      goto end;
    }

  while ((status = g_io_channel_read_line(channel,
					  &line,
					  NULL,
					  NULL,
					  &tmp_err)) == G_IO_STATUS_NORMAL)
    {
      if (! strncmp(line, "unseen", 6))
	{
	  int num;
	  int first;

	  num = sscanf(line, "unseen: %d", &first);
	  has_new = num == 1;

	  g_free(line);
	  break;
	}

      g_free(line);
    }

  if (status == G_IO_STATUS_ERROR)
    {
      g_set_error(err,
		  MN_MH_MAILBOX_ERROR,
		  MN_MH_MAILBOX_ERROR_READ_SEQUENCES,
		  _("error while reading %s: %s"),
		  mailbox->locator,
		  tmp_err->message);
      g_error_free(tmp_err);
    }

  g_io_channel_shutdown(channel, TRUE, NULL);
  g_io_channel_unref(channel);

 end:
  g_free(sequences);
  return has_new;
}

GQuark
mn_mh_mailbox_error_quark (void)
{
  static GQuark quark = 0;

  if (! quark)
    quark = g_quark_from_static_string("mn_mh_mailbox_error");

  return quark;
}
