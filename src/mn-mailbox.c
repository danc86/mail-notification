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
#include <stdarg.h>
#include <glib/gi18n-lib.h>
#include <libgnomevfs/gnome-vfs.h>
#include "mn-mailbox.h"
#ifdef WITH_MBOX
#include "mn-mbox-mailbox.h"
#endif
#ifdef WITH_MH
#include "mn-mh-mailbox.h"
#endif
#ifdef WITH_MAILDIR
#include "mn-maildir-mailbox.h"
#endif
#ifdef WITH_POP3
#include "mn-pop3-mailbox.h"
#endif
#ifdef WITH_SYLPHEED
#include "mn-sylpheed-mailbox.h"
#endif
#include "mn-unsupported-mailbox.h"
#include "mn-util.h"
#include "mn-uri.h"
#include "mn-conf.h"
#include "mn-dialog.h"
#include "mn-vfs.h"

/*** types *******************************************************************/

enum {
  PROP_0,
  PROP_URI,
  PROP_NAME,
  PROP_AUTOMATIC,
  PROP_HAS_NEW,
  PROP_ERROR
};

struct _MNMailboxPrivate
{
  char				*init_error;
  char				*uri;
  char				*name;
  gboolean			automatic;
  gboolean			has_new;
  char				*error;

  GnomeVFSMonitorHandle		*monitor_handle;
  char				*monitor_uri;
  MNMailboxMonitorEventType	monitor_events;

  gboolean			checking;
};

typedef struct
{
  char				*uri;
  MNMailbox			*mailbox;
  const GType			*types;
  int				i;
  MNMailboxNewAsyncCallback	*callback;
  gpointer			user_data;
} NewInfo;

/*** variables ***************************************************************/

static GObjectClass *parent_class = NULL;

/*** functions ***************************************************************/

static void	mn_mailbox_class_init	(MNMailboxClass	*class);
static void	mn_mailbox_init		(MNMailbox	*mailbox);
static void	mn_mailbox_finalize	(GObject	*object);
static void	mn_mailbox_set_property	(GObject	*object,
					 guint		prop_id,
					 const GValue	*value,
					 GParamSpec	*pspec);
static void	mn_mailbox_get_property (GObject	*object,
					 unsigned int	prop_id,
					 GValue		*value,
					 GParamSpec	*pspec);

static void	mn_mailbox_monitor_cb	(GnomeVFSMonitorHandle     *handle,
					 const char                *monitor_uri,
					 const char                *info_uri,
					 GnomeVFSMonitorEventType  event_type,
					 gpointer                  user_data);

static void mn_mailbox_new_async_test_cb (gboolean result, gpointer user_data);
static void mn_mailbox_new_async_continue (NewInfo *info);
static void mn_mailbox_new_async_is_cb (gboolean result, gpointer user_data);
static void mn_mailbox_new_async_unsupported (NewInfo *info, const char *reason);
static void mn_mailbox_new_async_finish (NewInfo *info);

/*** implementation **********************************************************/

const GType *
mn_mailbox_get_types (void)
{
  static GType *types = NULL;

  if (! types)
    {
      GType tmp_types[5];
      int n_types = 0;
      int i;

#ifdef WITH_MBOX
      tmp_types[n_types++] = MN_TYPE_MBOX_MAILBOX;
#endif
#ifdef WITH_MH
      tmp_types[n_types++] = MN_TYPE_MH_MAILBOX;
#endif
#ifdef WITH_MAILDIR
      tmp_types[n_types++] = MN_TYPE_MAILDIR_MAILBOX;
#endif
#ifdef WITH_POP3
      tmp_types[n_types++] = MN_TYPE_POP3_MAILBOX;
#endif
#ifdef WITH_SYLPHEED
      tmp_types[n_types++] = MN_TYPE_SYLPHEED_MAILBOX;
#endif

      types = g_new(GType, n_types + 1);
      for (i = 0; i < n_types; i++)
	types[i] = tmp_types[i];
      types[n_types] = 0;
    }

  return types;
}

GType
mn_mailbox_get_type (void)
{
  static GType mailbox_type = 0;
  
  if (! mailbox_type)
    {
      static const GTypeInfo mailbox_info = {
	sizeof(MNMailboxClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_mailbox_class_init,
	NULL,
	NULL,
	sizeof(MNMailbox),
	0,
	(GInstanceInitFunc) mn_mailbox_init,
      };
      
      mailbox_type = g_type_register_static(G_TYPE_OBJECT,
					    "MNMailbox",
					    &mailbox_info,
					    G_TYPE_FLAG_ABSTRACT);
    }
  
  return mailbox_type;
}

static void
mn_mailbox_class_init (MNMailboxClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS(class);

  parent_class = g_type_class_peek_parent(class);

  object_class->set_property = mn_mailbox_set_property;
  object_class->get_property = mn_mailbox_get_property;
  object_class->finalize = mn_mailbox_finalize;

  g_object_class_install_property(object_class,
                                  PROP_URI,
                                  g_param_spec_string("uri",
                                                      _("URI"),
                                                      _("The mailbox URI"),
                                                      NULL,
                                                      G_PARAM_WRITABLE | G_PARAM_READABLE | G_PARAM_CONSTRUCT_ONLY));
  g_object_class_install_property(object_class,
                                  PROP_NAME,
                                  g_param_spec_string("name",
                                                      _("Name"),
                                                      _("The mailbox human-readable name"),
                                                      NULL,
                                                      G_PARAM_READABLE));
  g_object_class_install_property(object_class,
                                  PROP_AUTOMATIC,
                                  g_param_spec_boolean("automatic",
						       _("Automatic"),
						       _("Whether the mailbox has to be manually checked or not"),
						       FALSE,
						       G_PARAM_WRITABLE | G_PARAM_READABLE));
  g_object_class_install_property(object_class,
                                  PROP_HAS_NEW,
                                  g_param_spec_boolean("has-new",
						       _("Has new"),
						       _("Whether the mailbox has new mail or not"),
						       FALSE,
						       G_PARAM_WRITABLE | G_PARAM_READABLE));
  g_object_class_install_property(object_class,
                                  PROP_ERROR,
                                  g_param_spec_string("error",
                                                      _("Error"),
                                                      _("The mailbox error if any"),
                                                      NULL,
                                                      G_PARAM_WRITABLE | G_PARAM_READABLE));
}

static void
mn_mailbox_init (MNMailbox *mailbox)
{
  mailbox->priv = g_new0(MNMailboxPrivate, 1);
}

static void
mn_mailbox_finalize (GObject *object)
{
  MNMailbox *mailbox = MN_MAILBOX(object);

  g_free(mailbox->priv->init_error);
  g_free(mailbox->priv->uri);
  g_free(mailbox->priv->name);
  g_free(mailbox->priv->error);
  if (mailbox->priv->monitor_handle)
    {
      GnomeVFSResult result;

      result = gnome_vfs_monitor_cancel(mailbox->priv->monitor_handle);
      if (result != GNOME_VFS_OK)
	g_warning(_("unable to cancel monitoring of %s: %s"),
		  mailbox->priv->monitor_uri,
		  gnome_vfs_result_to_string(result));
    }
  g_free(mailbox->priv->monitor_uri);
  g_free(mailbox->priv);

  G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void
mn_mailbox_set_property (GObject *object,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *pspec)
{
  MNMailbox *mailbox = MN_MAILBOX(object);

  switch (prop_id)
    {
    case PROP_URI:
      g_return_if_fail(mailbox->priv->uri == NULL);
      mailbox->priv->uri = g_value_dup_string(value);
      mailbox->priv->name = mn_uri_format_for_display(mailbox->priv->uri);
      break;

    case PROP_AUTOMATIC:
      mailbox->priv->automatic = g_value_get_boolean(value);
      break;

    case PROP_HAS_NEW:
      mailbox->priv->has_new = g_value_get_boolean(value);
      break;

    case PROP_ERROR:
      g_free(mailbox->priv->error);
      mailbox->priv->error = g_value_dup_string(value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
}

void
mn_mailbox_set_automatic (MNMailbox *mailbox, gboolean automatic)
{
  g_return_if_fail(MN_IS_MAILBOX(mailbox));
  
  g_object_set(G_OBJECT(mailbox), "automatic", automatic, NULL);
}

void
mn_mailbox_set_has_new (MNMailbox *mailbox, gboolean has_new)
{
  g_return_if_fail(MN_IS_MAILBOX(mailbox));
  
  g_object_set(G_OBJECT(mailbox), "has-new", has_new, NULL);
}

void
mn_mailbox_set_error (MNMailbox *mailbox, const char *format, ...)
{
  char *error = NULL;

  g_return_if_fail(MN_IS_MAILBOX(mailbox));

  if (format)
    {
      va_list args;
      
      va_start(args, format);
      error = g_strdup_vprintf(format, args);
      va_end(args);
    }

  g_object_set(G_OBJECT(mailbox), "error", error, NULL);
  g_free(error);
}

static void
mn_mailbox_get_property (GObject *object,
			 unsigned int prop_id,
			 GValue *value,
			 GParamSpec *pspec)
{
  MNMailbox *mailbox = MN_MAILBOX(object);

  switch (prop_id)
    {
    case PROP_URI:
      g_value_set_string(value, mn_mailbox_get_uri(mailbox));
      break;

    case PROP_NAME:
      g_value_set_string(value, mn_mailbox_get_name(mailbox));
      break;

    case PROP_AUTOMATIC:
      g_value_set_boolean(value, mn_mailbox_get_automatic(mailbox));
      break;

    case PROP_HAS_NEW:
      g_value_set_boolean(value, mn_mailbox_get_has_new(mailbox));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
}

const char *
mn_mailbox_get_uri (MNMailbox *mailbox)
{
  g_return_val_if_fail(MN_IS_MAILBOX(mailbox), NULL);

  return mailbox->priv->uri;
}

const char *
mn_mailbox_get_name (MNMailbox *mailbox)
{
  g_return_val_if_fail(MN_IS_MAILBOX(mailbox), NULL);

  return mailbox->priv->name;
}

gboolean
mn_mailbox_get_automatic (MNMailbox *mailbox)
{
  g_return_val_if_fail(MN_IS_MAILBOX(mailbox), FALSE);

  return mailbox->priv->automatic;
}

gboolean
mn_mailbox_get_has_new (MNMailbox *mailbox)
{
  g_return_val_if_fail(MN_IS_MAILBOX(mailbox), FALSE);

  return mailbox->priv->has_new;
}

const char *
mn_mailbox_get_error (MNMailbox *mailbox)
{
  g_return_val_if_fail(MN_IS_MAILBOX(mailbox), NULL);

  return mailbox->priv->error;
}

void
mn_mailbox_new_async (const char *uri,
		      MNMailboxNewAsyncCallback *callback,
		      gpointer user_data)
{
  NewInfo *info;
  GnomeVFSURI *vfs_uri;

  g_return_if_fail(uri != NULL);
  g_return_if_fail(callback != NULL);

  info = g_new(NewInfo, 1);
  info->uri = g_strdup(uri);
  info->types = mn_mailbox_get_types();
  info->i = 0;
  info->callback = callback;
  info->user_data = user_data;

  vfs_uri = gnome_vfs_uri_new(uri);
  if (vfs_uri)
    {
      mn_vfs_async_test(uri, G_FILE_TEST_EXISTS, mn_mailbox_new_async_test_cb, info);
      gnome_vfs_uri_unref(vfs_uri);
    }
  else
    mn_mailbox_new_async_continue(info);
}

static void
mn_mailbox_new_async_test_cb (gboolean result, gpointer user_data)
{
  NewInfo *info = user_data;

  if (result)
    mn_mailbox_new_async_continue(info);
  else
    mn_mailbox_new_async_unsupported(info, _("does not exist"));
}

static void
mn_mailbox_new_async_continue (NewInfo *info)
{
  g_return_if_fail(info != NULL);

  if (info->types[info->i])
    {
      MNMailboxClass *class;
      
      class = g_type_class_ref(info->types[info->i]);
      class->is(info->uri, mn_mailbox_new_async_is_cb, info);
      g_type_class_unref(class);
    }
  else
    mn_mailbox_new_async_unsupported(info, _("unknown format"));
}

static void
mn_mailbox_new_async_is_cb (gboolean result, gpointer user_data)
{
  NewInfo *info = user_data;

  if (result)
    {
      MNMailbox *mailbox;

      mailbox = g_object_new(info->types[info->i], "uri", info->uri, NULL);
      if (! mailbox->priv->init_error)
	{
	  info->mailbox = mailbox;
	  mn_mailbox_new_async_finish(info);
	}
      else
	{
	  mn_mailbox_new_async_unsupported(info, mailbox->priv->init_error);
	  g_object_unref(mailbox);
	}
    }
  else
    {
      info->i++;
      mn_mailbox_new_async_continue(info);
    }
}

static void
mn_mailbox_new_async_unsupported (NewInfo *info, const char *reason)
{
  g_return_if_fail(info != NULL);
  g_return_if_fail(reason != NULL);

  info->mailbox = mn_unsupported_mailbox_new(info->uri, reason);
  mn_mailbox_new_async_finish(info);
}

static void
mn_mailbox_new_async_finish (NewInfo *info)
{
  info->callback(info->mailbox, info->user_data);

  g_free(info->uri);
  /* do not unref info->mailbox, it's now owned by the callback */
  g_free(info);
}

void
mn_mailbox_set_init_error (MNMailbox *mailbox, const char *format, ...)
{
  va_list args;

  g_return_if_fail(MN_IS_MAILBOX(mailbox));
  g_return_if_fail(format != NULL);
  g_return_if_fail(mailbox->priv->init_error == NULL);

  va_start(args, format);
  mailbox->priv->init_error = g_strdup_vprintf(format, args);
  va_end(args);
}

void
mn_mailbox_monitor (MNMailbox *mailbox,
		    const char *uri,
		    GnomeVFSMonitorType monitor_type,
		    MNMailboxMonitorEventType events)
{
  GnomeVFSResult result;

  g_return_if_fail(MN_IS_MAILBOX(mailbox));
  g_return_if_fail(MN_MAILBOX_GET_CLASS(mailbox)->check != NULL);
  g_return_if_fail(mailbox->priv->monitor_handle == NULL);
  g_return_if_fail(uri != NULL);

  result = gnome_vfs_monitor_add(&mailbox->priv->monitor_handle,
				 uri,
				 monitor_type,
				 mn_mailbox_monitor_cb,
				 mailbox);
  if (result == GNOME_VFS_OK)
    {
      mailbox->priv->monitor_uri = g_strdup(uri);
      mailbox->priv->monitor_events = events;
      mn_mailbox_set_automatic(mailbox, TRUE);
    }
  else
    {
      static gboolean first_time = TRUE;

      g_warning(_("unable to monitor %s: %s"), uri, gnome_vfs_result_to_string(result));

      if (first_time)
	{
	  int minutes;
	  int seconds;
	  char *str;
	  
	  first_time = FALSE;

	  minutes = eel_gconf_get_integer(MN_CONF_DELAY_MINUTES);
	  seconds = eel_gconf_get_integer(MN_CONF_DELAY_SECONDS);
	  
	  if (minutes == 0)
	    str = g_strdup_printf(ngettext("As a fallback, they will be "
					   "checked every %i second (this "
					   "delay is configurable from the "
					   "Preferences Dialog).",
					   "As a fallback, they will be "
					   "checked every %i seconds (this "
					   "delay is configurable from the "
					   "Preferences Dialog).",
					   seconds),
				  seconds);
	  else if (seconds == 0)
	    str = g_strdup_printf(ngettext("As a fallback, they will be "
					   "checked every %i minute (this "
					   "delay is configurable from the "
					   "Preferences Dialog).",
					   "As a fallback, they will be "
					   "checked every %i minutes (this "
					   "delay is configurable from the "
					   "Preferences Dialog).",
					   minutes),
				  minutes);
	  else
	    str = g_strdup_printf(ngettext("As a fallback, they will be "
					   "checked approximately every %i "
					   "minute (this delay is "
					   "configurable from the "
					   "Preferences Dialog).",
					   "As a fallback, they will be "
					   "checked approximately every %i "
					   "minutes (this delay is "
					   "configurable from the "
					   "Preferences Dialog).",
					   minutes),
				  minutes);
	  
	  mn_error_dialog("automatic-notification",
			  _("A monitoring error has occurred."),
			  _("Mail Notification was unable to enable automatic "
			    "notification for one or more mailboxes. %s"), str);
	  g_free(str);
	}
    }
}

static void
mn_mailbox_monitor_cb (GnomeVFSMonitorHandle *handle,
		       const char *monitor_uri,
		       const char *info_uri,
		       GnomeVFSMonitorEventType event_type,
		       gpointer user_data)
{
  MNMailbox *mailbox = user_data;

  if (mailbox->priv->monitor_events & (1 << event_type))
    mn_mailbox_check(mailbox);
}

void
mn_mailbox_check (MNMailbox *mailbox)
{
  g_return_if_fail(MN_IS_MAILBOX(mailbox));
  g_return_if_fail(MN_MAILBOX_GET_CLASS(mailbox)->check != NULL);

  if (! mailbox->priv->checking)
    {
      g_object_ref(mailbox);
      mailbox->priv->checking = TRUE;
      mn_mailbox_set_error(mailbox, NULL);

      MN_MAILBOX_GET_CLASS(mailbox)->check(mailbox);
    }
}

void
mn_mailbox_end_check (MNMailbox *mailbox)
{
  g_return_if_fail(MN_IS_MAILBOX(mailbox));
  g_return_if_fail(mailbox->priv->checking == TRUE);

  mailbox->priv->checking = FALSE;
  g_object_unref(mailbox);
}
