EXTRA_DIST = doxyfile html doxyrun.stamp

doxy: doxyrun.stamp

doxyrun.stamp:
	rm -f html/* && cd ../../cvl && doxygen ../doc/doxy/doxyfile && touch ../doc/doxy/doxyrun.stamp

#

maintainer-clean-local:
	rm -f doxyrun.stamp html/*

install-data-local: doxy
	installfiles=`echo $(srcdir)/html/*`; \
	if test "$$installfiles" = '$(srcdir)/html/*'; \
	then echo '-- Nothing to install' ; \
	else \
	  $(mkinstalldirs) $(datadir)/doc/$(PACKAGE)/cvl-reference; \
	  for i in $$installfiles; do \
	    echo '-- Installing '$$i ; \
	    $(INSTALL_DATA) $$i $(datadir)/doc/$(PACKAGE)/cvl-reference; \
	  done; \
	fi

uninstall-local:
	rm -rf $(datadir)/doc/$(PACKAGE)

dist-hook-local: doxy
	cp $(srcdir)/html/* $(distdir)/html

.PHONY: dist-hook-local
