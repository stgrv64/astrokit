#!/bin/bash
#
# Rappels sur les tailles possibles en JPG sur le sony A7S
#
# LARGE _16.9_2768_1560.JPG      : Image size      : 2768 x 1560
# LARGE _3.2_ 2768_1848.JPG      : Image size      : 2768 x 1848
# MEDIUM_16_9_2128_1200.JPG      : Image size      : 2128 x 1200
# MEDIUM_3.2_ 2128_1416.JPG      : Image size      : 2128 x 1416
# SMALL _16_9_1376_776.JPG       : Image size      : 1376 x 776
# SMALL _3_2_ 1376_920.JPG       : Image size      : 1376 x 920
#
# ------------------------------------------------------------------
# 
# Ce script analyse dans le repertoire courant ou dans l'arboresence
# les images produites par une soiree d observation et trie les images automatiquement
# en fonction de leur caracteristique :
# - ISO  (sensiblity)
# - EXP  (exposure)
# - MEA  (moyenne de la valeur des pixels)
# - SIZ  (taille)
#
# Les binaires exiv2 et identify sont necessaires .
#
# Ce scipt permet de :
# - trier les photos grace a identify (image-magick) et exiv2 (exiv2) en fonction de la date
# - detecter grace a la valeur moyenne des pixels (identify) et du temps de pose si une image est de type darks, offsets/biases, brutes/lights, ou flats
# - creer les repertoires dont a besoin siril (biases, darks, flats, lights,) brutes, darks, flats et offsets  
# 
# Prerequis : 
#  - les images doivent etre en doublons : JPG et ARW (raw)
#  - en effet (juin 2022) aucun binaire n'a ete trouve pour retrouver la valeur moyenne des images raw
# ------------------------------------------------------------------

declare -a i_ded=( \
 "exposure"        \
 "mean"      \
 "size"   \
 "iso"     \
 "deduction" \
)

declare -a c_ded=( \
 ["${i_ded[0]}"]=""  \
 ["${i_ded[1]}"]="" \
 ["${i_ded[2]}"]=""  \
 ["${i_ded[3]}"]=""  \
 ["${i_ded[4]}"]=""  \
) ;

declare -a i_siz=( \
  "4240x2384" \
  "4240x2832" \
  "2768x1560" \
  "2768x1848" \
  "2128x1200" \
  "2128x1416" \
  "1376x776"  \
  "1376x920"  \ 
) ;

declare -A c_siz=( \*
 ["${i_siz[0]}"]="X169" \
 ["${i_siz[1]}"]="X032" \
 ["${i_siz[2]}"]="L169" \
 ["${i_siz[3]}"]="L032" \
 ["${i_siz[4]}"]="M169" \
 ["${i_siz[5]}"]="M032" \
 ["${i_siz[6]}"]="S169" \
 ["${i_siz[7]}"]="S032" \
) ;

for_JPG=JPG
for_jpg=jpg

fir_arw=arw
for_ARW=ARW

cmd_find_jpg="find . -type f \( -name \"*.${for_JPG}\" -or -name \"*.${for_jpg}\" \)"
cmd_find_arw="find . -type f \( -name \"*.${for_ARW}\" -or -name \"*.${for_arw}\" \)"

bin_exi=/usr/bin/exiv2
bin_ide=/usr/bin/identify

# non utilise 
cmd_mea="$bin_ide -format \"%[mean]\" $ima | cut -d'.' -f1"

valeur_moyenne_flats_admissible_min=150
valeur_moyenne_darks_admissible_max=2
valeur_moyenne_biase_admissible_max=2
valeur_moyenne_light_admissible_min=2

vmf=$valeur_moyenne_flats_admissible_min
vmd=$valeur_moyenne_darks_admissible_max
vmb=$valeur_moyenne_biase_admissible_max
vml=$valeur_moyenne_light_admissible_min

# ------------------------------------------------------------------
function Creation_Rep_Et_Copie() {

  c_path_dir=$1
  c_path_fic=$2
  
  # Creation du repertoire  si inexistant
  
  [ ! -d $c_path_dir ]&& {
    #printf "mkdir %s 2>/dev/null\n" $c_path_dir ;
    mkdir $c_path_dir 2>/dev/null ;
    [ $? -ne 0 ]&& {
     printf "!! attention sortie en erreur de mkdir $c_path_dir 2>/dev/null (KO)\n" ;
    }
  }
    
  # Copie du fichier dans le repertoire de deplacement
  
  [ -d $c_path_dir ]&& {
    #printf "cp -p %s %s\n" $c_path_fic $c_path_dir ;
    cp -p $c_path_fic $c_path_dir ;
    [ $? -ne 0 ]&& {
     printf "!! attention sortie en erreur de cp -p $c_path_fic $c_path_dir>/dev/null (KO)\n" ;
    }      
  }
}
# ------------------------------------------------------------------
function Image_Deduction() {

  ima_ded="und"
  
  c_ded[${i_ded[0]}]=$ima_exp
  c_ded[${i_ded[1]}]=$ima_mea
  c_ded[${i_ded[3]}]=$ima_siz
  c_ded[${i_ded[4]}]=$ima_iso 
  
  # Si a valeur moyenne est en dessous de zero et exposition < 1s : offset
  
  b=$(($ima_exp)) # exemple : a=1/8000 b=0
  
  [ $ima_mea -lt $vmb ] && [ $b -eq 0 ] &&           { ima_ded="bias" ; }
  [ $ima_mea -lt $vmb ] && [ $b -gt 0 ] &&           { ima_ded="dark" ; }
  [ $ima_mea -gt $vmf ] &&                           { ima_ded="flat" ; }
  [ $ima_mea -gt $vml ] && [ $ima_mea -le $vmf ] &&  { ima_ded="ligh" ; }
  
  c_ded[${i_ded[5]}]=$ima_ded
}

# ------------------------------------------------------------------
function Images_Parcourir() {
  for f in $(eval $cmd_find_jpg)
  do
   echo $f
  done

  for f in $(eval $cmd_find_arw)
  do
   echo $f
  done
}
# ------------------------------------------------------------------
function Image_Mean() {

  # ima_mean=$($bin_ide -verbose $1 | grep -A10 Overall | grep mean | awk '{print $2}'| cut -d'.' -f1) ;
  ima_mean=$($bin_ide -format "%[mean]" $f | cut -d'.' -f1) ;
  echo "ima mean : %s" "$ima_mean"
}
# ------------------------------------------------------------------
function Hachage_Affichage_Tab() {

  Index=$1
  Table=$2
    
  for c in ${i_siz[@]}
  do
   printf "%s : %s\n" "$c" "${c_siz[$c]}"
  done
}
# ------------------------------------------------------------------
function CreerRepsDeBase() {

  [ ! -d biases ] && {  mkdir biases 2>/dev/null ; }
  [ ! -d lights ] && {  mkdir lights 2>/dev/null ; }
  [ ! -d flats ] && {   mkdir flats 2>/dev/null ; }
  [ ! -d darks ] && {   mkdir darks 2>/dev/null ; }
  [ ! -d brutes ] && {  mkdir brutes 2>/dev/null ; }
  [ ! -d offsets ] && { mkdir offsets 2>/dev/null ; }
}
# ------------------------------------------------------------------
function Tri() {
 
  for ima in $(eval $cmd_find_jpg)
  do
    ima_nom=$(basename $ima)
    ima_loc=$(dirname $ima)
    ima_pre=$(echo $ima_nom | cut -d'.' -f1)
    
    ima_tim=$($bin_exi $ima | grep -i "timestamp" | sed -e 's/://g' | awk '{print $(NF-1)}' )
    ima_siz=$($bin_exi $ima | grep -i "Image size"    | awk '{ print $(NF-2)"x"$(NF) ; }') 
    ima_wid=$($bin_exi $ima | grep -i "Image size"    | awk '{ print $(NF-2) ; }') 
    ima_hei=$($bin_exi $ima | grep -i "Image size"    | awk '{ print $(NF) ; }') 
    ima_exp=$($bin_exi $ima | grep -i "Exposure time" | awk '{ print $(NF-1) ; }' ) 
    ima_iso=$($bin_exi $ima | grep -i "ISO speed"     | awk '{ print $(NF) ; }') 
    
#    Image_Mean $f
#    cmd="$bin_ide -verbose $ima | grep -A10 Overall | grep mean | awk '{print \$2}'| cut -d'.' -f1"
    cmd="$bin_ide -format \"%[mean]\" $ima | cut -d'.' -f1"
    
    ima_mea=$(eval $cmd)
    ima_mea=$((ima_mea/256))
    
#    c_ded[${i_ded[5]}]= champs a deduire
    
    Image_Deduction
    
    ima_siz=${c_siz[$ima_siz]}
 #   ima_exp=$(echo $ima_exp | sed -e 's/\//g' | printf "%-10s")
    ima_exp=$(echo $ima_exp | awk '{
     i=index($1,"/");
     if(i==0){
      out=sprintf("%ssec",$1);
      print out;}
     else{
      s=substr($1,i+1,length($1));
     out=sprintf("%se",s);
     print out; 
    }}')
    c_path_out="${ima_tim}-${ima_ded}-${ima_siz}-${ima_iso}-${ima_exp}"
    c_path_sav="${ima_tim}-save"
    
    printf "%-15s %-10s %-10s %-10s %-10s %-10s %-30s (%s) %s\n" $ima_nom $ima_siz $ima_exp $ima_iso $ima_mea $ima_ded $c_path_out $ima_loc $ima_pre
   

    Creation_Rep_Et_Copie $c_path_out $ima
    Creation_Rep_Et_Copie $c_path_sav $ima
            
  done
}
# =================================================================

ici=$PWD
nom_rep_courant=$(basename $ici)
path_rep_courant=$(basename $ici)
path_rep_parent=$(basename $path_rep_courant)

nom_shell=$(basename $0)
out_means=${nom_shell}_${nom_rep_courant}_means.txt
simu=1
resolution=0

echo "ici=$ici"
echo "nom_rep_courant=$nom_rep_courant"
echo "nom_shell=$nom_shell"

ima_wid_base=0
ima_hei_base=0
ima_base=""
ima_tmp="/tmp/${nom_shell}.$(date +%Y%m%d)"

incr=0

# Images_Parcourir
# Hachage_Affichage_Tab $i_siz $c_siz*

Tri

exit


# =================================================================
# Fin script - fin script - Fin script - fin script - Fin script
# =================================================================









