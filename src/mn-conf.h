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

#ifndef _MN_CONF_H
#define _MN_CONF_H

#include <stdarg.h>
#include <gtk/gtk.h>
#include <eel/eel.h>

#define MN_CONF_NAMESPACE \
  "/apps/mail-notification"
#define MN_CONF_LOCAL_NAMESPACE \
  MN_CONF_NAMESPACE "/local"			/* obsolete */
#define MN_CONF_LOCAL_ENABLED \
  MN_CONF_LOCAL_NAMESPACE "/enabled"		/* obsolete */
#define MN_CONF_LOCAL_DELAY_MINUTES \
  MN_CONF_LOCAL_NAMESPACE "/delay/minutes"	/* obsolete */
#define MN_CONF_LOCAL_DELAY_SECONDS \
  MN_CONF_LOCAL_NAMESPACE "/delay/seconds"	/* obsolete */
#define MN_CONF_REMOTE_NAMESPACE \
  MN_CONF_NAMESPACE "/remote"			/* obsolete */
#define MN_CONF_REMOTE_ENABLED \
  MN_CONF_REMOTE_NAMESPACE "/enabled"		/* obsolete */
#define MN_CONF_REMOTE_DELAY_MINUTES \
  MN_CONF_REMOTE_NAMESPACE "/delay/minutes"	/* obsolete */
#define MN_CONF_REMOTE_DELAY_SECONDS \
  MN_CONF_REMOTE_NAMESPACE "/delay/seconds"	/* obsolete */
#define MN_CONF_DELAY_NAMESPACE \
  MN_CONF_NAMESPACE "/delay"
#define MN_CONF_DELAY_MINUTES \
  MN_CONF_DELAY_NAMESPACE "/minutes"
#define MN_CONF_DELAY_SECONDS \
  MN_CONF_DELAY_NAMESPACE "/seconds"
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
#define MN_CONF_COMMANDS_CLICKED_NAMESPACE \
  MN_CONF_COMMANDS_NAMESPACE "/clicked"		/* obsolete */
#define MN_CONF_COMMANDS_CLICKED_ENABLED \
  MN_CONF_COMMANDS_CLICKED_NAMESPACE "/enabled"	/* obsolete */
#define MN_CONF_COMMANDS_CLICKED_COMMAND \
  MN_CONF_COMMANDS_CLICKED_NAMESPACE "/command"	/* obsolete */
#define MN_CONF_COMMANDS_DOUBLE_CLICKED_NAMESPACE \
  MN_CONF_COMMANDS_NAMESPACE "/double-clicked"		/* obsolete */
#define MN_CONF_COMMANDS_DOUBLE_CLICKED_ENABLED	\
  MN_CONF_COMMANDS_DOUBLE_CLICKED_NAMESPACE "/enabled"	/* obsolete */
#define MN_CONF_COMMANDS_DOUBLE_CLICKED_COMMAND	\
  MN_CONF_COMMANDS_DOUBLE_CLICKED_NAMESPACE "/command"	/* obsolete */
#define MN_CONF_MAILBOXES \
  MN_CONF_NAMESPACE "/mailboxes"
#define MN_CONF_UI_NAMESPACE \
  MN_CONF_NAMESPACE "/ui"
#define MN_CONF_PREFERENCES_DIALOG \
  MN_CONF_UI_NAMESPACE "/preferences-dialog"	/* obsolete */
#define MN_CONF_PROPERTIES_DIALOG \
  MN_CONF_UI_NAMESPACE "/properties-dialog"
#define MN_CONF_SUMMARY_DIALOG \
  MN_CONF_UI_NAMESPACE "/summary-dialog"
#define MN_CONF_IMMEDIATE_NOTIFICATION_ERROR_DIALOG_NAMESPACE \
  MN_CONF_UI_NAMESPACE "/immediate-notification-error-dialog"
#define MN_CONF_IMMEDIATE_NOTIFICATION_ERROR_DIALOG_DO_NOT_SHOW \
  MN_CONF_IMMEDIATE_NOTIFICATION_ERROR_DIALOG_NAMESPACE "/do-not-show"
#define MN_CONF_ALREADY_RUN \
  MN_CONF_NAMESPACE "/already-run"
#define MN_CONF_BLINK_ON_ERRORS \
  MN_CONF_NAMESPACE "/blink-on-errors"
#define MN_CONF_TRUSTED_X509_CERTIFICATES \
  MN_CONF_NAMESPACE "/trusted-x509-certificates"
#define MN_CONF_TRUSTED_SERVERS \
  MN_CONF_NAMESPACE "/trusted-servers"
#define MN_CONF_MAIL_SUMMARY_POPUP_NAMESPACE \
  MN_CONF_NAMESPACE "/mail-summary-popup"
#define MN_CONF_MAIL_SUMMARY_POPUP_ENABLE \
  MN_CONF_MAIL_SUMMARY_POPUP_NAMESPACE "/enable"
#define MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE \
  MN_CONF_MAIL_SUMMARY_POPUP_NAMESPACE "/autoclose"
#define MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_NAMESPACE \
  MN_CONF_MAIL_SUMMARY_POPUP_NAMESPACE "/autoclose-delay"
#define MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_MINUTES \
  MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_NAMESPACE "/minutes"
#define MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_SECONDS \
  MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_NAMESPACE "/seconds"
#define MN_CONF_MAIL_SUMMARY_POPUP_POSITION \
  MN_CONF_MAIL_SUMMARY_POPUP_NAMESPACE "/position"
#define MN_CONF_MAIL_SUMMARY_POPUP_OFFSET_NAMESPACE \
  MN_CONF_MAIL_SUMMARY_POPUP_NAMESPACE "/offset"
#define MN_CONF_MAIL_SUMMARY_POPUP_HORIZONTAL_OFFSET \
  MN_CONF_MAIL_SUMMARY_POPUP_OFFSET_NAMESPACE "/horizontal"
#define MN_CONF_MAIL_SUMMARY_POPUP_VERTICAL_OFFSET \
  MN_CONF_MAIL_SUMMARY_POPUP_OFFSET_NAMESPACE "/vertical"

void		mn_conf_init		(void);
void		mn_conf_unset_obsolete	(void);

void		mn_conf_recursive_unset	(const char	*key,
					 GConfUnsetFlags flags);

void		mn_conf_link		(GtkWidget	*widget,
					 ...);
void		mn_conf_link_combo_box_to_string (GtkComboBox	*combo,
						  int		string_column,
						  const char	*key);

gboolean	mn_conf_get_autostart	(void);
void		mn_conf_set_autostart	(gboolean	autostart);

GEnumValue	*mn_conf_get_enum_value	(GType		enum_type,
					 const char	*key);

#endif /* _MN_CONF_H */
