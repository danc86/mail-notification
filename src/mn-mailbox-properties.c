/* 
 * Copyright (c) 2004 Jean-Yves Lefort <jylefort@brutele.be>
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

#include "config.h"
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "mn-mailbox-properties.h"

/*** functions ***************************************************************/

static void mn_mailbox_properties_iface_init (MNMailboxPropertiesIface *iface);

/*** implementation **********************************************************/

GType
mn_mailbox_properties_get_type (void)
{
  static GType mailbox_properties_type = 0;
  
  if (! mailbox_properties_type)
    {
      static const GTypeInfo mailbox_properties_info = {
	sizeof(MNMailboxPropertiesIface),
	NULL,
	NULL,
	(GClassInitFunc) mn_mailbox_properties_iface_init,
	NULL,
	NULL,
	0,
	0,
	NULL
      };
      
      mailbox_properties_type = g_type_register_static(G_TYPE_INTERFACE,
						       "MNMailboxProperties",
						       &mailbox_properties_info,
						       0);
    }
  
  return mailbox_properties_type;
}

static void
mn_mailbox_properties_iface_init (MNMailboxPropertiesIface *iface)
{
  g_object_interface_install_property(iface,
				      g_param_spec_string("label",
							  _("Label"),
							  _("The marked up text to show in the type combo box"),
							  NULL,
							  G_PARAM_READABLE));
  g_object_interface_install_property(iface,
				      g_param_spec_object("size-group",
							  _("Size group"),
							  _("A GtkSizeGroup for aligning control labels"),
							  GTK_TYPE_SIZE_GROUP,
							  G_PARAM_WRITABLE | G_PARAM_READABLE | G_PARAM_CONSTRUCT_ONLY));
  g_object_interface_install_property(iface,
				      g_param_spec_boolean("complete",
							   _("Complete"),
							   _("Whether the properties are completely filled or not"),
							   FALSE,
							   G_PARAM_READABLE));
}

GtkSizeGroup *
mn_mailbox_properties_get_size_group (MNMailboxProperties *properties)
{
  GtkSizeGroup *size_group;

  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES(properties), NULL);

  g_object_get(G_OBJECT(properties), "size-group", &size_group, NULL);
  g_object_unref(size_group);

  return size_group;
}

char *
mn_mailbox_properties_get_label (MNMailboxProperties *properties)
{
  char *label;

  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES(properties), NULL);

  g_object_get(G_OBJECT(properties), "label", &label, NULL);
  return label;
}

gboolean
mn_mailbox_properties_set_uri (MNMailboxProperties *properties,
			       const char *uri)
{
  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES(properties), FALSE);
  g_return_val_if_fail(uri != NULL, FALSE);

  return MN_MAILBOX_PROPERTIES_GET_IFACE(properties)->set_uri(properties, uri);
}

char *
mn_mailbox_properties_get_uri (MNMailboxProperties *properties)
{
  g_return_val_if_fail(MN_IS_MAILBOX_PROPERTIES(properties), NULL);

  return MN_MAILBOX_PROPERTIES_GET_IFACE(properties)->get_uri(properties);
}
