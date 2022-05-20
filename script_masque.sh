#!/bin/bash
# Usage   : <$0> <masque>
# Example : <$0> 21,26,19,13

function old () {
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
  
}

binaire=/usr/local/bin/gpio
mot0="26,19,13,6"
mot1="6,5,7,11"
fpwm=500

# masque="0,1,2,3"

[ $# -gt 1 ]&& { echo "$0 <masque (exemple : 21,16,19,13) facultatif>" ; exit 1 ; }
[ ! -f $binaire]&& { echo "$binaire absent" ; exit 1 ; }

m0=0
m1=1
m2=2
m3=3

nbarg=$#

if [ $nbarg -eq 1 ];then
 m0=$(echo $1 | cut -d',' -f1)
 m1=$(echo $1 | cut -d',' -f2)
 m2=$(echo $1 | cut -d',' -f3)
 m3=$(echo $1 | cut -d',' -f4)
fi

declare -a masque=("$m0" "$m1" "$m2" "$m3")
declare -a combin=("" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "")

echo ${masque[0]}
echo ${masque[1]}
echo ${masque[2]}
echo ${masque[3]}

for i in $(seq 0 3) ; do
 for j in $(seq 0 3) ; do
  for k in $(seq 0 3) ; do
   for l in $(seq 0 3) ; do
    t=0
    [ $j -ne $i ] &&[ $k -ne $i ]&&[ $k -ne $j ]&&[ $l -ne $i ]&&[ $l -ne $j ]&&[ $l -ne $k ]&&{ t=1 ; }
#    [ $t -eq 0 ]&& { echo $t $i $j $k $l ${masque[$i]} ; }
    [ $t -eq 1 ]&& {
#     $binaire 21,26,19,13 6,5,7,11 $i $j $k $l 1 0 8 1 0 1000 & ;
      mas="${masque[$i]},${masque[$j]},${masque[$k]},${masque[$l]}"
      echo "masque=$mas"
      cmd="$binaire $mot0 $mot1 $mas 1 0 128 1 0 $fpwm 1>/dev/null 2>/dev/null &"
      echo $cmd
      $binaire $mot0 $mot1 $mas 1 0 64 1 0 $fpwm 1>/dev/null 2>/dev/null &
      pid=$!
      echo "0/1 ?"
      # read rep
      sleep 5
      rep=0
      [ $rep -eq 1 ]&& {
        echo "masque retenu = $mas"
        kill $pid
        exit 1
      }
      kill $pid
    }
   done
  done
 done
done

# FIN FICHIER

