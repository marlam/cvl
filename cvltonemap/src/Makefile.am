bin_PROGRAMS = cvltonemap

cvltonemap_SOURCES = main.cpp				\
	err.h err.cpp					\
	conf.h conf.cpp					\
	cvltonemap.h cvltonemap.cpp			\
	tonemap_selector.h tonemap_selector.cpp		\
	postproc_selector.h postproc_selector.cpp	\
	maxabslum_selector.h maxabslum_selector.cpp	\
	viewpoint_selector.h viewpoint_selector.cpp	\
	precision_selector.h precision_selector.cpp	\
	view_area.h view_area.cpp			\
	import.h import.c

if W32
cvltonemap_SOURCES += appicon.ico
cvltonemap_LDFLAGS = -mwindows
endif

RESOURCES = appicon.png

nodist_cvltonemap_SOURCES = \
	moc_cvltonemap.cpp		\
	moc_tonemap_selector.cpp	\
	moc_postproc_selector.cpp	\
	moc_maxabslum_selector.cpp	\
	moc_viewpoint_selector.cpp	\
	moc_precision_selector.cpp	\
	moc_view_area.cpp		\
	rcc_cvltonemap.cpp

BUILT_SOURCES = $(nodist_cvltonemap_SOURCES)

EXTRA_DIST = appicon.ico appicon.png cvltonemap.qrc

CLEANFILES = $(BUILT_SOURCES)

cvltonemap_LDADD = \
	$(top_builddir)/mhlib/libmh.a	\
	$(top_builddir)/gnulib/libgnu.a	\
	$(LIBQTOPENGL) $(LIBCVL) $(LIBGLEW)

cvltonemap_CPPFLAGS = \
	-I$(top_builddir)/gnulib -I$(top_srcdir)/gnulib	\
	-I$(top_srcdir)/mhlib

.ico.o:
	echo "IDI_ICON1 ICON DISCARDABLE \"$<\"" | $(WINDRES) -o $@

moc_verbose = $(moc_verbose_$(V))
moc_verbose_ = $(moc_verbose_$(AM_DEFAULT_VERBOSITY))
moc_verbose_0 = @echo "  MOC   " $@;

moc_%.cpp: %.h
	$(moc_verbose)$(MOC) -o $@ $<

rcc_verbose = $(rcc_verbose_$(V))
rcc_verbose_ = $(rcc_verbose_$(AM_DEFAULT_VERBOSITY))
rcc_verbose_0 = @echo "  RCC   " $@;

rcc_%.cpp: %.qrc
	$(rcc_verbose)$(RCC) -o $@ $<
