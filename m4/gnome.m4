# Mail Notification
# Copyright (C) 2003-2008 Jean-Yves Lefort <jylefort@brutele.be>
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
