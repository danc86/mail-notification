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

#include "jb-dbus.h"
#include "jb-tests.h"
#include "jb-util.h"
#include "jb-group.h"
#include "jb-action.h"

static void
dbus_init (void)
{
  jb_register_program("dbus-binding-tool", 0);
}

static void
dbus_configure (void)
{
  jb_require_program("dbus-binding-tool");
}

JBFeature jb_dbus_feature = {
  dbus_init,
  dbus_configure
};

G_DEFINE_TYPE(JBDBusInterfaceClient, jb_dbus_interface_client, JB_TYPE_GROUP_RESOURCE)

JBDBusInterfaceClient *
jb_dbus_interface_client_new (const char *name, const char *client)
{
  JBDBusInterfaceClient *self;

  g_return_val_if_fail(name != NULL, NULL);
  g_return_val_if_fail(client != NULL, NULL);
  g_return_val_if_fail(jb_feature_is_enabled(&jb_dbus_feature), NULL);

  self = g_object_new(JB_TYPE_DBUS_INTERFACE_CLIENT, NULL);
  self->name = g_strdup(name);
  self->client = g_strdup(client);

  return self;
}

static char *
dbus_interface_client_to_string (JBResource *res)
{
  JBDBusInterfaceClient *self = JB_DBUS_INTERFACE_CLIENT(res);

  return g_strdup_printf("client of D-Bus interface %s", self->name);
}

static void
dbus_interface_client_get_files (JBDBusInterfaceClient *self,
				 char **xmlfile,
				 char **clientfile)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(self);
  char *cprefix;

  cprefix = jb_strdelimit(self->name, ".", '-');
  if (xmlfile != NULL)
    *xmlfile = g_strdup_printf("%s/%s.xml", gres->group->srcdir, cprefix);
  if (clientfile != NULL)
    *clientfile = g_strdup_printf("%s/%s", gres->group->builddir, self->client);
  g_free(cprefix);
}

static void
dbus_interface_client_pre_build (JBResource *res)
{
  JBDBusInterfaceClient *self = JB_DBUS_INTERFACE_CLIENT(res);
  char *xmlfile;
  char *clientfile;

  dbus_interface_client_get_files(self, &xmlfile, &clientfile);

  if (! jb_is_uptodate(clientfile, xmlfile))
    {
      JBGroupResource *gres = JB_GROUP_RESOURCE(res);

      jb_resource_message_building(res);

      jb_mkdir(gres->group->builddir);

      jb_action_exec("$dbus-binding-tool --mode=glib-client $xmlfile > $clientfile.tmp && mv -f $clientfile.tmp $clientfile",
		     "xmlfile", xmlfile,
		     "clientfile", clientfile,
		     NULL);
    }

  g_free(xmlfile);
  g_free(clientfile);
}

static void
dbus_interface_client_makedist (JBResource *res)
{
  JBDBusInterfaceClient *self = JB_DBUS_INTERFACE_CLIENT(res);
  char *xmlfile;

  dbus_interface_client_get_files(self, &xmlfile, NULL);

  jb_action_add_to_dist(xmlfile);

  g_free(xmlfile);
}

static void
dbus_interface_client_clean (JBResource *res)
{
  JBDBusInterfaceClient *self = JB_DBUS_INTERFACE_CLIENT(res);
  char *clientfile;

  dbus_interface_client_get_files(self, NULL, &clientfile);

  jb_action_rm(clientfile);

  g_free(clientfile);
}

static void
jb_dbus_interface_client_init (JBDBusInterfaceClient *self)
{
}

static void
jb_dbus_interface_client_class_init (JBDBusInterfaceClientClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);

  rclass->to_string = dbus_interface_client_to_string;
  rclass->pre_build = dbus_interface_client_pre_build;
  rclass->makedist = dbus_interface_client_makedist;
  rclass->clean = dbus_interface_client_clean;
}

G_DEFINE_TYPE(JBDBusInterfaceServer, jb_dbus_interface_server, JB_TYPE_GROUP_RESOURCE)

JBDBusInterfaceServer *
jb_dbus_interface_server_new (const char *name,
			      const char *server,
			      const char *server_prefix)
{
  JBDBusInterfaceServer *self;

  g_return_val_if_fail(name != NULL, NULL);
  g_return_val_if_fail(server != NULL, NULL);
  g_return_val_if_fail(server_prefix != NULL, NULL);
  g_return_val_if_fail(jb_feature_is_enabled(&jb_dbus_feature), NULL);

  self = g_object_new(JB_TYPE_DBUS_INTERFACE_SERVER, NULL);
  self->name = g_strdup(name);
  self->server = g_strdup(server);
  self->server_prefix = g_strdup(server_prefix);

  return self;
}

static char *
dbus_interface_server_to_string (JBResource *res)
{
  JBDBusInterfaceServer *self = JB_DBUS_INTERFACE_SERVER(res);

  return g_strdup_printf("server of D-Bus interface %s", self->name);
}

static void
dbus_interface_server_get_files (JBDBusInterfaceServer *self,
				 char **xmlfile,
				 char **serverfile)
{
  JBGroupResource *gres = JB_GROUP_RESOURCE(self);
  char *cprefix;

  cprefix = jb_strdelimit(self->name, ".", '-');
  if (xmlfile != NULL)
    *xmlfile = g_strdup_printf("%s/%s.xml", gres->group->srcdir, cprefix);
  if (serverfile != NULL)
    *serverfile = g_strdup_printf("%s/%s", gres->group->builddir, self->server);
  g_free(cprefix);
}

static void
dbus_interface_server_pre_build (JBResource *res)
{
  JBDBusInterfaceServer *self = JB_DBUS_INTERFACE_SERVER(res);
  char *xmlfile;
  char *serverfile;

  dbus_interface_server_get_files(self, &xmlfile, &serverfile);

  if (! jb_is_uptodate(serverfile, xmlfile))
    {
      JBGroupResource *gres = JB_GROUP_RESOURCE(res);

      jb_resource_message_building(res);

      jb_mkdir(gres->group->builddir);

      jb_action_exec("$dbus-binding-tool --mode=glib-server --prefix=$server-prefix $xmlfile > $serverfile.tmp && mv -f $serverfile.tmp $serverfile",
		     "server-prefix", self->server_prefix,
		     "xmlfile", xmlfile,
		     "serverfile", serverfile,
		     NULL);
    }

  g_free(xmlfile);
  g_free(serverfile);
}

static void
dbus_interface_server_makedist (JBResource *res)
{
  JBDBusInterfaceServer *self = JB_DBUS_INTERFACE_SERVER(res);
  char *xmlfile;

  dbus_interface_server_get_files(self, &xmlfile, NULL);

  jb_action_add_to_dist(xmlfile);

  g_free(xmlfile);
}

static void
dbus_interface_server_clean (JBResource *res)
{
  JBDBusInterfaceServer *self = JB_DBUS_INTERFACE_SERVER(res);
  char *serverfile;

  dbus_interface_server_get_files(self, NULL, &serverfile);

  jb_action_rm(serverfile);

  g_free(serverfile);
}

static void
jb_dbus_interface_server_init (JBDBusInterfaceServer *self)
{
}

static void
jb_dbus_interface_server_class_init (JBDBusInterfaceServerClass *class)
{
  JBResourceClass *rclass = JB_RESOURCE_CLASS(class);

  rclass->to_string = dbus_interface_server_to_string;
  rclass->pre_build = dbus_interface_server_pre_build;
  rclass->makedist = dbus_interface_server_makedist;
  rclass->clean = dbus_interface_server_clean;
}

void
jb_group_add_dbus_interface (JBGroup *self,
			     const char *name,
			     const char *client,
			     const char *server,
			     const char *server_prefix)
{
  g_return_if_fail(JB_IS_GROUP(self));
  g_return_if_fail(name != NULL);

  if (client != NULL)
    jb_group_add_resource(self, JB_GROUP_RESOURCE(jb_dbus_interface_client_new(name, client)));
  if (server != NULL)
    jb_group_add_resource(self, JB_GROUP_RESOURCE(jb_dbus_interface_server_new(name, server, server_prefix)));
}
