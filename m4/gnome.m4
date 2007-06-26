# This file is part of Mail Notification.
#
# Copyright (C) 2005-2007 Jean-Yves Lefort.
#
# As a special exception to the Mail Notification licensing terms,
# Jean-Yves Lefort gives unlimited permission to copy, distribute and
# modify this file.

dnl AM_PATH_GNOME_PREFIX([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl
AC_DEFUN([AM_PATH_GNOME_PREFIX],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])

AC_MSG_CHECKING([for the GNOME installation prefix])

gnome_prefix=""
if test -n "$PKG_CONFIG"; then
	gnome_prefix=`$PKG_CONFIG --variable prefix libgnome-2.0 2>/dev/null`
fi

if test -n "$gnome_prefix"; then
	AC_MSG_RESULT([$gnome_prefix])
	ifelse([$1],, :, [$1])
else
	AC_MSG_RESULT([not found])
	ifelse([$2],, :, [$2])
fi

AC_SUBST(gnome_prefix)])
