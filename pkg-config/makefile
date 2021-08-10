include prorab.mk

define this_rule
    install::
$(.RECIPEPREFIX)$(a)myci-apply-version.sh `myci-deb-version.sh $(d)../debian/changelog` $(d)*.pc.in
$(.RECIPEPREFIX)$(a)install -d $(DESTDIR)$(PREFIX)/lib/pkgconfig
$(.RECIPEPREFIX)$(a)install $(d)*.pc $(DESTDIR)$(PREFIX)/lib/pkgconfig
endef
$(eval $(this_rule))
