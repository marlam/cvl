SUBDIRS = gnulib mhlib gllib src doc
ACLOCAL_AMFLAGS = -I m4 -I gnulib/m4
EXTRA_DIST = TODO

dist-hook:
	if test -d "$(srcdir)/../.git" && type git > /dev/null 2>&1; then \
		( cd "$(srcdir)/.." && git log --no-color --date=iso --stat cvlview ) \
			> "$(distdir)/ChangeLog"; \
	fi
