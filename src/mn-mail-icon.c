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
#include <libgnome/gnome-i18n.h>
#include <gtk/gtk.h>
#include "mn-mail-icon.h"
#include "mn-util.h"

/*** cpp *********************************************************************/

#define MAIL_NOTIFICATION_STOCK_MAIL		"mail-notification-mail"
#define MAIL_NOTIFICATION_STOCK_NO_MAIL		"mail-notification-no-mail"

/*** types *******************************************************************/

enum {
  ACTIVATE,
  LAST_SIGNAL
};

struct _MNMailIconPrivate
{
  GtkTooltips	*tooltips;
  GtkWidget	*image;
  GtkMenu	*menu;
};
  
/*** variables ***************************************************************/

static GObjectClass *parent_class = NULL;
static unsigned int mail_icon_signals[LAST_SIGNAL] = { 0 };

/*** functions ***************************************************************/

static void	mn_mail_icon_class_init	(MNMailIconClass	*class);
static void	mn_mail_icon_init	(MNMailIcon		*icon);
static void	mn_mail_icon_finalize	(GObject		*object);

static void	mn_mail_icon_stock_init (void);

static gboolean	mn_mail_icon_press	(GtkWidget		*widget,
					 GdkEventButton		*event,
					 gpointer		data);
static gboolean	mn_mail_icon_release	(GtkWidget		*widget,
					 GdkEventButton		*event,
					 gpointer		data);

/*** implementation **********************************************************/

GType
mn_mail_icon_get_type (void)
{
  static GType mail_icon_type = 0;
  
  if (! mail_icon_type)
    {
      static const GTypeInfo mail_icon_info = {
	sizeof(MNMailIconClass),
	NULL,
	NULL,
	(GClassInitFunc) mn_mail_icon_class_init,
	NULL,
	NULL,
	sizeof(MNMailIcon),
	0,
	(GInstanceInitFunc) mn_mail_icon_init
      };
      
      mail_icon_type = g_type_register_static(EGG_TYPE_TRAY_ICON,
					      "MNMailIcon",
					      &mail_icon_info,
					      0);
    }
  
  return mail_icon_type;
}

static void
mn_mail_icon_class_init (MNMailIconClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS(class);

  parent_class = g_type_class_peek_parent(class);

  object_class->finalize = mn_mail_icon_finalize;

  mn_mail_icon_stock_init();

  mail_icon_signals[ACTIVATE] = g_signal_new("activate",
					     MN_TYPE_MAIL_ICON,
					     G_SIGNAL_RUN_LAST,
					     G_STRUCT_OFFSET(MNMailIconClass, activate),
					     NULL,
					     NULL,
					     g_cclosure_marshal_VOID__VOID,
					     G_TYPE_NONE,
					     0);
}

static void
mn_mail_icon_init (MNMailIcon *icon)
{
  GtkWidget *event_box;
  
  icon->priv = g_new0(MNMailIconPrivate, 1);

  /* create widgets */

  event_box = gtk_event_box_new();

  icon->priv->tooltips = gtk_tooltips_new();
  g_object_ref(icon->priv->tooltips);
  gtk_object_sink(GTK_OBJECT(icon->priv->tooltips));

  icon->priv->image = gtk_image_new_from_stock(MAIL_NOTIFICATION_STOCK_NO_MAIL, GTK_ICON_SIZE_MENU);

  /* configure widgets */

  gtk_tooltips_set_tip(icon->priv->tooltips,
		       GTK_WIDGET(icon),
		       _("You have no new mail"),
		       NULL);

  /* pack widgets */

  gtk_container_add(GTK_CONTAINER(event_box), icon->priv->image);
  gtk_container_add(GTK_CONTAINER(icon), event_box);
  
  /* bind signals */

  g_signal_connect(G_OBJECT(event_box), "button-press-event",
		   G_CALLBACK(mn_mail_icon_press), icon);
  g_signal_connect(G_OBJECT(event_box), "button-release-event",
		   G_CALLBACK(mn_mail_icon_release), icon);

  /* show widgets */
  
  gtk_widget_show(icon->priv->image);
  gtk_widget_show(event_box);
}

static void
mn_mail_icon_finalize (GObject *object)
{
  MNMailIcon *icon = MN_MAIL_ICON(object);

  g_object_unref(icon->priv->tooltips);
  if (icon->priv->menu)
    gtk_widget_destroy(GTK_WIDGET(icon->priv->menu));
  g_free(icon->priv);

  G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void
mn_mail_icon_stock_init (void)
{
  const struct
  {
    const char	*stock_id;
    const char	*filename;
  } icons[] = {
    { MAIL_NOTIFICATION_STOCK_MAIL,		"mail.png"		},
    { MAIL_NOTIFICATION_STOCK_NO_MAIL,		"no-mail.png"		}
  };
  GtkIconFactory *factory;
  int i;

  factory = gtk_icon_factory_new();
  gtk_icon_factory_add_default(factory);

  for (i = 0; i < G_N_ELEMENTS(icons); i++)
    {
      GtkIconSet *icon_set;
      GdkPixbuf *pixbuf;
      
      pixbuf = mn_pixbuf_new(icons[i].filename);

      icon_set = gtk_icon_set_new_from_pixbuf(pixbuf);
      g_object_unref(pixbuf);

      gtk_icon_factory_add(factory, icons[i].stock_id, icon_set);
      gtk_icon_set_unref(icon_set);
    }
  
  g_object_unref(factory);
}

static gboolean
mn_mail_icon_press (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  MNMailIcon *icon = data;

  if (event->button == 1)
    g_signal_emit(icon, mail_icon_signals[ACTIVATE], 0);
  else if (event->button == 3 && icon->priv->menu)
    {
      gtk_menu_popup(icon->priv->menu, NULL, NULL, NULL, NULL, event->button, event->time);
      return TRUE;
    }

  return FALSE;
}

static gboolean
mn_mail_icon_release (GtkWidget *widget,
		      GdkEventButton *event,
		      gpointer data)
{
  MNMailIcon *icon = data;

  if (event->button == 3 && icon->priv->menu)
    {
      gtk_menu_popdown(icon->priv->menu);
      return TRUE;
    }
  
  return FALSE;
}

GtkWidget *
mn_mail_icon_new (void)
{
  return g_object_new(MN_TYPE_MAIL_ICON, "title", "Mail Notification", NULL);
}

/*
 * This owns MENU.
 */
void
mn_mail_icon_set_popup_menu (MNMailIcon *icon, GtkMenu *menu)
{
  g_return_if_fail(MN_IS_MAIL_ICON(icon));
  g_return_if_fail(GTK_IS_MENU(menu));
  g_return_if_fail(icon->priv->menu == NULL);
  
  icon->priv->menu = menu;
}

void
mn_mail_icon_set_has_new (MNMailIcon *icon, gboolean has_new)
{
  g_return_if_fail(MN_IS_MAIL_ICON(icon));

  gtk_image_set_from_stock(GTK_IMAGE(icon->priv->image),
			   has_new ? MAIL_NOTIFICATION_STOCK_MAIL : MAIL_NOTIFICATION_STOCK_NO_MAIL,
			   GTK_ICON_SIZE_MENU);
  gtk_tooltips_set_tip(icon->priv->tooltips,
		       GTK_WIDGET(icon),
		       has_new ? _("You have new mail") : _("You have no new mail"),
		       NULL);
}
