include prorab.mk

this_name := tests

this_cxxflags += -g
this_cxxflags += -Wall
this_cxxflags += -Wno-format #no warnings about format
this_cxxflags += -Wno-format-security #no warnings about format
this_cxxflags += -DDEBUG
this_cxxflags += -fstrict-aliasing #strict aliasing!!!
this_cxxflags += -std=c++17

this_srcs += main.cpp

this_clargs_lib = $(d)../../src/build/libclargs$(dot_so)

this_ldlibs += $(this_clargs_lib) -lstdc++

ifeq ($(os), macosx)
    this_cxxflags += -stdlib=libc++ #this is needed to be able to use c++11 std lib
    this_ldlibs += -lc++
else ifeq ($(os),windows)

endif

this_no_install := true

$(eval $(prorab-build-app))

# add dependency on libclargs
$(prorab_this_name): $(abspath $(this_clargs_lib))

include $(d)../test_target.mk

$(eval $(call prorab-include, ../../src/makefile))
