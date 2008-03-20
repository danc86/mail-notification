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

#include <string.h>
#include "jb-tests.h"
#include "jb-variable.h"
#include "jb-compile-options.h"

/* based on a similar test in glibc */
void
jb_string_arch_unaligned_check (void)
{
  const char *cpu;

  jb_check_host_system();

  cpu = jb_variable_get_string("host-cpu");

  if (! strcmp(cpu, "i386")
      || ! strcmp(cpu, "i486")
      || ! strcmp(cpu, "i586")
      || ! strcmp(cpu, "i686")
      || ! strcmp(cpu, "i786")
      || ! strcmp(cpu, "x86_64")
      || ! strcmp(cpu, "s390")
      || ! strcmp(cpu, "s390x"))
    jb_compile_options_add_cppflags(jb_compile_options, "-DSTRING_ARCH_UNALIGNED");
}
