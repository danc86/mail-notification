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

#ifndef _MN_MAILBOXES_H
#define _MN_MAILBOXES_H

#include <glib.h>
#include "mn-mailbox.h"

void		mn_mailboxes_register		(void);
void		mn_mailboxes_install_timeout	(void);
void		mn_mailboxes_check		(void);
GSList		*mn_mailboxes_get		(void);
MNMailbox	*mn_mailboxes_find		(const char *uri);

#endif /* _MN_MAILBOXES_H */
