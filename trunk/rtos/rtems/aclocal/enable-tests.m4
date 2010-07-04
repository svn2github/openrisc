dnl $Id: enable-tests.m4,v 1.2 2001-09-27 11:59:11 chris Exp $

AC_DEFUN(RTEMS_ENABLE_TESTS,
[
# If the tests are enabled, then find all the test suite Makefiles
AC_MSG_CHECKING([if the test suites are enabled? ])
AC_ARG_ENABLE(tests,
[  --enable-tests                       enable tests (default:disabled)],
  [case "${enableval}" in
    yes) tests_enabled=yes ;;
    no)  tests_enabled=no ;;
    *)   AC_MSG_ERROR(bad value ${enableval} for tests option) ;;
  esac], [tests_enabled=no])
AC_MSG_RESULT([$tests_enabled])
])
