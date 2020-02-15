# Project: monture altazimutale controlee par port gpio raspaberry pi
# Makefile created by stephane Gravois - 2020
# -------------------------------------------------------------------------------------
# Le compilateur est cense etre dans le PATH (dans /usr/bin par exemple)
# Il peut resulter sous debian d un 
#   apt-get install --buil-essential <architecture>
# ou bien de facon plus general : 
#		sudo apt-get install gcc-<arch>-linux
#   sudo apt-get install gcc-aarch64-linux-gnu
# -------------------------------------------------------------------------------------
# Regles implicites (rappel) : 
# * make va d'abord rechercher EXEC pui all puis ..

#--------------------------------------------------------------------------------------
# CC	= gcc
# CC	= armv6-gcc
# CC	=	/usr/bin/arm-linux-gnueabi-gcc
CC	= /usr/bin/arm-linux-gnueabihf-gcc
# CC  = /usr/bin/aarch64-linux-gnu-gcc
#--------------------------------------------------------------------------------------

GIT=git

ARCH	= armv8
PROJ	= astrokit
FICH	= astroreel
VERS	= 3.0
HOME	= /home/stef
RM 		= rm -f
EPHA	= libephe.a

RPWD		= ${HOME}/${GIT}/${PROJ}
RSRC		= ${RPWD}/src
REPH		= ${RPWD}/eph
RLIB		= ${RPWD}/lib

# la cible est ici EXEC pour make sans argument
EXEC	= ${FICH}.${VERS}

INCS 	= -I. -I${RSRC} -I${RPWD}/inc -I${RPWD}/lib -I${RPWD}/lib/lirc -I${REPH}
LIBS	= -L${RPWD}/lib/lirc -L${RPWD}/lib -L/usr/lib -L${REPH} -lpthread -lm -lrt

DEBUG		= -g -Wall -O2 -Wno-unused-result -Wno-misleading-indentation -Wno-format-overflow
CFLAGS 	= $(DEBUG) $(INCS) -Winline -pipe -Os -fPIC

SRC			=	src/arguments.c src/astro.c src/calculs.c src/cat.c src/config.c src/gpio.c src/i2c.c src/ir.c src/stat.c
OBJ			= $(SRC:.c=.o)

LEPH		= ${RLIB}/${EPHA}
EPHS		= $(REPH)/*.c
SRCS		= ${RSRC}/*.c
OBJS		= $(SRCS:.c=.o)
OBJSUP	= $(LEPH)

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(EXEC): $(OBJ)
	$(CC) $(OBJ) $(INCS) $(LEPH) -o ${EXEC} $(LIBS)

$(LEPH)	:
	cd ${REPH} ; make clean ; make

.PHONY: all all-before all-after clean clean-custom

clean: clean-custom
	${RM} $(OBJS) 
	${RM} $(EXEC)

cleanlib:
	${RM} ${REPH}/*.o
	${RM} ${LEPH}

cleanall: clean cleanlib

lib: $(LEPH)

all: all-before cleanall $(LEPH) $(EXEC) all-after


# -----------------------------------------------------------------------------
# help -help - help - help - help - help - help - help - help - help - help -  
# -----------------------------------------------------------------------------

help:
	@echo "make           : compiler les sources (a besoin de ${LEPH} : make lib si besoin)"
	@echo "make lib       : creer la librairie ${LEPH}"
	@echo "make all       : make lib puis make"
	@echo "make clean     : nettoie repertoire $RSRC et $(EXEC)"
	@echo "make cleanlib  : nettoie repertoire ${REPH} et ${LEPH}"
	@echo "make cleanall  : make clean et cleanall"

# ================================================================================
#
# Fin Makefile - Fin Makefile - Fin Makefile - Fin Makefile - Fin Makefile -  
#
# ================================================================================


