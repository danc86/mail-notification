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
#include "mn-vfs.h"

/*** cpp *********************************************************************/

#define VFS_BLOCK_SIZE			16384

/*** types *******************************************************************/

struct _MNVFSAsyncHandle
{
  GnomeVFSAsyncHandle		*handle;
  GnomeVFSOpenMode		open_mode;

  MNVFSAsyncOpenCallback	*open_callback;
  gpointer			open_user_data;

  MNVFSAsyncCloseCallback	*close_callback;
  gpointer			close_user_data;

  MNVFSAsyncReadLineCallback	*read_line_callback;
  gpointer			read_line_user_data;

  unsigned int			flags;
  char				*buffer;
  GString			*string;
  GnomeVFSResult		last_result;
  GnomeVFSFileSize		start;
  GnomeVFSFileSize		pos;
  GnomeVFSFileSize		len;
};

typedef struct
{
  GFileTest			test;
  MNVFSAsyncTestCallback	*callback;
  gpointer			user_data;
} TestInfo;

/*** functions ***************************************************************/

static void mn_vfs_async_open_cb (GnomeVFSAsyncHandle *handle,
				  GnomeVFSResult result,
				  gpointer user_data);
static void mn_vfs_async_close_cb (GnomeVFSAsyncHandle *handle,
				   GnomeVFSResult result,
				   gpointer user_data);
static void mn_vfs_async_free (MNVFSAsyncHandle *handle);

static gboolean mn_vfs_async_read_line_idle_cb (gpointer data);
static void mn_vfs_async_read_line_cb (GnomeVFSAsyncHandle *handle,
				       GnomeVFSResult result,
				       gpointer buffer,
				       GnomeVFSFileSize bytes_requested,
				       GnomeVFSFileSize bytes_read,
				       gpointer user_data);

static gboolean mn_vfs_async_read_line_flush (MNVFSAsyncHandle *handle);

static void mn_vfs_async_test_cb (GnomeVFSAsyncHandle *handle,
				  GList *results,
				  gpointer user_data);

/*** implementation **********************************************************/

void
mn_vfs_async_open (MNVFSAsyncHandle **handle,
		   const char *text_uri,
		   GnomeVFSOpenMode open_mode,
		   MNVFSAsyncOpenCallback *callback,
		   gpointer user_data)
{
  g_return_if_fail(handle != NULL);
  g_return_if_fail(text_uri != NULL);
  g_return_if_fail(callback != NULL);

  *handle = g_new0(MNVFSAsyncHandle, 1);
  (*handle)->open_mode = open_mode;
  (*handle)->open_callback = callback;
  (*handle)->open_user_data = user_data;
  (*handle)->buffer = g_new(char, VFS_BLOCK_SIZE);
  (*handle)->string = g_string_new(NULL);
  (*handle)->last_result = GNOME_VFS_OK;
  
  gnome_vfs_async_open(&(*handle)->handle,
		       text_uri,
		       open_mode,
		       GNOME_VFS_PRIORITY_DEFAULT,
		       mn_vfs_async_open_cb,
		       *handle);
}

static void
mn_vfs_async_open_cb (GnomeVFSAsyncHandle *handle,
		      GnomeVFSResult result,
		      gpointer user_data)
{
  MNVFSAsyncHandle *mhandle = user_data;

  mhandle->open_callback(mhandle, result, mhandle->open_user_data);
  if (result != GNOME_VFS_OK)
    mn_vfs_async_free(mhandle);
}

void
mn_vfs_async_close (MNVFSAsyncHandle *handle,
		    MNVFSAsyncCloseCallback *callback,
		    gpointer user_data)
{
  g_return_if_fail(handle != NULL);
  g_return_if_fail(callback != NULL);

  handle->close_callback = callback;
  handle->close_user_data = user_data;

  gnome_vfs_async_close(handle->handle, mn_vfs_async_close_cb, handle);
}

static void
mn_vfs_async_close_cb (GnomeVFSAsyncHandle *handle,
		       GnomeVFSResult result,
		       gpointer user_data)
{
  MNVFSAsyncHandle *mhandle = user_data;

  mhandle->close_callback(mhandle, result, mhandle->close_user_data);
  mn_vfs_async_free(mhandle);
}

static void
mn_vfs_async_free (MNVFSAsyncHandle *handle)
{
  g_return_if_fail(handle != NULL);

  g_free(handle->buffer);
  g_string_free(handle->string, TRUE);
  g_free(handle);
}

/*
 * Read a UNIX line from HANDLE.
 */
void
mn_vfs_async_read_line (MNVFSAsyncHandle *handle,
			unsigned int flags,
			MNVFSAsyncReadLineCallback *callback,
			gpointer user_data)
{
  g_return_if_fail(handle != NULL);
  g_return_if_fail((handle->open_mode & GNOME_VFS_OPEN_READ) != 0);
  g_return_if_fail(callback != NULL);

  handle->flags = flags;
  handle->read_line_callback = callback;
  handle->read_line_user_data = user_data;

  g_idle_add(mn_vfs_async_read_line_idle_cb, handle);
}

static gboolean
mn_vfs_async_read_line_idle_cb (gpointer data)
{
  MNVFSAsyncHandle *handle = data;

  if (! mn_vfs_async_read_line_flush(handle))
    gnome_vfs_async_read(handle->handle,
			 handle->buffer,
			 VFS_BLOCK_SIZE,
			 mn_vfs_async_read_line_cb,
			 handle);

  return FALSE;			/* remove source */
}

static void
mn_vfs_async_read_line_cb (GnomeVFSAsyncHandle *handle,
			   GnomeVFSResult result,
			   gpointer buffer,
			   GnomeVFSFileSize bytes_requested,
			   GnomeVFSFileSize bytes_read,
			   gpointer user_data)
{
  MNVFSAsyncHandle *mhandle = user_data;

  mhandle->start = 0;
  mhandle->pos = 0;
  mhandle->len = bytes_read;
  mhandle->last_result = result;

  if (! mn_vfs_async_read_line_flush(mhandle))
    gnome_vfs_async_read(mhandle->handle,
			 mhandle->buffer,
			 VFS_BLOCK_SIZE,
			 mn_vfs_async_read_line_cb,
			 mhandle);
}

static gboolean
mn_vfs_async_read_line_flush (MNVFSAsyncHandle *handle)
{
  g_return_val_if_fail(handle != NULL, FALSE);

  for (; handle->pos < handle->len; handle->pos++)
    if (handle->buffer[handle->pos] == '\n')
      {
	g_string_append_len(handle->string, &handle->buffer[handle->start], handle->pos - handle->start);
	if (handle->flags & MN_VFS_PASS_NEWLINE)
	  g_string_append_c(handle->string, handle->buffer[handle->pos]);
	
	handle->read_line_callback(handle, handle->last_result, handle->string->str, handle->read_line_user_data);
	g_string_truncate(handle->string, 0);

	handle->start = ++handle->pos;
	return TRUE;
      }

  if (handle->start < handle->len)
    g_string_append_len(handle->string, &handle->buffer[handle->start], handle->len - handle->start);

  if (handle->last_result != GNOME_VFS_OK)
    {
      handle->read_line_callback(handle, handle->last_result, *handle->string->str ? handle->string->str : NULL, handle->read_line_user_data);
      return TRUE;
    }
    
  return FALSE;
}

void
mn_vfs_async_test (const char *uri,
		   GFileTest test,
		   MNVFSAsyncTestCallback *callback,
		   gpointer user_data)
{
  GnomeVFSURI *vfs_uri;
  GList *uri_list = NULL;
  GnomeVFSFileInfoOptions options;
  GnomeVFSAsyncHandle *handle;
  TestInfo *info;

  g_return_if_fail(uri != NULL);
  g_return_if_fail(callback != NULL);

  vfs_uri = gnome_vfs_uri_new(uri);
  if (! vfs_uri)
    {
      callback(FALSE, user_data);
      return;
    }

  uri_list = g_list_append(uri_list, vfs_uri);

  options = GNOME_VFS_FILE_INFO_DEFAULT;
  if (! (test & G_FILE_TEST_IS_SYMLINK))
    options |= GNOME_VFS_FILE_INFO_FOLLOW_LINKS;
  if (test & G_FILE_TEST_IS_EXECUTABLE)
    options |= GNOME_VFS_FILE_INFO_GET_ACCESS_RIGHTS;

  info = g_new(TestInfo, 1);
  info->test = test;
  info->callback = callback;
  info->user_data = user_data;

  gnome_vfs_async_get_file_info(&handle,
				uri_list,
				options,
				GNOME_VFS_PRIORITY_DEFAULT,
				mn_vfs_async_test_cb,
				info);

  gnome_vfs_uri_unref(vfs_uri);
  g_list_free(uri_list);
}

static void
mn_vfs_async_test_cb (GnomeVFSAsyncHandle *handle,
		      GList *results,
		      gpointer user_data)
{
  TestInfo *info = user_data;
  GnomeVFSGetFileInfoResult *result;
  gboolean status = FALSE;

  g_return_if_fail(results->data != NULL);
  result = results->data;

  if (result->result == GNOME_VFS_OK)
    {
      if (result->file_info->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_TYPE)
	{
	  if (info->test & G_FILE_TEST_IS_REGULAR)
	    status = result->file_info->type == GNOME_VFS_FILE_TYPE_REGULAR;
	  if (! status && info->test & G_FILE_TEST_IS_SYMLINK)
	    status = result->file_info->type == GNOME_VFS_FILE_TYPE_SYMBOLIC_LINK;
	  if (! status && info->test & G_FILE_TEST_IS_DIR)
	    status = result->file_info->type == GNOME_VFS_FILE_TYPE_DIRECTORY;
	}
      if (result->file_info->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_PERMISSIONS)
	{
	  if (! status && info->test & G_FILE_TEST_IS_EXECUTABLE)
	    status = result->file_info->permissions & GNOME_VFS_PERM_ACCESS_EXECUTABLE;
	}
      if (! status && info->test & G_FILE_TEST_EXISTS)
	status = TRUE; /* gnome_vfs_get_file_info() succeeded, so the file exists */
    }

  info->callback(status, info->user_data);
  g_free(info);
}
