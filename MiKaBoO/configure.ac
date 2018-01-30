#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([MiKaBoO], [0.1], [francesco.fornari@studio.unibo.it, gabriele.fulgaro@studio.unibo.it, davide.paselli2@studio.unibo.it, mattia.polverini@studio.unibo.it])
AM_INIT_AUTOMAKE([foreign])
export CC= arm-none-eabi-gcc
AC_PROG_CC([])
AM_PROG_AR
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_CHECK_PROG([HAVE_UARM], [uarm], [yes], [no])
AC_CHECK_PROG([HAVE_EABI], [arm-none-eabi-gcc], [yes], [no])
AC_CHECK_PROG([HAVE_EABI_LD], [arm-none-eabi-ld], [yes], [no])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h], [AC_DEFINE([HAVE_STD_H], [1], [set variable HAVE_STD_H to 1 if found])], [AC_DEFINE([HAVE_STD_H], [0], [set variable HAVE_STD_H to 0 if not found])])
AC_CHECK_FILES([src/mikabooq.h src/listx.h src/const.h], [AC_DEFINE([H_COMPLETE], [1], [set variable H_COMPLETE to 1 if found])], [AC_DEFINE([H_COMPLETE], [0], [set variable H_COMPLETE to 0 if not found])])
AC_CHECK_FILES([src/p1test.c src/mikabooq.c], [AC_DEFINE([C_COMPLETE], [1], [set variable C_COMPLETE to 1 if found])], [AC_DEFINE([C_COMPLETE], [0], [set variable C_COMPLETE to 0 if not found])])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT([src/Makefile])
