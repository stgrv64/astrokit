#!/bin/bash
# 2022 - stgrv
# Be careful with links => use readlink
#===============================================

fic_commandes=package.commandes.necessaires
fic_out=${fic_commandes}.paquets.a.ajouter.a.yocto

declare -a USE_PATHS=("/bin" "/usr/bin" "/sbin" "/usr/sbin" "/usr/local/bin")

# On sauvegarde le chemin
path=$(echo $PATH)

[ ! -f $fic_commandes ]&& { echo "$fic_commandes absent" ; exit 0 ; }

for com in $(cat $fic_commandes)
do
 
 # echo "commande = $com"
 che=$(which $com)

 if [ $? -gt 0 ]; then
  echo "which $com (KO) => installer le paquet ou faire un find /" ;
  continue 
 fi
  
 # echo "che=$che"
 
 dpkg -S $che 2>/dev/null
 
 if [ $? -gt 0 ]; then
  echo "dpkg -S $che (KO) => essai avec autre chemin en supprimant dirname du PATH" ;
  res=$?
  res=0
  for Path in ${USE_PATHS[@]}
  do
  	dpkg -S $Path/$com 2>/dev/null
    [ $? -eq 0 ]&& { echo "dpkg -S $Path/$com (OK)" ; dpkg -S $Path/$com ; res=1 ; }
  done
  # Aucune commande trouvee : locate 
	[ $res -eq 0 ]&& {
		for alt in $(locate -r /${com}$ | xargs ls | xargs readlink); do
		  dpkg -S $alt 2>/dev/null
		  [ $? -eq 0 ]&& { 
		  	echo "dpkg -S $alt (OK)" ; 
		  	dpkg -S $alt  | tee -a $fic_out ; 
		  	res=1 ; 
		  }
		done
	}
 else
  echo "dpkg -S $che (OK)" ; 
 	dpkg -S $che | tee -a $fic_out
 fi
 
 
done














