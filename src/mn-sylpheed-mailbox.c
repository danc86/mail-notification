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
#include <string.h>
#include <glib/gi18n-lib.h>
#include <libgnomevfs/gnome-vfs.h>
#include "mn-sylpheed-mailbox.h"
#include "mn-util.h"
#include "mn-vfs.h"

/*** types *******************************************************************/

typedef struct
{
  MNMailboxIsCallback		*callback;
  gpointer			user_data;
} IsInfo;

typedef struct
{
  MNSylpheedMailbox		*mailbox;
  unsigned int			total_count;
  unsigned int			mark_count;
} CheckInfo;

/*** variables ***************************************************************/

static GObjectClass *parent_class = NULL;

/*** functions ***************************************************************/

static void mn_sylpheed_mailbox_class_init (MNSylpheedMailboxClass *class);

static GObject *mn_sylpheed_mailbox_constructor (GType type,
						 guint n_construct_properties,
						 GObjectConstructParam *construct_params);

static void mn_sylpheed_mailbox_is (const char *uri,
				    MNMailboxIsCallback *callback,
				    gpointer user_data);
static void mn_sylpheed_mailbox_is_cb (gboolean result, gpointer user_data);

static void mn_sylpheed_mailbox_check (MNMailbox *mailbox);
static void mn_sylpheed_mailbox_check_cb (GnomeVFSAsyncHandle *handle,
					  GnomeVFSResult result,
					  GList *list,
					  unsigned int entries_read,
					  gpointer user_data);

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
  GObjectClass *object_class = G_OBJECT_CLASS(class);
  MNMailboxClass *mailbox_class = MN_MAILBOX_CLASS(class);

  parent_class = g_type_class_peek_parent(class);

  object_class->constructor = mn_sylpheed_mailbox_constructor;

  mailbox_class->format = "Sylpheed";
  mailbox_class->is = mn_sylpheed_mailbox_is;
  mailbox_class->check = mn_sylpheed_mailbox_check;
}

static GObject *
mn_sylpheed_mailbox_constructor (GType type,
				 guint n_construct_properties,
				 GObjectConstructParam *construct_params)
{
  GObject *object;
  MNMailbox *mailbox;

  object = G_OBJECT_CLASS(parent_class)->constructor(type, n_construct_properties, construct_params);
  mailbox = MN_MAILBOX(object);

  mn_mailbox_monitor(mailbox,
		     mn_mailbox_get_uri(mailbox),
		     GNOME_VFS_MONITOR_DIRECTORY,
		     MN_MAILBOX_MONITOR_EVENT_CHANGED
		     | MN_MAILBOX_MONITOR_EVENT_DELETED
		     | MN_MAILBOX_MONITOR_EVENT_CREATED);

  return object;
}

static void
mn_sylpheed_mailbox_is (const char *uri,
			MNMailboxIsCallback *callback,
			gpointer user_data)
{
  IsInfo *info;
  char *markfile_uri;

  info = g_new(IsInfo, 1);
  info->callback = callback;
  info->user_data = user_data;

  markfile_uri = g_build_path("/", uri, ".sylpheed_mark", NULL);
  mn_vfs_async_test(markfile_uri, G_FILE_TEST_IS_REGULAR, mn_sylpheed_mailbox_is_cb, info);
  g_free(markfile_uri);
}

static void
mn_sylpheed_mailbox_is_cb (gboolean result, gpointer user_data)
{
  IsInfo *info = user_data;
  
  info->callback(result, info->user_data);
  g_free(info);
}

static void
mn_sylpheed_mailbox_check (MNMailbox *mailbox)
{
  MNSylpheedMailbox *sylpheed_mailbox = MN_SYLPHEED_MAILBOX(mailbox);
  CheckInfo *info;
  GnomeVFSAsyncHandle *handle;

  info = g_new(CheckInfo, 1);
  info->mailbox = sylpheed_mailbox;
  info->total_count = 0;
  info->mark_count = 0;
  
  gnome_vfs_async_load_directory(&handle,
				 mn_mailbox_get_uri(mailbox),
				 GNOME_VFS_FILE_INFO_DEFAULT,
				 32,
				 GNOME_VFS_PRIORITY_DEFAULT,
				 mn_sylpheed_mailbox_check_cb,
				 info);
}

static void
mn_sylpheed_mailbox_check_cb (GnomeVFSAsyncHandle *handle,
			      GnomeVFSResult result,
			      GList *list,
			      unsigned int entries_read,
			      gpointer user_data)
{
  CheckInfo *info = user_data;
  GList *l;

  MN_LIST_FOREACH(l, list)
    {
      GnomeVFSFileInfo *file_info = l->data;
	  
      if (file_info->name[0] != '.')
	{
	  if (mn_str_isnumeric(file_info->name))
	    info->total_count++;
	}
      else if (! strcmp(file_info->name, ".sylpheed_mark") && file_info->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_SIZE)
	/*
	 * Format of a Sylpheed markfile:
	 * int version, int num, int flags, int num, int flags, ...
	 */
	info->mark_count = (file_info->size - sizeof(int)) / (sizeof(int) * 2);
    }

  if (result != GNOME_VFS_OK)	/* we're done */
    {
      if (result != GNOME_VFS_ERROR_EOF)
	mn_mailbox_set_error(MN_MAILBOX(info->mailbox), _("error while reading folder: %s"), gnome_vfs_result_to_string(result));

      mn_mailbox_set_has_new(MN_MAILBOX(info->mailbox), info->total_count != info->mark_count);
      mn_mailbox_end_check(MN_MAILBOX(info->mailbox));

      g_free(info);
    }
}
