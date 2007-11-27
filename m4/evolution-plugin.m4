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

dnl AM_PATH_EVOLUTION_PLUGIN([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl
AC_DEFUN([AM_PATH_EVOLUTION_PLUGIN],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])

# MN uses em_folder_tree_set_selected(), whose prototype was changed in 2.12
evolution_plugin_min_version=ifelse([$1],, [2.12], [$1])

PKG_CHECK_MODULES(EVOLUTION_PLUGIN, [evolution-plugin >= $evolution_plugin_min_version], [found=yes], [found=no])

if test $found = yes; then
	AC_MSG_CHECKING([for the Evolution plugin directory])
	evolution_plugindir=`$PKG_CONFIG --variable=plugindir evolution-plugin 2>/dev/null`
	if test -n "$evolution_plugindir"; then
		AC_MSG_RESULT([$evolution_plugindir])
	else
		AC_MSG_RESULT([not found])
		found=no
	fi
fi

if test $found = yes; then
	ifelse([$2],, :, [$2])
else
	evolution_plugindir=""
	ifelse([$3],, :, [$3])
fi

AC_SUBST(evolution_plugindir)])
