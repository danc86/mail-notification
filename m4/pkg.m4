# $Id: pkg.m4,v 1.5 2004/05/26 18:02:11 jylefort Exp $
#
# This file is part of Mail Notification.
#
# Copyright (c) 2002, 2003, 2004 Jean-Yves Lefort.
#
# As a special exception to the Mail Notification licensing terms,
# Jean-Yves Lefort gives unlimited permission to copy, distribute and
# modify this file.

dnl AM_PATH_PKG(VARIABLE_PREFIX, MODULES, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl
AC_DEFUN([AM_PATH_PKG],
[found=no
$1_CFLAGS=""
$1_LIBS=""

if test -z "$PKG_CONFIG"; then
	AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
fi

if test "x$PKG_CONFIG" = "xno" ; then
	AC_MSG_WARN([the pkg-config script could not be found: make sure it is in your path, or set the PKG_CONFIG environment variable to the full path to pkg-config])
else
	PKG_CONFIG_MIN_VERSION=0.9.0

	if $PKG_CONFIG --atleast-pkgconfig-version $PKG_CONFIG_MIN_VERSION; then
		AC_MSG_CHECKING([for $2])

		if $PKG_CONFIG --exists "$2"; then
			found=yes

			$1_CFLAGS=`$PKG_CONFIG --cflags "$2"`
			$1_LIBS=`$PKG_CONFIG --libs "$2"`

			AC_MSG_RESULT(yes)
		else
			AC_MSG_RESULT(no)
		fi
	else
		AC_MSG_WARN([your version of pkg-config is too old, you need version $PKG_CONFIG_MIN_VERSION or newer])
	fi
fi

if test $found = no; then
	ifelse([$4],, :, [$4])
else
	ifelse([$3],, :, [$3])
fi

AC_SUBST($1_CFLAGS)
AC_SUBST($1_LIBS)])
