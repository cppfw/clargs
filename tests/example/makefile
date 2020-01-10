include prorab.mk

this_name := tests

this_cxxflags += -g
this_cxxflags += -Wall
this_cxxflags += -Wno-format #no warnings about format
this_cxxflags += -Wno-format-security #no warnings about format
this_cxxflags += -DDEBUG
this_cxxflags += -fstrict-aliasing #strict aliasing!!!
this_cxxflags += -std=c++11

this_srcs += main.cpp

this_clargs_lib := $(d)../../src/build/libclargs$(dot_so)

this_ldlibs += $(this_clargs_lib) -lstdc++

ifeq ($(os), macosx)
    this_cxxflags += -stdlib=libc++ #this is needed to be able to use c++11 std lib
    this_ldlibs += -lc++
else ifeq ($(os),windows)

endif

this_no_install := true

$(eval $(prorab-build-app))

this_dirs := $(subst /, ,$(d))
this_test := $(word $(words $(this_dirs)),$(this_dirs))

define this_rules
test:: $(prorab_this_name)
$(.RECIPEPREFIX)@myci-running-test.sh $(this_test)
$(.RECIPEPREFIX)$(Q)cp $(d)../../src/*.dll . > /dev/null 2>&1 || true
$(.RECIPEPREFIX)$(Q)LD_LIBRARY_PATH=$(d)../../src/build $(d)./tests \
        -a \
        -r10 \
        -b \
        hello \
        extrra1 \
        extra/321jh \
        --key-without-short-one=trololo \
        --key-with-optional-value=qwerty \
        --another-key-with-optional-value \
        --key-empty-value= \
        \
        | sed 's/\x0D//' > $(d)out.txt
$(.RECIPEPREFIX)$(Q)cmp $(d)out.txt $(d)out.txt.cmp; \
        if [ $$$$? -ne 0 ]; then echo "out.txt = "; cat $(d)out.txt; myci-error.sh "test program output was not as expected"; exit 1; fi
$(.RECIPEPREFIX)@myci-passed.sh
endef
$(eval $(this_rules))


# add dependency on libclargs
$(prorab_this_name): $(abspath $(this_clargs_lib))

$(eval $(call prorab-include, ../../src/makefile))
