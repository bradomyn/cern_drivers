# include the build environment
include ../common.mk

DIRS = \
    doc \
    driver \
    lib \
    lib/doc \
    lib/test \
    scripts \

.PHONY: all clean cleanall install $(DIRS)

all clean cleanall install: $(DIRS)
clean: TARGET = clean
cleanall: TARGET = cleanall
install: TARGET = install

$(DIRS):
	$(MAKE) -C $@ $(TARGET)

lib/test: lib
