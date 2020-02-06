# Project: monture altazimutale controlee par port gpio raspaberry pi
# Makefile created by stephane Gravois - 2014
# Reel6 : inclut la fonctionnalite infrarouge 

GIT=git

ARCH	  = armv8
PROJET	= astrokit
FICHIER	= astroreel
VERSION	= 3.0

HOME	= /home/stef

# TARGET est une sauvegarde du rep output/target de buildroot 
# (plus leger que l'ensemble du rep output car output/build est lourd ..
# c'est la qu'on va chercher les fichiers d'inclusion et les librairies

TARGET= ${HOME}/${GIT}/${PROJET}
SRC		= ${TARGET}/src
EPE		= ${TARGET}/eph
EXEC	= ${FICHIER}

# Le compilateur est cense etre dans le PATH (dans /usr/bin normalement)
# Il peut resulter sous debian d un apt-get install --buil-essential <architecture>

# CC	= gcc
# CC	= armv6-gcc
# CC	= /usr/local/bin/arm-linux-gnueabihf-gcc
# CC	= /usr/bin/arm-linux-gnueabihf-gcc
CC  = /usr/bin/aarch64-linux-gnu-gcc

INCS 	= -I. -I${SRC} -I${TARGET}/inc -I${TARGET}/lib -I${TARGET}/lib/lirc -I${EPE}
LIBS	= -L${TARGET}/lib/lirc -L${TARGET}/lib -L/usr/lib -L${EPE} -lpthread -lm -lrt

DEBUG	= -g -Wall -O2 -Wno-unused-result -Wno-misleading-indentation -Wno-format-overflow

OBJ	= astro.o arguments.o config.o calculs.o gpio.o cat.o i2c.o ir.o libephe.a 
LINKOBJ	= astro.o arguments.o config.o calculs.o gpio.o cat.o i2c.o ir.o

CFLAGS 	= $(DEBUG) $(INCS) -Winline -pipe -Os -fPIC

RM 	= rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(EXEC) all-after

clean: clean-custom
	${RM} $(OBJ) $(EXEC)

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(EXEC): $(LINKOBJ)
	$(CC) $(LINKOBJ) $(INCS) -o ${EXEC} $(LIBS)



