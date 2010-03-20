dnl configure.ac
dnl Process this file with autoconf to produce a configure script.
#
# This file is part of cvtool, a computer vision tool.
#
# Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010
# Martin Lambers <marlam@marlam.de>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

dnl Autotools init stuff
AC_PREREQ([2.65])
AC_INIT([cvtool], [1.0.0], [marlam@marlam.de])
AC_CONFIG_SRCDIR([cvtool/cvtool.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_TARGET
AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE
AM_INIT_AUTOMAKE([1.11.1 color-tests silent-rules -Wall -Wno-portability])
AM_SILENT_RULES([yes])
LT_PREREQ([2.2.6])
LT_INIT([disable-static])
AC_PROG_CC
AC_PROG_CC_C99
gl_EARLY
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_LANG([C])
AC_C_BIGENDIAN

dnl System
case "${target}" in *-*-mingw32*) w32=yes ;; *) w32=no ;; esac

dnl CVL version preprocessor macros for cvl/cvl/cvl_version.h.in
AC_SUBST([CVL_VERSION], [$PACKAGE_VERSION])
AC_SUBST([CVL_VERSION_MAJOR], [`echo $CVL_VERSION | sed -e 's/\(.*\)\..*\..*/\1/'`])
AC_SUBST([CVL_VERSION_MINOR], [`echo $CVL_VERSION | sed -e 's/.*\.\(.*\)\..*/\1/'`])
AC_SUBST([CVL_VERSION_PATCH], [`echo $CVL_VERSION | sed -e 's/.*\..*\.\(.*\)/\1/' -e 's/[[^0-9]].*//'`])
AC_SUBST([CVL_VERSION_NUMBER], [`printf "0x%02x%02x%02x" $CVL_VERSION_MAJOR $CVL_VERSION_MINOR $CVL_VERSION_PATCH`])

dnl CVL library version
dnl Library code modified:		REVISION++
dnl Interfaces changed/added/removed:   CURRENT++	REVISION=0
dnl Interfaces added: 			AGE++
dnl Interfaces removed:			AGE=0
AC_SUBST([LT_CURRENT], [10])
AC_SUBST([LT_REVISION], [0])
AC_SUBST([LT_AGE], [0])

dnl gnulib 
gl_INIT

dnl mhlib
AC_CHECK_FUNCS([backtrace sigaction])
AC_SEARCH_LIBS([clock_gettime], [rt])

dnl Math library
AC_SEARCH_LIBS([sqrtf], [m])

dnl GLEW (used only by CVL)
AM_GLEW()

dnl CAIRO (used only by the optional cvtool draw command)
PKG_CHECK_MODULES([CAIRO], [cairo],
	[have_cairo=1],
	[have_cairo=0
	 AC_MSG_WARN([Disabled the draw command because libcairo was not found.])])
AC_DEFINE_UNQUOTED([HAVE_LIBCAIRO], [$have_cairo], [Do we have libcairo?])

dnl Symbol visibility
AC_MSG_CHECKING([for visibility support])
CFLAGS_BAK="$CFLAGS"
dnl gcc may only issue a warning if the visibility functionality is not
dnl available in its configuration. -Werror causes it to fail so that we
dnl notice this condition.
dnl See http://lists.gnu.org/archive/html/bug-gnulib/2010-01/msg00162.html 
CFLAGS="$CFLAGS -fvisibility=hidden -Werror"
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
    [[extern __attribute__((visibility("default"))) int a;
      extern __attribute__((visibility("default"))) int b(void);]], [[]])],
    [RESULT="yes"], [RESULT="no"])
CFLAGS="$CFLAGS_BAK"
AC_MSG_RESULT([$RESULT])
if test "$RESULT" = "yes"; then
    CFLAG_VISIBILITY_HIDDEN="-fvisibility=hidden"
    AC_DEFINE([HAVE_VISIBILITY], [1], [Define to 1 if the compiler has visibility support.])
else
    CFLAG_VISIBILITY_HIDDEN=""
fi
AC_SUBST([CFLAG_VISIBILITY_HIDDEN])

dnl Global #defines for all source files
AH_VERBATIM([UNUSED],
[/* Let gcc know about unused variables to suppress warnings.
   Disable this feature for other compilers. */
#ifndef __attribute__
# if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 8)
#  define __attribute__(x)
# else
#  define __attribute__(x) __attribute__(x)
# endif
#endif
#ifndef UNUSED
# define UNUSED __attribute__ ((__unused__))
#endif])
AH_VERBATIM([W32_NATIVE],
[/* Define to 1 if the native W32 API should be used. */
#if (defined _WIN32 || defined __WIN32__) && !defined __CYGWIN__
#define W32_NATIVE 1
#endif])

dnl Ouput
AC_CONFIG_FILES([Makefile gnulib/Makefile mhlib/Makefile \
	cvl/Makefile cvl/cvl/cvl_version.h cvtool/Makefile \
	doc/Makefile doc/doxy/Makefile doc/doxy/doxyfile tests/Makefile])
AC_OUTPUT
