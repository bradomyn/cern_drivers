# include the build environment
include ../../../common.mk

SRCS := \
	acquire.c \
	available_event_lengths.c \
	available_freqs.c \
	clock.c \
	event_timestamping.c \
	fourier1.c \
	n_bits.c \
	n_events_max.c \
	offset.c \
	segments.c \
	start.c \
	stress.c \
	stop.c \
	trigger.c \
	two_segments.c \
	version.c

INCDIR := ../../include
LDFLAGS := -lm
CFLAGS := -Wall -D_GNU_SOURCE -g -I$(INCDIR)
CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
HLIBSIS33 := $(INCDIR)/libsis33.h
LIBSIS33 := ../libsis33.$(CPU).a
MYSTR := my_stringify.h
LDLIBS := $(LIBSIS33)
OBJS := $(SRCS:.c=.$(CPU).o)
FINAL_OBJS := $(SRCS:.c=.$(CPU))

all: $(LIBSIS33) $(FINAL_OBJS)
.PHONY: all clean
$(OBJS): $(HLIBSIS33) $(MYSTR)
$(FINAL_OBJS): $(LDLIBS)

clean:
	$(RM) *.$(CPU)
	$(RM) *.$(CPU).o

cleanall:
	$(RM) *.{$(ALL_CPUS_COMMAS)}
	$(RM) *.{$(ALL_CPUS_COMMAS)}.o

# empty install rule
install:
