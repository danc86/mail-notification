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

#ifndef _MN_MAILBOX_H
#define _MN_MAILBOX_H

#include <stdarg.h>
#include <glib-object.h>
#include <libgnomevfs/gnome-vfs.h>

#define MN_TYPE_MAILBOX			(mn_mailbox_get_type())
#define MN_MAILBOX(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), MN_TYPE_MAILBOX, MNMailbox))
#define MN_MAILBOX_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), MN_TYPE_MAILBOX, MNMailboxClass))
#define MN_IS_MAILBOX(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), MN_TYPE_MAILBOX))
#define MN_IS_MAILBOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), MN_TYPE_MAILBOX))
#define MN_MAILBOX_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), MN_TYPE_MAILBOX, MNMailboxClass))

typedef struct _MNMailboxPrivate MNMailboxPrivate;

typedef struct
{
  GObject		object;

  MNMailboxPrivate	*priv;
} MNMailbox;

typedef void (MNMailboxIsCallback) (gboolean is, gpointer user_data);

typedef struct
{
  GObjectClass		object_class;

  const char		*stock_id;
  const char		*format;

  void			(*is)		(const char		*uri,
					 MNMailboxIsCallback	*callback,
					 gpointer		user_data);
  void			(*check)	(MNMailbox		*mailbox);
} MNMailboxClass;

const GType	*mn_mailbox_get_types		(void);
GType		mn_mailbox_get_type		(void);

typedef void (MNMailboxNewAsyncCallback) (MNMailbox *mailbox,
					  gpointer user_data);

void		mn_mailbox_new_async		(const char	*uri,
						 MNMailboxNewAsyncCallback *callback,
						 gpointer	user_data);
void		mn_mailbox_set_init_error	(MNMailbox	*mailbox,
						 const char	*format,
						 ...);

const char	*mn_mailbox_get_uri		(MNMailbox	*mailbox);
const char	*mn_mailbox_get_name		(MNMailbox	*mailbox);

void		mn_mailbox_set_automatic	(MNMailbox	*mailbox,
						 gboolean	automatic);
gboolean	mn_mailbox_get_automatic	(MNMailbox	*mailbox);

void		mn_mailbox_set_has_new		(MNMailbox	*mailbox,
						 gboolean	has_new);
gboolean	mn_mailbox_get_has_new		(MNMailbox	*mailbox);

void		mn_mailbox_set_error		(MNMailbox	*mailbox,
						 const char	*format,
						 ...);
const char	*mn_mailbox_get_error		(MNMailbox	*mailbox);

typedef enum
{
  MN_MAILBOX_MONITOR_EVENT_CHANGED		= 1 << GNOME_VFS_MONITOR_EVENT_CHANGED,
  MN_MAILBOX_MONITOR_EVENT_DELETED		= 1 << GNOME_VFS_MONITOR_EVENT_DELETED,
  MN_MAILBOX_MONITOR_EVENT_STARTEXECUTING	= 1 << GNOME_VFS_MONITOR_EVENT_STARTEXECUTING,
  MN_MAILBOX_MONITOR_EVENT_STOPEXECUTING	= 1 << GNOME_VFS_MONITOR_EVENT_STOPEXECUTING,
  MN_MAILBOX_MONITOR_EVENT_CREATED		= 1 << GNOME_VFS_MONITOR_EVENT_CREATED,
  MN_MAILBOX_MONITOR_EVENT_METADATA_CHANGED	= 1 << GNOME_VFS_MONITOR_EVENT_METADATA_CHANGED
} MNMailboxMonitorEventType;

void		mn_mailbox_monitor		(MNMailbox	*mailbox,
						 const char	*uri,
						 GnomeVFSMonitorType monitor_type,
						 MNMailboxMonitorEventType events);

void		mn_mailbox_check		(MNMailbox	*mailbox);
void		mn_mailbox_end_check		(MNMailbox	*mailbox);

#endif /* _MN_MAILBOX_H */
