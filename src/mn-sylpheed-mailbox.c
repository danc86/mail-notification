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
#include "mn-sylpheed-mailbox.h"
#include "mn-util.h"

/*** functions ***************************************************************/

static void	mn_sylpheed_mailbox_class_init (MNSylpheedMailboxClass *class);
static gboolean	mn_sylpheed_mailbox_is         (const char           *locator);
static gboolean	mn_sylpheed_mailbox_has_new    (MNMailbox            *mailbox,
						GError               **err);

/*** implementation **********************************************************/

GType
mn_sylpheed_mailbox_get_type (void)
{
  static GType sylpheed_mailbox_type = 0;
  
  if (! sylpheed_mailbox_type)
    {
      static const GTypeInfo sylpheed_mailbox_info = {
	sizeof(MNSylpheedMailboxClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_sylpheed_mailbox_class_init,
	NULL,
	NULL,
	sizeof(MNSylpheedMailbox),
	0,
	NULL
      };
      
      sylpheed_mailbox_type = g_type_register_static(MN_TYPE_MAILBOX,
						     "MNSylpheedMailbox",
						     &sylpheed_mailbox_info,
						     0);
    }
  
  return sylpheed_mailbox_type;
}

static void
mn_sylpheed_mailbox_class_init (MNSylpheedMailboxClass *class)
{
  MNMailboxClass *mailbox_class;

  mailbox_class = MN_MAILBOX_CLASS(class);
  mailbox_class->format = "Sylpheed";
  mailbox_class->is_remote = FALSE;
  mailbox_class->is = mn_sylpheed_mailbox_is;
  mailbox_class->has_new = mn_sylpheed_mailbox_has_new;
}

static gboolean
mn_sylpheed_mailbox_is (const char *locator)
{
  char *markfile;
  gboolean is;

  markfile = g_build_filename(locator, ".sylpheed_mark", NULL);
  is = g_file_test(markfile, G_FILE_TEST_IS_REGULAR);
  g_free(markfile);

  return is;
}

static gboolean
mn_sylpheed_mailbox_has_new (MNMailbox *mailbox, GError **err)
{
  GError *tmp_err = NULL;
  GDir *dir;
  const char *filename;
  int total_count = 0;
  int mark_count = 0;
  char *markfile;
  gboolean has_new = FALSE;
  struct stat sb;

  /* count total number of messages */
  
  dir = g_dir_open(mailbox->locator, 0, &tmp_err);
  if (! dir)
    {
      g_set_error(err,
		  MN_SYLPHEED_MAILBOX_ERROR,
		  MN_SYLPHEED_MAILBOX_ERROR_OPEN_DIR,
		  _("unable to open directory %s: %s"),
		  mailbox->locator,
		  tmp_err->message);
      g_error_free(tmp_err);
      return FALSE;
    }

  while ((filename = g_dir_read_name(dir)))
    if (filename[0] != '.' && mn_str_isnumeric(filename))
      total_count++;

  g_dir_close(dir);

  /* extrapolate mark_count from size of markfile */

  markfile = g_build_filename(mailbox->locator, ".sylpheed_mark", NULL);

  if (stat(markfile, &sb) == -1)
    {
      g_set_error(err,
		  MN_SYLPHEED_MAILBOX_ERROR,
		  MN_SYLPHEED_MAILBOX_ERROR_STAT_MARKFILE,
		  _("unable to stat %s: %s"),
		  markfile,
		  g_strerror(errno));
      goto end;
    }

  /*
   * The format of a Sylpheed markfile is:
   *
   *	int version, int num, int flag, int num, int flag, ...
   */

  mark_count = (sb.st_size - sizeof(int)) / (sizeof(int) * 2);
  has_new = total_count != mark_count;

 end:
  g_free(markfile);
  return has_new;
}

GQuark
mn_sylpheed_mailbox_error_quark (void)
{
  static GQuark quark = 0;

  if (! quark)
    quark = g_quark_from_static_string("mn_sylpheed_mailbox_error");

  return quark;
}
