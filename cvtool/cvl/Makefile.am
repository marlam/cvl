%.glsl.h: %.glsl
	MACRONAME="CVL_`echo $< | sed -e s/^.*\\\/// -e s/\\\.glsl$$// -e s/-/_/g | tr [:lower:] [:upper:]`_GLSL_STR"; \
	(echo "#ifndef $$MACRONAME"; \
	 echo "#define $$MACRONAME \\"; \
	 sed -e s/\\\\/\\\\\\\\/g \
	     -e s/\"/\\\\\"/g \
	     -e s/^/\"/ \
	     -e s/$$/\\\\n\"\\\\/ < $<; \
	 echo \"\"; \
	 echo "#endif") > $@

lib_LTLIBRARIES = libcvl.la

nobase_include_HEADERS = \
	cvl/cvl_version.h	\
	cvl/cvl_error.h		\
	cvl/cvl_gl.h		\
	cvl/cvl_init.h		\
	cvl/cvl_taglist.h	\
	cvl/cvl_frame.h		\
	cvl/cvl_color.h		\
	cvl/cvl_basic.h		\
	cvl/cvl_io.h		\
	cvl/cvl_transform.h	\
	cvl/cvl_math.h		\
	cvl/cvl_mix.h		\
	cvl/cvl_filter.h	\
	cvl/cvl_misc.h		\
	cvl/cvl_features.h	\
	cvl/cvl_hdr.h		\
	cvl/cvl_visualization.h	\
	cvl/cvl.h

libcvl_la_SOURCES = cvl_intern.h \
	cvl_intern.c		\
	cvl_error.c		\
	cvl_gl.c		\
	cvl_init.c		\
	cvl_taglist.c		\
	cvl_frame.c		\
	cvl_color.c		\
	cvl_basic.c		\
	cvl_io.c		\
	cvl_transform.c		\
	cvl_math.c		\
	cvl_mix.c		\
	cvl_filter.c		\
	cvl_misc.c		\
	cvl_features.c		\
	cvl_hdr.c		\
	cvl_visualization.c

nodist_libcvl_la_SOURCES = \
	glsl/color/lum_to_rgb.glsl.h				\
	glsl/color/lum_to_xyz.glsl.h				\
	glsl/color/rgb_to_lum.glsl.h				\
	glsl/color/rgb_to_xyz.glsl.h				\
	glsl/color/xyz_to_rgb.glsl.h				\
	glsl/color/rgb_to_hsl.glsl.h				\
	glsl/color/hsl_to_rgb.glsl.h				\
	glsl/color/channel_combine.glsl.h			\
	glsl/color/channel_extract.glsl.h			\
	glsl/color/invert.glsl.h				\
	glsl/color/gamma_correct.glsl.h				\
	glsl/color/color_adjust.glsl.h				\
	glsl/color/transform_linear.glsl.h			\
	glsl/color/transform_log.glsl.h				\
	glsl/color/luminance_range.glsl.h			\
	glsl/color/pseudo_color.glsl.h				\
	glsl/color/threshold.glsl.h				\
	glsl/transform/bilinear.glsl.h				\
	glsl/transform/biquadratic.glsl.h			\
	glsl/transform/bicubic.glsl.h				\
	glsl/math/add.glsl.h					\
	glsl/math/mul.glsl.h					\
	glsl/math/first_derivative.glsl.h			\
	glsl/math/laplacian.glsl.h				\
	glsl/mix/layer.glsl.h					\
	glsl/mix/blend.glsl.h					\
	glsl/mix/mix.glsl.h					\
	glsl/filter/convolve.glsl.h				\
	glsl/filter/convolve_separable.glsl.h			\
	glsl/filter/convolve3d.glsl.h				\
	glsl/filter/convolve3d_separable.glsl.h			\
	glsl/filter/min.glsl.h					\
	glsl/filter/min3d.glsl.h				\
	glsl/filter/max.glsl.h					\
	glsl/filter/max3d.glsl.h				\
	glsl/filter/median.glsl.h				\
	glsl/filter/median3d.glsl.h				\
	glsl/filter/median_separated.glsl.h			\
	glsl/filter/median3d_separated.glsl.h			\
	glsl/filter/laplace.glsl.h				\
	glsl/filter/unsharpmask.glsl.h				\
	glsl/misc/resize_seq.glsl.h				\
	glsl/misc/reduce.glsl.h					\
	glsl/misc/sort.glsl.h					\
	glsl/misc/pyramid_gaussian.glsl.h			\
	glsl/features/sobel.glsl.h				\
	glsl/features/canny_nms.glsl.h				\
	glsl/features/canny_hysterese1.glsl.h			\
	glsl/features/canny_hysterese2.glsl.h			\
	glsl/hdr/tonemap_schlick94.glsl.h			\
	glsl/hdr/tonemap_tumblinrushmeier99_step1.glsl.h	\
	glsl/hdr/tonemap_tumblinrushmeier99_step2.glsl.h	\
	glsl/hdr/tonemap_drago03.glsl.h				\
	glsl/hdr/tonemap_durand02_step1.glsl.h			\
	glsl/hdr/tonemap_durand02_step2.glsl.h			\
	glsl/visualization/vector2_color.glsl.h

EXTRA_DIST = cvl/cvl_version.h.in \
	glsl/color/lum_to_rgb.glsl				\
	glsl/color/lum_to_xyz.glsl				\
	glsl/color/rgb_to_lum.glsl				\
	glsl/color/rgb_to_xyz.glsl				\
	glsl/color/xyz_to_rgb.glsl				\
	glsl/color/rgb_to_hsl.glsl				\
	glsl/color/hsl_to_rgb.glsl				\
	glsl/color/channel_combine.glsl				\
	glsl/color/channel_extract.glsl				\
	glsl/color/invert.glsl					\
	glsl/color/gamma_correct.glsl				\
	glsl/color/color_adjust.glsl				\
	glsl/color/transform_linear.glsl			\
	glsl/color/transform_log.glsl				\
	glsl/color/luminance_range.glsl				\
	glsl/color/pseudo_color.glsl				\
	glsl/color/threshold.glsl				\
	glsl/transform/bilinear.glsl				\
	glsl/transform/biquadratic.glsl				\
	glsl/transform/bicubic.glsl				\
	glsl/math/add.glsl					\
	glsl/math/mul.glsl					\
	glsl/math/first_derivative.glsl				\
	glsl/math/laplacian.glsl				\
	glsl/mix/blend.glsl					\
	glsl/mix/layer.glsl					\
	glsl/mix/mix.glsl					\
	glsl/filter/convolve.glsl				\
	glsl/filter/convolve_separable.glsl			\
	glsl/filter/convolve3d.glsl				\
	glsl/filter/convolve3d_separable.glsl			\
	glsl/filter/min.glsl					\
	glsl/filter/min3d.glsl					\
	glsl/filter/max.glsl					\
	glsl/filter/max3d.glsl					\
	glsl/filter/median.glsl					\
	glsl/filter/median3d.glsl				\
	glsl/filter/median_separated.glsl			\
	glsl/filter/median3d_separated.glsl			\
	glsl/filter/laplace.glsl				\
	glsl/filter/unsharpmask.glsl				\
	glsl/misc/resize_seq.glsl				\
	glsl/misc/reduce.glsl					\
	glsl/misc/sort.glsl					\
	glsl/misc/pyramid_gaussian.glsl				\
	glsl/features/sobel.glsl				\
	glsl/features/canny_nms.glsl				\
	glsl/features/canny_hysterese1.glsl			\
	glsl/features/canny_hysterese2.glsl			\
	glsl/hdr/tonemap_schlick94.glsl				\
	glsl/hdr/tonemap_tumblinrushmeier99_step1.glsl		\
	glsl/hdr/tonemap_tumblinrushmeier99_step2.glsl		\
	glsl/hdr/tonemap_drago03.glsl				\
	glsl/hdr/tonemap_durand02_step1.glsl			\
	glsl/hdr/tonemap_durand02_step2.glsl			\
	glsl/visualization/vector2_color.glsl

BUILT_SOURCES = $(nodist_libcvl_la_SOURCES)

CLEANFILES = $(BUILT_SOURCES)

libcvl_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) $(GL_LIBS)
libcvl_la_LIBADD = $(top_builddir)/mhlib/libmh.la $(top_builddir)/gnulib/libgnu.la

AM_CPPFLAGS = -I$(top_srcdir)/gnulib -I$(top_srcdir)/mhlib
