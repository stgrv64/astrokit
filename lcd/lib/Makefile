# stgrv64 - Projet astrokit - Tous droits reserves - 2022
# -------------------------------------------------------------------------------------
# HISTORIQUE MODIFICATION
#  09/04/2022 : 
#     * ajout prise en compte argument M= comme pour Makefile astrokit
#       make M=targ => compilation avec arm-gcc
#       make M=host => compilation avec gcc
# --------------------------------------------------------

# CROSS_COMPILE ?= 
# CROSS_COMPILE ?= arm-linux-gnueabihf-

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
RANLIB	= $(CROSS_COMPILE)ranlib

CFLAGS	= -Wall -Os -Wno-format-overflow
LDFLAGS	= -lpthread
LIBSO		= libfahw.so
LIBA	= libfahw.a
CFLAGS	+= -fPIC

OBJDIR	= .obj
DEPDIR	= .deps
LIBSRC	= $(wildcard *.c)
LIBOBJ	= $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(LIBSRC))))
DEPS	= $(patsubst %.c,$(DEPDIR)/%.Po,$(LIBSRC))

all: $(LIBSO) $(LIBA)
lib: $(LIBSO)
slib: $(LIBA)
$(OBJDIR)/%.o: %.c
	@test -d $(DEPDIR) || mkdir -p $(DEPDIR)
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $< -I ./includes
	@mv $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
	
-include $(DEPS)
$(LIBSO): $(LIBOBJ)
	$(CC) $(LDFLAGS) -shared -o $@ $^
	
$(LIBA): $(LIBOBJ)
	$(AR) rcu $@ $^
	$(RANLIB) $@
	
install: $(LIBSO)
	install ./includes/libfahw*.h /usr/local/include
	install --mode=644 libfahw.so /usr/local/lib
	install --mode=644 libfahw.a /usr/local/lib
	ldconfig
clean:
	rm -rf $(LIBSO) $(LIBOBJ) $(LIBA) $(DEPS) $(OBJDIR) $(DEPDIR)
