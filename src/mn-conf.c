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

#include "config.h"
#include <stdarg.h>
#include <eel/eel.h>
#include "mn-mailboxes.h"
#include "mn-util.h"
#include "mn-conf.h"

/*** cpp *********************************************************************/

#define WINDOW_WIDTH_KEY		"mn-conf-window-width-key"
#define WINDOW_HEIGHT_KEY		"mn-conf-window-height-key"

/*** functions ***************************************************************/

static void	mn_conf_notify_delay_cb		(GConfClient	*client,
						 guint		cnxn_id,
						 GConfEntry	*entry,
						 gpointer	user_data);
static void	mn_conf_notify_mailboxes_cb	(GConfClient	*client,
						 guint		cnxn_id,
						 GConfEntry	*entry,
						 gpointer	user_data);

static gboolean mn_conf_link_window_h (GtkWidget *widget,
				       GdkEventConfigure *event,
				       gpointer user_data);
static void mn_conf_link_window_notify_cb (GConfClient *client,
					   guint cnxn_id,
					   GConfEntry *entry,
					   gpointer user_data);

static void mn_conf_link_toggle_button_h (GtkToggleButton*button,
					  gpointer user_data);
static void mn_conf_link_toggle_button_notify_cb (GConfClient *client,
						  guint cnxn_id,
						  GConfEntry *entry,
						  gpointer user_data);

static void mn_conf_link_spin_button_h (GtkSpinButton *button,
					gpointer user_data);
static void mn_conf_link_spin_button_notify_cb (GConfClient *client,
						guint cnxn_id,
						GConfEntry *entry,
						gpointer user_data);

static void mn_conf_link_entry_h (GtkEditable *editable, gpointer user_data);
static void mn_conf_link_entry_notify_cb (GConfClient *client,
					  guint cnxn_id,
					  GConfEntry *entry,
					  gpointer user_data);

static void mn_conf_link_weak_notify_cb (gpointer data,
					 GObject *former_object);

/*** implementation **********************************************************/

static void
mn_conf_notify_delay_cb (GConfClient *client,
			 guint cnxn_id,
			 GConfEntry *entry,
			 gpointer user_data)
{
  mn_mailboxes_install_timeout();
}

static void
mn_conf_notify_mailboxes_cb (GConfClient *client,
			     guint cnxn_id,
			     GConfEntry *entry,
			     gpointer user_data)
{
  mn_mailboxes_register();
}

void
mn_conf_init (void)
{
  GSList *gconf_mailboxes;
  GSList *l;

  /* convert old style locators */

  gconf_mailboxes = eel_gconf_get_string_list(MN_CONF_MAILBOXES);
  MN_LIST_FOREACH(l, gconf_mailboxes)
    {
      char *locator_or_uri = l->data;
      char *uri;

      if (locator_or_uri[0] == '/')
	uri = g_strconcat("file://", locator_or_uri, NULL);
      else if (! strncmp(locator_or_uri, "pop3:", 5))
	uri = g_strconcat("pop://", locator_or_uri + 5, NULL);
      else
	uri = g_strdup(locator_or_uri);

      g_free(locator_or_uri);
      l->data = uri;
    }
  eel_gconf_set_string_list(MN_CONF_MAILBOXES, gconf_mailboxes);
  mn_slist_free(gconf_mailboxes);
  
  /* monitor some keys */

  eel_gconf_monitor_add(MN_CONF_NAMESPACE);

  eel_gconf_notification_add(MN_CONF_DELAY_NAMESPACE, mn_conf_notify_delay_cb, NULL);
  eel_gconf_notification_add(MN_CONF_MAILBOXES, mn_conf_notify_mailboxes_cb, NULL);
}

void
mn_conf_link (GtkWidget *widget, ...)
{
  va_list args;

  va_start(args, widget);
  while (widget)
    {
      const char *key;
      const char *signal_name;
      GCallback signal_handler;
      GConfClientNotifyFunc notification_cb;
      unsigned int notification_id;

      key = va_arg(args, const char *);
      g_return_if_fail(key != NULL);

      /* the order of these tests is important */
      if (GTK_IS_WINDOW(widget))
	{
	  char *width_key;
	  char *height_key;

	  width_key = g_strdup_printf("%s/width", key);
	  height_key = g_strdup_printf("%s/height", key);

	  g_object_set_data_full(G_OBJECT(widget), WINDOW_WIDTH_KEY, width_key, g_free);
	  g_object_set_data_full(G_OBJECT(widget), WINDOW_HEIGHT_KEY, height_key, g_free);

	  gtk_window_set_default_size(GTK_WINDOW(widget),
				      eel_gconf_get_integer(width_key),
				      eel_gconf_get_integer(height_key));

	  signal_name = "configure-event";
	  signal_handler = G_CALLBACK(mn_conf_link_window_h);
	  notification_cb = mn_conf_link_window_notify_cb;
	}
      else if (GTK_IS_TOGGLE_BUTTON(widget))
	{
	  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), eel_gconf_get_boolean(key));

	  signal_name = "toggled";
	  signal_handler = G_CALLBACK(mn_conf_link_toggle_button_h);
	  notification_cb = mn_conf_link_toggle_button_notify_cb;
	}
      else if (GTK_IS_SPIN_BUTTON(widget))
	{
	  gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), eel_gconf_get_integer(key));

	  signal_name = "value-changed";
	  signal_handler = G_CALLBACK(mn_conf_link_spin_button_h);
	  notification_cb = mn_conf_link_spin_button_notify_cb;
	}
      else if (GTK_IS_ENTRY(widget))
	{
	  char *str;

	  str = eel_gconf_get_string(key);
	  gtk_entry_set_text(GTK_ENTRY(widget), str ? str : "");
	  g_free(str);

	  signal_name = "changed";
	  signal_handler = G_CALLBACK(mn_conf_link_entry_h);
	  notification_cb = mn_conf_link_entry_notify_cb;
	}
      else
	g_return_if_reached();

      g_signal_connect_data(widget, signal_name, signal_handler, g_strdup(key), (GClosureNotify) g_free, 0);
      notification_id = eel_gconf_notification_add(key, notification_cb, widget);
      g_object_weak_ref(G_OBJECT(widget), mn_conf_link_weak_notify_cb, GUINT_TO_POINTER(notification_id));

      widget = va_arg(args, GtkWidget *);
    }
  va_end(args);
}

static gboolean
mn_conf_link_window_h (GtkWidget *widget,
		       GdkEventConfigure *event,
		       gpointer user_data)
{
  const char *width_key = g_object_get_data(G_OBJECT(widget), WINDOW_WIDTH_KEY);
  const char *height_key = g_object_get_data(G_OBJECT(widget), WINDOW_HEIGHT_KEY);

  eel_gconf_set_integer(width_key, event->width);
  eel_gconf_set_integer(height_key, event->height);

  return FALSE;
}

static void
mn_conf_link_window_notify_cb (GConfClient *client,
			       guint cnxn_id,
			       GConfEntry *entry,
			       gpointer user_data)
{
  GtkWindow *window = user_data;
  const char *width_key = g_object_get_data(G_OBJECT(window), WINDOW_WIDTH_KEY);
  const char *height_key = g_object_get_data(G_OBJECT(window), WINDOW_HEIGHT_KEY);

  gtk_window_resize(window,
		    eel_gconf_get_integer(width_key),
		    eel_gconf_get_integer(height_key));
}

static void
mn_conf_link_toggle_button_h (GtkToggleButton *button, gpointer user_data)
{
  const char *key = user_data;
  eel_gconf_set_boolean(key, gtk_toggle_button_get_active(button));
}

static void
mn_conf_link_toggle_button_notify_cb (GConfClient *client,
				      guint cnxn_id,
				      GConfEntry *entry,
				      gpointer user_data)
{
  GConfValue *value = gconf_entry_get_value(entry);
  GtkToggleButton *button = user_data;

  gtk_toggle_button_set_active(button, gconf_value_get_bool(value));
}

static void
mn_conf_link_spin_button_h (GtkSpinButton *button, gpointer user_data)
{
  const char *key = user_data;
  eel_gconf_set_integer(key, gtk_spin_button_get_value_as_int(button));
}

static void
mn_conf_link_spin_button_notify_cb (GConfClient *client,
				    guint cnxn_id,
				    GConfEntry *entry,
				    gpointer user_data)
{
  GConfValue *value = gconf_entry_get_value(entry);
  GtkSpinButton *button = user_data;

  gtk_spin_button_set_value(button, gconf_value_get_int(value));
}

static void
mn_conf_link_entry_h (GtkEditable *editable, gpointer user_data)
{
  const char *key = user_data;
  const char *str;

  str = gtk_entry_get_text(GTK_ENTRY(editable));
  if (*str)
    eel_gconf_set_string(key, str);
  else
    eel_gconf_unset(key);
}

static void
mn_conf_link_entry_notify_cb (GConfClient *client,
			      guint cnxn_id,
			      GConfEntry *entry,
			      gpointer user_data)
{
  GConfValue *value = gconf_entry_get_value(entry);
  GtkEntry *entry_widget = user_data;
  const char *str;

  str = gconf_value_get_string(value);
  gtk_entry_set_text(entry_widget, str ? str : "");
}

static void
mn_conf_link_weak_notify_cb (gpointer data, GObject *former_object)
{
  unsigned int notification_id = GPOINTER_TO_UINT(data);
  eel_gconf_notification_remove(notification_id);
}
