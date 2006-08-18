dnl AM_LIBCVL([MINIMUM-VERSION-NUMBER, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]]])
dnl A simple wrapper for AC_LIB_HAVE_LINKFLAGS that checks for a minimum 
dnl version of libcvl. The version number must be given in hexadecimal format,
dnl for example 0x01020a for version 1.2.10.
dnl Searches for libcvl and the libraries that are implicit dependencies, 
dnl together with the necessary include files and the ability to compile and 
dnl link simple testcode. If found and the version is sufficient, it sets and 
dnl AC_SUBSTs HAVE_LIBCVL=yes and the LIBCVL and LTLIBCVL variables and augments
dnl the CPPFLAGS variable, and #defines HAVE_LIBCVL to 1. Otherwise, it sets
dnl and AC_SUBSTs HAVE_LIBCVL=no and LIBCVL and LTLIBCVL to empty.

AC_DEFUN([AM_LIBCVL],
[
  min_cvl_version="$1"
  AC_MSG_CHECKING([for libcvl - version >= $min_cvl_version])
  AC_LIB_HAVE_LINKFLAGS([cvl], [], [#include <cvl/cvl.h>],
    [
      #if (LIBCVL_VERSION_NUMBER < $min_cvl_version)
	libcvl version too old
      #else
        cvl_check_version(0);
      #endif
    ])
  if test "$HAVE_LIBCVL" = "yes"; then
    AC_MSG_RESULT(yes)
    ifelse([$2], , :, [$2])
  else
    AC_MSG_RESULT(no)
    ifelse([$3], , :, [$3])
  fi
])
