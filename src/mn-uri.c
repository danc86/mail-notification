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

#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <glib/gi18n-lib.h>
#include <libgnomevfs/gnome-vfs-utils.h>
#include "mn-uri.h"
#include "mn-util.h"

/*** functions ***************************************************************/

static gboolean mn_uri_is_ipv6_address (const char *hostname);

/*** implementation **********************************************************/

static gboolean
mn_uri_is_ipv6_address (const char *hostname)
{
  char dummy[16];		/* sizeof(struct in6_addr) */

  g_return_val_if_fail(hostname != NULL, FALSE);

  return inet_pton(AF_INET6, hostname, dummy) == 1;
}

char *
mn_uri_build_pop (const char *username,
		  const char *password,
		  const char *hostname,
		  int port)
{
  g_return_val_if_fail(username != NULL, NULL);
  g_return_val_if_fail(password != NULL, NULL);
  g_return_val_if_fail(hostname != NULL, NULL);

  if (mn_uri_is_ipv6_address(hostname))
    return port == 110
      ? g_strdup_printf("pop://%s:%s@[%s]", username, password, hostname)
      : g_strdup_printf("pop://%s:%s@[%s]:%i", username, password, hostname, port);
  else
    return port == 110
      ? g_strdup_printf("pop://%s:%s@%s", username, password, hostname)
      : g_strdup_printf("pop://%s:%s@%s:%i", username, password, hostname, port);
}

/*
 * Parse a POP URI.
 *
 * About compliance:
 *
 * We comply with RFC 1738, but for obvious reasons we require <user>
 * and <password>. We do not comply with RFC 2384 because it forbids
 * <password>.
 *
 * For IPv6 addresses, we comply with RFC 2732.
 */
gboolean
mn_uri_parse_pop (const char *uri,
		  char **username,
		  char **password,
		  char **hostname,
		  int *port)
{
  char *username_start;
  char *username_end;
  char *password_start;
  char *password_end;
  char *hostname_start;
  char *hostname_end;
  char *port_start = NULL;

  g_return_val_if_fail(uri != NULL, FALSE);

  if (strncmp(uri, "pop://", 6))
    return FALSE;

  username_start = (char *) uri + 6;
  username_end = strchr(username_start, ':');
  if (! username_end)
    return FALSE;

  password_start = username_end + 1;
  password_end = strchr(password_start, '@');
  if (! password_end)
    return FALSE;

  hostname_start = password_end + 1;
  if (*hostname_start == '[')
    {
      hostname_end = strchr(++hostname_start, ']');
      if (hostname_end)
	{
	  if (hostname_end[1])
	    {
	      if (hostname_end[1] != ':')
		return FALSE;
	      port_start = hostname_end + 2;
	    }
	}
      else
	return FALSE;
    }
  else
    {
      hostname_end = strchr(hostname_start, ':');
      if (hostname_end)
	port_start = hostname_end + 1;
      else
	{
	  hostname_end = strchr(hostname_start, 0);
	  g_return_val_if_fail(hostname_end != NULL, FALSE);
	}
    }

  if ((username_end - username_start <= 0)
      || (password_end - password_start <= 0)
      || (hostname_end - hostname_start <= 0)
      || (port_start && ! mn_str_isnumeric(port_start)))
    return FALSE;

  if (username)
    *username = g_strndup(username_start, username_end - username_start);
  if (password)
    *password = g_strndup(password_start, password_end - password_start);
  if (hostname)
    *hostname = g_strndup(hostname_start, hostname_end - hostname_start);
  if (port)
    *port = port_start ? atoi(port_start) : 110;

  return TRUE;
}

char *
mn_uri_build_gmail (const char *username, const char *password)
{
  g_return_val_if_fail(username != NULL, NULL);
  g_return_val_if_fail(password != NULL, NULL);

  return g_strdup_printf("gmail://%s:%s", username, password);
}

gboolean
mn_uri_parse_gmail (const char *uri, char **username, char **password)
{
  char *username_start;
  char *username_end;
  char *password_start;

  g_return_val_if_fail(uri != NULL, FALSE);

  if (strncmp(uri, "gmail://", 8))
    return FALSE;

  username_start = (char *) uri + 8;
  username_end = strchr(username_start, ':');
  if (! username_end)
    return FALSE;

  password_start = username_end + 1;
  if (! password_start[0])
    return FALSE;

  if (username_end - username_start <= 0)
    return FALSE;

  if (username)
    *username = g_strndup(username_start, username_end - username_start);
  if (password)
    *password = g_strdup(password_start);

  return TRUE;
}

char *
mn_uri_canonicalize (const char *uri)
{
  char *canonical_uri;
  char *username;
  char *password;
  char *hostname;
  int port;

  g_return_val_if_fail(uri != NULL, NULL);

  if (mn_uri_parse_pop(uri, &username, &password, &hostname, &port))
    {
      canonical_uri = mn_uri_build_pop(username, password, hostname, port);
      g_free(username);
      g_free(password);
      g_free(hostname);
    }
  else if (mn_uri_parse_gmail(uri, &username, &password))
    {
      canonical_uri = mn_uri_build_gmail(username, password);
      g_free(username);
      g_free(password);
    }
  else
    canonical_uri = gnome_vfs_make_uri_canonical(uri);

  return canonical_uri;
}

int
mn_uri_cmp (const char *uri1, const char *uri2)
{
  char *canonicalized1;
  char *canonicalized2;
  int cmp;
  
  g_return_val_if_fail(uri1 != NULL, 0);
  g_return_val_if_fail(uri2 != NULL, 0);

  canonicalized1 = mn_uri_canonicalize(uri1);
  canonicalized2 = mn_uri_canonicalize(uri2);

  cmp = strcmp(canonicalized1, canonicalized2);

  g_free(canonicalized1);
  g_free(canonicalized2);

  return cmp;
}

char *
mn_uri_format_for_display (const char *uri)
{
  char *name = NULL;
  char *path;

  g_return_val_if_fail(uri != NULL, NULL);

  path = gnome_vfs_get_local_path_from_uri(uri);
  if (path)
    {
      name = g_filename_to_utf8(path, -1, NULL, NULL, NULL);
      g_free(path);
    }
  if (! name)
    {
      GnomeVFSURI *vfs_uri;
	    
      vfs_uri = gnome_vfs_uri_new(uri);
      if (vfs_uri)
	{
	  name = gnome_vfs_uri_to_string(vfs_uri, GNOME_VFS_URI_HIDE_PASSWORD);
	  gnome_vfs_uri_unref(vfs_uri);
	}
    }
  if (! name)
    {
      char *username;
      char *hostname;

      if (mn_uri_parse_pop(uri, &username, NULL, &hostname, NULL))
	{
	  name = g_strdup_printf("%s@%s", username, hostname);
	  g_free(username);
	  g_free(hostname);
	}
    }
  if (! name)
    {
      char *username;

      if (mn_uri_parse_gmail(uri, &username, NULL))
	{
	  name = g_strdup_printf("%s@gmail.com", username);
	  g_free(username);
	}
    }
  if (! name)
    name = g_strdup(uri);	/* fallback to the URI */
  
  return name;
}

gboolean
mn_uri_is_local (const char *uri)
{
  GnomeVFSURI *vfs_uri;
  gboolean is_local = FALSE;

  g_return_val_if_fail(uri != NULL, NULL);

  vfs_uri = gnome_vfs_uri_new(uri);
  if (vfs_uri)
    {
      is_local = gnome_vfs_uri_is_local(vfs_uri);
      gnome_vfs_uri_unref(vfs_uri);
    }

  return is_local;
}
