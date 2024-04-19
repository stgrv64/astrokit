#!/bin/bash

echo "verif detection des MCP23017 apparaissent en ports 20 et 21 normalement"

/usr/sbin/i2cdetect  -y 1

echo "sleep 3 secondes"
sleep 3

echo "configuration des ports GPA et GPB en sortie : mode 0x00 et Ox01"

i2cset -y 1 0x20 0x00 0x00
i2cset -y 1 0x20 0x01 0x00

i2cset -y 1 0x21 0x00 0x00
i2cset -y 1 0x21 0x01 0x00

echo "ecriture sur les ports GPA et GPB : mode 0x14 et Ox15"
echo " => allmage de toutes les leds pour verif (0xff) sur chaque port"
echo " => delai de une seconde sur chaque port de chaque MCP"

i2cset -y 1 0x20 0x14 0xff
sleep 1
i2cset -y 1 0x20 0x15 0xff
sleep 1
i2cset -y 1 0x21 0x14 0xff
sleep 1
i2cset -y 1 0x21 0x15 0xff

echo "on met 256  comme diviseur sur CD5059"
sleep 3

i2cset -y 1 0x20 0x14 0xff
i2cset -y 1 0x20 0x15 0xff

i2cset -y 1 0x21 0x14 0xff
i2cset -y 1 0x21 0x15 0xff

echo "Mettre les ports KbKc a 1 apres un sleep de 5 secondes"
sleep 5

gpio mode 0 output
gpio mode 1 output

gpio write 0 0 
gpio write 0 0 

sleep 2

gpio write 0 1
gpio write 1 1 
