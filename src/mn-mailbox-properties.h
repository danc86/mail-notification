/* 
 * Copyright (C) 2004 Jean-Yves Lefort <jylefort@brutele.be>
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

#ifndef _MN_MAILBOX_PROPERTIES_H
#define _MN_MAILBOX_PROPERTIES_H

#include <glib-object.h>
#include "mn-uri.h"

#define MN_TYPE_MAILBOX_PROPERTIES		(mn_mailbox_properties_get_type ())
#define MN_MAILBOX_PROPERTIES(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), MN_TYPE_MAILBOX_PROPERTIES, MNMailboxProperties))
#define MN_IS_MAILBOX_PROPERTIES(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), MN_TYPE_MAILBOX_PROPERTIES))
#define MN_MAILBOX_PROPERTIES_GET_IFACE(obj)	(G_TYPE_INSTANCE_GET_INTERFACE((obj), MN_TYPE_MAILBOX_PROPERTIES, MNMailboxPropertiesIface))

typedef struct _MNMailboxProperties MNMailboxProperties; /* dummy */

typedef struct
{
  GTypeInterface	parent;

  gboolean	(* set_uri)		(MNMailboxProperties	*properties,
					 MNURI			*uri);
  MNURI	*	(* get_uri)		(MNMailboxProperties	*properties);
} MNMailboxPropertiesIface;

GType mn_mailbox_properties_get_type (void);

GtkSizeGroup *mn_mailbox_properties_get_size_group (MNMailboxProperties *properties);
char *mn_mailbox_properties_get_label (MNMailboxProperties *properties);
gboolean mn_mailbox_properties_set_uri (MNMailboxProperties *properties,
					MNURI *uri);
MNURI *mn_mailbox_properties_get_uri (MNMailboxProperties *properties);

#endif /* _MN_MAILBOX_PROPERTIES_H */
