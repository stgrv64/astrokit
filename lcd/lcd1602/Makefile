# stgrv64 - Projet astrokit - Tous droits reserves - 2022
# -------------------------------------------------------------------------------------
# HISTORIQUE MODIFICATION
#  09/04/2022 : 
#     * ajout prise en compte argument M= comme pour Makefile astrokit
#       make M=targ => compilation avec arm-gcc
#       make M=host => compilation avec gcc
#     * remplacement SUBDIRS = lib demo par SUBDIRS = lib lcd1602
# --------------------------------------------------------

CCTARG=/usr/bin/arm-linux-gnueabihf-gcc
CCHOST=gcc
CROSS_COMPILE ?= 
REPLIB=lib
EXTENSION=

ifeq ($(M),host)
	EXTENSION=host
	REPLIB=libhost
	CC=$(CCHOST)
#	RLIB=$(HLIB)
else
	EXTENSION=targ
	REPLIB=libtarget
	CROSS_COMPILE = /usr/bin/arm-linux-gnueabihf-
	CC=$(CCTARG)
#	RLIB=$(TLIB)
endif

TARGET	= $(patsubst %.c,%, $(wildcard *.c))
SRC 	= $(wildcard *.c)
SRC 	+= $(wildcard *.h)

LIBS = -L ../$(REPLIB)
INCS = -I ../lib/includes

$(TARGET):$(SRC)
	$(CC) -o $@ $(LIBS) $(INCS) $^ -lfahw -lm -Wall
	cp $(TARGET) $(TARGET).$(EXTENSION)
	install --mode=755 $(TARGET).$(EXTENSION) ..
	install --mode=755 $(TARGET).$(EXTENSION) ../..
	install --mode=755 $(TARGET).$(EXTENSION) ../../bin

.PHONY: clean 
clean:
	rm -f $(TARGET)
