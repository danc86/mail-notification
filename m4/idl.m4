# This file is part of Mail Notification.
#
# Copyright (C) 2005 Jean-Yves Lefort.
#
# As a special exception to the Mail Notification licensing terms,
# Jean-Yves Lefort gives unlimited permission to copy, distribute and
# modify this file.

dnl AM_PATH_ORBIT_IDL([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl
AC_DEFUN([AM_PATH_ORBIT_IDL],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])

AC_MSG_CHECKING([for orbit-idl-2])

if test -n "$PKG_CONFIG"; then
	ORBIT_IDL="`$PKG_CONFIG --variable=orbit_idl ORBit-2.0 2>/dev/null`"
else
	ORBIT_IDL=""
fi

if test -n "$ORBIT_IDL"; then
	AC_MSG_RESULT([$ORBIT_IDL])
	ifelse([$1],, :, [$1])
else
	AC_MSG_RESULT([not found])
	ifelse([$2],, :, [$2])
fi

AC_SUBST(ORBIT_IDL)])

dnl IDL_CHECK_MODULES(VARIABLE-PREFIX, MODULES, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl
AC_DEFUN([IDL_CHECK_MODULES],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])

AC_MSG_CHECKING([for [$1] IDL])
PKG_CHECK_EXISTS([$2], found=yes, found=no)
AC_MSG_RESULT([$found])

$1_IDLFLAGS=""
if test $found = yes; then
	for module in $2; do
		idldir="`$PKG_CONFIG --variable=idldir $module 2>/dev/null`"
		if test -n "$idldir"; then
			$1_IDLFLAGS="$[$1]_IDLFLAGS -I$idldir"
		fi
	done
	ifelse([$3],, :, [$3])
else
	ifelse([$4],, :, [$4])
fi

AC_SUBST($1_IDLFLAGS)])
