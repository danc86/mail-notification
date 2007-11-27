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

### feature handling ##########################################################

dnl MN_FEATURE_VAR_NAME(FEATURE)
dnl
AC_DEFUN([MN_FEATURE_VAR_NAME], [enable_[]translit([$1], -, _)])

dnl MN_FEATURE_DISABLE_REASON_VAR_NAME(FEATURE)
dnl
AC_DEFUN([MN_FEATURE_DISABLE_REASON_VAR_NAME], [disable_[]translit([$1], -, _)_reason])

dnl MN_FEATURE_SYMBOL_NAME(FEATURE)
dnl
AC_DEFUN([MN_FEATURE_SYMBOL_NAME], [WITH_[]translit([$1], -a-z, _A-Z)])

dnl MN_FEATURE_ENABLED(FEATURE)
dnl
AC_DEFUN([MN_FEATURE_ENABLED], [test $MN_FEATURE_VAR_NAME([$1]) = yes])

dnl MN_FEATURE_DISABLED(FEATURE)
dnl
AC_DEFUN([MN_FEATURE_DISABLED], [test $MN_FEATURE_VAR_NAME([$1]) = no])

dnl MN_FEATURE_DISABLE(FEATURE, REASON)
dnl
AC_DEFUN([MN_FEATURE_DISABLE],
[AC_MSG_NOTICE([[$2]: forcing --disable-[$1]])
MN_FEATURE_VAR_NAME([$1])=no
MN_FEATURE_DISABLE_REASON_VAR_NAME([$1])="([$2])"])

dnl MN_FEATURE_DEFINE(FEATURE)
dnl
AC_DEFUN([MN_FEATURE_DEFINE],
[if MN_FEATURE_ENABLED([$1]); then _value=1; else _value=0; fi
AC_DEFINE_UNQUOTED(MN_FEATURE_SYMBOL_NAME([$1]), $_value, [Define to 1 if $1 is enabled, 0 otherwise])
])

dnl MN_FEATURES_DEFINE(FEATURES)
dnl
AC_DEFUN([MN_FEATURES_DEFINE], [m4_foreach(feature, [$1], [MN_FEATURE_DEFINE(feature)])])

### command line arguments ####################################################

dnl MN_ARG_ENABLE(FEATURE, DESCRIPTION, [DEFAULT])
dnl
AC_DEFUN([MN_ARG_ENABLE],
[AC_ARG_ENABLE([$1],
AS_HELP_STRING(--ifelse([$3], no, enable, disable)-[$1], $2),
[case "$enableval" in
	yes|no) MN_FEATURE_VAR_NAME([$1])=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-[$1]]) ;;
esac], [MN_FEATURE_VAR_NAME([$1])=ifelse([$3],, yes, [$3])])])

dnl MN_ARG_COMPILE_WARNINGS
dnl
AC_DEFUN([MN_ARG_COMPILE_WARNINGS],
[AC_ARG_ENABLE(compile-warnings,
AS_HELP_STRING([--enable-compile-warnings=no|yes|error],
[enable compiler warnings [[no]]]),
[case "$enableval" in
	yes|no|error) enable_compile_warnings=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-compile-warnings]) ;;
esac], [enable_compile_warnings=no])

if test $enable_compile_warnings = no; then
	WARN_CFLAGS=
else
	# most flags are specific to GCC
	WARN_CFLAGS="-Wall \
-Wformat-y2k \
-Wformat-security \
-Wno-unused-parameter \
-Wfloat-equal \
-Wdeclaration-after-statement \
-Wendif-labels \
-Wpointer-arith \
-Wcast-align \
-Waggregate-return \
-Wmissing-noreturn \
-Wmissing-format-attribute \
-Wpacked \
-Wredundant-decls \
-Wnested-externs \
-Winline \
-Wno-pointer-sign"
	test $enable_compile_warnings = error && WARN_CFLAGS="$WARN_CFLAGS -Werror"
fi

AC_SUBST(WARN_CFLAGS)])

dnl MN_ARG_DEBUG([DEFAULT])
dnl
AC_DEFUN([MN_ARG_DEBUG],
[AC_ARG_ENABLE(debug,
ifelse([$1], yes,
[AS_HELP_STRING([--disable-debug], [disable assertions and cast checks])],
[AS_HELP_STRING([--enable-debug], [enable assertions and cast checks])]),
[case "$enableval" in
	yes|no) enable_debug=$enableval ;;
	*) AC_MSG_ERROR([bad value $enableval for --enable-debug]) ;;
esac], [enable_debug=ifelse([$1],, no, [$1])])

if test $enable_debug = no; then
	DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
else
	DEBUG_CFLAGS=
fi

AC_SUBST(DEBUG_CFLAGS)])

### report ####################################################################

dnl MN_REPORT(NAME, VALUE)
dnl
AC_DEFUN([MN_REPORT], [  m4_format([%-30s %s], [$1], [$2])])

dnl MN_REPORT_ARG(NAME)
dnl
AC_DEFUN([MN_REPORT_ARG], [MN_REPORT([--[$1]], [$translit([$1], -, _)])])

dnl MN_REPORT_ARGS(NAMES)
dnl
AC_DEFUN([MN_REPORT_ARGS], [m4_foreach(name, [$1], [MN_REPORT_ARG(name)
])])

dnl MN_REPORT_FEATURE(FEATURE)
dnl
AC_DEFUN([MN_REPORT_FEATURE],
[MN_REPORT([--enable-[$1]],
[$MN_FEATURE_VAR_NAME([$1]) $MN_FEATURE_DISABLE_REASON_VAR_NAME([$1])])])

dnl MN_REPORT_FEATURES(FEATURES)
dnl
AC_DEFUN([MN_REPORT_FEATURES], [m4_foreach(feature, [$1], [MN_REPORT_FEATURE(feature)
])])
