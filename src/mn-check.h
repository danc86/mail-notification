/* 
 * Copyright (c) 2003 Jean-Yves Lefort <jylefort@brutele.be>
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

#ifndef _MN_CHECK_H
#define _MN_CHECK_H

#include <glib.h>

#define MN_CHECK_REMOTE		(1)		/* check remote mailboxes */
#define MN_CHECK_INTERACTIVE	(1 << 1)	/* called from the GUI */

void mn_check		(gboolean local);
void mn_check_install	(void);

#endif /* _MN_CHECK_H */
