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
#include <string.h>
#include <gtk/gtk.h>
#include <libgnome/gnome-i18n.h>
#include <glade/glade.h>
#ifdef HAVE_GNET
#include <gnet.h>
#endif
#include <stdarg.h>
#include "mn-util.h"

/*** implementation **********************************************************/

/*
 * Free a singly linked list of heap pointers.
 */
void
mn_slist_free (GSList *list)
{
  GSList *l;

  MN_LIST_FOREACH(l, list)
    g_free(l->data);

  g_slist_free(list);
}

/*
 * Free a singly linked list of objects.
 */
void
mn_objects_free (GSList *list)
{
  GSList *l;

  MN_LIST_FOREACH(l, list)
    g_object_unref(l->data);

  g_slist_free(list);
}

/*
 * Copy a singly linked list of objects.
 */
GSList *
mn_objects_copy (GSList *list)
{
  GSList *l;
  GSList *copy;

  copy = g_slist_copy(list);
  MN_LIST_FOREACH(l, copy)
    g_object_ref(l->data);

  return copy;
}

#ifdef HAVE_GNET
GIOError
mn_gnet_io_channel_printf (GIOChannel *channel, const char *format, ...)
{
  va_list args;
  char *message;
  GIOError status;
  int len;
  gsize count;

  g_return_val_if_fail(channel != NULL, G_IO_ERROR_UNKNOWN);
  
  va_start(args, format);
  message = g_strdup_vprintf(format, args);
  va_end(args);

  len = strlen(message);
  status = gnet_io_channel_writen(channel, message, len, &count);
  g_free(message);

  if (status == G_IO_ERROR_NONE && count != len)
    status = G_IO_ERROR_UNKNOWN;

  return status;
}
#endif /* HAVE_GNET */

gboolean
mn_str_isnumeric (const char *str)
{
  int i;

  g_return_val_if_fail(str != NULL, FALSE);

  for (i = 0; str[i]; i++)
    if (! g_ascii_isdigit(str[i]))
      return FALSE;

  return TRUE;
}

gboolean
mn_utf8_strcasecontains (const char *big, const char *little)
{
  gboolean contains;
  char *normalized_big;
  char *normalized_little;
  char *case_normalized_big;
  char *case_normalized_little;

  g_return_val_if_fail(big != NULL, NULL);
  g_return_val_if_fail(little != NULL, NULL);

  normalized_big = g_utf8_normalize(big, -1, G_NORMALIZE_ALL);
  normalized_little = g_utf8_normalize(little, -1, G_NORMALIZE_ALL);
  case_normalized_big = g_utf8_casefold(normalized_big, -1);
  case_normalized_little = g_utf8_casefold(normalized_little, -1);

  contains = strstr(case_normalized_big, case_normalized_little) != NULL;

  g_free(normalized_big);
  g_free(normalized_little);
  g_free(case_normalized_big);
  g_free(case_normalized_little);

  return contains;
}

GdkPixbuf *mn_pixbuf_new (const char *filename)
{
  char *pathname;
  GdkPixbuf *pixbuf;
  GError *err = NULL;

  g_return_val_if_fail(filename != NULL, NULL);

  pathname = g_build_filename(UIDIR, filename, NULL);
  pixbuf = gdk_pixbuf_new_from_file(pathname, &err);
  g_free(pathname);

  if (! pixbuf)
    {
      g_warning(_("error loading image: %s"), err->message);
      g_error_free(err);
    }

  return pixbuf;
}

GladeXML *mn_glade_xml_new (const char *filename)
{
  char *full_filename;
  char *pathname;
  GladeXML *xml;

  full_filename = g_strconcat(filename, ".glade", NULL);
  pathname = g_build_filename(UIDIR, full_filename, NULL);
  g_free(full_filename);

  xml = glade_xml_new(pathname, NULL, NULL);
  g_free(pathname);

  glade_xml_signal_autoconnect(xml);

  return xml;
}
