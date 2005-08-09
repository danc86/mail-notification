# This file is part of Mail Notification.
#
# Copyright (C) 2005 Jean-Yves Lefort.
#
# As a special exception to the Mail Notification licensing terms,
# Jean-Yves Lefort gives unlimited permission to copy, distribute and
# modify this file.

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
