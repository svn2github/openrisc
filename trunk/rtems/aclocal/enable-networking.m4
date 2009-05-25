dnl $Id: enable-networking.m4,v 1.2 2001-09-27 11:59:11 chris Exp $

AC_DEFUN(RTEMS_ENABLE_NETWORKING,
[
## AC_BEFORE([$0], [RTEMS_CHECK_NETWORKING])dnl

AC_ARG_ENABLE(networking,
[  --enable-networking                  enable TCP/IP stack],
[case "${enableval}" in
  yes) RTEMS_HAS_NETWORKING=yes ;;
  no) RTEMS_HAS_NETWORKING=no ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-networking option) ;;
esac],[RTEMS_HAS_NETWORKING=yes])
AC_SUBST(RTEMS_HAS_NETWORKING)dnl
])
