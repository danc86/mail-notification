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
#include "mn-preferences.h"
#include "mn-dialog.h"
#include "mn-util.h"
#include "mn-conf.h"
#include "mn-mailboxes.h"
#include "mn-stock.h"
#include "mn-unsupported-mailbox.h"

/*** variables ***************************************************************/

static MNMailIcon *mail_icon;
static GtkWidget *update_item;

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
  GtkWidget *menu;

  mail_icon = MN_MAIL_ICON(mn_mail_icon_new());

  mn_create_interface("menu",
		      "menu", &menu,
		      "update", &update_item,
		      NULL);
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
  if (eel_gconf_get_boolean(MN_CONF_COMMANDS_CLICKED_ENABLED))
    {
      char *command;

      command = eel_gconf_get_string(MN_CONF_COMMANDS_CLICKED_COMMAND);
      if (command)
	{
	  GError *err = NULL;
	  
	  if (! g_spawn_command_line_async(command, &err))
	    {
	      mn_error_dialog(NULL,
			      _("A command error has occurred."),
			      _("Unable to execute clicked command: %s."),
			      err->message);
	      g_error_free(err);
	    }

	  g_free(command);
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
mn_ui_update_sensitivity (void)
{
  gboolean has_manual = FALSE;
  GSList *l;

  MN_LIST_FOREACH(l, mn_mailboxes_get())
    {
      MNMailbox *mailbox = l->data;

      if (MN_MAILBOX_GET_CLASS(mailbox)->check && ! mn_mailbox_get_automatic(mailbox))
	{
	  has_manual = TRUE;
	  break;
	}
    }

  gtk_widget_set_sensitive(update_item, has_manual);
}

void
mn_ui_update_icon (void)
{
  GSList *mailboxes;
  const char *stock_id;
  GString *tooltip;

  mailboxes = mn_mailboxes_get();
  if (mailboxes)
    {
      GSList *l;
      int n_new = 0;
      int n_error = 0;
      int n_unsupported = 0;
      GString *new_string;
      GString *error_string;
      GString *unsupported_string;

      new_string = g_string_new(NULL);
      error_string = g_string_new(NULL);
      unsupported_string = g_string_new(NULL);

      MN_LIST_FOREACH(l, mn_mailboxes_get())
        {
	  MNMailbox *mailbox = l->data;
	  const char *name;
	  const char *error;

	  name = mn_mailbox_get_name(mailbox);
	  error = mn_mailbox_get_error(mailbox);

	  if (mn_mailbox_get_has_new(mailbox))
	    {
	      n_new++;
	      if (*new_string->str)
		g_string_append_c(new_string, '\n');
	      g_string_append_printf(new_string, "    %s", name);
	    }
	  
	  if (error)
	    {
	      n_error++;
	      if (*error_string->str)
		g_string_append_c(error_string, '\n');
	      g_string_append_printf(error_string, "    %s (%s)", name, error);
	    }
	  
	  if (MN_IS_UNSUPPORTED_MAILBOX(mailbox))
	    {
	      n_unsupported++;
	      if (*unsupported_string->str)
		g_string_append_c(unsupported_string, '\n');
	      g_string_append_printf(unsupported_string, "    %s (%s)", name, mn_unsupported_mailbox_get_reason(MN_UNSUPPORTED_MAILBOX(mailbox)));
	    }
	}

      if (n_new > 0)
	{
	  stock_id = n_error > 0 ? MN_STOCK_MAIL_ERROR : MN_STOCK_MAIL;
	  g_string_prepend(new_string, ngettext("The following mailbox has new mail:\n",
						"The following mailboxes have new mail:\n",
						n_new));
	}
      else
	{
	  stock_id = n_error > 0 ? MN_STOCK_NO_MAIL_ERROR : MN_STOCK_NO_MAIL;
	  g_string_prepend(new_string, _("You have no new mail."));
	}
      
      if (n_error > 0)
	g_string_prepend(error_string, ngettext("The following mailbox reported an error:\n",
						"The following mailboxes reported an error:\n",
						n_error));
      
      if (n_unsupported > 0)
	g_string_prepend(unsupported_string, ngettext("The following mailbox is unsupported:\n",
						      "The following mailboxes are unsupported:\n",
						      n_unsupported));

      tooltip = g_string_new(new_string->str);
      if (n_error > 0)
	g_string_append_printf(tooltip, "\n\n%s", error_string->str);
      if (n_unsupported > 0)
	g_string_append_printf(tooltip, "\n\n%s", unsupported_string->str);

      g_string_free(new_string, TRUE);
      g_string_free(error_string, TRUE);
      g_string_free(unsupported_string, TRUE);
    }
  else
    {
      stock_id = MN_STOCK_NO_MAIL;
      tooltip = g_string_new(_("No mailboxes are being monitored."));
    }
  
  mn_mail_icon_set_from_stock(mail_icon, stock_id);
  mn_mail_icon_set_tooltip(mail_icon, tooltip->str);

  g_string_free(tooltip, TRUE);
}

/* libglade callbacks */

void
mn_ui_update_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  mn_mailboxes_check();
}

void
mn_ui_preferences_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  mn_preferences_display();
}

void
mn_ui_help_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  mn_display_help(NULL);
}

void
mn_ui_about_activate_h (GtkMenuItem *menuitem, gpointer user_data)
{
  static const char *authors[] = { "Jean-Yves Lefort <jylefort@brutele.be>", NULL };
  static const char *documenters[] = { "Jean-Yves Lefort <jylefort@brutele.be>", NULL };
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
			  "Copyright \302\251 2003, 2004 Jean-Yves Lefort",
			  _("A Mail Notification Icon"),
			  authors,
			  documenters,
			  _("Jean-Yves Lefort <jylefort@brutele.be>"),
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
