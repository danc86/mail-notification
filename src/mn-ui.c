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
#include <gnome.h>
#include "mn-mail-icon.h"
#include "mn-check.h"
#include "mn-preferences.h"
#include "mn-dialog.h"
#include "mn-util.h"
#include "mn-conf.h"

/*** variables ***************************************************************/

static MNMailIcon *mail_icon;
static GtkWidget *check_for_mail_item;

/*** functions ***************************************************************/

static void mn_ui_icon_init		(void);
static void mn_ui_icon_activate_h	(MNMailIcon	*icon,
					 gpointer	user_data);
static void mn_ui_icon_destroy_h	(GtkObject	*object,
					 gpointer	user_data);

/*** implementation **********************************************************/

void
mn_ui_init (void)
{
  mn_ui_icon_init();
}

static void
mn_ui_icon_init (void)
{
  GladeXML *xml;
  GtkWidget *menu;

  mail_icon = MN_MAIL_ICON(mn_mail_icon_new());

  xml = mn_glade_xml_new("menu");
  menu = glade_xml_get_widget(xml, "menu");
  check_for_mail_item = glade_xml_get_widget(xml, "check_for_mail");
  g_object_unref(xml);

  mn_mail_icon_set_popup_menu(mail_icon, GTK_MENU(menu));

  g_signal_connect(G_OBJECT(mail_icon), "activate",
		   G_CALLBACK(mn_ui_icon_activate_h), NULL);
  g_signal_connect(G_OBJECT(mail_icon), "destroy",
		   G_CALLBACK(mn_ui_icon_destroy_h), NULL);

  gtk_widget_show(GTK_WIDGET(mail_icon));
}

static void
mn_ui_icon_activate_h (MNMailIcon *icon, gpointer user_data)
{
  if (mn_conf_get_bool("/apps/mail-notification/commands/clicked/enabled"))
    {
      const char *command;

      command = mn_conf_get_string("/apps/mail-notification/commands/clicked/command");
      if (command)
	{
	  GError *err = NULL;
	  
	  if (! g_spawn_command_line_async(command, &err))
	    {
	      mn_error_dialog(_("Command error."),
			      _("Unable to execute clicked command: %s."),
			      err->message);
	      g_error_free(err);
	    }
	}
    }
}

static void
mn_ui_icon_destroy_h (GtkObject *object, gpointer user_data)
{
  /* The Notification Area applet has been terminated. Recreate the icon. */
  mn_ui_icon_init();
}

void
mn_ui_set_has_new (gboolean has_new)
{
  mn_mail_icon_set_has_new(mail_icon, has_new);
}

void
mn_ui_set_can_check (gboolean can_check)
{
  gtk_widget_set_sensitive(check_for_mail_item, can_check);
}

/* libglade callbacks */

void
mn_ui_check_for_mail_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  mn_check(MN_CHECK_INTERACTIVE);			/* local mail */
  mn_check(MN_CHECK_INTERACTIVE | MN_CHECK_REMOTE);	/* remote mail */
}

void
mn_ui_preferences_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  mn_preferences_display();
}

void
mn_ui_about_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  static const char *authors[] = { "Jean-Yves Lefort <jylefort@brutele.be>", NULL };
  GdkPixbuf *logo;
  GdkPixbuf *icon;
  static GtkWidget *about = NULL;

  if (about)
    {
      gtk_window_present(GTK_WINDOW(about));
      return;
    }

  logo = mn_pixbuf_new("logo.png");
  about = gnome_about_new(_("Mail Notification"),
			  VERSION,
			  _("Copyright (c) 2003, 2004 Jean-Yves Lefort"),
			  _("A Mail Notification for the Panel Notification Area"),
			  authors,
			  NULL,
			  NULL,
			  logo);
  g_object_unref(logo);

  icon = mn_pixbuf_new("about-icon.png");
  gtk_window_set_icon(GTK_WINDOW(about), icon);
  g_object_unref(icon);
			  
  g_object_add_weak_pointer(G_OBJECT(about), (gpointer *) &about);
  gtk_widget_show(GTK_WIDGET(about));
}

void
mn_ui_quit_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  gtk_main_quit();
}
