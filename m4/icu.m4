# This file is part of Mail Notification.
#
# Copyright (C) 2005 Jean-Yves Lefort.
#
# As a special exception to the Mail Notification licensing terms,
# Jean-Yves Lefort gives unlimited permission to copy, distribute and
# modify this file.

dnl AM_PATH_ICU([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl
AC_DEFUN([AM_PATH_ICU],
[AC_PATH_PROG(ICU_CONFIG, icu-config, no)

if test "x$ICU_CONFIG" = xno; then
	ICU_CFLAGS=""
	ICU_LIBS=""

	ifelse([$3],, :, [$3])
else
	ICU_CFLAGS=`$ICU_CONFIG --cppflags`
	ICU_LIBS=`$ICU_CONFIG --ldflags`

	# the version checking API we need has been introduced in 2.4
	icu_min_version=ifelse([$1],, 2.4, [$1])

	ac_save_CFLAGS="$CFLAGS"
	ac_save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $ICU_CFLAGS"
	LIBS="$LIBS $ICU_LIBS"

	AC_MSG_CHECKING([for ICU - version >= $icu_min_version])

	AC_RUN_IFELSE([
#include <string.h>
#include <unicode/uversion.h>

int main() {
  UVersionInfo min_version;
  UVersionInfo version;

  u_versionFromString(min_version, "$icu_min_version");
  u_getVersion(version);

  if (memcmp(min_version, version, U_MAX_VERSION_LENGTH) > 0)
    exit(1); /* version too old */

  exit(0);
}
], [found=yes], [found=no], [found=yes])

	AC_MSG_RESULT($found)

	CFLAGS="$ac_save_CFLAGS"
	LIBS="$ac_save_LIBS"

	if test $found = yes; then
		ifelse([$2],, :, [$2])
	else
		ICU_CFLAGS=""
		ICU_LIBS=""

		ifelse([$3],, :, [$3])
	fi
fi

AC_SUBST(ICU_CFLAGS)
AC_SUBST(ICU_LIBS)])
