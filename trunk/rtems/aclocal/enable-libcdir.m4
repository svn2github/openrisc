dnl $Id: enable-libcdir.m4,v 1.2 2001-09-27 11:59:11 chris Exp $

AC_DEFUN(RTEMS_ENABLE_LIBCDIR,
[
AC_ARG_ENABLE(libcdir,
[  --enable-libcdir=directory           set the directory for the C library],
[ RTEMS_LIBC_DIR="${enableval}" ; \
test -d ${enableval} || AC_MSG_ERROR("$enableval is not a directory" ) ] )
AC_SUBST(RTEMS_LIBC_DIR)dnl
])
