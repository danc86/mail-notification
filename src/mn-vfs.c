/* 
 * Mail Notification
 * Copyright (C) 2003-2006 Jean-Yves Lefort <jylefort@brutele.be>
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"
#include <string.h>
#include <glib/gi18n.h>
#include "mn-vfs.h"

/*** cpp *********************************************************************/

#ifndef MN_REGRESSION_TEST
#define READ_LINE_BLOCK_SIZE		16384
#endif

/*** types *******************************************************************/

struct _MNVFSReadLineContext
{
  GnomeVFSHandle	*handle;
  GString		*buf;
  char			*terminator;
  GnomeVFSResult	last_result;
  gboolean		eof;
};

/*** implementation **********************************************************/

/* FIXME: must also handle \r and \r\n terminators */
GnomeVFSResult
mn_vfs_read_line (MNVFSReadLineContext **context,
		  GnomeVFSHandle *handle,
		  const char **line)
{
  GnomeVFSResult result;
  gboolean first_pass = TRUE;

  g_return_val_if_fail(context != NULL, GNOME_VFS_ERROR_BAD_PARAMETERS);
  g_return_val_if_fail(handle != NULL, GNOME_VFS_ERROR_BAD_PARAMETERS);
  g_return_val_if_fail(line != NULL, GNOME_VFS_ERROR_BAD_PARAMETERS);

  if (*context)
    {
      g_return_val_if_fail((*context)->handle == handle, GNOME_VFS_ERROR_BAD_PARAMETERS);
      if ((*context)->terminator)
	{
	  g_string_erase((*context)->buf, 0, (*context)->terminator - (*context)->buf->str + 1);
	  (*context)->terminator = NULL;
	}
      else if ((*context)->eof)
	return GNOME_VFS_ERROR_EOF; /* we're done */
    }
  else
    {
      *context = g_new0(MNVFSReadLineContext, 1);
      (*context)->handle = handle;
      (*context)->buf = g_string_new(NULL);
    }

  while (TRUE)
    {
      if (! (*context)->buf->str[0] || ! first_pass)
	{
	  char buf[READ_LINE_BLOCK_SIZE];
	  GnomeVFSFileSize bytes_read;

	  result = (*context)->last_result = gnome_vfs_read(handle, buf, sizeof(buf), &bytes_read);
	  if (result == GNOME_VFS_OK || result == GNOME_VFS_ERROR_EOF)
	    g_string_append_len((*context)->buf, buf, bytes_read);
	  else
	    break;		/* error */
	}

      (*context)->terminator = strchr((*context)->buf->str, '\n');
      if ((*context)->terminator || (*context)->last_result == GNOME_VFS_ERROR_EOF)
	{
	  result = (*context)->last_result;
	  if ((*context)->terminator || (*context)->buf->str[0])
	    {
	      *line = (*context)->buf->str;
	      if (result == GNOME_VFS_ERROR_EOF)
		result = GNOME_VFS_OK;
	    }
	  if ((*context)->terminator)
	    (*context)->terminator[0] = 0;
	  else if ((*context)->last_result == GNOME_VFS_ERROR_EOF)
	    (*context)->eof = TRUE;

	  break;		/* line found, or last line */
	}

      first_pass = FALSE;
    }

  return result;
}

void
mn_vfs_read_line_context_free (MNVFSReadLineContext *context)
{
  g_return_if_fail(context != NULL);

  g_string_free(context->buf, TRUE);
  g_free(context);
}

gboolean
mn_vfs_test (GnomeVFSURI *uri, GFileTest test)
{
  GnomeVFSFileInfoOptions options;
  GnomeVFSFileInfo *file_info;
  gboolean status = FALSE;

  g_return_val_if_fail(uri != NULL, FALSE);

  options = GNOME_VFS_FILE_INFO_DEFAULT;
  if (! (test & G_FILE_TEST_IS_SYMLINK))
    options |= GNOME_VFS_FILE_INFO_FOLLOW_LINKS;
  if (test & G_FILE_TEST_IS_EXECUTABLE)
    options |= GNOME_VFS_FILE_INFO_GET_ACCESS_RIGHTS;

  file_info = gnome_vfs_file_info_new();
  if (gnome_vfs_get_file_info_uri(uri, file_info, options) == GNOME_VFS_OK)
    {
      if (file_info->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_TYPE)
	{
	  if (test & G_FILE_TEST_IS_REGULAR)
	    status = file_info->type == GNOME_VFS_FILE_TYPE_REGULAR;
	  if (! status && test & G_FILE_TEST_IS_SYMLINK)
	    status = file_info->type == GNOME_VFS_FILE_TYPE_SYMBOLIC_LINK;
	  if (! status && test & G_FILE_TEST_IS_DIR)
	    status = file_info->type == GNOME_VFS_FILE_TYPE_DIRECTORY;
	}
      if (file_info->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_PERMISSIONS)
	{
	  if (! status && test & G_FILE_TEST_IS_EXECUTABLE)
	    status = file_info->permissions & GNOME_VFS_PERM_ACCESS_EXECUTABLE;
	}
      if (! status && test & G_FILE_TEST_EXISTS)
	status = TRUE; /* gnome_vfs_get_file_info() succeeded, so the file exists */
    }
  gnome_vfs_file_info_unref(file_info);

  return status;
}

GnomeVFSResult
mn_vfs_read_entire_file_uri (GnomeVFSURI *uri,
			     int *file_size,
			     char **file_contents)
{
  char *text_uri;
  GnomeVFSResult result;

  g_return_val_if_fail(uri != NULL, GNOME_VFS_ERROR_BAD_PARAMETERS);

  text_uri = gnome_vfs_uri_to_string(uri, GNOME_VFS_URI_HIDE_NONE);
  result = gnome_vfs_read_entire_file(text_uri, file_size, file_contents);
  g_free(text_uri);

  return result;
}

GnomeVFSResult
mn_vfs_write_entire_file_uri (GnomeVFSURI *uri,
			      gsize file_size,
			      const char *file_contents,
			      gboolean exclusive,
			      unsigned int perms)
{
  GnomeVFSHandle *handle;
  GnomeVFSResult result;
  GnomeVFSFileSize bytes_written = 0;

  result = gnome_vfs_create_uri(&handle, uri, GNOME_VFS_OPEN_WRITE | GNOME_VFS_OPEN_TRUNCATE, exclusive, perms);
  if (result != GNOME_VFS_OK)
    return result;

  while (bytes_written < file_size)
    {
      GnomeVFSFileSize this_bytes_written;

      result = gnome_vfs_write(handle, file_contents + bytes_written, file_size - bytes_written, &this_bytes_written);
      if (result != GNOME_VFS_OK)
	{
	  gnome_vfs_close(handle);
	  return result;
	}

      bytes_written += this_bytes_written;
    }

  return gnome_vfs_close(handle);
}

gboolean
mn_vfs_write_entire_file_uri_safe (GnomeVFSURI *uri,
				   gsize file_size,
				   const char *file_contents,
				   unsigned int perms,
				   GError **err)
{
  GnomeVFSResult result;
  char *text_uri;
  GnomeVFSURI *tmp_uri;
  char *tmp_text_uri;
  GnomeVFSURI *old_uri;
  char *old_text_uri;
  gboolean status = FALSE;
  gboolean old_exists;

  g_return_val_if_fail(uri != NULL, FALSE);

  text_uri = gnome_vfs_uri_to_string(uri, GNOME_VFS_URI_HIDE_PASSWORD);
  tmp_uri = mn_vfs_uri_append_file_suffix(uri, ".tmp");
  tmp_text_uri = gnome_vfs_uri_to_string(tmp_uri, GNOME_VFS_URI_HIDE_PASSWORD);
  old_uri = mn_vfs_uri_append_file_suffix(uri, ".old");
  old_text_uri = gnome_vfs_uri_to_string(old_uri, GNOME_VFS_URI_HIDE_PASSWORD);

  if (mn_vfs_test(tmp_uri, G_FILE_TEST_EXISTS))
    {
      result = gnome_vfs_unlink_from_uri(tmp_uri);
      if (result != GNOME_VFS_OK)
	{
	  g_set_error(err, 0, 0, _("Unable to remove %s: %s."), tmp_text_uri, gnome_vfs_result_to_string(result));
	  goto end;
	}
    }

  result = mn_vfs_write_entire_file_uri(tmp_uri, file_size, file_contents, TRUE, perms);
  if (result != GNOME_VFS_OK)
    {
      g_set_error(err, 0, 0, _("Unable to write %s: %s."), tmp_text_uri, gnome_vfs_result_to_string(result));
      goto end;
    }

  old_exists = mn_vfs_test(uri, G_FILE_TEST_EXISTS);
  if (old_exists)
    {
      result = gnome_vfs_move_uri(uri, old_uri, TRUE);
      if (result != GNOME_VFS_OK)
	{
	  g_set_error(err, 0, 0, _("Unable to rename %s to %s: %s."), text_uri, old_text_uri, gnome_vfs_result_to_string(result));
	  goto end;
	}
    }

  result = gnome_vfs_move_uri(tmp_uri, uri, TRUE);
  if (result != GNOME_VFS_OK)
    {
      g_set_error(err, 0, 0, _("Unable to rename %s to %s: %s."), tmp_text_uri, text_uri, gnome_vfs_result_to_string(result));
      goto end;
    }

  if (old_exists)
    {
      GnomeVFSResult this_result;

      this_result = gnome_vfs_unlink_from_uri(old_uri);
      if (this_result != GNOME_VFS_OK) /* non fatal */
	g_warning(_("unable to delete %s: %s"), old_text_uri, gnome_vfs_result_to_string(this_result));
    }

  status = TRUE;		/* success */

 end:
  g_free(text_uri);
  gnome_vfs_uri_unref(tmp_uri);
  g_free(tmp_text_uri);
  gnome_vfs_uri_unref(old_uri);
  g_free(old_text_uri);

  return status;
}

GnomeVFSURI *
mn_vfs_uri_append_file_suffix (GnomeVFSURI *uri, const char *suffix)
{
  GnomeVFSURI *result;

  g_return_val_if_fail(uri != NULL, NULL);
  g_return_val_if_fail(suffix != NULL, NULL);

  result = gnome_vfs_uri_dup(uri);
  if (result->text)
    {
      char *new_text;

      new_text = g_strconcat(result->text, suffix, NULL);
      g_free(result->text);
      result->text = new_text;
    }
  else
    result->text = g_strdup(suffix);

  return result;
}

char *
mn_vfs_get_local_path (GnomeVFSURI *uri)
{
  char *text_uri;
  char *path;

  g_return_val_if_fail(uri != NULL, NULL);

  text_uri = gnome_vfs_uri_to_string(uri, GNOME_VFS_URI_HIDE_NONE);
  path = gnome_vfs_get_local_path_from_uri(text_uri);
  g_free(text_uri);

  return path;
}
