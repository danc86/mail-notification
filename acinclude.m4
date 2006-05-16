# Copyright (C) 2003-2006 Jean-Yves Lefort.
#
# This file is part of Mail Notification.
# It may be distributed under the same terms as Mail Notification.

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
-Winline"
	test $enable_compile_warnings = error && WARN_CFLAGS="$WARN_CFLAGS -Werror"
fi

AC_SUBST(WARN_CFLAGS)])

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
