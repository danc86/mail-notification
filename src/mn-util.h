/*
 * Mail Notification
 * Copyright (C) 2003-2008 Jean-Yves Lefort <jylefort@brutele.be>
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

#ifndef _MN_UTIL_H
#define _MN_UTIL_H

#include <stdarg.h>
#include <time.h>
#include <gtk/gtk.h>

void		mn_info				(const char	*format,
						 ...) G_GNUC_PRINTF(1, 2);

#define MN_LIST_FOREACH(var, head)		\
  for ((var) = (head);				\
       (var);					\
       (var) = (var)->next)

void		mn_g_list_free_deep_custom	(GList		*list,
						 GFunc		element_free_func,
						 gpointer	user_data);

GSList		*mn_g_slist_append_elements	(GSList		*list,
						 gpointer	data,
						 ...) G_GNUC_NULL_TERMINATED;
void		mn_g_slist_free_deep		(GSList		*list);
void		mn_g_slist_free_deep_custom	(GSList		*list,
						 GFunc		element_free_func,
						 gpointer	user_data);
void		mn_g_slist_clear		(GSList		**list);
void		mn_g_slist_clear_deep		(GSList		**list);
void		mn_g_slist_clear_deep_custom	(GSList		**list,
						 GFunc		element_free_func,
						 gpointer	user_data);
GSList		*mn_g_slist_delete_link_deep_custom (GSList	*list,
						     GSList	*link_,
						     GFunc	element_free_func,
						     gpointer	user_data);

GSList		*mn_g_str_slist_find		(GSList		*list,
						 const char	*str);

void		mn_g_object_list_free		(GList		*list);

GSList		*mn_g_object_slist_ref		(GSList		*list);
GSList		*mn_g_object_slist_copy		(GSList		*list);
void		mn_g_object_slist_free		(GSList		*list);
void		mn_g_object_slist_clear		(GSList		**list);

#define MN_QUEUE_FOREACH(var, queue)		\
  for ((var) = (queue)->head;			\
       (var);					\
       (var) = (var)->next)

gboolean	mn_str_isnumeric		(const char	*str);
gboolean	mn_str_ishex			(const char	*str);
char		*mn_strstr_span			(const char	*big,
						 const char	*little);

GdkPixbuf	*mn_pixbuf_new			(const char	*filename);

void		mn_container_create_interface	(GtkContainer	*container,
						 const char	*filename,
						 const char	*child_name,
						 const char	*callback_prefix,
						 ...) G_GNUC_NULL_TERMINATED;

GtkWindow	*mn_widget_get_parent_window	(GtkWidget	*widget);

void mn_file_chooser_dialog_allow_select_folder (GtkFileChooserDialog *dialog,
						 int accept_id);

void		mn_setup_dnd			(GtkWidget	*widget);

typedef enum
{
  MN_GNOME_COPIED_FILES_CUT,
  MN_GNOME_COPIED_FILES_COPY
} MNGnomeCopiedFilesType;

gboolean	mn_parse_gnome_copied_files	(const char	*gnome_copied_files,
						 MNGnomeCopiedFilesType *type,
						 GSList		**uri_list);

void		mn_show_help			(GtkWindow	*parent,
						 const char	*link_id);
void		mn_open_link			(GtkWindow	*parent,
						 const char	*url);

void		mn_thread_create		(GThreadFunc	func,
						 gpointer	data);

GtkWidget	*mn_menu_shell_append		(GtkMenuShell	*shell,
						 const char	*stock_id,
						 const char	*mnemonic);
GtkWidget	*mn_menu_shell_prepend		(GtkMenuShell	*shell,
						 const char	*stock_id,
						 const char	*mnemonic);

void		mn_show_error_dialog		(GtkWindow	*parent,
						 const char	*primary,
						 const char	*format,
						 ...) G_GNUC_PRINTF(3, 4);
void		mn_show_error_dialog_with_markup (GtkWindow	*parent,
						  const char	*primary,
						  const char	*format,
						  ...) G_GNUC_PRINTF(3, 4);
void		mn_show_invalid_uri_dialog	(GtkWindow	*parent,
						 const char	*primary,
						 const char	*invalid_uri);
void		mn_show_invalid_uri_list_dialog	(GtkWindow	*parent,
						 const char	*primary,
						 GSList		*invalid_uri_list);
void		mn_show_fatal_error_dialog	(GtkWindow	*parent,
						 const char	*format,
						 ...) G_GNUC_PRINTF(2, 3) G_GNUC_NORETURN;

typedef enum
{
  MN_DIALOG_BLOCKING	= 1 << 0,
  MN_DIALOG_MARKUP	= 1 << 1
} MNDialogFlags;

GtkWidget	*mn_alert_dialog_new		(GtkWindow	*parent,
						 GtkMessageType	type,
						 MNDialogFlags	flags,
						 const char	*primary,
						 const char	*secondary);

#define MN_SECS(n)		(n)
#define MN_MINS(n)		((n) * 60)
#define MN_HOURS(n)		((n) * 3600)
#define MN_DAYS(n)		((n) * 86400)

time_t		mn_time				(void);
char		*mn_strftime			(const char	*format,
						 const struct tm *timeptr);
char		*mn_format_past_time		(time_t		past_time,
						 time_t		now);
char		*mn_format_seconds		(int		seconds);

void		mn_g_object_null_unref		(gpointer	object);
gpointer	mn_g_object_connect		(gpointer	object,
						 gpointer	instance,
						 const char	*signal_spec,
						 ...) G_GNUC_NULL_TERMINATED;
gpointer	mn_g_object_clone		(gpointer	object,
						 const char	*property_name,
						 ...) G_GNUC_NULL_TERMINATED;

#define MN_STRDUP_VPRINTF(result, format) \
  G_STMT_START{							\
    va_list _mn_strdup_vprintf_args;				\
								\
    va_start(_mn_strdup_vprintf_args, format);			\
    result = g_strdup_vprintf(format, _mn_strdup_vprintf_args);	\
    va_end(_mn_strdup_vprintf_args);				\
  }G_STMT_END

int		mn_utf8_strcasecmp		(const char	*s1,
						 const char	*s2);
gboolean	mn_utf8_str_case_has_suffix	(const char	*str,
						 const char	*suffix);
char		*mn_utf8_escape			(const char	*str);

int		mn_dialog_run_nonmodal		(GtkDialog	*dialog);

void		mn_source_clear			(unsigned int	*tag);

gboolean	mn_ascii_str_case_has_prefix	(const char	*str,
						 const char	*prefix);
char		*mn_ascii_strcasestr		(const char	*big,
						 const char	*little);
char		*mn_ascii_strcasestr_span	(const char	*big,
						 const char	*little);

typedef gboolean (*MNSubstCommandFunction)	(const char	*name,
						 char		**value,
						 gpointer	data);
char		*mn_subst_command		(const char	*command,
						 MNSubstCommandFunction subst,
						 gpointer	data,
						 GError		**err);

void		mn_execute_command		(const char	*command);
void		mn_execute_command_in_terminal	(const char	*command);

char		*mn_shell_quote_safe		(const char	*unquoted_string);

GtkWidget	*mn_hig_section_new		(const char	*title,
						 GtkWidget	**label,
						 GtkWidget	**alignment);
GtkWidget	*mn_hig_section_new_with_box	(const char	*title,
						 GtkWidget	**label,
						 GtkWidget	**vbox);

char		*mn_g_value_to_string		(const GValue	*value);
gboolean	mn_g_value_from_string		(GValue		*value,
						 const char	*str);

void		mn_window_present_from_event	(GtkWindow	*window);

void		mn_add_weak_pointer		(gpointer	object_location);
void		mn_remove_weak_pointer		(gpointer	object_location);

int		mn_strv_find			(char		**strv,
						 const char	*elem);

#define MN_ARRAY_FOREACH(var, array) \
  for ((var) = 0; (var) < (array)->len; (var)++)

GSList		*mn_g_ptr_array_to_slist	(GPtrArray	*array);
void		mn_g_ptr_array_free_deep_custom	(GPtrArray	*array,
						 GFunc		element_free_func,
						 gpointer	user_data);

void		mn_g_object_ptr_array_free	(GPtrArray	*array);

#define MN_TREE_MODEL_FOREACH(valid, iter, model)			\
  for ((valid) = gtk_tree_model_get_iter_first((model), (iter));	\
       (valid);								\
       (valid) = gtk_tree_model_iter_next((model), (iter)))

const char	*mn_enum_get_value_nick		(GType		type,
						 int		value);

#endif /* _MN_UTIL_H */
