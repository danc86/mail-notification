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

#ifndef _JB_FEATURE_H
#define _JB_FEATURE_H

#include <glib.h>

/*
 * A JBFeature is a modular chunk that must be brought in by the
 * package if it uses a certain JB functionality. For instance it must
 * reference jb_gconf_feature if it uses jb_group_add_gconf_schemas().
 */

typedef struct
{
  void (*init)		(void);
  void (*configure)	(void);
} JBFeature;

/* ordered from least to most specific (roughly) */
extern JBFeature jb_pkg_config_feature;
extern JBFeature jb_gettext_feature;
extern JBFeature jb_intltool_feature;
extern JBFeature jb_gconf_feature;
extern JBFeature jb_gnome_help_feature;
extern JBFeature jb_gob2_feature;

extern gboolean jb_intltool_use_xml;

void jb_feature_set_list (const JBFeature **features, int num_features);
gboolean jb_feature_is_enabled (JBFeature *feature);

void jb_feature_init (void);
void jb_feature_configure (void);

#endif /* _JB_FEATURE_H */
