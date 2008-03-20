/*
 * JB, the Jean-Yves Lefort's Build System
 * Copyright (C) 2008 Jean-Yves Lefort <jylefort@brutele.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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

#ifndef _JB_ACTION_H
#define _JB_ACTION_H

#include <sys/types.h>
#include <sys/stat.h>
#include <glib.h>

/* package callbacks, should be defined in jbsrc/jb.c */
void jb_package_init (void);
void jb_package_configure (void);
void jb_package_add_resources (void);

void jb_action_help (void);
void jb_action_configure (void);
void jb_action_build (GSList *group_names);
void jb_action_install (GSList *group_names);
void jb_action_makedist (void);
void jb_action_clean (GSList *group_names);
void jb_action_distclean (GSList *group_names);
void jb_action_maintainerclean (GSList *group_names);

void jb_action_exec (const char *str, ...) G_GNUC_NULL_TERMINATED;

void jb_action_install_file (const char *srcfile,
			     const char *dstdir,
			     const char *owner,
			     const char *group,
			     mode_t mode);
void jb_action_install_to_file (const char *srcfile,
				const char *dstfile,
				const char *owner,
				const char *group,
				mode_t mode);

void jb_action_install_data (const char *srcfile, const char *dstdir);
void jb_action_install_data_to_file (const char *srcfile, const char *dstfile);
void jb_action_install_data_list (GSList *srcfiles, const char *dstdir);

void jb_action_install_program (const char *srcfile, const char *dstdir);
void jb_action_install_library (const char *srcfile, const char *dstdir);

void jb_action_rm (const char *file);
void jb_action_rm_array (char **files, int len);
void jb_action_rm_list (GSList *files);

void jb_action_rmdir (const char *dir);
void jb_action_rmdir_list (GSList *dirs);

void jb_action_add_to_dist (const char *file);
void jb_action_add_to_dist_list (GSList *files);
void jb_action_add_to_dist_string_list (const char *files);

const char *jb_action_get_distdir (void);

#endif /* _JB_ACTION_H */
