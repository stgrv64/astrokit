#!/bin/bash

gpio reset ;

# ======================================

GPIO_4="0 1 2 3"
GPIO_8="0 1 2 3 4 5 6 7 8"
GPIO_16="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15"
GPIO_4059_Kab="15"

# INITIALISATION ============================

for i in 0 1 2 3 4 5;do
 echo "Initialisation GPIO $i"
 gpio mode $i out
 gpio write $i 1 
done

# INCREMENTER ============================

for l in 1 0; do
for k in 1 0; do
for j in 1 0; do
for i in 1 0; do
  echo "Ecriture  GPIO $l $k $j $i"
  #gpio write 5 0
 
  gpio write 0 $i
  gpio write 1 $j
  gpio write 2 $k
  gpio write 3 $l
 
  #gpio write 5 0
  #gpio write 5 1
  #sleep 1

done
done
done
done

# FIN SCRIPT ==============================