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

dnl AC_STRING_ARCH_UNALIGNED
dnl
AC_DEFUN([AC_STRING_ARCH_UNALIGNED],
[case "$target_cpu" in
	i[[34567]]86|x86_64|s390|s390x)	string_arch_unaligned=yes ;;
	*)				string_arch_unaligned=no ;;
esac

if test $string_arch_unaligned = yes; then
	AC_DEFINE(STRING_ARCH_UNALIGNED, 1, [taken from glibc])
fi])
