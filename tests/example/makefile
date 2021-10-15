include prorab.mk
include prorab-test.mk

$(eval $(call prorab-try-simple-include, $(CONANBUILDINFO_DIR)conanbuildinfo.mak))

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

this_test_deps := $(prorab_this_name)
this_test_ld_path := ../../src/out/$(c) $(CONAN_LIB_DIRS)

this_test_cmd := $(prorab_this_name) \
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
        | sed 's/\x0D//' > $(d)out.txt && \
        cmp $(d)out.txt $(d)out.txt.cmp; \
        if [ $$$$? -ne 0 ]; then echo "out.txt = "; cat $(d)out.txt; myci-error.sh "test program output was not as expected"; exit 1; fi

$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))
