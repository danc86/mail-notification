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

#ifndef _JB_DBUS_H
#define _JB_DBUS_H

#include "jb-resource.h"
#include "jb-feature.h"

extern JBFeature jb_dbus_feature;

#define JB_TYPE_DBUS_INTERFACE_CLIENT		(jb_dbus_interface_client_get_type())
#define JB_DBUS_INTERFACE_CLIENT(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_DBUS_INTERFACE_CLIENT, JBDBusInterfaceClient))
#define JB_IS_DBUS_INTERFACE_CLIENT(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_DBUS_INTERFACE_CLIENT))

typedef struct
{
  JBGroupResource	parent;

  char			*name;
  char			*client;
} JBDBusInterfaceClient;

typedef struct
{
  JBGroupResourceClass	parent;
} JBDBusInterfaceClientClass;

GType jb_dbus_interface_client_get_type (void);

JBDBusInterfaceClient *jb_dbus_interface_client_new (const char *name,
						     const char *client);

#define JB_TYPE_DBUS_INTERFACE_SERVER		(jb_dbus_interface_server_get_type())
#define JB_DBUS_INTERFACE_SERVER(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), JB_TYPE_DBUS_INTERFACE_SERVER, JBDBusInterfaceServer))
#define JB_IS_DBUS_INTERFACE_SERVER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), JB_TYPE_DBUS_INTERFACE_SERVER))

typedef struct
{
  JBGroupResource	parent;

  char			*name;
  char			*server;
  char			*server_prefix;
} JBDBusInterfaceServer;

typedef struct
{
  JBGroupResourceClass	parent;
} JBDBusInterfaceServerClass;

GType jb_dbus_interface_server_get_type (void);

JBDBusInterfaceServer *jb_dbus_interface_server_new (const char *name,
						     const char *server,
						     const char *server_prefix);

void jb_group_add_dbus_interface (JBGroup *self,
				  const char *name,
				  const char *client,
				  const char *server,
				  const char *server_prefix);

#endif /* _JB_DBUS_H */
