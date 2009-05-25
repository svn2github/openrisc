dnl $Id: path-perl.m4,v 1.2 2001-09-27 11:59:11 chris Exp $

AC_DEFUN(RTEMS_PATH_PERL,
[
AC_PATH_PROG(PERL,perl)
if test -z "$PERL" ; then
AC_MSG_WARN(
[***]
[   perl was not found]
[   Note: Some tools will not be built.])
fi
])
