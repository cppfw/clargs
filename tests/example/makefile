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

this_ldlibs += $(d)../../src/libclargs$(soext) -lstdc++

ifeq ($(os), macosx)
    this_cxxflags += -stdlib=libc++ #this is needed to be able to use c++11 std lib
    this_ldlibs += -lc++
else ifeq ($(os),windows)
    
endif

$(eval $(prorab-build-app))

this_dirs := $(subst /, ,$(d))
this_test := $(word $(words $(this_dirs)),$(this_dirs))

define this_rules
test:: $(prorab_this_name)
	@myci-running-test.sh $(this_test)
	$(prorab_echo)cp $(d)../../src/*.dll . || true
	$(prorab_echo)LD_LIBRARY_PATH=$(d)../../src $(d)./tests -a -r10 -b hello extrra1 extra/321jh --key-without-short-one=trololo > $(d)out.txt
	$(prorab_echo)cmp $(d)out.txt $(d)out.txt.cmp; \
	    if [ $$$$? -ne 0 ]; then myci-error.sh "test program output was not as expected"; exit 1; fi
	@myci-passed.sh
endef
$(eval $(this_rules))


#add dependency on libclargs
$(prorab_this_name): $(abspath $(d)../../src/libclargs$(soext))

$(eval $(call prorab-include,$(d)../../src/makefile))
