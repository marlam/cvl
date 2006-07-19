SUBDIRS = doxy

man_MANS = cvtool.1

info_TEXINFOS = cvl.texi cvtool.texi
cvl_TEXINFOS = cvl.texi
cvtool_TEXINFOS = cvtool.texi cvtool-example-nasa-script.sh

cvtool_IMAGES_jpg = \
	cvtool-example-nasac000.jpg \
	cvtool-example-nasad000.jpg \
	cvtool-example-nasac225.jpg \
	cvtool-example-nasad225.jpg \
	cvtool-example-nasac450.jpg \
	cvtool-example-nasad450.jpg

cvtool_IMAGES_pdf = \
	cvtool-example-nasac000.pdf \
	cvtool-example-nasad000.pdf \
	cvtool-example-nasac225.pdf \
	cvtool-example-nasad225.pdf \
	cvtool-example-nasac450.pdf \
	cvtool-example-nasad450.pdf

AM_MAKEINFOHTMLFLAGS = --no-split

EXTRA_DIST = cvl.html cvl.pdf cvtool.html cvtool.pdf $(man_MANS) \
	$(cvtool_IMAGES_jpg) $(cvtool_IMAGES_pdf) \
	cvtool-example-nasa-script.sh

install-data-local: cvl.html cvl.pdf cvtool.html cvtool.pdf
	$(mkinstalldirs) $(datadir)/doc/$(PACKAGE); \
	echo '-- Installing 'cvl.html; \
	$(INSTALL_DATA) cvl.html $(datadir)/doc/$(PACKAGE); \
	echo '-- Installing 'cvl.pdf; \
	$(INSTALL_DATA) cvl.pdf $(datadir)/doc/$(PACKAGE);
	echo '-- Installing 'cvtool.html; \
	$(INSTALL_DATA) cvtool.html $(datadir)/doc/$(PACKAGE); \
	for img in $(cvtool_IMAGES_jpg); do $(INSTALL_DATA) $$img $(datadir)/doc/$(PACKAGE); done; \
	echo '-- Installing 'cvtool.pdf; \
	$(INSTALL_DATA) cvtool.pdf $(datadir)/doc/$(PACKAGE);
	echo '-- Installing 'cvtool-example-nasa-script.sh; \
	$(INSTALL_DATA) cvtool-example-nasa-script.sh $(datadir)/doc/$(PACKAGE);

uninstall-local:
	rm -rf $(datadir)/doc/$(PACKAGE)
