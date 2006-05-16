/* 
 * Copyright (C) 2004-2006 Jean-Yves Lefort <jylefort@brutele.be>
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

typedef struct _MNVFSReadLineContext MNVFSReadLineContext;

GnomeVFSResult mn_vfs_read_line (MNVFSReadLineContext **context,
				 GnomeVFSHandle *handle,
				 const char **line);
void mn_vfs_read_line_context_free (MNVFSReadLineContext *context);

gboolean mn_vfs_test (GnomeVFSURI *uri, GFileTest test);

GnomeVFSResult mn_vfs_read_entire_file_uri (GnomeVFSURI *uri,
					    int *file_size,
					    char **file_contents);

char *mn_vfs_get_local_path (GnomeVFSURI *uri);

#endif /* _MN_VFS_H */
