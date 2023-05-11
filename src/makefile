include prorab.mk
include prorab-license.mk
include prorab-clang-format.mk

$(eval $(call prorab-config, ../config))

this_name := clargs

this_soname := $(shell cat $(d)soname.txt)

this_srcs := $(call prorab-src-dir,.)

this_ldlibs += -lutki

$(eval $(prorab-build-lib))

this_src_dir := 
this_license_file := ../LICENSE
$(eval $(prorab-license))

$(eval $(prorab-clang-format))
