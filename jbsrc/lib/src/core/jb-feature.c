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

#include <stdio.h>
#include <string.h>
#include "jb-util.h"
#include "jb-variable.h"
#include "jb-tests.h"
#include "jb-feature.h"

typedef struct
{
  int major;
  int minor;
  int micro;
} Version;

static const JBFeature **_features = NULL;
static int _num_features = 0;

gboolean jb_intltool_use_xml = TRUE;

void
jb_feature_set_list (const JBFeature **features, int num_features)
{
  g_return_if_fail(features != NULL);

  _features = features;
  _num_features = num_features;
}

gboolean
jb_feature_is_enabled (JBFeature *feature)
{
  int i;

  g_return_val_if_fail(feature != NULL, FALSE);

  for (i = 0; i < _num_features; i++)
    if (_features[i] == feature)
      return TRUE;

  return FALSE;
}

void
jb_feature_init (void)
{
  int i;

  for (i = 0; i < _num_features; i++)
    {
      const JBFeature *feature = _features[i];

      if (feature->init)
	feature->init();
    }
}

void
jb_feature_configure (void)
{
  int i;

  for (i = 0; i < _num_features; i++)
    {
      const JBFeature *feature = _features[i];

      if (feature->configure)
	feature->configure();
    }
}

static void
pkg_config_init (void)
{
  /* NO_REPORT since it is only used in configure */
  jb_register_program("pkg-config", JB_VARIABLE_NO_REPORT);
}

static void
gettext_init (void)
{
  jb_register_program("msgfmt", 0);
}

static void
gettext_configure (void)
{
  static const char *functions = "ngettext dgettext bind_textdomain_codeset";

  if (jb_check_functions(functions, NULL))
    jb_variable_set_package_flags("gettext", NULL, "-DENABLE_NLS", NULL, NULL);
  else if (jb_check_functions(functions, "intl"))
    jb_variable_set_package_flags("gettext", NULL, "-DENABLE_NLS", NULL, "-lintl");
  else
    jb_error("gettext found neither in libc nor in libintl");

  jb_require_program("msgfmt");
}

static void
intltool_init (void)
{
  jb_register_program("perl", 0);
}

static void
intltool_configure (void)
{
  jb_require_program("perl");

  if (jb_intltool_use_xml)
    {
      gboolean result;

      jb_message_checking("for XML::Parser");
      result = jb_exec_expand(NULL, NULL, "$perl -e 'require XML::Parser'", NULL);
      jb_message_result_bool(result);

      if (! result)
	jb_error("intltool requires the XML::Parser Perl module");
    }
}

static void
gconf_init (void)
{
  jb_register_program("gconftool-2", 0);

  jb_variable_add_string("gconf-config-source",
			 "GConf configuration source address",
			 jb_variable_group_installation_options,
			 0,
			 "autodetect");
  jb_variable_add_string("gconf-schemas-dir",
			 "GConf schemas installation directory",
			 jb_variable_group_installation_options,
			 0,
			 "$sysconfdir/gconf/schemas");
  jb_variable_add_bool("install-gconf-schemas",
		       "install GConf schemas",
		       jb_variable_group_installation_options,
		       0,
		       TRUE);
}

static void
gconf_configure (void)
{
  JBVariable *variable;

  jb_require_program("gconftool-2");

  if (! strcmp(jb_variable_get_string("gconf-config-source"), "autodetect"))
    {
      char *config_source;

      if (! jb_exec_expand(&config_source, NULL, "$gconftool-2 --get-default-source", NULL))
	jb_error("unable to detect the GConf configuration source address");

      jb_variable_set_string("gconf-config-source", config_source);
      g_free(config_source);
    }

  /* fix the default schemas dir on Ubuntu */
  variable = jb_variable_get_variable_or_error("gconf-schemas-dir");
  if (! variable->user_set)
    {
      static const char *ubuntu_dir = "$datadir/gconf/schemas";
      char *expanded;

      expanded = jb_variable_expand(ubuntu_dir, NULL);

      if (g_file_test(expanded, G_FILE_TEST_IS_DIR))
	jb_variable_set_string("gconf-schemas-dir", ubuntu_dir);

      g_free(expanded);
    }
}

static void
gnome_help_init (void)
{
  /* NO_REPORT since it is only used in configure */
  jb_register_program("scrollkeeper-config", JB_VARIABLE_NO_REPORT);
  jb_register_program("scrollkeeper-preinstall", 0);
  jb_register_program("scrollkeeper-update", 0);

  jb_variable_add_string("help-dir",
			 "GNOME help directory",
			 jb_variable_group_installation_options,
			 0,
			 "$datadir/gnome/help");
  jb_variable_add_string("omf-dir",
			 "OMF directory",
			 jb_variable_group_installation_options,
			 0,
			 "autodetect");
  jb_variable_add_string("scrollkeeper-dir",
			 "ScrollKeeper database directory",
			 jb_variable_group_installation_options,
			 0,
			 "autodetect");
}

static void
gnome_help_check_dir (const char *name,
		      const char *description,
		      const char *scrollkeeper_config_arg)
{
  const char *dir;
  char *result;

  dir = jb_variable_get_string(name);
  if (strcmp(dir, "autodetect"))
    return;

  jb_require_program("scrollkeeper-config");

  jb_message_checking("for the %s", description);

  if (jb_exec_expand(&result, NULL, "$scrollkeeper-config $arg",
		     "arg", scrollkeeper_config_arg,
		     NULL))
    {
      jb_message_result_string(result);
      jb_variable_set_string(name, result);
    }
  else
    {
      jb_message_result_string("not found");
      jb_error("unable to autodetect the %s", description);
    }

  g_free(result);
}

static void
gnome_help_configure (void)
{
  jb_require_program("scrollkeeper-preinstall");
  jb_require_program("scrollkeeper-update");

  gnome_help_check_dir("omf-dir",
		       "OMF directory",
		       "--omfdir");
  gnome_help_check_dir("scrollkeeper-dir",
		       "ScrollKeeper database directory",
		       "--pkglocalstatedir");
}

static gboolean
parse_version (const char *version_string, Version *version)
{
  int _major;
  int _minor;
  int _micro;

  switch (sscanf(version_string, "%d.%d.%d", &_major, &_minor, &_micro))
    {
    case 1:
      version->major = _major;
      version->minor = 0;
      version->micro = 0;
      return TRUE;

    case 2:
      version->major = _major;
      version->minor = _minor;
      version->micro = 0;
      return TRUE;

    case 3:
      version->major = _major;
      version->minor = _minor;
      version->micro = _micro;
      return TRUE;

    default:
      return FALSE;
    }
}

static int
version_to_int (Version *version)
{
  return (version->major << 16) + (version->minor << 8) + version->micro;
}

static void
gob2_init (void)
{
  jb_register_program("gob2", 0);
  jb_variable_set_string("gob2-minversion", "2.0");
}

static gboolean
parse_gob2_version_output (char *str,
			   char **version_string,
			   Version *version)
{
  char *space;
  char *_version_string;

  space = strrchr(str, ' ');
  if (space == NULL)
    return FALSE;

  _version_string = space + 1;
  if (parse_version(_version_string, version))
    {
      *version_string = _version_string;
      return TRUE;
    }

  return FALSE;
}

static char *
get_gob2_not_found_error (const char *minversion)
{
  return g_strdup_printf("One or more .gob source files were modified but gob2 was not found. Please install gob2 >= %s and run configure again.",
			 minversion);
}

static void
gob2_configure (void)
{
  const char *minversion_string;
  Version minversion;
  char *error = NULL;

  minversion_string = jb_variable_get_string("gob2-minversion");
  if (! parse_version(minversion_string, &minversion))
    g_error("invalid gob2-minversion \"%s\"", minversion_string);

  if (jb_check_program("gob2"))
    {
      char *output;
      gboolean result = FALSE;

      jb_message_checking("for gob2 >= %s", minversion_string);

      if (jb_exec_expand(NULL, &output, "$gob2 --version", NULL))
	{
	  char *version_string;
	  Version version;

	  if (parse_gob2_version_output(output, &version_string, &version))
	    {
	      if (version_to_int(&version) >= version_to_int(&minversion))
		result = TRUE;
	      else
		error = g_strdup_printf("One or more .gob source files were modified but the version of gob2 (%s) is too old. Please install gob2 >= %s and run configure again.",
					version_string,
					minversion_string);
	    }
	  else
	    error = get_gob2_not_found_error(minversion_string);
	}
      else
	error = get_gob2_not_found_error(minversion_string);

      g_free(output);

      jb_message_result_bool(result);
    }
  else
    error = get_gob2_not_found_error(minversion_string);

  jb_variable_set_string("gob2-error", error);
  g_free(error);
}

JBFeature jb_pkg_config_feature = {
  pkg_config_init,
  NULL
};

JBFeature jb_gettext_feature = {
  gettext_init,
  gettext_configure
};

JBFeature jb_intltool_feature = {
  intltool_init,
  intltool_configure
};

JBFeature jb_gconf_feature = {
  gconf_init,
  gconf_configure
};

JBFeature jb_gnome_help_feature = {
  gnome_help_init,
  gnome_help_configure
};

JBFeature jb_gob2_feature = {
  gob2_init,
  gob2_configure
};
