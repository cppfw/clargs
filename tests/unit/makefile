include prorab.mk
include prorab-test.mk

# include harness for tst_cfg variable
$(eval $(call prorab-include, ../harness/makefile))

$(eval $(prorab-clear-this-vars))

$(eval $(call prorab-config, ../../config))

this_no_install := true

this_name := tests

this_srcs := $(call prorab-src-dir, src)

this__libclargs := ../../src/out/$(c)/libclargs$(this_dbg)$(dot_so)

this__harness_ld_paths := $(abspath $(d)../harness/tst/src/out/$(tst_cfg))

this_cxxflags += -I ../harness/tst/src
this_cxxflags += -I ../../src

this_ldflags += $(addprefix -L,$(this__harness_ld_paths))
this_ldlibs +=  -l tst

this_ldlibs += -l utki$(this_dbg)
this_ldlibs += $(this__libclargs)

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), ../harness/tst/src/out/$(tst_cfg)/libtst$(dot_so)))

this_test_cmd := $(prorab_this_name) --junit-out=$(dir $(prorab_this_name))/junit.xml
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(dir $(this__libclargs)) $(this__harness_ld_paths)
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))
