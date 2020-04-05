#!/bin/bash

echo "usage : $0 <chaine a remplacer> <chaine de remplacement>" 
echo "usage : $0 <comp>"  
echo "usage : $0 <remp>" 

COMP=0;           [ $# -eq 1 ]&&[ $1 == "comp" ] && { COMP=1 ; }
REMP=0;           [ $# -eq 1 ]&&[ $1 == "remp" ] && { REMP=1 ; }
OLD="" ; NEW="" ; [ $# -eq 2 ] && { OLD=$1 ; NEW=$2 ; }
[ $# -eq 0 ] && exit 1 ;

if [ $COMP -eq 1 ]
then
 for fic in `ls *.new`
 do
  fic2=`echo $fic | sed -e 's/.new//g'`
  echo "------------------ diff $fic $fic2 ------------------"
  diff $fic $fic2
 done
fi
if [ $REMP -eq 1 ]
then
 for fic in `ls *.new`
 do
  fic2=`echo $fic | sed -e 's/.new//g'`
  echo "------------------ mv $fic $fic2 ------------------"
  mv $fic $fic2 2>/dev/null
 done
fi
if [ $# -eq 2 ]
then
 for fic in `ls`
 do
  nbl=`cat $fic | grep $OLD | wc -l`
  cmd="cat $fic | sed -e 's/$OLD/$NEW/g' > $fic.new"
  [ $nbl -ne 0 ]&& { eval $cmd ;} 
 done
fi
