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
#include <stdio.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include <libgnomevfs/gnome-vfs.h>
#include "mn-mh-mailbox.h"
#include "mn-vfs.h"

/*** types *******************************************************************/

typedef struct
{
  MNMailboxIsCallback		*callback;
  gpointer			user_data;
} IsInfo;

/*** variables ***************************************************************/

static GObjectClass *parent_class = NULL;

/*** functions ***************************************************************/

static void mn_mh_mailbox_class_init (MNMHMailboxClass *class);

static GObject *mn_mh_mailbox_constructor (GType type,
					   guint n_construct_properties,
					   GObjectConstructParam *construct_params);

static void mn_mh_mailbox_is (const char *uri,
			      MNMailboxIsCallback *callback,
			      gpointer user_data);
static void mn_mh_mailbox_is_cb (gboolean result, gpointer user_data);

static void mn_mh_mailbox_check (MNMailbox *mailbox);
static void mn_mh_mailbox_check_open_cb (MNVFSAsyncHandle *handle,
					 GnomeVFSResult result,
					 gpointer user_data);
static void mn_mh_mailbox_check_read_line_cb (MNVFSAsyncHandle *handle,
					      GnomeVFSResult result,
					      const char *line,
					      gpointer user_data);
static void mn_mh_mailbox_check_close_cb (MNVFSAsyncHandle *handle,
					  GnomeVFSResult result,
					  gpointer user_data);
     
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
  GObjectClass *object_class = G_OBJECT_CLASS(class);
  MNMailboxClass *mailbox_class = MN_MAILBOX_CLASS(class);

  parent_class = g_type_class_peek_parent(class);

  object_class->constructor = mn_mh_mailbox_constructor;

  mailbox_class->format = "MH";
  mailbox_class->is = mn_mh_mailbox_is;
  mailbox_class->check = mn_mh_mailbox_check;
}

static GObject *
mn_mh_mailbox_constructor (GType type,
			   guint n_construct_properties,
			   GObjectConstructParam *construct_params)
{
  GObject *object;
  MNMailbox *mailbox;
  char *sequences_uri;

  object = G_OBJECT_CLASS(parent_class)->constructor(type, n_construct_properties, construct_params);
  mailbox = MN_MAILBOX(object);

  sequences_uri = g_build_path("/", mn_mailbox_get_uri(mailbox), ".mh_sequences", NULL);
  mn_mailbox_monitor(mailbox,
		     sequences_uri,
		     GNOME_VFS_MONITOR_FILE,
		     MN_MAILBOX_MONITOR_EVENT_CHANGED
		     | MN_MAILBOX_MONITOR_EVENT_DELETED
		     | MN_MAILBOX_MONITOR_EVENT_CREATED);
  g_free(sequences_uri);
  
  return object;
}

static void
mn_mh_mailbox_is (const char *uri,
		  MNMailboxIsCallback *callback,
		  gpointer user_data)
{
  IsInfo *info;
  char *sequences_uri;

  info = g_new(IsInfo, 1);
  info->callback = callback;
  info->user_data = user_data;

  sequences_uri = g_build_path("/", uri, ".mh_sequences", NULL);
  mn_vfs_async_test(sequences_uri, G_FILE_TEST_IS_REGULAR, mn_mh_mailbox_is_cb, info);
  g_free(sequences_uri);
}

static void
mn_mh_mailbox_is_cb (gboolean result, gpointer user_data)
{
  IsInfo *info = user_data;
  
  info->callback(result, info->user_data);
  g_free(info);
}

static void
mn_mh_mailbox_check (MNMailbox *mailbox)
{
  char *sequences_uri;
  MNVFSAsyncHandle *handle;

  sequences_uri = g_build_path("/", mn_mailbox_get_uri(mailbox), ".mh_sequences", NULL);
  mn_vfs_async_open(&handle,
		    sequences_uri,
		    GNOME_VFS_OPEN_READ,
		    mn_mh_mailbox_check_open_cb,
		    mailbox);
  g_free(sequences_uri);
}

static void
mn_mh_mailbox_check_open_cb (MNVFSAsyncHandle *handle,
			     GnomeVFSResult result,
			     gpointer user_data)
{
  MNMHMailbox *mailbox = user_data;

  if (result == GNOME_VFS_OK)
    mn_vfs_async_read_line(handle, 0, mn_mh_mailbox_check_read_line_cb, mailbox);
  else
    {
      mn_mailbox_set_error(MN_MAILBOX(mailbox), _("unable to open .mh_sequences: %s"), gnome_vfs_result_to_string(result));
      mn_mailbox_end_check(MN_MAILBOX(mailbox));
    }
}

static void
mn_mh_mailbox_check_read_line_cb (MNVFSAsyncHandle *handle,
				  GnomeVFSResult result,
				  const char *line,
				  gpointer user_data)
{
  MNMHMailbox *mailbox = user_data;
  gboolean has_new = FALSE;

  if (line && ! strncmp(line, "unseen", 6))
    {
      int first;

      if (sscanf(line, "unseen: %d", &first) == 1)
	{
	  has_new = TRUE;
	  goto end;
	}
    }

  if (result == GNOME_VFS_OK)
    mn_vfs_async_read_line(handle, 0, mn_mh_mailbox_check_read_line_cb, mailbox);
  else
    {
      if (result != GNOME_VFS_ERROR_EOF)
	mn_mailbox_set_error(MN_MAILBOX(mailbox), _("error while reading .mh_sequences: %s"), gnome_vfs_result_to_string(result));

    end:
      mn_mailbox_set_has_new(MN_MAILBOX(mailbox), has_new);
      mn_vfs_async_close(handle, mn_mh_mailbox_check_close_cb, mailbox);
    }
}

static void
mn_mh_mailbox_check_close_cb (MNVFSAsyncHandle *handle,
			      GnomeVFSResult result,
			      gpointer user_data)
{
  MNMHMailbox *mailbox = user_data;

  if (result != GNOME_VFS_OK)
    mn_mailbox_set_error(MN_MAILBOX(mailbox), _("unable to close .mh_sequences: %s"), gnome_vfs_result_to_string(result));
  mn_mailbox_end_check(MN_MAILBOX(mailbox));
}
