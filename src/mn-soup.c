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
#include <glib/gi18n.h>
#include <eel/eel.h>
#include <libsoup/soup.h>

/*** cpp *********************************************************************/

#define CONF_HTTP_PROXY_NAMESPACE		"/system/http_proxy"
#define CONF_HTTP_PROXY_USE_HTTP_PROXY		CONF_HTTP_PROXY_NAMESPACE "/use_http_proxy"
#define CONF_HTTP_PROXY_HOST			CONF_HTTP_PROXY_NAMESPACE "/host"
#define CONF_HTTP_PROXY_PORT			CONF_HTTP_PROXY_NAMESPACE "/port"
#define CONF_HTTP_PROXY_USE_AUTHENTICATION	CONF_HTTP_PROXY_NAMESPACE "/use_authentication"
#define CONF_HTTP_PROXY_AUTHENTICATION_USER	CONF_HTTP_PROXY_NAMESPACE "/authentication_user"
#define CONF_HTTP_PROXY_AUTHENTICATION_PASSWORD	CONF_HTTP_PROXY_NAMESPACE "/authentication_password"
#define CONF_PROXY_NAMESPACE			"/system/proxy"
#define CONF_PROXY_MODE				CONF_PROXY_NAMESPACE "/mode"
#define CONF_PROXY_SOCKS_HOST			CONF_PROXY_NAMESPACE "/socks_host"
#define CONF_PROXY_SOCKS_PORT			CONF_PROXY_NAMESPACE "/socks_port"

/*** functions ***************************************************************/

static char *mn_soup_build_proxy_uri (void);

/*** implementation **********************************************************/

static char *
mn_soup_build_proxy_uri (void)
{
  char *uri = NULL;
  char *mode;

  mode = eel_gconf_get_string(CONF_PROXY_MODE);
  if (mode)
    {
      if (! strcmp(mode, "manual"))
	{
	  char *socks_host;

	  socks_host = eel_gconf_get_string(CONF_PROXY_SOCKS_HOST);
	  if (socks_host)
	    {
	      if (*socks_host)
		{
		  int socks_port;

		  socks_port = eel_gconf_get_integer(CONF_PROXY_SOCKS_PORT);
		  if (socks_port <= 0 || socks_port > 65535)
		    socks_port = 1080;

		  /* we assume it's a Socks 5 proxy, since it is not specified */
		  uri = g_strdup_printf("socks5://%s:%i", socks_host, socks_port);
		}

	      g_free(socks_host);
	    }
	}
      g_free(mode);
    }
  
  if (! uri)
    {
      if (eel_gconf_get_boolean(CONF_HTTP_PROXY_USE_HTTP_PROXY))
	{
	  char *host;

	  /* FIXME: also honour /system/http_proxy/ignore_hosts */

	  host = eel_gconf_get_string(CONF_HTTP_PROXY_HOST);
	  if (host)
	    {
	      if (*host)
		{
		  GString *string;
		  int port;
	      
		  string = g_string_new("http://");
		  
		  port = eel_gconf_get_integer(CONF_HTTP_PROXY_PORT);
		  if (port <= 0 || port > 65535)
		    port = 8080; /* default as recommended by http-method.c of GnomeVFS */
		  
		  if (eel_gconf_get_boolean(CONF_HTTP_PROXY_USE_AUTHENTICATION))
		    {
		      char *authentication_user;
		      char *authentication_password;
		      
		      authentication_user = eel_gconf_get_string(CONF_HTTP_PROXY_AUTHENTICATION_USER);
		      authentication_password = eel_gconf_get_string(CONF_HTTP_PROXY_AUTHENTICATION_PASSWORD);
		      
		      if (authentication_user && authentication_password
			  && *authentication_user && *authentication_password)
			g_string_append_printf(string, "%s:%s@", authentication_user, authentication_password);
		      
		      g_free(authentication_user);
		      g_free(authentication_password);
		    }
	      
		  g_string_append_printf(string, "%s:%i", host, port);
		  uri = g_string_free(string, FALSE);
		}

	      g_free(host);
	    }
	}
    }

  return uri;
}

SoupSession *
mn_soup_session_sync_new (void)
{
  char *proxy_text_uri;
  SoupUri *proxy_uri = NULL;
  SoupSession *session;

  proxy_text_uri = mn_soup_build_proxy_uri();
  if (proxy_text_uri)
    {
      proxy_uri = soup_uri_new(proxy_text_uri);
      if (! proxy_uri)
	g_warning(_("unable to parse proxy URI \"%s\""), proxy_text_uri);
      g_free(proxy_text_uri);
    }

  session = soup_session_sync_new_with_options(SOUP_SESSION_PROXY_URI, proxy_uri, NULL);

  if (proxy_uri)
    soup_uri_free(proxy_uri);

  return session;
}
