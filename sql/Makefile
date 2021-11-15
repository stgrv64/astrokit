# Project: MIRZA - Aquitaine Electronique
# Makefile created by stephane Gravois - 2018

HOME	= /home/stef
BLDROOT	= buildroot-2018.02

DIR_BUILDROOT	= ${HOME}/${BLDROOT}
DIR_TOOLCHAIN	= ${DIR_BUILDROOT}/output/host/bin

#CC	= /usr/local/bin/arm-linux-gnueabihf-gcc  # 
#CC	= /usr/bin/arm-linux-gnueabihf-gcc        # compilation croisée avec compilateur générique (exemple LINARO, etc..)
#CC	= /usr/bin/arm-poky-linux-gnueabi-gcc     # compilation cible avec compilateur installé sur la cible (impossible avec buildroot=)
#CC	= ${DIR_TOOLCHAIN}/arm-linux-gcc          # compilation croisee avec compilateur interne buildroot (attention : interpreteur ucLibc)
CC	= /usr/bin/gcc                            # compilation hôte

PROJET		= mirza
PROGRAMME	= sqlite3
MAIN_FIC	= main

HOMEDIR	= ${HOME}/${PROJET}/${PROGRAMME}
HOMESRC	= ${HOME}/${PROJET}/${PROGRAMME}/src
HOMELIB	= ${HOME}/${PROJET}/${PROGRAMME}/lib
HOMEINC	= ${HOME}/${PROJET}/${PROGRAMME}/inc

TARGET	= ${HOME}/${PROJET}/${PROGRAMME}/target
SRC	= ${HOME}/${PROJET}/${PROGRAMME}/src


INCS 	= -I${HOMESRC} -I${HOMEINC}
LIBS	= -L${HOMELIB} -lm

INCS 	= -I/usr/include
LIBS	= -lm -lsqlite3

DEBUG	= -g -O2

OBJ	= ${MAIN_FIC}.o tuning.o sql.o 

BIN	= ${MAIN_FIC}
CFLAGS 	= $(DEBUG) $(INCS) -Winline -pipe -Os

RM 	= rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before ${BIN} all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) $(INCS) -o ${BIN} $(LIBS)

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

