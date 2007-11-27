AC_INIT([Mail Notification], 5.0-rc1, jylefort@brutele.be)
AC_CONFIG_SRCDIR(src/mn-main.c)

AC_PREREQ(2.59)

# by default, install in the same prefix as GNOME
AC_PREFIX_PROGRAM(gnome-open)

### command line arguments

# mailbox backends
MN_ARG_ENABLE(evolution, [disable Evolution support])
MN_ARG_ENABLE(gmail, [disable Gmail support])
MN_ARG_ENABLE(hotmail, [disable Windows Live Hotmail support])
MN_ARG_ENABLE(imap, [disable IMAP support])
MN_ARG_ENABLE(maildir, [disable Maildir support])
MN_ARG_ENABLE(mbox, [disable mbox support])
MN_ARG_ENABLE(mh, [disable MH support])
MN_ARG_ENABLE(mozilla, [disable Mozilla products support])
MN_ARG_ENABLE(pop3, [disable POP3 support])
MN_ARG_ENABLE(sylpheed, [disable Sylpheed support])
MN_ARG_ENABLE(yahoo, [disable Yahoo! Mail support])

# POP3 and IMAP features
MN_ARG_ENABLE(ipv6, [disable IPv6 support])
MN_ARG_ENABLE(sasl, [disable SASL authentication support])
MN_ARG_ENABLE(ssl, [disable SSL/TLS support])

# miscellaneous
MN_ARG_COMPILE_WARNINGS
MN_ARG_DEBUG(yes)
MN_ARG_ENABLE(regression-tests, [build and run regression tests], no)
MN_ARG_ENABLE(gconf-sanity-check, [disable the startup GConf sanity check])

### initialization

# needed by AC_REENTRANT_RESOLVER and AC_STRING_ARCH_UNALIGNED
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_MAINTAINER_MODE
AM_CONFIG_HEADER(config.h)
AC_PROG_LIBTOOL

### i18n

GETTEXT_PACKAGE=mail-notification
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Define to the gettext package name])
AC_SUBST(GETTEXT_PACKAGE)

AM_GLIB_GNU_GETTEXT

IT_PROG_INTLTOOL([0.35.0])

### system features

AC_PROG_CC
AC_SYS_LARGEFILE

# check for ANSI C headers
AC_HEADER_STDC
if test $ac_cv_header_stdc != yes; then
	AC_MSG_ERROR([ANSI C headers not present])
fi

AC_REENTRANT_RESOLVER

AC_STRING_ARCH_UNALIGNED

# timegm() is non-standard, define HAVE_TIMEGM if it is present
AC_CHECK_FUNCS(timegm)

# see if "struct tm" is defined in sys/time.h
AC_STRUCT_TM

# defines WORDS_BIGENDIAN if relevant, needed by mn-md5.c
AC_C_BIGENDIAN

### mandatory libraries and programs

PKG_CHECK_MODULES(GNOME, [glib-2.0 >= 2.10 gthread-2.0 gconf-2.0 >= 2.4.0 gtk+-2.0 >= 2.10 libgnomeui-2.0 >= 2.14.0 gnome-vfs-2.0 libglade-2.0 eel-2.0 >= 2.6.0 bonobo-activation-2.0 libxml-2.0 libnotify >= 0.4.1],, [AC_MSG_ERROR([unable to find the GNOME libraries])])
AM_GCONF_SOURCE_2

AM_PATH_GNOME_PREFIX

AC_PATH_PROG(GCONFTOOL, gconftool-2)
if test -z "$GCONFTOOL"; then
	AC_MSG_ERROR([unable to find the gconftool-2 program])
fi

AM_PATH_ORBIT_IDL(, [AC_MSG_ERROR([unable to find the orbit-idl-2 program])])

IDL_CHECK_MODULES(BONOBO, [libbonobo-2.0 bonobo-activation-2.0],, [AC_MSG_ERROR([unable to find the Bonobo IDL modules])])

### optional libraries and programs

# not needed by end-users, as generated files are distributed
AC_PATH_PROG(AML, aml)
AC_PATH_PROG(GOB2, gob2)

MN_FEATURE_VAR_NAME(mime)=no
if MN_FEATURE_ENABLED(hotmail) ||
   MN_FEATURE_ENABLED(imap) ||
   MN_FEATURE_ENABLED(maildir) ||
   MN_FEATURE_ENABLED(mbox) ||
   MN_FEATURE_ENABLED(mh) ||
   MN_FEATURE_ENABLED(mozilla) ||
   MN_FEATURE_ENABLED(pop3) ||
   MN_FEATURE_ENABLED(sylpheed) ||
   MN_FEATURE_ENABLED(yahoo); then
	PKG_CHECK_MODULES(GMIME, [gmime-2.0 >= 2.1.0], [MN_FEATURE_VAR_NAME(mime)=yes])
	if MN_FEATURE_DISABLED(mime); then
		if MN_FEATURE_ENABLED(hotmail); then
			MN_FEATURE_DISABLE(hotmail, [GMime not found])
		fi
		if MN_FEATURE_ENABLED(imap); then
			MN_FEATURE_DISABLE(imap, [GMime not found])
		fi
		if MN_FEATURE_ENABLED(maildir); then
			MN_FEATURE_DISABLE(maildir, [GMime not found])
		fi
		if MN_FEATURE_ENABLED(mbox); then
			MN_FEATURE_DISABLE(mbox, [GMime not found])
		fi
		if MN_FEATURE_ENABLED(mh); then
			MN_FEATURE_DISABLE(mh, [GMime not found])
		fi
		if MN_FEATURE_ENABLED(mozilla); then
			MN_FEATURE_DISABLE(mozilla, [GMime not found])
		fi
		if MN_FEATURE_ENABLED(pop3); then
			MN_FEATURE_DISABLE(pop3, [GMime not found])
		fi
		if MN_FEATURE_ENABLED(sylpheed); then
			MN_FEATURE_DISABLE(sylpheed, [GMime not found])
		fi
		if MN_FEATURE_ENABLED(yahoo); then
			MN_FEATURE_DISABLE(yahoo, [GMime not found])
		fi
	fi
fi

MN_FEATURE_VAR_NAME(gnome-keyring)=no
if MN_FEATURE_ENABLED(pop3) ||
   MN_FEATURE_ENABLED(imap) ||
   MN_FEATURE_ENABLED(gmail) ||
   MN_FEATURE_ENABLED(yahoo) ||
   MN_FEATURE_ENABLED(hotmail); then
	PKG_CHECK_MODULES(GNOME_KEYRING, [gnome-keyring-1], [MN_FEATURE_VAR_NAME(gnome-keyring=yes)])
	if MN_FEATURE_DISABLED(gnome-keyring); then
		if MN_FEATURE_ENABLED(pop3); then
			MN_FEATURE_DISABLE(pop3, [gnome-keyring not found])
		fi
		if MN_FEATURE_ENABLED(imap); then
			MN_FEATURE_DISABLE(imap, [gnome-keyring not found])
		fi
		if MN_FEATURE_ENABLED(gmail); then
			MN_FEATURE_DISABLE(gmail, [gnome-keyring not found])
		fi
		if MN_FEATURE_ENABLED(yahoo); then
			MN_FEATURE_DISABLE(yahoo, [gnome-keyring not found])
		fi
		if MN_FEATURE_ENABLED(hotmail); then
			MN_FEATURE_DISABLE(hotmail, [gnome-keyring not found])
		fi
	fi
fi
	
if MN_FEATURE_DISABLED(pop3) && MN_FEATURE_DISABLED(imap); then
	if MN_FEATURE_ENABLED(ssl); then
		MN_FEATURE_DISABLE(ssl, [POP3 and IMAP support disabled])
	fi
	if MN_FEATURE_ENABLED(sasl); then
		MN_FEATURE_DISABLE(sasl, [POP3 and IMAP support disabled])
	fi
	if MN_FEATURE_ENABLED(ipv6); then
		MN_FEATURE_DISABLE(ipv6, [POP3 and IMAP support disabled])
	fi
fi

if MN_FEATURE_ENABLED(ssl); then
	AM_PATH_OPENSSL(0.9.6,, [MN_FEATURE_DISABLE(ssl, [OpenSSL not found])])
	if MN_FEATURE_ENABLED(ssl); then
		AC_OPENSSL_MT(, [MN_FEATURE_DISABLE(ssl, [OpenSSL does not support multi-threading])])
	fi
fi
if MN_FEATURE_ENABLED(sasl); then
	AM_PATH_SASL2(,, [MN_FEATURE_DISABLE(sasl, [Cyrus SASL not found])])
fi

if MN_FEATURE_ENABLED(evolution); then
	AM_PATH_EVOLUTION_PLUGIN(,, [MN_FEATURE_DISABLE(evolution, [Evolution not found])])
fi

### Automake conditionals

AM_CONDITIONAL(WITH_MBOX, [MN_FEATURE_ENABLED(mbox)])
AM_CONDITIONAL(WITH_MOZILLA, [MN_FEATURE_ENABLED(mozilla)])
AM_CONDITIONAL(WITH_MBOX_OR_MOZILLA, [MN_FEATURE_ENABLED(mbox) || MN_FEATURE_ENABLED(mozilla)])
AM_CONDITIONAL(WITH_MH, [MN_FEATURE_ENABLED(mh)])
AM_CONDITIONAL(WITH_MAILDIR, [MN_FEATURE_ENABLED(maildir)])
AM_CONDITIONAL(WITH_POP3, [MN_FEATURE_ENABLED(pop3)])
AM_CONDITIONAL(WITH_IMAP, [MN_FEATURE_ENABLED(imap)])
AM_CONDITIONAL(WITH_POP3_OR_IMAP, [MN_FEATURE_ENABLED(pop3) || MN_FEATURE_ENABLED(imap)])
AM_CONDITIONAL(WITH_POP3_OR_IMAP_OR_GMAIL_OR_YAHOO_OR_HOTMAIL, [MN_FEATURE_ENABLED(pop3) || MN_FEATURE_ENABLED(imap) || MN_FEATURE_ENABLED(gmail) || MN_FEATURE_ENABLED(yahoo) || MN_FEATURE_ENABLED(hotmail)])
AM_CONDITIONAL(WITH_YAHOO_OR_HOTMAIL, [MN_FEATURE_ENABLED(yahoo) || MN_FEATURE_ENABLED(hotmail)])
AM_CONDITIONAL(WITH_MBOX_OR_MOZILLA_OR_MH_OR_MAILDIR_OR_SYLPHEED, [MN_FEATURE_ENABLED(mbox) || MN_FEATURE_ENABLED(mozilla) || MN_FEATURE_ENABLED(mh) || MN_FEATURE_ENABLED(maildir) || MN_FEATURE_ENABLED(sylpheed)])
AM_CONDITIONAL(WITH_MBOX_OR_MOZILLA_OR_MH_OR_MAILDIR_OR_SYLPHEED_OR_EVOLUTION, [MN_FEATURE_ENABLED(mbox) || MN_FEATURE_ENABLED(mozilla) || MN_FEATURE_ENABLED(mh) || MN_FEATURE_ENABLED(maildir) || MN_FEATURE_ENABLED(sylpheed) || MN_FEATURE_ENABLED(evolution)])
AM_CONDITIONAL(WITH_MAILDIR_OR_SYLPHEED, [MN_FEATURE_ENABLED(maildir) || MN_FEATURE_ENABLED(sylpheed)])
AM_CONDITIONAL(WITH_SSL, [MN_FEATURE_ENABLED(ssl)])
AM_CONDITIONAL(WITH_SASL, [MN_FEATURE_ENABLED(sasl)])
AM_CONDITIONAL(WITH_GMAIL, [MN_FEATURE_ENABLED(gmail)])
AM_CONDITIONAL(WITH_YAHOO, [MN_FEATURE_ENABLED(yahoo)])
AM_CONDITIONAL(WITH_HOTMAIL, [MN_FEATURE_ENABLED(hotmail)])
AM_CONDITIONAL(WITH_EVOLUTION, [MN_FEATURE_ENABLED(evolution)])
AM_CONDITIONAL(WITH_SYLPHEED, [MN_FEATURE_ENABLED(sylpheed)])
AM_CONDITIONAL(WITH_MIME, [MN_FEATURE_ENABLED(mime)])
AM_CONDITIONAL(WITH_REGRESSION_TESTS, [MN_FEATURE_ENABLED(regression-tests)])

### config.h definitions

MN_FEATURES_DEFINE([mbox, mh, maildir, pop3, imap, ssl, sasl, ipv6, gmail, yahoo, hotmail, evolution, mozilla, sylpheed, mime, gconf-sanity-check])

### output

AC_CONFIG_FILES(Makefile
		art/Makefile
		data/Makefile
		help/Makefile
		help/C/Makefile
		m4/Makefile 
		po/Makefile.in
		src/Makefile
		tests/Makefile
		ui/Makefile)
AC_OUTPUT

### report

cat <<EOF

$PACKAGE_NAME $VERSION is ready to be built.
The following settings will be used:

Installation prefixes
MN_REPORT_ARGS([prefix, exec-prefix])
Installation directories
MN_REPORT_ARGS([bindir, datadir, datarootdir, libdir, sysconfdir])
Mailbox backends
MN_REPORT_FEATURES([evolution, gmail, hotmail, imap, maildir, mbox, mh, mozilla, pop3, sylpheed, yahoo])
POP3 and IMAP features
MN_REPORT_FEATURES([ipv6, sasl, ssl])
EOF

if test -z "$gnome_prefix"; then
	AC_MSG_WARN([$PACKAGE_NAME must be installed in the same prefix as GNOME, but the GNOME prefix could not be found. $PACKAGE_NAME might be inoperable.])
	echo "" >&2
else
	if test "x$prefix" != "x$gnome_prefix"; then
		AC_MSG_WARN([the $PACKAGE_NAME prefix ($prefix) is different from the GNOME prefix ($gnome_prefix). $PACKAGE_NAME will be inoperable. Unless you know what you are doing, please run "./configure --prefix=$gnome_prefix".])
		echo "" >&2
	fi
fi

cat <<EOF
Type "make" to build $PACKAGE_NAME $VERSION.
EOF
