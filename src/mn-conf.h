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

#ifndef _MN_CONF_H
#define _MN_CONF_H

#include <gconf/gconf-client.h>

void		mn_conf_init		(void);
void		mn_conf_deinit		(void);

gboolean	mn_conf_get_bool	(const char	*key);
int		mn_conf_get_int		(const char	*key);
char		*mn_conf_get_string	(const char	*key);
GSList		*mn_conf_get_list	(const char	*key,
					 GConfValueType	list_type);

void		mn_conf_set_bool	(const char	*key,
					 gboolean	value);
void		mn_conf_set_int		(const char	*key,
					 int		value);
void		mn_conf_set_string	(const char	*key,
					 const char	*value);
void		mn_conf_set_list	(const char	*key,
					 GConfValueType	list_type,
					 GSList		*value);
void		mn_conf_unset		(const char	*key);
void		mn_conf_remove_mailbox	(const char	*locator);

#endif /* _MN_CONF_H */
