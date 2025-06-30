include prorab.mk

tst_cfg := no_par_no_install

# load config in order to resolve 'c' and 'config' in case of default config
$(eval $(call prorab-config, ../../config))

$(eval $(call prorab-depend, \
		tst/src/out/$(tst_cfg)/libtst$(dot_so), \
		../../src/out/$(c)/libclargs$(this_dbg)$(dot_so) \
	))

harness_old_cxxflags := $(CXXFLAGS)
harness_old_ldflags := $(LDFLAGS)
CXXFLAGS += -I$(d)../../src
LDFLAGS += -L$(d)../../src/out/$(c)
$(eval $(call prorab-include, tst/src/makefile, $(tst_cfg)))
CXXFLAGS := $(harness_old_cxxflags)
LDFLAGS := $(harness_old_ldflags)

$(eval $(call prorab-include, ../../src/makefile))
