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
#include <errno.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include "mn-mbox-mailbox.h"
#include "mn-vfs.h"

/*** types *******************************************************************/

struct _MNmboxMailboxPrivate
{
  time_t			last_mtime;
  GnomeVFSFileSize		last_size;
};

typedef struct
{
  gboolean			is;
  char				*uri;
  char				frombuf[5];
  MNMailboxIsCallback		*callback;
  gpointer			user_data;
} IsInfo;

typedef struct
{
  MNmboxMailbox			*mailbox;
  gboolean			in_header;
  gboolean			seen;
} CheckInfo;

/*** variables ***************************************************************/

static GObjectClass *parent_class = NULL;

/*** functions ***************************************************************/

static void mn_mbox_mailbox_class_init (MNmboxMailboxClass *class);
static void mn_mbox_mailbox_init (MNmboxMailbox *mailbox);
static void mn_mbox_mailbox_finalize (GObject *object);

static GObject *mn_mbox_mailbox_constructor (GType type,
					     guint n_construct_properties,
					     GObjectConstructParam *construct_params);

static void mn_mbox_mailbox_is (const char *uri,
				MNMailboxIsCallback *callback,
				gpointer user_data);
static void mn_mbox_mailbox_is_get_file_info_cb (GnomeVFSAsyncHandle *handle,
						 GList *results,
						 gpointer user_data);
static void mn_mbox_mailbox_is_open_cb (GnomeVFSAsyncHandle *handle,
					GnomeVFSResult result,
					gpointer user_data);
static void mn_mbox_mailbox_is_read_cb (GnomeVFSAsyncHandle *handle,
					GnomeVFSResult result,
					gpointer buffer,
					GnomeVFSFileSize bytes_requested,
					GnomeVFSFileSize bytes_read,
					gpointer user_data);
static void mn_mbox_mailbox_is_close_cb (GnomeVFSAsyncHandle *handle,
					 GnomeVFSResult result,
					 gpointer user_data);
static void mn_mbox_mailbox_is_finish (IsInfo *info, gboolean is);

static void mn_mbox_mailbox_check (MNMailbox *mailbox);
static void mn_mbox_mailbox_check_open_cb (MNVFSAsyncHandle *handle,
					   GnomeVFSResult result,
					   gpointer user_data);
static void mn_mbox_mailbox_check_read_line_cb (MNVFSAsyncHandle *handle,
						GnomeVFSResult result,
						const char *line,
						gpointer user_data);
static void mn_mbox_mailbox_check_close_cb (MNVFSAsyncHandle *handle,
					    GnomeVFSResult result,
					    gpointer user_data);

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
	(GInstanceInitFunc) mn_mbox_mailbox_init
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
  GObjectClass *object_class = G_OBJECT_CLASS(class);
  MNMailboxClass *mailbox_class = MN_MAILBOX_CLASS(class);

  parent_class = g_type_class_peek_parent(class);

  object_class->finalize = mn_mbox_mailbox_finalize;
  object_class->constructor = mn_mbox_mailbox_constructor;

  mailbox_class->format = "mbox";
  mailbox_class->is = mn_mbox_mailbox_is;
  mailbox_class->check = mn_mbox_mailbox_check;
}

static void
mn_mbox_mailbox_init (MNmboxMailbox *mailbox)
{
  mailbox->priv = g_new0(MNmboxMailboxPrivate, 1);
}

static void
mn_mbox_mailbox_finalize (GObject *object)
{
  MNmboxMailbox *mailbox = MN_MBOX_MAILBOX(object);

  g_free(mailbox->priv);

  G_OBJECT_CLASS(parent_class)->finalize(object);
}

static GObject *
mn_mbox_mailbox_constructor (GType type,
			     guint n_construct_properties,
			     GObjectConstructParam *construct_params)
{
  GObject *object;
  MNMailbox *mailbox;

  object = G_OBJECT_CLASS(parent_class)->constructor(type, n_construct_properties, construct_params);
  mailbox = MN_MAILBOX(object);

  mn_mailbox_monitor(mailbox,
		     mn_mailbox_get_uri(mailbox),
		     GNOME_VFS_MONITOR_FILE,
		     MN_MAILBOX_MONITOR_EVENT_CHANGED
		     | MN_MAILBOX_MONITOR_EVENT_DELETED
		     | MN_MAILBOX_MONITOR_EVENT_CREATED);
		     
  return object;
}

static void
mn_mbox_mailbox_is (const char *uri,
		    MNMailboxIsCallback *callback,
		    gpointer user_data)
{
  GnomeVFSURI *vfs_uri;
  GList *uri_list = NULL;
  IsInfo *info;
  GnomeVFSAsyncHandle *handle;

  vfs_uri = gnome_vfs_uri_new(uri);
  if (! vfs_uri)
    {
      callback(FALSE, user_data);
      return;
    }

  uri_list = g_list_append(uri_list, vfs_uri);

  info = g_new(IsInfo, 1);
  info->is = FALSE;
  info->uri = g_strdup(uri);
  info->callback = callback;
  info->user_data = user_data;

  gnome_vfs_async_get_file_info(&handle,
				uri_list,
				GNOME_VFS_FILE_INFO_FOLLOW_LINKS,
				GNOME_VFS_PRIORITY_DEFAULT,
				mn_mbox_mailbox_is_get_file_info_cb,
				info);

  gnome_vfs_uri_unref(vfs_uri);
  g_list_free(uri_list);
}

static void
mn_mbox_mailbox_is_get_file_info_cb (GnomeVFSAsyncHandle *handle,
				     GList *results,
				     gpointer user_data)
{
  IsInfo *info = user_data;
  GnomeVFSGetFileInfoResult *result;

  g_return_if_fail(results->data != NULL);
  result = results->data;
  
  if (result->result == GNOME_VFS_OK)
    {
      if (result->file_info->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_TYPE
	  && result->file_info->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_SIZE
	  && result->file_info->type == GNOME_VFS_FILE_TYPE_REGULAR)
	{
	  if (result->file_info->size == 0)
	    info->is = TRUE;
	  else
	    {
	      GnomeVFSAsyncHandle *handle;

	      gnome_vfs_async_open(&handle,
				   info->uri,
				   GNOME_VFS_OPEN_READ,
				   GNOME_VFS_PRIORITY_DEFAULT,
				   mn_mbox_mailbox_is_open_cb,
				   info);
	      return;
	    }
	}
    }

  mn_mbox_mailbox_is_finish(info, info->is);
}

static void
mn_mbox_mailbox_is_open_cb (GnomeVFSAsyncHandle *handle,
			    GnomeVFSResult result,
			    gpointer user_data)
{
  IsInfo *info = user_data;

  if (result == GNOME_VFS_OK)
    gnome_vfs_async_read(handle,
			 info->frombuf,
			 sizeof(info->frombuf),
			 mn_mbox_mailbox_is_read_cb,
			 info);
  else
    mn_mbox_mailbox_is_finish(info, FALSE);
}

static void
mn_mbox_mailbox_is_read_cb (GnomeVFSAsyncHandle *handle,
			    GnomeVFSResult result,
			    gpointer buffer,
			    GnomeVFSFileSize bytes_requested,
			    GnomeVFSFileSize bytes_read,
			    gpointer user_data)
{
  IsInfo *info = user_data;

  if (result == GNOME_VFS_OK && bytes_requested == bytes_read && ! strncmp(info->frombuf, "From ", sizeof(info->frombuf)))
    info->is = TRUE;

  gnome_vfs_async_close(handle, mn_mbox_mailbox_is_close_cb, info);
}

static void
mn_mbox_mailbox_is_close_cb (GnomeVFSAsyncHandle *handle,
			     GnomeVFSResult result,
			     gpointer user_data)
{
  IsInfo *info = user_data;

  mn_mbox_mailbox_is_finish(info, result == GNOME_VFS_OK && info->is);
}

static void
mn_mbox_mailbox_is_finish (IsInfo *info, gboolean is)
{
  info->callback(is, info->user_data);
  g_free(info->uri);
  g_free(info);
}

static void
mn_mbox_mailbox_check (MNMailbox *mailbox)
{
  MNmboxMailbox *mbox_mailbox = MN_MBOX_MAILBOX(mailbox);
  const char *uri;
  GnomeVFSFileInfo *file_info;
  GnomeVFSResult result;
  gboolean changed;

  uri = mn_mailbox_get_uri(mailbox);

  file_info = gnome_vfs_file_info_new();
  result = gnome_vfs_get_file_info(uri, file_info, GNOME_VFS_FILE_INFO_FOLLOW_LINKS);
  changed = result == GNOME_VFS_OK && (file_info->mtime != mbox_mailbox->priv->last_mtime || file_info->size != mbox_mailbox->priv->last_size);
  if (changed)
    {
      mbox_mailbox->priv->last_mtime = file_info->mtime;
      mbox_mailbox->priv->last_size = file_info->size;
    }
  gnome_vfs_file_info_unref(file_info);
      
  if (result == GNOME_VFS_OK)
    {
      if (changed)
	{
	  MNVFSAsyncHandle *handle;
	  
	  mn_vfs_async_open(&handle,
			    uri,
			    GNOME_VFS_OPEN_READ,
			    mn_mbox_mailbox_check_open_cb,
			    mailbox);
	  return;
	}
    }
  else
    mn_mailbox_set_error(mailbox, _("unable to get mailbox information: %s"), gnome_vfs_result_to_string(result));

  mn_mailbox_end_check(mailbox);
}

static void
mn_mbox_mailbox_check_open_cb (MNVFSAsyncHandle *handle,
			       GnomeVFSResult result,
			       gpointer user_data)
{
  MNmboxMailbox *mailbox = user_data;

  if (result == GNOME_VFS_OK)
    {
      CheckInfo *info;

      info = g_new(CheckInfo, 1);
      info->mailbox = mailbox;
      info->in_header = FALSE;
      info->seen = TRUE; /* so that mailbox with errors or empty mailbox will not be reported as having new mail */

      mn_vfs_async_read_line(handle, 0, mn_mbox_mailbox_check_read_line_cb, info);
    }
  else
    {
      mn_mailbox_set_error(MN_MAILBOX(mailbox), _("unable to open mailbox: %s"), gnome_vfs_result_to_string(result));
      mn_mailbox_end_check(MN_MAILBOX(mailbox));
    }
}

static void
mn_mbox_mailbox_check_read_line_cb (MNVFSAsyncHandle *handle,
				    GnomeVFSResult result,
				    const char *line,
				    gpointer user_data)
{
  CheckInfo *info = user_data;

  if (line)
    {
      if (! *line)		/* end of headers */
	{
	  if (! info->seen)
	    goto end;
	  info->in_header = FALSE;
	}
      else if (! strncmp(line, "From ", 5))
	{
	  info->in_header = TRUE;
	  info->seen = FALSE;
	}
      else if (info->in_header
	       && ! strncmp(line, "Status:", 7)
	       && (strchr(line, 'O') || strchr(line, 'R')))
	info->seen = TRUE;
    }

  if (result == GNOME_VFS_OK)
    mn_vfs_async_read_line(handle, 0, mn_mbox_mailbox_check_read_line_cb, info);
  else				/* we're done */
    {
      if (result != GNOME_VFS_ERROR_EOF)
	mn_mailbox_set_error(MN_MAILBOX(info->mailbox), _("error while reading mailbox: %s"), gnome_vfs_result_to_string(result));

    end:
      mn_mailbox_set_has_new(MN_MAILBOX(info->mailbox), ! info->seen);
      mn_vfs_async_close(handle, mn_mbox_mailbox_check_close_cb, info);
    }
}

static void
mn_mbox_mailbox_check_close_cb (MNVFSAsyncHandle *handle,
				GnomeVFSResult result,
				gpointer user_data)
{
  CheckInfo *info = user_data;

  if (result != GNOME_VFS_OK)
    mn_mailbox_set_error(MN_MAILBOX(info->mailbox), _("unable to close mailbox: %s"), gnome_vfs_result_to_string(result));

  mn_mailbox_end_check(MN_MAILBOX(info->mailbox));
  g_free(info);
}
