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
# 2022 octobre | * prise en compte virgule dans exposition (image non astro)
#                * prise en compte des images arx et ARW => copie dans un sous rep dedie
#                * 
# ------------------------------------------------------------------

############################################
#
# Script for Siril 1.0
# July 2020
# (C) Cyril Richard
# Preprocessing v1.0
#
########### PREPROCESSING SCRIPT ###########
#
# Script for color camera preprocessing
#
# Needs 4 sets of RAW images in the working
# directory, within 4 directories:
#   biases/
#   flats/
#   darks/
#   lights/
#
############################################

MAX_img_a_traiter=500

declare -a i_ded=( \
 "exposure"        \
 "mean"      \
 "size"   \
 "iso"     \
 "deduction" \
)
# non utilise : c_ded
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

declare -A c_siz=( \
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

for_arw=arw
for_ARW=ARW

cmd_find_jpg="find . -maxdepth 1 -type f \( -name \"*.${for_JPG}\" -or -name \"*.${for_jpg}\" \)"
cmd_find_arw="find . -maxdepth 1 -type f \( -name \"*.${for_ARW}\" -or -name \"*.${for_arw}\" \)"

bin_exi=/usr/bin/exiv2
bin_ide=/usr/bin/identify

nom_scr=$(basename $0 | cut -d'.' -f1)
inf_dat=$(date +%Y%m%d%H%M%S)
fic_log="${nom_scr}.${inf_dat}.log"

# fic_bin_exi=/tmp/${nom_scr}.${inf_dat}.exiv2.output

# non utilise 
# cmd_mea="$bin_ide -format \"%[mean]\" $ima | cut -d'.' -f1"

valeur_moyenne_flats_admissible_min=100
valeur_moyenne_darks_admissible_max=2
valeur_moyenne_biase_admissible_max=2
valeur_moyenne_light_admissible_min=2
valeur_moyenne_light_admissible_max=150

val_flat_min=$valeur_moyenne_flats_admissible_min
val_dark_max=$valeur_moyenne_darks_admissible_max
val_bias_max=$valeur_moyenne_biase_admissible_max
val_ligh_min=$valeur_moyenne_light_admissible_min
val_ligh_max=$valeur_moyenne_light_admissible_max

# Needs 4 sets of RAW images in the working
# directory, within 4 directories:
#   biases/
#   flats/
#   darks/
#   lights/

rep_bias=biases
rep_lig=lights
rep_fla=flats
rep_dar=darks

# ------------------------------------------------------------------

function Verification_Presence_Rep() {

  pattern=dark
  
  if [ ! -d "*${pattern}*" ]
  then
    find .. -type d -name "*${pattern}*"
  fi
}
# ------------------------------------------------------------------
function Creation_Rep_Et_Copie() {

  c_path_dir=$1
  c_path_fic=$2
  c_ima_ded=$3

  ima_nom=$(basename $c_path_fic)
  ima_loc=$(dirname $c_path_fic)
  ima_pre=$(echo $ima_nom | cut -d'.' -f1)
  # ajout 03 oct 2022
  ima_arw="${ima_loc}/${ima_pre}.arw"
  ima_ARW="${ima_loc}/${ima_pre}.ARW"    
      
  # Creation du repertoire  si inexistant
  
  [ ! -d $c_path_dir ]&& {
    #printf "mkdir %s 2>/dev/null\n" $c_path_dir ;
    mkdir $c_path_dir 2>/dev/null ;
    [ $? -ne 0 ]&& { printf "!! attention sortie en erreur de mkdir $c_path_dir 2>/dev/null (KO)\n" ; }
  }
    
  # Copie du fichier dans le repertoire de deplacement
  
  [ -d $c_path_dir ]&& {
    #printf "cp -p %s %s\n" $c_path_fic $c_path_dir ;
    cp -p $c_path_fic $c_path_dir ;
    [ $? -ne 0 ]&& { printf "!! attention sortie en erreur de cp -p $c_path_fic $c_path_dir\n" ; }
    
    # ajout 03 oct 2022 : test existence image .arw de meme prefixe que le jpg
    
    [ -f $ima_arw ]&& {
      [ ! -d $c_path_dir/arw ]&& {
        #printf "mkdir %s 2>/dev/null\n" $c_path_dir ;
        mkdir $c_path_dir/arw 2>/dev/null ;
        [ $? -ne 0 ]&& { printf "!! attention sortie en erreur de mkdir $c_path_dir/arw 2>/dev/null (KO)\n" ; }
      }
      # ajout oct 2023
      cp -pf $ima_arw $c_ima_ded

      mv $ima_arw $c_path_dir/arw ;

    }
    # ajout 03 oct 2022 : test existence image .ARW de meme prefixe que le jpg
    
    [ -f $ima_ARW ]&& {
      [ ! -d $c_path_dir/ARW ]&& {
        #printf "mkdir %s 2>/dev/null\n" $c_path_dir ;
        mkdir $c_path_dir/ARW 2>/dev/null ;
        [ $? -ne 0 ]&& { printf "!! attention sortie en erreur de mkdir $c_path_dir/ARW 2>/dev/null (KO)\n" ; }
      }
      # ajout oct 2023
      cp -pf $ima_ARW $c_ima_ded

      mv $ima_ARW $c_path_dir/ARW ;
    }      
  }
}
# ------------------------------------------------------------------
# Cette fonction a pour but de deuire le type d image astro : 
# bias / dark / flat / offset

function Image_Deduction() {

  # printf "%s %s %s %s\n" "$ima_exp" "$ima_mea" "$ima_siz" "$ima_iso"
  
  ima_ded="undefined"
  
  # non utilise : c_ded
#  c_ded[${i_ded[0]}]=$ima_exp
#  c_ded[${i_ded[1]}]=$ima_mea
#  c_ded[${i_ded[3]}]=$ima_siz
#  c_ded[${i_ded[4]}]=$ima_iso 
  
  # Si exposition contient une virgule : valeur exposition non conforme a une image astro (exemple : 0.4 seconde)
  # On ne peut avoir que des valeurs comme 1/8000 ou bien 15 ou bien 30 par convention 
  # Ceci est valable pour les darks / bias / light ou expo est soir tres courte soit superiere a la seconde
  # Pour les FLATS, on peur avoir une expo comme 60e (1 60eme de seconde) mais on ne peut pas discriminer avec
  # une image normale
  
  presence=$(echo $ima_exp | awk 'BEGIN { FS = "." } ; { print NF}')
  
  if [ ${presence} -gt 1 ]
  then
    pre_e=1
  else
    pre_e=0
  fi

  presence=$(echo $ima_exp | awk 'BEGIN { FS = "e" } ; { print NF}')
  
  if [ ${presence} -gt 1 ]
  then
    pre_e=1
  else
    pre_e=0
  fi

  presence=$(echo $ima_exp | awk 'BEGIN { FS = "s" } ; { print NF}')
  
  if [ ${presence} -gt 1 ]
  then
    pre_s=1
  else
    pre_s=0
  fi
  
  #printf "ima_exp %s pres_e %s pres_s %s \n" "$ima_exp" "$pre_e" "$pre_s"
  
  # Deduction des LIGHTS : poses longues > 1s 
  
  [ $ima_mea -gt $val_ligh_min ] && [ $ima_mea -lt $val_ligh_max ] && [ $pre_s -eq 1 ] && { 
    ima_ded=$rep_lig ; 
#    c_ded[${i_ded[5]}]=$ima_ded ;
    return ; 
  }
  # Deduction des DARKS : poses longues > 1s 
  
  [ $ima_mea -lt $val_dark_max ] && [ $pre_s -eq 1 ] && { 
    ima_ded=$rep_dar ; 
#    c_ded[${i_ded[5]}]=$ima_ded ;
    return ; 
  }
  
  # Deduction des FLATS : poses courtes < 1s ( ecran a flat ou de jour) 
  
  [ $ima_mea -gt $val_flat_min ] && [ $pre_e -eq 1 ] && { 
    ima_ded=$rep_fla ; 
#    c_ded[${i_ded[5]}]=$ima_ded ;
    return ; 
  }
  # Deduction des BIASES : poses longues < 1s ( par definition : la plus courte possible)
  
  [ $ima_mea -lt $val_bias_max ] && [ $pre_e -eq 1 ] && { 
    ima_ded=$rep_bias ; 
#    c_ded[${i_ded[5]}]=$ima_ded ;
    return ; 
  }
  
#  c_ded[${i_ded[5]}]=$ima_ded
  
  return
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

  # ima_mean=$($bin_ide -verbose $1 | grep --binary-file=text -A10 Overall | grep --binary-file=text mean | awk '{print $2}'| cut -d'.' -f1) ;
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

  [ ! -d $rep_bias ] && {  mkdir $rep_bias 2>/dev/null ; }
  [ ! -d $rep_lig ] && {   mkdir $rep_lig 2>/dev/null ; }
  [ ! -d $rep_fla ] && {   mkdir $rep_fla 2>/dev/null ; }
  [ ! -d $rep_dar ] && {   mkdir $rep_dar 2>/dev/null ; }
}
# ------------------------------------------------------------------
function Tri() {
 
  # FIXME oct 2023
  # On recherche tous les fichiers jpg ou JPG (necessaires) 
  # le fichier ARW ou arw doit correspondre
  max_img_a_traiter=0

  for ima in $(eval $cmd_find_jpg)
  do
    max_img_a_traiter=$((max_img_a_traiter+1))
    [ $max_img_a_traiter -eq $MAX_img_a_traiter ]&& { break ; }

    printf "\n --------------- traitement %s ---------------\n\r\n" $ima >> $fic_log

    ima_nom=$(basename $ima)
    ima_loc=$(dirname $ima)
    ima_pre=$(echo $ima_nom | cut -d'.' -f1)

    ima_ded="ras"

    fic_bin_exi=/tmp/${ima_nom}.${inf_dat}.exiv2.output

    $bin_exi $ima > $fic_bin_exi

    cmd="cat $fic_bin_exi | grep --binary-file=text -i \"timestamp\" | sed -e 's/://g' | awk '{print \$(NF-1)}'"   # ima_tim
    echo $cmd >> tee -a $fic_log
    ima_tim=$(eval $cmd)
    [ $(echo $ima_tim | egrep -i "inpu|matc" | wc -l) -gt 0 ]&& {
      echo $cmd >>  $fic_log
      cat $fic_bin_exi >> $fic_log
    }

    cmd="cat $fic_bin_exi | grep --binary-file=text -i \"Image size\"    | awk '{ print \$(NF-2)"x"\$(NF) ; }'" # ima_siz
    echo $cmd >> $fic_log
    ima_siz=$(eval $cmd)
    [ $(echo $ima_siz | egrep -i "inpu|matc" | wc -l) -gt 0 ]&& {
      echo $cmd >>  $fic_log
      cat $fic_bin_exi >> $fic_log
    }

    cmd="cat $fic_bin_exi | grep --binary-file=text -i \"Image size\"    | awk '{ print \$(NF-2) ; }'"         # ima_wid
    echo $cmd >> $fic_log
    ima_wid=$(eval $cmd)
    [ $(echo $ima_wid | egrep -i "inpu|matc" | wc -l) -gt 0 ]&& {
      echo $cmd >>  $fic_log
      cat $fic_bin_exi >> $fic_log
    }

    cmd="cat $fic_bin_exi | grep --binary-file=text -i \"Image size\"    | awk '{ print \$(NF) ; }'"           # ima_hei
    echo $cmd >> $fic_log
    ima_hei=$(eval $cmd)
    [ $(echo $ima_hei | egrep -i "inpu|matc" | wc -l) -gt 0 ]&& {
      echo $cmd >>  $fic_log
      cat $fic_bin_exi >> $fic_log
    }

    cmd="cat $fic_bin_exi | grep --binary-file=text -i \"Exposure time\" | awk '{ print \$(NF-1) ; }'"        # ima_exp
    echo $cmd >> $fic_log
    ima_exp=$(eval $cmd)
    [ $(echo $ima_exp | egrep -i "inpu|matc" | wc -l) -gt 0 ]&& {
      echo $cmd >>  $fic_log
      cat $fic_bin_exi >> $fic_log
    }

    cmd="cat $fic_bin_exi | grep --binary-file=text -i \"ISO speed\"     | awk '{ print \$(NF) ; }'"           # ima_iso
    echo $cmd >> $fic_log
    ima_iso=$(eval $cmd)    
    [ $(echo $ima_iso | egrep -i "inpu|matc" | wc -l) -gt 0 ]&& {
      echo $cmd >>  $fic_log
      cat $fic_bin_exi >> $fic_log
    }

     # printf "nom %-10s WID %-10s HEI %-10s tim %-10s siz %-10s exp %-10s iso %-10s : ded %-10s\n" \
     #       $ima_nom $ima_wid $ima_hei $ima_tim $ima_siz $ima_exp $ima_iso $ima_ded  | tee -a $fic_log
 
#    Image_Mean $f
#    cmd="$bin_ide -verbose $ima | grep --binary-file=text -A10 Overall | grep --binary-file=text mean | awk '{print \$2}'| cut -d'.' -f1"

    cmd="$bin_ide -format \"%[mean]\" $ima | cut -d'.' -f1"
    
    ima_mea=$(eval $cmd)
    ima_mea=$((ima_mea/256))
    
#    c_ded[${i_ded[5]}]= champs a deduire
    
    ima_siz=${c_siz[$ima_siz]}
 #   ima_exp=$(echo $ima_exp | sed -e 's/\//g' | printf "%-10s")
 
    ima_expo=$ima_exp
    
    ima_exp=$(echo $ima_expo | awk '{
     i=index($1,"/");
     if(i==0){
      out=sprintf("%ss",$1);
      print out;}
     else{
      s=substr($1,i+1,length($1));
     out=sprintf("%se",s);
     print out; 
    }}')
    
    
    Image_Deduction
    
    
    c_path_out="${ima_tim}-${ima_ded}-${ima_siz}-${ima_iso}-${ima_exp}"
    c_path_sav="${ima_tim}-save"
    
#    printf "%-10s %-10s %-10s %-10s %-10s %-10s %-30s (%s) %s\n" $ima_nom $ima_siz $ima_exp $ima_iso $ima_mea $ima_ded $c_path_out $ima_loc $ima_pre
    rm $0.infos
    printf "%-10s %-10s %-10s %-10s %-10s %-10s => %-10s\n" $ima_nom $ima_siz $ima_exp $ima_expo $ima_iso $ima_mea $ima_ded | tee -a $0.infos
    
    # Creation et populate des repertoires de sortie
    
    Creation_Rep_Et_Copie $c_path_out $ima $ima_ded
    Creation_Rep_Et_Copie $c_path_sav $ima $ima_ded
    
    # Look if dark folder is present
    
    # echo "image suivante ?"
    # read s_response
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

CreerRepsDeBase

Tri

exit


# =================================================================
# Fin script - fin script - Fin script - fin script - Fin script
# =================================================================









