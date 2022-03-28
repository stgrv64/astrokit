#!/bin/bash
# ========================================================
# Astrokit - 2015
# Script qui recherche depuis la racine tous les repertoires
# de nom OLD et recherche dans ces repertoires les fichiers
# ayant ete a priori modifier par astrokit* en extension .ori
# Ces fichiers modifies sont copiers dans ce rep courant
# en recreant l arborescence depuis la racine 
# ========================================================

DIR=/root/INSTALL
echo "Recherche des repertoires OLD dans le rootfs"
find / -type d -name OLD > $DIR/reps_old
for fic in `cat $DIR/reps_old`
do
 
 dir=`dirname $fic`
 newdir=${DIR}${dir}
 
 echo "Creation rep $newdir si inexistant"
 mkdir -p $newdir
 echo "Recherche des fichiers a conserver dans $dir"
 
 for ori in `find $fic -type f -name "*.ori"`
 do 
  new=`echo $ori | sed -e 's/\/OLD//g' | sed -e 's/.ori//g'`
  echo " ... Fichier $new trouve dans $dir"
  echo " ... Copie du fichier $new dans rep $newdir"
  cp -p $new $newdir
 done
done

find $DIR -type f -ls
