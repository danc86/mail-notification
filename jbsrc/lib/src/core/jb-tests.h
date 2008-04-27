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

#ifndef _JB_TESTS_H
#define _JB_TESTS_H

#include <stdarg.h>
#include <glib.h>
#include "jb-variable.h"

void jb_check_host_system (void);

void jb_register_program (const char *name, JBVariableFlags flags);
gboolean jb_check_program (const char *name);
void jb_require_program (const char *name);

gboolean jb_test_compile (const char *filename,
			  const char *cflags,
			  const char *cppflags);
gboolean jb_test_compile_string (const char *str,
				 const char *cflags,
				 const char *cppflags);

gboolean jb_test_link (const char *filename,
		       const char *cflags,
		       const char *cppflags,
		       const char *ldflags,
		       const char *libs);
gboolean jb_test_link_string (const char *str,
			      const char *cflags,
			      const char *cppflags,
			      const char *ldflags,
			      const char *libs);

gboolean jb_test_run (const char *filename,
		      const char *cflags,
		      const char *cppflags,
		      const char *ldflags,
		      const char *libs);
gboolean jb_test_run_string (const char *str,
			     const char *cflags,
			     const char *cppflags,
			     const char *ldflags,
			     const char *libs);

gboolean jb_check_functions (const char *functions, const char *libname);

gboolean jb_check_packages (const char *group_name,
			    const char *varprefix,
			    const char *packages);

void jb_check_packages_for_options (const char *group_name,
				    const char *varprefix,
				    const char *packages,
				    ...) G_GNUC_NULL_TERMINATED;

void jb_require_packages (const char *group_name,
			  const char *varprefix,
			  const char *packages);

char *jb_get_package_variable (const char *package, const char *variable);

void jb_check_cc_dependency_style (void);

void jb_check_glibc (void);

void jb_check_reentrant_dns_resolver (void);

#endif /* _JB_TESTS_H */
