bin_PROGRAMS = cvtool

cvtool_SOURCES = cvtool.c 	\
	cmd_affine.c		\
	cmd_blend.c		\
	cmd_channelextract.c	\
	cmd_channelcombine.c	\
	cmd_color.c		\
	cmd_combine.c		\
	cmd_convert.c		\
	cmd_convolve.c		\
	cmd_create.c		\
	cmd_cut.c		\
	cmd_diff.c		\
	cmd_draw.c		\
	cmd_edge.c		\
	cmd_flip.c		\
	cmd_flop.c		\
	cmd_foreach.c		\
	cmd_gamma.c		\
	cmd_gauss.c		\
	cmd_info.c		\
	cmd_invert.c		\
	cmd_laplace.c		\
	cmd_layer.c		\
	cmd_max.c		\
	cmd_mean.c		\
	cmd_median.c		\
	cmd_merge.c		\
	cmd_min.c		\
	cmd_mix.c		\
	cmd_resize.c		\
	cmd_reverse.c		\
	cmd_rotate.c		\
	cmd_scale.c		\
	cmd_select.c		\
	cmd_shear.c		\
	cmd_sort.c		\
	cmd_split.c		\
	cmd_tonemap.c		\
	cmd_unsharpmask.c	\
	cmd_visualize.c		\
	cmd_wavelets.c

cvtool_LDADD = $(top_builddir)/cvl/libcvl.la		\
	$(LTLIBGLEW) $(LTLIBCAIRO) $(LTLIBICONV)	\
	$(top_builddir)/mhlib/libmh.la			\
	$(top_builddir)/gnulib/libgnu.la

AM_CPPFLAGS = \
	-I$(top_builddir)/gnulib -I$(top_srcdir)/gnulib	\
	-I$(top_srcdir)/mhlib				\
	-I$(top_builddir)/cvl/cvl -I$(top_srcdir)/cvl
