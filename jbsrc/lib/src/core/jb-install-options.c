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

#include "jb-install-options.h"
#include "jb-action.h"
#include "jb-variable.h"

JBInstallOptions *
jb_install_options_new (void)
{
  return g_new0(JBInstallOptions, 1);
}

void
jb_install_options_set_installdir (JBInstallOptions *self,
				   const char *installdir)
{
  g_return_if_fail(self != NULL);

  g_free(self->installdir);
  self->installdir = g_strdup(installdir);
}

void
jb_install_options_set_owner (JBInstallOptions *self, const char *owner)
{
  g_return_if_fail(self != NULL);

  g_free(self->owner);
  self->owner = g_strdup(owner);
}

void
jb_install_options_set_group (JBInstallOptions *self, const char *group)
{
  g_return_if_fail(self != NULL);

  g_free(self->group);
  self->group = g_strdup(group);
}

void
jb_install_options_set_extra_mode (JBInstallOptions *self, mode_t extra_mode)
{
  g_return_if_fail(self != NULL);

  self->extra_mode = extra_mode;
}

void
jb_install_options_install (JBInstallOptions *self,
			    const char *srcfile,
			    const char *dstfile,
			    const char *default_owner,
			    const char *default_group,
			    mode_t default_mode)
{
  const char *owner;
  const char *group;
  mode_t mode;

  g_return_if_fail(self != NULL);
  g_return_if_fail(srcfile != NULL);

  if (self->installdir == NULL)
    return;

  owner = self->owner != NULL ? self->owner : default_owner;
  group = self->group != NULL ? self->group : default_group;
  mode = default_mode | self->extra_mode;

  if (dstfile == NULL)
    jb_action_install_file(srcfile,
			   self->installdir,
			   owner,
			   group,
			   mode);
  else
    {
      char *full_dstfile;

      full_dstfile = g_strdup_printf("%s/%s", self->installdir, dstfile);
      jb_action_install_to_file(srcfile,
				full_dstfile,
				owner,
				group,
				mode);
      g_free(full_dstfile);
    }
}

void
jb_install_options_install_data (JBInstallOptions *self,
				 const char *srcfile,
				 const char *dstfile)
{
  g_return_if_fail(self != NULL);
  g_return_if_fail(srcfile != NULL);

  jb_install_options_install(self,
			     srcfile,
			     dstfile,
			     jb_variable_get_string_or_null("data-owner"),
			     jb_variable_get_string_or_null("data-group"),
			     jb_variable_get_mode("data-mode"));
}

void
jb_install_options_install_program (JBInstallOptions *self,
				    const char *srcfile,
				    const char *dstfile)
{
  g_return_if_fail(self != NULL);
  g_return_if_fail(srcfile != NULL);

  jb_install_options_install(self,
			     srcfile,
			     dstfile,
			     jb_variable_get_string_or_null("program-owner"),
			     jb_variable_get_string_or_null("program-group"),
			     jb_variable_get_mode("program-mode"));
}

void
jb_install_options_install_library (JBInstallOptions *self,
				    const char *srcfile,
				    const char *dstfile)
{
  g_return_if_fail(self != NULL);
  g_return_if_fail(srcfile != NULL);

  jb_install_options_install(self,
			     srcfile,
			     dstfile,
			     jb_variable_get_string_or_null("library-owner"),
			     jb_variable_get_string_or_null("library-group"),
			     jb_variable_get_mode("library-mode"));
}
