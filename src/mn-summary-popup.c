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
#include <glib/gi18n-lib.h>
#include <eel/eel.h>
#include "mn-summary.h"
#include "mn-util.h"
#include "mn-shell.h"
#include "mn-conf.h"
#include "mn-stock.h"
#include "mn-summary-dialog.h"
#include "mn-summary-popup.h"

/*** types *******************************************************************/

typedef struct
{
  gboolean	enabled;

  GtkWidget	*popup;
  GtkWidget	*image;
  GtkWidget	*vbox;
  unsigned int	timeout_id;
} SummaryPopup;

/*** variables ***************************************************************/

static SummaryPopup popup = { FALSE };
  
/*** functions ***************************************************************/
    
static void mn_summary_popup_notify_enable_cb (GConfClient *client,
					       guint cnxn_id,
					       GConfEntry *entry,
					       gpointer user_data);
static void mn_summary_popup_notify_autoclose_cb (GConfClient *client,
						  guint cnxn_id,
						  GConfEntry *entry,
						  gpointer user_data);
static void mn_summary_popup_notify_delay_cb (GConfClient *client,
					      guint cnxn_id,
					      GConfEntry *entry,
					      gpointer user_data);
static void mn_summary_popup_notify_geometry_cb (GConfClient *client,
						 guint cnxn_id,
						 GConfEntry *entry,
						 gpointer user_data);

static void mn_summary_popup_enable (void);
static void mn_summary_popup_disable (void);

static void mn_summary_popup_messages_changed_h (MNMailboxes *mailboxes,
						 gboolean has_new,
						 gpointer user_data);

static void mn_summary_popup_update (gboolean has_new);
static void mn_summary_popup_set_geometry (void);

static void mn_summary_popup_install_timeout (void);
static void mn_summary_popup_remove_timeout (void);

static gboolean mn_summary_popup_timeout_cb (gpointer data);

/*** implementation **********************************************************/

void
mn_summary_popup_init (void)
{
  g_return_if_fail(mn_shell != NULL);

  if (eel_gconf_get_boolean(MN_CONF_MAIL_SUMMARY_POPUP_ENABLE))
    mn_summary_popup_enable();

  eel_gconf_notification_add(MN_CONF_MAIL_SUMMARY_POPUP_ENABLE, mn_summary_popup_notify_enable_cb, NULL);
  eel_gconf_notification_add(MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE, mn_summary_popup_notify_autoclose_cb, NULL);
  eel_gconf_notification_add(MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_NAMESPACE, mn_summary_popup_notify_delay_cb, NULL);
  eel_gconf_notification_add(MN_CONF_MAIL_SUMMARY_POPUP_POSITION, mn_summary_popup_notify_geometry_cb, NULL);
  eel_gconf_notification_add(MN_CONF_MAIL_SUMMARY_POPUP_OFFSET_NAMESPACE, mn_summary_popup_notify_geometry_cb, NULL);
}

static void
mn_summary_popup_notify_enable_cb (GConfClient *client,
				   guint cnxn_id,
				   GConfEntry *entry,
				   gpointer user_data)
{
  GDK_THREADS_ENTER();

  if (eel_gconf_get_boolean(MN_CONF_MAIL_SUMMARY_POPUP_ENABLE))
    mn_summary_popup_enable();
  else
    mn_summary_popup_disable();

  GDK_THREADS_LEAVE();
}

static void
mn_summary_popup_notify_autoclose_cb (GConfClient *client,
				     guint cnxn_id,
				     GConfEntry *entry,
				     gpointer user_data)
{
  GDK_THREADS_ENTER();

  if (popup.enabled)
    {
      if (eel_gconf_get_boolean(MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE))
	{
	  if (popup.popup && ! popup.timeout_id)
	    mn_summary_popup_install_timeout();
	}
      else
	mn_summary_popup_remove_timeout();
    }

  GDK_THREADS_LEAVE();
}

static void
mn_summary_popup_notify_delay_cb (GConfClient *client,
				  guint cnxn_id,
				  GConfEntry *entry,
				  gpointer user_data)
{
  GDK_THREADS_ENTER();

  if (popup.enabled && popup.popup && eel_gconf_get_boolean(MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE))
    {
      mn_summary_popup_remove_timeout();
      mn_summary_popup_install_timeout();
    }

  GDK_THREADS_LEAVE();
}

static void
mn_summary_popup_notify_geometry_cb (GConfClient *client,
				     guint cnxn_id,
				     GConfEntry *entry,
				     gpointer user_data)
{
  GDK_THREADS_ENTER();

  if (popup.enabled && popup.popup)
    mn_summary_popup_set_geometry();

  GDK_THREADS_LEAVE();
}

static void
mn_summary_popup_enable (void)
{
  if (! popup.enabled)
    {
      popup.enabled = TRUE;
      g_signal_connect(mn_shell->mailboxes, "messages-changed", G_CALLBACK(mn_summary_popup_messages_changed_h), NULL);
    }
}

static void
mn_summary_popup_disable (void)
{
  if (popup.enabled)
    {
      popup.enabled = FALSE;
      g_signal_handlers_disconnect_by_func(mn_shell->mailboxes, mn_summary_popup_messages_changed_h, NULL);
      mn_summary_popup_destroy();
    }
}

static void
mn_summary_popup_messages_changed_h (MNMailboxes *mailboxes,
				     gboolean has_new,
				     gpointer user_data)
{
  mn_summary_popup_update(has_new);
}

static void
mn_summary_popup_update (gboolean has_new)
{
  gboolean displayed = FALSE;

  if (popup.popup)
    {
      displayed = TRUE;
      gtk_widget_destroy(popup.popup);
    }

  if (! mn_summary_dialog_is_displayed() && (displayed || has_new))
    {
      GSList *messages;

      messages = mn_mailboxes_get_messages(mn_shell->mailboxes);
      if (messages)
	{
	  mn_create_interface("summary-popup",
			      "popup", &popup.popup,
			      "image", &popup.image,
			      "vbox", &popup.vbox,
			      NULL);
	  eel_add_weak_pointer(&popup.popup);
	  
	  /*
	   * The popup must:
	   *
	   *	- be sticky
	   *	- be kept above other windows
	   *	- not be focusable
	   *	- not be decorated
	   *	- not appear in the pager
	   *	- not appear in the taskbar
	   *
	   * An easy way to meet these requirements is to use a window
	   * of type GTK_WINDOW_POPUP, but unfortunately these are not
	   * hidden by xscreensaver
	   * (http://bugzilla.gnome.org/show_bug.cgi?id=154529).
	   *
	   * We therefore use a window of type GTK_WINDOW_TOPLEVEL,
	   * and we set the appropriate properties in the glade file
	   * and below (but a drawback is
	   * http://bugzilla.gnome.org/show_bug.cgi?id=154593).
	   */

	  gtk_window_stick(GTK_WINDOW(popup.popup));
	  gtk_window_set_keep_above(GTK_WINDOW(popup.popup), TRUE);
	  gtk_window_set_accept_focus(GTK_WINDOW(popup.popup), FALSE);

	  gtk_widget_realize(popup.popup);
	  gdk_window_set_decorations(popup.popup->window, GDK_DECOR_BORDER);

	  gtk_image_set_from_stock(GTK_IMAGE(popup.image), MN_STOCK_MAIL_SUMMARY, -1);

	  mn_summary_update(GTK_VBOX(popup.vbox), messages, FALSE);
	  mn_g_object_slist_free(messages);

	  mn_summary_popup_set_geometry();
	  gtk_widget_show(popup.popup);

	  /* we only reset the timeout if there is new mail */
	  
	  if (eel_gconf_get_boolean(MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE) && has_new)
	    {
	      mn_summary_popup_remove_timeout();
	      mn_summary_popup_install_timeout();
	    }
	}
    }
}

static void
mn_summary_popup_set_geometry (void)
{
  GEnumValue *enum_value;
  MNPosition position;
  char *geometry;

  g_return_if_fail(popup.popup != NULL);

  enum_value = mn_conf_get_enum_value(MN_TYPE_POSITION, MN_CONF_MAIL_SUMMARY_POPUP_POSITION);
  position = enum_value ? enum_value->value : MN_POSITION_TOP_LEFT;
  
  geometry = g_strdup_printf("%c%i%c%i",
			     (position == MN_POSITION_TOP_RIGHT || position == MN_POSITION_BOTTOM_RIGHT) ? '-' : '+',
			     eel_gconf_get_integer(MN_CONF_MAIL_SUMMARY_POPUP_HORIZONTAL_OFFSET),
			     (position == MN_POSITION_BOTTOM_LEFT || position == MN_POSITION_BOTTOM_RIGHT) ? '-' : '+',
			     eel_gconf_get_integer(MN_CONF_MAIL_SUMMARY_POPUP_VERTICAL_OFFSET));
  gtk_window_parse_geometry(GTK_WINDOW(popup.popup), geometry);
  g_free(geometry);
}

void
mn_summary_popup_destroy (void)
{
  if (popup.popup)
    {
      mn_summary_popup_remove_timeout();
      gtk_widget_destroy(popup.popup);
    }
}

static void
mn_summary_popup_install_timeout (void)
{
  int minutes;
  int seconds;

  g_return_if_fail(popup.timeout_id == 0);

  minutes = eel_gconf_get_integer(MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_MINUTES);
  seconds = eel_gconf_get_integer(MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_SECONDS);

  if (minutes != 0 || seconds != 0)
    popup.timeout_id = g_timeout_add(((minutes * 60) + seconds) * 1000,
				     mn_summary_popup_timeout_cb,
				     NULL);
}

static void
mn_summary_popup_remove_timeout (void)
{
  if (popup.timeout_id)
    {
      g_source_remove(popup.timeout_id);
      popup.timeout_id = 0;
    }
}

static gboolean
mn_summary_popup_timeout_cb (gpointer data)
{
  GDK_THREADS_ENTER();
  gtk_widget_destroy(popup.popup);
  GDK_THREADS_LEAVE();
  
  return FALSE;			/* remove timeout */
}

/* libglade callbacks */

gboolean
mn_summary_popup_button_press_event_h (GtkWidget *widget,
				       GdkEventButton *button,
				       gpointer user_data)
{
  mn_summary_popup_destroy();

  return TRUE;			/* do not propagate event */
}
