EXTRA_DIST = doxyfile doxyrun.stamp cvl-reference

doxy: doxyrun.stamp

cvl-reference: doxy

doxyrun.stamp: 
	doxygen doxyfile \
	&& touch doxyrun.stamp

maintainer-clean-local:
	rm -f doxyrun.stamp cvl-reference/*

install-data-local: doxy
	installfiles=`echo cvl-reference/*`; \
	if test "$$installfiles" != 'cvl-reference/*'; then \
	  $(mkinstalldirs) $(DESTDIR)$(docdir)/cvl-reference; \
	  for i in $$installfiles; do \
	    $(INSTALL_DATA) $$i $(DESTDIR)$(docdir)/cvl-reference; \
	  done; \
	fi;

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/cvl-reference
