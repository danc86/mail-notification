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
#include <gtk/gtk.h>
#include "mn-stock.h"
#include "mn-util.h"

/*** implementation **********************************************************/

void
mn_stock_init (void)
{
  const struct
  {
    const char	*stock_id;
    const char	*filename;
    const char	*icon_name;
  } icons[] = {
    { MN_STOCK_MAIL,		"mail.png", NULL		},
    { MN_STOCK_MAIL_ERROR,	"mail-error.png", NULL		},
    { MN_STOCK_NO_MAIL,		"no-mail.png", NULL		},
    { MN_STOCK_NO_MAIL_ERROR,	"no-mail-error.png", NULL	},
    { MN_STOCK_LOCAL,		NULL, "stock_folder"		},
    { MN_STOCK_REMOTE,		NULL, "stock_internet"		},
    { MN_STOCK_UNSUPPORTED,	"unsupported.png", NULL		}
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
      else
	g_return_if_reached();

      gtk_icon_factory_add(factory, icons[i].stock_id, icon_set);
      gtk_icon_set_unref(icon_set);
    }
  
  g_object_unref(factory);
}
