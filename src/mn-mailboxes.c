/* 
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
#include <string.h>
#include <glib/gi18n-lib.h>
#include "mn-conf.h"
#include "mn-dialog.h"
#include "mn-mailbox.h"
#include "mn-util.h"
#include "mn-ui.h"
#include "mn-mailboxes.h"
#include "mn-uri.h"
#include "mn-preferences.h"
#include "mn-pending-mailbox.h"

/*** variables ***************************************************************/

static GSList *mailboxes = NULL;
static unsigned int timeout_id = -1;

/*** functions ***************************************************************/

static void mn_mailboxes_changed (void);
static void mn_mailboxes_new_cb (MNMailbox *mailbox, gpointer user_data);
static void mn_mailboxes_notify_h (GObject *object,
				   GParamSpec *pspec,
				   gpointer user_data);
static gboolean mn_mailboxes_timeout_cb (gpointer data);
static int mn_mailboxes_compare_func (gconstpointer a, gconstpointer b);

/*** implementation **********************************************************/

void
mn_mailboxes_register (void)
{
  GSList *gconf_mailboxes;
  GSList *l;

  gconf_mailboxes = eel_gconf_get_string_list(MN_CONF_MAILBOXES);

  /* first step: remove old mailboxes */

 loop:
  MN_LIST_FOREACH(l, mailboxes)
    {
      MNMailbox *mailbox = l->data;

      if (! g_slist_find_custom(gconf_mailboxes, mn_mailbox_get_uri(mailbox), (GCompareFunc) mn_uri_cmp))
	{
	  g_object_unref(mailbox);
	  mailboxes = g_slist_delete_link(mailboxes, l);
	  goto loop;
	}
    }

  /* second step: add new mailboxes */

  MN_LIST_FOREACH(l, gconf_mailboxes)
    {
      const char *uri = l->data;

      if (! mn_mailboxes_find(uri))
	{
	  MNMailbox *mailbox;

	  mailbox = mn_pending_mailbox_new(uri);
	  mailboxes = g_slist_append(mailboxes, mailbox);
	  
	  mn_mailbox_new_async(uri, mn_mailboxes_new_cb, NULL);
	}
    }

  mn_slist_free(gconf_mailboxes);
  mn_mailboxes_changed();
}

static void
mn_mailboxes_changed (void)
{
  mn_preferences_update_list();
  mn_ui_update_sensitivity();
  mn_ui_update_icon();
}

static void
mn_mailboxes_new_cb (MNMailbox *mailbox, gpointer user_data)
{
  GSList *elem;

  elem = g_slist_find_custom(mailboxes, mn_mailbox_get_uri(mailbox), mn_mailboxes_compare_func);
  if (elem)
    {
      g_signal_connect(G_OBJECT(mailbox), "notify", G_CALLBACK(mn_mailboxes_notify_h), NULL);
      if (MN_MAILBOX_GET_CLASS(mailbox)->check)
	mn_mailbox_check(mailbox);

      g_object_unref(elem->data);
      elem->data = mailbox;

      mn_mailboxes_changed();
    }
  else
    g_object_unref(mailbox);
}

static void
mn_mailboxes_notify_h (GObject *object, GParamSpec *pspec, gpointer user_data)
{
  MNMailbox *mailbox = MN_MAILBOX(object);
  gboolean update = FALSE;

  if (! strcmp(g_param_spec_get_name(pspec), "has-new"))
    {
      gboolean has_new;

      has_new = mn_mailbox_get_has_new(mailbox);
      mn_info(has_new ? _("%s has new mail") : _("%s has no new mail"), mn_mailbox_get_name(mailbox));

      update = TRUE;
    }
  else if (! strcmp(g_param_spec_get_name(pspec), "error"))
    {
      const char *error;

      error = mn_mailbox_get_error(mailbox);
      if (error)
	mn_info(_("%s reported an error: %s"), mn_mailbox_get_name(mailbox), error);

      update = TRUE;
    }
  
  if (update)
    mn_ui_update_icon();
}

void
mn_mailboxes_install_timeout (void)
{
  int minutes;
  int seconds;

  if (timeout_id != -1)
    {
      g_source_remove(timeout_id);
      timeout_id = -1;
    }

  minutes = eel_gconf_get_integer(MN_CONF_DELAY_MINUTES);
  seconds = eel_gconf_get_integer(MN_CONF_DELAY_SECONDS);

  if (minutes != 0 || seconds != 0)
    timeout_id = g_timeout_add(((minutes * 60) + seconds) * 1000,
			       mn_mailboxes_timeout_cb,
			       NULL);
}

static gboolean
mn_mailboxes_timeout_cb (gpointer data)
{
  mn_mailboxes_check();
  
  return TRUE;			/* continue */
}

void
mn_mailboxes_check (void)
{
  GSList *l;

  MN_LIST_FOREACH(l, mailboxes)
    {
      MNMailbox *mailbox = l->data;

      if (MN_MAILBOX_GET_CLASS(mailbox)->check && ! mn_mailbox_get_automatic(mailbox))
	mn_mailbox_check(mailbox);
    }
}

GSList *
mn_mailboxes_get (void)
{
  return mailboxes;
}

MNMailbox *
mn_mailboxes_find (const char *uri)
{
  GSList *elem;

  g_return_val_if_fail(uri != NULL, NULL);

  elem = g_slist_find_custom(mailboxes, uri, mn_mailboxes_compare_func);
  return elem ? elem->data : NULL;
}

static int
mn_mailboxes_compare_func (gconstpointer a, gconstpointer b)
{
  MNMailbox *mailbox = (MNMailbox *) a;
  const char *uri = b;

  return mn_uri_cmp(mn_mailbox_get_uri(mailbox), uri);
}
