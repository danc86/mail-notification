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

#ifndef _JB_UTIL_H
#define _JB_UTIL_H

#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define JB_SEPARATOR "==============================================================================="

#define JB_MODE_FORMAT	"0%03o"

void jb_set_log_file (const char *filename);
void jb_log (const char *format, ...) G_GNUC_PRINTF(1, 2);

void jb_message (const char *format, ...) G_GNUC_PRINTF(1, 2);
void jb_message_expand (const char *str, ...) G_GNUC_NULL_TERMINATED;

void jb_message_action (const char *format, ...) G_GNUC_PRINTF(1, 2);

void jb_message_checking (const char *format, ...) G_GNUC_PRINTF(1, 2);

void jb_message_result_bool (gboolean result);
void jb_message_result_string (const char *result);
void jb_message_result_string_format (const char *format, ...) G_GNUC_PRINTF(1, 2);

void jb_warning (const char *format, ...) G_GNUC_PRINTF(1, 2);
void jb_warning_expand (const char *str, ...) G_GNUC_NULL_TERMINATED;

/*
 * Do not use this function to report errors that are caused by an
 * invalid jb.c. They are programming errors, use g_error().
 */
void jb_error (const char *format, ...) G_GNUC_PRINTF(1, 2) G_GNUC_NORETURN;
void jb_error_expand (const char *str, ...) G_GNUC_NULL_TERMINATED G_GNUC_NORETURN;

#define JB_LIST_FOREACH(var, head)		\
  for ((var) = (head);				\
       (var) != NULL;				\
       (var) = (var)->next)

void jb_g_slist_free_deep (GSList *list);
void jb_g_slist_free_deep_custom (GSList *list,
				  GFunc element_free_func,
				  gpointer user_data);

#define JB_STRDUP_VPRINTF(result, format) \
  G_STMT_START{							\
    va_list _jb_strdup_vprintf_args;				\
								\
    va_start(_jb_strdup_vprintf_args, format);			\
    result = g_strdup_vprintf(format, _jb_strdup_vprintf_args);	\
    va_end(_jb_strdup_vprintf_args);				\
  }G_STMT_END

char *jb_strdelimit (const char *str, const char *delimiters, char new_delimiter);

char *jb_strip_newline (const char *str);

char *jb_c_quote (const char *str);

char *jb_strip_extension (const char *filename);

char *jb_strip_chars (const char *str, const char *chars);

char *jb_utf8_escape (const char *str);

gboolean jb_parse_uint32 (const char *str, int base, guint32 *value, GError **err);
gboolean jb_parse_uint64 (const char *str, int base, guint64 *value, GError **err);

gboolean jb_write_file (const char *filename, const char *contents, GError **err);
void jb_write_file_or_exit (const char *filename, const char *contents);

char *jb_read_file (const char *filename, GError **err);
char *jb_read_file_or_exit (const char *filename);

GSList *jb_match_files (const char *pattern);

void jb_chdir (const char *path);

void jb_mkdir (const char *pathname);
void jb_mkdir_of_file (const char *filename);

void jb_rename (const char *oldpath, const char *newpath);

void jb_chmod (const char *path, mode_t mode);

gboolean jb_fchown_by_name (int fd,
			    const char *owner,
			    const char *group,
			    GError **err);

void jb_rmtree (const char *dir);

void jb_subst (const char *infile,
	       const char *outfile,
	       GHashTable *variables);

gboolean jb_exec (char **standard_output,
		  char **standard_error,
		  const char *format,
		  ...) G_GNUC_PRINTF(3, 4);
gboolean jb_exec_expand (char **standard_output,
			 char **standard_error,
			 const char *str,
			 ...) G_GNUC_NULL_TERMINATED;

typedef GHashTable JBStringHashSet;

JBStringHashSet *jb_string_hash_set_new (void);
gboolean jb_string_hash_set_add (JBStringHashSet *set, const char *value);
gboolean jb_string_hash_set_contains (JBStringHashSet *set, const char *value);

gboolean jb_is_uptodate (const char *dst, const char *src);
gboolean jb_is_uptodate_list (const char *dst, GSList *src_list);
gboolean jb_is_uptodate_list_list (GSList *dst_list, GSList *src_list);

char *jb_string_list_join (GSList *list, const char *separator);

#endif /* _JB_UTIL_H */
