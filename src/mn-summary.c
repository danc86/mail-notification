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

#include <gtk/gtk.h>
#include "mn-util.h"
#include "mn-message-box.h"

void
mn_summary_update (GtkVBox *vbox, GSList *messages, gboolean selectable)
{
  GtkSizeGroup *size_group;
  GSList *l;

  g_return_if_fail(GTK_IS_VBOX(vbox));

  gtk_container_foreach(GTK_CONTAINER(vbox), (GtkCallback) gtk_widget_destroy, NULL);

  size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

  MN_LIST_FOREACH(l, messages)
    {
      MNMessage *message = l->data;
      GtkWidget *message_box;

      message_box = mn_message_box_new(size_group, message, selectable);

      gtk_box_pack_start(GTK_BOX(vbox), message_box, FALSE, FALSE, 0);
      gtk_widget_show(message_box);

      if (l->next)
	{			/* spacing between messages */
	  GtkWidget *label;

	  label = gtk_label_new(NULL);
	  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
	  gtk_widget_show(label);
	}
    }

  g_object_unref(size_group);
}
