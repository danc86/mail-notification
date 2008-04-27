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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>
#include "jb-util.h"
#include "jb-variable.h"
#include "jb-compile-options.h"
#include "jb-tests.h"
#include "jb-feature.h"

void
jb_check_host_system (void)
{
  char *output;
  gboolean status = FALSE;

  if (jb_variable_get_variable("host-cpu") != NULL)
    return;			/* already checked */

  jb_message_checking("the host system type");

  if (jb_exec(&output, NULL, "jbsrc/tools/config.sub `jbsrc/tools/config.guess`"))
    {
      char **fields;
      int len;

      fields = g_strsplit(output, "-", 0);
      len = g_strv_length(fields);

      if (len == 3)
	{
	  jb_variable_set_string("host-cpu", fields[0]);
	  jb_variable_set_string("host-manufacturer", fields[1]);
	  jb_variable_set_string("host-kernel", NULL);
	  jb_variable_set_string("host-os", fields[2]);
	  status = TRUE;
	}
      else if (len == 4)
	{
	  jb_variable_set_string("host-cpu", fields[0]);
	  jb_variable_set_string("host-manufacturer", fields[1]);
	  jb_variable_set_string("host-kernel", fields[2]);
	  jb_variable_set_string("host-os", fields[3]);
	  status = TRUE;
	}

      g_strfreev(fields);
    }

  if (status)
    jb_message_result_string(output);
  else
    jb_message_result_string("unknown");

  g_free(output);

  if (! status)
    jb_error("unable to determine host system type");
}

void
jb_register_program (const char *name, JBVariableFlags flags)
{
  char *description;
  char *program;

  g_return_if_fail(name != NULL);

  description = g_strdup_printf("%s program", name);

  program = g_find_program_in_path(name);

  jb_variable_add_string(name,
			 description,
			 jb_variable_group_external_programs,
			 flags,
			 program);

  g_free(description);
  g_free(program);
}

/* the program must have been registered with jb_register_program() */
gboolean
jb_check_program (const char *name)
{
  static GHashTable *checked_programs = NULL;
  JBVariable *variable;
  const char *program;
  gboolean result = FALSE;

  g_return_val_if_fail(name != NULL, FALSE);

  /* do not check for the same program twice */
  if (checked_programs != NULL)
    {
      gpointer checked_result;

      if (g_hash_table_lookup_extended(checked_programs, name, NULL, &checked_result))
	return GPOINTER_TO_INT(checked_result);
    }
  else
    checked_programs = g_hash_table_new(g_str_hash, g_str_equal);

  jb_message_checking("for %s", name);

  variable = jb_variable_get_variable_or_error(name);

  program = g_value_get_string(&variable->value);

  if (variable->user_set)
    {
      char *absolute_program;

      absolute_program = g_find_program_in_path(program);
      if (absolute_program != NULL)
	{
	  jb_message_result_string(absolute_program);
	  g_value_take_string(&variable->value, absolute_program);
	  result = TRUE;
	}
      else
	{
	  jb_message_result_string_format("not found (\"%s\" was specified but was not found, is a directory or is not executable)", program);
	  g_value_set_string(&variable->value, NULL);
	}
    }
  else
    {
      if (program != NULL)
	{
	  jb_message_result_string(program);
	  result = TRUE;
	}
      else
	jb_message_result_string("not found");
    }

  g_hash_table_insert(checked_programs, g_strdup(name), GINT_TO_POINTER(result));

  return result;
}

void
jb_require_program (const char *name)
{
  g_return_if_fail(name != NULL);

  if (! jb_check_program(name))
    jb_error("required program \"%s\" not found", name);
}

static void
log_c_test (const char *filename, const char *action)
{
  char *source;
  char **lines;
  int i;

  source = jb_read_file_or_exit(filename);
  lines = g_strsplit(source, "\n", 0);
  g_free(source);

  jb_log("attempting to %s program %s:", action, filename);

  for (i = 0; lines[i] != NULL; i++)
    {
      const char *line = lines[i];

      /* do not output a spurious empty last line */
      if (*line == '\0' && lines[i + 1] == NULL)
	break;

      jb_log("%4i %s", i + 1, lines[i]);
    }

  jb_log(JB_SEPARATOR);

  g_strfreev(lines);
}

gboolean
jb_test_compile (const char *filename,
		 const char *cflags,
		 const char *cppflags)
{
  g_return_val_if_fail(filename != NULL, FALSE);

  log_c_test(filename, "compile");

  return jb_exec_expand(NULL, NULL,
			"$cc -c -o build/test.o"
			" $cflags $extra-cflags"
			" $cppflags $extra-cppflags"
			" $filename",
			"extra-cflags", cflags,
			"extra-cppflags", cppflags,
			"filename", filename,
			NULL);
}

gboolean
jb_test_compile_string (const char *str,
			const char *cflags,
			const char *cppflags)
{
  g_return_val_if_fail(str != NULL, FALSE);

  jb_write_file_or_exit("build/test.c", str);

  return jb_test_compile("build/test.c", cflags, cppflags);
}

gboolean
jb_test_link (const char *filename,
	      const char *cflags,
	      const char *cppflags,
	      const char *ldflags,
	      const char *libs)
{
  g_return_val_if_fail(filename != NULL, FALSE);

  log_c_test(filename, "link");

  return jb_exec_expand(NULL, NULL,
			"$cc -o build/test"
			" $cflags $extra-cflags"
			" $cppflags $extra-cppflags"
			" $ldflags $extra-ldflags"
			" $filename"
			" $libs $extra-libs",
			"filename", filename,
			"extra-cflags", cflags,
			"extra-cppflags", cppflags,
			"extra-ldflags", ldflags,
			"extra-libs", libs,
			NULL);
}

gboolean
jb_test_link_string (const char *str,
		     const char *cflags,
		     const char *cppflags,
		     const char *ldflags,
		     const char *libs)
{
  g_return_val_if_fail(str != NULL, FALSE);

  jb_write_file_or_exit("build/test.c", str);

  return jb_test_link("build/test.c", cflags, cppflags, ldflags, libs);
}

gboolean
jb_test_run (const char *filename,
	     const char *cflags,
	     const char *cppflags,
	     const char *ldflags,
	     const char *libs)
{
  g_return_val_if_fail(filename != NULL, FALSE);

  if (! jb_test_link(filename, cflags, cppflags, ldflags, libs))
    return FALSE;

  return jb_exec(NULL, NULL, "build/test");
}

gboolean
jb_test_run_string (const char *str,
		    const char *cflags,
		    const char *cppflags,
		    const char *ldflags,
		    const char *libs)
{
  g_return_val_if_fail(str != NULL, FALSE);

  jb_write_file_or_exit("build/test.c", str);

  return jb_test_run("build/test.c", cflags, cppflags, ldflags, libs);
}

gboolean
jb_check_functions (const char *functions, const char *libname)
{
  char **functions_array;
  int i;
  GString *checking_message;
  GString *program;
  char *libs = NULL;
  gboolean result;

  g_return_val_if_fail(functions != NULL, FALSE);

  functions_array = g_strsplit(functions, " ", 0);

  checking_message = g_string_new("for ");

  for (i = 0; functions_array[i] != NULL; i++)
    {
      if (i != 0)
	g_string_append_printf(checking_message, ", ");

      g_string_append_printf(checking_message, "%s()", functions_array[i]);
    }

  if (libname)
    g_string_append_printf(checking_message, " in -l%s", libname);
  else
    g_string_append(checking_message, " in libc");

  jb_message_checking("%s", checking_message->str);
  g_string_free(checking_message, TRUE);

  program = g_string_new(NULL);

  /*
   * Quoting c.m4 in autoconf: "Override any GCC internal prototype to
   * avoid an error. Use char because int might match the return type
   * of a GCC builtin and then its argument prototype would still
   * apply."
   */
  for (i = 0; functions_array[i] != NULL; i++)
    g_string_append_printf(program, "char %s ();\n", functions_array[i]);

  g_string_append(program, "int main () {");

  for (i = 0; functions_array[i] != NULL; i++)
    g_string_append_printf(program, " %s();", functions_array[i]);

  g_string_append(program, " }\n");

  if (libname)
    libs = g_strdup_printf("-l%s", libname);

  result = jb_test_link_string(program->str, NULL, NULL, NULL, libs);

  g_string_free(program, TRUE);
  g_free(libs);

  jb_message_result_bool(result);

  return result;
}

gboolean
jb_check_packages (const char *group_name,
		   const char *varprefix,
		   const char *packages)
{
  char *quoted_packages;
  char *cflags = NULL;
  char *libs = NULL;
  char *error = NULL;
  gboolean status;

  g_return_val_if_fail(group_name != NULL, FALSE);
  g_return_val_if_fail(varprefix != NULL, FALSE);
  g_return_val_if_fail(packages != NULL, FALSE);
  g_return_val_if_fail(jb_feature_is_enabled(&jb_pkg_config_feature), FALSE);

  jb_require_program("pkg-config");

  jb_message_checking("for %s", group_name);

  quoted_packages = g_shell_quote(packages);

  status = jb_exec_expand(&cflags, NULL, "$pkg-config --cflags $packages",
			  "packages", quoted_packages,
			  NULL)
    && jb_exec_expand(&libs, NULL, "$pkg-config --libs $packages",
		      "packages", quoted_packages,
		      NULL);

  if (status)
    jb_variable_set_package_flags(varprefix, cflags, NULL, NULL, libs);
  else
    jb_exec_expand(NULL, &error, "$pkg-config --print-errors $packages",
		   "packages", quoted_packages,
		   NULL);

  g_free(quoted_packages);
  g_free(cflags);
  g_free(libs);

  jb_message_result_bool(status);

  if (error != NULL)
    {
      jb_warning("%s", error);
      g_free(error);
    }

  return status;
}

/*
 * If one or more options (bool variables) in the provided
 * NULL-terminated list are enabled, checks for the specified
 * packages. If the packages are not found, disable the provided
 * options.
 */
void
jb_check_packages_for_options (const char *group_name,
			       const char *varprefix,
			       const char *packages,
			       ...)
{
  GSList *options = NULL;
  va_list args;
  const char *option;
  gboolean needs_packages = FALSE;

  g_return_if_fail(group_name != NULL);
  g_return_if_fail(varprefix != NULL);
  g_return_if_fail(packages != NULL);
  g_return_if_fail(jb_feature_is_enabled(&jb_pkg_config_feature));

  va_start(args, packages);
  while ((option = va_arg(args, const char *)) != NULL)
    {
      options = g_slist_append(options, (gpointer) option);

      if (jb_variable_get_bool(option))
	needs_packages = TRUE;
    }
  va_end(args);

  if (needs_packages && ! jb_check_packages(group_name, varprefix, packages))
    {
      GSList *l;

      JB_LIST_FOREACH(l, options)
	{
	  option = l->data;

	  if (jb_variable_get_bool(option))
	    {
	      jb_warning("disabling option \"%s\" since %s was not found", option, group_name);
	      jb_variable_set_bool(option, FALSE);
	    }
	}
    }

  g_slist_free(options);
}

void
jb_require_packages (const char *group_name,
		     const char *varprefix,
		     const char *packages)
{
  g_return_if_fail(group_name != NULL);
  g_return_if_fail(varprefix != NULL);
  g_return_if_fail(packages != NULL);
  g_return_if_fail(jb_feature_is_enabled(&jb_pkg_config_feature));

  if (! jb_check_packages(group_name, varprefix, packages))
    jb_error("unable to find %s", group_name);
}

char *
jb_get_package_variable (const char *package, const char *variable)
{
  char *value;

  g_return_val_if_fail(package != NULL, NULL);
  g_return_val_if_fail(variable != NULL, NULL);
  g_return_val_if_fail(jb_feature_is_enabled(&jb_pkg_config_feature), FALSE);

  jb_require_program("pkg-config");

  if (! jb_exec_expand(&value, NULL, "$pkg-config --variable=$variable $package",
		       "variable", variable,
		       "package", package,
		       NULL))
    {
      g_free(value);
      value = NULL;
    }

  return value;
}

void
jb_check_cc_dependency_style (void)
{
  if (! jb_variable_get_bool("cc-dependency-tracking"))
    return; /* we don't need to know since we will not use it */

  jb_message_checking("the C compiler dependency style");

  /* make sure they do not exist beforehand */
  unlink("build/test.o");
  unlink("build/test.o.deps");

  if (jb_test_compile_string("#include <stdio.h>\n"
			     "int main () {}\n",
			     "-MT build/test.o -MD -MP -MF build/test.o.deps",
			     NULL)
      && g_file_test("build/test.o", G_FILE_TEST_IS_REGULAR)
      && g_file_test("build/test.o.deps", G_FILE_TEST_IS_REGULAR))
    jb_message_result_string("GCC");
  else
    {
      jb_message_result_string("unknown, disabling dependency tracking");
      jb_variable_set_bool("cc-dependency-tracking", FALSE);
    }
}

void
jb_check_glibc (void)
{
  gboolean result;

  jb_message_checking("for the GNU C library");
  result = jb_test_compile_string("#include <features.h>\n"
				  "int main() {\n"
				  "#ifndef __GLIBC__\n"
				  "#error \"glibc not found\"\n"
				  "#endif\n"
				  "}\n",
				  NULL,
				  NULL);
  jb_message_result_bool(result);

  jb_variable_set_bool("glibc", result);
}

static gboolean
check_reentrant_resolver_real (gboolean *result)
{
  const char *os;
  int freebsd_major;
  int freebsd_minor;

  if (! strcmp(jb_variable_get_string("host-kernel"), "linux"))
    {
      *result = TRUE;
      return TRUE;
    }

  os = jb_variable_get_string("host-os");

  if (sscanf(os, "freebsd%d.%d", &freebsd_major, &freebsd_minor) == 2)
    {
      /* FreeBSD >= 5.3 */
      *result = (freebsd_major == 5 && freebsd_minor >= 3) || freebsd_major >= 6;
      return TRUE;
    }

  if (g_str_has_prefix(os, "netbsd") || g_str_has_prefix(os, "openbsd"))
    {
      *result = FALSE;
      return TRUE;
    }

  return FALSE;			/* unknown */
}

void
jb_check_reentrant_dns_resolver (void)
{
  gboolean result;

  jb_check_host_system();

  jb_message_checking("if the DNS resolver is reentrant");

  if (check_reentrant_resolver_real(&result))
    {
      jb_message_result_bool(result);
      if (result)
	jb_compile_options_add_cppflags(jb_compile_options, "-DHAVE_REENTRANT_RESOLVER");
    }
  else
    jb_message_result_string("unknown, assuming it is not");
}
