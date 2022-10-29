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

# -------------------------------------------------------------------------------------
#  HISTORIQUE MODIFICATION
#  2022 : * pouvoir compiler le binaire en plus de l archive 
#            - le nom de larchive est EPHA
#            - le nom de le executable est EPHX
#         * ajout librairie ncurses pour acces fonction getch (flux input keyboard)
#         * ajout clause -Wno-unused-variable
#
# jan 2022    * prise en compte CCHOST et CCTARG
# avril 2022  * ajout compilation pour lcd1602 (git matrix adapter)
# juin 2022   * ajout lcd.c dans SRC
# 21 juin 2022   * ajout pid.c / .hdans SRC
#--------------------------------------------------------------------------------------
# CC	= gcc
# CC	= armv6-gcc
# CC	=	/usr/bin/arm-linux-gnueabi-gcc
# pour rpi3 cortex A53 :
# CC	= /usr/bin/arm-linux-gnueabihf-gcc # pour rpi3 cortex A53
# CC  = /usr/bin/aarch64-linux-gnu-gcc
#--------------------------------------------------------------------------------------

CCTARG=/usr/bin/arm-linux-gnueabihf-gcc
CCHOST=gcc

GIT=git

FICH	= astro_test
VERS	= $(shell date +%B.%Y)

RM 		= rm -f
EPHA	= libephe.a
LCDA  = libfahw.a
EPHX	= solarsystem
RPWD	= ${PWD}
RSRC	= ${RPWD}
REPH	= ${RPWD}/eph
RLCD	= ${RPWD}/lcd

TLIB	= ${RPWD}/libtarget
HLIB  = ${RPWD}/libhost

NCURSESLIB	= ${RPWD}/libncurses

#--------------------------------------------------------------------------------------
# la cible est ici EXEC pour make sans argument
#--------------------------------------------------------------------------------------

EXEC	= ${FICH}.${VERS}

#--------------------------------------------------------------------------------------
# Pour les cibles suivantes, il faut de commenter les main dans les fichiers .c
# et renommer le main dans le fichier astro.c (entree prog astrokit)
#--------------------------------------------------------------------------------------

EXECIR	= ir
EXECGP	= gpio

#--------------------------------------------------------------------------------------

ifeq ($(M),host)
	CC=$(CCHOST)
	RLIB=$(HLIB)
else
	CC=$(CCTARG)
	RLIB=$(TLIB)
endif

INCS 	= -I. -I${RPWD}/inc -I${RLIB} -I${RLIB}/lirc -I${REPH} -I$(NCURSESLIB) -I${RPWD}/lcd/lib/includes
LIBS	= -L${RLIB} -lpthread -lm -lrt -llirc_client -lncurses -ltinfo -lephe -lfahw

# ========================================
# Prevoir la compilation avec GCC 
# ========================================

# make M=host host => compile pour le host
# make M=targ targ => compile pour le host


ifeq ($(CC),gcc)
  EXEC = ${FICH}.${VERS}.host
else
	EXEC = ${FICH}.${VERS}.targ
endif

DEBUG	= -g -Wall -O2 -Wno-unused-result -Wno-misleading-indentation -Wno-format-overflow -Wno-unused-variable -Wno-unused-but-set-variable
CFLAGS 	= $(DEBUG) $(INCS) -Winline -pipe -Os -fPIC -static -pthread

SRC	= astro_test0.c astro_test1.c  

OBJ	= $(SRC:.c=.o)

SRCIR	= src/ir.c src/gpio.c
OBJIR	= $(SRCIR:.c=.o)

LLCD  = ${RLIB}/${LCDA}
LEPH	= ${RLIB}/${EPHA}
XEPH	= ${RLIB}/${EPHX}

EPHS	= $(REPH)/*.c
SRCS	= ${RSRC}/*.c
OBJS	= $(SRCS:.c=.o)
OBJSUP	= $(LEPH)

.c.o:
	@echo @$(CC) -c $(CFLAGS) $< -o $@
	@$(CC) -c $(CFLAGS) $< -o $@

$(EXEC): $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o ${EXEC}

$(EXECIR): $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o ${EXECIR}

$(EXECGP): $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o ${EXECGP}

$(LEPH)	:
	cd ${REPH} ; make CC=$(CC) $(EPHA)

$(LLCD)	:
	cd ${RLCD} ; make CC=$(CC) lib
	cp ${RLCD}/lib/${LCDA} ${RLIB}
	
$(XEPH)	:
	cd ${REPH} ; make CC=$(CC) $(EPHX)

.PHONY: all all-before all-after clean clean-custom

clean: clean-custom
	${RM} $(OBJS) 
	${RM} $(EXEC)
	
cleanlcd:
	${RM} ${RLCD}/*.a
	${RM} ${RLCD}/*.o
	${RM} ${RLIB}/${LCDA}
	
cleanlib:
	${RM} ${REPH}/*.o
	${RM} ${LEPH}

cleansolarsystem:
	${RM} ${REPH}/*.o
	${RM} ${XEPH}	

cleanall: clean cleanlib cleansolarsystem cleanlcd

lib: $(LEPH)

lcd: $(LLCD)

$(EPHX): $(XEPH)

all: all-before $(LLCD) $(LEPH) $(EXEC) all-after

host: cleanall lib $(EXEC)

targ: cleanall lib $(EXEC)

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
	@echo "make M=host ...  : memes directives avec CC=gcc    ; nom final .host"
	@echo "make M=targ ...  : memes directives avec CC=arm-xx ; nom final .targ"

# ================================================================================
#
# Fin Makefile - Fin Makefile - Fin Makefile - Fin Makefile - Fin Makefile -  
#
# ================================================================================


