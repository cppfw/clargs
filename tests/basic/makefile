include prorab.mk
include myci.mk

$(eval $(call myci-try-include, $(CONANBUILDINFO_DIR)/conanbuildinfo.mak))

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

include $(d)../test_target.mk

$(eval $(call prorab-include, ../../src/makefile))
