/* 
 * mn-pop3-mailbox.c - POP3 support for Mail Notification
 * Fully complies with RFC 1939
 *
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

#include "config.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include <gnet.h>
#include <libgnomevfs/gnome-vfs-utils.h>
#include "mn-pop3-mailbox.h"
#include "mn-util.h"
#include "mn-uri.h"

/*** types *******************************************************************/

typedef enum
{
  STATE_CONNECT,
  STATE_ACK,
  STATE_USER,
  STATE_PASS,
  STATE_STAT,
  STATE_QUIT
} State;
    
struct _MNPOP3MailboxPrivate
{
  char		*hostname;
  int		port;
  char		*username;
  char		*password;

  State		state;
};

/*** variables ***************************************************************/

static GObjectClass *parent_class = NULL;

/*** functions ***************************************************************/

static void mn_pop3_mailbox_class_init (MNPOP3MailboxClass *class);
static void mn_pop3_mailbox_init (MNPOP3Mailbox *mailbox);
static void mn_pop3_mailbox_finalize (GObject *object);

static GObject *mn_pop3_mailbox_constructor (GType type,
					     guint n_construct_properties,
					     GObjectConstructParam *construct_params);

static void mn_pop3_mailbox_is (const char *uri,
				MNMailboxIsCallback *callback,
				gpointer user_data);
static void mn_pop3_mailbox_check (MNMailbox *mailbox);

static void mn_pop3_mailbox_conn_write (GConn *conn, const char *format, ...);
static void mn_pop3_mailbox_conn_cb (GConn *conn,
				     GConnEvent *event,
				     gpointer user_data);

/*** implementation **********************************************************/

GType
mn_pop3_mailbox_get_type (void)
{
  static GType pop3_mailbox_type = 0;
  
  if (! pop3_mailbox_type)
    {
      static const GTypeInfo pop3_mailbox_info = {
	sizeof(MNPOP3MailboxClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_pop3_mailbox_class_init,
	NULL,
	NULL,
	sizeof(MNPOP3Mailbox),
	0,
	(GInstanceInitFunc) mn_pop3_mailbox_init,
      };
      
      pop3_mailbox_type = g_type_register_static(MN_TYPE_MAILBOX,
						 "MNPOP3Mailbox",
						 &pop3_mailbox_info,
						 0);
    }
  
  return pop3_mailbox_type;
}

static void
mn_pop3_mailbox_class_init (MNPOP3MailboxClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS(class);
  MNMailboxClass *mailbox_class = MN_MAILBOX_CLASS(class);

  parent_class = g_type_class_peek_parent(class);

  object_class->constructor = mn_pop3_mailbox_constructor;
  object_class->finalize = mn_pop3_mailbox_finalize;

  mailbox_class->format = "POP3";
  mailbox_class->is = mn_pop3_mailbox_is;
  mailbox_class->check = mn_pop3_mailbox_check;
}

static void
mn_pop3_mailbox_init (MNPOP3Mailbox *mailbox)
{
  mailbox->priv = g_new0(MNPOP3MailboxPrivate, 1);
}

static void
mn_pop3_mailbox_finalize (GObject *object)
{
  MNPOP3Mailbox *pop3_mailbox = MN_POP3_MAILBOX(object);

  g_free(pop3_mailbox->priv->username);
  g_free(pop3_mailbox->priv->password);
  g_free(pop3_mailbox->priv->hostname);
  g_free(pop3_mailbox->priv);

  G_OBJECT_CLASS(parent_class)->finalize(object);
}

static GObject *
mn_pop3_mailbox_constructor (GType type,
			     guint n_construct_properties,
			     GObjectConstructParam *construct_params)
{
  GObject *object;
  MNMailbox *mailbox;
  MNPOP3Mailbox *pop3_mailbox;

  object = G_OBJECT_CLASS(parent_class)->constructor(type,
						     n_construct_properties,
						     construct_params);
  mailbox = MN_MAILBOX(object);
  pop3_mailbox = MN_POP3_MAILBOX(object);

  if (! mn_uri_parse_pop(mn_mailbox_get_uri(mailbox),
			 &pop3_mailbox->priv->username,
			 &pop3_mailbox->priv->password,
			 &pop3_mailbox->priv->hostname,
			 &pop3_mailbox->priv->port))
    mn_mailbox_set_init_error(mailbox, _("unable to parse POP3 URI"));
  
  return object;
}

static void
mn_pop3_mailbox_is (const char *uri,
		    MNMailboxIsCallback *callback,
		    gpointer user_data)
{
  char *scheme;
  gboolean is;

  scheme = gnome_vfs_get_uri_scheme(uri);
  is = scheme && ! strcmp(scheme, "pop");
  g_free(scheme);

  callback(is, user_data);
}

static void
mn_pop3_mailbox_check (MNMailbox *mailbox)
{
  MNPOP3Mailbox *pop3_mailbox = MN_POP3_MAILBOX(mailbox);
  GConn *conn;

  mn_info(_("connecting to POP3 server %s"), pop3_mailbox->priv->hostname);

  pop3_mailbox->priv->state = STATE_CONNECT;
  conn = gnet_conn_new(pop3_mailbox->priv->hostname, pop3_mailbox->priv->port, mn_pop3_mailbox_conn_cb, mailbox);
  gnet_conn_connect(conn);
}

static void
mn_pop3_mailbox_conn_write (GConn *conn, const char *format, ...)
{
  va_list args;
  char *command;
  char *full;

  g_return_if_fail(conn != NULL);
  g_return_if_fail(format != NULL);

  va_start(args, format);
  command = g_strdup_vprintf(format, args);
  va_end(args);

  mn_info("<%20-s> %s", PACKAGE, command);
  full = g_strconcat(command, "\r\n", NULL);
  g_free(command);

  gnet_conn_write(conn, full, strlen(full));
  g_free(full);
}

static void
mn_pop3_mailbox_conn_cb (GConn *conn, GConnEvent *event, gpointer user_data)
{
  MNPOP3Mailbox *mailbox = user_data;

  switch (event->type)
    {
    case GNET_CONN_CONNECT:
      mn_info(_("successfully connected to %s"), mailbox->priv->hostname);
      mailbox->priv->state = STATE_ACK;
      gnet_conn_readline(conn);
      break;
      
    case GNET_CONN_ERROR:
      mn_mailbox_set_error(MN_MAILBOX(mailbox), _("unable to connect to server"));
      goto unref;

    case GNET_CONN_CLOSE:
      if (! mn_mailbox_get_error(MN_MAILBOX(mailbox)))
	mn_mailbox_set_error(MN_MAILBOX(mailbox), _("connection to server unexpectedly closed"));
      goto unref;

    case GNET_CONN_READ:
      {
	mn_info("<%20-s> %s", mailbox->priv->hostname, event->buffer);

	if (! strcmp(event->buffer, "+OK") || ! strncmp(event->buffer, "+OK ", 4))
	  switch (mailbox->priv->state)
	    {
	    case STATE_ACK:
	      mailbox->priv->state = STATE_USER;
	      mn_pop3_mailbox_conn_write(conn, "USER %s", mailbox->priv->username);
	      break;
	      
	    case STATE_USER:
	      mailbox->priv->state = STATE_PASS;
	      mn_pop3_mailbox_conn_write(conn, "PASS %s", mailbox->priv->password);
	      break;
	      
	    case STATE_PASS:
	      mailbox->priv->state = STATE_STAT;
	      mn_pop3_mailbox_conn_write(conn, "STAT");
	      break;
	      
	    case STATE_STAT:
	      {
		char ok[4];
		int count;
		int size;
		
		if (sscanf(event->buffer, "%3s %d %d", ok, &count, &size) == 3)
		  {
		    mn_mailbox_set_has_new(MN_MAILBOX(mailbox), count != 0);
		    mailbox->priv->state = STATE_QUIT;
		    mn_pop3_mailbox_conn_write(conn, "QUIT");
		  }
		else
		  {
		    mn_mailbox_set_error(MN_MAILBOX(mailbox), _("invalid STAT reply \"%s\""), event->buffer);
		    /* the server is uncompliant, we do not send a QUIT */
		    goto unref;
		  }
	      }
	      break;
	      
	    case STATE_QUIT:
	      goto unref;
	      break;
	      
	    default:
	      g_return_if_reached();
	    }
	else if (! strcmp(event->buffer, "-ERR") || ! strncmp(event->buffer, "-ERR ", 5))
	  {
	    if (event->buffer[4])
	      mn_mailbox_set_error(MN_MAILBOX(mailbox), "\"%s\"", event->buffer + 5);
	    else
	      mn_mailbox_set_error(MN_MAILBOX(mailbox), _("unknown error"));

	    switch (mailbox->priv->state)
	      {
	      case STATE_ACK:
	      case STATE_QUIT:
		goto unref;

	      case STATE_USER:
	      case STATE_PASS:
	      case STATE_STAT:
		mailbox->priv->state = STATE_QUIT;
		mn_pop3_mailbox_conn_write(conn, "QUIT");
		break;

	      default:
		g_return_if_reached();
	      }
	  }
	else
	  {
	    mn_mailbox_set_error(MN_MAILBOX(mailbox), _("invalid reply \"%s\""), event->buffer);
	    /* the server is uncompliant, we do not send a QUIT */
	    goto unref;
	  }
      }
      break;

    case GNET_CONN_WRITE:
      switch (mailbox->priv->state)
	{
	case STATE_USER:
	case STATE_PASS:
	case STATE_STAT:
	case STATE_QUIT:
	  gnet_conn_readline(conn);
	  break;

	default:
	  g_return_if_reached();
	}
      break;
      
    default:
      /* nop */
      break;
    }
  return;

 unref:
  gnet_conn_unref(conn);
  mn_mailbox_end_check(MN_MAILBOX(mailbox));
}
