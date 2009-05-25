dnl $Id: canonical-host.m4,v 1.2 2001-09-27 11:59:11 chris Exp $

AC_DEFUN(RTEMS_CANONICAL_HOST,
[dnl
AC_REQUIRE([AC_CANONICAL_HOST])
RTEMS_HOST=$host_os
changequote(,)dnl
case "${target}" in
  # hpux unix port should go here
  i[34567]86-*linux*)          # unix "simulator" port
        RTEMS_HOST=Linux
	;;
  i[34567]86-*freebsd*)         # unix "simulator" port
        RTEMS_HOST=FreeBSD
	;;
  i[34567]86-pc-cygwin*) 	# Cygwin is just enough unix like :)
        RTEMS_HOST=Cygwin
	;;
  sparc-sun-solaris*)           # unix "simulator" port
        RTEMS_HOST=Solaris
	;;
  *) 
	;;
esac
changequote([,])dnl
AC_SUBST(RTEMS_HOST)
])dnl
