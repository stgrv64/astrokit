#!/bin/bash

gpio reset ;

# ======================================

GPIO_4="0 1 2 3"
GPIO_8="0 1 2 3 4 5 6 7 8"
GPIO_16="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15"
GPIO_4059_Kab="15"

# INITIALISATION ============================

for i in $GPIO_16 ;do
 echo "Initialisation GPIO $i"
 gpio mode $i out
 gpio write $i 1 
done

i2cdetect -y 1

# FIN SCRIPT ==============================
