# This file is part of Mail Notification.
#
# Copyright (C) 2005 Jean-Yves Lefort.
#
# As a special exception to the Mail Notification licensing terms,
# Jean-Yves Lefort gives unlimited permission to copy, distribute and
# modify this file.

dnl AM_PATH_GNOME_PREFIX([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl
AC_DEFUN([AM_PATH_GNOME_PREFIX],
[PKG_PROG_PKG_CONFIG

AC_MSG_CHECKING([for the GNOME installation prefix])

gnome_prefix=""
if test -n "$PKG_CONFIG"; then
	gnome_prefix=`$PKG_CONFIG --variable prefix libgnome-2.0`
fi

if test -n "$gnome_prefix"; then
	AC_MSG_RESULT([$gnome_prefix])
	ifelse([$1],, :, [$1])
else
	AC_MSG_RESULT([not found])
	ifelse([$2],, :, [$2])
fi

AC_SUBST(gnome_prefix)])

dnl AM_PATH_GNOME_CAPPLETDIR
dnl
AC_DEFUN([AM_PATH_GNOME_CAPPLETDIR],
[PKG_PROG_PKG_CONFIG

AC_MSG_CHECKING([for the GNOME capplet directory])

# GNOME < 2.10
gnome_cappletdir='${datadir}/control-center-2.0/capplets'

if test -n "$PKG_CONFIG" && $PKG_CONFIG --exists 'libgnome-2.0 >= 2.10.0' >/dev/null 2>&1; then
	# GNOME >= 2.10
	gnome_cappletdir='${datadir}/applications'
fi

AC_MSG_RESULT([$gnome_cappletdir])

AC_SUBST(gnome_cappletdir)])
