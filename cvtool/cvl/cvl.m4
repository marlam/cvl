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
  gl_ABSOLUTE_HEADER([cairo/cairo-features.h])
  CAIRO_INCLUDE_DIR=`echo $gl_cv_absolute_cairo_cairo_features_h \
  | sed -e 's/\/cairo-features.h$//' -e 's/"//g' -e 's/^\/\/\//\//'`
  CPPFLAGS="$CPPFLAGS -I$CAIRO_INCLUDE_DIR"
  AC_LIB_HAVE_LINKFLAGS([cvl], [], 
    [
      #include <cvl/cvl.h>
      #if (CVL_VERSION_NUMBER < $min_cvl_version)
	libcvl version too old
      #endif
    ], [cvl_check_version(0);])
  if test "$HAVE_LIBCVL" = "yes"; then
    ifelse([$2], , :, [$2])
  else
    ifelse([$3], , :, [$3])
  fi
])
