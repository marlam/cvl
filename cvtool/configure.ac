dnl configure.ac
dnl Process this file with autoconf to produce a configure script.
#
# This file is part of cvtool, a computer vision tool.
#
# Copyright (C) 2005, 2006, 2007  Martin Lambers <marlam@marlam.de>
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
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

AC_INIT([cvtool], [0.2.3], [marlam@marlam.de])
AC_CONFIG_SRCDIR([cvtool/cvtool.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])

AC_GNU_SOURCE
AC_PROG_CC
gl_EARLY
AC_PROG_CC_C99
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_SYS_LARGEFILE

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
AC_SUBST([LT_CURRENT], [6])
AC_SUBST([LT_REVISION], [0])
AC_SUBST([LT_AGE], [0])

dnl gnulib 
gl_INIT

dnl mhlib
AC_CHECK_FUNCS([backtrace sigaction])
AC_SEARCH_LIBS([clock_gettime], [rt])

dnl Math library
AC_SEARCH_LIBS([sqrtf], [m])

dnl GLEW, OpenGL (used only by CVL)
GL_LIBS=""
if test "$w32" = "yes"; then
	AC_DEFINE([GLEW_STATIC], [1], [Use static GLEW on W32.])
	AC_LIB_HAVE_LINKFLAGS([glew32s], [gdi32 glu32 opengl32], 
	  [#define GLEW_STATIC 1
	   #include <GL/glew.h>],
	  [GLEW_VERSION_2_1])
	if test "$HAVE_LIBGLEW32S" != "yes"; then
		AC_MSG_ERROR([could not find GLEW >= 1.3.5])
	else
		GL_LIBS="$LIBGLEW32S"
	fi
else
	AC_LIB_HAVE_LINKFLAGS([GLEW], [GLU GL], [#include <GL/glew.h>], [GLEW_VERSION_2_1])
	if test "$HAVE_LIBGLEW" != "yes"; then
		AC_MSG_ERROR([could not find GLEW >= 1.3.5])
	else
		GL_LIBS="$LIBGLEW"
	fi
fi
AC_SUBST([GL_LIBS])

dnl CAIRO (used only by the optional cvtool draw command)
AC_LIB_HAVE_LINKFLAGS([cairo], [], 
  [#include <cairo/cairo-features.h>
   #if CAIRO_VERSION_MAJOR < 1 || CAIRO_VERSION_MINOR < 2
   CAIRO version too old
   #endif],
  [char *s = CAIRO_VERSION_STRING;])
if test "$HAVE_LIBCAIRO" = "yes"; then
	CAIRO_LIBS="$LIBCAIRO"
	gl_ABSOLUTE_HEADER([cairo/cairo-features.h])
	CAIRO_INCLUDE_DIR=`echo $gl_cv_absolute_cairo_cairo_features_h \
	| sed -e 's/\/cairo-features.h$//' -e 's/"//g' -e 's/^\/\/\//\//'`
	CAIRO_CPPFLAGS="-I$CAIRO_INCLUDE_DIR"
fi
AC_SUBST([CAIRO_LIBS])
AC_SUBST([CAIRO_CPPFLAGS])

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
AC_CONFIG_FILES([Makefile build-aux/Makefile gnulib/Makefile \
	mhlib/Makefile cvl/Makefile cvl/cvl/cvl_version.h cvtool/Makefile \
	doc/Makefile doc/doxy/Makefile tests/Makefile])
AC_OUTPUT
