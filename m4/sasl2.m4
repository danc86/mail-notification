# $Id: sasl2.m4,v 1.2 2004/08/04 22:54:08 jylefort Exp $
#
# This file is part of Mail Notification.
#
# Copyright (c) 2004 Jean-Yves Lefort.
#
# As a special exception to the Mail Notification licensing terms,
# Jean-Yves Lefort gives unlimited permission to copy, distribute and
# modify this file.

dnl AM_PATH_SASL2([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl
AC_DEFUN([AM_PATH_SASL2],
[SASL_CFLAGS=""
SASL_LIBS="-lsasl2"

ac_save_CFLAGS="$CFLAGS"
ac_save_LIBS="$LIBS"
CFLAGS="$CFLAGS $SASL_CFLAGS"
LIBS="$LIBS $SASL_LIBS"

sasl2_min_version=ifelse([$1],, 2.0, [$1])

AC_MSG_CHECKING([for Cyrus SASL - version >= $sasl2_min_version])
AC_RUN_IFELSE([
#include <sasl/sasl.h>

int main() {
  int major, minor, step = 0, patch = 0;
  int sasl_major, sasl_minor, sasl_step, sasl_patch;

  if (sscanf("$sasl2_min_version", "%d.%d.%d.%d", &major, &minor, &step, &patch) < 2)
    exit(1); /* bad version string */

  sasl_version_info(0, 0, &sasl_major, &sasl_minor, &sasl_step, &sasl_patch);
  if ((sasl_major << 24) + (sasl_minor << 16) + (sasl_step << 8) + sasl_patch
      < (major << 24) + (minor << 16) + (step << 8) + patch)
    exit(2); /* version too old */

  exit(0);
}
], [found=yes], [found=no], [found=yes])
AC_MSG_RESULT($found)

CFLAGS="$ac_save_CFLAGS"
LIBS="$ac_save_LIBS"

if test $found = yes; then
	ifelse([$2],, :, [$2])
else
	SASL_CFLAGS=""
	SASL_LIBS=""
	ifelse([$3],, :, [$3])
fi

AC_SUBST(SASL_CFLAGS)
AC_SUBST(SASL_LIBS)])
