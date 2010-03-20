SUBDIRS = gnulib mhlib cvl cvtool doc tests
ACLOCAL_AMFLAGS = -I m4 -I gnulib/m4

dist-hook:
	if test -d "$(srcdir)/../.git" && type git > /dev/null 2>&1; then \
		( cd "$(srcdir)/.." && git log --no-color --date=iso --stat cvtool ) \
			> "$(distdir)/ChangeLog"; \
	fi; \
	cd doc/doxy; rm -f doxyrun.stamp; make doxy
