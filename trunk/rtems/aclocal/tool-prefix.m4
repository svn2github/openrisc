dnl
dnl  $Id: tool-prefix.m4,v 1.2 2001-09-27 11:59:11 chris Exp $
dnl 
dnl Set program_prefix
dnl
dnl 98/05/20 Ralf Corsepius	(corsepiu@faw.uni-ulm.de)
dnl				Extracted from configure

AC_DEFUN(RTEMS_TOOL_PREFIX,
[AC_REQUIRE([AC_CANONICAL_TARGET])dnl
AC_REQUIRE([AC_CANONICAL_BUILD])dnl

changequote(,)dnl
if [ "${program_prefix}" = "NONE" ] ; then
  if [ "${target}" = "${host}" ] ; then
    program_prefix=
  else
    program_prefix=${target}-
  fi
fi
changequote([,])dnl
])
