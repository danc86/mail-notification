/* 
 * Copyright (c) 1999 Free Software Foundation, Inc.
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
#include <gnome.h>
#include <eel/eel.h>
#include "mn-util.h"
#include "mn-conf.h"

/*** cpp *********************************************************************/

#define SESSION_MANUAL_CONFIG_PREFIX	"session-manual/"
#define SESSION_MANUAL_NAME		"Default"

#define WINDOW_WIDTH_KEY		"mn-conf-window-width-key"
#define WINDOW_HEIGHT_KEY		"mn-conf-window-height-key"

/*** types *******************************************************************/

typedef struct
{
  int	order;
  int	argc;
  char	**argv;
} StartupClient;

/*** functions ***************************************************************/

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

static void mn_conf_startup_client_free (StartupClient *client);
static void mn_conf_startup_clients_free (GSList *list);

static int mn_conf_startup_client_compare (gconstpointer a, gconstpointer b);

static GSList *mn_conf_startup_list_read (const char *name);
static void mn_conf_startup_list_write (GSList *list, const char *name);

static GSList *mn_conf_get_autostart_elem (GSList *list);

/*** implementation **********************************************************/

void
mn_conf_init (void)
{
  /* monitor our namespace */
  eel_gconf_monitor_add(MN_CONF_NAMESPACE);
}

void
mn_conf_unset_obsolete (void)
{
  const char *obsolete[] = {
    MN_CONF_LOCAL_NAMESPACE,
    MN_CONF_REMOTE_NAMESPACE,
    MN_CONF_COMMANDS_CLICKED_NAMESPACE,
    MN_CONF_PREFERENCES_DIALOG
  };
  int i;
      
  for (i = 0; i < G_N_ELEMENTS(obsolete); i++)
    {
      g_message(_("recursively unsetting %s"), obsolete[i]);
      mn_conf_recursive_unset(obsolete[i], GCONF_UNSET_INCLUDING_SCHEMA_NAMES);
    }

  g_message(_("syncing the GConf database"));
  eel_gconf_suggest_sync();
  
  g_message(_("completed"));
}

void
mn_conf_recursive_unset (const char *key, GConfUnsetFlags flags)
{
  GConfClient *client;
  GError *err = NULL;

  g_return_if_fail(key != NULL);

  client = eel_gconf_client_get_global();
  g_return_if_fail(client != NULL);

  gconf_client_recursive_unset(client, key, flags, &err);
  eel_gconf_handle_error(&err);
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
  const char *width_key;
  const char *height_key;

  GDK_THREADS_ENTER();
  width_key = g_object_get_data(G_OBJECT(window), WINDOW_WIDTH_KEY);
  height_key = g_object_get_data(G_OBJECT(window), WINDOW_HEIGHT_KEY);
  gtk_window_resize(window,
		    eel_gconf_get_integer(width_key),
		    eel_gconf_get_integer(height_key));
  GDK_THREADS_LEAVE();
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

  GDK_THREADS_ENTER();
  gtk_toggle_button_set_active(button, gconf_value_get_bool(value));
  GDK_THREADS_LEAVE();
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

  GDK_THREADS_ENTER();
  gtk_spin_button_set_value(button, gconf_value_get_int(value));
  GDK_THREADS_LEAVE();
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
  const char *str = NULL;

  GDK_THREADS_ENTER();
  if (value)
    str = gconf_value_get_string(value);
  gtk_entry_set_text(entry_widget, str ? str : "");
  GDK_THREADS_LEAVE();
}

static void
mn_conf_link_weak_notify_cb (gpointer data, GObject *former_object)
{
  unsigned int notification_id = GPOINTER_TO_UINT(data);
  eel_gconf_notification_remove(notification_id);
}

/*
 * The mn_conf_startup_* functions are based on code taken from
 * startup-programs.c in gnome-session, written by Owen Taylor.
 */

static void
mn_conf_startup_client_free (StartupClient *client)
{
  int i;

  g_return_if_fail(client != NULL);

  for (i = 0; i < client->argc; i++)
    g_free(client->argv[i]);

  g_free(client->argv);
  g_free(client);
}

static void
mn_conf_startup_clients_free (GSList *list)
{
  eel_g_slist_free_deep_custom(list, (GFunc) mn_conf_startup_client_free, NULL);
}

static int
mn_conf_startup_client_compare (gconstpointer a, gconstpointer b)
{
  const StartupClient *client_a = a;
  const StartupClient *client_b = b;

  return client_a->order - client_b->order;
}

static GSList *
mn_conf_startup_list_read (const char *name)
{
  GSList *list = NULL;
  gpointer iterator;
  char *p;
  StartupClient *client = NULL;
  char *handle = NULL;

  g_return_val_if_fail(name != NULL, NULL);
  
  gnome_config_push_prefix(SESSION_MANUAL_CONFIG_PREFIX);

  iterator = gnome_config_init_iterator(name);
  while (iterator)
    {
      char *key;
      char *value;

      iterator = gnome_config_iterator_next(iterator, &key, &value);
      if (! iterator)
	break;

      p = strchr(key, ',');
      if (p)
	{
	  *p = 0;

	  if (! client || strcmp(handle, key))
	    {
	      g_free(handle);
	      handle = g_strdup(key);

	      client = g_new0(StartupClient, 1);
	      list = g_slist_append(list, client);
	    }

	  if (! strcmp(p + 1, "Priority"))
	    client->order = atoi(value);
	  else if (! strcmp(p + 1, "RestartCommand"))
	    gnome_config_make_vector(value, &client->argc, &client->argv);
	}

      g_free(key);
      g_free(value);
    }

  g_free(handle);
  gnome_config_pop_prefix();

  return g_slist_sort(list, mn_conf_startup_client_compare);
}

static void
mn_conf_startup_list_write (GSList *list, const char *name)
{
  char *prefix;
  int i = 0;
  GSList *l;

  g_return_if_fail(name != NULL);

  gnome_config_push_prefix(SESSION_MANUAL_CONFIG_PREFIX);
  gnome_config_clean_section(name);
  gnome_config_pop_prefix();

  prefix = g_strconcat(SESSION_MANUAL_CONFIG_PREFIX, name, "/", NULL);
  gnome_config_push_prefix(prefix);
  g_free(prefix);
  
  gnome_config_set_int("num_clients", g_slist_length(list));

  MN_LIST_FOREACH(l, list)
    {
      StartupClient *client = l->data;
      char *key;

      key = g_strdup_printf("%d,%s", i, "RestartStyleHint");
      gnome_config_set_int(key, 3); /* RestartNever */
      g_free(key);

      key = g_strdup_printf("%d,%s", i, "Priority");
      gnome_config_set_int(key, client->order);
      g_free(key);

      key = g_strdup_printf("%d,%s", i, "RestartCommand");
      gnome_config_set_vector(key, client->argc, (const char * const *) client->argv);
      g_free(key);

      i++;
    }
  
  gnome_config_pop_prefix();
  gnome_config_sync();
}

static GSList *
mn_conf_get_autostart_elem (GSList *list)
{
  GSList *l;

  MN_LIST_FOREACH(l, list)
    {
      StartupClient *client = l->data;

      if (client->argc > 0 && ! strcmp(client->argv[0], "mail-notification"))
	return l;
    }

  return NULL;
}

gboolean
mn_conf_get_autostart (void)
{
  GSList *clients;
  gboolean autostart;

  clients = mn_conf_startup_list_read(SESSION_MANUAL_NAME);
  autostart = mn_conf_get_autostart_elem(clients) != NULL;
  mn_conf_startup_clients_free(clients);

  return autostart;
}

void
mn_conf_set_autostart (gboolean autostart)
{
  GSList *clients;
  GSList *elem;

  clients = mn_conf_startup_list_read(SESSION_MANUAL_NAME);
  elem = mn_conf_get_autostart_elem(clients);
  if ((elem != NULL) != autostart)
    {
      if (autostart)
	{
	  StartupClient *client;

	  client = g_new0(StartupClient, 1);
	  client->order = 50;
	  gnome_config_make_vector("mail-notification", &client->argc, &client->argv);

	  clients = g_slist_append(clients, client);
	  clients = g_slist_sort(clients, mn_conf_startup_client_compare);
	}
      else
	clients = mn_g_slist_delete_link_deep_custom(clients, elem, (GFunc) mn_conf_startup_client_free, NULL);

      mn_conf_startup_list_write(clients, SESSION_MANUAL_NAME);
    }
  mn_conf_startup_clients_free(clients);
}
