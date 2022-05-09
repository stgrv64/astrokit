#!/bin/bash
# -------------------------------------------------------------------------------------------------
# mai 2022
# -------------------------------------------------------------------------------------------------
# Ce script permet de rechercher les repertoires old
# et de copier les fichiers trouves (dans les .../old) ainsi que le meme fichier
# du repertoire en dessous (d ou provient la sauvegarde dans old) dans un repertoire dedie.
# La recherche est sauvegarde dans un repertoire dedie (/media/stef/datas1 par defaut)
# ou est preserve la structure arborescente
# -------------------------------------------------------------------------------------------------

nom=old
dest=/media/stef/datas1
echo "$dest :"
ls -l $dest

for d0 in $(sudo find . -type d -name $nom); do
  sudo mkdir -p $dest/$d0
  d1=$(echo $d0 | sed -e 's/\/old//g')
  dest0=$dest/$d0
  dest1=$dest/$d1
  #d1=$(echo $d | sed -e 's/\.\///g' | tr -s '/' '.')
  #d2=$(echo $d | sed -e 's/.old//g' | sed -e 's/\.\///g' | tr -s '/' '.')
#  echo "--------------------------------------------- $d -----------------------------------------------"
  # printf "%-30s %-30s %-30s %-30s\n" $d $d0 $d1 $d2
  # cp -prf 
  for f in $(ls $d0) ; do
    #ls -l $d/$f
    f1=$(basename $f)
    # printf "%-30s %-30s %-30s %-30s %s\n" $d $d0 $d1 $d2 $f
    # printf "%-30s %-30s %s\n" $d0 $d1 $f
    printf "%-27s %-24s %-48s %-48s %s\n" $d0 $d1 $dest0 $dest1 $f
    sudo cp -pf $d0/$f $dest0
    sudo cp -pf $d1/$f $dest1
  done
done

tree $dest
