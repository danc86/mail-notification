AC_INIT(mail-notification, 0.5.0, jylefort@brutele.be)
AC_CONFIG_SRCDIR(src/mn-main.c)

AC_PREREQ(2.59)

### command line arguments

AC_ARG_ENABLE(mbox,
AC_HELP_STRING([--disable-mbox], [disable mbox support]),
[case "$enableval" in
	yes|no) enable_mbox=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-mbox]) ;;
esac], [enable_mbox=yes])

AC_ARG_ENABLE(mh,
AC_HELP_STRING([--disable-mh], [disable MH support]),
[case "$enableval" in
	yes|no) enable_mh=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-mh]) ;;
esac], [enable_mh=yes])

AC_ARG_ENABLE(maildir,
AC_HELP_STRING([--disable-maildir], [disable Maildir support]),
[case "$enableval" in
	yes|no) enable_maildir=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-maildir]) ;;
esac], [enable_maildir=yes])

AC_ARG_ENABLE(pop3,
AC_HELP_STRING([--disable-pop3], [disable POP3 support]),
[case "$enableval" in
	yes|no) enable_pop3=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-pop3]) ;;
esac], [enable_pop3=yes])

AC_ARG_ENABLE(sylpheed,
AC_HELP_STRING([--disable-sylpheed], [disable Sylpheed support]),
[case "$enableval" in
	yes|no) enable_sylpheed=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-sylpheed]) ;;
esac], [enable_sylpheed=yes])

AC_ARG_ENABLE(gmail,
AC_HELP_STRING([--disable-gmail], [disable Gmail support]),
[case "$enableval" in
	yes|no) enable_gmail=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-gmail]) ;;
esac], [enable_gmail=yes])

AC_ARG_G_ASSERTIONS
AC_ARG_COMPILE_WARNINGS

### initialization

AM_INIT_AUTOMAKE(foreign)
AM_MAINTAINER_MODE
AM_CONFIG_HEADER(config.h)

### i18n

GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Define to the gettext package name])
AC_SUBST(GETTEXT_PACKAGE)

ALL_LINGUAS="fr ru"
AM_GLIB_GNU_GETTEXT

AC_PROG_INTLTOOL

### system features

AC_PROG_CC
AC_SYS_LARGEFILE

AC_CHECK_HEADERS(errno.h stdarg.h stdio.h stdlib.h string.h,, [AC_MSG_ERROR([header not found])])
AC_CHECK_FUNCS(atoi exit sscanf strchr strcmp strlen strncmp,, [AC_MSG_ERROR([library function not found])])

### build dependencies

AM_PATH_GTK_2_0(2.4.0,, [AC_MSG_ERROR([unable to find the GTK+ library])])

AM_PATH_PKG(GNOME, [libgnomeui-2.0 gnome-vfs-2.0 libglade-2.0 eel-2.0],, [AC_MSG_ERROR([unable to find the GNOME libraries])])
AC_SUBST(GNOME_CFLAGS)
AC_SUBST(GNOME_LIBS)

AM_GCONF_SOURCE_2

AC_PATH_PROG(GCONFTOOL, gconftool-2)
if test -z "$GCONFTOOL"; then
	AC_MSG_ERROR([unable to find the gconftool-2 program])
fi

if test $enable_pop3 = yes; then
	AM_PATH_PKG(GNET, gnet-2.0,, [AC_MSG_WARN([unable to find the GNet library: forcing --disable-pop3]); enable_pop3=no; disable_pop3_reason="(GNet not found)"])
fi

have_soup=no
if test $enable_gmail = yes; then
	AM_PATH_PKG(SOUP, soup-2.0, have_soup=yes, [AC_MSG_WARN([unable to find the soup library: forcing --disable-gmail]); enable_gmail=no; disable_gmail_reason="(libsoup not found)"])
fi	

### Automake conditionals

AM_CONDITIONAL(WITH_MBOX, [test $enable_mbox != no])
AM_CONDITIONAL(WITH_MH, [test $enable_mh != no])
AM_CONDITIONAL(WITH_MAILDIR, [test $enable_maildir != no])
AM_CONDITIONAL(WITH_POP3, [test $enable_pop3 != no])
AM_CONDITIONAL(WITH_SYLPHEED, [test $enable_sylpheed != no])
AM_CONDITIONAL(WITH_GMAIL, [test $enable_gmail != no])
AM_CONDITIONAL(HAVE_SOUP, [test $have_soup != no])

### config.h definitions

AC_DEFINE(G_LOG_DOMAIN, ["AC_PACKAGE_NAME"], [log domain])

if test $enable_mbox = yes; then
	AC_DEFINE(WITH_MBOX, 1, [Define to 1 if mbox support is enabled])
fi
if test $enable_mh = yes; then
	AC_DEFINE(WITH_MH, 1, [Define to 1 if MH support is enabled])
fi
if test $enable_maildir = yes; then
	AC_DEFINE(WITH_MAILDIR, 1, [Define to 1 if Maildir support is enabled])
fi
if test $enable_pop3 = yes; then
	AC_DEFINE(WITH_POP3, 1, [Define to 1 if POP3 support is enabled])
fi
if test $enable_sylpheed = yes; then
	AC_DEFINE(WITH_SYLPHEED, 1, [Define to 1 if Sylpheed support is enabled])
fi
if test $enable_gmail = yes; then
	AC_DEFINE(WITH_GMAIL, 1, [Define to 1 if Gmail support is enabled])
fi

### output

AC_CONFIG_FILES(Makefile
		art/Makefile
		data/Makefile
		help/Makefile
		help/C/Makefile
		m4/Makefile 
		po/Makefile.in
		src/Makefile
		ui/Makefile)
AC_OUTPUT

### report

cat <<EOF

$PACKAGE $VERSION is ready to be built.
The following settings will be used:

Installation prefixes
  --prefix                      $prefix
  --exec-prefix                 $exec_prefix

Installation directories
  --bindir                      $bindir
  --datadir                     $datadir
  --sysconfdir                  $sysconfdir

Features
  --enable-mbox                 $enable_mbox
  --enable-mh                   $enable_mh
  --enable-maildir              $enable_maildir
  --enable-pop3                 $enable_pop3 $disable_pop3_reason
  --enable-sylpheed             $enable_sylpheed
  --enable-gmail                $enable_gmail $disable_gmail_reason
EOF
