/* 
 * Copyright (c) 2003, 2004 Jean-Yves Lefort <jylefort@brutele.be>
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

void		mn_info				(const char	*format,
						 ...) G_GNUC_PRINTF(1, 2);

GSList		*mn_g_slist_delete_link_deep	(GSList		*list,
						 GSList		*link_);
GSList		*mn_g_slist_delete_link_deep_custom (GSList	*list,
						     GSList	*link_,
						     GFunc	element_free_func,
						     gpointer	user_data);

GSList		*mn_g_str_slist_find		(GSList		*list,
						 const char	*str);

GSList		*mn_g_object_slist_ref		(GSList		*list);
GSList		*mn_g_object_slist_copy		(GSList		*list);
void		mn_g_object_slist_free		(GSList		*list);
GSList		*mn_g_object_slist_delete_link	(GSList		*list,
						 GSList		*link_);

gboolean	mn_str_isnumeric		(const char	*str);

GdkPixbuf	*mn_pixbuf_new			(const char	*filename);
void		mn_create_interface		(const char	*name,
						 ...);

void mn_file_chooser_dialog_allow_select_folder (GtkFileChooserDialog *dialog,
						 int accept_id);

void		mn_setup_dnd			(GtkWidget	*widget);

typedef enum
{
  MN_GNOME_COPIED_FILES_CUT,
  MN_GNOME_COPIED_FILES_COPY
} MNGnomeCopiedFilesType;

char		*mn_build_gnome_copied_files	(MNGnomeCopiedFilesType type,
						 GSList		*uri_list);
gboolean	mn_parse_gnome_copied_files	(const char	*gnome_copied_files,
						 MNGnomeCopiedFilesType *type,
						 GSList		**uri_list);

void		mn_display_help			(GtkWindow	*parent,
						 const char	*link_id);
void		mn_thread_create		(GThreadFunc	func,
						 gpointer	data);

GtkTooltips	*mn_gtk_tooltips_new		(void);
void		mn_gtk_tooltips_set_tips	(GtkTooltips	*tooltips,
						 ...);

#define mn_gtk_tooltips_set_tip(tooltips, widget, tip) \
  gtk_tooltips_set_tip((tooltips), (widget), (tip), NULL)

GtkWidget	*mn_menu_shell_append		(GtkMenuShell	*shell,
						 const char	*stock_id,
						 const char	*mnemonic);
GtkWidget	*mn_menu_shell_prepend		(GtkMenuShell	*shell,
						 const char	*stock_id,
						 const char	*mnemonic);

void		mn_error_dialog			(GtkWindow	*parent,
						 const char	*not_again_key,
						 const char	*help_link_id,
						 const char	*primary,
						 const char	*format,
						 ...) G_GNUC_PRINTF(5, 6);
void		mn_fatal_error_dialog		(GtkWindow	*parent,
						 const char	*format,
						 ...) G_GNUC_PRINTF(2, 3);

time_t		mn_time				(void);

typedef enum
{
  MN_POSITION_TOP_LEFT,
  MN_POSITION_TOP_RIGHT,
  MN_POSITION_BOTTOM_LEFT,
  MN_POSITION_BOTTOM_RIGHT
} MNPosition;

GType		mn_position_get_type		(void);
#define MN_TYPE_POSITION (mn_position_get_type())

typedef enum
{
  MN_ACTION_DISPLAY_MAIL_SUMMARY,
  MN_ACTION_LAUNCH_MAIL_READER
} MNAction;

GType		mn_action_get_type		(void);
#define MN_TYPE_ACTION (mn_action_get_type())

gpointer	mn_g_object_connect		(gpointer	object,
						 gpointer	instance,
						 const char	*signal_spec,
						 ...);

void		mn_execute_command		(const char	*conf_key);

#define MN_STRING_TO_POINTER(str)	((*str) ? (str) : NULL)
#define MN_POINTER_TO_STRING(ptr)	((ptr) ? (ptr) : "")

#endif /* _MN_UTIL_H */
