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
#include <string.h>
#include <stdlib.h>
#include <glib/gi18n-lib.h>
#include <libgnomevfs/gnome-vfs-utils.h>
#include <libsoup/soup.h>
#include "mn-gmail-mailbox.h"
#include "mn-uri.h"
#include "mn-util.h"
#include "mn-stock.h"
#include "mn-soup.h"

/*** types *******************************************************************/

struct _MNGmailMailboxPrivate
{
  char		*username;
  char		*password;
  char		*cookie;
};

/*** variables ***************************************************************/

static GObjectClass *parent_class = NULL;

/*** functions ***************************************************************/

static void mn_gmail_mailbox_class_init (MNGmailMailboxClass *class);
static void mn_gmail_mailbox_init (MNGmailMailbox *mailbox);
static void mn_gmail_mailbox_finalize (GObject *object);

static GObject *mn_gmail_mailbox_constructor (GType type,
					      guint n_construct_properties,
					      GObjectConstructParam *construct_params);

static void mn_gmail_mailbox_is (const char *uri,
				 MNMailboxIsCallback *callback,
				 gpointer user_data);
static void mn_gmail_mailbox_check (MNMailbox *mailbox);

static SoupMessage *mn_gmail_mailbox_soup_message_new (const char *uri,
						       const char *method);

static char *mn_gmail_mailbox_get_token (const char *str,
					 const char *pre,
					 const char *post,
					 gboolean include_pre,
					 gboolean include_post);

static void mn_gmail_mailbox_login (MNGmailMailbox *mailbox);
static void mn_gmail_mailbox_login_cb (SoupMessage *message, gpointer user_data);

static char *mn_gmail_mailbox_get_cookie (SoupMessage *message);

static void mn_gmail_mailbox_check_internal (MNGmailMailbox *mailbox);
static void mn_gmail_mailbox_check_internal_cb (SoupMessage *message,
						gpointer user_data);

static int mn_gmail_mailbox_get_unread_count (SoupMessage *message);

/*** implementation **********************************************************/

GType
mn_gmail_mailbox_get_type (void)
{
  static GType gmail_mailbox_type = 0;
  
  if (! gmail_mailbox_type)
    {
      static const GTypeInfo gmail_mailbox_info = {
	sizeof(MNGmailMailboxClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_gmail_mailbox_class_init,
	NULL,
	NULL,
	sizeof(MNGmailMailbox),
	0,
	(GInstanceInitFunc) mn_gmail_mailbox_init,
      };
      
      gmail_mailbox_type = g_type_register_static(MN_TYPE_MAILBOX,
						  "MNGmailMailbox",
						  &gmail_mailbox_info,
						  0);
    }
  
  return gmail_mailbox_type;
}

static void
mn_gmail_mailbox_class_init (MNGmailMailboxClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS(class);
  MNMailboxClass *mailbox_class = MN_MAILBOX_CLASS(class);

  parent_class = g_type_class_peek_parent(class);

  object_class->constructor = mn_gmail_mailbox_constructor;
  object_class->finalize = mn_gmail_mailbox_finalize;

  mailbox_class->stock_id = MN_STOCK_GMAIL;
  mailbox_class->format = "Gmail";
  mailbox_class->is = mn_gmail_mailbox_is;
  mailbox_class->check = mn_gmail_mailbox_check;
}

static void
mn_gmail_mailbox_init (MNGmailMailbox *mailbox)
{
  mn_soup_use();
  mailbox->priv = g_new0(MNGmailMailboxPrivate, 1);
}

static void
mn_gmail_mailbox_finalize (GObject *object)
{
  MNGmailMailbox *gmail_mailbox = MN_GMAIL_MAILBOX(object);

  g_free(gmail_mailbox->priv->username);
  g_free(gmail_mailbox->priv->password);
  g_free(gmail_mailbox->priv->cookie);
  g_free(gmail_mailbox->priv);
  mn_soup_unuse();

  G_OBJECT_CLASS(parent_class)->finalize(object);
}

static GObject *
mn_gmail_mailbox_constructor (GType type,
			      guint n_construct_properties,
			      GObjectConstructParam *construct_params)
{
  GObject *object;
  MNMailbox *mailbox;
  MNGmailMailbox *gmail_mailbox;

  object = G_OBJECT_CLASS(parent_class)->constructor(type,
						     n_construct_properties,
						     construct_params);
  mailbox = MN_MAILBOX(object);
  gmail_mailbox = MN_GMAIL_MAILBOX(object);

  if (! mn_uri_parse_gmail(mn_mailbox_get_uri(mailbox),
			   &gmail_mailbox->priv->username,
			   &gmail_mailbox->priv->password))
    mn_mailbox_set_init_error(mailbox, _("unable to parse Gmail URI"));
  
  return object;
}

static void
mn_gmail_mailbox_is (const char *uri,
		     MNMailboxIsCallback *callback,
		     gpointer user_data)
{
  char *scheme;
  gboolean is;

  scheme = gnome_vfs_get_uri_scheme(uri);
  is = scheme && ! strcmp(scheme, "gmail");
  g_free(scheme);

  callback(is, user_data);
}

static void
mn_gmail_mailbox_check (MNMailbox *mailbox)
{
  MNGmailMailbox *gmail_mailbox = MN_GMAIL_MAILBOX(mailbox);
  
  if (gmail_mailbox->priv->cookie)
    mn_gmail_mailbox_check_internal(gmail_mailbox);
  else
    mn_gmail_mailbox_login(gmail_mailbox);
}

static SoupMessage *
mn_gmail_mailbox_soup_message_new (const char *uri, const char *method)
{
  SoupContext *context;
  SoupMessage *message;

  context = soup_context_get(uri);
  message = soup_message_new(context, method);
  soup_context_unref(context);

  soup_message_add_header(message->request_headers, "User-Agent", "Mail Notification");

  return message;
}

/*
 * Gets the token of STR included between the first occurrence of PRE
 * and the last occurrence of POST.
 *
 * If INCLUDE_PRE is true, include PRE in the result.
 * If INCLUDE_POST is true, include POST in the result.
 */
static char *
mn_gmail_mailbox_get_token (const char *str,
			    const char *pre,
			    const char *post,
			    gboolean include_pre,
			    gboolean include_post)
{
  char *token = NULL;
  char *pre_loc;

  g_return_val_if_fail(str != NULL, NULL);
  g_return_val_if_fail(pre != NULL, NULL);
  g_return_val_if_fail(post != NULL, NULL);

  pre_loc = strstr(str, pre);
  if (pre_loc)
    {
      char *after_pre_loc;
      char *post_loc;

      after_pre_loc = pre_loc + strlen(pre);
      post_loc = strstr(after_pre_loc, post);
      if (post_loc)
	{
	  char *start = include_pre ? pre_loc : after_pre_loc;
	  char *end = include_post ? post_loc + strlen(post) : post_loc;

	  token = g_strndup(start, end - start);
	}
    }

  return token;
}

static void
mn_gmail_mailbox_login (MNGmailMailbox *mailbox)
{
  char *uri;
  SoupMessage *message;

  g_return_if_fail(MN_IS_GMAIL_MAILBOX(mailbox));
  g_return_if_fail(mailbox->priv->cookie == NULL);

  mn_info(_("logging into Gmail as user %s"), mailbox->priv->username);

  uri = g_strdup_printf("https://gmail.google.com/accounts/ServiceLoginBoxAuth?Email=%s&Passwd=%s&null=Sign%%20in&service=mail", mailbox->priv->username, mailbox->priv->password);
  message = mn_gmail_mailbox_soup_message_new(uri, SOUP_METHOD_GET);
  g_free(uri);

  soup_message_queue(message, mn_gmail_mailbox_login_cb, mailbox);
  /* message will be freed by libsoup after invoking the callback */
}

static void
mn_gmail_mailbox_login_cb (SoupMessage *message, gpointer user_data)
{
  MNGmailMailbox *mailbox = user_data;

  if (SOUP_MESSAGE_IS_ERROR(message))
    mn_mailbox_set_error(MN_MAILBOX(mailbox), _("unable to transfer data: %s"), message->errorphrase);
  else
    {
      mailbox->priv->cookie = mn_gmail_mailbox_get_cookie(message);
      if (mailbox->priv->cookie)
	{
	  mn_info(_("successfully logged into Gmail as user %s"), mailbox->priv->username);
	  mn_gmail_mailbox_check_internal(mailbox);
	  return;
	}
      else
	mn_mailbox_set_error(MN_MAILBOX(mailbox), _("login failed"));
    }

  /* if this point is reached, an error occurred */
  mn_mailbox_end_check(MN_MAILBOX(mailbox));
}

static char *
mn_gmail_mailbox_get_cookie (SoupMessage *message)
{
  char *cookie = NULL;
  const GSList *set_cookie_headers;
  const GSList *l;
  char *sid = NULL;
  
  set_cookie_headers = soup_message_get_header_list(message->response_headers, "Set-Cookie");
  MN_LIST_FOREACH(l, set_cookie_headers)
    {
      const char *value = l->data;

      sid = mn_gmail_mailbox_get_token(value, "SID=", "=", TRUE, TRUE);
      if (sid)
	break;
    }
  
  if (sid)
    {
      char *body;
      char *cookie_val;

      body = g_strndup(message->response.body, message->response.length);
      cookie_val = mn_gmail_mailbox_get_token(body, "var cookieVal= \"", "\"", FALSE, FALSE);
      g_free(body);

      if (cookie_val)
	{
	  cookie = g_strdup_printf("%s; GV=%s", sid, cookie_val);
	  g_free(cookie_val);
	}
      
      g_free(sid);
    }

  return cookie;
}

static void
mn_gmail_mailbox_check_internal (MNGmailMailbox *mailbox)
{
  SoupMessage *message;
  
  g_return_if_fail(MN_IS_GMAIL_MAILBOX(mailbox));

  mn_info(_("searching for unread mail in %s"), mn_mailbox_get_name(MN_MAILBOX(mailbox)));

  message = mn_gmail_mailbox_soup_message_new("https://gmail.google.com/gmail?search=adv&as_subset=unread&view=tl&start=0", SOUP_METHOD_GET);
  soup_message_add_header(message->request_headers, "Cookie", mailbox->priv->cookie);

  soup_message_queue(message, mn_gmail_mailbox_check_internal_cb, mailbox);
  /* message will be freed by libsoup after invoking the callback */
}

static void
mn_gmail_mailbox_check_internal_cb (SoupMessage *message, gpointer user_data)
{
  MNGmailMailbox *mailbox = user_data;

  if (SOUP_MESSAGE_IS_ERROR(message))
    mn_mailbox_set_error(MN_MAILBOX(mailbox), _("unable to transfer data: %s"), message->errorphrase);
  else
    {
      int count;

      count = mn_gmail_mailbox_get_unread_count(message);
      if (count == -1)
	mn_mailbox_set_error(MN_MAILBOX(mailbox), _("unable to parse Gmail data"));
      else
	mn_mailbox_set_has_new(MN_MAILBOX(mailbox), count != 0);
    }

  mn_mailbox_end_check(MN_MAILBOX(mailbox));
}

static int
mn_gmail_mailbox_get_unread_count (SoupMessage *message)
{
  int count = -1;
  char *body;
  char *results;
  
  g_return_val_if_fail(message != NULL, -1);

  body = g_strndup(message->response.body, message->response.length);

  results = strstr(body, "Search results for: is:unread\",\"");
  if (results)
    {
      char *count_str;

      count_str = mn_gmail_mailbox_get_token(results + 32, "\",", "]", FALSE, FALSE);
      if (count_str)
	{
	  if (mn_str_isnumeric(count_str))
	    count = atoi(count_str);
	  g_free(count_str);
	}
    }
  
  g_free(body);

  return count;
}
