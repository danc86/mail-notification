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

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "jb-group.h"
#include "jb-resource.h"
#include "jb-tests.h"
#include "jb-feature.h"
#include "jb-config.h"
#include "jb-action.h"

static const char *standard_distfiles[] = {
  "jb",
  "jbsrc/jb.c",
  "jbsrc/lib/COPYING",
  "jbsrc/lib/README",
  "jbsrc/tools/config.guess",
  "jbsrc/tools/config.sub",
  JB_PACKAGE_SOURCES,
  JB_SOURCES
};

static const char *optional_distfiles[] = {
  "AUTHORS",
  "COPYING",
  "COPYING-DOCS",
  "INSTALL",
  "NEWS",
  "README",
  "TODO",
  "TRANSLATING",
  "po/POTFILES.in",
  "po/POTFILES.skip"
};

static const char *distcleanfiles[] = {
  JB_CONFIG_FILE,
  "build/jb",
  "build/jb.c",
  "build/jbsrc/jb",
  "build/jbsrc/jb.c"
};

static const char *maintainercleanfiles[] = {
  "jb",
  "jbsrc/lib",
  "jbsrc/tools/config.guess",
  "jbsrc/tools/config.sub"
};

static GSList *
get_groups (GSList *group_names)
{
  GSList *l;
  GSList *groups = NULL;

  if (group_names == NULL)
    groups = g_slist_copy(jb_groups);
  else
    JB_LIST_FOREACH(l, group_names)
      {
	const char *name = l->data;
	JBGroup *group;

	group = jb_group_get(name);
	if (group == NULL)
	  jb_error("unknown group \"%s\"", name);

	groups = g_slist_append(groups, group);
      }

  return groups;
}

static void
perform_action (GSList *groups, void (*perform) (JBResource *res))
{
  GSList *la;

  JB_LIST_FOREACH(la, groups)
    {
      JBGroup *group = la->data;
      GSList *lb;

      JB_LIST_FOREACH(lb, group->resources)
	{
	  JBResource *res = lb->data;

	  perform(res);
	}
    }
}

static void
core_configure (void)
{
  jb_check_cc_dependency_style();
}

static void
report_variable_group (const JBVariableGroup *group, int varname_len)
{
  GSList *l;

  jb_message("");
  jb_message("  %s:", group->name);
  jb_message("");

  JB_LIST_FOREACH(l, jb_variables)
    {
      JBVariable *variable = l->data;

      if (variable->group == group && (variable->flags & JB_VARIABLE_NO_REPORT) == 0)
	{
	  char *varname;
	  char *value;

	  varname = g_strdup_printf("%s:", variable->name);
	  value = jb_variable_to_string(variable);

	  jb_message("    %-*s %s", varname_len, varname, value);

	  g_free(varname);
	  g_free(value);
	}
    }
}

static int
get_longest_displayable_variable_name_len (void)
{
  GSList *l;
  int longest = 0;

  JB_LIST_FOREACH(l, jb_variables)
    {
      JBVariable *variable = l->data;
      int len;

      if (variable->group == NULL)
	continue;

      len = strlen(variable->name) + 1;	/* +1 for the semicolon */
      if (len > longest)
	longest = len;
    }

  return longest;
}

static void
print_configure_report (void)
{
  int varname_len;
  GSList *l;

  jb_message("");
  jb_message_expand("$human-package $version was configured successfully.", NULL);
  jb_message("The following variables are in effect:");

  varname_len = get_longest_displayable_variable_name_len();

  JB_LIST_FOREACH(l, jb_variable_groups)
    report_variable_group(l->data, varname_len);

  jb_message("");
  jb_message_expand("Type \"./jb build\" to build $human-package $version.", NULL);
}

static void
show_variable_group_help (const JBVariableGroup *group)
{
  GSList *l;

  jb_message("");
  jb_message("  %s:", group->name);

  JB_LIST_FOREACH(l, jb_variables)
    {
      JBVariable *variable = l->data;
      char *value;

      if (variable->group != group)
	continue;

      jb_message("");
      jb_message("    variable:      %s (%s)", variable->name, jb_variable_get_type_name(variable));
      jb_message("    description:   %s", variable->description);

      value = jb_variable_to_string(variable);
      jb_message("    default value: %s", value);
      g_free(value);
    }
}

void
jb_action_help (void)
{
  GSList *l;

  jb_message("Usage:");
  jb_message("");
  jb_message("  ./jb help");
  jb_message("  ./jb configure [VARIABLE=VALUE...]");
  jb_message("  ./jb build [GROUP...]");
  jb_message("  ./jb install [GROUP...]");
  jb_message("  ./jb makedist");
  jb_message("  ./jb clean [GROUP...]");
  jb_message("  ./jb distclean [GROUP...]");
  jb_message("  ./jb maintainerclean [GROUP...]");
  jb_message("");
  jb_message("Variables:");

  JB_LIST_FOREACH(l, jb_variable_groups)
    show_variable_group_help(l->data);
}

static void
configure_real (void)
{
  jb_set_log_file("build/configure.log");

  jb_feature_configure();
  core_configure();
  jb_package_configure();

  /* remove our temporary test files */
  unlink("build/test");
  unlink("build/test.c");
  unlink("build/test.o");
  unlink("build/test.o.deps");

  jb_config_save();
}

void
jb_action_configure (void)
{
  configure_real();

  print_configure_report();
}

static void
ensure_configure (void)
{
  if (g_file_test(JB_CONFIG_FILE, G_FILE_TEST_EXISTS))
    jb_config_load();
  else
    {
      jb_message_expand("configuring $human-package $version with default values since \"./jb configure\" was not run...", NULL);
      configure_real();
    }
}

static void
build_real (GSList *groups)
{
  jb_set_log_file("build/build.log");

  perform_action(groups, jb_resource_pre_build);
  perform_action(groups, jb_resource_build);
}

void
jb_action_build (GSList *group_names)
{
  GSList *groups;

  ensure_configure();
  jb_package_add_resources();

  groups = get_groups(group_names);

  build_real(groups);

  jb_message("");
  jb_message_expand("$human-package $version was built successfully.", NULL);
  jb_message_expand("Type \"sudo ./jb install\" to install $human-package $version.", NULL);

  g_slist_free(groups);
}

static void
install_real (GSList *groups)
{
  jb_set_log_file("build/install.log");

  perform_action(groups, jb_resource_install);
}

void
jb_action_install (GSList *group_names)
{
  GSList *groups;

  ensure_configure();
  jb_package_add_resources();

  groups = get_groups(group_names);

  build_real(groups);
  install_real(groups);

  jb_message("");
  jb_message_expand("$human-package $version was installed successfully.", NULL);

  g_slist_free(groups);
}

static void
add_standard_distfiles_to_dist (void)
{
  int i;

  for (i = 0; i < G_N_ELEMENTS(standard_distfiles); i++)
    jb_action_add_to_dist_string_list(standard_distfiles[i]);
}

static void
add_optional_distfiles_to_dist (void)
{
  int i;

  for (i = 0; i < G_N_ELEMENTS(optional_distfiles); i++)
    {
      const char *file = optional_distfiles[i];

      if (g_file_test(file, G_FILE_TEST_EXISTS))
	jb_action_add_to_dist(file);
    }
}

static void
makedist_real (GSList *groups)
{
  jb_set_log_file("build/makedist.log");

  /* just in case */
  jb_rmtree(jb_action_get_distdir());

  add_standard_distfiles_to_dist();
  add_optional_distfiles_to_dist();

  perform_action(groups, jb_resource_makedist);
}

void
jb_action_makedist (void)
{
  GSList *groups;
  char *tarball;

  ensure_configure();
  jb_package_add_resources();

  groups = get_groups(NULL);

  build_real(groups);
  makedist_real(groups);

  tarball = jb_variable_expand("${package}-$version.tar.bz2", NULL);

  jb_message("creating build/%s", tarball);

  jb_action_exec("cd build && tar -chof - ${package}-$version | bzip2 -9 -c >$tarball",
		 "tarball", tarball,
		 NULL);

  jb_rmtree(jb_action_get_distdir());

  jb_message("");
  jb_message("build/%s was created successfully.", tarball);

  g_slist_free(groups);
  g_free(tarball);
}

static void
clean_real (GSList *groups)
{
  jb_set_log_file("build/clean.log");

  perform_action(groups, jb_resource_clean);
}

void
jb_action_clean (GSList *group_names)
{
  GSList *groups;

  ensure_configure();
  jb_package_add_resources();

  groups = get_groups(group_names);

  clean_real(groups);

  jb_message("");
  jb_message_expand("$human-package $version was cleaned successfully.", NULL);

  g_slist_free(groups);
}

static void
distclean_real (GSList *groups)
{
  jb_set_log_file("build/distclean.log");

  perform_action(groups, jb_resource_distclean);

  jb_action_rm_array((char **) distcleanfiles, G_N_ELEMENTS(distcleanfiles));
}

void
jb_action_distclean (GSList *group_names)
{
  GSList *groups;

  ensure_configure();
  jb_package_add_resources();

  groups = get_groups(group_names);

  clean_real(groups);
  distclean_real(groups);

  jb_message("");
  jb_message_expand("$human-package $version was dist-cleaned successfully.", NULL);

  g_slist_free(groups);
}

static void
maintainerclean_real (GSList *groups)
{
  jb_set_log_file("build/maintainerclean.log");

  perform_action(groups, jb_resource_maintainerclean);

  jb_action_rm_array((char **) maintainercleanfiles, G_N_ELEMENTS(maintainercleanfiles));
}

void
jb_action_maintainerclean (GSList *group_names)
{
  GSList *groups;

  ensure_configure();
  jb_package_add_resources();

  groups = get_groups(group_names);

  clean_real(groups);
  distclean_real(groups);
  maintainerclean_real(groups);

  jb_message("");
  jb_message_expand("$human-package $version was maintainer-cleaned successfully.", NULL);

  g_slist_free(groups);
}

void
jb_action_exec (const char *str, ...)
{
  va_list args;
  char *command;
  gboolean can_fail = FALSE;
  char *standard_output;
  char *standard_error;

  g_return_if_fail(str != NULL);

  va_start(args, str);
  command = jb_variable_expandv(str, args);
  va_end(args);

  if (command[0] == '-')
    {
      char *tmp;

      can_fail = TRUE;

      tmp = g_strdup(command + 1);
      g_free(command);
      command = tmp;
    }

  if (! jb_exec(&standard_output, &standard_error, "%s", command) && ! can_fail)
    {
      g_printerr("%s\n", command);

      if (*standard_output != '\0' && *standard_error != '\0')
	{
	  g_printerr("standard output:\n");
	  g_printerr("%s\n", standard_output);

	  g_printerr("standard error:\n");
	  g_printerr("%s\n", standard_error);
	}
      else if (*standard_output != '\0')
	g_printerr("%s\n", standard_output);
      else if (*standard_error != '\0')
	g_printerr("%s\n", standard_error);

      jb_error("command failed");
    }

  g_free(command);
}

/*
 * Doing this internally is much faster than using an external program
 * (install or cp), and also more portable.
 */
static gboolean
install_file_real (const char *srcfile,
		   const char *dstfile,
		   const char *owner,
		   const char *group,
		   mode_t mode,
		   GError **err)
{
  int in;
  int out;

  in = open(srcfile, O_RDONLY);
  if (in < 0)
    {
      g_set_error(err, 0, 0, "cannot open %s for reading: %s", srcfile, g_strerror(errno));
      return FALSE;
    }

  out = open(dstfile, O_CREAT | O_WRONLY | O_TRUNC, mode);
  if (out < 0)
    {
      /*
       * Unlink the file and try again, in case the file could not be
       * opened because it already existed and was not writable. Do
       * this unconditionally without testing EPERM, since it might
       * not be portable.
       */
      unlink(dstfile);

      out = open(dstfile, O_CREAT | O_WRONLY | O_TRUNC, mode);
      if (out < 0)
	{
	  g_set_error(err, 0, 0, "cannot open %s for writing: %s", dstfile, g_strerror(errno));
	  goto error;
	}
    }

  while (TRUE)
    {
      char buf[4096];
      ssize_t bytes_read;
      ssize_t bytes_written;

      bytes_read = read(in, buf, sizeof(buf));
      if (bytes_read < 0)
	{
	  g_set_error(err, 0, 0, "cannot read from %s: %s", srcfile, g_strerror(errno));
	  goto error;
	}
      if (bytes_read == 0)
	break;

      bytes_written = write(out, buf, bytes_read);
      if (bytes_written < 0)
	{
	  g_set_error(err, 0, 0, "cannot write to %s: %s", dstfile, g_strerror(errno));
	  goto error;
	}
      if (bytes_written != bytes_read)
	{
	  g_set_error(err, 0, 0, "cannot write to %s", dstfile);
	  goto error;
	}
    }

  /*
   * The Linux manpage of fchmod() mentions that "as a security
   * measure, depending on the file system, the set-user-ID and
   * set-group-ID execution bits may be turned off if a file is
   * written", so set the ownership and permissions after writing the
   * file.
   */

  if (owner != NULL || group != NULL)
    {
      GError *tmp_err = NULL;

      if (! jb_fchown_by_name(out, owner, group, &tmp_err))
	{
	  if (owner != NULL && group != NULL)
	    g_set_error(err, 0, 0, "cannot chown %s to %s:%s: %s", dstfile, owner, group, tmp_err->message);
	  else if (owner != NULL)
	    g_set_error(err, 0, 0, "cannot chown %s to owner %s: %s", dstfile, owner, tmp_err->message);
	  else
	    g_set_error(err, 0, 0, "cannot chown %s to group %s: %s", dstfile, group, tmp_err->message);

	  g_error_free(tmp_err);
	  goto error;
	}
    }

  /*
   * Set the permissions after chowning the file, since the chown can
   * clear the setuid/setgid bits.
   */

  if (fchmod(out, mode) < 0)
    {
      g_set_error(err, 0, 0, "cannot chmod %s to " JB_MODE_FORMAT ": %s", dstfile, (unsigned int) mode, g_strerror(errno));
      goto error;
    }

  if (close(out) < 0)
    {
      g_set_error(err, 0, 0, "cannot close %s: %s", dstfile, g_strerror(errno));
      goto error;
    }

  close(in);

  return TRUE;

 error:
  close(in);
  close(out);

  return FALSE;
}

void
jb_action_install_file (const char *srcfile,
			const char *dstdir,
			const char *owner,
			const char *group,
			mode_t mode)
{
  char *srcfile_basename;
  char *dstfile;

  g_return_if_fail(srcfile != NULL);
  g_return_if_fail(dstdir != NULL);

  srcfile_basename = g_path_get_basename(srcfile);
  dstfile = g_strdup_printf("%s/%s", dstdir, srcfile_basename);
  g_free(srcfile_basename);

  jb_action_install_to_file(srcfile, dstfile, owner, group, mode);
  g_free(dstfile);
}

void
jb_action_install_to_file (const char *srcfile,
			   const char *dstfile,
			   const char *owner,
			   const char *group,
			   mode_t mode)
{
  char *real_dstfile;
  GError *err = NULL;

  g_return_if_fail(srcfile != NULL);
  g_return_if_fail(dstfile != NULL);

  real_dstfile = jb_variable_expand("$destdir$dstfile",
				    "dstfile", dstfile,
				    NULL);

  jb_message("installing %s", real_dstfile);

  jb_mkdir_of_file(real_dstfile);

  if (! install_file_real(srcfile, real_dstfile, owner, group, mode, &err))
    jb_error("%s", err->message);

  g_free(real_dstfile);
}

void
jb_action_install_data (const char *srcfile, const char *dstdir)
{
  g_return_if_fail(srcfile != NULL);
  g_return_if_fail(dstdir != NULL);

  jb_action_install_file(srcfile,
			 dstdir,
			 jb_variable_get_string_or_null("data-owner"),
			 jb_variable_get_string_or_null("data-group"),
			 jb_variable_get_mode("data-mode"));
}

void
jb_action_install_data_to_file (const char *srcfile, const char *dstfile)
{
  g_return_if_fail(srcfile != NULL);
  g_return_if_fail(dstfile != NULL);

  jb_action_install_to_file(srcfile,
			    dstfile,
			    jb_variable_get_string_or_null("data-owner"),
			    jb_variable_get_string_or_null("data-group"),
			    jb_variable_get_mode("data-mode"));
}

void
jb_action_install_data_list (GSList *srcfiles, const char *dstdir)
{
  GSList *l;

  g_return_if_fail(dstdir != NULL);

  JB_LIST_FOREACH(l, srcfiles)
    {
      const char *srcfile = l->data;

      jb_action_install_data(srcfile, dstdir);
    }
}

void
jb_action_install_program (const char *srcfile, const char *dstdir)
{
  g_return_if_fail(srcfile != NULL);
  g_return_if_fail(dstdir != NULL);

  jb_action_install_file(srcfile,
			 dstdir,
			 jb_variable_get_string_or_null("program-owner"),
			 jb_variable_get_string_or_null("program-group"),
			 jb_variable_get_mode("program-mode"));
}

void
jb_action_install_library (const char *srcfile, const char *dstdir)
{
  g_return_if_fail(srcfile != NULL);
  g_return_if_fail(dstdir != NULL);

  jb_action_install_file(srcfile,
			 dstdir,
			 jb_variable_get_string_or_null("library-owner"),
			 jb_variable_get_string_or_null("library-group"),
			 jb_variable_get_mode("library-mode"));
}

void
jb_action_rm (const char *file)
{
  g_return_if_fail(file != NULL);

  if (g_file_test(file, G_FILE_TEST_EXISTS))
    {
      jb_message("removing %s", file);
      unlink(file);
    }

  /* if the parent directories were created by JB, also remove them */

  if (g_str_has_prefix(file, "jbsrc/tools/"))
    jb_action_rmdir("jbsrc/tools");
  else if (g_str_has_prefix(file, "build/"))
    {
      GSList *parentdirs = NULL;
      char *dir;

      dir = g_path_get_dirname(file);

      while (TRUE)
	{
	  parentdirs = g_slist_append(parentdirs, dir);

	  if (! strcmp(dir, "build"))
	    break;

	  dir = g_path_get_dirname(dir);
	  g_assert(strcmp(dir, "/"));
	}

      jb_action_rmdir_list(parentdirs);

      jb_g_slist_free_deep(parentdirs);
    }
}

void
jb_action_rm_array (char **files, int len)
{
  int i;

  for (i = 0; i < len; i++)
    jb_action_rm(files[i]);
}

void
jb_action_rm_list (GSList *files)
{
  GSList *l;

  JB_LIST_FOREACH(l, files)
    jb_action_rm(l->data);
}

static gboolean
dir_is_empty (const char *dir)
{
  GDir *gdir;
  gboolean is_empty;

  gdir = g_dir_open(dir, 0, NULL);
  if (gdir == NULL)
    return TRUE;

  is_empty = g_dir_read_name(gdir) == NULL;

  g_dir_close(gdir);

  return is_empty;
}

void
jb_action_rmdir (const char *dir)
{
  g_return_if_fail(dir != NULL);

  if (g_file_test(dir, G_FILE_TEST_EXISTS) && dir_is_empty(dir))
    {
      jb_message("removing directory %s", dir);
      rmdir(dir);
    }
}

void
jb_action_rmdir_list (GSList *dirs)
{
  GSList *l;

  JB_LIST_FOREACH(l, dirs)
    jb_action_rmdir(l->data);
}

static gboolean
add_to_hash_set (JBStringHashSet **hash_set, const char *value)
{
  if (*hash_set == NULL)
    *hash_set = jb_string_hash_set_new();

  if (! jb_string_hash_set_add(*hash_set, value))
    return FALSE;

  return TRUE;
}

void
jb_action_add_to_dist (const char *file)
{
  static JBStringHashSet *distfiles = NULL;
  char *dir;
  char *distdir;

  g_return_if_fail(file != NULL);
  g_return_if_fail(*file != '\0');

  if (! add_to_hash_set(&distfiles, file))
    return;

  jb_message("adding %s to dist", file);

  dir = g_path_get_dirname(file);
  distdir = g_strdup_printf("%s/%s", jb_action_get_distdir(), dir);
  g_free(dir);

  jb_mkdir(distdir);

  jb_action_exec("cp -p $file $distdir",
		 "file", file,
		 "distdir", distdir,
		 NULL);

  g_free(distdir);
}

void
jb_action_add_to_dist_list (GSList *files)
{
  GSList *l;

  JB_LIST_FOREACH(l, files)
    {
      const char *file = l->data;
      jb_action_add_to_dist(file);
    }
}

void
jb_action_add_to_dist_string_list (const char *files)
{
  int i;
  char **array;

  g_return_if_fail(files != NULL);

  array = g_strsplit(files, " ", 0);

  for (i = 0; array[i] != NULL; i++)
    {
      const char *file = array[i];

      /* the files array can contain extra spaces */
      if (*file != '\0')
	jb_action_add_to_dist(array[i]);
    }

  g_strfreev(array);
}

const char *
jb_action_get_distdir (void)
{
  static char *distdir = NULL;

  if (distdir == NULL)
    distdir = jb_variable_expand("build/${package}-$version", NULL);

  return distdir;
}
