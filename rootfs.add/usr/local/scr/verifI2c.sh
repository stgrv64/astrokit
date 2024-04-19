#!/bin/bash

i2c0=20
i2c1=21

I2C[0]="0x20"     # port i2c du- MCP23017 avec les Ai à 0
I2C[1]="0x21"     # port i2c du MCP23017 avec les Ai à 1 

I2Ci[0]="0x00"   # port i2c du MCP23017 avec les Ai à 0 en mode init
I2Ci[1]="0x01"   # port i2c du MCP23017 avec les Ai à 1 en mode init

I2Co[0]="0x14"   # port i2c du MCP23017 avec les Ai à 0 en mode output
I2Co[1]="0x15"   # port i2c du MCP23017 avec les Ai à 1 en mode output

I2C_LOG=/var/log/I2C
NOM="`basename $0`"
DAT="`date +%Y%m%d.%H%M`"
FIC="$NOM.$DAT"

mkdir $I2C_LOG 2>/dev/null
mkdir $I2C_LOG/OLD 2>/dev/null

i2cdetect -y 1 > $I2C_LOG/$FIC

[ `cat $I2C_LOG/$FIC | grep $i2c0 | grep $i2c1  | wc -l` -eq 0 ] && { echo "pas de MCP23017 detectes" ; exit 1 ; }

# initialisation 

i2cset -y 1 0x20 0x00 0x00
i2cset -y 1 0x20 0x01 0x00

i2cset -y 1 0x21 0x00 0x00
i2cset -y 1 0x21 0x01 0x00

# ecriture avec un sleep de 1 entre chaque

i2cset -y 1 0x20 0x14 0
i2cset -y 1 0x20 0x15 0
i2cset -y 1 0x21 0x14 0
i2cset -y 1 0x21 0x15 0

i2cset -y 1 0x20 0x14 0xff

sleep 1

i2cset -y 1 0x20 0x15 0xff

sleep 1

i2cset -y 1 0x21 0x14 0xff

sleep 1

i2cset -y 1 0x21 0x15 0xff

