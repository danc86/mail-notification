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
#include <eel/eel.h>

#define MN_CONF_NAMESPACE			"/apps/mail-notification"
#define MN_CONF_LOCAL_NAMESPACE			MN_CONF_NAMESPACE "/local"			/* obsolete */
#define MN_CONF_LOCAL_ENABLED			MN_CONF_LOCAL_NAMESPACE "/enabled"		/* obsolete */
#define MN_CONF_LOCAL_DELAY_MINUTES		MN_CONF_LOCAL_NAMESPACE "/delay/minutes"	/* obsolete */
#define MN_CONF_LOCAL_DELAY_SECONDS		MN_CONF_LOCAL_NAMESPACE "/delay/seconds"	/* obsolete */
#define MN_CONF_REMOTE_NAMESPACE		MN_CONF_NAMESPACE "/remote"			/* obsolete */
#define MN_CONF_REMOTE_ENABLED			MN_CONF_REMOTE_NAMESPACE "/enabled"		/* obsolete */
#define MN_CONF_REMOTE_DELAY_MINUTES		MN_CONF_REMOTE_NAMESPACE "/delay/minutes"	/* obsolete */
#define MN_CONF_REMOTE_DELAY_SECONDS		MN_CONF_REMOTE_NAMESPACE "/delay/seconds"	/* obsolete */
#define MN_CONF_DELAY_NAMESPACE			MN_CONF_NAMESPACE "/delay"
#define MN_CONF_DELAY_MINUTES			MN_CONF_DELAY_NAMESPACE "/minutes"
#define MN_CONF_DELAY_SECONDS			MN_CONF_DELAY_NAMESPACE "/seconds"
#define MN_CONF_COMMANDS_NEW_MAIL_ENABLED	MN_CONF_NAMESPACE "/commands/new-mail/enabled"
#define MN_CONF_COMMANDS_NEW_MAIL_COMMAND	MN_CONF_NAMESPACE "/commands/new-mail/command"
#define MN_CONF_COMMANDS_CLICKED_ENABLED	MN_CONF_NAMESPACE "/commands/clicked/enabled"
#define MN_CONF_COMMANDS_CLICKED_COMMAND	MN_CONF_NAMESPACE "/commands/clicked/command"
#define MN_CONF_MAILBOXES			MN_CONF_NAMESPACE "/mailboxes"
#define MN_CONF_UI_NAMESPACE			MN_CONF_NAMESPACE "/ui"
#define MN_CONF_PREFERENCES_DIALOG		MN_CONF_UI_NAMESPACE "/preferences-dialog"

void		mn_conf_init		(void);

void		mn_conf_link		(GtkWidget	*widget,
					 ...);

#endif /* _MN_CONF_H */
