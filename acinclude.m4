# $Id: acinclude.m4,v 1.6 2004/06/03 19:15:30 jylefort Exp $
#
# Copyright (c) 2003 Jean-Yves Lefort.
#
# This file is part of Mail Notification.
# It may be distributed under the same terms as Mail Notification.

dnl AC_ARG_G_ASSERTIONS
dnl
AC_DEFUN([AC_ARG_G_ASSERTIONS],
[AC_ARG_ENABLE(assertions,
AC_HELP_STRING([--disable-assertions],
[disable GLib assertions (not recommended)]),
[case "$enableval" in
	yes|no) enable_assertions=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-assertions]) ;;
esac], [enable_assertions=yes])

if test $enable_assertions = no; then
	G_ASSERTIONS="-DG_DISABLE_ASSERT"
else
	G_ASSERTIONS=""
fi

AC_SUBST(G_ASSERTIONS)])

dnl AC_ARG_COMPILE_WARNINGS
dnl (only works with gcc)
dnl
AC_DEFUN([AC_ARG_COMPILE_WARNINGS],
[AC_ARG_ENABLE(compile-warnings,
AC_HELP_STRING([--enable-compile-warnings=no|yes|error],
[enable compiler warnings [[no]]]),
[case "$enableval" in
	yes|no|error) enable_compile_warnings=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-compile-warnings]) ;;
esac], [enable_compile_warnings=no])

if test $enable_compile_warnings = no; then
	WARN_CFLAGS=
else
	WARN_CFLAGS="-Wall -Wcast-align -Wredundant-decls -Wnested-externs -Winline"
	test $enable_compile_warnings = error && WARN_CFLAGS="$WARN_CFLAGS -Werror"
fi

AC_SUBST(WARN_CFLAGS)])
