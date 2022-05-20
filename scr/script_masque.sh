#!/bin/bash
# Usage   : <$0> <masque>
# Example : <$0> 21,26,19,13

[ $# -ne 1 ]&& { echo "$0 <masque (exemple : 21,16,19,13)>" ; exit 1 ; }

m0=$(echo $1 | cut -d',' -f1)
m1=$(echo $1 | cut -d',' -f2)
m2=$(echo $1 | cut -d',' -f3)
m3=$(echo $1 | cut -d',' -f4)

declare -a masque=("$m0" "$m1" "$m2" "$m3")
declare -a combin=("" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "")

#echo ${masque[0]}
#echo ${masque[1]}
#echo ${masque[2]}
#echo ${masque[3]}

for i in $(seq 0 3) ; do 
 for j in $(seq 0 3) ; do  
  for k in $(seq 0 3) ; do  
   for l in $(seq 0 3) ; do
    t=0 
    [ $j -ne $i ]&&[ $k -ne $i ]&&[ $k -ne $j ]&&[ $l -ne $i ]&&[ $l -ne $j ]&&[ $l -ne $k ]&&{ t=1 ; }
#    [ $t -eq 0 ]&& { echo $t $i $j $k $l ${masque[$i]} ; }
    [ $t -eq 1 ]&& { echo $t $i $j $k $l ${masque[$i]} ${masque[$j]} ${masque[$k]} ${masque[$l]} ; }
   done ; 
  done ; 
 done ; 
done
  

