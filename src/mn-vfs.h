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

#ifndef _MN_VFS_H
#define _MN_VFS_H

#include "config.h"
#include <glib.h>
#include <libgnomevfs/gnome-vfs.h>

typedef struct _MNVFSAsyncHandle MNVFSAsyncHandle;

typedef void (MNVFSAsyncCallback) (MNVFSAsyncHandle *handle,
				   GnomeVFSResult result,
				   gpointer user_data);

typedef MNVFSAsyncCallback MNVFSAsyncOpenCallback;
typedef MNVFSAsyncCallback MNVFSAsyncCloseCallback;

typedef void (MNVFSAsyncReadLineCallback) (MNVFSAsyncHandle *handle,
					   GnomeVFSResult result,
					   const char *line,
					   gpointer user_data);

typedef void (MNVFSAsyncTestCallback) (gboolean result, gpointer user_data);

#define MN_VFS_PASS_NEWLINE		(1 << 0)

void mn_vfs_async_open (MNVFSAsyncHandle **handle,
			const char *text_uri,
			GnomeVFSOpenMode open_mode,
			MNVFSAsyncOpenCallback *callback,
			gpointer user_data);
void mn_vfs_async_close (MNVFSAsyncHandle *handle,
			 MNVFSAsyncCloseCallback *callback,
			 gpointer user_data);
void mn_vfs_async_read_line (MNVFSAsyncHandle *handle,
			     unsigned int flags,
			     MNVFSAsyncReadLineCallback *callback,
			     gpointer user_data);

void mn_vfs_async_test (const char *uri,
			GFileTest test,
			MNVFSAsyncTestCallback *callback,
			gpointer user_data);

#endif /* _MN_VFS_H */
