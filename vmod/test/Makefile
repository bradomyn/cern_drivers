# include the build environment
include ../../common.mk

#include version information
include ../versions.mk

CFLAGS += -I ../include/ -I./ -I../driver/
CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
LDFLAGS += -L ../lib
LDLIBS += -lpthread -lvmod.$(CPU)

ALL=\
	tstlibttl.$(CPU) \
	tstlibdor.$(CPU) \
	load_test.$(CPU) \
	tstlibttl_quick_write.$(CPU) \
	tstlibttl_quick_read.$(CPU) \
	tstlibttl_read_device.$(CPU) \
	tstlib12a2.$(CPU) \
	tstlib12a4.$(CPU) \
	tstlib16a2.$(CPU) \
	tstlib12e16.$(CPU) \

all: $(ALL)

clean:
	$(RM) *.$(CPU)

cleanall:
	$(RM) *.{$(ALL_CPUS_COMMAS)}

PROGS_LIST=$(ALL)

install: install_prog_global
