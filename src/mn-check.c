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
#include <gdk/gdk.h>
#include <libgnome/gnome-i18n.h>
#include "mn-conf.h"
#include "mn-dialog.h"
#include "mn-ui.h"
#include "mn-util.h"
#include "mn-mailbox.h"
#include "mn-mailboxes.h"
#include "mn-check.h"
#include "mn-settings.h"

/*** variables ***************************************************************/

static GStaticMutex local_mutex = G_STATIC_MUTEX_INIT;
static GStaticMutex remote_mutex = G_STATIC_MUTEX_INIT;

/*** functions ***************************************************************/

static gpointer	mn_check_thread_cb		(gpointer	data);
static void	mn_check_update_state		(gboolean	has_new,
						 int		flags);
static void	mn_check_install_callback	(guint		*id,
						 const char	*key_enabled,
						 const char	*key_minutes,
						 const char	*key_seconds,
						 int		flags);
static gboolean	mn_check_timeout_cb		(gpointer	data);

/*** implementation **********************************************************/

/*
 * Checks for new mail.
 *
 * FLAGS must be a bitwise or'ed combination of the MN_CHECK_* flags.
 *
 * If there is already a check (of the same type, local or remote) in
 * progress, do nothing.
 */
void
mn_check (int flags)
{
  GStaticMutex *mutex = flags & MN_CHECK_REMOTE ? &remote_mutex : &local_mutex;

  if (g_static_mutex_trylock(mutex)) /* not already checking, proceed */
    {
      GError *err = NULL;
      
      g_static_mutex_unlock(mutex); /* will be locked in thread, not here */

      if (! g_thread_create(mn_check_thread_cb,
			    GINT_TO_POINTER(flags),
			    FALSE,
			    &err))
	{
	  if (flags & MN_CHECK_INTERACTIVE)
	    mn_error_dialog(_("Check error."),
			    _("Unable to create a thread: %s."),
			    err->message);
	  else
	    g_warning(_("unable to create a thread: %s"), err->message);

	  g_error_free(err);
	}
    }
}

static gpointer
mn_check_thread_cb (gpointer data)
{
  GSList *l;
  int flags = GPOINTER_TO_INT(data);
  GStaticMutex *mutex = flags & MN_CHECK_REMOTE ? &remote_mutex : &local_mutex;
  static gboolean has_new_local = FALSE;
  static gboolean has_new_remote = FALSE;
  gboolean *has_new = flags & MN_CHECK_REMOTE ? &has_new_remote : &has_new_local;
  static int threads = 0;	/* number of check threads currently running */
  G_LOCK_DEFINE_STATIC(threads);
  GSList *mailboxes;

  G_LOCK(threads);
  if (threads++ == 0)		/* threads just switched from 0 to 1, update */
    {
      GDK_THREADS_ENTER();
      mn_ui_set_can_check(FALSE);
      gdk_flush();
      GDK_THREADS_LEAVE();
    }
  G_UNLOCK(threads);

  g_static_mutex_lock(mutex);

  *has_new = FALSE;

  mailboxes = mn_mailboxes_get();
  MN_LIST_FOREACH(l, mailboxes)
    {
      MNMailbox *mailbox = l->data;
      MNMailboxClass *class;

      class = g_type_class_peek(G_TYPE_FROM_INSTANCE(mailbox));
      
      if ((flags & MN_CHECK_REMOTE) == class->is_remote && class->has_new)
	{
	  GError *err = NULL;

	  if (mn_settings.debug)
	    mn_notice(_("checking if there is new mail in %s"), mailbox->name);

	  *has_new = class->has_new(mailbox, &err);
	  if (err)
	    {
	      if (flags & MN_CHECK_INTERACTIVE)
		{
		  GDK_THREADS_ENTER();
		  mn_error_dialog(_("Check error."),
				  _("Unable to check mailbox <i>%s</i>: %s."),
				  mailbox->name,
				  err->message);
		  gdk_flush();
		  GDK_THREADS_LEAVE();
		}
	      else
		g_warning(_("unable to check mailbox %s: %s"),
			  mailbox->name,
			  err->message);

	      g_error_free(err);

	      /* reset has_new and let proceed */
	      *has_new = FALSE;
	    }
	  else
	    {
	      if (mn_settings.debug)
		mn_notice(*has_new
			  ? _("%s has new mail")
			  : _("%s has no new mail"),
			  mailbox->name);

	      if (*has_new)
		break;
	    }
	}
    }
  mn_objects_free(mailboxes);
  
  mn_check_update_state(has_new_local || has_new_remote, flags);
  
  g_static_mutex_unlock(mutex);

  G_LOCK(threads);
  if (--threads == 0)
    {
      GDK_THREADS_ENTER();
      mn_ui_set_can_check(TRUE);
      gdk_flush();
      GDK_THREADS_LEAVE();
    }
  G_UNLOCK(threads);

  return NULL;
}

static void
mn_check_update_state (gboolean has_new, int flags)
{
  static gboolean old_has_new = FALSE;
  G_LOCK_DEFINE_STATIC(old_has_new);
  gboolean state_changed;

  G_LOCK(old_has_new);
  state_changed = has_new != old_has_new;
  old_has_new = has_new;
  G_UNLOCK(old_has_new);
  
  if (state_changed)
    {
      GDK_THREADS_ENTER();
      mn_ui_set_has_new(has_new);
      gdk_flush();
      GDK_THREADS_LEAVE();

      if (has_new && mn_conf_get_bool("/apps/mail-notification/commands/new-mail/enabled"))
	{
	  const char *command;
	  
	  command = mn_conf_get_string("/apps/mail-notification/commands/new-mail/command");
	  if (command)
	    {
	      GError *err = NULL;
	      
	      if (! g_spawn_command_line_async(command, &err))
		{
		  if (flags & MN_CHECK_INTERACTIVE)
		    {
		      GDK_THREADS_ENTER();
		      mn_error_dialog(_("Command error."),
				      _("Unable to execute new mail command: %s."),
				      err->message);
		      gdk_flush();
		      GDK_THREADS_LEAVE();
		    }
		  else
		    g_warning(_("unable to execute new mail command: %s"), err->message);

		  g_error_free(err);
		}
	    }
	}
    }
}

static void
mn_check_install_callback (guint *id,
			   const char *key_enabled,
			   const char *key_minutes,
			   const char *key_seconds,
			   int flags)
{
  if (*id != -1)
    {
      g_source_remove(*id);
      *id = -1;
    }
  
  if (mn_conf_get_bool(key_enabled))
    {
      int minutes;
      int seconds;
      
      minutes = mn_conf_get_int(key_minutes);
      seconds = mn_conf_get_int(key_seconds);

      *id = g_timeout_add(((minutes * 60) + seconds) * 1000,
			  mn_check_timeout_cb,
			  GINT_TO_POINTER(flags));
    }
}

static gboolean
mn_check_timeout_cb (gpointer data)
{
  int flags = GPOINTER_TO_INT(flags);
  
  mn_check(flags);

  return TRUE;
}

void
mn_check_install (void)
{
  static guint local_id = -1;
  static guint remote_id = -1;

  mn_check_install_callback(&local_id,
			    "/apps/mail-notification/local/enabled",
			    "/apps/mail-notification/local/delay/minutes",
			    "/apps/mail-notification/local/delay/seconds",
			    0);

  mn_check_install_callback(&remote_id,
			    "/apps/mail-notification/remote/enabled",
			    "/apps/mail-notification/remote/delay/minutes",
			    "/apps/mail-notification/remote/delay/seconds",
			    MN_CHECK_REMOTE);
}
