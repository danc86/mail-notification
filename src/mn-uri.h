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

#ifndef _MN_URI_H
#define _MN_URI_H

#include <glib.h>

#define MN_URI_POP3_PORT(ssl)		((ssl) ? 995 : 110)
#define MN_URI_IMAP_PORT(ssl)		((ssl) ? 993 : 143)

char *mn_uri_build_pop (gboolean ssl,
			const char *username,
			const char *password,
			const char *authmech,
			const char *hostname,
			int port);
gboolean mn_uri_parse_pop (const char *uri,
			   gboolean *ssl,
			   char **username,
			   char **password,
			   char **authmech,
			   char **hostname,
			   int *port);

char *mn_uri_build_imap (gboolean ssl,
			 const char *username,
			 const char *password,
			 const char *authmech,
			 const char *hostname,
			 int port,
			 const char *mailbox);
gboolean mn_uri_parse_imap (const char *uri,
			    gboolean *ssl,
			    char **username,
			    char **password,
			    char **authmech,
			    char **hostname,
			    int *port,
			    char **mailbox);

char *mn_uri_build_gmail (const char *username, const char *password);
gboolean mn_uri_parse_gmail (const char *uri,
			     char **username,
			     char **password);

char *mn_uri_canonicalize (const char *uri);
int mn_uri_cmp (const char *uri1, const char *uri2);

char *mn_uri_format_for_display (const char *uri);

gboolean mn_uri_is_local (const char *uri);
const char *mn_uri_get_system_mailbox (void);

#endif /* _MN_URI_H */
