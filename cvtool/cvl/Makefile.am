lib_LTLIBRARIES = libcvl.la

EXTRA_DIST = \
	cvl/cvl_version.h.in \
	cvl_assert.c \
	cvl_msg.c \
	cvl_math.c \
	cvl_getopt.c \
	cvl_pixel.c \
	cvl_vector.c \
	cvl_field.c \
	cvl_frame.c \
	cvl_io.c \
	cvl_color.c \
	cvl_cairo.c \
	cvl_convolve.c \
	cvl_filter.c \
	cvl_edge.c \
	cvl_transform.c \
	cvl_mix.c \
	cvl_stereo.c \
	cvl_opticalflow.c \
	cvl_misc.c

libcvl_la_SOURCES = cvl.c
libcvl_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
libcvl_la_LIBADD = $(top_builddir)/notgnulib/libnotgnu.la $(top_builddir)/gnulib/libgnu.la
nobase_include_HEADERS = \
	cvl/cvl_version.h \
	cvl/cvl_msg.h \
	cvl/cvl_assert.h \
	cvl/cvl_math.h \
	cvl/cvl_getopt.h \
	cvl/cvl_pixel.h \
	cvl/cvl_vector.h \
	cvl/cvl_field.h \
	cvl/cvl_frame.h \
	cvl/cvl_io.h \
	cvl/cvl_color.h \
	cvl/cvl_cairo.h \
	cvl/cvl_convolve.h \
	cvl/cvl_filter.h \
	cvl/cvl_edge.h \
	cvl/cvl_transform.h \
	cvl/cvl_mix.h \
	cvl/cvl_stereo.h \
	cvl/cvl_opticalflow.h \
	cvl/cvl_misc.h \
	cvl/cvl.h

AM_CPPFLAGS = -I$(top_srcdir)/gnulib -I$(top_srcdir)/notgnulib \
	-DLIBRARY_NAME=\"CVL\" -DCVL_DEBUG=$(CVL_DEBUG) -DGSL_RANGE_CHECK_OFF

m4datadir = $(datadir)/aclocal
dist_m4data_DATA = cvl.m4
