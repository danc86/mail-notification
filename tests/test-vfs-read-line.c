/* 
 * Copyright (C) 2004 Jean-Yves Lefort <jylefort@brutele.be>
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

#include "mn-vfs.h"

int
main (int argc, char **argv)
{
  GnomeVFSResult result;
  GnomeVFSHandle *handle;
  MNVFSReadLineContext *context = NULL;
  const char *line;

  g_log_set_fatal_mask(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL);

  if (argc != 2)
    g_critical("wrong number of arguments");

  if (! gnome_vfs_init())
    g_critical("unable to initialize GnomeVFS");

  result = gnome_vfs_open(&handle, argv[1], GNOME_VFS_OPEN_READ);
  if (result != GNOME_VFS_OK)
    g_critical("unable to open %s: %s", argv[1], gnome_vfs_result_to_string(result));
  
  while ((result = mn_vfs_read_line(&context, handle, &line)) == GNOME_VFS_OK)
    g_print("line: %s\n", line);

  if (result != GNOME_VFS_OK && result != GNOME_VFS_ERROR_EOF)
    g_critical("error while reading %s: %s", argv[1], gnome_vfs_result_to_string(result));

  mn_vfs_read_line_context_free(context);
  gnome_vfs_close(handle);

  return 0;
}
