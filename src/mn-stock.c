/*
 * Mail Notification
 * Copyright (C) 2003-2007 Jean-Yves Lefort <jylefort@brutele.be>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include "mn-stock.h"
#include "mn-util.h"

/*** implementation **********************************************************/

void
mn_stock_init (void)
{
  static const struct
  {
    const char	*stock_id;
    const char	*filename;
    const char	*icon_name;
    const char	*source_stock_id;
  } icons[] = {
    { MN_STOCK_MAIL,			NULL, "stock_mail" },
    { MN_STOCK_NO_MAIL,			NULL, "stock_inbox" },
    { MN_STOCK_LOCAL,			NULL, "stock_folder" },
    { MN_STOCK_REMOTE,			NULL, "stock_internet" },
    { MN_STOCK_UNKNOWN,			NULL, "stock_unknown" },
    { MN_STOCK_ERROR,			NULL, NULL, GTK_STOCK_DIALOG_ERROR },
#if WITH_GMAIL
    { MN_STOCK_GMAIL,			PKGDATADIR G_DIR_SEPARATOR_S "gmail.png" },
#endif
#if WITH_YAHOO
    { MN_STOCK_YAHOO,			PKGDATADIR G_DIR_SEPARATOR_S "yahoo.png" },
#endif
#if WITH_HOTMAIL
    { MN_STOCK_HOTMAIL,			PKGDATADIR G_DIR_SEPARATOR_S "hotmail.png" },
#endif
#if WITH_MBOX || WITH_MOZILLA || WITH_MH || WITH_MAILDIR || WITH_SYLPHEED
    { MN_STOCK_SYSTEM_MAILBOX,		NULL, "system" },
#endif
#if WITH_EVOLUTION
    { MN_STOCK_EVOLUTION_MAILBOX,	NULL, "evolution" },
#endif
    { MN_STOCK_MAIL_READER,		NULL, "stock_mail-handling" },
    { MN_STOCK_OPEN_MESSAGE,		NULL, "stock_mail-open" },
    { MN_STOCK_CONSIDER_NEW_MAIL_AS_READ, NULL, "stock_mark" }
  };
  GtkIconFactory *factory;
  GtkIconTheme *icon_theme;
  int i;

  factory = gtk_icon_factory_new();
  gtk_icon_factory_add_default(factory);
  icon_theme = gtk_icon_theme_get_default();

  for (i = 0; i < G_N_ELEMENTS(icons); i++)
    {
      GtkIconSet *icon_set;

      if (icons[i].filename)
	{
	  GdkPixbuf *pixbuf;

	  pixbuf = mn_pixbuf_new(icons[i].filename);
	  icon_set = gtk_icon_set_new_from_pixbuf(pixbuf);
	  g_object_unref(pixbuf);
	}
      else if (icons[i].icon_name)
	{
	  GtkIconSource *icon_source;

	  icon_set = gtk_icon_set_new();
	  icon_source = gtk_icon_source_new();
	  gtk_icon_source_set_icon_name(icon_source, icons[i].icon_name);
	  gtk_icon_set_add_source(icon_set, icon_source);
	  gtk_icon_source_free(icon_source);
	}
      else if (icons[i].source_stock_id)
	{
	  icon_set = gtk_icon_factory_lookup_default(icons[i].source_stock_id);
	  gtk_icon_set_ref(icon_set);
	}
      else
	g_assert_not_reached();

      gtk_icon_factory_add(factory, icons[i].stock_id, icon_set);
      gtk_icon_set_unref(icon_set);
    }

  g_object_unref(factory);
}
