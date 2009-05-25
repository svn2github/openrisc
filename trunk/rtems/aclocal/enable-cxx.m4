dnl $Id: enable-cxx.m4,v 1.2 2001-09-27 11:59:11 chris Exp $

AC_DEFUN(RTEMS_ENABLE_CXX,
[
AC_ARG_ENABLE(cxx,
[  --enable-cxx                         enable C++ support,]
[                                       and build the rtems++ library],
[case "${enableval}" in
  yes) RTEMS_HAS_CPLUSPLUS=yes ;;
  no) RTEMS_HAS_CPLUSPLUS=no   ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-cxx option) ;;
esac], [RTEMS_HAS_CPLUSPLUS=no])
])
