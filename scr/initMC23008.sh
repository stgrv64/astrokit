#!/bin/bash

[ $# -eq 0 ]&& { echo "USAGE : $0 <input=0,output=1> <value for output(fac)>" ; exit 1 ; }
[ $# -eq 2 ]&& { OLATv=$2 ; }

MODE="OUTPUT"
HADR=0x20
IODIR=0x00
GPIO=0x09
OLAT=0x0A
IODIRv=0x00

[ $1 -eq 0 ]&& { IODIRv=0xFF ; MODE="INPUT" ; }

printf "Verif detection du MCP23008 avec hardware adress = $HADR\n# "
printf "/usr/sbin/i2cdetect -y 1\n"
/usr/sbin/i2cdetect -y 1

printf "Configuration du port GPA en $MODE ( $IODIR=$IODIRv )\n# "
echo "i2cset -y 1 $HADR $IODIR $IODIRv"

i2cset -y 1 $HADR $IODIR $IODIRv

if [ $1 -eq 0 ];then
 printf "Lecture du port OLAT = $OLAT : \n# i2cget -y 1 $HADR $OLAT\n"
 i2cget -y 1 $HADR $OLAT
 printf "Lecture du port GPIO = $GPIO : \n# i2cget -y 1 $HADR $GPIO\n"
 i2cget -y 1 $HADR $GPIO
else
 printf "Eciture sur le port GPA : \n# i2cset -y 1 $HADR $OLAT $OLATv\n"
 i2cset -y 1 $HADR $OLAT $OLATv
fi
