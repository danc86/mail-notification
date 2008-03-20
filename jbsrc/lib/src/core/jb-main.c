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

#include <locale.h>
#include <string.h>
#include "jb-action.h"
#include "jb-feature.h"
#include "jb-util.h"
#include "jb-variable.h"
#include "jb-compile-options.h"

char *jb_topsrcdir = NULL;

static gboolean
parse_variable (const char *pair, char **name, char **value)
{
  char *p;
  char *_name = NULL;

  p = strchr(pair, '=');
  if (! p)
    return FALSE;

  _name = g_strndup(pair, p - pair);
  if (*_name == '\0')
    return FALSE;

  *name = _name;
  *value = g_strdup(p + 1);

  return TRUE;
}

static void
handle_variable (const char *pair)
{
  char *name;
  char *value;
  JBVariable *variable;
  GError *err = NULL;

  if (! parse_variable(pair, &name, &value))
    jb_error("invalid variable specification \"%s\"", pair);

  variable = jb_variable_get_variable(name);
  if (variable == NULL || (variable->flags & JB_VARIABLE_USER_SETTABLE) == 0)
    jb_error("unknown variable \"%s\"", name);

  if (! jb_variable_set_from_string(variable, value, &err))
    jb_error("invalid value \"%s\" for %s variable \"%s\": %s",
	     value,
	     variable->type->name,
	     name,
	     err->message);

  variable->user_set = TRUE;
}

static GSList *
parse_group_args (int argc, char **argv)
{
  GSList *group_names = NULL;
  int i;

  for (i = 0; i < argc; i++)
    group_names = g_slist_append(group_names, g_strdup(argv[i]));

  return group_names;
}

static void
parse_args (int argc, char **argv)
{
  GSList *group_names;
  int i = 1;

  if (argc >= 2)
    {
      if (! strcmp(argv[1], "help")
	  || ! strcmp(argv[1], "--help")
	  || ! strcmp(argv[1], "-h")
	  || ! strcmp(argv[1], "-?"))
	{
	  if (argc != 2)
	    jb_error("too many arguments for \"help\" action; run \"./jb help\" for an usage summary");

	  jb_action_help();
	}
      else if (! strcmp(argv[1], "configure"))
	{
	  for (i++; i < argc; i++)
	    handle_variable(argv[i]);

	  jb_action_configure();
	}
      else if (! strcmp(argv[1], "build"))
	{
	  group_names = parse_group_args(argc - 2, argv + 2);
	  jb_action_build(group_names);
	}
      else if (! strcmp(argv[1], "install"))
	{
	  group_names = parse_group_args(argc - 2, argv + 2);
	  jb_action_install(group_names);
	}
      else if (! strcmp(argv[1], "makedist"))
	{
	  if (argc != 2)
	    jb_error("too many arguments for \"makedist\" action; run \"./jb help\" for an usage summary");

	  jb_action_makedist();
	}
      else if (! strcmp(argv[1], "clean"))
	{
	  group_names = parse_group_args(argc - 2, argv + 2);
	  jb_action_clean(group_names);
	}
      else if (! strcmp(argv[1], "distclean"))
	{
	  group_names = parse_group_args(argc - 2, argv + 2);
	  jb_action_distclean(group_names);
	}
      else if (! strcmp(argv[1], "maintainerclean"))
	{
	  group_names = parse_group_args(argc - 2, argv + 2);
	  jb_action_maintainerclean(group_names);
	}
      else
	jb_error("unknown action \"%s\"; run \"./jb help\" for an usage summary", argv[1]);
    }
  else
    jb_error("not enough arguments; run \"./jb help\" for an usage summary");
}

void
jb_main (int argc,
	 char **argv,
	 const char *package,
	 const char *version,
	 const char *human_package,
	 const JBFeature **features,
	 int num_features)
{
  setlocale(LC_ALL, "");

  g_log_set_fatal_mask(G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL);

  g_type_init();

  jb_topsrcdir = g_get_current_dir();

  jb_set_log_file("build/jb.log");

  jb_variable_init();

  jb_compile_options = jb_compile_options_new("package");

  jb_feature_set_list(features, num_features);

  /* these flags are needed by the JB GOB2 build mechanism */
  jb_compile_options_add_gob2flags(jb_compile_options, "--always-private-header --no-touch");

  jb_variable_set_string("package", package);
  jb_variable_set_string("version", version);
  jb_variable_set_string("human-package", human_package);

  jb_variable_add_string("cc",
			 "C compiler command",
			 jb_variable_group_compiler_options,
			 0,
			 "cc");
  jb_variable_add_string("cflags",
			 "C compiler flags",
			 jb_variable_group_compiler_options,
			 0,
			 "-O2");
  jb_variable_add_string("cppflags",
			 "C preprocessor flags",
			 jb_variable_group_compiler_options,
			 0,
			 NULL);
  jb_variable_add_string("ldflags",
			 "C linker flags",
			 jb_variable_group_compiler_options,
			 0,
			 NULL);
  jb_variable_add_bool("cc-dependency-tracking",
		       "enable C compiler dependency tracking",
		       jb_variable_group_compiler_options,
		       0,
		       TRUE);

  jb_variable_add_string("destdir",
			 "destination directory",
			 jb_variable_group_installation_options,
			 0,
			 NULL);
  jb_variable_add_string("prefix",
			 "installation prefix",
			 jb_variable_group_installation_options,
			 0,
			 "/usr/local");

  jb_variable_add_string("bindir",
			 "user executables directory",
			 jb_variable_group_installation_options,
			 0,
			 "$prefix/bin");
  jb_variable_add_string("libdir",
			 "shared libraries directory",
			 jb_variable_group_installation_options,
			 0,
			 "$prefix/lib");
  jb_variable_add_string("libexecdir",
			 "private executables directory",
			 jb_variable_group_installation_options,
			 0,
			 "$prefix/libexec");
  jb_variable_add_string("datadir",
			 "read-only architecture-independent data directory",
			 jb_variable_group_installation_options,
			 0,
			 "$prefix/share");
  jb_variable_add_string("sysconfdir",
			 "per-machine configuration directory",
			 jb_variable_group_installation_options,
			 0,
			 "$prefix/etc");
  jb_variable_add_string("localstatedir",
			 "per-machine state directory",
			 jb_variable_group_installation_options,
			 0,
			 "$prefix/var");

  jb_variable_set_string("pkglibdir", "$libdir/$package");
  jb_variable_set_string("pkgdatadir", "$datadir/$package");
  jb_variable_set_string("pkgsysconfdir", "$sysconfdir/$package");

  jb_variable_add_mode("data-mode",
		       "data file permissions (octal)",
		       jb_variable_group_installation_options,
		       0,
		       0644);
  jb_variable_add_string("data-owner",
			 "data file owner",
			 jb_variable_group_installation_options,
			 0,
			 NULL);
  jb_variable_add_string("data-group",
			 "data file group",
			 jb_variable_group_installation_options,
			 0,
			 NULL);
  jb_variable_add_mode("program-mode",
		       "program file permissions (octal)",
		       jb_variable_group_installation_options,
		       0,
		       0755);
  jb_variable_add_string("program-owner",
			 "program file owner",
			 jb_variable_group_installation_options,
			 0,
			 NULL);
  jb_variable_add_string("program-group",
			 "program file group",
			 jb_variable_group_installation_options,
			 0,
			 NULL);
  jb_variable_add_mode("library-mode",
		       "library file permissions (octal)",
		       jb_variable_group_installation_options,
		       0,
		       0644);
  jb_variable_add_string("library-owner",
			 "library file owner",
			 jb_variable_group_installation_options,
			 0,
			 NULL);
  jb_variable_add_string("library-group",
			 "library file group",
			 jb_variable_group_installation_options,
			 0,
			 NULL);

  jb_feature_init();
  jb_package_init();

  parse_args(argc, argv);
}

void
jb_set_prefix_from_program (const char *name)
{
  char *program;

  g_return_if_fail(name != NULL);

  program = g_find_program_in_path(name);
  if (program != NULL)
    {
      char *parent;
      char *prefix;

      parent = g_path_get_dirname(program);
      g_free(program);

      prefix = g_path_get_dirname(parent);
      g_free(parent);

      jb_variable_set_string("prefix", prefix);
      g_free(prefix);
    }
}
