dnl $Id: enable-hwapi.m4,v 1.2 2001-09-27 11:59:11 chris Exp $
dnl
dnl FIXME: this needs to be reworked

AC_DEFUN(RTEMS_ENABLE_HWAPI,
[dnl
AC_ARG_ENABLE(hwapi, \
[  --enable-hwapi                       enable hardware API library],
[case "${enableval}" in
    yes) RTEMS_HAS_HWAPI=yes ;;
    no)  RTEMS_HAS_HWAPI=no ;;
    *)  AC_MSG_ERROR(bad value ${enableval} for hwapi option) ;;
  esac],[RTEMS_HAS_HWAPI=no])
AC_SUBST(RTEMS_HAS_HWAPI)dnl
])dnl
