/* 
 * Copyright (C) 1999 Free Software Foundation, Inc.
 * Copyright (C) 2003-2005 Jean-Yves Lefort <jylefort@brutele.be>
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
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <gnome.h>
#include <eel/eel.h>
#include "mn-util.h"
#include "mn-conf.h"
#include "mn-shell.h"

/*** cpp *********************************************************************/

/* obsolete keys */
#define MN_CONF_OBSOLETE_LOCAL_NAMESPACE \
  MN_CONF_NAMESPACE "/local"
#define MN_CONF_OBSOLETE_LOCAL_ENABLED \
  MN_CONF_OBSOLETE_LOCAL_NAMESPACE "/enabled"
#define MN_CONF_OBSOLETE_LOCAL_DELAY_MINUTES \
  MN_CONF_OBSOLETE_LOCAL_NAMESPACE "/delay/minutes"
#define MN_CONF_OBSOLETE_LOCAL_DELAY_SECONDS \
  MN_CONF_OBSOLETE_LOCAL_NAMESPACE "/delay/seconds"
#define MN_CONF_OBSOLETE_REMOTE_NAMESPACE \
  MN_CONF_NAMESPACE "/remote"
#define MN_CONF_OBSOLETE_REMOTE_ENABLED \
  MN_CONF_OBSOLETE_REMOTE_NAMESPACE "/enabled"
#define MN_CONF_OBSOLETE_REMOTE_DELAY_MINUTES \
  MN_CONF_OBSOLETE_REMOTE_NAMESPACE "/delay/minutes"
#define MN_CONF_OBSOLETE_REMOTE_DELAY_SECONDS \
  MN_CONF_OBSOLETE_REMOTE_NAMESPACE "/delay/seconds"
#define MN_CONF_OBSOLETE_COMMANDS_CLICKED_NAMESPACE \
  MN_CONF_COMMANDS_NAMESPACE "/clicked"
#define MN_CONF_OBSOLETE_COMMANDS_CLICKED_ENABLED \
  MN_CONF_OBSOLETE_COMMANDS_CLICKED_NAMESPACE "/enabled"
#define MN_CONF_OBSOLETE_COMMANDS_CLICKED_COMMAND \
  MN_CONF_OBSOLETE_COMMANDS_CLICKED_NAMESPACE "/command"
#define MN_CONF_OBSOLETE_COMMANDS_DOUBLE_CLICKED_NAMESPACE \
  MN_CONF_COMMANDS_NAMESPACE "/double-clicked"
#define MN_CONF_OBSOLETE_COMMANDS_DOUBLE_CLICKED_ENABLED	\
  MN_CONF_OBSOLETE_COMMANDS_DOUBLE_CLICKED_NAMESPACE "/enabled"
#define MN_CONF_OBSOLETE_COMMANDS_DOUBLE_CLICKED_COMMAND	\
  MN_CONF_OBSOLETE_COMMANDS_DOUBLE_CLICKED_NAMESPACE "/command"
#define MN_CONF_OBSOLETE_PREFERENCES_DIALOG \
  MN_CONF_UI_NAMESPACE "/preferences-dialog"
#define MN_CONF_OBSOLETE_SUMMARY_DIALOG \
  MN_CONF_UI_NAMESPACE "/summary-dialog"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_TITLE_ENABLED \
  MN_CONF_MAIL_SUMMARY_POPUP_FONTS_TITLE_NAMESPACE "/enabled"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_CONTENTS_ENABLED \
  MN_CONF_MAIL_SUMMARY_POPUP_FONTS_CONTENTS_NAMESPACE "/enabled"
#define MN_CONF_OBSOLETE_DOUBLE_CLICK_ACTION \
  MN_CONF_NAMESPACE "/double-click-action"

#define BLOCK(info) \
  g_signal_handler_block((info)->object, (info)->handler_id)
#define UNBLOCK(info) \
  g_signal_handler_unblock((info)->object, (info)->handler_id)

#define LINK_INFO(ptr)			((LinkInfo *) (ptr))

#define SESSION_MANUAL_CONFIG_PREFIX	"session-manual/"
#define SESSION_MANUAL_NAME		"Default"

/*** types *******************************************************************/

typedef struct
{
  gpointer		object;
  char			*key;
  unsigned long		handler_id;
  GDestroyNotify	finalize;
} LinkInfo;

typedef struct
{
  LinkInfo		parent;
  int			string_column;
} LinkComboBoxToStringInfo;

typedef struct
{
  LinkInfo		parent;
  char			*str;
} LinkRadioButtonToStringInfo;

typedef struct
{
  LinkInfo		parent;
  char			*width_key;
  char			*height_key;
} LinkWindowInfo;

typedef struct
{
  LinkInfo		parent;
  GEnumClass		*enum_class;
} LinkRadioActionInfo;

typedef struct
{
  LinkInfo		parent;
  GParamSpec		*pspec;
} LinkObjectInfo;

typedef struct
{
  int	order;
  int	argc;
  char	**argv;
} StartupClient;

/*** variables ***************************************************************/

const char *mn_conf_dot_dir = NULL;

/*** functions ***************************************************************/

static void mn_conf_handle_obsolete_key (const char *obsolete, const char *new);

static void mn_conf_link_weak_notify_cb (gpointer data, GObject *former_object);

static void mn_conf_link_combo_box_to_string_set (LinkComboBoxToStringInfo *info,
						  const GConfValue *value);
static void mn_conf_link_combo_box_to_string_h (GtkComboBox *combo,
						gpointer user_data);
static void mn_conf_link_combo_box_to_string_notify_cb (GConfClient *client,
							unsigned int cnxn_id,
							GConfEntry *entry,
							gpointer user_data);

static void mn_conf_link_radio_button_to_string (GtkRadioButton *radio,
						 const char *key,
						 const char *str);
static void mn_conf_link_radio_button_to_string_h (GtkToggleButton *toggle,
						   gpointer user_data);
static void mn_conf_link_radio_button_to_string_notify_cb (GConfClient *client,
							   unsigned int cnxn_id,
							   GConfEntry *entry,
							   gpointer user_data);
static void mn_conf_link_radio_button_to_string_free_info (LinkRadioButtonToStringInfo *info);

static gboolean mn_conf_link_window_h (GtkWidget *widget,
				       GdkEventConfigure *event,
				       gpointer user_data);
static void mn_conf_link_window_notify_cb (GConfClient *client,
					   unsigned int cnxn_id,
					   GConfEntry *entry,
					   gpointer user_data);
static void mn_conf_link_window_free_info (LinkWindowInfo *info);

static void mn_conf_link_radio_action_set (LinkRadioActionInfo *info,
					   const GConfValue *value);
static void mn_conf_link_radio_action_h (GtkRadioAction *action,
					 GtkRadioAction *current,
					 gpointer user_data);
static void mn_conf_link_radio_action_notify_cb (GConfClient *client,
						 unsigned int cnxn_id,
						 GConfEntry *entry,
						 gpointer user_data);
static void mn_conf_link_radio_action_free_info (LinkRadioActionInfo *info);

static void mn_conf_link_toggle_action_h (GtkToggleAction *action,
					  gpointer user_data);
static void mn_conf_link_toggle_action_notify_cb (GConfClient *client,
						  unsigned int cnxn_id,
						  GConfEntry *entry,
						  gpointer user_data);

static void mn_conf_link_spin_button_h (GtkSpinButton *button,
					gpointer user_data);
static void mn_conf_link_spin_button_notify_cb (GConfClient *client,
						unsigned int cnxn_id,
						GConfEntry *entry,
						gpointer user_data);

static void mn_conf_link_object_set (LinkObjectInfo *info,
				     const GConfValue *value);
static void mn_conf_link_object_h (GObject *object,
				   GParamSpec *pspec,
				   gpointer user_data);
static void mn_conf_link_object_notify_cb (GConfClient *client,
					   unsigned int cnxn_id,
					   GConfEntry *entry,
					   gpointer user_data);

static void mn_conf_startup_client_free (StartupClient *client);
static void mn_conf_startup_clients_free (GSList *list);

static int mn_conf_startup_client_compare (gconstpointer a, gconstpointer b);

static GSList *mn_conf_startup_list_read (const char *name);
static void mn_conf_startup_list_write (GSList *list, const char *name);

static GSList *mn_conf_get_autostart_elem (GSList *list);

static void mn_conf_notification_add_weak_notify_cb (gpointer data,
						     GObject *former_object);

/*** implementation **********************************************************/

void
mn_conf_init (void)
{
  g_return_if_fail(mn_conf_dot_dir == NULL);

  /* create our dot dir if it does not already exist */

  mn_conf_dot_dir = g_build_filename(g_get_home_dir(),
				     GNOME_DOT_GNOME,
				     "mail-notification",
				     NULL);

  if (! g_file_test(mn_conf_dot_dir, G_FILE_TEST_IS_DIR))
    {
      if (mkdir(mn_conf_dot_dir, 0755) < 0)
	mn_error_dialog(NULL,
			NULL,
			NULL,
			_("A directory creation error has occurred"),
			_("Unable to create directory \"%s\": %s."),
			mn_conf_dot_dir,
			g_strerror(errno));
    }

  /* monitor our namespace */

  eel_gconf_monitor_add(MN_CONF_NAMESPACE);

  /* import obsolete keys */

  mn_conf_handle_obsolete_key(MN_CONF_OBSOLETE_PREFERENCES_DIALOG "/height",
			      MN_CONF_PROPERTIES_DIALOG "/height");
  mn_conf_handle_obsolete_key(MN_CONF_OBSOLETE_PREFERENCES_DIALOG "/width",
			      MN_CONF_PROPERTIES_DIALOG "/width");

  if (! mn_conf_is_set(MN_CONF_MAIL_SUMMARY_POPUP_FONTS_ASPECT_SOURCE)
      && mn_conf_is_set(MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_TITLE_ENABLED)
      && mn_conf_is_set(MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_CONTENTS_ENABLED)
      && eel_gconf_get_boolean(MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_TITLE_ENABLED)
      && eel_gconf_get_boolean(MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_CONTENTS_ENABLED))
    {
      GEnumClass *enum_class;
      GEnumValue *enum_value;

      enum_class = g_type_class_ref(MN_TYPE_ASPECT_SOURCE);
      enum_value = g_enum_get_value(enum_class, MN_ASPECT_SOURCE_CUSTOM);
      g_return_if_fail(enum_value != NULL);
      
      eel_gconf_set_string(MN_CONF_MAIL_SUMMARY_POPUP_FONTS_ASPECT_SOURCE, enum_value->value_nick);
      g_type_class_unref(enum_class);
    }

  mn_conf_handle_obsolete_key(MN_CONF_OBSOLETE_DOUBLE_CLICK_ACTION,
			      MN_CONF_DOUBLE_CLICK_ACTION_2);
}

static void
mn_conf_handle_obsolete_key (const char *obsolete, const char *new)
{
  g_return_if_fail(obsolete != NULL);
  g_return_if_fail(new != NULL);

  if (! mn_conf_is_set(new) && mn_conf_is_set(obsolete))
    {
      GConfValue *value;

      value = eel_gconf_get_value(obsolete);
      if (value)
	{
	  mn_conf_set_value(new, value);
	  gconf_value_free(value);
	}
    }
}

void
mn_conf_unset_obsolete (void)
{
  const char *obsolete[] = {
    MN_CONF_OBSOLETE_LOCAL_NAMESPACE,
    MN_CONF_OBSOLETE_REMOTE_NAMESPACE,
    MN_CONF_OBSOLETE_COMMANDS_CLICKED_NAMESPACE,
    MN_CONF_OBSOLETE_COMMANDS_DOUBLE_CLICKED_NAMESPACE,
    MN_CONF_OBSOLETE_PREFERENCES_DIALOG,
    MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_TITLE_ENABLED,
    MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_CONTENTS_ENABLED,
    MN_CONF_OBSOLETE_SUMMARY_DIALOG,
    MN_CONF_OBSOLETE_DOUBLE_CLICK_ACTION
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

gboolean
mn_conf_is_set (const char *key)
{
  GConfValue *value;
  GConfClient *client;
  GError *err = NULL;
  gboolean set = FALSE;

  g_return_val_if_fail(key != NULL, FALSE);

  client = eel_gconf_client_get_global();
  g_return_val_if_fail(client != NULL, FALSE);

  value = gconf_client_get_without_default(client, key, &err);
  if (value)
    {
      set = TRUE;
      gconf_value_free(value);
    }

  if (eel_gconf_handle_error(&err))
    set = FALSE;

  return set;
}

void
mn_conf_set_value (const char *key, const GConfValue *value)
{
  GConfClient *client;
  GError *err = NULL;

  g_return_if_fail(key != NULL);
  g_return_if_fail(value != NULL);
  
  client = eel_gconf_client_get_global();
  g_return_if_fail(client != NULL);

  gconf_client_set(client, key, value, &err);
  eel_gconf_handle_error(&err);
}

void
mn_conf_link (gpointer object, ...)
{
  va_list args;

  va_start(args, object);
  while (object)
    {
      const char *key;
      const char *signal_name;
      gpointer free_me = NULL;
      GCallback signal_handler;
      GConfClientNotifyFunc notification_cb;
      LinkInfo *info = NULL;

      key = va_arg(args, const char *);
      g_return_if_fail(key != NULL);

      /* the order of these tests is important */
      if (GTK_IS_WINDOW(object))
	{
	  LinkWindowInfo *window_info;
	  
	  window_info = g_new0(LinkWindowInfo, 1);
	  window_info->width_key = g_strdup_printf("%s/width", key);
	  window_info->height_key = g_strdup_printf("%s/height", key);
	  info = LINK_INFO(window_info);
	  info->finalize = (GDestroyNotify) mn_conf_link_window_free_info;

	  gtk_window_set_default_size(object,
				      eel_gconf_get_integer(window_info->width_key),
				      eel_gconf_get_integer(window_info->height_key));

	  signal_name = "configure-event";
	  signal_handler = G_CALLBACK(mn_conf_link_window_h);
	  notification_cb = mn_conf_link_window_notify_cb;
	}
      else if (GTK_IS_RADIO_ACTION(object))
	{
	  LinkRadioActionInfo *radio_action_info;
	  GType enum_type;
	  GConfValue *value;

	  enum_type = va_arg(args, GType);
	  g_return_if_fail(enum_type != 0);

	  radio_action_info = g_new0(LinkRadioActionInfo, 1);
	  radio_action_info->enum_class = g_type_class_ref(enum_type);
	  info = LINK_INFO(radio_action_info);
	  info->object = object;
	  info->finalize = (GDestroyNotify) mn_conf_link_radio_action_free_info;

	  value = eel_gconf_get_value(key);
	  mn_conf_link_radio_action_set(radio_action_info, value);
	  if (value)
	    gconf_value_free(value);

	  signal_name = g_strdup("changed");
	  signal_handler = G_CALLBACK(mn_conf_link_radio_action_h);
	  notification_cb = mn_conf_link_radio_action_notify_cb;
	}
      else if (GTK_IS_TOGGLE_ACTION(object))
	{
	  gtk_toggle_action_set_active(object, eel_gconf_get_boolean(key));
	  
	  signal_name = g_strdup("toggled");
	  signal_handler = G_CALLBACK(mn_conf_link_toggle_action_h);
	  notification_cb = mn_conf_link_toggle_action_notify_cb;
	}
      else if (GTK_IS_SPIN_BUTTON(object))
	{
	  gtk_spin_button_set_value(object, eel_gconf_get_integer(key));

	  signal_name = "value-changed";
	  signal_handler = G_CALLBACK(mn_conf_link_spin_button_h);
	  notification_cb = mn_conf_link_spin_button_notify_cb;
	}
      else if (G_IS_OBJECT(object))
	{
	  LinkObjectInfo *object_info;
	  const char *property_name;
	  GConfValue *value;

	  property_name = va_arg(args, const char *);
	  g_return_if_fail(property_name != NULL);

	  object_info = g_new0(LinkObjectInfo, 1);
	  object_info->pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(object), property_name);
	  g_return_if_fail(object_info->pspec != NULL);

	  info = LINK_INFO(object_info);
	  info->object = object;

	  value = eel_gconf_get_value(key);
	  mn_conf_link_object_set(object_info, value);
	  if (value)
	    gconf_value_free(value);

	  signal_name = free_me = g_strconcat("notify::", property_name, NULL);
	  signal_handler = G_CALLBACK(mn_conf_link_object_h);
	  notification_cb = mn_conf_link_object_notify_cb;
	}
      else
	g_return_if_reached();

      if (! info)
	info = g_new0(LinkInfo, 1);

      info->object = object;
      g_free(info->key);
      info->key = g_strdup(key);
      info->handler_id = g_signal_connect(object, signal_name, signal_handler, info);
      mn_conf_notification_add(object, key, notification_cb, info);
      g_object_weak_ref(object, mn_conf_link_weak_notify_cb, info);

      g_free(free_me);
      object = va_arg(args, gpointer);
    }
  va_end(args);
}

static void
mn_conf_link_weak_notify_cb (gpointer data, GObject *former_object)
{
  LinkInfo *info = data;

  if (info->finalize)
    info->finalize(info);
  g_free(info);
}

void
mn_conf_link_combo_box_to_string (GtkComboBox *combo,
				  int string_column,
				  const char *key)
{
  LinkComboBoxToStringInfo *info;
  GConfValue *value;

  g_return_if_fail(GTK_IS_COMBO_BOX(combo));
  g_return_if_fail(key != NULL);

  info = g_new0(LinkComboBoxToStringInfo, 1);
  info->string_column = string_column;
  LINK_INFO(info)->object = combo;
  LINK_INFO(info)->key = g_strdup(key);

  value = eel_gconf_get_value(key);
  mn_conf_link_combo_box_to_string_set(info, value);
  if (value)
    eel_gconf_value_free(value);

  LINK_INFO(info)->handler_id = g_signal_connect(combo, "changed", G_CALLBACK(mn_conf_link_combo_box_to_string_h), info);
  mn_conf_notification_add(combo, key, mn_conf_link_combo_box_to_string_notify_cb, info);
  g_object_weak_ref(G_OBJECT(combo), mn_conf_link_weak_notify_cb, info);
}

static void
mn_conf_link_combo_box_to_string_set (LinkComboBoxToStringInfo *info,
				      const GConfValue *value)
{
  const char *str;

  g_return_if_fail(info != NULL);

  str = value ? gconf_value_get_string(value) : NULL;
  if (str)
    {
      GtkTreeModel *model;
      GtkTreeIter iter;
      gboolean valid;
      
      model = gtk_combo_box_get_model(LINK_INFO(info)->object);
      valid = gtk_tree_model_get_iter_first(model, &iter);

      while (valid)
	{
	  char *this_str;
	  gboolean found;
	  
	  gtk_tree_model_get(model, &iter, info->string_column, &this_str, -1);
	  found = this_str && ! strcmp(this_str, str);
	  g_free(this_str);
	  
	  if (found)
	    {
	      gtk_combo_box_set_active_iter(LINK_INFO(info)->object, &iter);
	      break;
	    }
	  
	  valid = gtk_tree_model_iter_next(model, &iter);
	}
    }
}

static void
mn_conf_link_combo_box_to_string_h (GtkComboBox *combo, gpointer user_data)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  model = gtk_combo_box_get_model(combo);
  if (gtk_combo_box_get_active_iter(combo, &iter))
    {
      LinkComboBoxToStringInfo *info = user_data;
      char *value;

      gtk_tree_model_get(model, &iter, info->string_column, &value, -1);
      eel_gconf_set_string(LINK_INFO(info)->key, value);
      g_free(value);
    }
}

static void
mn_conf_link_combo_box_to_string_notify_cb (GConfClient *client,
					    unsigned int cnxn_id,
					    GConfEntry *entry,
					    gpointer user_data)
{
  LinkComboBoxToStringInfo *info = user_data;

  GDK_THREADS_ENTER();
  BLOCK(LINK_INFO(info));
  mn_conf_link_combo_box_to_string_set(info, gconf_entry_get_value(entry));
  UNBLOCK(LINK_INFO(info));
  GDK_THREADS_LEAVE();
}

void
mn_conf_link_radio_group_to_enum (GType enum_type,
				  const char *key,
				  ...)
{
  GEnumClass *enum_class;
  GtkRadioButton *radio;
  va_list args;

  g_return_if_fail(key != NULL);

  enum_class = g_type_class_ref(enum_type);
  g_return_if_fail(enum_class != NULL);

  va_start(args, key);

  while ((radio = va_arg(args, GtkRadioButton *)))
    {
      int value;
      GEnumValue *enum_value;

      value = va_arg(args, int);

      enum_value = g_enum_get_value(enum_class, value);
      g_return_if_fail(enum_value != NULL);

      mn_conf_link_radio_button_to_string(radio, key, enum_value->value_nick);
    }

  va_end(args);
}

static void
mn_conf_link_radio_button_to_string (GtkRadioButton *radio,
				     const char *key,
				     const char *str)
{
  LinkRadioButtonToStringInfo *info;
  char *current_str;

  g_return_if_fail(GTK_IS_RADIO_BUTTON(radio));
  g_return_if_fail(key != NULL);
  g_return_if_fail(str != NULL);

  current_str = eel_gconf_get_string(key);
  if (current_str)
    {
      if (! strcmp(current_str, str))
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
      g_free(current_str);
    }
  
  info = g_new0(LinkRadioButtonToStringInfo, 1);
  info->str = g_strdup(str);
  LINK_INFO(info)->object = radio;
  LINK_INFO(info)->key = g_strdup(key);
  LINK_INFO(info)->finalize = (GDestroyNotify) mn_conf_link_radio_button_to_string_free_info;

  LINK_INFO(info)->handler_id = g_signal_connect(radio, "toggled", G_CALLBACK(mn_conf_link_radio_button_to_string_h), info);
  mn_conf_notification_add(radio, key, mn_conf_link_radio_button_to_string_notify_cb, info);
  g_object_weak_ref(G_OBJECT(radio), mn_conf_link_weak_notify_cb, info);
}

static void
mn_conf_link_radio_button_to_string_h (GtkToggleButton *toggle,
				       gpointer user_data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle)))
    {
      LinkRadioButtonToStringInfo *info = user_data;

      eel_gconf_set_string(LINK_INFO(info)->key, info->str);
    }
}

static void
mn_conf_link_radio_button_to_string_notify_cb (GConfClient *client,
					       unsigned int cnxn_id,
					       GConfEntry *entry,
					       gpointer user_data)
{
  LinkRadioButtonToStringInfo *info = user_data;
  GConfValue *value = gconf_entry_get_value(entry);
  const char *str = value ? gconf_value_get_string(value) : NULL;

  if (str && ! strcmp(str, info->str))
    {
      GDK_THREADS_ENTER();
      BLOCK(LINK_INFO(info));
      gtk_toggle_button_set_active(LINK_INFO(info)->object, TRUE);
      UNBLOCK(LINK_INFO(info));
      GDK_THREADS_LEAVE();
    }
}

static void
mn_conf_link_radio_button_to_string_free_info (LinkRadioButtonToStringInfo *info)
{
  g_free(info->str);
}

static gboolean
mn_conf_link_window_h (GtkWidget *widget,
		       GdkEventConfigure *event,
		       gpointer user_data)
{
  LinkWindowInfo *info = user_data;

  eel_gconf_set_integer(info->width_key, event->width);
  eel_gconf_set_integer(info->height_key, event->height);

  return FALSE;
}

static void
mn_conf_link_window_notify_cb (GConfClient *client,
			       unsigned int cnxn_id,
			       GConfEntry *entry,
			       gpointer user_data)
{
  LinkWindowInfo *info = user_data;

  GDK_THREADS_ENTER();
  BLOCK(LINK_INFO(info));
  gtk_window_resize(LINK_INFO(info)->object,
		    eel_gconf_get_integer(info->width_key),
		    eel_gconf_get_integer(info->height_key));
  UNBLOCK(LINK_INFO(info));
  GDK_THREADS_LEAVE();
}

static void
mn_conf_link_window_free_info (LinkWindowInfo *info)
{
  g_free(info->width_key);
  g_free(info->height_key);
}

static void
mn_conf_link_radio_action_set (LinkRadioActionInfo *info,
			       const GConfValue *value)
{
  GEnumValue *enum_value;
  const char *nick;

  g_return_if_fail(info != NULL);

  if (! value)
    return;

  nick = gconf_value_get_string(value);
  enum_value = nick ? g_enum_get_value_by_nick(info->enum_class, nick) : NULL;

  if (enum_value)
    {
      GSList *l;

      MN_LIST_FOREACH(l, gtk_radio_action_get_group(LINK_INFO(info)->object))
        {
	  GtkRadioAction *this_action = l->data;
	  int this_value;

	  g_object_get(this_action, "value", &this_value, NULL);
	  if (this_value == enum_value->value)
	    {
	      gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(this_action), TRUE);
	      break;
	    }
	}
    }
}

static void
mn_conf_link_radio_action_h (GtkRadioAction *action,
			     GtkRadioAction *current,
			     gpointer user_data)
{
  LinkRadioActionInfo *info = user_data;
  int current_value;
  GEnumValue *enum_value;

  g_object_get(current, "value", &current_value, NULL);

  enum_value = g_enum_get_value(info->enum_class, current_value);
  g_return_if_fail(enum_value != NULL);

  eel_gconf_set_string(LINK_INFO(info)->key, enum_value->value_nick);
}

static void
mn_conf_link_radio_action_notify_cb (GConfClient *client,
				     unsigned int cnxn_id,
				     GConfEntry *entry,
				     gpointer user_data)
{
  LinkRadioActionInfo *info = user_data;

  GDK_THREADS_ENTER();
  BLOCK(LINK_INFO(info));
  mn_conf_link_radio_action_set(info, gconf_entry_get_value(entry));
  UNBLOCK(LINK_INFO(info));
  GDK_THREADS_LEAVE();
}

static void
mn_conf_link_radio_action_free_info (LinkRadioActionInfo *info)
{
  g_type_class_unref(info->enum_class);
}

static void
mn_conf_link_toggle_action_h (GtkToggleAction *action,
			      gpointer user_data)
{
  LinkInfo *info = user_data;

  eel_gconf_set_boolean(info->key, gtk_toggle_action_get_active(action));
}

static void
mn_conf_link_toggle_action_notify_cb (GConfClient *client,
				      unsigned int cnxn_id,
				      GConfEntry *entry,
				      gpointer user_data)
{
  LinkInfo *info = user_data;
  GConfValue *value = gconf_entry_get_value(entry);
  
  GDK_THREADS_ENTER();
  BLOCK(info);
  gtk_toggle_action_set_active(info->object, value ? gconf_value_get_bool(value) : FALSE);
  UNBLOCK(info);
  GDK_THREADS_LEAVE();
}

static void
mn_conf_link_spin_button_h (GtkSpinButton *button, gpointer user_data)
{
  LinkInfo *info = user_data;

  eel_gconf_set_integer(info->key, gtk_spin_button_get_value_as_int(button));
}

static void
mn_conf_link_spin_button_notify_cb (GConfClient *client,
				    unsigned int cnxn_id,
				    GConfEntry *entry,
				    gpointer user_data)
{
  GConfValue *value = gconf_entry_get_value(entry);
  LinkInfo *info = user_data;

  GDK_THREADS_ENTER();
  BLOCK(info);
  gtk_spin_button_set_value(info->object, value ? gconf_value_get_int(value) : 0);
  UNBLOCK(info);
  GDK_THREADS_LEAVE();
}

static void
mn_conf_link_object_set (LinkObjectInfo *info, const GConfValue *value)
{
  GValue gvalue = { 0, };

  g_return_if_fail(info != NULL);

  if (! value)
    return;

  g_value_init(&gvalue, G_PARAM_SPEC_VALUE_TYPE(info->pspec));
  
  if (G_PARAM_SPEC_VALUE_TYPE(info->pspec) == G_TYPE_BOOLEAN)
    g_value_set_boolean(&gvalue, gconf_value_get_bool(value));
  else if (G_PARAM_SPEC_VALUE_TYPE(info->pspec) == G_TYPE_STRING)
    g_value_set_string(&gvalue, gconf_value_get_string(value));
  else
    g_return_if_reached();

  g_object_set_property(LINK_INFO(info)->object, g_param_spec_get_name(info->pspec), &gvalue);
  g_value_unset(&gvalue);
}

static void
mn_conf_link_object_h (GObject *object, GParamSpec *pspec, gpointer user_data)
{
  LinkInfo *info = user_data;
  GValue value = { 0, };

  g_value_init(&value, G_PARAM_SPEC_VALUE_TYPE(pspec));
  g_object_get_property(object, g_param_spec_get_name(pspec), &value);

  if (G_PARAM_SPEC_VALUE_TYPE(pspec) == G_TYPE_BOOLEAN)
    eel_gconf_set_boolean(info->key, g_value_get_boolean(&value));
  else if (G_PARAM_SPEC_VALUE_TYPE(pspec) == G_TYPE_STRING)
    {
      const char *str = g_value_get_string(&value);
      eel_gconf_set_string(info->key, str ? str : "");
    }
  else
    g_return_if_reached();

  g_value_unset(&value);
}

static void
mn_conf_link_object_notify_cb (GConfClient *client,
			       unsigned int cnxn_id,
			       GConfEntry *entry,
			       gpointer user_data)
{
  LinkObjectInfo *info = user_data;

  GDK_THREADS_ENTER();
  BLOCK(LINK_INFO(info));
  mn_conf_link_object_set(info, gconf_entry_get_value(entry));
  UNBLOCK(LINK_INFO(info));
  GDK_THREADS_LEAVE();
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

int
mn_conf_get_enum_value (GType enum_type, const char *key)
{
  GEnumClass *enum_class;
  GEnumValue *enum_value = NULL;
  char *nick;

  g_return_val_if_fail(key != NULL, 0);

  enum_class = g_type_class_ref(enum_type);
  g_return_val_if_fail(enum_class != NULL, 0);

  nick = eel_gconf_get_string(key);
  if (nick)
    {
      enum_value = g_enum_get_value_by_nick(enum_class, nick);
      g_free(nick);
    }

  g_type_class_unref(enum_class);

  return enum_value ? enum_value->value : 0;
}

void
mn_conf_notification_add (gpointer object,
			  const char *key,
			  GConfClientNotifyFunc callback,
			  gpointer user_data)
{
  unsigned int notification_id;

  g_return_if_fail(G_IS_OBJECT(object));
  g_return_if_fail(key != NULL);
  g_return_if_fail(callback != NULL);

  notification_id = eel_gconf_notification_add(key, callback, user_data);
  g_object_weak_ref(G_OBJECT(object), mn_conf_notification_add_weak_notify_cb, GUINT_TO_POINTER(notification_id));
}

static void
mn_conf_notification_add_weak_notify_cb (gpointer data, GObject *former_object)
{
  unsigned int notification_id = GPOINTER_TO_UINT(data);
  eel_gconf_notification_remove(notification_id);
}

gboolean
mn_conf_has_command (const char *namespace)
{
  char *enabled_key;
  gboolean has = FALSE;

  g_return_val_if_fail(namespace != NULL, FALSE);

  enabled_key = g_strconcat(namespace, "/enabled", NULL);
  if (eel_gconf_get_boolean(enabled_key))
    {
      char *command_key;
      char *command;

      command_key = g_strconcat(namespace, "/command", NULL);
      command = eel_gconf_get_string(command_key);
      g_free(command_key);

      if (command)
	{
	  if (*command)
	    has = TRUE;
	  g_free(command);
	}
    }
  g_free(enabled_key);

  return has;
}
