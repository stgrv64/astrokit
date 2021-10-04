#!/bin/bash
# ======================================================================
function tempo() {
 t=$1
 i=0; while true; do i=$((i+1)) ; [ $i -eq $t ]&& { break ; } ; done ;
}
# ======================================================================

i2cdetect -y 1
i2cset -y 1 0x20 0x00 0x00
i=0
while true 
do
 i=$((i+1))
 if [ $i -eq 256 ] 
 then
  break
 fi
 echo $i
 i2cset -y 1 0x20 0x09 $i 
done
j=0
while true
do
 j=$((j+1))
 if [ $j -eq 10 ]
 then
  break
 fi
 tempo 1000
 i2cset -y 1 0x20 0x09 170
 tempo 1000
 i2cset -y 1 0x20 0x09 85 
done
 i2cset -y 1 0x20 0x09 0 
exit 0
