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
#include <stdio.h>		/* required by stdlib.h on Darwin */
#include <stdlib.h>		/* required by sys/socket.h on Darwin */
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

static char *mn_uri_build_generic (const char *scheme,
				   const char *username,
				   const char *password,
				   const char *authmech,
				   const char *hostname,
				   int port,
				   const char *path);
static gboolean mn_uri_parse_generic (const char *uri,
				      char **scheme,
				      char **username,
				      char **password,
				      char **authmech,
				      char **hostname,
				      int *port,
				      char **path);

/*** implementation **********************************************************/

static gboolean
mn_uri_is_ipv6_address (const char *hostname)
{
  char dummy[16];		/* sizeof(struct in6_addr) */

  g_return_val_if_fail(hostname != NULL, FALSE);

  return inet_pton(AF_INET6, hostname, dummy) == 1;
}

/**
 * mn_uri_build_generic:
 * @scheme: the URI scheme
 * @username: the username, or NULL
 * @password: the password, or NULL
 * @authmech: the authentication mechanism, or NULL
 * @hostname: the hostname, or NULL
 * @port: the port number, or -1 (if specified, @hostname must not be NULL)
 * @path: the path, or NULL
 *
 * Builds a RFC 1738 URI. If @authmech is specified, a RFC 2384-style
 * notation will be used.
 *
 * If hostname is an IPv6 address, a RFC 2732-style notation will be
 * used.
 *
 * Return value: the new URI.
 **/
static char *
mn_uri_build_generic (const char *scheme,
		      const char *username,
		      const char *password,
		      const char *authmech,
		      const char *hostname,
		      int port,
		      const char *path)
{
  GString *uri;

  g_return_val_if_fail(scheme != NULL, NULL);

  uri = g_string_new(scheme);
  g_string_append(uri, "://");

  if (username)
    g_string_append(uri, username);
  if (password)
    g_string_append_printf(uri, ":%s", password);
  if (authmech)
    g_string_append_printf(uri, ";auth=%s", authmech);
  if (hostname)
    g_string_append_printf(uri, mn_uri_is_ipv6_address(hostname) ? "@[%s]" : "@%s", hostname);
  if (port >= 0)
    {
      g_return_val_if_fail(hostname != NULL, NULL);
      g_string_append_printf(uri, ":%i", port);
    }
  if (path)
    g_string_append_printf(uri, "/%s", path);

  return g_string_free(uri, FALSE);
}

/**
 * mn_uri_parse_generic:
 * @uri: the URI to parse
 * @scheme: a location to store the scheme, or NULL
 * @username: a location to store the username, or NULL
 * @password: a location to store the password, or NULL
 * @authmech: a location to store the authentication mechanism, or NULL
 * @hostname: a location to store the hostname, or NULL
 * @port: a location to store the port number, or NULL
 * @path: a location to store the path, or NULL
 *
 * Parses a RFC 1738 URI. RFC 2384-style authentication mechanism
 * notation and RFC 2732-style IPv6 address notation are supported.
 *
 * WARNING: the parsing is very lax, this function is NOT intended to
 * be used on untrusted URIs.
 *
 * Return value: TRUE is @uri has at least a scheme, FALSE otherwise.
 **/
static gboolean
mn_uri_parse_generic (const char *uri,
		      char **scheme,
		      char **username,
		      char **password,
		      char **authmech,
		      char **hostname,
		      int *port,
		      char **path)
{
  int n1;
  char scheme_buf[513];
  char auth_buf[513];
  char host_buf[513];

  g_return_val_if_fail(uri != NULL, FALSE);
  
  n1 = sscanf(uri, "%512[^:]://%512[^@]@%512s", scheme_buf, auth_buf, host_buf);
  if (n1 > 0)
    {
      char username_buf[513];
      char password_buf[513];
      char authmech_buf[513];
      char hostname_buf[513];
      int _port;
      char path_buf[513];
      gboolean has_username = FALSE;
      gboolean has_password = FALSE;
      gboolean has_authmech = FALSE;
      gboolean has_hostname = FALSE;
      gboolean has_port = FALSE;
      gboolean has_path = FALSE;

      if (n1 > 1)
	{
	  int n2;

	  n2 = sscanf(auth_buf, "%512[^:]:%512[^;];auth=%512s", username_buf, password_buf, authmech_buf);
	  has_username = n2 > 0;
	  has_password = n2 > 1;
	  has_authmech = n2 > 2;

	  if (n1 > 2)
	    {
	      int n3;
	      char hostport_buf[513];

	      n3 = sscanf(host_buf, "%512[^/]/%512s", hostport_buf, path_buf);
	      has_path = n3 > 1;

	      if (n3 > 0)
		{
		  int n4;
		  
		  n4 = sscanf(hostport_buf, "[%512[^]]]:%i", hostname_buf, &_port);
		  if (n4 == 0)
		    n4 = sscanf(hostport_buf, "%512[^:]:%i", hostname_buf, &_port);

		  has_hostname = n4 > 0;
		  has_port = n4 > 1;
		}
	    }
	}

      if (scheme)
	*scheme = g_strdup(scheme_buf);
      if (username)
	*username = has_username ? g_strdup(username_buf) : NULL;
      if (password)
	*password = has_password ? g_strdup(password_buf) : NULL;
      if (authmech)
	*authmech = has_authmech ? g_strdup(authmech_buf) : NULL;
      if (hostname)
	*hostname = has_hostname ? g_strdup(hostname_buf) : NULL;
      if (port)
	*port = has_port ? _port : -1;
      if (path)
	*path = has_path ? g_strdup(path_buf) : NULL;
      
      return TRUE;
    }
  else
    return FALSE;
}

/**
 * mn_uri_build_pop:
 * @ssl: whether to build a SSL URI or not
 * @username: the username
 * @password: the password
 * @authmech: the authentication mechanism, or NULL
 * @hostname: the hostname
 * @port: the port number, or -1
 *
 * Builds a RFC 1738 pop or pops URI. Does not conform to RFC 2384
 * because it forbids the use of a password.
 *
 * If @port is -1, the default POP3 port number for @ssl will be used.
 *
 * Return value: the new URI.
 **/
char *
mn_uri_build_pop (gboolean ssl,
		  const char *username,
		  const char *password,
		  const char *authmech,
		  const char *hostname,
		  int port)
{
  g_return_val_if_fail(username != NULL, NULL);
  g_return_val_if_fail(password != NULL, NULL);
  g_return_val_if_fail(hostname != NULL, NULL);

  return mn_uri_build_generic(ssl ? "pops" : "pop",
			      username,
			      password,
			      authmech,
			      hostname,
			      port == MN_URI_POP3_PORT(ssl) ? -1 : port,
			      NULL);
}

/**
 * mn_uri_parse_pop:
 * @uri: the URI to parse
 * @ssl: a location to store the SSL status, or NULL
 * @username: a location to store the username, or NULL
 * @password: a location to store the password, or NULL
 * @authmech: a location to store the authentication mechanism, or NULL
 * @hostname: a location to store the hostname, or NULL
 * @port: a location to store the port number, or NULL
 *
 * Parses a RFC 1738 pop or pops URI.
 *
 * WARNING: the parsing is very lax, this function is NOT intended to
 * be used on untrusted URIs.
 *
 * Return value: TRUE if @uri is a valid pop or pops URI, FALSE otherwise.
 **/
gboolean
mn_uri_parse_pop (const char *uri,
		  gboolean *ssl,
		  char **username,
		  char **password,
		  char **authmech,
		  char **hostname,
		  int *port)
{
  char *scheme;
  char *_username;
  char *_password;
  char *_authmech;
  char *_hostname;
  int _port;
  gboolean is_pop = FALSE;

  g_return_val_if_fail(uri != NULL, FALSE);

  if (mn_uri_parse_generic(uri,
			   &scheme,
			   &_username,
			   &_password,
			   &_authmech,
			   &_hostname,
			   &_port,
			   NULL))
    {
      gboolean _ssl;

      if (_hostname)
	{
	  if (! strcmp(scheme, "pop"))
	    {
	      is_pop = TRUE;
	      _ssl = FALSE;
	    }
	  else if (! strcmp(scheme, "pops"))
	    {
	      is_pop = TRUE;
	      _ssl = TRUE;
	    }
	}

      if (is_pop)
	{
	  if (ssl)
	    *ssl = _ssl;
	  if (username)
	    *username = g_strdup(_username);
	  if (password)
	    *password = g_strdup(_password);
	  if (authmech)
	    *authmech = g_strdup(_authmech);
	  if (hostname)
	    *hostname = g_strdup(_hostname);
	  if (port)
	    *port = _port >= 0 ? _port : MN_URI_POP3_PORT(_ssl);
	}

      g_free(scheme);
      g_free(_username);
      g_free(_password);
      g_free(_authmech);
      g_free(_hostname);
    }

  return is_pop;
}

/**
 * mn_uri_build_imap:
 * @ssl: whether to build a SSL URI or not
 * @username: the username
 * @password: the password
 * @authmech: the authentication mechanism, or NULL
 * @hostname: the hostname
 * @port: the port number, or -1
 * @mailbox: the mailbox name, or NULL
 *
 * Builds a RFC 1738 imap or imaps URI. Does not conform to RFC 2192
 * because it forbids the use of a password.
 *
 * If @port is -1, the default IMAP4 port number for @ssl will be used.
 *
 * If @mailbox is not specified, INBOX will be used.
 *
 * Return value: the new URI.
 **/
char *
mn_uri_build_imap (gboolean ssl,
		   const char *username,
		   const char *password,
		   const char *authmech,
		   const char *hostname,
		   int port,
		   const char *mailbox)
{
  g_return_val_if_fail(username != NULL, NULL);
  g_return_val_if_fail(password != NULL, NULL);
  g_return_val_if_fail(hostname != NULL, NULL);
  g_return_val_if_fail(mailbox != NULL, NULL);

  return mn_uri_build_generic(ssl ? "imaps" : "imap",
			      username,
			      password,
			      authmech,
			      hostname,
			      port == MN_URI_IMAP_PORT(ssl) ? -1 : port,
			      ! strcmp(mailbox, "INBOX") ? NULL : mailbox);
}

/**
 * mn_uri_parse_imap:
 * @uri: the URI to parse
 * @ssl: a location to store the SSL status, or NULL
 * @username: a location to store the username, or NULL
 * @password: a location to store the password, or NULL
 * @authmech: a location to store the authentication mechanism, or NULL
 * @hostname: a location to store the hostname, or NULL
 * @port: a location to store the port number, or NULL
 * @mailbox: a location to store the mailbox, or NULL
 *
 * Parses a RFC 1738 imap or imaps URI.
 *
 * WARNING: the parsing is very lax, this function is NOT intended to
 * be used on untrusted URIs.
 *
 * Return value: TRUE if @uri is a valid imap or imaps URI, FALSE otherwise.
 **/
gboolean
mn_uri_parse_imap (const char *uri,
		   gboolean *ssl,
		   char **username,
		   char **password,
		   char **authmech,
		   char **hostname,
		   int *port,
		   char **mailbox)
{
  char *scheme;
  char *_username;
  char *_password;
  char *_authmech;
  char *_hostname;
  int _port;
  char *_mailbox;
  gboolean is_imap = FALSE;

  g_return_val_if_fail(uri != NULL, FALSE);

  if (mn_uri_parse_generic(uri,
			   &scheme,
			   &_username,
			   &_password,
			   &_authmech,
			   &_hostname,
			   &_port,
			   &_mailbox))
    {
      gboolean _ssl;

      if (_hostname)
	{
	  if (! strcmp(scheme, "imap"))
	    {
	      is_imap = TRUE;
	      _ssl = FALSE;
	    }
	  else if (! strcmp(scheme, "imaps"))
	    {
	      is_imap = TRUE;
	      _ssl = TRUE;
	    }
	}

      if (is_imap)
	{
	  if (ssl)
	    *ssl = _ssl;
	  if (username)
	    *username = g_strdup(_username);
	  if (password)
	    *password = g_strdup(_password);
	  if (authmech)
	    *authmech = g_strdup(_authmech);
	  if (hostname)
	    *hostname = g_strdup(_hostname);
	  if (port)
	    *port = _port >= 0 ? _port : MN_URI_IMAP_PORT(_ssl);
	  if (mailbox)
	    *mailbox = g_strdup(_mailbox ? _mailbox : "INBOX");
	}

      g_free(scheme);
      g_free(_username);
      g_free(_password);
      g_free(_authmech);
      g_free(_hostname);
      g_free(_mailbox);
    }

  return is_imap;
}

/**
 * mn_uri_build_gmail:
 * @username: the username
 * @password: the password
 *
 * Builds a Gmail URI (gmail://username:password).
 *
 * Return value: the new URI.
 **/
char *
mn_uri_build_gmail (const char *username, const char *password)
{
  g_return_val_if_fail(username != NULL, NULL);
  g_return_val_if_fail(password != NULL, NULL);

  return mn_uri_build_generic("gmail",
			      username,
			      password,
			      NULL,
			      NULL,
			      -1,
			      NULL);
}

/**
 * mn_uri_parse_gmail:
 * @uri: the URI to parse
 * @username: a location to store the username, or NULL
 * @password: a location to store the password, or NULL
 *
 * Parses a Gmail URI (gmail://username:password).
 *
 * WARNING: the parsing is very lax, this function is NOT intended to
 * be used on untrusted URIs.
 *
 * Return value: TRUE if @uri is a valid Gmail URI, FALSE otherwise.
 **/
gboolean
mn_uri_parse_gmail (const char *uri, char **username, char **password)
{
  char *scheme;
  char *_username;
  char *_password;
  gboolean is_gmail = FALSE;

  g_return_val_if_fail(uri != NULL, FALSE);

  if (mn_uri_parse_generic(uri,
			   &scheme,
			   &_username,
			   &_password,
			   NULL,
			   NULL,
			   NULL,
			   NULL))
    {
      is_gmail = ! strcmp(scheme, "gmail") && _password;

      if (is_gmail)
	{
	  if (username)
	    *username = g_strdup(_username);
	  if (password)
	    *password = g_strdup(_password);
	}

      g_free(scheme);
      g_free(_username);
      g_free(_password);
    }

  return is_gmail;
}

char *
mn_uri_canonicalize (const char *uri)
{
  char *canonical_uri;
  gboolean ssl;
  char *username = NULL;
  char *password = NULL;
  char *authmech = NULL;
  char *hostname = NULL;
  int port;
  char *mailbox = NULL;

  g_return_val_if_fail(uri != NULL, NULL);

  if (mn_uri_parse_pop(uri, &ssl, &username, &password, &authmech, &hostname, &port))
    canonical_uri = mn_uri_build_pop(ssl, username, password, authmech, hostname, port);
  else if (mn_uri_parse_imap(uri, &ssl, &username, &password, &authmech, &hostname, &port, &mailbox))
    canonical_uri = mn_uri_build_imap(ssl, username, password, authmech, hostname, port, mailbox);
  else if (mn_uri_parse_gmail(uri, &username, &password))
    canonical_uri = mn_uri_build_gmail(username, password);
  else
    canonical_uri = gnome_vfs_make_uri_canonical(uri);

  g_free(username);
  g_free(password);
  g_free(authmech);
  g_free(hostname);
  g_free(mailbox);

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
  const char *system_uri;
  char *name = NULL;

  g_return_val_if_fail(uri != NULL, NULL);

  system_uri = mn_uri_get_system_mailbox();
  if (system_uri && ! mn_uri_cmp(uri, system_uri))
    name = g_strdup(_("System Mailbox"));

  if (! name)
    {
      char *path;
      
      path = gnome_vfs_get_local_path_from_uri(uri);
      if (path)
	{
	  name = g_filename_to_utf8(path, -1, NULL, NULL, NULL);
	  g_free(path);
	}
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

      if (mn_uri_parse_pop(uri, NULL, &username, NULL, NULL, &hostname, NULL))
	{
	  name = g_strdup_printf("%s@%s", username, hostname);
	  g_free(username);
	  g_free(hostname);
	}
    }
  if (! name)
    {
      char *username;
      char *hostname;
      char *mailbox;

      if (mn_uri_parse_imap(uri, NULL, &username, NULL, NULL, &hostname, NULL, &mailbox))
	{
	  name = ! strcmp(mailbox, "INBOX")
	    ? g_strdup_printf("%s@%s", username, hostname)
	    : g_strdup_printf("%s@%s/%s", username, hostname, mailbox);
	  g_free(username);
	  g_free(hostname);
	  g_free(mailbox);
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

const char *
mn_uri_get_system_mailbox (void)
{
  static char *global_uri = NULL;
  G_LOCK_DEFINE_STATIC(global_uri);
  const char *uri;

  G_LOCK(global_uri);
  if (! global_uri)
    {
      const char *mail = g_getenv("MAIL");
      if (mail)
	global_uri = gnome_vfs_get_uri_from_local_path(mail);
    }
  uri = global_uri;
  G_UNLOCK(global_uri);

  return uri;
}
