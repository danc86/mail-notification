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

#ifndef _MN_CONF_H
#define _MN_CONF_H

#include <stdarg.h>
#include <gtk/gtk.h>
#include <gconf/gconf-client.h>

#define MN_CONF_NAMESPACE \
  "/apps/mail-notification"
#define MN_CONF_COMMANDS_NAMESPACE \
  MN_CONF_NAMESPACE "/commands"
#define MN_CONF_COMMANDS_NEW_MAIL_NAMESPACE \
  MN_CONF_COMMANDS_NAMESPACE "/new-mail"
#define MN_CONF_COMMANDS_NEW_MAIL_ENABLED \
  MN_CONF_COMMANDS_NEW_MAIL_NAMESPACE "/enabled"
#define MN_CONF_COMMANDS_NEW_MAIL_COMMAND \
  MN_CONF_COMMANDS_NEW_MAIL_NAMESPACE "/command"
#define MN_CONF_COMMANDS_MAIL_READ_NAMESPACE \
  MN_CONF_COMMANDS_NAMESPACE "/mail-read"
#define MN_CONF_COMMANDS_MAIL_READ_ENABLED \
  MN_CONF_COMMANDS_MAIL_READ_NAMESPACE "/enabled"
#define MN_CONF_COMMANDS_MAIL_READ_COMMAND \
  MN_CONF_COMMANDS_MAIL_READ_NAMESPACE "/command"
#define MN_CONF_COMMANDS_MAIL_CHANGED_NAMESPACE \
  MN_CONF_COMMANDS_NAMESPACE "/mail-changed"
#define MN_CONF_COMMANDS_MAIL_CHANGED_ENABLED \
  MN_CONF_COMMANDS_MAIL_CHANGED_NAMESPACE "/enabled"
#define MN_CONF_COMMANDS_MAIL_CHANGED_COMMAND \
  MN_CONF_COMMANDS_MAIL_CHANGED_NAMESPACE "/command"
#define MN_CONF_SOUNDS_NAMESPACE \
  MN_CONF_NAMESPACE "/sounds"
#define MN_CONF_SOUNDS_NEW_MAIL_NAMESPACE \
  MN_CONF_SOUNDS_NAMESPACE "/new-mail"
#define MN_CONF_SOUNDS_NEW_MAIL_ENABLED \
  MN_CONF_SOUNDS_NEW_MAIL_NAMESPACE "/enabled"
#define MN_CONF_SOUNDS_NEW_MAIL_FILE \
  MN_CONF_SOUNDS_NEW_MAIL_NAMESPACE "/file"
#define MN_CONF_SOUNDS_PLAY_COMMAND \
  MN_CONF_SOUNDS_NAMESPACE "/play-command"
#define MN_CONF_UI_NAMESPACE \
  MN_CONF_NAMESPACE "/ui"
#define MN_CONF_PROPERTIES_DIALOG \
  MN_CONF_UI_NAMESPACE "/properties-dialog"
#define MN_CONF_BLINK_ON_ERRORS \
  MN_CONF_NAMESPACE "/blink-on-errors"
#define MN_CONF_TRUSTED_X509_CERTIFICATES \
  MN_CONF_NAMESPACE "/trusted-x509-certificates"
#define MN_CONF_TRUSTED_SERVERS \
  MN_CONF_NAMESPACE "/trusted-servers"
#define MN_CONF_DISPLAY_SEEN_MAIL \
  MN_CONF_NAMESPACE "/display-seen-mail"
#define MN_CONF_TOOLTIP_MAIL_SUMMARY \
  MN_CONF_NAMESPACE "/tooltip-mail-summary"
#define MN_CONF_TOOLTIP_MAIL_SUMMARY_LIMIT \
  MN_CONF_NAMESPACE "/tooltip-mail-summary-limit"
#define MN_CONF_ALWAYS_DISPLAY_ICON \
  MN_CONF_NAMESPACE "/always-display-icon"
#define MN_CONF_DISPLAY_MESSAGE_COUNT \
  MN_CONF_NAMESPACE "/display-message-count"
#define MN_CONF_CLICK_ACTION \
  MN_CONF_NAMESPACE "/click-action-3"
#define MN_CONF_POPUPS_NAMESPACE \
  MN_CONF_NAMESPACE "/popups"
#define MN_CONF_POPUPS_ENABLED \
  MN_CONF_POPUPS_NAMESPACE "/enabled"
#define MN_CONF_POPUPS_POSITION \
  MN_CONF_POPUPS_NAMESPACE "/position"
#define MN_CONF_POPUPS_EXPIRATION_NAMESPACE \
  MN_CONF_POPUPS_NAMESPACE "/expiration"
#define MN_CONF_POPUPS_EXPIRATION_ENABLED \
  MN_CONF_POPUPS_EXPIRATION_NAMESPACE "/enabled"
#define MN_CONF_POPUPS_EXPIRATION_DELAY \
  MN_CONF_POPUPS_EXPIRATION_NAMESPACE "/delay-2"
#define MN_CONF_POPUPS_ACTIONS \
  MN_CONF_POPUPS_NAMESPACE "/actions"
#define MN_CONF_POPUPS_LIMIT \
  MN_CONF_POPUPS_NAMESPACE "/limit"
#define MN_CONF_FALLBACK_CHARSETS \
  MN_CONF_NAMESPACE "/fallback-charsets"

/* obsolete keys */
#define MN_CONF_OBSOLETE_MAILBOXES \
  MN_CONF_NAMESPACE "/mailboxes"

/* foreign keys */
#define MN_CONF_GNOME_MAIL_READER_NAMESPACE \
  "/desktop/gnome/url-handlers/mailto"
#define MN_CONF_GNOME_MAIL_READER_COMMAND \
  MN_CONF_GNOME_MAIL_READER_NAMESPACE "/command"
#define MN_CONF_GNOME_MAIL_READER_NEEDS_TERMINAL \
  MN_CONF_GNOME_MAIL_READER_NAMESPACE "/needs_terminal"
#define MN_CONF_SYSTEM_HTTP_PROXY_NAMESPACE \
  "/system/http_proxy"
#define MN_CONF_SYSTEM_HTTP_PROXY_USE_HTTP_PROXY \
  MN_CONF_SYSTEM_HTTP_PROXY_NAMESPACE "/use_http_proxy"
#define MN_CONF_SYSTEM_HTTP_PROXY_HOST \
  MN_CONF_SYSTEM_HTTP_PROXY_NAMESPACE "/host"
#define MN_CONF_SYSTEM_HTTP_PROXY_PORT \
  MN_CONF_SYSTEM_HTTP_PROXY_NAMESPACE "/port"
#define MN_CONF_SYSTEM_HTTP_PROXY_USE_AUTHENTICATION \
  MN_CONF_SYSTEM_HTTP_PROXY_NAMESPACE "/use_authentication"
#define MN_CONF_SYSTEM_HTTP_PROXY_AUTHENTICATION_USER \
  MN_CONF_SYSTEM_HTTP_PROXY_NAMESPACE "/authentication_user"
#define MN_CONF_SYSTEM_HTTP_PROXY_AUTHENTICATION_PASSWORD \
  MN_CONF_SYSTEM_HTTP_PROXY_NAMESPACE "/authentication_password"

extern const char *mn_conf_dot_dir;

void		mn_conf_init		(void);
void		mn_conf_unset_obsolete	(void);

GConfClient	*mn_conf_get_client	(void);

GConfValue	*mn_conf_get_value	(const char	*key);
void		mn_conf_set_value	(const char	*key,
					 const GConfValue *value);

gboolean	mn_conf_get_bool	(const char	*key);
void		mn_conf_set_bool	(const char	*key,
					 gboolean	value);

int		mn_conf_get_int		(const char	*key);
void		mn_conf_set_int		(const char	*key,
					 int		value);

char		*mn_conf_get_string	(const char	*key);
void		mn_conf_set_string	(const char	*key,
					 const char	*value);

GSList		*mn_conf_get_string_list	(const char	*key);
void		mn_conf_set_string_list		(const char	*key,
						 GSList		*list);

void		mn_conf_suggest_sync	(void);

void		mn_conf_recursive_unset	(const char	*key,
					 GConfUnsetFlags flags);

gboolean	mn_conf_is_set		(const char	*key);

void		mn_conf_monitor_add	(const char	*directory);

unsigned int	mn_conf_notification_add	(const char		*key,
						 GConfClientNotifyFunc	callback,
						 gpointer		user_data,
						 GFreeFunc		destroy_notify);
void		mn_conf_notification_remove	(unsigned int		notification_id);

void		mn_conf_link_object	(gpointer	object,
					 const char	*key,
					 const char	*property_name);
void		mn_conf_link_window	(GtkWindow	*window,
					 const char	*key);
void		mn_conf_link_non_linear_range	(GtkRange	*range,
						 const char	*key);
void		mn_conf_link_radio_group_to_enum (GType		enum_type,
						  const char	*key,
						  ...) G_GNUC_NULL_TERMINATED;

int		mn_conf_get_enum_value	(GType		enum_type,
					 const char	*key);

gboolean	mn_conf_has_command		(const char	*namespace);
void		mn_conf_execute_command		(const char	*conf_key);
void		mn_conf_execute_mail_reader	(void);

#endif /* _MN_CONF_H */
