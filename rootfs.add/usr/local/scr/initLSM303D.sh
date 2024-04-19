#!/bin/bash

# test opérateur décalage

VAL=0x80 # 128
RES=$(($VAL << 1)) # decalage a gauche => 128 << 1 = 256
echo $RES

COMPOSANT="LSM303D"
ADRESS="1d"
AH=0x1d    # adresse officielle du composant

# Definition des registres 

REG_CTRL1=0x20
REG_CTRL2=0x21
REG_CTRL5=0x24
REG_CTRL6=0x25
REG_CTRL7=0x26

REG_OUT_ACC_X_L=0x28
REG_OUT_ACC_X_H=0x29
REG_OUT_ACC_Y_L=0x2a
REG_OUT_ACC_Y_H=0x2b
REG_OUT_ACC_Z_L=0x2c
REG_OUT_ACC_Z_H=0x2d

REG_OUT_MAG_X_L=0x08
REG_OUT_MAG_X_H=0x09
REG_OUT_MAG_Y_L=0x0a
REG_OUT_MAG_Y_H=0x0b
REG_OUT_MAG_Z_L=0x0c
REG_OUT_MAG_Z_H=0x0d

NOM=`basename $0`
DATE=`date +%Y%m%d.%H%M`
LOG=/root/SCRIPTS/LOG
mkdir $LOG 2>/dev/null
/usr/sbin/i2cdetect -y 1 > $LOG/$NOM.$DATE

[ `cat $LOG/$NOM.$DATE | grep $ADRESS | wc -l` -ne 1 ] && { echo "$COMPOSANT non trouver", exit 1 ;  }
[ `cat $LOG/$NOM.$DATE | grep $ADRESS | wc -l` -eq 1 ] && { echo "$COMPOSANT  trouver", exit 1 ;  }

# Initialisation des registres pour activer l'accelerometre, la boussole
# et mettre la boussole en mode haute resolution

i2cset -y 1 $AH $REG_CTRL1 0x57
i2cset -y 1 $AH $REG_CTRL2 0x00
i2cset -y 1 $AH $REG_CTRL5 0x64
i2cset -y 1 $AH $REG_CTRL6 0x00
i2cset -y 1 $AH $REG_CTRL7 0x00

i=0

while true 
do

 axl=`i2cget -y 1 $AH $REG_OUT_ACC_X_L`
 axh=`i2cget -y 1 $AH $REG_OUT_ACC_X_H`
 ayl=`i2cget -y 1 $AH $REG_OUT_ACC_Y_L`
 ayh=`i2cget -y 1 $AH $REG_OUT_ACC_Y_H`
 azl=`i2cget -y 1 $AH $REG_OUT_ACC_Z_L`
 azh=`i2cget -y 1 $AH $REG_OUT_ACC_Z_H`

 mxl=`i2cget -y 1 $AH $REG_OUT_MAG_X_L`
 mxh=`i2cget -y 1 $AH $REG_OUT_MAG_X_H`
 myl=`i2cget -y 1 $AH $REG_OUT_MAG_Y_L`
 myh=`i2cget -y 1 $AH $REG_OUT_MAG_Y_H`
 mzl=`i2cget -y 1 $AH $REG_OUT_MAG_Z_L`
 mzh=`i2cget -y 1 $AH $REG_OUT_MAG_Z_H`
 
 echo "---- acc et mag -------" 
 $PWD/initLSM303D "$axl" "$ayl" "$azl" "$axh" "$ayh" "$azh" 
 $PWD/initLSM303D "$mxl" "$myl" "$mzl" "$mxh" "$myh" "$mzh"
 
done


