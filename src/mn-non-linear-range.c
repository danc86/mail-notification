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

#include <math.h>
#include "mn-non-linear-range.h"
#include "mn-util.h"

typedef struct
{
  const MNNonLinearRangeBlock	*blocks;
  int				num_blocks;
} RangeInfo;

static GQuark info_quark = 0;

static void
global_init (void)
{
  if (! info_quark)
    info_quark = g_quark_from_static_string("mn-non-linear-range-info");
}

static RangeInfo *
get_info (GtkRange *range)
{
  RangeInfo *info;

  g_return_val_if_fail(GTK_IS_RANGE(range), NULL);
  g_return_val_if_fail(info_quark != 0, NULL);

  info = g_object_get_qdata(G_OBJECT(range), info_quark);
  g_return_val_if_fail(info != NULL, NULL);

  return info;
}

static int
get_block_len (const MNNonLinearRangeBlock *block)
{
  g_return_val_if_fail(block != NULL, -1);

  return ((block->max - block->min) / block->step) + 1;
}

void
mn_non_linear_range_setup_static (GtkRange *range,
				  const MNNonLinearRangeBlock *blocks,
				  int num_blocks)
{
  RangeInfo *info;
  int i;
  int num_values = 0;

  g_return_if_fail(GTK_IS_RANGE(range));
  g_return_if_fail(blocks != NULL);
  g_return_if_fail(num_blocks > 0);

  global_init();

  info = g_new0(RangeInfo, 1);
  info->blocks = blocks;
  info->num_blocks = num_blocks;

  g_object_set_qdata_full(G_OBJECT(range), info_quark, info, g_free);

  for (i = 0; i < num_blocks; i++)
    num_values += get_block_len(&blocks[i]);

  gtk_range_set_range(range, 0, num_values - 1);
}

gboolean
mn_is_non_linear_range (gpointer object)
{
  return object != NULL && info_quark != 0 && g_object_get_qdata(object, info_quark) != NULL;
}

int
mn_non_linear_range_get_value (GtkRange *range)
{
  RangeInfo *info;
  int raw;
  int offset = 0;
  int i;

  g_return_val_if_fail(GTK_IS_RANGE(range), -1);

  info = get_info(range);

  raw = (int) gtk_range_get_value(range);

  for (i = 0; i < info->num_blocks; i++)
    {
      const MNNonLinearRangeBlock *block = &info->blocks[i];
      int next_offset;

      next_offset = offset + get_block_len(block);

      if (raw >= offset && raw < next_offset)
	return block->min + (raw - offset) * block->step;

      offset = next_offset;
    }

  g_assert_not_reached();
  return -1;
}

static int
value_to_index (RangeInfo *info, int value)
{
  int offset = 0;
  int i;

  /* if smaller than the first value, use the first value */
  if (value < info->blocks[0].min)
    return 0;

  for (i = 0; i < info->num_blocks; i++)
    {
      const MNNonLinearRangeBlock *block = &info->blocks[i];

      if (value >= block->min && value <= block->max)
	{
	  int rounded;
	  int index;
	  int j;

	  /* round the value to the nearest step */
	  rounded = lround((double) value / block->step) * block->step;

	  for (j = block->min, index = 0; j <= block->max; j += block->step, index++)
	    if (j == rounded)
	      return offset + index;

	  g_assert_not_reached();
	}

      offset += get_block_len(block);
    }

  /* block not found: fallback to the maximum value */
  return offset - 1;
}

/*
 * If the value is not found in the blocks, the nearest existing value
 * will be used.
 */
void
mn_non_linear_range_set_value (GtkRange *range, int value)
{
  g_return_if_fail(GTK_IS_RANGE(range));

  gtk_range_set_value(range, value_to_index(get_info(range), value));
}
