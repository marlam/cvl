dnl
dnl qt4.m4
dnl 
dnl This file is part of cvlview, an image viewer using the CVL library.
dnl
dnl Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
dnl
dnl   This program is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
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

dnl Written by Martin Lambers <marlam@marlam.de> based on information from 
dnl <http://qtnode.net/wiki/Qt_with_autotools> as seen on 2006-07-31.

dnl AM_QT4 detects Qt4 and fills in the variables
dnl QT4_RCC, QT4_MOC, QT4_CXXFLAGS, and QT4_LIBS.
dnl It aborts on errors (Qt4 not found or not working properly).
dnl It should work with cross-compiles.
dnl You must call AC_PROG_CXX and AC_CANONICAL_SYSTEM before AM_QT4.
dnl Put the following rules into your Makefile.am:
dnl moc_%.cpp: %.h
dnl 	$(QT4_MOC) -o $@ $<
dnl rcc_%.cpp: %.qrc
dnl 	$(QT4_RCC) -o $@ $<

AC_DEFUN([AM_QT4],
[
AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([AC_CANONICAL_SYSTEM])

AC_MSG_CHECKING([for Qt4 prefix])
AC_ARG_WITH([qt4-prefix], 
	AC_HELP_STRING([--with-qt4-prefix=PFX], [Prefix where Qt4 is installed (optional)]),
	[QT4PFX="$withval"],
	[QT4PFX=""])
QT4TMPPFX=""
for x in "$QT4PFX" "/usr/local" "/usr"; do
	if test -z "$QT4TMPPFX"; then
		for i in "include" "include/qt4"; do
			if test -d "$x/$i/QtCore"; then
				QT4TMPPFX="$x"
				QT4INCDIR="$x/$i"
			fi
		done
	fi
done
if test -z "$QT4TMPPFX"; then
	AC_MSG_RESULT([not found])
	AC_MSG_ERROR([Qt4 not found])
elif test -n "$QT4PFX" -a "$QT4TMPPFX" != "$QT4PFX"; then
	AC_MSG_RESULT([not found])
	AC_MSG_ERROR([Qt4 not found])
else
	QT4PFX="$QT4TMPPFX"
	AC_MSG_RESULT([$QT4PFX])
fi
QT4_BIN="$QT4PFX/bin"
dnl Set preprocessor and linker flags.
case "${target}" in *-*-mingw32*) windows=yes ;; *) windows=no ;; esac
if test "$windows" = "yes"; then
	QT4_LIBS="-lQtOpenGL4 -lQtGui4 -lQtCore4"
	QT4_LIBS="-L$QT4PFX/lib -mthreads -Wl,-enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc -Wl,-s -Wl,-subsystem,windows -lopengl32 -lglu32 -lgdi32 -luser32 -lmingw32 -lqtmain $QT4_LIBS"
	QT4_CXXFLAGS="-I$QT4INCDIR -I$QT4INCDIR/QtCore -I$QT4INCDIR/QtGui -I$QT4INCDIR/QtOpenGL -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_DLL -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_THREAD_SUPPORT -DQT_NEEDS_QMAIN -frtti -fexceptions"
else
	AC_PATH_XTRA
	QT4_LIBS="-lQtGui -lQtOpenGL -lQtCore"
	QT4_LIBS="-Wl,-rpath,$QT4PFX/lib -L$QT4PFX/lib $QT4_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS -lGLU -lGL -lpthread"
	QT4_CXXFLAGS="-I$QT4INCDIR -I$QT4INCDIR/QtGui -I$QT4INCDIR/QtCore -I$QT4INCDIR/QtOpenGL $X_CFLAGS -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED"
fi
dnl Minimal set of files to test compiling and linking of a Qt application.
cat > qt4conftest.h << EOF
#include <QObject>
class Test: public QObject
{
	Q_OBJECT
	public:
		Test() {}
		~Test() {}
	public slots:
		void receive() {}
	signals:
		void send();
};
EOF
cat > qt4conftest.cpp << EOF
#include "qt4conftest.h"
#include <QApplication>
int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	Test t;
	QObject::connect(&t, SIGNAL(send()), &t, SLOT(receive()));
}
EOF
cat > qt4conftest.qrc << EOF
<!DOCTYPE RCC><RCC version="1.0">
<qresource>
	<file>qt4conftest.cpp</file>
</qresource>
</RCC>
EOF
dnl Check for rcc
AC_ARG_VAR([QT4_RCC], [Qt4 rcc command])
AC_CHECK_TOOL([QT4_RCC], [rcc-qt4], [:], [$QT4_BIN:$PATH])
if test "$QT4_RCC" = ":"; then
	unset QT4_RCC
	AC_CHECK_TOOL([QT4_RCC], [rcc], [:], [$QT4_BIN:$PATH])
	if test "$QT4_RCC" = ":"; then
		AC_MSG_RESULT([no])
		AC_MSG_ERROR([Qt4 not fully functional])
	fi
fi
AC_MSG_CHECKING([whether $QT4_RCC works])
AC_RUN_LOG(["$QT4_RCC" -o rcc_qt4conftest.cpp qt4conftest.qrc])
if test $ac_status -eq 0; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([Qt4 not fully functional])
fi
dnl Check whether rcc output can be compiled
AC_MSG_CHECKING([whether $QT4_RCC output can be compiled])
AC_RUN_LOG(["$CXX" -c $CXXFLAGS $QT4_CXXFLAGS -o rcc_qt4conftest.o rcc_qt4conftest.cpp])
if test $ac_status -eq 0; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([Qt4 not fully functional])
fi
dnl Check for moc
AC_ARG_VAR([QT4_MOC], [Qt4 moc command])
AC_CHECK_TOOL([QT4_MOC], [moc-qt4], [:], [$QT4_BIN:$PATH])
if test "$QT4_MOC" = ":"; then
	unset QT4_MOC
	AC_CHECK_TOOL([QT4_MOC], [moc], [:], [$QT4_BIN:$PATH])
	if test "$QT4_MOC" = ":"; then
		AC_MSG_RESULT([no])
		AC_MSG_ERROR([Qt4 not fully functional])
	fi
fi
AC_MSG_CHECKING([whether $QT4_MOC works])
AC_RUN_LOG(["$QT4_MOC" -o moc_qt4conftest.cpp qt4conftest.h])
if test $ac_status -eq 0; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([Qt4 not fully functional])
fi
dnl Check whether moc output can be compiled
AC_MSG_CHECKING([whether $QT4_MOC output can be compiled])
AC_RUN_LOG(["$CXX" -c $CXXFLAGS $QT4_CXXFLAGS -o moc_qt4conftest.o moc_qt4conftest.cpp])
if test $ac_status -eq 0; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([Qt4 not fully functional])
fi
dnl Check whether test app can be compiled
AC_MSG_CHECKING([whether a Qt4 application can be compiled])
AC_RUN_LOG(["$CXX" -c $CXXFLAGS $QT4_CXXFLAGS -o qt4conftest.o qt4conftest.cpp])
if test $ac_status -eq 0; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([Qt4 not fully functional])
fi
dnl Check whether test app can be linked
AC_MSG_CHECKING([whether linking with Qt4 works])
AC_RUN_LOG(["$CXX" -o qt4conftest qt4conftest.o moc_qt4conftest.o rcc_qt4conftest.o $QT4_LIBS $LIBS])
if test $ac_status -eq 0; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([Qt4 not fully functional])
fi
dnl Cleanup
rm -f qt4conftest.h qt4conftest.cpp qt4conftest.qrc \
	rcc_qt4conftest.cpp moc_qt4conftest.cpp \
	rcc_qt4conftest.o moc_qt4conftest.o qt4conftest.o \
	qt4conftest
dnl Output
AC_SUBST([QT4_CXXFLAGS])
AC_SUBST([QT4_LIBS])
])
