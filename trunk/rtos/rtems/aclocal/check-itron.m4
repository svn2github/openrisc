dnl $Id: check-itron.m4,v 1.2 2001-09-27 11:59:11 chris Exp $
dnl
AC_DEFUN(RTEMS_CHECK_ITRON_API,
[dnl
AC_REQUIRE([RTEMS_CHECK_CPU])dnl
AC_REQUIRE([RTEMS_ENABLE_ITRON])dnl

AC_CACHE_CHECK([whether BSP supports libitron],
  rtems_cv_HAS_ITRON_API,
  [dnl
    case "$RTEMS_CPU" in
    unix*)
      rtems_cv_HAS_ITRON_API="no"
      ;;
    *)
      if test "${RTEMS_HAS_ITRON_API}" = "yes"; then
        rtems_cv_HAS_ITRON_API="yes";
      else
        rtems_cv_HAS_ITRON_API="disabled";
      fi
      ;;
    esac])
if test "$rtems_cv_HAS_ITRON_API" = "yes"; then
  HAS_ITRON_API="yes";
else
  HAS_ITRON_API="no";
fi
AC_SUBST(HAS_ITRON_API)dnl
])
