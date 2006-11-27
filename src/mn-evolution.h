/* 
 * Mail Notification
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _MN_EVOLUTION_H
#define _MN_EVOLUTION_H

#define MN_EVOLUTION_EVENT_PREFIX		"GNOME/MailNotification/Evolution"

#define MN_EVOLUTION_GLUE_IID			"OAFIID:GNOME_MailNotification_Evolution_Glue"
#define MN_EVOLUTION_GLUE_EVENT_PREFIX		MN_EVOLUTION_EVENT_PREFIX "/Glue"
#define MN_EVOLUTION_GLUE_EVENT_FOLDER_CHANGED	"folder-changed"
#define MN_EVOLUTION_GLUE_EVENT_MESSAGE_READING	"message-reading"

#define MN_EVOLUTION_FOLDER_TREE_CONTROL_IID			"OAFIID:GNOME_MailNotification_Evolution_FolderTree_Control"
#define MN_EVOLUTION_FOLDER_TREE_CONTROL_EVENT_PREFIX		MN_EVOLUTION_EVENT_PREFIX "/FolderTreeControl"
#define MN_EVOLUTION_FOLDER_TREE_CONTROL_EVENT_ACTIVATED	"activated"

#endif /* _MN_EVOLUTION_H */
