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
#include "mn-conf.h"
#include "mn-dialog.h"
#include "mn-mailbox.h"
#include "mn-settings.h"
#include "mn-util.h"

/*** variables ***************************************************************/

GSList *mn_mailboxes = NULL;

/*** functions ***************************************************************/

static void mn_mailboxes_unregister_all (void);

/*** implementation **********************************************************/

void
mn_mailboxes_register_all (void)
{
  GSList *gconf_mailboxes;
  GSList *l;

  if (mn_mailboxes)
    mn_mailboxes_unregister_all();

  gconf_mailboxes = mn_conf_get_list("/apps/mail-notification/mailboxes", GCONF_VALUE_STRING);

  MN_LIST_FOREACH(l, gconf_mailboxes)
    {
      char *locator = l->data;
      MNMailbox *mailbox;
      GError *err = NULL;

      mailbox = mn_mailbox_new(locator, &err);
      g_return_if_fail(mailbox != NULL);

      mn_mailboxes = g_slist_append(mn_mailboxes, mailbox);
      if (err)
	{
	  mn_error_dialog(_("Mailbox error."),
			  _("Mailbox <i>%s</i> is unsupported: %s."),
			  mailbox->name,
			  err->message);
	}
      else
	{
	  MNMailboxClass *class;

	  class = g_type_class_peek(G_TYPE_FROM_INSTANCE(mailbox));

	  if (mn_settings.debug)
	    mn_notice(class->is_remote
		      ? _("registered remote %s mailbox %s")
		      : _("registered local %s mailbox %s"),
		      class->format, mailbox->name);
	}
      
      g_free(locator);
    }

  g_slist_free(gconf_mailboxes);
}

static void
mn_mailboxes_unregister_all (void)
{
  GSList *l;

  MN_LIST_FOREACH(l, mn_mailboxes)
    {
      MNMailbox *mailbox = l->data;
      g_object_unref(mailbox);
    }
  
  g_slist_free(mn_mailboxes);
  mn_mailboxes = NULL;
  
  if (mn_settings.debug)
    mn_notice(_("unregistered all mailboxes"));
}
