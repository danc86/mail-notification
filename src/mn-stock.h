/* 
 * Copyright (c) 2004 Jean-Yves Lefort <jylefort@brutele.be>
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

#define MN_STOCK_MAIL			"mail-notification-mail"
#define MN_STOCK_LOCAL			"mail-notification-local"
#define MN_STOCK_REMOTE			"mail-notification-remote"
#define MN_STOCK_UNSUPPORTED		"mail-notification-unsupported"
#ifdef WITH_GMAIL
#define MN_STOCK_GMAIL			"mail-notification-gmail"
#endif
#define MN_STOCK_SYSTEM_MAILBOX		"mail-notification-system-mailbox"
#define MN_STOCK_SELECT_ALL		"mail-notification-select-all"
#define MN_STOCK_MAIL_SUMMARY		"mail-notification-mail-summary"

void mn_stock_init (void);

#endif /* _MN_STOCK_H */
