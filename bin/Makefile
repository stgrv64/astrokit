# Project: monture altazimutale controlee par port gpio raspaberry pi
# Makefile created by stephane Gravois - 2014
# Reel6 : inclut la fonctionnalite infrarouge 

ARCH	= armv7
PROJET	= projet_astrokit
FICHIER	= astro
VERSION	= reel30

HOME	= /home/stef

# TARGET est une sauvegarde du rep output/target de buildroot 
# (plus leger que l'ensemble du rep output car output/build est lourd ..
# c'est la qu'on va chercher les fichiers d'inclusion et les librairies

TARGET	= ${HOME}/${PROJET}/kernel
SRC	= ${HOME}/${PROJET}/src
EPE	= ${SRC}/ephemerides

# Le compilateur est cense etre dans le PATH (dans /usr/bin normalement)
# Il peut resulter sous debian d un apt-get install --buil-essential <architecture>

#CC	= gcc
#CC	= armv6-gcc
#CC	= /usr/local/bin/arm-linux-gnueabihf-gcc
CC	= /usr/bin/arm-linux-gnueabihf-gcc
#CC	= /home/stef/poky/rpi3-build/tmp/work/raspberrypi3-poky-linux-gnueabi/linux-raspberrypi/1_4.19.97+gitAUTOINC+c8fbe9c63c-r0/recipe-sysroot-native/usr/libexec/arm-poky-linux-gnueabi/gcc/arm-poky-linux-gnueabi/9.2.0/arm-poky-linux-gnueabi-gcc

# INCS 	= -I${TARGET}/include -I${TARGET}/include/lirc -I${SRC} -I${EPE}
INCS 	= -I${TARGET}/lib -I${TARGET}/lib/lirc -I${SRC} -I${EPE}
LIBS	= -L${TARGET}/lib -L/usr/lib -L${EPE} -lpthread -lm -lrt -llirc_client -lsss

#INCS 	= -I/usr/include/lirc -I${SRC} -I${EPE}
#LIBS	= -L/usr/lib -L${EPE} -lpthread -lm -lrt -llirc_client -lsss

DEBUG	= -g -Wall -O2 -Wno-unused-result -Wno-misleading-indentation -Wno-format-overflow
#DEBUG	= -g -Wall -O1

OBJ	= ${FICHIER}.o arguments.o config.o calculs.o gpio.o cat.o i2c.o ir.o 
LINKOBJ	= ${FICHIER}.o arguments.o config.o calculs.o gpio.o cat.o i2c.o ir.o 

BIN	= ${FICHIER}${VERSION}
CFLAGS 	= $(DEBUG) $(INCS) -Winline -pipe -Os -fPIC

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

