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
#include <glib/gi18n-lib.h>
#include <libgnomevfs/gnome-vfs-utils.h>
#include "mn-uri.h"
#include "mn-util.h"

/*** implementation **********************************************************/

char *
mn_uri_build_pop (const char *username,
		  const char *password,
		  const char *hostname,
		  int port)
{
  g_return_val_if_fail(username != NULL, NULL);
  g_return_val_if_fail(password != NULL, NULL);
  g_return_val_if_fail(hostname != NULL, NULL);

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
 */
gboolean
mn_uri_parse_pop (const char *uri,
		  char **username,
		  char **password,
		  char **hostname,
		  int *port)
{
  const char *_username;
  char *_password;
  char *_hostname;
  char *_port = NULL;

  g_return_val_if_fail(uri != NULL, FALSE);

  if (strncmp(uri, "pop://", 6))
    return FALSE;

  _username = uri + 6;
  if (strlen(uri) < 6)
    return FALSE;

  _password = strchr(_username, ':');
  if (! _password++)		/* ++ to skip the colon */
    return FALSE;

  _hostname = strchr(_password, '@');
  if (! _hostname++)		/* ++ to skip the arobas */
    return FALSE;

  _port = strchr(_hostname, ':');
  /* port is optional */
  if (_port && ! mn_str_isnumeric(++_port))	/* ++ to skip the colon */
    return FALSE;
  
  if ((_password - _username - 1 <= 0)
      || (_hostname - _password - 1 <= 0)
      || (_port && _port - _hostname - 1 <= 0))
    return FALSE;

  if (username)
    *username = g_strndup(_username, _password - _username - 1);
  if (password)
    *password = g_strndup(_password, _hostname - _password - 1);
  if (hostname)
    *hostname = _port ? g_strndup(_hostname, _port - _hostname - 1) : g_strdup(_hostname);
  if (port)
    *port = _port ? atoi(_port) : 110;

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
      else
	name = g_strdup(uri); /* fallback to the URI */
    }

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
