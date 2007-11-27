# Mail Notification
# Copyright (C) 2003-2007 Jean-Yves Lefort <jylefort@brutele.be>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

dnl AC_REENTRANT_RESOLVER
dnl
AC_DEFUN([AC_REENTRANT_RESOLVER],
[AC_ARG_WITH(reentrant-resolver,
AC_HELP_STRING([--with-reentrant-resolver], [specify that the system resolver is reentrant [[autodetect]]]),
[case "$withval" in
	yes|no|autodetect) reentrant_resolver=$withval ;;
	*) AC_MSG_ERROR([bad value $withval for --with-reentrant-resolver]) ;;
esac], [reentrant_resolver=autodetect])

if test $reentrant_resolver = autodetect; then
	AC_MSG_CHECKING([if the system resolver is reentrant])

	case "$target_os" in
		# FreeBSD >= 5.3
		freebsd5.[[3-9]]*|freebsd5.[[1-9]][[0-9]]*|freebsd[[6-9]]*|freebsd[[1-9]][[0-9]]*)
			reentrant_resolver=yes ;;

		# FreeBSD < 5.3, NetBSD, OpenBSD
		freebsd*|netbsd*|openbsd*)
			reentrant_resolver=no ;;

		# Linux
		linux*)
			reentrant_resolver=yes ;;

		*)
			reentrant_resolver=unknown ;;
	esac

	if test $reentrant_resolver = unknown; then
		reentrant_resolver=no
		AC_MSG_RESULT([unknown, assuming it is not (use --with-reentrant-resolver to override)])
	else
		AC_MSG_RESULT($reentrant_resolver)
	fi
fi

if test $reentrant_resolver = yes; then
	AC_DEFINE(HAVE_REENTRANT_RESOLVER, 1, [Define to 1 if the system resolver is reentrant])
fi])
