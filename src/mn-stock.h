/* 
 * Copyright (C) 2004, 2005 Jean-Yves Lefort <jylefort@brutele.be>
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

#ifndef _MN_STOCK_H
#define _MN_STOCK_H

#include "config.h"

#define MN_STOCK_MAIL			"mn-mail"
#define MN_STOCK_LOCAL			"mn-local"
#define MN_STOCK_REMOTE			"mn-remote"
#define MN_STOCK_POLLED			"mn-polled"
#define MN_STOCK_UNKNOWN		"mn-unknown"
#define MN_STOCK_ERROR			"mn-error"
#ifdef WITH_GMAIL
#define MN_STOCK_GMAIL			"mn-gmail"
#endif
#if defined(WITH_MBOX) || defined(WITH_MH) || defined(WITH_MAILDIR) || defined(WITH_SYLPHEED)
#define MN_STOCK_SYSTEM_MAILBOX		"mn-system-mailbox"
#endif
#ifdef WITH_EVOLUTION
#define MN_STOCK_EVOLUTION_MAILBOX	"mn-evolution-mailbox"
#endif
#define MN_STOCK_SELECT_ALL		"mn-select-all"
#define MN_STOCK_MAIL_SUMMARY		"mn-mail-summary"
#define MN_STOCK_MAIL_READER		"mn-mail-reader"
#define MN_STOCK_MAIN_WINDOW		"mn-main-window"
#define MN_STOCK_LEAVE_FULLSCREEN	"mn-leave-fullscreen"
#define MN_STOCK_CONNECT		"mn-connect"

void mn_stock_init (void);

#endif /* _MN_STOCK_H */
