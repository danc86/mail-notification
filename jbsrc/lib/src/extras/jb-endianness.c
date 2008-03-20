/*
 * JB, the Jean-Yves Lefort's Build System
 * Copyright (C) 2008 Jean-Yves Lefort <jylefort@brutele.be>
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

#include "jb-util.h"
#include "jb-compile-options.h"

static gboolean
is_bigendian (void)
{
  int i = 1;

  return ((char *) &i)[sizeof(int) - 1] == 1;
}

void
jb_endianness_check (void)
{
  jb_message_checking("the host CPU endianness");
  if (is_bigendian())
    {
      jb_message_result_string("big-endian");
      jb_compile_options_add_cppflags(jb_compile_options, "-DWORDS_BIGENDIAN");
    }
  else
    jb_message_result_string("little-endian");
}
