/*
 * Mail Notification
 * Copyright (C) 2003-2008 Jean-Yves Lefort <jylefort@brutele.be>
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

#ifndef _MN_NON_LINEAR_RANGE_H
#define _MN_NON_LINEAR_RANGE_H

#include <gtk/gtk.h>

typedef struct
{
  int min;			/* inclusive */
  int max;			/* inclusive */
  int step;
} MNNonLinearRangeBlock;

void mn_non_linear_range_setup_static (GtkRange *range,
				       const MNNonLinearRangeBlock *blocks,
				       int num_blocks);

gboolean mn_is_non_linear_range (gpointer object);

int mn_non_linear_range_get_value (GtkRange *range);
void mn_non_linear_range_set_value (GtkRange *range, int value);

#endif /* _MN_NON_LINEAR_RANGE_H */
