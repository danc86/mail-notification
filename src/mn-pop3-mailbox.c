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

#include "config.h"
#include <libgnome/gnome-i18n.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <gnet.h>
#include <string.h>
#include "mn-pop3-mailbox.h"
#include "mn-dialog.h"
#include "mn-settings.h"
#include "mn-util.h"

/*** cpp *********************************************************************/

#define MN_POP3_MAILBOX_MAX_RESPONSE_LEN	512	/* RFC 1939 */

/*** variables ***************************************************************/

static GObjectClass *parent_class = NULL;

/*** functions ***************************************************************/

static void	mn_pop3_mailbox_class_init	(MNPOP3MailboxClass *class);
static GObject	*mn_pop3_mailbox_constructor	(GType type,
						 guint n_construct_properties,
						 GObjectConstructParam *construct_params);
static void	mn_pop3_mailbox_finalize	(GObject            *object);
static void	mn_pop3_mailbox_init		(MNPOP3Mailbox      *mailbox);

static gboolean	mn_pop3_mailbox_is		(const char         *locator);
static gboolean	mn_pop3_mailbox_has_new		(MNMailbox          *mailbox,
						 GError             **err);

static void	mn_pop3_mailbox_parse_locator	(MNPOP3Mailbox *pop3_mailbox);
static void	mn_pop3_mailbox_strip_crlf	(char               *str);
static gboolean	mn_pop3_mailbox_server_connect	(MNPOP3Mailbox      *mailbox,
						 GError             **err);
static gboolean	mn_pop3_mailbox_server_read	(MNPOP3Mailbox      *mailbox,
						 char               *buf);
static gboolean	mn_pop3_mailbox_server_command	(MNPOP3Mailbox      *mailbox,
						 const char         *format,
						 ...);

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
  GObjectClass *object_class;
  MNMailboxClass *mailbox_class;

  parent_class = g_type_class_peek_parent(class);

  object_class = G_OBJECT_CLASS(class);
  object_class->constructor = mn_pop3_mailbox_constructor;
  object_class->finalize = mn_pop3_mailbox_finalize;

  mailbox_class = MN_MAILBOX_CLASS(class);
  mailbox_class->format = "POP3";
  mailbox_class->is_remote = TRUE;
  mailbox_class->is = mn_pop3_mailbox_is;
  mailbox_class->has_new = mn_pop3_mailbox_has_new;
}

static void
mn_pop3_mailbox_init (MNPOP3Mailbox *mailbox)
{
  mailbox->hostname = NULL;
  mailbox->port = 0;
  mailbox->user = NULL;
  mailbox->password = NULL;
  mailbox->socket = NULL;
  mailbox->channel = NULL;
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

  mn_pop3_mailbox_parse_locator(pop3_mailbox);

  return object;
}

static void
mn_pop3_mailbox_finalize (GObject *object)
{
  MNMailbox *mailbox;
  MNPOP3Mailbox *pop3_mailbox;

  mailbox = MN_MAILBOX(object);
  pop3_mailbox = MN_POP3_MAILBOX(object);

  if (! mailbox->err) /* mailbox->name has been set only if ! mailbox->err */
    g_free(mailbox->name);
  
  g_free(pop3_mailbox->user);
  g_free(pop3_mailbox->password);
  g_free(pop3_mailbox->hostname);

  G_OBJECT_CLASS(parent_class)->finalize(object);
}

static gboolean
mn_pop3_mailbox_is (const char *locator)
{
  g_return_val_if_fail(locator != NULL, FALSE);

  return ! strncmp(locator, "pop3:", 5);
}

static void
mn_pop3_mailbox_parse_locator (MNPOP3Mailbox *pop3_mailbox)
{
  MNMailbox *mailbox;
  char *user;
  char *password;
  char *hostname;
  char *port;
  
  mailbox = MN_MAILBOX(pop3_mailbox);

  g_assert(mailbox->locator != NULL);

  user = mailbox->locator + 5;
  if (strlen(mailbox->locator) < 5)
    goto error;

  password = strchr(user, ':');
  if (! password++)		/* ++ to skip the colon */
    goto error;
  
  hostname = strchr(password, '@');
  if (! hostname++)		/* ++ to skip the arobas */
    goto error;

  port = strchr(hostname, ':');
  if (! (port++ && mn_str_isnumeric(port)))	/* ++ to skip the colon */
    goto error;

  if ((password - user - 1 <= 0)
      || (hostname - password - 1 <= 0)
      || (port - hostname - 1 <= 0))
    goto error;

  pop3_mailbox->user = g_strdup(user);
  pop3_mailbox->user[password - user - 1] = 0;

  pop3_mailbox->password = g_strdup(password);
  pop3_mailbox->password[hostname - password - 1] = 0;

  pop3_mailbox->hostname = g_strdup(hostname);
  pop3_mailbox->hostname[port - hostname - 1] = 0;

  pop3_mailbox->port = atoi(port);

  mailbox->name = g_strdup_printf("%s@%s:%i",
				  pop3_mailbox->user,
				  pop3_mailbox->hostname,
				  pop3_mailbox->port);

  return;			/* locator's okay */

 error:
  mailbox->err = g_error_new(MN_POP3_MAILBOX_ERROR,
			     MN_POP3_MAILBOX_ERROR_PARSE_LOCATOR,
			     _("unable to parse locator \"%s\""),
			     mailbox->locator);
}

static void
mn_pop3_mailbox_strip_crlf (char *str)
{
  char *ptr;

  ptr = strrchr(str, '\n');
  if (ptr)
    *ptr = 0;

  ptr = strrchr(str, '\r');
  if (ptr)
    *ptr = 0;
}

static gboolean
mn_pop3_mailbox_server_connect (MNPOP3Mailbox *mailbox, GError **err)
{
  GInetAddr *addr;

  if (mn_settings.debug)
    mn_notice(_("resolving IP address of %s"), mailbox->hostname);

  addr = gnet_inetaddr_new(mailbox->hostname, mailbox->port);
  if (! addr)
    {
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_RESOLVE,
		  _("unable to resolve %s"), mailbox->hostname);
      return FALSE;
    }

  if (mn_settings.debug)
    mn_notice(_("connecting to POP3 server %s:%i"),
	      mailbox->hostname, mailbox->port);
  
  mailbox->socket = gnet_tcp_socket_new(addr);
  gnet_inetaddr_unref(addr);

  if (! mailbox->socket)
    {
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_CONNECT,
		  _("unable to connect to %s"), mailbox->hostname);
      return FALSE;
    }

  if (mn_settings.debug)
    mn_notice(_("successfully connected to %s:%i"),
	      mailbox->hostname, mailbox->port);
  
  mailbox->channel = gnet_tcp_socket_get_io_channel(mailbox->socket);

  return TRUE;
}

/*
 * Reads a line from MAILBOX's server, storing the output in BUF.
 *
 * Returns true if the line was read successfully, and the server reported
 * a POP3 success message (+OK).
 *
 * BUF is guaranteed to be valid in all cases (eitheir server's reply
 * or empty string).
 */
static gboolean
mn_pop3_mailbox_server_read (MNPOP3Mailbox *mailbox, char *buf)
{
  GIOError status;
  gsize count;

  status = gnet_io_channel_readline(mailbox->channel,
				    buf,
				    MN_POP3_MAILBOX_MAX_RESPONSE_LEN,
				    &count);
  
  if (status != G_IO_ERROR_NONE)
    buf[0] = 0;

  if (mn_settings.debug)
    {
      if (status == G_IO_ERROR_NONE && count != 0)
	{
	  char tmp[MN_POP3_MAILBOX_MAX_RESPONSE_LEN];
	  
	  strcpy(tmp, buf);
	  mn_pop3_mailbox_strip_crlf(tmp);
	  
	  mn_notice("<%20-s> %s", mailbox->hostname, tmp);
	}
      else
	mn_notice(_("unable to read from %s"), mailbox->hostname);
    }
  
  return status == G_IO_ERROR_NONE && count != 0 && ! strncmp(buf, "+OK", 3);
}

/*
 * Sends a command to MAILBOX's server, automatically appending a
 * trailing crlf.
 *
 * Returns true if the command was sent successfully.
 */
static gboolean
mn_pop3_mailbox_server_command (MNPOP3Mailbox *mailbox,
				const char *format,
				...)
{
  va_list args;
  char *command;
  char *full;
  int len;
  GIOError status;
  gsize count;

  va_start(args, format);
  command = g_strdup_vprintf(format, args);
  va_end(args);

  if (mn_settings.debug)
    mn_notice("<%20-s> %s", PACKAGE, command);

  full = g_strconcat(command, "\r\n", NULL);
  g_free(command);

  len = strlen(full);
  status = gnet_io_channel_writen(mailbox->channel, full, len, &count);
  g_free(full);

  return status == G_IO_ERROR_NONE && count == len;
}

static gboolean
mn_pop3_mailbox_has_new (MNMailbox *mailbox, GError **err)
{
  MNPOP3Mailbox *pop3_mailbox;
  char buf[MN_POP3_MAILBOX_MAX_RESPONSE_LEN];
  gboolean has_new = FALSE;

  char ok[4];
  int count;
  int size;

  pop3_mailbox = MN_POP3_MAILBOX(mailbox);

  if (! mn_pop3_mailbox_server_connect(pop3_mailbox, err))
    return FALSE;

  /* get ack */

  if (! mn_pop3_mailbox_server_read(pop3_mailbox, buf))
    {
      mn_pop3_mailbox_strip_crlf(buf);
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_ACK,
		  _("acknowledgement error (server replied \"%s\")"), buf);
      goto end;
    }

  /* authenticate */

  if (! mn_pop3_mailbox_server_command(pop3_mailbox,
				       "USER %s",
				       pop3_mailbox->user))
    {
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_SEND,
		  _("unable to send username"));
      goto end;
    }

  if (! mn_pop3_mailbox_server_read(pop3_mailbox, buf))
    {
      mn_pop3_mailbox_strip_crlf(buf);
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_AUTH,
		  _("username authentication error (server replied \"%s\")"), buf);
      goto quit;
    }

  if (! mn_pop3_mailbox_server_command(pop3_mailbox,
				       "PASS %s",
				       pop3_mailbox->password))
    {
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_SEND,
		  _("unable to send password"));
      goto end;
    }

  if (! mn_pop3_mailbox_server_read(pop3_mailbox, buf))
    {
      mn_pop3_mailbox_strip_crlf(buf);
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_AUTH,
		  _("password authentication error (server replied \"%s\")"), buf);
      goto quit;
    }

  /* issue a STAT */

  if (! mn_pop3_mailbox_server_command(pop3_mailbox, "STAT"))
    {
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_SEND,
		  _("unable to issue a STAT"));
      goto end;
    }

  if (! mn_pop3_mailbox_server_read(pop3_mailbox, buf))
    {
      mn_pop3_mailbox_strip_crlf(buf);
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_REPLY,
		  _("error in STAT reply (server replied \"%s\")"), buf);
      goto quit;
    }

  if (sscanf(buf, "%s %d %d", ok, &count, &size) < 3)
    {
      mn_pop3_mailbox_strip_crlf(buf);
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_REPLY,
		  _("could not parse STAT reply \"%s\""), buf);
      goto quit;
    }
      
  has_new = count != 0;

 quit:
  if (! mn_pop3_mailbox_server_command(pop3_mailbox, "QUIT"))
    {
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_SEND,
		  _("unable to issue a QUIT"));
      goto end;
    }
  
  if (! mn_pop3_mailbox_server_read(pop3_mailbox, buf))
    {
      mn_pop3_mailbox_strip_crlf(buf);
      g_set_error(err, MN_POP3_MAILBOX_ERROR, MN_POP3_MAILBOX_ERROR_REPLY,
		  _("error in QUIT reply (server replied \"%s\")"), buf);
      goto end;
    }
  
 end:
  gnet_tcp_socket_delete(pop3_mailbox->socket);
  return has_new;
}

GQuark
mn_pop3_mailbox_error_quark (void)
{
  static GQuark quark = 0;

  if (! quark)
    quark = g_quark_from_static_string("mn_pop3_mailbox_error");

  return quark;
}
