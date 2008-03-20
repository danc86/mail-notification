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

#ifndef _JB_COMPILE_OPTIONS_H
#define _JB_COMPILE_OPTIONS_H

#include <stdarg.h>
#include <glib.h>

typedef struct
{
  char	*name;
  char	*cppflags;		/* private, use accessor */
} JBCompileOptions;

extern JBCompileOptions *jb_compile_options;

JBCompileOptions *jb_compile_options_new (const char *name);

void jb_compile_options_add_cflags (JBCompileOptions *self, const char *cflags);
const char *jb_compile_options_get_cflags (JBCompileOptions *self);

void jb_compile_options_add_cppflags (JBCompileOptions *self, const char *cppflags);
const char *jb_compile_options_get_cppflags (JBCompileOptions *self);

void jb_compile_options_add_ldflags (JBCompileOptions *self, const char *ldflags);
const char *jb_compile_options_get_ldflags (JBCompileOptions *self);

void jb_compile_options_add_gob2flags (JBCompileOptions *self, const char *gob2flags);
const char *jb_compile_options_get_gob2flags (JBCompileOptions *self);

void jb_compile_options_add_package (JBCompileOptions *self, const char *name);

void jb_compile_options_add_string_defines (JBCompileOptions *self,
					    const char *name,
					    ...) G_GNUC_NULL_TERMINATED;

#endif /* _JB_COMPILE_OPTIONS_H */
