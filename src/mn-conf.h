/*
 * Mail Notification
 * Copyright (C) 2003-2007 Jean-Yves Lefort <jylefort@brutele.be>
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _MN_CONF_H
#define _MN_CONF_H

#include <stdarg.h>
#include <gtk/gtk.h>
#include <eel/eel.h>
#include "mn-util.h"

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
#define MN_CONF_ALWAYS_DISPLAY_ICON \
  MN_CONF_NAMESPACE "/always-display-icon"
#define MN_CONF_CLICK_ACTION \
  MN_CONF_NAMESPACE "/click-action-2"
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
#define MN_CONF_POPUPS_EXPIRATION_DELAY_NAMESPACE \
  MN_CONF_POPUPS_EXPIRATION_NAMESPACE "/delay"
#define MN_CONF_POPUPS_EXPIRATION_DELAY_MINUTES \
  MN_CONF_POPUPS_EXPIRATION_DELAY_NAMESPACE "/minutes"
#define MN_CONF_POPUPS_EXPIRATION_DELAY_SECONDS \
  MN_CONF_POPUPS_EXPIRATION_DELAY_NAMESPACE "/seconds"
#define MN_CONF_POPUPS_ACTIONS \
  MN_CONF_POPUPS_NAMESPACE "/actions"

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

extern const char *mn_conf_dot_dir;

void		mn_conf_init		(void);
void		mn_conf_unset_obsolete	(void);

void		mn_conf_recursive_unset	(const char	*key,
					 GConfUnsetFlags flags);

gboolean	mn_conf_is_set		(const char	*key);

void		mn_conf_set_value	(const char	*key,
					 const GConfValue *value);

void		mn_conf_link		(gpointer	object,
					 ...);
void		mn_conf_link_radio_group_to_enum (GType		enum_type,
						  const char	*key,
						  ...);

int		mn_conf_get_enum_value	(GType		enum_type,
					 const char	*key);

unsigned int	mn_conf_notification_add_full	(const char		*key,
						 GConfClientNotifyFunc	callback,
						 gpointer		user_data,
						 GFreeFunc		destroy_notify);

gboolean	mn_conf_has_command		(const char	*namespace);
void		mn_conf_execute_command		(const char	*conf_key);
void		mn_conf_execute_mail_reader	(void);

int		mn_conf_get_milliseconds (const char	*minutes_key,
					  const char	*seconds_key);

#endif /* _MN_CONF_H */
