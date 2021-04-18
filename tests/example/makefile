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

this_dirs := $(subst /, ,$(d))
this_test := $(word $(words $(this_dirs)),$(this_dirs))

define this_rules
test:: $(prorab_this_name)
$(.RECIPEPREFIX)@myci-running-test.sh $(this_test)
$(.RECIPEPREFIX)$(Q)cp $(d)../../src/out/$(c)/*.dll . > /dev/null 2>&1 || true
$(.RECIPEPREFIX)$(Q)LD_LIBRARY_PATH=$(d)../../src/out/$(c) $(d)out/$(c)/tests \
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
        --key-to-test-quoted-value="expected long key quoted value" \
        --boolean-long-key-without-short-one \
        -q"expected short key quoted value" \
        -d "expected short key space quoted value" \
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
