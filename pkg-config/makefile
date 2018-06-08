include prorab.mk

define this_rule
    install::
    #install pkg-config files
		$(prorab_echo)myci-apply-version.sh `myci-deb-version.sh $(d)../debian/changelog` $(d)*.pc.in
		$(prorab_echo)install -d $(DESTDIR)$(PREFIX)/lib/pkgconfig
		$(prorab_echo)install $(d)*.pc $(DESTDIR)$(PREFIX)/lib/pkgconfig
endef
$(eval $(this_rule))
