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
#include <gconf/gconf-client.h>
#include <string.h>
#include "mn-check.h"
#include "mn-dialog.h"
#include "mn-mailboxes.h"
#include "mn-preferences.h"
#include "mn-util.h"

/*** variables ***************************************************************/

static GConfClient *mn_conf_client;

/*** functions ***************************************************************/

static void	mn_conf_notify_all_cb		(GConfClient	*client,
						 guint		cnxn_id,
						 GConfEntry	*entry,
						 gpointer	user_data);
static void	mn_conf_notify_mailboxes_cb	(GConfClient	*client,
						 guint		cnxn_id,
						 GConfEntry	*entry,
						 gpointer	user_data);
static void	mn_conf_install_notify		(const char *namespace_section,
						 GConfClientNotifyFunc func);
static int	mn_conf_mailbox_cmp		(gconstpointer	a,
						 gconstpointer	b);

/*** implementation **********************************************************/

static void
mn_conf_notify_all_cb (GConfClient *client,
		       guint cnxn_id,
		       GConfEntry *entry,
		       gpointer user_data)
{
  mn_preferences_update_values();
  mn_check_install();
}

static void
mn_conf_notify_mailboxes_cb (GConfClient *client,
			     guint cnxn_id,
			     GConfEntry *entry,
			     gpointer user_data)
{
  mn_mailboxes_register_all();
  mn_preferences_update_values();
}

static void
mn_conf_install_notify (const char *namespace_section,
			GConfClientNotifyFunc func)
{
  GError *err = NULL;
  
  gconf_client_notify_add(mn_conf_client,
			  namespace_section,
			  func,
			  NULL,
			  NULL,
			  &err);
  if (err)
    {
      mn_error_dialog(_("Initialization error."),
		      _("Error while calling <i>gconf_client_notify_add()</i>: %s."),
		      err->message);
      g_error_free(err);
    }
}

void
mn_conf_init (void)
{
  GError *err = NULL;

  mn_conf_client = gconf_client_get_default();

  gconf_client_add_dir(mn_conf_client,
		       "/apps/mail-notification",
		       GCONF_CLIENT_PRELOAD_NONE,
		       &err);
  if (err)
    {
      mn_error_dialog(_("Initialization error."),
		      _("Error while calling <i>gconf_client_add_dir()</i>: %s."),
		      err->message);
      g_error_free(err);
    }
  
  mn_conf_install_notify("/apps/mail-notification", mn_conf_notify_all_cb);
  mn_conf_install_notify("/apps/mail-notification/mailboxes", mn_conf_notify_mailboxes_cb);
}

void
mn_conf_deinit (void)
{
  g_object_unref(G_OBJECT(mn_conf_client));
}

gboolean
mn_conf_get_bool (const char *key)
{
  GError *err = NULL;
  gboolean value;

  value = gconf_client_get_bool(mn_conf_client, key, &err);
  if (err)
    {
      g_warning(_("unable to read in configuration key %s: %s"), key, err->message);
      g_error_free(err);

      value = FALSE;		/* fallback */
    }

  return value;
}

int
mn_conf_get_int (const char *key)
{
  GError *err = NULL;
  int value;

  value = gconf_client_get_int(mn_conf_client, key, &err);
  if (err)
    {
      g_warning(_("unable to read in configuration key %s: %s"), key, err->message);
      g_error_free(err);

      value = 0;		/* fallback */
    }

  return value;
}

char *
mn_conf_get_string (const char *key)
{
  GError *err = NULL;
  char *value;

  value = gconf_client_get_string(mn_conf_client, key, &err);
  if (err)
    {
      g_warning(_("unable to read in configuration key %s: %s"), key, err->message);
      g_error_free(err);
      
      value = NULL;		/* fallback */
    }

  return value;
}

GSList *
mn_conf_get_list (const char *key, GConfValueType list_type)
{
  GError *err = NULL;
  GSList *value;

  value = gconf_client_get_list(mn_conf_client, key, list_type, &err);
  if (err)
    {
      g_warning(_("unable to read in configuration key %s: %s"), key, err->message);
      g_error_free(err);
      
      value = NULL;		/* fallback */
    }

  return value;
}

void
mn_conf_set_bool (const char *key, gboolean value)
{
  GError *err = NULL;

  gconf_client_set_bool(mn_conf_client, key, value, &err);
  if (err)
    {
      g_warning(_("unable to write in configuration key %s: %s"), key, err->message);
      g_error_free(err);
    }
}

void
mn_conf_set_int (const char *key, int value)
{
  GError *err = NULL;

  gconf_client_set_int(mn_conf_client, key, value, &err);
  if (err)
    {
      g_warning(_("unable to write in configuration key %s: %s"), key, err->message);
      g_error_free(err);
    }
}

void
mn_conf_set_string (const char *key, const char *value)
{
  GError *err = NULL;

  gconf_client_set_string(mn_conf_client, key, value, &err);
  if (err)
    {
      g_warning(_("unable to write in configuration key %s: %s"), key, err->message);
      g_error_free(err);
    }
}

void
mn_conf_set_list (const char *key, GConfValueType list_type, GSList *value)
{
  GError *err = NULL;

  gconf_client_set_list(mn_conf_client, key, list_type, value, &err);
  if (err)
    {
      g_warning(_("unable to write in configuration key %s: %s"), key, err->message);
      g_error_free(err);
    }
}

void
mn_conf_unset (const char *key)
{
  GError *err = NULL;

  gconf_client_unset(mn_conf_client, key, &err);
  if (err)
    {
      g_warning(_("unable to unset configuration key %s: %s"), key, err->message);
      g_error_free(err);
    }
}

void
mn_conf_remove_mailbox (const char *locator)
{
  GSList *gconf_mailboxes;
  GSList *elem;

  gconf_mailboxes = mn_conf_get_list("/apps/mail-notification/mailboxes", GCONF_VALUE_STRING);
  elem = g_slist_find_custom(gconf_mailboxes, locator, mn_conf_mailbox_cmp);

  if (elem)
    {
      gconf_mailboxes = g_slist_remove_link(gconf_mailboxes, elem);

      g_free(elem->data);
      g_slist_free(elem);
    }

  mn_conf_set_list("/apps/mail-notification/mailboxes",
		   GCONF_VALUE_STRING,
		   gconf_mailboxes);
  mn_slist_free(gconf_mailboxes);
}

/*
 * Just in case strcmp() is implemented as a macro.
 */
static int
mn_conf_mailbox_cmp (gconstpointer a, gconstpointer b)
{
  return strcmp(a, b);
}
