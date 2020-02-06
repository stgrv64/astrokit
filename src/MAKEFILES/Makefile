# Project: monture altazimutale controlee par port gpio raspaberry pi
# Makefile created by stephane Gravois - 2014
# Reel6 : inclut la fonctionnalite infrarouge 

ARCH	= armv7
PROJET	= astrokit
FICHIER	= astro
VERSION	= reel13

HOME	= /home/save

# TARGET est une sauvegarde du rep output/target de buildroot 
# (plus leger que l'ensemble du rep output car output/build est lourd ..
# c'est la qu'on va chercher les fichiers d'inclusion et les librairies

OUTPUT	= ${HOME}/host/output/
TARGET	= ${OUTPUT}/${ARCH}/target
SRC	= ${HOME}/${PROJET}/src/${VERSION}
EPE	= ${HOME}/${PROJET}/src/ephemerides

# Le compilateur est cense etre dans le PATH (dans /usr/bin normalement)
# Il peut resulter sous debian d un apt-get install --buil-essential <architecture>

CC	= gcc

#INCS 	= -I${TARGET}/usr/include -I${TARGET}/usr/include/lirc -I${SRC}
#LIBS	= -L${TARGET}/usr/lib -lpthread -lm -lrt -llirc_client

INCS 	= -I/usr/include/lirc -I${SRC} -I${EPE}
LIBS	= -L/usr/lib -L${EPE} -lpthread -lm -lrt -llirc_client

DEBUG	= -g -Wall -O2
OBJ	= ${FICHIER}.o arguments.o config.o calculs.o gpio.o cat.o i2c.o ir.o ss.o
LINKOBJ	= ${FICHIER}.o arguments.o config.o calculs.o gpio.o cat.o i2c.o ir.o ss.o

BIN	= ${FICHIER}${VERSION}
CFLAGS 	= $(DEBUG) $(INCS) -Winline -pipe -Os

RM 	= rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before ${FICHIER}${VERSION} all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) $(INCS) -o ${FICHIER}${VERSION} $(LIBS)

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@
