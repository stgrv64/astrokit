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
#--------------------------------------------------------------------------------------
CC	= gcc
# CC	= armv6-gcc
# CC	=	/usr/bin/arm-linux-gnueabi-gcc
# pour rpi3 cortex A53 :
# CC	= /usr/bin/arm-linux-gnueabihf-gcc # pour rpi3 cortex A53
# CC  = /usr/bin/aarch64-linux-gnu-gcc
#--------------------------------------------------------------------------------------

GIT=git

ARCH	= armv8
PROJ	= astrokit
FICH	= astrokit
VERS	= $(shell date +%B.%Y)
HOME	= /home/stef
RM 	= rm -f
EPHA	= libephe.a
EPHX	= solarsystem

RPWD	= ${HOME}/${GIT}/${PROJ}
RSRC	= ${RPWD}/src
REPH	= ${RPWD}/eph

RLIB	= ${RPWD}/lib32
HLIB  = ${RPWD}/lib

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
EXECGPIO= gpios

#--------------------------------------------------------------------------------------

INCS 	= -I. -I${RSRC} -I${RPWD}/inc -I${RLIB} -I${RLIB}/lirc -I${REPH} -I$(NCURSESLIB) 
LIBS	= -L${RLIB} -L${REPH} -L$(NCURSESLIB) -lpthread -lm -lrt -llirc_client -lncurses -ltinfo

# ========================================
# Prevoir la compilation avec GCC 
# ========================================

ifeq ($(CC),gcc)
  LIBS = -L${HLIB} -L${REPH} -lpthread -lm -lrt -llirc_client -lncurses -ltinfo
  EXEC = ${FICH}.${VERS}.host
endif

DEBUG	= -g -Wall -O2 -Wno-unused-result -Wno-misleading-indentation -Wno-format-overflow
CFLAGS 	= $(DEBUG) $(INCS) -Winline -pipe -Os -fPIC

SRC	= \
src/arguments.c src/astro.c src/calculs.c src/cat.c \
src/config.c src/gpio.c src/i2c.c src/ir.c src/stat.c \
src/keyboard.c

OBJ	= $(SRC:.c=.o)

SRCIR	= src/ir.c src/gpio.c
OBJIR	= $(SRCIR:.c=.o)

LEPH	= ${RLIB}/${EPHA}
XEPH	= ${RLIB}/${EPHX}

EPHS	= $(REPH)/*.c
SRCS	= ${RSRC}/*.c
OBJS	= $(SRCS:.c=.o)
OBJSUP	= $(LEPH)

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(EXEC): $(OBJ)
	$(CC) $(OBJ) $(INCS) $(LEPH) -o ${EXEC} $(LIBS)

$(EXECIR): $(OBJ)
	$(CC) $(OBJ) $(INCS) $(LEPH) -o ${EXECIR} $(LIBS)

$(GPIO): $(OBJ)
	$(CC) $(OBJ) $(INCS) $(LEPH) -o ${GPIO} $(LIBS)

$(LEPH)	:
	cd ${REPH} ; make CC=$(CC) $(EPHA)

$(XEPH)	:
	cd ${REPH} ; make CC=$(CC) $(EPHX)

.PHONY: all all-before all-after clean clean-custom

clean: clean-custom
	${RM} $(OBJS) 
	${RM} $(EXEC)

cleanlib:
	${RM} ${REPH}/*.o
	${RM} ${LEPH}

cleansolarsystem:
	${RM} ${REPH}/*.o
	${RM} ${XEPH}	

cleanall: clean cleanlib cleansolarsystem

lib: $(LEPH)
$(EPHX): $(XEPH)

all: all-before $(LEPH) $(EXEC) all-after


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


