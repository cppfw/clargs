include prorab.mk
include prorab-test.mk

# include harness for tst_cfg variable
$(eval $(call prorab-include, ../harness/makefile))

$(eval $(prorab-clear-this-vars))

$(eval $(call prorab-config, ../../config))

this_no_install := true

this_name := tests

this_srcs := $(call prorab-src-dir, src)

this_libclargs := ../../src/out/$(c)/libclargs$(dot_so)

this_harness_ld_paths := $(abspath $(d)../harness/tst/src/out/$(tst_cfg))

this_cxxflags += -I ../harness/tst/src -I ../../src
this_ldflags += $(addprefix -L,$(this_harness_ld_paths))
this_ldlibs += $(this_libclargs) -ltst -lutki

$(info LDFLAGS__unit = $(LDFLAGS))
$(info this_ldflags__unit = $(this_ldflags))

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), $(this_libclargs) ../harness/tst/src/out/$(tst_cfg)/libtst$(dot_so)))

this_test_cmd := $(prorab_this_name) --junit-out=$(dir $(prorab_this_name))/junit.xml
this_test_deps := $(prorab_this_name)
# for conan get ld paths from LDFLAGS
this_test_ld_path := $(dir $(this_libclargs)) $(this_harness_ld_paths) $(patsubst -L%,%,$(filter -L%,$(LDFLAGS)))
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))
