#!/bin/bash

A=`basename $0`
B=`which $A`
WRK=`dirname $B`
NBA=$#
RWK=`dirname $0`
NBA=`echo $#`
DAT=`date +%Y%m%d.%H`

TIMEOUT=5
OBEXFT="Obex File Transfer"

# fichiers sur la raspi -----------------------------------------------

HAD=""
RFC=rfcomm
L2C=l2ping
LOG=$WRK/LOG/bluetooth.${DAT}
CON=$WRK/CON
REP=/etc/bluetooth
RFF=${REP}/${RFC}.conf
RFT=${RFF}.template

mkdir $WRK/LOG 2>/dev/null
mkdir $WRK/CON 2>/dev/null

# binaires et démons

BLD=/etc/init.d/bluetooth

HCT=/usr/bin/hcitool
BLA=/usr/bin/bluetooth-agent
RFB=/usr/bin/$RFC
L2P=/usr/bin/$L2C
SDP=/usr/bin/sdptool
FTP=/usr/bin/obexftp

# fichiers sur le remote portable -------------------------------------

# sensor logger

SLO_ACC="/SensorLogger/acceleration"
SLO_MAG="/SensorLogger/magnetic"
SLO_ORI="/SensorLogger/orientation"

# sensor track

STR_ACC="/sensortrack/accelerometer"
STR_MAG="/sensortrack/magnetic"
STR_ORI="/sensortrack/orientation"

# sensor data logger

SDL=/SensorDataLogger

# functions -----------------------------------------------------------

function EXIT { echo "\---------------------------" ; cat $LOG ; exit $1 ; }
function TESTF { [ ! -f $1 ]&&{ echo "$1 inexistant" >> $LOG ; EXIT 1 ;} }
function TRACE { echo "$1" >> $LOG ; }
function RETKO { RET=`echo $?` ; echo $RET ;[ $RET -ne 0 ] && { echo "$1" >> $LOG ; EXIT 1 ; } }

# on a besoin des fichiers suivants pour travailler sinon installer les paquets

TESTF $RFB
TESTF $BLA
TESTF $BLD
TESTF $HCT
TESTF $RFF
TESTF $L2P
TESTF $SDP
TESTF $FTP

[ $# -eq 1 ]&&[ "$1" == "--help" ]||[ "$1" == "-h" ]&&{
  echo "Usage : $0 <channel(fac)>"
  echo "Channel predefini :"
  cat $RFF
  EXIT 0
}

# LE PORTABLE EST JOIGNABLE ET N'A JAMAIS ETE CONFIGURE ---------------

if [ $NBA -eq 0 ];then
 [ `lsusb | grep -i bluetooth | wc -l` -eq 0 ]&& { LOG "Pas de dongle bluetooth" ; EXIT 1 ; }
 [ `lsusb | grep -i bluetooth | wc -l` -ne 0 ]&& { lsusb | grep -i bluetooth ; }
 $BLD status 2>/dev/null
 RET=`echo $?`
 [ $RET -ne 0 ]&&[ $RET -eq 3 ]&& $BLD start 2>/dev/null
 [ $RET -ne 0 ]&&[ $RET -ne 3 ]&& { tee -a "$BLD status failed" $LOG ; EXIT 1 ; }
 $HCT scan > $RWK/scan.${DAT}
 cat $RWK/scan.${DAT}
 if [ `cat $RWK/scan.${DAT} | grep -v Scanning | wc -l` -lt 1 ];then
   TRACE "Aucun remote dispos : tentative connexion sur 1er remote de conf"
 fi
fi

# ON SAIT QUE LE PORTABLE EST JOIGNABLE OU DEJA CONFIGURE -------------

if [ $NBA -eq 1 ];then
 CHA=$1
 $RFB show $CHA >>$LOG 2>&1
 RET=`echo $?`
 [ $RET -eq 0 ] && { TRACE "Channel $CHA non disponible regarder $LOG" ; }
 # On extrait les donnees depuis le fichier de conf
 HAD=`cat $RFF | grep device | grep ":" | cut -d';' -f1 | cut -d' ' -f2`
 TRACE "hardware adress trouve = $HAD"
 # On teste que le portable est joignable avec un ping du protocole L2CAP
 $L2P -c 2 -t 3 $HAD >>$LOG 2>&1
 RET=`echo $?`
 [ $RET -ne 0 ] && { TRACE "Impossible de pinger $HAD" ; EXIT 1 ; }
 TRACE "ping l2cap sur $HAD ok"
 $RFB release $CHA >>$LOG 2>&1
 # le release met parfois un peu de temps
 sleep 3
 $RFB show $CHA >>$LOG 2>&1
 RET=`echo $?`
 [ $RET -ne 1 ] && { TRACE "Channel $CHA impossible a liberer : regarder $LOG" ; EXIT 1 ; }
 TRACE "release channel $CHA ok"
 # Lancement de bluetooth-agent pour changer la clé
 $BLA 0000 &
 $RFB connect $CHA >>$LOG 2>&1 & 
 sleep 5
 
fi

# ON SAIT QUE LE PORTABLE EST CONFIGURE ET CONNECTE -------------
# ON COMMENCE LES TRANSFERTS

if [ $NBA -eq 1 ]||[ $NBA -gt 2 ]&&[ "$2" == "transfert" ]
then
 CHA=$1

 # On extrait les donnees depuis le fichier de conf (a modifier pour tenir compte de plusieurs entrees)

 HAD=`cat $RFF | grep device | grep ":" | cut -d';' -f1 | cut -d' ' -f2`
 TRACE "hardware adress trouve = $HAD"

 # On verifie avec sdptool que le portable a la fonction "OBEX Object Push"
 # Pour eviter d'interroger pour rien on regarde si le fichier de conf existe deja
 
 if [ ! -f $CON/sdptool.${HAD} ]
 then
  $SDP browse $HAD > $CON/sdptool.${HAD} 2>&1
 fi
 
 TESTF $CON/sdptool.${HAD}

 [ `cat ${CON}/sdptool.${HAD} | grep -i "$OBEXFT" | wc -l` -eq 0 ]&&{ TRACE "$HAD ne semble pas avoir de $OBEXFT" ; EXIT 1 ; }
 cat ${CON}/sdptool.${HAD} | grep -i "$OBEXFT" >> $LOG
 
 # On peut commencer a jouer avec OBEX
 
 # DAT=`date --date '+1hour' +%Y-%m-%d_%H-%M`
 DAT=`date --date '+1hour' +%Y%m%d_%H%M`
 [ $NBA -eq 3 ]&&{ DAT=$3 ; }
 FIC=${DAT}.txt
 
 echo "$FTP -b $HAD -g $STR_ACC/$FIC"
 $FTP -b $HAD -g $STR_ACC/$FIC >/dev/null 2>&1
 TESTF $WRK/$FIC
 mv $WRK/$FIC $WRK/ACC.${DAT}.csv
 
 echo "$FTP -b $HAD -g $STR_MAG/$FIC"
 $FTP -b $HAD -g $STR_MAG/$FIC >/dev/null 2>&1
 TESTF $WRK/$FIC
 mv $WRK/$FIC $WRK/MAG.${DAT}.csv
 
 echo "$FTP -b $HAD -g $STR_ORI/$FIC"
 $FTP -b $HAD -g $STR_ORI/$FIC >/dev/null 2>&1
 TESTF $WRK/$FIC
 mv $WRK/$FIC $WRK/ORI.${DAT}.csv
 
fi
####################################################################


