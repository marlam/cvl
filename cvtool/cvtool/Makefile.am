bin_PROGRAMS = cvtool

cvtool_SOURCES = cvtool.c crash.c crash.h options.c options.h \
	cmd_affine.c		\
	cmd_binarize.c		\
	cmd_blend.c		\
	cmd_channel.c		\
	cmd_color.c		\
	cmd_combine.c		\
	cmd_convert.c		\
	cmd_convolve.c		\
	cmd_create.c		\
	cmd_cut.c		\
	cmd_dibr.c		\
	cmd_diff.c		\
	cmd_draw.c		\
	cmd_edge.c		\
	cmd_equalize.c		\
	cmd_filter.c		\
	cmd_flip.c		\
	cmd_flop.c		\
	cmd_foreach.c		\
	cmd_info.c		\
	cmd_invert.c		\
	cmd_layer.c		\
	cmd_merge.c		\
	cmd_opticalflow.c	\
	cmd_resize.c		\
	cmd_reverse.c		\
	cmd_rotate.c		\
	cmd_scale.c		\
	cmd_sedt.c		\
	cmd_select.c		\
	cmd_shear.c		\
	cmd_split.c		\
	cmd_stereoview.c	\
	cmd_trackdepth.c	\
	cmd_vectors.c

cvtool_LDADD = \
	$(top_builddir)/cvl/libcvl.la \
	$(top_builddir)/gnulib/libgnu.la \
	$(top_builddir)/notgnulib/libnotgnu.la \
	$(GSL_LIBS) \
	$(CAIRO_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/gnulib -I$(top_srcdir)/notgnulib -I$(top_srcdir)/cvl
AM_CFLAGS = -DPROGRAM_NAME=\"cvtool\" $(GSL_CFLAGS) $(CAIRO_CFLAGS)
