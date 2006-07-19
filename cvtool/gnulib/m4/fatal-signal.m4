# fatal-signal.m4 serial 3
dnl Copyright (C) 2003-2004 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FATAL_SIGNAL],
[
  AC_REQUIRE([gt_SIGNALBLOCKING])
  AC_REQUIRE([gt_TYPE_SIG_ATOMIC_T])
  AC_CHECK_HEADERS_ONCE(unistd.h)
  AC_CHECK_FUNCS(raise sigaction)
])
