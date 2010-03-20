dnl
dnl qt.m4
dnl 
dnl This file is part of cvlview, an image viewer using the CVL library.
dnl
dnl Copyright (C) 2007, 2008, 2009, 2010  Martin Lambers <marlam@marlam.de>
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

dnl AM_QTGUI(major, minor, patch) detects the Qt GUI and core libraries, version
dnl major.minor.patch or greater. The major number must be at least 4; this
dnl macro does not work for older versions.
dnl It uses AC_LIB_HAVE_LINKFLAGS macro and thus
dnl - provides a --with-qt-prefix option for the configure script
dnl - sets the HAVE_LIBQTGUI and LIBQTGUI/LTLIBQTGUI variables
dnl - augments the CPPFLAGS variable.
dnl Test HAVE_LIBQTGUI to see if the Qt GUI and core libraries were found.
dnl This macros also detects the Qt tools rcc and moc. Use the variables
dnl $(RCC) and $(MOC) in Makefile.am.

AC_DEFUN([AM_QTGUI],
[
    AC_REQUIRE([AC_CANONICAL_SYSTEM])
    AC_LANG_PUSH([C++])
    RCC=""
    AC_ARG_VAR([RCC], [rcc command])
    AC_CHECK_TOOLS([RCC], [rcc-qt4 rcc])
    MOC=""
    AC_ARG_VAR([MOC], [moc command])
    AC_CHECK_TOOLS([MOC], [moc-qt4 moc])
    AC_CHECK_HEADER([qt4/Qt/qconfig.h], [HAVE_QTINC=yes], [HAVE_QTINC=no])
    if test "$HAVE_QTINC" = "yes"; then
        gl_ABSOLUTE_HEADER([qt4/Qt/qconfig.h])
        QT_INCLUDE_DIR=`echo $gl_cv_absolute_qt4_Qt_qconfig_h | sed -e 's/\/Qt\/qconfig.h$//' -e 's/^\/\/\//\//'`
    else
        AC_CHECK_HEADER([Qt/qconfig.h], [HAVE_QTINC=yes], [HAVE_QTINC=no])
        if test "$HAVE_QTINC" = "yes"; then
            gl_ABSOLUTE_HEADER([Qt/qconfig.h])
            QT_INCLUDE_DIR=`echo $gl_cv_absolute_Qt_qconfig_h | sed -e 's/\/Qt\/qconfig.h$//' -e 's/^\/\/\//\//'`
        fi
    fi
    if test "$RCC" != ":" -a "$MOC" != ":" -a "$HAVE_QTINC" = "yes"; then
        AC_LIB_APPENDTOVAR([CXXFLAGS], [-I$QT_INCLUDE_DIR])
        AC_LIB_APPENDTOVAR([CXXFLAGS], [-I$QT_INCLUDE_DIR/Qt])
        AC_LIB_APPENDTOVAR([CXXFLAGS], [-I$QT_INCLUDE_DIR/QtCore])
        AC_LIB_APPENDTOVAR([CXXFLAGS], [-I$QT_INCLUDE_DIR/QtGui])
        AC_LIB_FROMPACKAGE([QtGui], [qt])
        case "${target}" in
        *-*-mingw32*)
            AC_LIB_HAVE_LINKFLAGS([QtGui], [gdi32 comdlg32 oleaut32 imm32 winmm winspool QtCore kernel32 user32 shell32 uuid ole32 advapi32 ws2_32 png z],
                [#include <QApplication>
                 #if (QT_VERSION < QT_VERSION_CHECK($1, $2, $3))
                 # error Qt version too old
                 #endif],
                [QApplication::exec();])
            ;;
        *)
            AC_LIB_HAVE_LINKFLAGS([QtGui], [QtCore],
                [#include <QApplication>
                 #if (QT_VERSION < QT_VERSION_CHECK($1, $2, $3))
                 # error Qt version too old
                 #endif],
                [QApplication::exec();])
            ;;
        esac
    fi
    AC_LANG_POP([C++])
])

dnl AM_QTOPENGL(major, minor, patch) detects the Qt OpenGL libraries and their
dnl prerequisites.
dnl This macro calls AM_QTGUI(major, minor, patch); see above.
dnl It uses AC_LIB_HAVE_LINKFLAGS macro and thus
dnl - provides a --with-qt-prefix option for the configure script
dnl - sets the HAVE_LIBQTOPENGL and LIBQTOPENGL/LTLIBQTOPENGL variables
dnl - augments the CPPFLAGS variable.
dnl Test HAVE_LIBQTOPENGL to see if the Qt OpenGL libraries were found.
AC_DEFUN([AM_QTOPENGL],
[
    AM_QTGUI([$1], [$2], [$3])
    if test "$HAVE_LIBQTGUI" = "yes"; then
        AC_LANG_PUSH([C++])
        AC_LIB_APPENDTOVAR([CXXFLAGS], [-I$QT_INCLUDE_DIR/QtOpenGL])
        AC_LIB_FROMPACKAGE([QtOpenGL], [qt])
        case "${target}" in
        *-*-mingw32*)
            AC_LIB_HAVE_LINKFLAGS([QtOpenGL], [opengl32 glu32 QtGui],
                [#include <QGLWidget>], [QGLWidget::mouseGrabber();])
            ;;
        *)
            AC_LIB_HAVE_LINKFLAGS([QtOpenGL], [QtGui],
                [#include <QGLWidget>], [QGLWidget::mouseGrabber();])
            ;;
        esac
        AC_LANG_POP([C++])
    fi
])
