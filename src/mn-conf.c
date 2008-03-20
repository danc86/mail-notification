/*
 * Mail Notification
 * Copyright (C) 2003-2008 Jean-Yves Lefort <jylefort@brutele.be>
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

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <glib/gi18n.h>
#include <gnome.h>
#include "mn-util.h"
#include "mn-conf.h"
#include "mn-shell.h"
#include "mn-locked-callback.h"
#include "mn-non-linear-range.h"

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
#define MN_CONF_OBSOLETE_COMMANDS_MAIL_READER_NAMESPACE \
  MN_CONF_COMMANDS_NAMESPACE "/mail-reader"
#define MN_CONF_OBSOLETE_COMMANDS_MAIL_READER_ENABLED \
  MN_CONF_OBSOLETE_COMMANDS_MAIL_READER_NAMESPACE "/enabled"
#define MN_CONF_OBSOLETE_COMMANDS_MAIL_READER_COMMAND \
  MN_CONF_OBSOLETE_COMMANDS_MAIL_READER_NAMESPACE "/command"
#define MN_CONF_OBSOLETE_PREFERENCES_DIALOG \
  MN_CONF_UI_NAMESPACE "/preferences-dialog"
#define MN_CONF_OBSOLETE_SUMMARY_DIALOG \
  MN_CONF_UI_NAMESPACE "/summary-dialog"
#define MN_CONF_OBSOLETE_DOUBLE_CLICK_ACTION \
  MN_CONF_NAMESPACE "/double-click-action"
#define MN_CONF_OBSOLETE_ALREADY_RUN \
  MN_CONF_NAMESPACE "/already-run"
#define MN_CONF_OBSOLETE_SUMMARY_TOOLTIP \
  MN_CONF_NAMESPACE "/summary-tooltip"
#define MN_CONF_OBSOLETE_DELAY_NAMESPACE \
  MN_CONF_NAMESPACE "/delay"
#define MN_CONF_OBSOLETE_DELAY_MINUTES \
  MN_CONF_OBSOLETE_DELAY_NAMESPACE "/minutes"
#define MN_CONF_OBSOLETE_DELAY_SECONDS \
  MN_CONF_OBSOLETE_DELAY_NAMESPACE "/seconds"
#define MN_CONF_OBSOLETE_IMMEDIATE_NOTIFICATION_ERROR_DIALOG_NAMESPACE \
  MN_CONF_UI_NAMESPACE "/immediate-notification-error-dialog"
#define MN_CONF_OBSOLETE_IMMEDIATE_NOTIFICATION_ERROR_DIALOG_DO_NOT_SHOW \
  MN_CONF_OBSOLETE_IMMEDIATE_NOTIFICATION_ERROR_DIALOG_NAMESPACE "/do-not-show"
#define MN_CONF_OBSOLETE_DOUBLE_CLICK_ACTION_2 \
  MN_CONF_NAMESPACE "/double-click-action-2"
#define MN_CONF_OBSOLETE_MAIN_WINDOW_NAMESPACE \
  MN_CONF_UI_NAMESPACE "/main-window"
#define MN_CONF_OBSOLETE_MAIN_WINDOW_DIMENSIONS \
  MN_CONF_OBSOLETE_MAIN_WINDOW_NAMESPACE "/dimensions"
#define MN_CONF_OBSOLETE_MAIN_WINDOW_VIEW_TOOLBARS \
  MN_CONF_OBSOLETE_MAIN_WINDOW_NAMESPACE "/view-toolbars"
#define MN_CONF_OBSOLETE_MAIN_WINDOW_VIEW_STATUSBAR \
  MN_CONF_OBSOLETE_MAIN_WINDOW_NAMESPACE "/view-statusbar"
#define MN_CONF_OBSOLETE_MAIN_WINDOW_TOOLBARS_STYLE \
  MN_CONF_OBSOLETE_MAIN_WINDOW_NAMESPACE "/toolbars-style"
#define MN_CONF_OBSOLETE_MAIN_WINDOW_EDIT_TOOLBARS_DIALOG \
  MN_CONF_OBSOLETE_MAIN_WINDOW_NAMESPACE "/edit-toolbars-dialog"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_NAMESPACE \
  MN_CONF_NAMESPACE "/mail-summary-popup"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_ENABLE \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_NAMESPACE "/enable"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_AUTOCLOSE \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_NAMESPACE "/autoclose"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_NAMESPACE \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_NAMESPACE "/autoclose-delay"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_MINUTES \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_NAMESPACE "/minutes"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_SECONDS \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_NAMESPACE "/seconds"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_LAYOUT \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_NAMESPACE "/layout"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_POSITION \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_NAMESPACE "/position"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_OFFSET_NAMESPACE \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_NAMESPACE "/offset"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_HORIZONTAL_OFFSET \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_OFFSET_NAMESPACE "/horizontal"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_VERTICAL_OFFSET \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_OFFSET_NAMESPACE "/vertical"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_ONLY_RECENT \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_NAMESPACE "/only-recent"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_NAMESPACE \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_NAMESPACE "/fonts"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_ASPECT_SOURCE \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_NAMESPACE "/aspect-source"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_TITLE_NAMESPACE \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_NAMESPACE "/title"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_TITLE_FONT \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_TITLE_NAMESPACE "/font"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_CONTENTS_NAMESPACE \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_NAMESPACE "/contents"
#define MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_CONTENTS_FONT \
  MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_FONTS_CONTENTS_NAMESPACE "/font"
#define MN_CONF_OBSOLETE_CLICK_ACTION \
  MN_CONF_NAMESPACE "/click-action"
#define MN_CONF_OBSOLETE_CLICK_ACTION_2 \
  MN_CONF_NAMESPACE "/click-action-2"
#define MN_CONF_OBSOLETE_POPUPS_EXPIRATION_DELAY_NAMESPACE \
  MN_CONF_POPUPS_EXPIRATION_NAMESPACE "/delay"
#define MN_CONF_OBSOLETE_POPUPS_EXPIRATION_DELAY_MINUTES \
  MN_CONF_OBSOLETE_POPUPS_EXPIRATION_DELAY_NAMESPACE "/minutes"
#define MN_CONF_OBSOLETE_POPUPS_EXPIRATION_DELAY_SECONDS \
  MN_CONF_OBSOLETE_POPUPS_EXPIRATION_DELAY_NAMESPACE "/seconds"

#define BLOCK(info) \
  g_signal_handler_block((info)->object, (info)->handler_id)
#define UNBLOCK(info) \
  g_signal_handler_unblock((info)->object, (info)->handler_id)

#define LINK_INFO(ptr)			((LinkInfo *) (ptr))

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
  GParamSpec		*pspec;
} LinkObjectInfo;

const char *mn_conf_dot_dir = NULL;

#if WITH_GCONF_SANITY_CHECK
/*
 * If the GConf schemas were not installed properly, refuse to
 * proceed. This ensures that MN will not behave unexpectedly because
 * of missing default values. The --disable-gconf-sanity-check
 * configure argument disables this check and is meant to be used for
 * development only.
 */
static void
check_schemas (void)
{
  static const char *keys[] = {
    MN_CONF_COMMANDS_NEW_MAIL_ENABLED,
    MN_CONF_COMMANDS_NEW_MAIL_COMMAND,
    MN_CONF_COMMANDS_MAIL_READ_ENABLED,
    MN_CONF_COMMANDS_MAIL_READ_COMMAND,
    MN_CONF_COMMANDS_MAIL_CHANGED_ENABLED,
    MN_CONF_COMMANDS_MAIL_CHANGED_COMMAND,
    MN_CONF_SOUNDS_NEW_MAIL_ENABLED,
    MN_CONF_SOUNDS_NEW_MAIL_FILE,
    MN_CONF_SOUNDS_PLAY_COMMAND,
    MN_CONF_PROPERTIES_DIALOG "/width",
    MN_CONF_PROPERTIES_DIALOG "/height",
    MN_CONF_BLINK_ON_ERRORS,
    MN_CONF_TRUSTED_X509_CERTIFICATES,
    MN_CONF_TRUSTED_SERVERS,
    MN_CONF_DISPLAY_SEEN_MAIL,
    MN_CONF_TOOLTIP_MAIL_SUMMARY,
    MN_CONF_TOOLTIP_MAIL_SUMMARY_LIMIT,
    MN_CONF_ALWAYS_DISPLAY_ICON,
    MN_CONF_DISPLAY_MESSAGE_COUNT,
    MN_CONF_CLICK_ACTION,
    MN_CONF_POPUPS_ENABLED,
    MN_CONF_POPUPS_POSITION,
    MN_CONF_POPUPS_EXPIRATION_ENABLED,
    MN_CONF_POPUPS_EXPIRATION_DELAY,
    MN_CONF_POPUPS_ACTIONS,
    MN_CONF_POPUPS_LIMIT,
    MN_CONF_FALLBACK_CHARSETS,
    MN_CONF_POPUPS_EXPIRATION_DELAY
  };
  int i;
  GConfClient *client;
  gboolean schema_missing = FALSE;

  client = mn_conf_client_get_global();
  g_assert(client != NULL);

  for (i = 0; i < G_N_ELEMENTS(keys); i++)
    {
      GConfEntry *entry;

      entry = gconf_client_get_entry(client, keys[i], NULL, TRUE, NULL);
      if (entry)
	{
	  gboolean has_schema;

	  has_schema = gconf_entry_get_schema_name(entry) != NULL;
	  gconf_entry_unref(entry);

	  if (has_schema)
	    continue;
	}

      g_warning(_("cannot find default value of configuration key \"%s\""), keys[i]);
      schema_missing = TRUE;
    }

  if (schema_missing)
    mn_fatal_error_dialog(NULL, _("The default configuration has not been installed properly. Please check your Mail Notification installation."));
}
#endif /* WITH_GCONF_SANITY_CHECK */

static void
import_obsolete_key (const char *obsolete, const char *new)
{
  g_return_if_fail(obsolete != NULL);
  g_return_if_fail(new != NULL);

  if (! mn_conf_is_set(new) && mn_conf_is_set(obsolete))
    {
      GConfValue *value;

      value = mn_conf_get_value(obsolete);
      if (value)
	{
	  mn_conf_set_value(new, value);
	  gconf_value_free(value);
	}
    }
}

void
mn_conf_init (void)
{
  g_assert(mn_conf_dot_dir == NULL);

  mn_conf_dot_dir = g_build_filename(g_get_home_dir(),
				     GNOME_DOT_GNOME,
				     "mail-notification",
				     NULL);

  if (! g_file_test(mn_conf_dot_dir, G_FILE_TEST_IS_DIR))
    {
      if (mkdir(mn_conf_dot_dir, 0755) < 0)
	mn_show_error_dialog(NULL,
			     _("A directory creation error has occurred"),
			     _("Unable to create directory \"%s\": %s."),
			     mn_conf_dot_dir,
			     g_strerror(errno));
    }

#if WITH_GCONF_SANITY_CHECK
  check_schemas();
#endif

  mn_conf_monitor_add(MN_CONF_NAMESPACE);

  import_obsolete_key(MN_CONF_OBSOLETE_PREFERENCES_DIALOG "/height",
		      MN_CONF_PROPERTIES_DIALOG "/height");
  import_obsolete_key(MN_CONF_OBSOLETE_PREFERENCES_DIALOG "/width",
		      MN_CONF_PROPERTIES_DIALOG "/width");
  import_obsolete_key(MN_CONF_OBSOLETE_CLICK_ACTION,
		      MN_CONF_CLICK_ACTION);

  if (! mn_conf_is_set(MN_CONF_TOOLTIP_MAIL_SUMMARY)
      && mn_conf_is_set(MN_CONF_OBSOLETE_SUMMARY_TOOLTIP))
    {
      if (! mn_conf_get_bool(MN_CONF_OBSOLETE_SUMMARY_TOOLTIP))
	mn_conf_set_string(MN_CONF_TOOLTIP_MAIL_SUMMARY,
			   mn_enum_get_value_nick(MN_TYPE_SHELL_TOOLTIP_MAIL_SUMMARY,
						  MN_SHELL_TOOLTIP_MAIL_SUMMARY_NONE));
    }

  if (! mn_conf_is_set(MN_CONF_POPUPS_EXPIRATION_DELAY)
      && (mn_conf_is_set(MN_CONF_OBSOLETE_POPUPS_EXPIRATION_DELAY_MINUTES)
	  || mn_conf_is_set(MN_CONF_OBSOLETE_POPUPS_EXPIRATION_DELAY_SECONDS)))
    {
      int minutes;
      int seconds;

      minutes = mn_conf_get_int(MN_CONF_OBSOLETE_POPUPS_EXPIRATION_DELAY_MINUTES);
      seconds = mn_conf_get_int(MN_CONF_OBSOLETE_POPUPS_EXPIRATION_DELAY_SECONDS);

      mn_conf_set_int(MN_CONF_POPUPS_EXPIRATION_DELAY, minutes * 60 + seconds);
    }
}

void
mn_conf_unset_obsolete (void)
{
  static const char *obsolete[] = {
    MN_CONF_OBSOLETE_LOCAL_NAMESPACE,
    MN_CONF_OBSOLETE_REMOTE_NAMESPACE,
    MN_CONF_OBSOLETE_COMMANDS_CLICKED_NAMESPACE,
    MN_CONF_OBSOLETE_COMMANDS_DOUBLE_CLICKED_NAMESPACE,
    MN_CONF_OBSOLETE_COMMANDS_MAIL_READER_NAMESPACE,
    MN_CONF_OBSOLETE_PREFERENCES_DIALOG,
    MN_CONF_OBSOLETE_SUMMARY_DIALOG,
    MN_CONF_OBSOLETE_DOUBLE_CLICK_ACTION,
    MN_CONF_OBSOLETE_MAILBOXES,
    MN_CONF_OBSOLETE_ALREADY_RUN,
    MN_CONF_OBSOLETE_SUMMARY_TOOLTIP,
    MN_CONF_OBSOLETE_DELAY_NAMESPACE,
    MN_CONF_OBSOLETE_IMMEDIATE_NOTIFICATION_ERROR_DIALOG_NAMESPACE,
    MN_CONF_OBSOLETE_DOUBLE_CLICK_ACTION_2,
    MN_CONF_OBSOLETE_MAIN_WINDOW_NAMESPACE,
    MN_CONF_OBSOLETE_MAIL_SUMMARY_POPUP_NAMESPACE,
    MN_CONF_OBSOLETE_CLICK_ACTION,
    MN_CONF_OBSOLETE_CLICK_ACTION_2,
    MN_CONF_OBSOLETE_POPUPS_EXPIRATION_DELAY_NAMESPACE
  };
  int i;

  for (i = 0; i < G_N_ELEMENTS(obsolete); i++)
    {
      g_message(_("recursively unsetting %s"), obsolete[i]);
      mn_conf_recursive_unset(obsolete[i], GCONF_UNSET_INCLUDING_SCHEMA_NAMES);
    }

  g_message(_("syncing the GConf database"));
  mn_conf_suggest_sync();

  g_message(_("completed"));
}

GConfClient *
mn_conf_get_client (void)
{
  static GConfClient *client = NULL;

  /*
   * Does not need to be thread-safe since it is always called early
   * in the main thread.
   */

  if (! client)
    {
      client = gconf_client_get_default();
      g_assert(client != NULL);
    }

  return client;
}

static void
handle_error (GError **err)
{
  if (*err)
    mn_show_fatal_error_dialog(NULL, "A GConf error has occurred: %s.", (*err)->message);
}

GConfValue *
mn_conf_get_value (const char *key)
{
  GConfValue *v;
  GError *err = NULL;

  g_return_val_if_fail(key != NULL, NULL);

  v = gconf_client_get(mn_conf_get_client(), key, &err);
  handle_error(&err);

  return v;
}

void
mn_conf_set_value (const char *key, const GConfValue *value)
{
  GError *err = NULL;

  g_return_if_fail(key != NULL);
  g_return_if_fail(value != NULL);

  gconf_client_set(mn_conf_get_client(), key, value, &err);
  handle_error(&err);
}

#define CONF_GETTER(name, ctype, fail_retval)				\
  ctype									\
  mn_conf_get_ ## name (const char *key)				\
  {									\
    ctype v;								\
    GError *err = NULL;							\
									\
    g_return_val_if_fail(key != NULL, (fail_retval));			\
									\
    v = gconf_client_get_ ## name(mn_conf_get_client(), key, &err);	\
    handle_error(&err);							\
									\
    return v;								\
  }

#define CONF_SETTER(name, ctype)					\
  void									\
  mn_conf_set_ ## name (const char *key, ctype value)			\
  {									\
    GError *err = NULL;							\
									\
    g_return_if_fail(key != NULL);					\
									\
    gconf_client_set_ ## name(mn_conf_get_client(), key, value, &err);	\
    handle_error(&err);							\
  }

#define CONF_ACCESSORS(name, get_ctype, set_ctype, fail_retval) 	\
  CONF_GETTER(name, get_ctype, fail_retval)				\
  CONF_SETTER(name, set_ctype)

CONF_ACCESSORS(bool, gboolean, gboolean, FALSE)
CONF_ACCESSORS(int, int, int, 0)
CONF_ACCESSORS(string, char *, const char *, NULL)

GSList *
mn_conf_get_string_list (const char *key)
{
  GSList *v;
  GError *err = NULL;

  g_return_val_if_fail(key != NULL, NULL);

  v = gconf_client_get_list(mn_conf_get_client(), key, GCONF_VALUE_STRING, &err);
  handle_error(&err);

  return v;
}

void
mn_conf_set_string_list (const char *key, GSList *list)
{
  GError *err = NULL;

  g_return_if_fail(key != NULL);

  gconf_client_set_list(mn_conf_get_client(), key, GCONF_VALUE_STRING, list, &err);
  handle_error(&err);
}

void
mn_conf_suggest_sync (void)
{
  GError *err = NULL;

  gconf_client_suggest_sync(mn_conf_get_client(), &err);
  handle_error(&err);
}

void
mn_conf_recursive_unset (const char *key, GConfUnsetFlags flags)
{
  GError *err = NULL;

  g_return_if_fail(key != NULL);

  gconf_client_recursive_unset(mn_conf_get_client(), key, flags, &err);
  handle_error(&err);
}

gboolean
mn_conf_is_set (const char *key)
{
  GConfValue *value;
  GError *err = NULL;
  gboolean set = FALSE;

  g_return_val_if_fail(key != NULL, FALSE);

  value = gconf_client_get_without_default(mn_conf_get_client(), key, &err);
  if (value)
    {
      set = TRUE;
      gconf_value_free(value);
    }

  if (err)
    {
      g_error_free(err);
      set = FALSE;
    }

  return set;
}

void
mn_conf_monitor_add (const char *directory)
{
  GError *err = NULL;

  gconf_client_add_dir(mn_conf_get_client(), directory, GCONF_CLIENT_PRELOAD_NONE, &err);
  handle_error(&err);
}

unsigned int
mn_conf_notification_add (const char *key,
			  GConfClientNotifyFunc callback,
			  gpointer user_data,
			  GFreeFunc destroy_notify)
{
  unsigned int id;
  GError *err = NULL;

  g_return_val_if_fail(key != NULL, 0);
  g_return_val_if_fail(callback != NULL, 0);

  id = gconf_client_notify_add(mn_conf_get_client(), key, callback, user_data, destroy_notify, &err);
  handle_error(&err);

  return id;
}

void
mn_conf_notification_remove (unsigned int notification_id)
{
  gconf_client_notify_remove(mn_conf_get_client(), notification_id);
}

static void
link_weak_notify_cb (gpointer data, GObject *former_object)
{
  LinkInfo *info = data;

  if (info->finalize)
    info->finalize(info);
  g_free(info);
}

static void
link_real (LinkInfo *info,
	   gpointer object,
	   const char *key,
	   const char *signal_name,
	   GCallback signal_handler,
	   GConfClientNotifyFunc notification_cb)
{
  g_return_if_fail(G_IS_OBJECT(object));
  g_return_if_fail(key != NULL);
  g_return_if_fail(signal_name != NULL);
  g_return_if_fail(signal_handler != NULL);
  g_return_if_fail(notification_cb != NULL);

  if (info)
    {
      g_return_if_fail(info->object == NULL);
      g_return_if_fail(info->key == NULL);
      g_return_if_fail(info->handler_id == 0);
    }
  else
    info = g_new0(LinkInfo, 1);

  info->object = object;
  info->key = g_strdup(key);
  info->handler_id = g_signal_connect(object, signal_name, signal_handler, info);
  mn_g_object_gconf_notification_add_gdk_locked(object, key, notification_cb, info);
  g_object_weak_ref(object, link_weak_notify_cb, info);
}

static void
link_object_set (gpointer object, GParamSpec *pspec, const GConfValue *value)
{
  GValue gvalue = { 0, };

  g_return_if_fail(G_IS_OBJECT(object));
  g_return_if_fail(pspec != NULL);

  if (! value)
    return;

  g_value_init(&gvalue, G_PARAM_SPEC_VALUE_TYPE(pspec));

  if (G_PARAM_SPEC_VALUE_TYPE(pspec) == G_TYPE_BOOLEAN)
    g_value_set_boolean(&gvalue, gconf_value_get_bool(value));
  else if (G_PARAM_SPEC_VALUE_TYPE(pspec) == G_TYPE_STRING)
    g_value_set_string(&gvalue, gconf_value_get_string(value));
  else
    g_return_if_reached();

  g_object_set_property(object, g_param_spec_get_name(pspec), &gvalue);
  g_value_unset(&gvalue);
}

static void
link_object_h (GObject *object, GParamSpec *pspec, gpointer user_data)
{
  LinkInfo *info = user_data;
  GValue value = { 0, };

  g_value_init(&value, G_PARAM_SPEC_VALUE_TYPE(pspec));
  g_object_get_property(object, g_param_spec_get_name(pspec), &value);

  if (G_PARAM_SPEC_VALUE_TYPE(pspec) == G_TYPE_BOOLEAN)
    mn_conf_set_bool(info->key, g_value_get_boolean(&value));
  else if (G_PARAM_SPEC_VALUE_TYPE(pspec) == G_TYPE_STRING)
    {
      const char *str = g_value_get_string(&value);
      mn_conf_set_string(info->key, str ? str : "");
    }
  else
    g_return_if_reached();

  g_value_unset(&value);
}

static void
link_object_notify_cb (GConfClient *client,
		       unsigned int cnxn_id,
		       GConfEntry *entry,
		       gpointer user_data)
{
  LinkObjectInfo *info = user_data;

  BLOCK(LINK_INFO(info));
  link_object_set(LINK_INFO(info)->object, info->pspec, gconf_entry_get_value(entry));
  UNBLOCK(LINK_INFO(info));
}

void
mn_conf_link_object (gpointer object,
		     const char *key,
		     const char *property_name)
{
  LinkObjectInfo *info;
  GConfValue *value;
  char *signal_name;

  g_return_if_fail(G_IS_OBJECT(object));
  g_return_if_fail(key != NULL);
  g_return_if_fail(property_name != NULL);

  info = g_new0(LinkObjectInfo, 1);
  info->pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(object), property_name);
  g_return_if_fail(info->pspec != NULL);

  value = mn_conf_get_value(key);
  link_object_set(object, info->pspec, value);
  if (value)
    gconf_value_free(value);

  signal_name = g_strconcat("notify::", property_name, NULL);

  link_real(LINK_INFO(info), object, key,
	    signal_name,
	    G_CALLBACK(link_object_h),
	    link_object_notify_cb);

  g_free(signal_name);
}

static gboolean
link_window_h (GtkWidget *widget, GdkEventConfigure *event, gpointer user_data)
{
  LinkWindowInfo *info = user_data;

  mn_conf_set_int(info->width_key, event->width);
  mn_conf_set_int(info->height_key, event->height);

  return FALSE;
}

static void
link_window_notify_cb (GConfClient *client,
		       unsigned int cnxn_id,
		       GConfEntry *entry,
		       gpointer user_data)
{
  LinkWindowInfo *info = user_data;

  BLOCK(LINK_INFO(info));
  gtk_window_resize(LINK_INFO(info)->object,
		    mn_conf_get_int(info->width_key),
		    mn_conf_get_int(info->height_key));
  UNBLOCK(LINK_INFO(info));
}

static void
link_window_free_info (LinkWindowInfo *info)
{
  g_free(info->width_key);
  g_free(info->height_key);
}

void
mn_conf_link_window (GtkWindow *window, const char *key)
{
  LinkWindowInfo *window_info;
  LinkInfo *info;

  g_return_if_fail(GTK_IS_WINDOW(window));
  g_return_if_fail(key != NULL);

  window_info = g_new0(LinkWindowInfo, 1);
  window_info->width_key = g_strdup_printf("%s/width", key);
  window_info->height_key = g_strdup_printf("%s/height", key);

  info = LINK_INFO(window_info);
  info->finalize = (GDestroyNotify) link_window_free_info;

  gtk_window_set_default_size(window,
			      mn_conf_get_int(window_info->width_key),
			      mn_conf_get_int(window_info->height_key));

  link_real(info, window, key,
	    "configure-event",
	    G_CALLBACK(link_window_h),
	    link_window_notify_cb);
}

static void
link_non_linear_range_h (GtkRange *range, gpointer user_data)
{
  LinkInfo *info = user_data;

  mn_conf_set_int(info->key, mn_non_linear_range_get_value(range));
}

static void
link_non_linear_range_notify_cb (GConfClient *client,
				 unsigned int cnxn_id,
				 GConfEntry *entry,
				 gpointer user_data)
{
  GConfValue *value = gconf_entry_get_value(entry);
  LinkInfo *info = user_data;

  BLOCK(info);
  mn_non_linear_range_set_value(info->object, value ? gconf_value_get_int(value) : 0);
  UNBLOCK(info);
}

void
mn_conf_link_non_linear_range (GtkRange *range, const char *key)
{
  g_return_if_fail(mn_is_non_linear_range(range));
  g_return_if_fail(key != NULL);

  mn_non_linear_range_set_value(range, mn_conf_get_int(key));

  link_real(NULL, range, key,
	    "value-changed",
	    G_CALLBACK(link_non_linear_range_h),
	    link_non_linear_range_notify_cb);
}

static void
link_radio_button_to_string_h (GtkToggleButton *toggle, gpointer user_data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle)))
    {
      LinkRadioButtonToStringInfo *info = user_data;

      mn_conf_set_string(LINK_INFO(info)->key, info->str);
    }
}

static void
link_radio_button_to_string_notify_cb (GConfClient *client,
				       unsigned int cnxn_id,
				       GConfEntry *entry,
				       gpointer user_data)
{
  LinkRadioButtonToStringInfo *info = user_data;
  GConfValue *value = gconf_entry_get_value(entry);
  const char *str = value ? gconf_value_get_string(value) : NULL;

  if (str && ! strcmp(str, info->str))
    {
      BLOCK(LINK_INFO(info));
      gtk_toggle_button_set_active(LINK_INFO(info)->object, TRUE);
      UNBLOCK(LINK_INFO(info));
    }
}

static void
link_radio_button_to_string_free_info (LinkRadioButtonToStringInfo *info)
{
  g_free(info->str);
}

static void
link_radio_button_to_string (GtkRadioButton *radio,
			     const char *key,
			     const char *str)
{
  LinkRadioButtonToStringInfo *rinfo;
  LinkInfo *info;
  char *current_str;

  g_return_if_fail(GTK_IS_RADIO_BUTTON(radio));
  g_return_if_fail(key != NULL);
  g_return_if_fail(str != NULL);

  current_str = mn_conf_get_string(key);
  if (current_str)
    {
      if (! strcmp(current_str, str))
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
      g_free(current_str);
    }

  rinfo = g_new0(LinkRadioButtonToStringInfo, 1);
  rinfo->str = g_strdup(str);

  info = LINK_INFO(rinfo);
  info->finalize = (GDestroyNotify) link_radio_button_to_string_free_info;

  link_real(info, radio, key,
	    "toggled",
	    G_CALLBACK(link_radio_button_to_string_h),
	    link_radio_button_to_string_notify_cb);
}

void
mn_conf_link_radio_group_to_enum (GType enum_type,
				  const char *key,
				  ...)
{
  GEnumClass *enum_class;
  GtkRadioButton *radio;
  va_list args;

  g_return_if_fail(G_TYPE_IS_ENUM(enum_type));
  g_return_if_fail(key != NULL);

  enum_class = g_type_class_ref(enum_type);
  g_return_if_fail(G_IS_ENUM_CLASS(enum_class));

  va_start(args, key);

  while ((radio = va_arg(args, GtkRadioButton *)))
    {
      int value;
      GEnumValue *enum_value;

      value = va_arg(args, int);

      enum_value = g_enum_get_value(enum_class, value);
      g_return_if_fail(enum_value != NULL);

      link_radio_button_to_string(radio, key, enum_value->value_nick);
    }

  va_end(args);

  g_type_class_unref(enum_class);
}

int
mn_conf_get_enum_value (GType enum_type, const char *key)
{
  GEnumClass *enum_class;
  GEnumValue *enum_value = NULL;
  char *nick;

  g_return_val_if_fail(G_TYPE_IS_ENUM(enum_type), 0);
  g_return_val_if_fail(key != NULL, 0);

  enum_class = g_type_class_ref(enum_type);
  g_return_val_if_fail(G_IS_ENUM_CLASS(enum_class), 0);

  nick = mn_conf_get_string(key);
  if (nick)
    {
      enum_value = g_enum_get_value_by_nick(enum_class, nick);
      g_free(nick);
    }

  g_type_class_unref(enum_class);

  return enum_value ? enum_value->value : 0;
}

gboolean
mn_conf_has_command (const char *namespace)
{
  char *enabled_key;
  gboolean has = FALSE;

  g_return_val_if_fail(namespace != NULL, FALSE);

  enabled_key = g_strconcat(namespace, "/enabled", NULL);
  if (mn_conf_get_bool(enabled_key))
    {
      char *command_key;
      char *command;

      command_key = g_strconcat(namespace, "/command", NULL);
      command = mn_conf_get_string(command_key);
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

void
mn_conf_execute_command (const char *conf_key)
{
  char *command;

  g_return_if_fail(conf_key != NULL);

  command = mn_conf_get_string(conf_key);
  g_return_if_fail(command != NULL && *command != 0);

  mn_execute_command(command);
  g_free(command);
}

void
mn_conf_execute_mail_reader (void)
{
  char *command;
  char *separator;
  char *program;

  command = mn_conf_get_string(MN_CONF_GNOME_MAIL_READER_COMMAND);
  g_return_if_fail(command != NULL && *command != 0);

  separator = strpbrk(command, " \t\n\r");
  program = separator
    ? g_strndup(command, separator - command)
    : g_strdup(command);
  g_free(command);

  if (! strcmp(program, "mozilla"))
    command = g_strdup_printf("%s -mail", program);
  else if (! strcmp(program, "evolution"))
    command = g_strdup_printf("%s --component=mail", program);
  else
    command = g_strdup(program);
  g_free(program);

  if (mn_conf_get_bool(MN_CONF_GNOME_MAIL_READER_NEEDS_TERMINAL))
    mn_execute_command_in_terminal(command);
  else
    mn_execute_command(command);
  g_free(command);
}
