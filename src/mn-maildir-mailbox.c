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
#include "mn-maildir-mailbox.h"

/*** functions ***************************************************************/

static void	mn_maildir_mailbox_class_init (MNMaildirMailboxClass *class);
static gboolean	mn_maildir_mailbox_is         (const char            *locator);
static gboolean	mn_maildir_mailbox_has_new    (MNMailbox             *mailbox,
					       GError                **err);

/*** implementation **********************************************************/

GType
mn_maildir_mailbox_get_type (void)
{
  static GType maildir_mailbox_type = 0;
  
  if (! maildir_mailbox_type)
    {
      static const GTypeInfo maildir_mailbox_info = {
	sizeof(MNMaildirMailboxClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_maildir_mailbox_class_init,
	NULL,
	NULL,
	sizeof(MNMaildirMailbox),
	0,
	NULL
      };
      
      maildir_mailbox_type = g_type_register_static(MN_TYPE_MAILBOX,
						    "MNMaildirMailbox",
						    &maildir_mailbox_info,
						    0);
    }
  
  return maildir_mailbox_type;
}

static void
mn_maildir_mailbox_class_init (MNMaildirMailboxClass *class)
{
  MNMailboxClass *mailbox_class = MN_MAILBOX_CLASS(class);

  mailbox_class->format = "Maildir";
  mailbox_class->is_remote = FALSE;
  mailbox_class->is = mn_maildir_mailbox_is;
  mailbox_class->has_new = mn_maildir_mailbox_has_new;
}

static gboolean
mn_maildir_mailbox_is (const char *locator)
{
  gboolean is;
  char *cur, *new, *tmp;

  cur = g_build_filename(locator, "cur", NULL);
  new = g_build_filename(locator, "new", NULL);
  tmp = g_build_filename(locator, "tmp", NULL);

  is = g_file_test(cur, G_FILE_TEST_IS_DIR)
    && g_file_test(new, G_FILE_TEST_IS_DIR)
    && g_file_test(tmp, G_FILE_TEST_IS_DIR);

  g_free(cur);
  g_free(new);
  g_free(tmp);

  return is;
}

static gboolean
mn_maildir_mailbox_has_new (MNMailbox *mailbox, GError **err)
{
  char *new;
  GDir *dir;
  GError *tmp_err = NULL;
  gboolean has_new = FALSE;
  const char *filename;

  new = g_build_filename(mailbox->locator, "new", NULL);
  dir = g_dir_open(new, 0, &tmp_err);
  g_free(new);

  if (! dir)
    {
      g_set_error(err,
		  MN_MAILDIR_MAILBOX_ERROR,
		  MN_MAILDIR_MAILBOX_ERROR_OPEN_NEW,
		  _("unable to open %s: %s"),
		  mailbox->locator,
		  tmp_err->message);
      g_error_free(tmp_err);
      return FALSE;
    }
  
  while ((filename = g_dir_read_name(dir)))
    if (filename[0] != '.')
      {
	has_new = TRUE;
	break;
      }
  
  g_dir_close(dir);
  
  return has_new;
}

GQuark
mn_maildir_mailbox_error_quark (void)
{
  static GQuark quark = 0;

  if (! quark)
    quark = g_quark_from_static_string("mn_maildir_mailbox_error");

  return quark;
}
