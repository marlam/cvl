SUBDIRS = doxy

man_MANS = cvtool.1

info_TEXINFOS = cvl.texi cvtool.texi
cvl_TEXINFOS = cvl.texi fdl.texi gpl.texi
cvtool_TEXINFOS = cvtool.texi fdl.texi gpl.texi

AM_MAKEINFOHTMLFLAGS = --no-split

EXTRA_DIST = cvl.html cvl.pdf cvtool.html cvtool.pdf $(man_MANS)

install-data-local: cvl.html cvl.pdf cvtool.html cvtool.pdf
	$(mkinstalldirs) $(docdir); \
	echo '-- Installing 'cvl.html; \
	$(INSTALL_DATA) cvl.html $(docdir); \
	echo '-- Installing 'cvl.pdf; \
	$(INSTALL_DATA) cvl.pdf $(docdir); \
	echo '-- Installing 'cvtool.html; \
	$(INSTALL_DATA) cvtool.html $(docdir); \
	echo '-- Installing 'cvtool.pdf; \
	$(INSTALL_DATA) cvtool.pdf $(docdir);

uninstall-local:
	rm -rf $(docdir)
