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
#include <gnome.h>
#include "mn-mail-icon.h"
#include "mn-check.h"
#include "mn-preferences.h"
#include "mn-dialog.h"
#include "mn-util.h"

/*** variables ***************************************************************/

static MNMailIcon *mail_icon;

/*** functions ***************************************************************/

static void mn_ui_icon_init		(void);
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
  mail_icon = MN_MAIL_ICON(mn_mail_icon_new());
  g_signal_connect(G_OBJECT(mail_icon), "destroy",
		   G_CALLBACK(mn_ui_icon_destroy_h), NULL);

  gtk_widget_show(GTK_WIDGET(mail_icon));
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
  mn_mail_icon_set_can_check(mail_icon, can_check);
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
			  _("Copyright (c) 2003 Jean-Yves Lefort"),
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
