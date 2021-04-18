include prorab.mk
include prorab-test.mk
include myci.mk

$(eval $(call myci-try-include, $(CONANBUILDINFO_DIR)conanbuildinfo.mak))

$(eval $(call prorab-config, ../../config))

this_name := tests

this_srcs += $(call prorab-src-dir, .)

this_clargs_lib = $(d)../../src/out/$(c)/libclargs$(dot_so)

this_cxxflags += $(addprefix -I,$(CONAN_INCLUDE_DIRS))
this_ldflags += $(addprefix -L,$(CONAN_LIB_DIRS))

this_ldlibs += $(this_clargs_lib) -lutki

this_no_install := true

$(eval $(prorab-build-app))

# add dependency on libclargs
$(prorab_this_name): $(abspath $(this_clargs_lib))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := ../../src/out/$(c) $(CONAN_LIB_DIRS)
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))
