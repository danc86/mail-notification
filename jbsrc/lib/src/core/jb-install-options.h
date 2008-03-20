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

#ifndef _JB_INSTALL_OPTIONS_H
#define _JB_INSTALL_OPTIONS_H

#include <sys/types.h>
#include <sys/stat.h>

typedef struct
{
  char		*installdir;
  char		*owner;
  char		*group;
  mode_t	extra_mode;
} JBInstallOptions;

JBInstallOptions *jb_install_options_new (void);

void jb_install_options_set_installdir (JBInstallOptions *self,
					const char *installdir);
void jb_install_options_set_owner (JBInstallOptions *self, const char *owner);
void jb_install_options_set_group (JBInstallOptions *self, const char *group);
void jb_install_options_set_extra_mode (JBInstallOptions *self, mode_t extra_mode);

void jb_install_options_install (JBInstallOptions *self,
				 const char *srcfile,
				 const char *dstfile,
				 const char *default_owner,
				 const char *default_group,
				 mode_t default_mode);
void jb_install_options_install_data (JBInstallOptions *self,
				      const char *srcfile,
				      const char *dstfile);
void jb_install_options_install_program (JBInstallOptions *self,
					 const char *srcfile,
					 const char *dstfile);
void jb_install_options_install_library (JBInstallOptions *self,
					 const char *srcfile,
					 const char *dstfile);

#endif /* _JB_INSTALL_OPTIONS_H */
