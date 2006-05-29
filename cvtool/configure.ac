dnl configure.ac
dnl Process this file with autoconf to produce a configure script.
#
# This file is part of cvl, a computer vision library.
#
# Copyright (C) 2005, 2006  Martin Lambers <marlam@marlam.de>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software Foundation,
#   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#

AC_INIT(cvtool, 0.0.3, marlam@marlam.de)
AC_CONFIG_SRCDIR(cvtool/cvtool.c)
AC_CONFIG_AUX_DIR(build-aux)
AC_CONFIG_MACRO_DIR(build-aux)

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_SYS_LARGEFILE
gl_EARLY

dnl cvl library version
dnl Library code modified:		REVISION++
dnl Interfaces changed/added/removed:   CURRENT++	REVISION=0
dnl Interfaces added: 			AGE++
dnl Interfaces removed:			AGE=0
AC_SUBST(LT_CURRENT, 1)
AC_SUBST(LT_REVISION, 0)
AC_SUBST(LT_AGE, 1)

dnl configure switches
AC_ARG_ENABLE(cvl-debug, 
  AC_HELP_STRING([--enable-cvl-debug], [Enable (costly) checks in CVL, for debugging purposes.]),
  CVL_DEBUG=1, CVL_DEBUG=0)
AC_SUBST(CVL_DEBUG)

dnl gnulib 
gl_INIT

dnl notgnulib
ngl_INIT

dnl optional features
AC_CHECK_FUNCS(backtrace)

dnl pkg-config
PKG_PROG_PKG_CONFIG

dnl GNU Scientific Library
AM_PATH_GSL([1.6.0], [], [AC_MSG_ERROR(could not find required version of GSL)])

dnl CAIRO
PKG_CHECK_MODULES(CAIRO, cairo >= 1.0.2, [], [AC_MSG_ERROR(could not find required version of CAIRO)])

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

CVL_LIBS="-L${libdir} -lcvl $GSL_LIBS $CAIRO_LIBS $LIBS"
CVL_CFLAGS="-I${includedir} $GSL_CFLAGS $CAIRO_CFLAGS"
AC_SUBST(CVL_LIBS)
AC_SUBST(CVL_CFLAGS)

AC_CONFIG_COMMANDS([chmod-config],[[chmod +x cvl/cvl-config]],[[]])

AC_CONFIG_FILES(Makefile build-aux/Makefile gnulib/Makefile notgnulib/Makefile \
	cvl/Makefile cvl/cvl-config cvl/cvl.pc cvl/cvl/cvl_version.h cvtool/Makefile \
	doc/Makefile doc/doxy/Makefile tests/Makefile)
AC_OUTPUT
