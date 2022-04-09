# CROSS_COMPILE ?= 
# CROSS_COMPILE ?= arm-linux-gnueabihf-

AS                      = $(CROSS_COMPILE)as
LD                      = $(CROSS_COMPILE)ld
CC                      = $(CROSS_COMPILE)gcc
AR                      = $(CROSS_COMPILE)ar
NM                      = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump
RANLIB          = $(CROSS_COMPILE)ranlib

export AS LD CC AR NM STRIP OBJCOPY OBJDUMP RANLIB CROSS_COMPILE

SUBDIRS			= $(wildcard matrix*)
TARGET			= subdirs
PHONY			+= $(TARGET) $(SUBDIRS) %.clean

all: $(TARGET)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

%.clean:
	@(cd $(patsubst %.clean, %, $@) && $(MAKE) clean)

clean distclean: $(patsubst %, %.clean, $(SUBDIRS))

install: 
	mkdir -p $(INSTALLDIR)/usr/bin
	for demo_folder in $(wildcard matrix*); do \
		cd $$demo_folder; \
		install --mode=755 matrix* /usr/bin; \
		cd ..;\
	done;  
	
.PHONY: $(PHONY) install clean distclean
