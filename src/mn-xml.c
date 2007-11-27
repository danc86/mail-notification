/*
 * Mail Notification
 * Copyright (C) 2003-2007 Jean-Yves Lefort <jylefort@brutele.be>
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

#include "config.h"
#include <string.h>
#include <glib/gi18n.h>
#include "mn-xml.h"
#include "mn-util.h"

/*** implementation **********************************************************/

void
mn_xml_import_properties (GObject *object, xmlNode *node)
{
  GParamSpec **properties;
  unsigned int n_properties;
  int i;

  g_return_if_fail(G_IS_OBJECT(object));
  g_return_if_fail(node != NULL);

  properties = g_object_class_list_properties(G_OBJECT_GET_CLASS(object), &n_properties);
  for (i = 0; i < n_properties; i++)
    if ((properties[i]->flags & MN_XML_PARAM_IMPORT) != 0)
      {
	char *content;

	content = xmlGetProp(node, g_param_spec_get_name(properties[i]));
	if (content)
	  {
	    GValue value = { 0, };

	    g_value_init(&value, G_PARAM_SPEC_VALUE_TYPE(properties[i]));

	    if (mn_g_value_from_string(&value, content))
	      g_object_set_property(object, g_param_spec_get_name(properties[i]), &value);
	    else
	      g_warning(_("property \"%s\": unable to transform string \"%s\" into a value of type \"%s\""),
			g_param_spec_get_name(properties[i]), content, G_VALUE_TYPE_NAME(&value));

	    g_value_unset(&value);
	    g_free(content);
	  }
      }
  g_free(properties);
}

void
mn_xml_export_properties (GObject *object, xmlNode *node)
{
  GParamSpec **properties;
  unsigned int n_properties;
  int i;

  g_return_if_fail(G_IS_OBJECT(object));
  g_return_if_fail(node != NULL);

  properties = g_object_class_list_properties(G_OBJECT_GET_CLASS(object), &n_properties);
  for (i = 0; i < n_properties; i++)
    if ((properties[i]->flags & MN_XML_PARAM_EXPORT) != 0)
      {
	GValue value = { 0, };
	gboolean is_default;

	g_value_init(&value, G_PARAM_SPEC_VALUE_TYPE(properties[i]));
	g_object_get_property(object, g_param_spec_get_name(properties[i]), &value);

	if ((properties[i]->flags & MN_XML_PARAM_IGNORE_CASE) != 0)
	  {
	    GValue default_value = { 0, };
	    const char *str;
	    const char *default_str;

	    g_assert(G_IS_PARAM_SPEC_STRING(properties[i]));

	    g_value_init(&default_value, G_TYPE_STRING);
	    g_param_value_set_default(properties[i], &default_value);

	    str = g_value_get_string(&value);
	    default_str = g_value_get_string(&default_value);

	    is_default = str && default_str && ! mn_utf8_strcasecmp(str, default_str);

	    g_value_unset(&default_value);
	  }
	else
	  is_default = g_param_value_defaults(properties[i], &value);

	if (! is_default)
	  {
	    char *str;

	    str = mn_g_value_to_string(&value);
	    xmlSetProp(node, g_param_spec_get_name(properties[i]), str);
	    g_free(str);
	  }

	g_value_unset(&value);
      }
  g_free(properties);
}
