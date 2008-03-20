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
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <glob.h>
#include "jb-util.h"
#include "jb-variable.h"
#include "jb-main.h"

static char *log_file = NULL;

static gboolean printing_action = FALSE;

void
jb_set_log_file (const char *filename)
{
  g_return_if_fail(filename != NULL);

  g_free(log_file);
  log_file = g_strdup(filename);
}

void
jb_log (const char *format, ...)
{
  static gboolean logging = FALSE;
  static GIOChannel *log_channel = NULL;
  static char *current_log_file = NULL;
  char *message;
  char *with_nl;
  GError *err = NULL;
  gsize bytes_written;

  g_return_if_fail(format != NULL);
  g_return_if_fail(log_file != NULL);

  if (logging)
    return;

  logging = TRUE;

  if (log_channel != NULL)
    {
      if (strcmp(current_log_file, log_file))
	{
	  if (g_io_channel_shutdown(log_channel, TRUE, &err) != G_IO_STATUS_NORMAL)
	    jb_error("unable to write to %s: %s", current_log_file, err->message);

	  g_io_channel_unref(log_channel);
	  log_channel = NULL;

	  g_free(current_log_file);
	}
    }

  if (log_channel == NULL)
    {
      current_log_file = g_strdup(log_file);

      log_channel = g_io_channel_new_file(log_file, "w", &err);
      if (log_channel == NULL)
	jb_error("unable to open %s for writing: %s", log_file, err->message);
    }

  JB_STRDUP_VPRINTF(message, format);

  with_nl = g_strdup_printf("%s\n", message);
  g_free(message);

  if (g_io_channel_write_chars(log_channel, with_nl, -1, &bytes_written, &err) != G_IO_STATUS_NORMAL
      || g_io_channel_flush(log_channel, &err) != G_IO_STATUS_NORMAL)
    jb_error("unable to write to %s: %s", log_file, err->message);

  g_free(with_nl);

  logging = FALSE;
}

void
jb_message (const char *format, ...)
{
  char *message;

  g_return_if_fail(format != NULL);
  g_return_if_fail(! printing_action);

  JB_STRDUP_VPRINTF(message, format);

  g_print("%s\n", message);
  jb_log("%s", message);

  g_free(message);
}

void
jb_message_expand (const char *str, ...)
{
  va_list args;
  char *message;

  g_return_if_fail(str != NULL);

  va_start(args, str);
  message = jb_variable_expandv(str, args);
  va_end(args);

  jb_message("%s", message);
  g_free(message);
}

void
jb_message_action (const char *format, ...)
{
  char *message;

  g_return_if_fail(format != NULL);
  g_return_if_fail(! printing_action);

  JB_STRDUP_VPRINTF(message, format);

  g_print("%s...", message);
  jb_log("%s", message);

  g_free(message);

  printing_action = TRUE;
}

void
jb_message_checking (const char *format, ...)
{
  char *message;

  g_return_if_fail(format != NULL);
  g_return_if_fail(! printing_action);

  JB_STRDUP_VPRINTF(message, format);
  jb_message_action("checking %s", message);
  g_free(message);
}

void
jb_message_result_bool (gboolean result)
{
  g_return_if_fail(printing_action);

  jb_message_result_string(result ? "yes" : "no");
}

void
jb_message_result_string (const char *result)
{
  g_return_if_fail(result != NULL);
  g_return_if_fail(printing_action);

  g_print(" %s\n", result);
  jb_log("result: %s", result);
  jb_log("");

  printing_action = FALSE;
}

static void
finish_printing_action (void)
{
  if (printing_action)
    {
      g_print("\n");
      printing_action = FALSE;
    }
}

void
jb_message_result_string_format (const char *format, ...)
{
  char *message;

  g_return_if_fail(format != NULL);
  g_return_if_fail(printing_action);

  JB_STRDUP_VPRINTF(message, format);
  jb_message_result_string(message);
  g_free(message);
}

static void
print_warning_or_error (const char *prefix, const char *format, va_list args)
{
  char *message;
  char **lines;
  int i;

  /*
   * We allow to interrupt an action print, in case the caller is a
   * library function which does not know that an action is in
   * progress.
   */
  finish_printing_action();

  message = g_strdup_vprintf(format, args);
  lines = g_strsplit(message, "\n", 0);
  g_free(message);

  for (i = 0; lines[i] != NULL; i++)
    {
      const char *line = lines[i];

      g_printerr("%s: %s\n", prefix, line);
      jb_log("%s: %s", prefix, line);
    }

  g_strfreev(lines);
}

void
jb_warning (const char *format, ...)
{
  va_list args;

  g_return_if_fail(format != NULL);

  va_start(args, format);
  print_warning_or_error("WARNING", format, args);
  va_end(args);
}

void
jb_warning_expand (const char *str, ...)
{
  va_list args;
  char *message;

  g_return_if_fail(str != NULL);

  va_start(args, str);
  message = jb_variable_expandv(str, args);
  va_end(args);

  jb_warning("%s", message);
  g_free(message);
}

void
jb_error (const char *format, ...)
{
  va_list args;

  g_assert(format != NULL);

  va_start(args, format);
  print_warning_or_error("ERROR", format, args);
  va_end(args);

  exit(1);
}

void
jb_error_expand (const char *str, ...)
{
  va_list args;
  char *message;

  g_assert(str != NULL);

  va_start(args, str);
  message = jb_variable_expandv(str, args);
  va_end(args);

  jb_error("%s", message);
  g_free(message);
}

void
jb_g_slist_free_deep (GSList *list)
{
  jb_g_slist_free_deep_custom(list, (GFunc) g_free, NULL);
}

void
jb_g_slist_free_deep_custom (GSList *list,
			     GFunc element_free_func,
			     gpointer user_data)
{
  g_slist_foreach(list, element_free_func, user_data);
  g_slist_free(list);
}

char *
jb_strdelimit (const char *str, const char *delimiters, char new_delimiter)
{
  char *result;

  g_return_val_if_fail(str != NULL, NULL);
  g_return_val_if_fail(delimiters != NULL, NULL);

  result = g_strdup(str);
  g_strdelimit(result, delimiters, new_delimiter);

  return result;
}

char *
jb_strip_newline (const char *str)
{
  int len;

  g_return_val_if_fail(str != NULL, NULL);

  len = strlen(str);
  if (len > 0 && str[len - 1] == '\n')
    return g_strndup(str, len - 1);
  else
    return g_strdup(str);
}

char *
jb_c_quote (const char *str)
{
  GString *result;
  const char *p;

  g_return_val_if_fail(str != NULL, NULL);

  result = g_string_new("\"");

  for (p = str; *p != '\0'; p++)
    {
      char c = *p;

      switch (c)
	{
	case '\\':
	  g_string_append(result, "\\\\");
	  break;

	case '"':
	  g_string_append(result, "\\\"");
	  break;

	case '\r':
	  g_string_append(result, "\\r");
	  break;

	case '\n':
	  g_string_append(result, "\\n");
	  break;

	case '\t':
	  g_string_append(result, "\\t");
	  break;

	default:
	  g_string_append_c(result, c);
	  break;
	}
    }

  g_string_append_c(result, '"');

  return g_string_free(result, FALSE);
}

char *
jb_strip_extension (const char *filename)
{
  char *p;

  p = strrchr(filename, '.');
  if (p != NULL)
    return g_strndup(filename, p - filename);
  else
    return g_strdup(filename);
}

char *
jb_strip_chars (const char *str, const char *chars)
{
  const char *p;
  GString *result;

  g_return_val_if_fail(str != NULL, NULL);
  g_return_val_if_fail(chars != NULL, NULL);

  result = g_string_new(NULL);

  for (p = str; *p != '\0'; p++)
    {
      char c = *p;

      if (strchr(chars, c) == NULL)
	g_string_append_c(result, c);
    }

  return g_string_free(result, FALSE);
}

char *
jb_utf8_escape (const char *str)
{
  GString *escaped;

  g_return_val_if_fail(str != NULL, NULL);

  escaped = g_string_new(NULL);

  while (*str != '\0')
    {
      gunichar c;

      c = g_utf8_get_char_validated(str, -1);
      if (c != (gunichar) -2 && c != (gunichar) -1)
	{
	  g_string_append_unichar(escaped, c);
	  str = g_utf8_next_char(str);
	}
      else
	{
	  g_string_append_printf(escaped, "\\x%02x", (unsigned int) (unsigned char) *str);
	  str++;
	}
    }

  return g_string_free(escaped, FALSE);
}

gboolean
jb_parse_uint32 (const char *str, int base, guint32 *value, GError **err)
{
  guint64 v;

  g_return_val_if_fail(str != NULL, FALSE);

  if (! jb_parse_uint64(str, base, &v, err))
    return FALSE;

  if (v > G_MAXUINT32)
    {
      g_set_error(err, 0, 0, "number out of range");
      return FALSE;
    }

  *value = v;
  return TRUE;
}

gboolean
jb_parse_uint64 (const char *str, int base, guint64 *value, GError **err)
{
  guint64 v;
  char *end;

  g_return_val_if_fail(str != NULL, FALSE);

  v = g_ascii_strtoull(str, &end, base);

  if (*end != '\0')
    {
      g_set_error(err, 0, 0, "invalid number");
      return FALSE;
    }

  if (v == G_MAXUINT64 && errno == ERANGE)
    {
      g_set_error(err, 0, 0, "number out of range");
      return FALSE;
    }

  *value = v;
  return TRUE;
}

gboolean
jb_write_file (const char *filename, const char *contents, GError **err)
{
  GIOChannel *channel;
  gsize bytes_written;
  gboolean status = FALSE;

  g_return_val_if_fail(filename != NULL, FALSE);
  g_return_val_if_fail(contents != NULL, FALSE);

  channel = g_io_channel_new_file(filename, "w", err);
  if (channel == NULL)
    return FALSE;

  if (g_io_channel_write_chars(channel, contents, -1, &bytes_written, err) == G_IO_STATUS_NORMAL)
    {
      if (g_io_channel_shutdown(channel, TRUE, err) == G_IO_STATUS_NORMAL)
	status = TRUE;
    }
  else
    g_io_channel_shutdown(channel, FALSE, NULL);

  g_io_channel_unref(channel);

  return status;
}

void
jb_write_file_or_exit (const char *filename, const char *contents)
{
  GError *err = NULL;

  if (! jb_write_file(filename, contents, &err))
    jb_error("cannot write %s: %s", filename, err->message);
}

char *
jb_read_file (const char *filename, GError **err)
{
  GIOChannel *channel;
  char *contents = NULL;
  gsize length;

  g_return_val_if_fail(filename != NULL, FALSE);

  channel = g_io_channel_new_file(filename, "r", err);
  if (channel == NULL)
    return NULL;

  g_io_channel_read_to_end(channel, &contents, &length, err);

  g_io_channel_shutdown(channel, FALSE, NULL);
  g_io_channel_unref(channel);

  return contents;
}

char *
jb_read_file_or_exit (const char *filename)
{
  GError *err = NULL;
  char *contents;

  contents = jb_read_file(filename, &err);
  if (contents == NULL)
    jb_error("cannot read %s: %s", filename, err->message);

  return contents;
}

GSList *
jb_match_files (const char *pattern)
{
  glob_t glob_result;

  g_return_val_if_fail(pattern != NULL, NULL);

  if (glob(pattern, 0, NULL, &glob_result) == 0)
    {
      int i;
      GSList *files = NULL;

      for (i = 0; i < glob_result.gl_pathc; i++)
	files = g_slist_append(files, g_strdup(glob_result.gl_pathv[i]));

      globfree(&glob_result);

      return files;
    }
  else
    return NULL;
}

void
jb_chdir (const char *path)
{
  g_return_if_fail(path != NULL);

  if (chdir(path) < 0)
    jb_error("cannot change directory to %s: %s", path, g_strerror(errno));
}

void
jb_mkdir (const char *pathname)
{
  g_return_if_fail(pathname != NULL);

  if (g_mkdir_with_parents(pathname, 0755) < 0)
    jb_error("cannot create directory %s: %s", pathname, g_strerror(errno));
}

void
jb_mkdir_of_file (const char *filename)
{
  char *dir;

  g_return_if_fail(filename != NULL);

  dir = g_path_get_dirname(filename);
  jb_mkdir(dir);
  g_free(dir);
}

void
jb_rename (const char *oldpath, const char *newpath)
{
  g_return_if_fail(oldpath != NULL);
  g_return_if_fail(newpath != NULL);

  if (rename(oldpath, newpath) < 0)
    jb_error("cannot rename %s to %s: %s", oldpath, newpath, g_strerror(errno));
}

void
jb_chmod (const char *path, mode_t mode)
{
  g_return_if_fail(path != NULL);

  if (chmod(path, mode) < 0)
    jb_error("cannot chmod %s to " JB_MODE_FORMAT ": %s", path, (unsigned int) mode, g_strerror(errno));
}

gboolean
jb_fchown_by_name (int fd,
		   const char *owner,
		   const char *group,
		   GError **err)
{
  uid_t uid = -1;
  gid_t gid = -1;

  g_return_val_if_fail(fd >= 0, FALSE);
  g_return_val_if_fail(owner != NULL || group != NULL, FALSE);

  if (owner != NULL)
    {
      struct passwd *info;

      info = getpwnam(owner);
      if (info == NULL)
	{
	  g_set_error(err, 0, 0, "unknown user \"%s\"", owner);
	  return FALSE;
	}

      uid = info->pw_uid;
    }

  if (group != NULL)
    {
      struct group *info;

      info = getgrnam(group);
      if (group == NULL)
	{
	  g_set_error(err, 0, 0, "unknown group \"%s\"", group);
	  return FALSE;
	}

      gid = info->gr_gid;
    }

  if (fchown(fd, uid, gid) < 0)
    {
      g_set_error(err, 0, 0, "%s", g_strerror(errno));
      return FALSE;
    }

  return TRUE;
}

void
jb_rmtree (const char *dir)
{
  /*
   * Be paranoid and refuse abberant inputs. These tests are not
   * assertions to make sure that they won't be compiled out.
   */
  if (dir == NULL)
    g_error("dir is NULL");
  if (g_path_is_absolute(dir))
    g_error("refusing to rmtree an absolute path");

  jb_exec(NULL, NULL, "rm -rf %s", dir);
}

static char *
subst_real (const char *str, GHashTable *variables)
{
  GString *result;
  const char *p;

  result = g_string_new(NULL);

  for (p = str; *p != '\0';)
    {
      char c = *p;

      if (c == '@')
	{
	  const char *start;
	  char *end;

	  start = p + 1;
	  end = strpbrk(start, "@\n");

	  if (end != NULL && *end == '@')
	    {
	      char *name;
	      const char *value;

	      name = g_strndup(start, end - start);
	      value = g_hash_table_lookup(variables, name);
	      g_free(name);

	      if (value != NULL)
		{
		  g_string_append(result, value);
		  p = end + 1;
		  continue;
		}
	    }
	}

      g_string_append_c(result, c);
      p++;
    }

  return g_string_free(result, FALSE);
}

void
jb_subst (const char *infile,
	  const char *outfile,
	  GHashTable *variables)
{
  char *contents;
  char *result;
  char *tmp_outfile;

  g_return_if_fail(infile != NULL);
  g_return_if_fail(outfile != NULL);
  g_return_if_fail(variables != NULL);

  contents = jb_read_file_or_exit(infile);
  result = subst_real(contents, variables);
  g_free(contents);

  tmp_outfile = g_strdup_printf("%s.tmp", outfile);
  jb_write_file_or_exit(tmp_outfile, result);
  g_free(result);

  jb_rename(tmp_outfile, outfile);
  g_free(tmp_outfile);
}

static char *
convert_process_output (const char *output)
{
  char *utf8;
  char *no_nl;

  if (g_utf8_validate(output, -1, NULL))
    utf8 = g_strdup(output);
  else
    {
      utf8 = g_locale_to_utf8(output, -1, NULL, NULL, NULL);
      if (utf8 == NULL)
	utf8 = jb_utf8_escape(output);
    }

  no_nl = jb_strip_newline(utf8);
  g_free(utf8);

  return no_nl;
}

/*
 * Returns TRUE if the command exited with status 0.
 *
 * The trailing newline of @standard_output and @standard_error will
 * be stripped.
 *
 * @standard_output and @standard_error will be set even if FALSE is
 * returned, since a command can produce an output even if it exits
 * with a non-zero status.
 */
gboolean
jb_exec (char **standard_output,
	 char **standard_error,
	 const char *format,
	 ...)
{
  char *command;
  int command_status;
  gboolean status = FALSE;
  char *_stdout;
  char *_stderr;
  char *converted_stdout = NULL;
  char *converted_stderr = NULL;
  char *shell_argv[4];
  GError *err = NULL;

  g_return_val_if_fail(format != NULL, FALSE);

  JB_STRDUP_VPRINTF(command, format);

  shell_argv[0] = "/bin/sh";
  shell_argv[1] = "-c";
  shell_argv[2] = command;
  shell_argv[3] = NULL;

  if (g_spawn_sync(NULL,
		   shell_argv,
		   NULL,
		   G_SPAWN_SEARCH_PATH,
		   NULL,
		   NULL,
		   &_stdout,
		   &_stderr,
		   &command_status,
		   &err))
    {
      converted_stdout = convert_process_output(_stdout);
      g_free(_stdout);

      converted_stderr = convert_process_output(_stderr);
      g_free(_stderr);

      if (WIFEXITED(command_status))
	{
	  int exit_status;

	  exit_status = WEXITSTATUS(command_status);
	  if (exit_status == 0)
	    {
	      jb_log("command \"%s\" succeeded", command);
	      status = TRUE;
	    }
	  else
	    jb_log("command \"%s\" failed with status %i", command, exit_status);

	  if (*converted_stdout != '\0')
	    {
	      jb_log("standard output:");
	      jb_log("%s", converted_stdout);
	      jb_log(JB_SEPARATOR);
	    }
	  if (*converted_stderr != '\0')
	    {
	      jb_log("standard error output:");
	      jb_log("%s", converted_stderr);
	      jb_log(JB_SEPARATOR);
	    }
	}
      else
	jb_log("command exited abnormally");
    }
  else
    /* fatal error: it should not happend since we exec the shell */
    jb_error("cannot execute command \"%s\": %s", command, err->message);

  g_free(command);

  if (standard_output)
    *standard_output = converted_stdout;
  else
    g_free(converted_stdout);

  if (standard_error)
    *standard_error = converted_stderr;
  else
    g_free(converted_stderr);

  return status;
}

gboolean
jb_exec_expand (char **standard_output,
		char **standard_error,
		const char *str,
		...)
{
  va_list args;
  char *command;
  gboolean result;

  g_return_val_if_fail(str != NULL, FALSE);

  va_start(args, str);
  command = jb_variable_expandv(str, args);
  va_end(args);

  result = jb_exec(standard_output, standard_error, "%s", command);
  g_free(command);

  return result;
}

JBStringHashSet *
jb_string_hash_set_new (void)
{
  return g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
}

gboolean
jb_string_hash_set_add (JBStringHashSet *set, const char *value)
{
  g_return_val_if_fail(set != NULL, FALSE);
  g_return_val_if_fail(value != NULL, FALSE);

  if (g_hash_table_lookup(set, value) != NULL)
    return FALSE;

  g_hash_table_insert(set, g_strdup(value), GINT_TO_POINTER(TRUE));
  return TRUE;
}

gboolean
jb_string_hash_set_contains (JBStringHashSet *set, const char *value)
{
  g_return_val_if_fail(set != NULL, FALSE);
  g_return_val_if_fail(value != NULL, FALSE);

  return g_hash_table_lookup_extended(set, value, NULL, NULL);
}

gboolean
jb_is_uptodate (const char *dst, const char *src)
{
  struct stat dst_sb;
  struct stat src_sb;

  g_return_val_if_fail(dst != NULL, FALSE);
  g_return_val_if_fail(src != NULL, FALSE);

  if (stat(dst, &dst_sb) < 0)
    return FALSE;

  if (stat(src, &src_sb) < 0)
    g_error("%s (dependency of %s) does not exist", src, dst);

  return dst_sb.st_mtime >= src_sb.st_mtime;
}

gboolean
jb_is_uptodate_list (const char *dst, GSList *src_list)
{
  struct stat dst_sb;
  GSList *l;

  g_return_val_if_fail(dst != NULL, FALSE);
  g_return_val_if_fail(src_list != NULL, FALSE);

  if (stat(dst, &dst_sb) < 0)
    return FALSE;

  JB_LIST_FOREACH(l, src_list)
    {
      const char *src = l->data;
      struct stat src_sb;

      if (stat(src, &src_sb) < 0)
	g_error("%s (dependency of %s) does not exist", src, dst);

      if (dst_sb.st_mtime < src_sb.st_mtime)
	return FALSE;
    }

  return TRUE;
}

gboolean
jb_is_uptodate_list_list (GSList *dst_list, GSList *src_list)
{
  GArray *dst_mtimes;
  GSList *l;
  gboolean result = TRUE;
  const char *filename;
  struct stat sb;

  dst_mtimes = g_array_new(FALSE, FALSE, sizeof(time_t));

  JB_LIST_FOREACH(l, dst_list)
    {
      filename = l->data;

      if (stat(filename, &sb) < 0)
	{
	  result = FALSE;
	  goto end;
	}

      g_array_append_val(dst_mtimes, sb.st_mtime);
    }

  JB_LIST_FOREACH(l, src_list)
    {
      int i;

      filename = l->data;

      if (stat(filename, &sb) < 0)
	g_error("%s (dependency of %s) does not exist", filename, jb_string_list_join(dst_list, " "));

      for (i = 0; i < dst_mtimes->len; i++)
	{
	  time_t dst_mtime = g_array_index(dst_mtimes, time_t, i);

	  if (dst_mtime < sb.st_mtime)
	    {
	      result = FALSE;
	      goto end;
	    }
	}
    }

 end:
  g_array_free(dst_mtimes, TRUE);
  return result;
}

char *
jb_string_list_join (GSList *list, const char *separator)
{
  GString *result;
  GSList *l;

  g_return_val_if_fail(separator != NULL, NULL);

  result = g_string_new(NULL);

  JB_LIST_FOREACH(l, list)
    {
      const char *str = l->data;

      g_string_append(result, str);

      if (l->next != NULL)
	g_string_append(result, separator);
    }

  return g_string_free(result, FALSE);
}
