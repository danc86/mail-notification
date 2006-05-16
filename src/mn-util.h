/* 
 * Copyright (C) 2003-2006 Jean-Yves Lefort <jylefort@brutele.be>
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

#ifndef _MN_UTIL_H
#define _MN_UTIL_H

#include "config.h"
#include <stdarg.h>
#include <time.h>
#include <gtk/gtk.h>

#define MN_LIST_FOREACH(var, head)		\
for ((var) = (head);				\
     (var);					\
     (var) = (var)->next)

#define MN_QUEUE_FOREACH(var, queue)		\
for ((var) = (queue)->head;			\
     (var);					\
     (var) = (var)->next)

void		mn_info				(const char	*format,
						 ...) G_GNUC_PRINTF(1, 2);

GSList		*mn_g_slist_append_elements	(GSList		*list,
						 gpointer	data,
						 ...);
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

GSList		*mn_g_object_slist_ref		(GSList		*list);
GSList		*mn_g_object_slist_copy		(GSList		*list);
void		mn_g_object_slist_free		(GSList		*list);
void		mn_g_object_slist_clear		(GSList		**list);
GSList		*mn_g_object_slist_delete_link	(GSList		*list,
						 GSList		*link_);

void		mn_g_queue_free_deep_custom	(GQueue		*queue,
						 GFunc		element_free_func,
						 gpointer	user_data);

gboolean	mn_str_isnumeric		(const char	*str);
char		*mn_strstr_span			(const char	*big,
						 const char	*little);

GdkPixbuf	*mn_pixbuf_new			(const char	*filename);

void		mn_container_create_interface	(GtkContainer	*container,
						 const char	*filename,
						 const char	*child_name,
						 const char	*callback_prefix,
						 ...);

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

void		mn_display_help			(GtkWindow	*parent,
						 const char	*link_id);
void		mn_thread_create		(GThreadFunc	func,
						 gpointer	data);

GtkWidget	*mn_menu_shell_append		(GtkMenuShell	*shell,
						 const char	*stock_id,
						 const char	*mnemonic);
GtkWidget	*mn_menu_shell_prepend		(GtkMenuShell	*shell,
						 const char	*stock_id,
						 const char	*mnemonic);

void		mn_error_dialog			(GtkWindow	*parent,
						 const char	*primary,
						 const char	*format,
						 ...) G_GNUC_PRINTF(3, 4);
void		mn_invalid_uri_dialog		(GtkWindow	*parent,
						 const char	*primary,
						 const char	*invalid_uri);
void		mn_invalid_uri_list_dialog	(GtkWindow	*parent,
						 const char	*primary,
						 const GSList	*invalid_uri_list);
void		mn_fatal_error_dialog		(GtkWindow	*parent,
						 const char	*format,
						 ...) G_GNUC_PRINTF(2, 3) G_GNUC_NORETURN;

GtkWidget	*mn_alert_dialog_new		(GtkWindow	*parent,
						 GtkMessageType	type,
						 const char	*primary,
						 const char	*secondary);

time_t		mn_time				(void);

gpointer	mn_g_object_connect		(gpointer	object,
						 gpointer	instance,
						 const char	*signal_spec,
						 ...);

void		mn_gtk_object_ref_and_sink	(GtkObject	*object);

int		mn_utf8_strcmp			(const char	*s1,
						 const char	*s2);
int		mn_utf8_strcasecmp		(const char	*s1,
						 const char	*s2);
char		*mn_utf8_escape			(const char	*str);

int		mn_dialog_run_nonmodal		(GtkDialog	*dialog);

void		mn_source_clear			(unsigned int	*tag);

gboolean	mn_ascii_str_case_has_prefix	(const char	*str,
						 const char	*prefix);

char		*mn_format_past_time		(time_t		past_time,
						 time_t		now);

void		mn_gdk_threads_enter		(void);
void		mn_gdk_threads_leave		(void);
void		mn_g_static_mutex_lock		(GStaticMutex *mutex);
void		mn_g_static_mutex_unlock	(GStaticMutex *mutex);

#endif /* _MN_UTIL_H */
