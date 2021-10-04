#!/bin/bash

REV=1 # rev = 0 pour premiere raspi, rev=1 pour suivantes

HDWADR="68"
COMPOSANT="ds1307"
PATH_DEVICE="/sys/class/i2c-adapter/i2c-${REV}/new_device"
MODULES="rtc-ds1307 i2c-bcm2708 i2c-dev"

EXPR=/usr/bin/expr
MODPROBE=/sbin/modprobe
LSMOD=/sbin/lsmod
I2CDETECT=/usr/sbin/i2cdetect
HWCLOCK=/sbin/hwclock

if [ $# -lt 1 ]
then 
 echo "Usage : $0 <2=mettre a l'heure la RTC       via HEURE SYS>" 
 echo "Usage : $0 <1=mettre a l'heure la HEURE SYS via le RTC>"
 echo "Usage : $0 <0=montrer heure du module RTC>"
 exit 1
fi

NBA=$#
CHOIX=$1

# ===================================================================================
echo "Verification de existence des binaires utiles pour ce script"
# ===================================================================================

[ ! -f $MODPROBE ]&&  { echo "$MODPROBE inexistant" ; exit 1 ; }
[ ! -f $LSMOD ]&&     { echo "$LSMOD inexistant" ; exit 1 ; }
[ ! -f $I2CDETECT ]&& { echo "$I2CDETECT inexistant : installer i2ctools" ; exit 1 ; }
[ ! -f $HWCLOCK ]&&   { echo "$HWCLOCK inexistant " ; exit 1 ; }
[ ! -f $EXPR ]&&      { echo "$EXPR inexistant" ; exit 1 ; }

# ===================================================================================
echo "Verification du chargement des modules"
# ===================================================================================

for module in $MODULES
do
  module_expr_reg=`echo $module | tr -s '_' '.' | tr -s '-' '.'`
  
  if [ `$LSMOD | grep $module_expr_reg | wc -l` -ne 1 ] 
  then
   echo "$module non charger dans le noyau : tentative de chargement" 
   
   $MODPROBE $module
   
   if [ `$LSMOD | grep $module_expr_reg | wc -l` -ne 1 ] 
   then
     echo "$module toujours non charger dans le noyau : modprobe en echec : exit"
     exit 1 
   fi
   echo "OK : $module charge dans le noyau" 
  fi 
done

# ===================================================================================
echo "Verification de la detection du composant"
# ===================================================================================

  if [ `$I2CDETECT -y $REV | grep $HDWADR | wc -l` -ne 1 ] 
  then
   echo "$HDWADR non detecte par $I2CDETECT : peut etre est il utilise en UU : verification" 
   
   j=`$EXPR $HDWADR % 10`
   k=`$EXPR $HDWADR - $j`
   l="${k}:"
   
   if [ `$I2CDETECT -y $REV | grep $l | grep UU | wc -l` -ne 1 ] 
   then
     echo "pas de UU detecte non plus"
     exit 1
   else
     echo "bien detecte en UU : `$I2CDETECT -y $REV | grep $l | grep UU`"
   fi
  fi 

# ===================================================================================
echo "Charger le composant dans /sys et lire ou ecrire"
# ===================================================================================

echo "Ecriture dans $PATH_DEVICE (si erreur deja ecrit via /etc/rc.local au demarrage)"
echo $COMPOSANT 0x${HDWADR} > $PATH_DEVICE
 
case "$CHOIX" in
 0) $HWCLOCK -r ;;
 1) $HWCLOCK -s ;;
 2) $HWCLOCK -w ;;
esac


