/* 
 * Copyright (C) 2004, 2005 Jean-Yves Lefort <jylefort@brutele.be>
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
#include <glib/gi18n.h>
#include <eel/eel.h>
#include "mn-util.h"
#include "mn-shell.h"
#include "mn-conf.h"
#include "mn-stock.h"
#include "mn-main-window.h"
#include "mn-summary-popup.h"
#include "mn-message-view.h"

/*** types *******************************************************************/

typedef struct
{
  gboolean	enabled;
  GtkWidget	*popup;
  GtkWidget	*image;
  GtkWidget	*title;
  GtkWidget	*message_view;
  unsigned int	timeout_id;

  GSList	*displayed_messages;
  GSList	*previous_messages;

  MNPosition	position;
  int		horizontal_offset;
  int		vertical_offset;
  GdkGravity	gravity;
} SummaryPopup;

/*** variables ***************************************************************/

static SummaryPopup self = {
  FALSE,			/* enabled */
  NULL,				/* popup */
  NULL,				/* image */
  NULL,				/* title */
  NULL,				/* message_view */
  0,				/* timeout_id */

  NULL,				/* displayed_messages */
  NULL,				/* previous_messages */

  -1,				/* position */
  0,				/* horizontal_offset */
  0,				/* vertical_offset */
  0				/* gravity */
};
  
/*** functions ***************************************************************/
    
static void mn_summary_popup_notify_enable_cb (GConfClient *client,
					       unsigned int cnxn_id,
					       GConfEntry *entry,
					       gpointer user_data);
static void mn_summary_popup_notify_autoclose_cb (GConfClient *client,
						  unsigned int cnxn_id,
						  GConfEntry *entry,
						  gpointer user_data);
static void mn_summary_popup_notify_delay_cb (GConfClient *client,
					      unsigned int cnxn_id,
					      GConfEntry *entry,
					      gpointer user_data);
static void mn_summary_popup_notify_geometry_cb (GConfClient *client,
						 unsigned int cnxn_id,
						 GConfEntry *entry,
						 gpointer user_data);
static void mn_summary_popup_notify_fonts_aspect_source_cb (GConfClient *client,
							    unsigned int cnxn_id,
							    GConfEntry *entry,
							    gpointer user_data);
static void mn_summary_popup_notify_fonts_title_font_cb (GConfClient *client,
							 unsigned int cnxn_id,
							 GConfEntry *entry,
							 gpointer user_data);
static void mn_summary_popup_notify_fonts_contents_font_cb (GConfClient *client,
							    unsigned int cnxn_id,
							    GConfEntry *entry,
							    gpointer user_data);

static void mn_summary_popup_enable (void);
static void mn_summary_popup_disable (void);

static void mn_summary_popup_messages_changed_h (MNMailboxes *mailboxes,
						 gboolean has_new,
						 gpointer user_data);

static void mn_summary_popup_update (gboolean has_new);
static void mn_summary_popup_weak_notify_cb (gpointer data,
					     GObject *former_object);

static void mn_summary_popup_set_geometry (void);
static void mn_summary_popup_set_title_font (void);
static void mn_summary_popup_set_contents_font (void);

static void mn_summary_popup_install_timeout (void);

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
  eel_gconf_notification_add(MN_CONF_MAIL_SUMMARY_POPUP_FONTS_ASPECT_SOURCE, mn_summary_popup_notify_fonts_aspect_source_cb, NULL);
  eel_gconf_notification_add(MN_CONF_MAIL_SUMMARY_POPUP_FONTS_TITLE_FONT, mn_summary_popup_notify_fonts_title_font_cb, NULL);
  eel_gconf_notification_add(MN_CONF_MAIL_SUMMARY_POPUP_FONTS_CONTENTS_FONT, mn_summary_popup_notify_fonts_contents_font_cb, NULL);
}

static void
mn_summary_popup_notify_enable_cb (GConfClient *client,
				   unsigned int cnxn_id,
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
				     unsigned int cnxn_id,
				     GConfEntry *entry,
				     gpointer user_data)
{
  GDK_THREADS_ENTER();

  if (self.enabled)
    {
      if (eel_gconf_get_boolean(MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE))
	{
	  if (self.popup && ! self.timeout_id)
	    mn_summary_popup_install_timeout();
	}
      else
	mn_source_remove(&self.timeout_id);
    }

  GDK_THREADS_LEAVE();
}

static void
mn_summary_popup_notify_delay_cb (GConfClient *client,
				  unsigned int cnxn_id,
				  GConfEntry *entry,
				  gpointer user_data)
{
  GDK_THREADS_ENTER();

  if (self.enabled && self.popup && eel_gconf_get_boolean(MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE))
    {
      mn_source_remove(&self.timeout_id);
      mn_summary_popup_install_timeout();
    }

  GDK_THREADS_LEAVE();
}

static void
mn_summary_popup_notify_geometry_cb (GConfClient *client,
				     unsigned int cnxn_id,
				     GConfEntry *entry,
				     gpointer user_data)
{
  GDK_THREADS_ENTER();

  self.position = -1;		/* invalidate the position */

  if (self.enabled && self.popup)
    mn_summary_popup_set_geometry();

  GDK_THREADS_LEAVE();
}

static void
mn_summary_popup_notify_fonts_aspect_source_cb (GConfClient *client,
						unsigned int cnxn_id,
						GConfEntry *entry,
						gpointer user_data)
{
  GDK_THREADS_ENTER();

  if (self.enabled && self.popup)
    {
      mn_summary_popup_set_title_font();
      mn_summary_popup_set_contents_font();
    }

  GDK_THREADS_LEAVE();
}

static void
mn_summary_popup_notify_fonts_title_font_cb (GConfClient *client,
					     unsigned int cnxn_id,
					     GConfEntry *entry,
					     gpointer user_data)
{
  GDK_THREADS_ENTER();

  if (self.enabled && self.popup)
    mn_summary_popup_set_title_font();

  GDK_THREADS_LEAVE();
}

static void
mn_summary_popup_notify_fonts_contents_font_cb (GConfClient *client,
						unsigned int cnxn_id,
						GConfEntry *entry,
						gpointer user_data)
{
  GDK_THREADS_ENTER();

  if (self.enabled && self.popup)
    mn_summary_popup_set_contents_font();

  GDK_THREADS_LEAVE();
}

static void
mn_summary_popup_enable (void)
{
  if (! self.enabled)
    {
      self.enabled = TRUE;
      g_signal_connect(mn_shell->mailboxes, "messages-changed", G_CALLBACK(mn_summary_popup_messages_changed_h), NULL);
    }
}

static void
mn_summary_popup_disable (void)
{
  if (self.enabled)
    {
      self.enabled = FALSE;
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
  if (! mn_main_window_is_displayed() && (self.popup || has_new))
    {
      GSList *l;

      mn_g_object_slist_free(self.displayed_messages);
      self.displayed_messages = mn_mailboxes_get_messages(mn_shell->mailboxes);

    loop1:
      MN_LIST_FOREACH(l, self.previous_messages)
        {
	  MNMessage *message = l->data;

	  if (! mn_message_slist_find_by_id(self.displayed_messages, message))
	    {
	      self.previous_messages = mn_g_object_slist_delete_link(self.previous_messages, l);
	      goto loop1;
	    }
	}
      
      if (eel_gconf_get_boolean(MN_CONF_MAIL_SUMMARY_POPUP_ONLY_RECENT))
	{
	loop2:
	  MN_LIST_FOREACH(l, self.displayed_messages)
	    {
	      MNMessage *message = l->data;
	    
	      if (mn_message_slist_find_by_id(self.previous_messages, message))
		{
		  self.displayed_messages = mn_g_object_slist_delete_link(self.displayed_messages, l);
		  goto loop2;
		}
	    }
	}

      if (self.displayed_messages)
	{
	  if (! self.popup)
	    {
	      mn_create_interface(MN_INTERFACE_FILE("summary-popup.glade"),
				  "mn-mail-summary-popup", &self.popup,
				  "image", &self.image,
				  "mn-mail-summary-popup-title", &self.title,
				  "mn-message-view", &self.message_view,
				  NULL);

	      eel_add_weak_pointer(&self.popup);
	      g_object_weak_ref(G_OBJECT(self.popup), mn_summary_popup_weak_notify_cb, NULL);

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
	       * An easy way to meet these requirements is to use a
	       * window of type GTK_WINDOW_POPUP, but unfortunately
	       * these are not hidden by xscreensaver
	       * (http://bugzilla.gnome.org/show_bug.cgi?id=154529).
	       *
	       * We therefore use a window of type
	       * GTK_WINDOW_TOPLEVEL, and we set the appropriate
	       * properties in the glade file and below (but a
	       * drawback is
	       * http://bugzilla.gnome.org/show_bug.cgi?id=154593).
	       */

	      gtk_window_stick(GTK_WINDOW(self.popup));
	      gtk_window_set_keep_above(GTK_WINDOW(self.popup), TRUE);
	      gtk_window_set_accept_focus(GTK_WINDOW(self.popup), FALSE);
	      
	      gtk_widget_realize(self.popup);
	      gdk_window_set_decorations(self.popup->window, GDK_DECOR_BORDER);
	      
	      gtk_image_set_from_stock(GTK_IMAGE(self.image), MN_STOCK_MAIL_SUMMARY, -1);

	      mn_summary_popup_set_title_font();
	    }
	  
	  mn_message_view_set_messages(MN_MESSAGE_VIEW(self.message_view), self.displayed_messages);
	  mn_summary_popup_set_contents_font();
	  mn_summary_popup_set_geometry();
	  gtk_widget_show(self.popup);

	  /* we only reset the timeout if there is new mail */
	  
	  if (has_new && eel_gconf_get_boolean(MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE))
	    {
	      mn_source_remove(&self.timeout_id);
	      mn_summary_popup_install_timeout();
	    }

	  return;		/* something has been shown */
	}
    }

  /* nothing has been shown, destroy the popup if it exists */
  mn_summary_popup_destroy();
}

static void
mn_summary_popup_weak_notify_cb (gpointer data, GObject *former_object)
{
  mn_source_remove(&self.timeout_id);

  self.previous_messages = g_slist_concat(self.previous_messages, self.displayed_messages);
  self.displayed_messages = NULL;
}

static void
mn_summary_popup_set_geometry (void)
{
  int popup_width;
  int popup_height;
  int screen_width;
  int screen_height;
  GdkGravity gravity_mapping[] = {
    GDK_GRAVITY_NORTH_WEST,
    GDK_GRAVITY_NORTH_EAST,
    GDK_GRAVITY_SOUTH_WEST,
    GDK_GRAVITY_SOUTH_EAST
  };
  GdkGravity gravity;
  int x;
  int y;
  gboolean reshow;

  g_return_if_fail(self.popup != NULL);

  if (self.position == -1)
    {
      self.position = mn_conf_get_enum_value(MN_TYPE_POSITION, MN_CONF_MAIL_SUMMARY_POPUP_POSITION);
      self.horizontal_offset = eel_gconf_get_integer(MN_CONF_MAIL_SUMMARY_POPUP_HORIZONTAL_OFFSET);
      self.vertical_offset = eel_gconf_get_integer(MN_CONF_MAIL_SUMMARY_POPUP_VERTICAL_OFFSET);
    }

  gtk_window_get_size(GTK_WINDOW(self.popup), &popup_width, &popup_height);
  screen_width = gdk_screen_width();
  screen_height = gdk_screen_height();

  g_return_if_fail(self.position >= 0 && self.position < G_N_ELEMENTS(gravity_mapping));
  gravity = gravity_mapping[self.position];

  x = (gravity == GDK_GRAVITY_NORTH_WEST || gravity == GDK_GRAVITY_SOUTH_WEST)
    ? self.horizontal_offset
    : screen_width - popup_width - self.horizontal_offset;
  y = (gravity == GDK_GRAVITY_NORTH_WEST || gravity == GDK_GRAVITY_NORTH_EAST)
    ? self.vertical_offset
    : screen_height - popup_height - self.vertical_offset;

  /*
   * If the gravity changes, the gtk_window_move() call will misplace
   * the window (probably a Metacity bug).
   *
   * A workaround is to hide and reshow the window.
   */
  if (gravity != self.gravity)
    {
      self.gravity = gravity;
      reshow = TRUE;
    }
  else
    reshow = FALSE;
  
  if (reshow)
    gtk_widget_hide(self.popup);

  gtk_window_set_gravity(GTK_WINDOW(self.popup), gravity);
  gtk_window_move(GTK_WINDOW(self.popup), x, y);

  if (reshow)
    gtk_widget_show(self.popup);
}

static void
mn_summary_popup_set_title_font (void)
{
  PangoFontDescription *font_desc = NULL;

  if (mn_conf_get_enum_value(MN_TYPE_ASPECT_SOURCE, MN_CONF_MAIL_SUMMARY_POPUP_FONTS_ASPECT_SOURCE) == MN_ASPECT_SOURCE_CUSTOM)
    {
      char *str;

      str = eel_gconf_get_string(MN_CONF_MAIL_SUMMARY_POPUP_FONTS_TITLE_FONT);
      if (str)
	{
	  font_desc = pango_font_description_from_string(str);
	  g_free(str);
	}
    }

  gtk_widget_modify_font(self.title, font_desc);
  if (font_desc)
    pango_font_description_free(font_desc);
}

static void
mn_summary_popup_set_contents_font (void)
{
  PangoFontDescription *font_desc = NULL;

  if (mn_conf_get_enum_value(MN_TYPE_ASPECT_SOURCE, MN_CONF_MAIL_SUMMARY_POPUP_FONTS_ASPECT_SOURCE) == MN_ASPECT_SOURCE_CUSTOM)
    {
      char *str;

      str = eel_gconf_get_string(MN_CONF_MAIL_SUMMARY_POPUP_FONTS_CONTENTS_FONT);
      if (str)
	{
	  font_desc = pango_font_description_from_string(str);
	  g_free(str);
	}
    }

  gtk_widget_modify_font(GTK_WIDGET(self.message_view), font_desc);
  if (font_desc)
    pango_font_description_free(font_desc);
}

void
mn_summary_popup_destroy (void)
{
  if (self.popup)
    gtk_widget_destroy(self.popup);
}

static void
mn_summary_popup_install_timeout (void)
{
  g_return_if_fail(self.timeout_id == 0);

  self.timeout_id = mn_timeout_add(MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_MINUTES,
				   MN_CONF_MAIL_SUMMARY_POPUP_AUTOCLOSE_DELAY_SECONDS,
				   mn_summary_popup_timeout_cb,
				   NULL);
}

static gboolean
mn_summary_popup_timeout_cb (gpointer data)
{
  GDK_THREADS_ENTER();
  mn_summary_popup_destroy();
  GDK_THREADS_LEAVE();
  
  return FALSE;			/* remove timeout */
}

/* libglade callbacks */

gboolean
mn_summary_popup_button_press_event_h (GtkWidget *widget,
				       GdkEventButton *button,
				       gpointer user_data)
{
  gtk_widget_destroy(self.popup);

  return TRUE;			/* do not propagate event */
}

GtkWidget *
mn_summary_popup_message_view_new_cb (void)
{
  GtkWidget *view;

  view = mn_message_view_new(TRUE, FALSE);
  gtk_widget_show(view);

  return view;
}
