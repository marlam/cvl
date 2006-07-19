SUBDIRS = build-aux gnulib notgnulib cvl cvtool doc tests
EXTRA_DIST = TODO
ACLOCAL_AMFLAGS = -I build-aux -I gnulib/m4 -I notgnulib/m4

doxy:
	cd doc/doxy && rm -f doxyrun.stamp && make doxy
