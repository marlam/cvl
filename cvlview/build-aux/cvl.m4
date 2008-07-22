dnl
dnl cvl.m4
dnl 
dnl This file is part of cvlview, an image viewer using the CVL library.
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

dnl AM_CVL(major, minor, patch) detects the CVL library. It uses 
dnl AC_LIB_HAVE_LINKFLAGS macro and thus
dnl - provides a --with-libglew-prefix option for the configure script
dnl - sets the HAVE_LIBGLEW and LIBGLEW/LTLIBGLEW variables
dnl - augments the CPPFLAGS variable. 
dnl If CVL is not found, this macro exits with an error message.
dnl The arguments major, minor, patch give the minimum required version of
dnl CVL.

AC_DEFUN([AM_CVL],
[
AC_LIB_HAVE_LINKFLAGS([cvl], [], 
	[#include <cvl/cvl.h>
	 #if CVL_VERSION_MAJOR < $1 || CVL_VERSION_MINOR < $2 || CVL_VERSION_PATCH < $3
	 CVL version too old
	 #endif], 
	[cvl_init();])
if test "$HAVE_LIBCVL" != "yes"; then
	AC_MSG_ERROR([could not find CVL >= $1.$2.$3])
fi
])
