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

#ifndef _JB_MAIN_H
#define _JB_MAIN_H

#include "jb-feature.h"

extern char *jb_topsrcdir;

void jb_main (int argc,
	      char **argv,
	      const char *package,
	      const char *version,
	      const char *human_package,
	      const JBFeature **features,
	      int num_features);

#define JB_MAIN(package, version, human_package)	\
  int main (int argc, char **argv)		 	\
  {						 	\
    jb_main(argc,					\
	    argv,					\
	    package,					\
	    version,					\
	    human_package,				\
	    jb_features,				\
	    G_N_ELEMENTS(jb_features));			\
    return 0;						\
  }

void jb_set_prefix_from_program (const char *name);

#endif /* _JB_MAIN_H */
