include prorab.mk

$(eval $(call prorab-config, ../config))

this_name := clargs

this_soname := $(shell cat $(d)soname.txt)

this_srcs := $(call prorab-src-dir,.)

this_ldlibs += -lutki

$(eval $(prorab-build-lib))
