include prorab.mk
include prorab-test.mk

$(eval $(call prorab-try-simple-include, $(CONANBUILDINFO_DIR)conanbuildinfo.mak))

$(eval $(call prorab-include, ../../src/makefile))
$(eval $(call prorab-include, ../harness/makefile))

$(eval $(call prorab-config, ../../config))

this_no_install := true

this_name := tests

this_srcs := $(call prorab-src-dir, src)

this_libclargs := ../../src/out/$(c)/libclargs$(dot_so)

this_cxxflags += $(addprefix -I,$(CONAN_INCLUDE_DIRS))
this_ldflags += $(addprefix -L,$(CONAN_LIB_DIRS))

this_harness_ld_paths := $(abspath $(d)../harness/tst/src/out/$(tst_cfg))

this_cxxflags += -I ../harness/tst/src -I ../../src
this_ldflags += $(addprefix -L,$(this_harness_ld_paths))
this_ldlibs += $(this_libclargs) -ltst -lutki

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), $(this_libclargs) ../harness/tst/src/out/$(tst_cfg)/libtst$(dot_so)))

this_test_cmd := $(prorab_this_name) --junit-out=$(dir $(prorab_this_name))/junit.xml
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(dir $(this_libclargs)) $(this_harness_ld_paths) $(CONAN_LIB_DIRS)
$(eval $(prorab-test))