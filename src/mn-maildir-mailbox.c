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
#include <glib/gi18n-lib.h>
#include <libgnomevfs/gnome-vfs.h>
#include "mn-maildir-mailbox.h"
#include "mn-util.h"
#include "mn-vfs.h"

/*** types *******************************************************************/

typedef struct
{
  char			*uri;
  int			i;
  MNMailboxIsCallback	*callback;
  gpointer		user_data;
} IsInfo;

/*** variables ***************************************************************/

static GObjectClass *parent_class = NULL;
static const char *constitutive_dirs[] = { "cur", "new", "tmp" };

/*** functions ***************************************************************/

static void mn_maildir_mailbox_class_init (MNMaildirMailboxClass *class);

static GObject *mn_maildir_mailbox_constructor (GType type,
						guint n_construct_properties,
						GObjectConstructParam *construct_params);

static void mn_maildir_mailbox_is (const char *uri,
				   MNMailboxIsCallback *callback,
				   gpointer user_data);
static void mn_maildir_mailbox_is_continue (IsInfo *info);
static void mn_maildir_mailbox_is_cb (gboolean result, gpointer user_data);

static void mn_maildir_mailbox_check (MNMailbox *mailbox);
static void mn_maildir_mailbox_check_cb (GnomeVFSAsyncHandle *handle,
					 GnomeVFSResult result,
					 GList *list,
					 unsigned int entries_read,
					 gpointer user_data);

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
  GObjectClass *object_class = G_OBJECT_CLASS(class);
  MNMailboxClass *mailbox_class = MN_MAILBOX_CLASS(class);

  parent_class = g_type_class_peek_parent(class);

  object_class->constructor = mn_maildir_mailbox_constructor;

  mailbox_class->format = "Maildir";
  mailbox_class->is = mn_maildir_mailbox_is;
  mailbox_class->check = mn_maildir_mailbox_check;
}

static GObject *
mn_maildir_mailbox_constructor (GType type,
				guint n_construct_properties,
				GObjectConstructParam *construct_params)
{
  GObject *object;
  MNMailbox *mailbox;
  char *new_uri;
  
  object = G_OBJECT_CLASS(parent_class)->constructor(type, n_construct_properties, construct_params);
  mailbox = MN_MAILBOX(object);

  new_uri = g_build_path("/", mn_mailbox_get_uri(mailbox), "new", NULL);
  mn_mailbox_monitor(mailbox,
		     new_uri,
		     GNOME_VFS_MONITOR_DIRECTORY,
		     MN_MAILBOX_MONITOR_EVENT_DELETED
		     | MN_MAILBOX_MONITOR_EVENT_CREATED);
  g_free(new_uri);

  return object;
}

static void
mn_maildir_mailbox_is (const char *uri,
		       MNMailboxIsCallback *callback,
		       gpointer user_data)
{
  IsInfo *info;
  
  info = g_new(IsInfo, 1);
  info->uri = g_strdup(uri);
  info->i = 0;
  info->callback = callback;
  info->user_data = user_data;
  
  mn_maildir_mailbox_is_continue(info);
}

static void
mn_maildir_mailbox_is_continue (IsInfo *info)
{
  char *uri;

  uri = g_build_path("/", info->uri, constitutive_dirs[info->i], NULL);
  mn_vfs_async_test(uri, G_FILE_TEST_IS_DIR, mn_maildir_mailbox_is_cb, info);
  g_free(uri);
}

static void
mn_maildir_mailbox_is_cb (gboolean result, gpointer user_data)
{
  IsInfo *info = user_data;
  gboolean is = FALSE;

  if (result)
    {
      if (++info->i < G_N_ELEMENTS(constitutive_dirs))
	{
	  mn_maildir_mailbox_is_continue(info);
	  return;
	}
      else
	is = TRUE;
    }

  info->callback(is, info->user_data);
  g_free(info->uri);
  g_free(info);
}

static void
mn_maildir_mailbox_check (MNMailbox *mailbox)
{
  char *new_uri;
  GnomeVFSAsyncHandle *handle;

  new_uri = g_build_path("/", mn_mailbox_get_uri(mailbox), "new", NULL);
  gnome_vfs_async_load_directory(&handle,
				 new_uri,
				 GNOME_VFS_FILE_INFO_DEFAULT,
				 32,
				 GNOME_VFS_PRIORITY_DEFAULT,
				 mn_maildir_mailbox_check_cb,
				 mailbox);
  g_free(new_uri);
}

static void
mn_maildir_mailbox_check_cb (GnomeVFSAsyncHandle *handle,
			     GnomeVFSResult result,
			     GList *list,
			     unsigned int entries_read,
			     gpointer user_data)
{
  MNMaildirMailbox *mailbox = user_data;
  gboolean has_new = FALSE;
  GList *l;

  MN_LIST_FOREACH(l, list)
    {
      GnomeVFSFileInfo *file_info = l->data;

      if (file_info->name[0] != '.')
	{
	  has_new = TRUE;
	  break;
	}
    }
  
  if (has_new)
    {
      gnome_vfs_async_cancel(handle);
      goto end;
    }
  
  if (result != GNOME_VFS_OK)	/* we're done */
    {
      if (result != GNOME_VFS_ERROR_EOF)
	mn_mailbox_set_error(MN_MAILBOX(mailbox), _("error while reading folder: %s"), gnome_vfs_result_to_string(result));

    end:
      mn_mailbox_set_has_new(MN_MAILBOX(mailbox), has_new);
      mn_mailbox_end_check(MN_MAILBOX(mailbox));
    }
}
