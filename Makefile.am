AUTOMAKE_OPIONS=foreign no-dependencies subdir-objects
ACLOCAL_AMFLAGS =
SUBDIRS = src

#INCLUDES = @INCLUDES@

install-data-local:
	test -z "$(DESTDIR)$(sysconfdir)" || /bin/mkdir -p "$(DESTDIR)$(sysconfdir)"
	test -f "$(DESTDIR)$(sysconfdir)/meer.yaml" || $(INSTALL_DATA) etc/meer.yaml "$(DESTDIR)$(sysconfdir)/meer.yaml"
	test -z "$(DESTDIR)$(sbindir)" || /bin/mkdir -p "$(DESTDIR)$(sbindir)"
	$(INSTALL) -m 755 src/meer "$(DESTDIR)$(sbindir)/meer"
	@echo ""
	@echo "------------------------------------------------------------------------------"
	@echo "Meer has been installed!"
	@echo "------------------------------------------------------------------------------"
	@echo ""



