EXTRA_DIST = doxyfile doxyrun.stamp cvl-reference

doxy: doxyrun.stamp

doxyrun.stamp: doxyfile
	doxygen $(builddir)/doxyfile && touch doxyrun.stamp

maintainer-clean-local:
	rm -f doxyrun.stamp $(builddir)/cvl-reference/*

install-data-local: doxy
	installfiles=`echo $(builddir)/cvl-reference/*`; \
	if test "$$installfiles" != '$(builddir)/cvl-reference/*'; then \
	  $(mkinstalldirs) $(DESTDIR)$(docdir)/cvl-reference; \
	  for i in $$installfiles; do \
	    $(INSTALL_DATA) $$i $(DESTDIR)$(docdir)/cvl-reference; \
	  done; \
	fi

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/cvl-reference
