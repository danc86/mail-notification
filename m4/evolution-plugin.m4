# This file is part of Mail Notification.
#
# Copyright (C) 2005, 2006 Jean-Yves Lefort.
#
# As a special exception to the Mail Notification licensing terms,
# Jean-Yves Lefort gives unlimited permission to copy, distribute and
# modify this file.

dnl AM_PATH_EVOLUTION_PLUGIN([BRANCH], [MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl
AC_DEFUN([AM_PATH_EVOLUTION_PLUGIN],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])

evolution_branch=ifelse([$1],, [2.4], [$1])
evolution_plugin_min_version=ifelse([$2],,, [>= $2])

PKG_CHECK_MODULES(EVOLUTION_PLUGIN, [evolution-plugin-$evolution_branch $evolution_plugin_min_version], [found=yes], [found=no])

if test $found = yes; then
	AC_MSG_CHECKING([for the Evolution plugin directory])
	evolution_plugindir=`$PKG_CONFIG --variable=plugindir evolution-plugin-$evolution_branch 2>/dev/null`
	if test -n "$evolution_plugindir"; then
		AC_MSG_RESULT([$evolution_plugindir])
	else
		AC_MSG_RESULT([not found])
		found=no
	fi
fi

if test $found = yes; then
	ifelse([$3],, :, [$3])
else
	evolution_plugindir=""
	ifelse([$4],, :, [$4])
fi

AC_SUBST(evolution_plugindir)])
