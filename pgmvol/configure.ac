dnl configure.ac
dnl Process this file with autoconf to produce a configure script.

AC_INIT([pgmvol], [0.2], [marlam@marlam.de])
AC_CONFIG_SRCDIR([src/pgmtovol.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])

AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_CC_C99

AM_LIBCVL([0x000004], [], [AC_MSG_ERROR(could not find required version of CVL)])
LIBS="$LIBS $LIBCVL"

AH_VERBATIM([W32_NATIVE],
[/* Define to 1 if the native W32 API should be used. */
#if (defined _WIN32 || defined __WIN32__) && !defined __CYGWIN__
#define W32_NATIVE 1
#endif])

AC_CONFIG_FILES([Makefile build-aux/Makefile doc/Makefile src/Makefile])
AC_OUTPUT
