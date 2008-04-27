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

#include "jb.h"

#jb_include <jb-dbus>
#jb_include <jb-endianness>
#jb_include <jb-evolution-plugin>
#jb_include <jb-openssl>
#jb_include <jb-sasl2>
#jb_include <jb-string-arch-unaligned>

#define MN_WARNING_CFLAGS \
  "-Wall "				\
  "-Wformat-y2k "			\
  "-Wformat-security "			\
  "-Wno-unused-parameter "		\
  "-Wfloat-equal "			\
  "-Wdeclaration-after-statement "	\
  "-Wendif-labels "			\
  "-Wpointer-arith "			\
  "-Wcast-align	"			\
  "-Waggregate-return "			\
  "-Wmissing-noreturn "			\
  "-Wmissing-format-attribute "		\
  "-Wpacked "				\
  "-Wredundant-decls "			\
  "-Wnested-externs "			\
  "-Winline "				\
  "-Wno-pointer-sign "			\
  "-Wshadow "

static const JBFeature *jb_features[] = {
  &jb_pkg_config_feature,
  &jb_gettext_feature,
  &jb_intltool_feature,
  &jb_gconf_feature,
  &jb_gnome_help_feature,
  &jb_dbus_feature,
  &jb_gob2_feature
};

void
jb_package_init (void)
{
  JBVariableGroup *backends_group;
  JBVariableGroup *pi_group;

  /* by default, use the GNOME prefix */
  jb_set_prefix_from_program("gnome-open");

  /*
   * At the time of this writing, gob2 2.0.16 is not yet
   * released. What is required is gob2 2.0.15 + my patches (see the
   * gob mailing list), which should eventually become gob2 2.0.16.
   */
  jb_variable_set_string("gob2-minversion", "2.0.16");

  jb_evolution_plugin_init();

  backends_group = jb_variable_add_group("Mailbox backends");
  pi_group = jb_variable_add_group("IMAP and POP3 features");

  /* mailbox backends */
  jb_variable_add_bool("evolution",
		       "enable Evolution mailbox support",
		       backends_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("gmail",
		       "enable Gmail mailbox support",
		       backends_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("hotmail",
		       "enable Hotmail mailbox support",
		       backends_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("imap",
		       "enable IMAP mailbox support",
		       backends_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("maildir",
		       "enable Maildir mailbox support",
		       backends_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("mbox",
		       "enable mbox mailbox support",
		       backends_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("mh",
		       "enable MH mailbox support",
		       backends_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("mozilla",
		       "enable Mozilla products mailbox support",
		       backends_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("pop3",
		       "enable POP3 mailbox support",
		       backends_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("sylpheed",
		       "enable Sylpheed mailbox support",
		       backends_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("yahoo",
		       "enable Yahoo! Mail mailbox support",
		       backends_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);

  /* POP3 and IMAP features */
  jb_variable_add_bool("ipv6",
		       "enable IPv6 support",
		       pi_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("sasl",
		       "enable SASL authentication support",
		       pi_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);
  jb_variable_add_bool("ssl",
		       "enable SSL/TLS support",
		       pi_group,
		       JB_VARIABLE_C_DEFINE,
		       TRUE);

  /* misc */
  jb_variable_add_bool("compile-warnings", NULL, NULL, 0, FALSE);
  jb_variable_add_bool("debug", NULL, NULL, 0, FALSE);
  jb_variable_add_bool("regression-tests", NULL, NULL, 0, FALSE);
  jb_variable_add_bool("gconf-sanity-check", NULL, NULL, JB_VARIABLE_C_DEFINE, TRUE);
}

void
jb_package_configure (void)
{
  jb_check_glibc();

  jb_require_packages("GNOME", "gnome", "glib-2.0 >= 2.14 gthread-2.0 gconf-2.0 >= 2.4.0 gtk+-2.0 >= 2.12 libgnomeui-2.0 >= 2.14.0 gnome-vfs-2.0 libglade-2.0 libxml-2.0 libnotify >= 0.4.1");
  jb_require_packages("D-Bus", "dbus", "dbus-glib-1");

  jb_check_packages_for_options("GMime", "gmime", "gmime-2.0 >= 2.2.7",
				"hotmail",
				"imap",
				"maildir",
				"mbox",
				"mh",
				"mozilla",
				"pop3",
				"sylpheed",
				"yahoo",
				NULL);

  jb_check_packages_for_options("GNOME Keyring", "gnome-keyring", "gnome-keyring-1",
				"pop3",
				"imap",
				"gmail",
				"yahoo",
				"hotmail",
				NULL);

  if (jb_variable_get_bool("pop3") || jb_variable_get_bool("imap"))
    {
      jb_check_reentrant_dns_resolver();

      if (jb_variable_get_bool("pop3"))
	{
	  /* needed by mn-md5.c */
	  jb_endianness_check();
	  jb_string_arch_unaligned_check();
	}
    }
  else
    {
      if (jb_variable_get_bool("ipv6"))
	{
	  jb_warning("disabling option \"ipv6\" since options \"pop3\" and \"imap\" are disabled");
	  jb_variable_set_bool("ipv6", FALSE);
	}
      if (jb_variable_get_bool("sasl"))
	{
	  jb_warning("disabling option \"sasl\" since options \"pop3\" and \"imap\" are disabled");
	  jb_variable_set_bool("sasl", FALSE);
	}
      if (jb_variable_get_bool("ssl"))
	{
	  jb_warning("disabling option \"ssl\" since options \"pop3\" and \"imap\" are disabled");
	  jb_variable_set_bool("ssl", FALSE);
	}
    }

  if (jb_variable_get_bool("sasl"))
    {
      if (! jb_sasl2_check(NULL))
	{
	  jb_warning("disabling option \"sasl\" since Cyrus SASL was not found");
	  jb_variable_set_bool("sasl", FALSE);
	}
    }

  if (jb_variable_get_bool("ssl"))
    {
      if (jb_openssl_check("0.9.6"))
	{
	  if (! jb_openssl_check_mt())
	    {
	      jb_warning("disabling option \"ssl\" since OpenSSL does not support multi-threading");
	      jb_variable_set_bool("ssl", FALSE);
	    }
	}
      else
	{
	  jb_warning("disabling option \"ssl\" since OpenSSL was not found");
	  jb_variable_set_bool("ssl", FALSE);
	}
    }

  if (jb_variable_get_bool("evolution"))
    {
      if (! jb_evolution_plugin_check(NULL))
	{
	  jb_warning("disabling option \"evolution\" since Evolution was not found");
	  jb_variable_set_bool("evolution", FALSE);
	}
    }

  if (jb_variable_get_bool("gmail"))
    {
      if (jb_check_functions("timegm", NULL))
	jb_compile_options_add_cppflags(jb_compile_options, "-DHAVE_TIMEGM");
      else
	jb_warning("timegm() not available, Gmail message dates will not be displayed");
    }
}

static void
add_vfs_test (JBGroup *group, const char *name, int block_size)
{
  JBObject *object;
  char *program_name;
  char *cppflags;
  int i;

  program_name = g_strdup_printf("test-vfs-read-line-%s", name);
  object = JB_OBJECT(jb_program_new(program_name));

  jb_install_options_set_installdir(object->install_options, NULL);

  jb_object_add_sources(object,
			"../src/mn-vfs.c",
			"test-vfs-read-line.c",
			NULL);

  cppflags = g_strdup_printf("-Isrc -DREAD_LINE_BLOCK_SIZE=%i -DMN_REGRESSION_TEST", block_size);
  jb_compile_options_add_cppflags(object->compile_options, cppflags);
  g_free(cppflags);

  jb_compile_options_add_package(object->compile_options, "gnome");

  jb_group_add_resource(group, JB_GROUP_RESOURCE(object));

  for (i = 1; i < 3; i++)
    {
      JBRule *rule;
      char *outfile;
      char *infile;
      char *expected;

      outfile = g_strdup_printf("test-vfs-read-line%i-%s.output", i, name);
      infile = g_strdup_printf("test-vfs-read-line%i.input", i);
      expected = g_strdup_printf("test-vfs-read-line%i.expected", i);

      rule = jb_rule_new();

      jb_rule_add_dependency(rule, JB_GROUP_RESOURCE(object));

      jb_rule_add_input_file(rule, "$builddir/%s", program_name);
      jb_rule_add_input_file(rule, "$srcdir/%s", infile);
      jb_rule_add_input_file(rule, "$srcdir/%s", expected);
      jb_rule_add_output_file(rule, "$builddir/%s", outfile);

      jb_rule_set_build_message(rule, "running VFS test %i-%s", i, name);

      jb_rule_add_build_command(rule, "$builddir/%s file://`pwd`/$srcdir/%s > $builddir/%s",
				program_name, infile, outfile);
      jb_rule_add_build_command(rule, "cmp $builddir/%s $srcdir/%s",
				outfile, expected);

      jb_group_add_resource(group, JB_GROUP_RESOURCE(rule));

      jb_group_add_dist_files(group, infile, expected, NULL);

      g_free(outfile);
      g_free(infile);
      g_free(expected);
    }

  g_free(program_name);
}

void
jb_package_add_resources (void)
{
  JBGroup *group;
  JBRule *rule;
  JBObject *object;

  if (jb_variable_get_bool("compile-warnings"))
    jb_compile_options_add_cflags(jb_compile_options, MN_WARNING_CFLAGS " -Werror");

  if (! jb_variable_get_bool("debug"))
    jb_compile_options_add_cflags(jb_compile_options, "-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS");

  jb_compile_options_add_gob2flags(jb_compile_options, "--exit-on-warn");

  /*** art *******************************************************************/

  group = jb_group_new("art");

  jb_group_add_data_files(group,
			  "16x16/apps/mail-notification.png", "$datadir/icons/hicolor/16x16/apps",
			  "22x22/apps/mail-notification.png", "$datadir/icons/hicolor/22x22/apps",
			  "24x24/apps/mail-notification.png", "$datadir/icons/hicolor/24x24/apps",
			  "32x32/apps/mail-notification.png", "$datadir/icons/hicolor/32x32/apps",
			  "48x48/apps/mail-notification.png", "$datadir/icons/hicolor/48x48/apps",
			  "scalable/apps/mail-notification.svg", "$datadir/icons/hicolor/scalable/apps",
			  NULL);

  jb_group_add_data_file(group, "logo.png", "$pkgdatadir");

  if (jb_variable_get_bool("gmail"))
    jb_group_add_data_file(group, "gmail.png", "$pkgdatadir");

  if (jb_variable_get_bool("yahoo"))
    jb_group_add_data_file(group, "yahoo.png", "$pkgdatadir");

  if (jb_variable_get_bool("hotmail"))
    jb_group_add_data_file(group, "hotmail.png", "$pkgdatadir");

  rule = jb_rule_new();
  jb_rule_set_install_message(rule, "updating the GTK+ icon cache");
  jb_rule_add_install_command(rule, "-gtk-update-icon-cache -f -t $datadir/icons/hicolor");
  jb_group_add_resource(group, JB_GROUP_RESOURCE(rule));

  jb_group_add(group);

  /*** data ******************************************************************/

  group = jb_group_new("data");

  jb_group_add_desktop_file(group, "mail-notification-properties.desktop.in", "$datadir/applications");
  jb_group_add_desktop_file(group, "mail-notification.desktop.in", "$sysconfdir/xdg/autostart");

  jb_group_add_resource(group, JB_GROUP_RESOURCE(jb_template_new("mail-notification.schemas.in.in")));
  jb_group_add_gconf_schemas(group, "mail-notification.schemas.in");

  jb_group_add_data_file(group, "new-mail.wav", "$pkgdatadir");

  jb_group_add_dist_file(group, "sylpheed-locking.diff");

  jb_group_add(group);

  /*** help ******************************************************************/

  group = jb_group_new("help");

  jb_group_add_resource(group, JB_GROUP_RESOURCE(jb_gnome_help_new("C", "documentation-license.xml software-license.xml")));

  jb_group_add(group);

  /*** po ********************************************************************/

  group = jb_group_new("po");

  jb_group_add_translations(group, "bg ca cs de es fr ja nl pl pt pt_BR ru sr sr@Latn sv");

  jb_group_add(group);

  /*** src *******************************************************************/

  group = jb_group_new("src");

  jb_compile_options_add_string_defines(group->compile_options,
					"GETTEXT_PACKAGE", "$package",
					NULL);

  object = JB_OBJECT(jb_program_new("mail-notification"));

  if (jb_variable_get_bool("glibc"))
    {
      /*
       * We need -std=c99 for lround(), ...
       * We need _BSD_SOURCE (which requires -lbsd-compat) for fchmod(), ...
       * We need _POSIX_C_SOURCE for fdopen(), ...
       */
      jb_compile_options_add_cflags(object->compile_options, "-std=c99");
      jb_compile_options_add_cppflags(object->compile_options, "-D_BSD_SOURCE -D_POSIX_C_SOURCE=199309L");
      jb_compile_options_add_libs(object->compile_options, "-lbsd-compat");
    }

  jb_compile_options_add_string_defines(object->compile_options,
					"PACKAGE", "$package",
					"VERSION", "$version",
					"PREFIX", "$prefix",
					"SYSCONFDIR", "$sysconfdir",
					"DATADIR", "$datadir",
					"PKGDATADIR", "$pkgdatadir",
					"LIBDIR", "$libdir",
					"GNOMELOCALEDIR", "$datadir/locale",
					NULL);

  /*
   * We need --export-dynamic because because libglade needs to
   * resolve symbols from our own binary in order to autoconnect
   * signal handlers.
   */
  jb_compile_options_add_ldflags(object->compile_options, "-Wl,--export-dynamic");

  jb_compile_options_add_libs(object->compile_options, "-lm");

  jb_compile_options_add_package(object->compile_options, "gettext");
  jb_compile_options_add_package(object->compile_options, "gnome");
  jb_compile_options_add_package(object->compile_options, "dbus");

  jb_group_add_dbus_interface(group,
			      "org.gnome.MailNotification",
			      "mn-client-dbus.h",
			      "mn-server-dbus.h",
			      "mn_server");

  jb_object_add_sources(object,
			"MN:About:Dialog",
			"MN:Autodetect:Mailbox:Properties",
			"MN:Compact:Message:View",
			"MN:Dialog",
			"MN:File:Chooser:Button",
			"MN:Mail:Icon",
			"MN:Mail:Icon:Widget",
			"MN:Mailbox",
			"MN:Mailbox:Properties",
			"MN:Mailbox:Properties:Dialog",
			"MN:Mailbox:View",
			"MN:Mailboxes",
			"MN:Message",
			"MN:Message:View",
			"MN:Popup",
			"MN:Popups",
			"MN:Properties:Dialog",
			"MN:Server",
			"MN:Shell",
			"MN:Sound:File:Chooser:Dialog",
			"MN:Sound:Player",
			"MN:Standard:Message:View",
			"MN:Test:Mailbox",
			"MN:Text:Table",
			"MN:Tooltips",
			"eggtrayicon",
			"mn-conf",
			"mn-decls.h",
			"mn-locked-callback",
			"mn-main.c",
			"mn-non-linear-range",
			"mn-stock",
			"mn-util",
			"mn-vfs",
			"mn-xml",
			"nautilus-cell-renderer-pixbuf-emblem",
			NULL);

  if (jb_variable_get_bool("mbox"))
    jb_object_add_source(object, "MN:Mbox:Mailbox:Backend");

  if (jb_variable_get_bool("mozilla"))
    jb_object_add_source(object, "MN:Mozilla:Mailbox:Backend");

  if (jb_variable_get_bool("mbox") || jb_variable_get_bool("mozilla"))
    jb_object_add_source(object, "MN:Base:Mbox:Mailbox:Backend");

  if (jb_variable_get_bool("mh"))
    jb_object_add_source(object, "MN:MH:Mailbox:Backend");

  if (jb_variable_get_bool("maildir"))
    jb_object_add_sources(object,
			  "MN:Maildir:Mailbox:Backend",
			  "MN:Maildir:Message",
			  NULL);

  if (jb_variable_get_bool("pop3"))
    jb_object_add_sources(object,
			  "MN:POP3:Mailbox",
			  "MN:POP3:Mailbox:Properties",
			  "mn-md5",
			  NULL);

  if (jb_variable_get_bool("imap"))
    jb_object_add_sources(object,
			  "MN:IMAP:Mailbox",
			  "MN:IMAP:Mailbox:Properties",
			  NULL);

  if (jb_variable_get_bool("pop3") || jb_variable_get_bool("imap"))
    jb_object_add_sources(object,
			  "MN:Auth:Combo:Box",
			  "MN:PI:Mailbox",
			  "MN:PI:Mailbox:Properties",
			  "mn-client-session",
			  NULL);

  if (jb_variable_get_bool("pop3")
      || jb_variable_get_bool("imap")
      || jb_variable_get_bool("gmail")
      || jb_variable_get_bool("yahoo")
      || jb_variable_get_bool("hotmail"))
    {
      jb_compile_options_add_package(object->compile_options, "gnome-keyring");
      jb_object_add_sources(object,
			    "MN:Authenticated:Mailbox",
			    "MN:Authenticated:Mailbox:Properties",
			    "mn-keyring",
			    NULL);
    }

  if (jb_variable_get_bool("sylpheed"))
    jb_object_add_sources(object,
			  "MN:Sylpheed:Mailbox:Backend",
			  "MN:Sylpheed:Message",
			  NULL);

  if (jb_variable_get_bool("maildir") || jb_variable_get_bool("sylpheed"))
    jb_object_add_source(object, "MN:VFS:Message");

  if (jb_variable_get_bool("gmail"))
    jb_object_add_sources(object,
			  "MN:Gmail:Mailbox",
			  "MN:Gmail:Mailbox:Properties",
			  NULL);

  if (jb_variable_get_bool("yahoo"))
    jb_object_add_sources(object,
			  "MN:Yahoo:Mailbox",
			  "MN:Yahoo:Mailbox:Properties",
			  NULL);

  if (jb_variable_get_bool("hotmail"))
    jb_object_add_sources(object,
			  "MN:Hotmail:Mailbox",
			  "MN:Hotmail:Mailbox:Properties",
			  NULL);

  if (jb_variable_get_bool("yahoo") || jb_variable_get_bool("hotmail"))
    jb_object_add_sources(object,
			  "MN:Webmail:Mailbox",
			  "MN:Webmail:Mailbox:Properties",
			  NULL);

  if (jb_variable_get_bool("mbox")
      || jb_variable_get_bool("mozilla")
      || jb_variable_get_bool("mh")
      || jb_variable_get_bool("maildir")
      || jb_variable_get_bool("sylpheed"))
    jb_object_add_sources(object,
			  "MN:Custom:VFS:Mailbox",
			  "MN:Reentrant:Mailbox",
			  "MN:System:VFS:Mailbox",
			  "MN:System:VFS:Mailbox:Properties",
			  "MN:VFS:Mailbox",
			  "MN:VFS:Mailbox:Backend",
			  NULL);

  if (jb_variable_get_bool("hotmail")
      || jb_variable_get_bool("imap")
      || jb_variable_get_bool("maildir")
      || jb_variable_get_bool("mbox")
      || jb_variable_get_bool("mh")
      || jb_variable_get_bool("mozilla")
      || jb_variable_get_bool("pop3")
      || jb_variable_get_bool("sylpheed")
      || jb_variable_get_bool("yahoo"))
    {
      jb_compile_options_add_cppflags(object->compile_options, "-DWITH_GMIME=1");
      jb_compile_options_add_package(object->compile_options, "gmime");
      jb_object_add_sources(object,
			    "mn-message-mime",
			    "MN:GMime:Stream:VFS",
			    NULL);
    }

  if (jb_variable_get_bool("ssl"))
    {
      jb_compile_options_add_package(object->compile_options, "openssl");
      jb_object_add_source(object, "mn-ssl");
    }

  if (jb_variable_get_bool("sasl"))
    {
      jb_compile_options_add_package(object->compile_options, "sasl2");
      jb_object_add_source(object, "mn-sasl");
    }

  if (jb_variable_get_bool("evolution"))
    {
      JBObject *plugin;

      jb_group_add_resource(group, JB_GROUP_RESOURCE(jb_template_new("org-jylefort-mail-notification.eplug.in")));
      jb_group_add_data_file(group, "org-jylefort-mail-notification.eplug", "$evolution-plugin-dir");

      jb_compile_options_add_cflags(object->compile_options, "$evolution-plugin-cflags");

      jb_group_add_dbus_interface(group,
				  "org.freedesktop.DBus.Properties",
				  "mn-dbus-properties-client-dbus.h",
				  NULL,
				  NULL);
      jb_group_add_dbus_interface(group,
				  "org.gnome.MailNotification.Evolution",
				  "mn-evolution-client-dbus.h",
				  NULL,
				  NULL);

      jb_object_add_sources(object,
			    "MN:Evolution:Client",
			    "MN:Evolution:Folder:Tree:Client",
			    "MN:Evolution:Mailbox",
			    "MN:Evolution:Mailbox:Properties",
			    "MN:Evolution:Message",
			    "mn-evolution.h",
			    NULL);

      plugin = JB_OBJECT(jb_module_new("liborg-jylefort-mail-notification"));

      jb_install_options_set_installdir(plugin->install_options, "$evolution-plugin-dir");

      jb_compile_options_add_package(plugin->compile_options, "gettext");
      jb_compile_options_add_package(plugin->compile_options, "evolution-plugin");
      jb_compile_options_add_package(plugin->compile_options, "dbus");

      jb_group_add_dbus_interface(group,
				  "org.gnome.MailNotification.Evolution",
				  NULL,
				  "mn-evolution-server-dbus.h",
				  "mn_evolution_server");
      jb_group_add_dbus_interface(group,
				  "org.gnome.MailNotification.Evolution.FolderTree",
				  NULL,
				  "mn-evolution-folder-tree-server-dbus.h",
				  "mn_evolution_folder_tree_server");

      jb_object_add_sources(plugin,
			    "MN:Evolution:Folder:Tree:Server",
			    "MN:Evolution:Server",
			    "mn-evolution-plugin",
			    "mn-evolution.h",
			    NULL);

      jb_group_add_resource(group, JB_GROUP_RESOURCE(plugin));
    }

  jb_group_add_resource(group, JB_GROUP_RESOURCE(object));

  jb_group_add(group);

  /*** ui ********************************************************************/

  group = jb_group_new("ui");

  jb_group_add_data_files(group,
			  "mailbox-properties-dialog.glade", "$pkgdatadir",
			  "properties-dialog.glade", "$pkgdatadir",
			  NULL);

  jb_group_add(group);

  if (jb_variable_get_bool("regression-tests"))
    {
      group = jb_group_new("tests");

      add_vfs_test(group, "smallblock", 4);
      add_vfs_test(group, "largeblock", 16384);

      jb_group_add(group);
    }
}

JB_MAIN("mail-notification", "5.3", "Mail Notification")
