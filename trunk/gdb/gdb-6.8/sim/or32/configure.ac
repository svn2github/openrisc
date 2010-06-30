dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)dnl
AC_INIT(Makefile.in)
AC_CONFIG_HEADER(config.h:config.in)

sinclude(../common/aclocal.m4)

# Invoke the autoconf macros most often used by the simulators
# README_HACKING says to use SIM_AC_COMMON, but that breaks in autoconf 2.59
sinclude(../common/common.m4)

# Option to explicitly specify the location of the Or1ksim installation
AC_ARG_WITH([or1ksim],
            [AS_HELP_STRING([--with-or1ksim=ARG],
              [specify the Or1ksim installation directory explicitly (otherwise use default search paths)])],
            [],
            [])
if test x"$with_or1ksim" != x
then
  SIM_CPPFLAGS="$SIM_CPPFLAGS -I$with_or1ksim/include"
  SIM_LDFLAGS="$SIM_LDFLAGS -Wl,-rpath -Wl,$with_or1ksim/lib -L$with_or1ksim/lib"
fi

# No target specific stuff as yet...
AC_CHECK_HEADERS(stdlib.h)

# Set up simulator output
AC_SUBST(SIM_CPPFLAGS)
AC_SUBST(SIM_LDFLAGS)

SIM_AC_OUTPUT
