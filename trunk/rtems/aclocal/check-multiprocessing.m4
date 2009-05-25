dnl
dnl $Id: check-multiprocessing.m4,v 1.2 2001-09-27 11:59:11 chris Exp $
dnl

AC_DEFUN(RTEMS_CHECK_MULTIPROCESSING,
[dnl
AC_REQUIRE([RTEMS_ENV_RTEMSBSP])dnl
AC_REQUIRE([RTEMS_TOP])dnl
AC_REQUIRE([RTEMS_CHECK_CPU])dnl
AC_REQUIRE([RTEMS_ENABLE_MULTIPROCESSING])dnl

AC_CACHE_CHECK([whether BSP supports multiprocessing],
  rtems_cv_HAS_MP,
  [dnl
    RTEMS_BSP_ALIAS(${$1},bspdir) 
    if test -d "$srcdir/${RTEMS_TOPdir}/c/src/lib/libbsp/${RTEMS_CPU}/${bspdir}/shmsupp"; then
      if test "$RTEMS_HAS_MULTIPROCESSING" = "yes"; then
        rtems_cv_HAS_MP="yes" ;
      else
        rtems_cv_HAS_MP="disabled";
      fi
    else
      rtems_cv_HAS_MP="no";
    fi])
if test "$rtems_cv_HAS_MP" = "yes"; then
HAS_MP="yes"
else
HAS_MP="no"
fi
AC_SUBST(HAS_MP)
])
