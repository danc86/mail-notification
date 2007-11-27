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

#ifndef _MN_STOCK_H
#define _MN_STOCK_H

#include "config.h"

#define MN_STOCK_MAIL			"mn-mail"
#define MN_STOCK_NO_MAIL		"mn-no-mail"
#define MN_STOCK_LOCAL			"mn-local"
#define MN_STOCK_REMOTE			"mn-remote"
#define MN_STOCK_UNKNOWN		"mn-unknown"
#define MN_STOCK_ERROR			"mn-error"
#if WITH_GMAIL
#define MN_STOCK_GMAIL			"mn-gmail"
#endif
#if WITH_YAHOO
#define MN_STOCK_YAHOO			"mn-yahoo"
#endif
#if WITH_HOTMAIL
#define MN_STOCK_HOTMAIL		"mn-hotmail"
#endif
#if WITH_MBOX || WITH_MOZILLA || WITH_MH || WITH_MAILDIR || WITH_SYLPHEED
#define MN_STOCK_SYSTEM_MAILBOX		"mn-system-mailbox"
#endif
#if WITH_EVOLUTION
#define MN_STOCK_EVOLUTION_MAILBOX	"mn-evolution-mailbox"
#endif
#define MN_STOCK_MAIL_READER		"mn-mail-reader"
#define MN_STOCK_OPEN_MESSAGE		"mn-open-message"
#define MN_STOCK_CONSIDER_NEW_MAIL_AS_READ	"mn-consider-new-mail-as-read"

void mn_stock_init (void);

#endif /* _MN_STOCK_H */
