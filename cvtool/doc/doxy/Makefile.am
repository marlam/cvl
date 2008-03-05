EXTRA_DIST = doxyfile doxyrun.stamp cvl-reference cvl-reference.pdf

doxy: doxyrun.stamp

cvl-reference: doxy

cvl-reference.pdf: doxy

doxyrun.stamp: 
	doxygen doxyfile \
	&& make -C latex \
	&& cp latex/refman.pdf cvl-reference.pdf \
	&& touch doxyrun.stamp

maintainer-clean-local:
	rm -f doxyrun.stamp cvl-reference/* cvl-reference.pdf

install-data-local: doxy
	installfiles=`echo cvl-reference/*`; \
	if test "$$installfiles" != 'cvl-reference/*'; then \
	  $(mkinstalldirs) $(DESTDIR)$(docdir)/cvl-reference; \
	  for i in $$installfiles; do \
	    $(INSTALL_DATA) $$i $(DESTDIR)$(docdir)/cvl-reference; \
	  done; \
	fi; \
	if test -f cvl-reference.pdf; then \
	  $(INSTALL_DATA) cvl-reference.pdf $(DESTDIR)$(docdir); \
	fi

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/cvl-reference; \
	rm -f $(DESTDIR)$(docdir)/cvl-reference.pdf
