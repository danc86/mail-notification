AC_INIT(mail-notification, 0.6.2, jylefort@brutele.be)
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

AC_ARG_ENABLE(imap,
AC_HELP_STRING([--disable-imap], [disable IMAP support]),
[case "$enableval" in
	yes|no) enable_imap=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-imap]) ;;
esac], [enable_imap=yes])

AC_ARG_ENABLE(ssl,
AC_HELP_STRING([--disable-ssl], [disable SSL/TLS support]),
[case "$enableval" in
	yes|no) enable_ssl=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-ssl]) ;;
esac], [enable_ssl=yes])

AC_ARG_ENABLE(sasl,
AC_HELP_STRING([--disable-sasl], [disable SASL authentication support]),
[case "$enableval" in
	yes|no) enable_sasl=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-sasl]) ;;
esac], [enable_sasl=yes])

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

AC_ARG_ENABLE(ipv6,
AC_HELP_STRING([--disable-ipv6], [disable IPv6 support]),
[case "$enableval" in
	yes|no) enable_ipv6=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-ipv6]) ;;
esac], [enable_ipv6=yes])

AC_ARG_G_ASSERTIONS
AC_ARG_COMPILE_WARNINGS

AC_ARG_ENABLE(regression-tests,
AC_HELP_STRING([--enable-regression-tests], [build and run regression tests [[no]]]),
[case "$enableval" in
	yes|no) enable_regression_tests=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-regression-tests]) ;;
esac], [enable_regression_tests=no])

### initialization

AM_INIT_AUTOMAKE(foreign)
AM_MAINTAINER_MODE
AM_CONFIG_HEADER(config.h)

### i18n

GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Define to the gettext package name])
AC_SUBST(GETTEXT_PACKAGE)

ALL_LINGUAS="de fr pt ru"
AM_GLIB_GNU_GETTEXT

AC_PROG_INTLTOOL

### system features

AC_PROG_CC
AC_SYS_LARGEFILE

# the order of these tests matters
AC_CHECK_HEADERS(errno.h signal.h stdarg.h stdio.h stdlib.h string.h sys/types.h unistd.h,, [AC_MSG_ERROR([header not found])])
AC_CHECK_HEADER(sys/uio.h,, [AC_MSG_ERROR([header not found])],
[#include <sys/types.h>])
AC_CHECK_HEADER(sys/socket.h,, [AC_MSG_ERROR([header not found])],
[#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>])
AC_CHECK_HEADER(netdb.h,, [AC_MSG_ERROR([header not found])],
[#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>])
AC_CHECK_HEADER(netinet/in.h,, [AC_MSG_ERROR([header not found])],
[#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>])
AC_CHECK_HEADER(arpa/inet.h,, [AC_MSG_ERROR([header not found])],
[#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>])

AC_CHECK_FUNCS(atoi close connect exit freeaddrinfo gai_strerror getaddrinfo getnameinfo getpeername getsockname memcpy memset read signal sleep socket sscanf strchr strcmp strlen strncmp strstr write,, [AC_MSG_ERROR([library function not found])])

### build dependencies

AM_PATH_GTK_2_0(2.4.0,, [AC_MSG_ERROR([unable to find the GTK+ library])])

PKG_CHECK_MODULES(GNOME, [gthread-2.0 gconf-2.0 >= 2.4.0 libgnomeui-2.0 gnome-vfs-2.0 libglade-2.0 eel-2.0 >= 2.6.0 bonobo-activation-2.0],, [AC_MSG_ERROR([unable to find the GNOME libraries])])
AC_SUBST(GNOME_CFLAGS)
AC_SUBST(GNOME_LIBS)

AM_GCONF_SOURCE_2

AC_PATH_PROG(GCONFTOOL, gconftool-2)
if test -z "$GCONFTOOL"; then
	AC_MSG_ERROR([unable to find the gconftool-2 program])
fi

if test $enable_pop3 = no && test $enable_imap = no; then
	if test $enable_ssl = yes; then
		AC_MSG_NOTICE([POP3 and IMAP support disabled: forcing --disable-ssl])
		enable_ssl=no
		disable_ssl_reason="(not needed)"
	fi
	if test $enable_sasl = yes; then
		AC_MSG_NOTICE([POP3 and IMAP support disabled: forcing --disable-sasl])
		enable_sasl=no
		disable_sasl_reason="(not needed)"
	fi
fi

if test $enable_ssl = yes; then
	AM_PATH_OPENSSL(,, [AC_MSG_WARN([unable to find the OpenSSL library: forcing --disable-ssl]); enable_ssl=no; disable_ssl_reason="(OpenSSL not found)"])
fi

if test $enable_sasl = yes; then
	AM_PATH_SASL2(,, [AC_MSG_WARN([unable to find the Cyrus SASL library: forcing --disable-sasl]); enable_sasl=no; disable_sasl_reason="(Cyrus SASL not found)"])
fi

have_soup=no
if test $enable_gmail = yes; then
	PKG_CHECK_MODULES(SOUP, soup-2.0, have_soup=yes, [AC_MSG_WARN([unable to find the soup library: forcing --disable-gmail]); enable_gmail=no; disable_gmail_reason="(libsoup not found)"])
fi	

ORBIT_IDL="`$PKG_CONFIG --variable=orbit_idl ORBit-2.0`"
AC_SUBST(ORBIT_IDL)

LIBBONOBO_IDL="`$PKG_CONFIG --variable=idldir libbonobo-2.0`"
AC_SUBST(LIBBONOBO_IDL)

BONOBO_ACTIVATION_IDL="`$PKG_CONFIG --variable=idldir bonobo-activation-2.0`"
AC_SUBST(BONOBO_ACTIVATION_IDL)

AC_PATH_PROG(GOB2, gob2)

### Automake conditionals

AM_CONDITIONAL(WITH_MBOX, [test $enable_mbox = yes])
AM_CONDITIONAL(WITH_MH, [test $enable_mh = yes])
AM_CONDITIONAL(WITH_MAILDIR, [test $enable_maildir = yes])
AM_CONDITIONAL(WITH_POP3, [test $enable_pop3 = yes])
AM_CONDITIONAL(WITH_IMAP, [test $enable_imap = yes])
AM_CONDITIONAL(WITH_POP3_OR_IMAP, [test $enable_pop3 = yes || test $enable_imap = yes])
AM_CONDITIONAL(WITH_SSL, [test $enable_ssl = yes])
AM_CONDITIONAL(WITH_SASL, [test $enable_sasl = yes])
AM_CONDITIONAL(WITH_SYLPHEED, [test $enable_sylpheed = yes])
AM_CONDITIONAL(WITH_GMAIL, [test $enable_gmail = yes])
AM_CONDITIONAL(HAVE_SOUP, [test $have_soup = yes])
AM_CONDITIONAL(WITH_REGRESSION_TESTS, [test $enable_regression_tests = yes])

### config.h definitions

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
if test $enable_imap = yes; then
	AC_DEFINE(WITH_IMAP, 1, [Define to 1 if IMAP support is enabled])
fi
if test $enable_ssl = yes; then
	AC_DEFINE(WITH_SSL, 1, [Define to 1 if SSL/TLS support is enabled])
fi
if test $enable_sasl = yes; then
	AC_DEFINE(WITH_SASL, 1, [Define to 1 if SASL support is enabled])
fi
if test $enable_sylpheed = yes; then
	AC_DEFINE(WITH_SYLPHEED, 1, [Define to 1 if Sylpheed support is enabled])
fi
if test $enable_gmail = yes; then
	AC_DEFINE(WITH_GMAIL, 1, [Define to 1 if Gmail support is enabled])
fi
if test $enable_ipv6 = yes; then
	AC_DEFINE(WITH_IPV6, 1, [Define to 1 if IPv6 support is enabled])
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
		tests/Makefile
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
  --enable-imap                 $enable_imap $disable_imap_reason
  --enable-ssl                  $enable_ssl $disable_ssl_reason
  --enable-sasl                 $enable_sasl $disable_sasl_reason
  --enable-sylpheed             $enable_sylpheed
  --enable-gmail                $enable_gmail $disable_gmail_reason
  --enable-ipv6                 $enable_ipv6
EOF
