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

/*** variables ***************************************************************/

static int use_count = 0;
G_LOCK_DEFINE_STATIC(use_count);
static unsigned int http_proxy_notification_id;
static unsigned int proxy_notification_id;

/*** functions ***************************************************************/

static void mn_soup_notify_proxy_cb (GConfClient *client,
				     unsigned int cnxn_id,
				     GConfEntry *entry,
				     gpointer user_data);
static void mn_soup_update_proxy (void);

/*** implementation **********************************************************/

void
mn_soup_use (void)
{
  G_LOCK(use_count);
  if (++use_count == 1)
    {
      eel_gconf_monitor_add(CONF_HTTP_PROXY_NAMESPACE);
      eel_gconf_monitor_add(CONF_PROXY_NAMESPACE);

      mn_soup_update_proxy();

      http_proxy_notification_id = eel_gconf_notification_add(CONF_HTTP_PROXY_NAMESPACE, mn_soup_notify_proxy_cb, NULL);
      proxy_notification_id = eel_gconf_notification_add(CONF_PROXY_NAMESPACE, mn_soup_notify_proxy_cb, NULL);
    }
  G_UNLOCK(use_count);
}

void
mn_soup_unuse (void)
{
  G_LOCK(use_count);
  g_return_if_fail(use_count > 0);
  if (--use_count == 0)
    {
      eel_gconf_notification_remove(http_proxy_notification_id);
      eel_gconf_notification_remove(proxy_notification_id);
      eel_gconf_monitor_remove(CONF_HTTP_PROXY_NAMESPACE);
      eel_gconf_monitor_remove(CONF_PROXY_NAMESPACE);

      soup_shutdown();
    }
  G_UNLOCK(use_count);
}

static void
mn_soup_notify_proxy_cb (GConfClient *client,
			 unsigned int cnxn_id,
			 GConfEntry *entry,
			 gpointer user_data)
{
  GDK_THREADS_ENTER();
  mn_soup_update_proxy();
  GDK_THREADS_LEAVE();
}
 
static void
mn_soup_update_proxy (void)
{
  SoupContext *context = NULL;
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
		  char *uri;

		  socks_port = eel_gconf_get_integer(CONF_PROXY_SOCKS_PORT);
		  if (socks_port <= 0 || socks_port > 65535)
		    socks_port = 1080;

		  /* we assume it's a Socks 5 proxy, since it is not specified */
		  uri = g_strdup_printf("socks5://%s:%i", socks_host, socks_port);
		  context = soup_context_get(uri);
		  
		  g_free(uri);
		}

	      g_free(socks_host);
	    }
	}
      g_free(mode);
    }
  
  if (! context)
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
		  GString *uri;
		  int port;
	      
		  uri = g_string_new("http://");
		  
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
			g_string_append_printf(uri, "%s:%s@", authentication_user, authentication_password);
		      
		      g_free(authentication_user);
		      g_free(authentication_password);
		    }
	      
		  g_string_append_printf(uri, "%s:%i", host, port);
	      
		  context = soup_context_get(uri->str);
		  g_string_free(uri, TRUE);
		}

	      g_free(host);
	    }
	}
    }

  soup_set_proxy(context);
  if (context)
    soup_context_unref(context);
}
