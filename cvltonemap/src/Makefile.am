bin_PROGRAMS = cvltonemap

cvltonemap_SOURCES = main.cpp				\
	err.h err.cpp					\
	conf.h conf.cpp					\
	cvltonemap.h cvltonemap.cpp			\
	tonemap_selector.h tonemap_selector.cpp		\
	postproc_selector.h postproc_selector.cpp	\
	viewpoint_selector.h viewpoint_selector.cpp	\
	view_area.h view_area.cpp

if W32
cvltonemap_SOURCES += appicon.ico
endif

nodist_cvltonemap_SOURCES = \
	moc_cvltonemap.cpp		\
	moc_tonemap_selector.cpp	\
	moc_postproc_selector.cpp	\
	moc_viewpoint_selector.cpp	\
	moc_view_area.cpp		\
	rcc_cvltonemap.cpp

BUILT_SOURCES = $(nodist_cvltonemap_SOURCES)

EXTRA_DIST = appicon.ico appicon.png cvltonemap.qrc

CLEANFILES = $(BUILT_SOURCES)

cvltonemap_LDADD = \
	$(top_builddir)/mhlib/libmh.a	\
	$(top_builddir)/gnulib/libgnu.a	\
	$(QT4_LIBS) $(CVL_LIBS)

cvltonemap_CPPFLAGS = \
	-I$(top_srcdir)/gnulib			\
	-I$(top_srcdir)/mhlib

cvltonemap_CXXFLAGS = $(QT4_CXXFLAGS)

.ico.o:
	echo "IDI_ICON1 ICON DISCARDABLE \"$<\"" | $(WINDRES) -o $@

moc_%.cpp: %.h
	$(QT4_MOC) -o $@ $<

rcc_%.cpp: %.qrc
	$(QT4_RCC) -o $@ $<

%.glsl.h: %.glsl
	MACRONAME="`echo $< | sed -e s/^.*\\\/// -e s/\\\.glsl$$// -e s/-/_/g | tr [:lower:] [:upper:]`_GLSL_STR"; \
	(echo "#ifndef $$MACRONAME"; \
	 echo "#define $$MACRONAME \\"; \
	 sed -e s/\\\\/\\\\\\\\/g \
	     -e s/\"/\\\\\"/g \
	     -e s/^/\"/ \
	     -e s/$$/\\\\n\"\\\\/ < $<; \
	 echo \"\"; \
	 echo "#endif") > $@