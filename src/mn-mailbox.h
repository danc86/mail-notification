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

#include <glib.h>
#include <glib-object.h>

#define MN_TYPE_MAILBOX			(mn_mailbox_get_type())
#define MN_MAILBOX(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), MN_TYPE_MAILBOX, MNMailbox))
#define MN_MAILBOX_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), MN_TYPE_MAILBOX, MNMailboxClass))
#define MN_IS_MAILBOX(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), MN_TYPE_MAILBOX))
#define MN_IS_MAILBOX_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), MN_TYPE_MAILBOX))
#define MN_MAILBOX_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), MN_TYPE_MAILBOX, MNMailboxClass))

#define MN_MAILBOX_ERROR		(mn_mailbox_error_quark())

typedef enum
{
  MN_MAILBOX_ERROR_NOT_FOUND,
  MN_MAILBOX_ERROR_INITIALIZATION,
  MN_MAILBOX_ERROR_UNKNOWN_FORMAT
} MNMailboxError;

typedef struct
{
  GObject		object;

  char			*locator;	/* internal locator */
  char			*name;		/* visible mailbox name */

  GError		*err;		/* initialization error */
} MNMailbox;

typedef struct
{
  GObjectClass		object_class;

  const char		*format;	/* visible format name */
  gboolean		is_remote;

  gboolean		(*is)		(const char *locator);
  gboolean		(*has_new)	(MNMailbox *mailbox, GError **err);
} MNMailboxClass;

GType		mn_mailbox_get_type	(void);
GQuark		mn_mailbox_error_quark	(void);

MNMailbox	*mn_mailbox_new		(const char	*locator,
					 GError		**err);

#endif /* _MN_MAILBOX_H */
