include prorab.mk

$(eval $(call prorab-config, ../../config))

this_name := tests

this_srcs += $(call prorab-src-dir, .)

this_clargs_lib = $(d)../../src/out/$(c)/libclargs$(dot_so)

this_ldlibs += $(this_clargs_lib) -lutki

this_no_install := true

$(eval $(prorab-build-app))

# add dependency on libclargs
$(prorab_this_name): $(abspath $(this_clargs_lib))

include $(d)../test_target.mk

$(eval $(call prorab-include, ../../src/makefile))
