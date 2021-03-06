# ======================================================
# Makefile to produce the SL TG8 driver
# ======================================================

CPU=ppc4
ifeq ($(CPU),ppc)
    DDIR=3.1.0
    DSCDEV=psdsc01
endif
ifeq ($(CPU),ppc4)
    DDIR=4.0.0
    DSCDEV=ppccodv20
endif

include /ps/src/dsc/co/Make.auto

DRVR=tg8drvr
SRC1=tg8drvr.c

#========================================================================
# compiler to be used to compile drivers
#========================================================================

DRCC=$(KERN_CC)

#========================================================================
# final binaries to produce
#========================================================================

all: $(DRVR) tg8drvr.info

#========================================================================
# How to produce the device driver itself
#========================================================================
%.$(CPU).$(BSP).o: %.c
	 $(DRCC) $(KERN_CFLAGS) $(SPECIAL_INCLUDES) $(KERN_INCLUDES) \
	 -I$(ENV_PREFIX)/usr/include/ces $(CPPFLAGS) -I. -c -o $@ $^

$(DRVR).$(BSP).$(CPU).o: $(SRC1:.c=.$(CPU).$(BSP).o)
	@$(RM) $(DRVR).o /tmp/$@
	$(KERN_LD) -r -o /tmp/$@ $^
	$(TOOLS1)nm -u /tmp/$@  | sed '/^[.]/s///' | sort -u > $(DRVR).$(BSP).imp
	$(KERN_LD) -bM:SRE -bimport:$(DRVR).$(BSP).imp -o $@ $^
	@$(RM) $(DRVR).o /tmp/$@

$(DRVR): $(DRVR).$(BSP).$(CPU).o
	cp -p $(DRVR).$(BSP).$(CPU).o $(DRVR)

install: $(DRVR).$(BSP).$(CPU).o tg8drvr.info
	@-rm -f tg8drvr
	cp -p $(DRVR).$(BSP).$(CPU).o tg8drvr
	@for a in $(ACCS); do \
	    if [ -w /ps/dsc/$$a/$(CPU)/drivers/$(DDIR) ]; then \
		echo Installing tg8drvr in /ps/dsc/$$a/$(CPU)/drivers/$(DDIR) ;\
		dsc_install tg8drvr /ps/dsc/$$a/$(CPU)/drivers/$(DDIR) ;\
	    else \
		echo No write access to /ps/dsc/$$a/$(CPU)/drivers/$(DDIR); \
	    fi; \
	    if [ -w /ps/dsc/$$a/$(CPU)/devices ]; then \
		echo Installing tg8drvr.info in /ps/dsc/$$a/$(CPU)/devices ;\
		dsc_install tg8drvr.info /ps/dsc/$$a/$(CPU)/devices ;\
	    else \
		echo No write access to /ps/dsc/$$a/$(CPU)/devices; \
	    fi; \
	    done

tg8drvr.info: tg8drvr.info.c
	$(RM) $@.$(CPU)
	$(CC) -I. $(CPPFLAGS) -o $@.$(CPU) $^
	@sync; sleep 1
ifeq ($(OS),lynxos)
	./$@.$(CPU) > $@
else
	@if [ ! -f /ps/home/${USER}/.ssh/authorized_keys ]; then dsc_keygen; fi
	ssh -x -k $(DSCDEV) -n `pwd`/$@.$(CPU) > $@
endif

clean:
	-$(RM) $(DRVR).*.o $(DRVR).*.imp $(BAKS) $(DRVR) tg8drvr.info.ppc4 tg8drvr.info


#========================================================================
# Rule to produce a prototype of all functions
#========================================================================

ANSI.h: $(DRVR).c
	cproto -m -e -o $@ -I- $(CFLAGS) $(STDFLAGS) $^

