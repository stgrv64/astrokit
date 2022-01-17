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
# * make va d'abord rechercher LEPH EXEC pui all puis ..
# -------------------------------------------------------------------------------------
#  HISTORIQUE MODIFICATION
#  2022 : pouvoir compiler le binaire en plus de l archive 
#         le nom de larchive est EPHA
#         le nom de le executable est EPHX
#-------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------
# CC	= gcc
# CC	= armv6-gcc
# CC	=	/usr/bin/arm-linux-gnueabi-gcc
#CC	= /usr/bin/arm-linux-gnueabihf-gcc
#CC  = /usr/bin/aarch64-linux-gnu-gcc
#--------------------------------------------------------------------------------------

GIT=git

ARCH	= armv8
PROJ	= astrokit
FICH	= astroreel
VERS	= 3.0
HOME	= /home/stef
RM 	= rm -f

EPHA	= libephe.a
EPHX	= solarsystem

RPWD	= ${HOME}/${GIT}/${PROJ}
RSRC	= ${RPWD}/src
REPH	= ${RPWD}/eph
RLIB	= ${RPWD}/lib32
LEPH	= ${RLIB}/${EPHA}
# la cible est ici EXEC pour make sans argument
EXEC	= ${RLIB}/${EPHX}

SRC = altaz.c angles.c annuab.c constel.c deflec.c deltat.c diurab.c \
diurpx.c dms.c epsiln.c fk4fk5.c kepler.c kfiles.c lightt.c lonlat.c \
nutate.c precess.c refrac.c rplanet.c rstar.c sidrlt.c sun.c domoon.c \
trnsit.c vearth.c zatan2.c gplan.c mer404.c ven404.c ear404.c mar404.c jup404.c \
sat404.c ura404.c nep404.c plu404.c mlr404.c mlat404.c ss.c

OBJ	= $(SRC:.c=.o)

WFLAGS	= -Wno-unused-result
WFLAGS2	= -Wno-misleading-indentation -Wno-format-overflow
DEBUG		= -g -Wall -O2 -Winline -pipe -fPIC 
CFLAGS 	= $(DEBUG) $(WFLAGS)

INCS 	= -I.
LIBS	= -L. -lm 

.c.o:
#	@echo $(CC) -c $(CFLAGS) $(INCS) $< -o $@
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $(INCS) $< -o $@

# ---------------------------------------------
# Construction de l executable : 
# (necessite un main valide dans ss.c)
# ---------------------------------------------

$(EXEC): $(OBJ)
#	@echo ar -r $(EXEC) $(OBJ)
#	ar -r $(EXEC) $(OBJ)
#	chmod 755 $(EXEC)
	@echo $(CC) $(OBJ) $(INCS) -o ${EXEC} $(LIBS)
	$(CC) $(OBJ) $(INCS) -o ${EXEC} $(LIBS)

# ---------------------------------------------
# Construction de l archive : 
# ---------------------------------------------

$(LEPH): $(OBJ)
#	@echo ar -r $(LEPH) $(OBJ)
	ar -r $(LEPH) $(OBJ)
	chmod 755 $(LEPH)
#	@echo $(CC) $(OBJ) $(INCS) -o ${LEPH} $(LIBS)
#	$(CC) $(OBJ) $(INCS) -o ${LEPH} $(LIBS)

#	ar -r  $(LEPH) $@
.PHONY: all all-before all-after clean clean-custom

all: all-before $(EXEC) $(LEPH) all-after

$(EPHA):$(LEPH)
$(EPHX):$(EXEC)

clean: clean-custom
	${RM} $(OBJ) $(EXEC) $(LEPH)

# ================================================================================
#
# Fin Makefile - Fin Makefile - Fin Makefile - Fin Makefile - Fin Makefile -  
#
# ================================================================================

