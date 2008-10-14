dnl
dnl glew.m4
dnl 
dnl This file is part of CVL, a computer vision library.
dnl
dnl Copyright (C) 2007, 2008  Martin Lambers <marlam@marlam.de>
dnl
dnl   This program is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 3 of the License, or
dnl   (at your option) any later version.
dnl
dnl   This program is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl
dnl   You should have received a copy of the GNU General Public License
dnl   along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl

dnl AM_GLEW detects the GLEW library. It uses AC_LIB_HAVE_LINKFLAGS macro and
dnl thus
dnl - provides a --with-libglew-prefix option for the configure script
dnl - sets the HAVE_LIBGLEW and LIBGLEW/LTLIBGLEW variables
dnl - augments the CPPFLAGS variable. 
dnl Test HAVE_LIBGLEW to see if libglew was found.

AC_DEFUN([AM_GLEW],
[
	AC_REQUIRE([AC_CANONICAL_SYSTEM])
	AC_LANG([C])
	case "${target}" in
	*-*-mingw32*)
		AC_DEFINE([GLEW_STATIC], [1], [Use static GLEW on W32.])
		AC_LIB_FROMPACKAGE([glew32s], [libglew])
		AC_LIB_HAVE_LINKFLAGS([glew32s], [glu32 opengl32], 
		  [#define GLEW_STATIC 1
		   #include <GL/glew.h>],
		  [GLEW_VERSION_2_1])
		if test "$HAVE_LIBGLEW32S" = "yes"; then
			HAVE_LIBGLEW=yes
			AC_SUBST([HAVE_LIBGLEW])
			LIBGLEW="$LIBGLEW32S"
			AC_SUBST([LIBGLEW])
			LTLIBGLEW="$LTLIBGLEW32S"
			AC_SUBST([LTLIBGLEW])
		fi
		;; 
	*) 
		AC_LIB_HAVE_LINKFLAGS([GLEW], [GLU GL], [#include <GL/glew.h>], [GLEW_VERSION_2_1])
		;; 
	esac
])
