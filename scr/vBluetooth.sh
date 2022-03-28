#!/bin/bash

A=`basename $0`
B=`which $A`

[ `echo "$B" | grep "\/"|wc -l` -eq 0 ]&& {
 echo "Ce script doit etre lance avec le rep courant dans le PATH .."
 exit 1
}

WRK=`dirname $B`
NBA=$#
RWK=`dirname $0`
NBA=`echo $#`
DAT=`date +%Y%m%d.%H%M`

CLE=7568
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

# ====================================================================
# FONCTIONS
# ====================================================================

function EXIT { echo "\---------------------------" ; cat $LOG ; exit $1 ; }
function TESTF { [ ! -f $1 ]&&{ echo "$1 inexistant" >> $LOG ; EXIT 1 ;} }
function TRACE { echo "$1" >> $LOG ; }
function RETKO { RET=`echo $?` ; echo $RET ;[ $RET -ne 0 ] && { echo "$1" >> $LOG ; EXIT 1 ; } }

# ====================================================================
function TEST_PING {
 RETURN=0
 HADD=$1
 # On teste que le portable est joignable avec un ping du protocole L2CAP
 $L2P -c 2 -t 3 $HADD >>$LOG 2>&1
 RET=`echo $?`
 if [ $RET -ne 0 ]
 then
  TRACE "Impossible de pinger $HADD"
  RETURN=1
 else
  TRACE "ping l2cap sur $HADD ok"
 fi
 return $RETURN
}
# ====================================================================

function TEST_HOST {
 if [ `lsusb | grep -i bluetooth | wc -l` -eq 0 ];then
  TRACE "Pas de dongle bluetooth"
  EXIT 1
 fi
 lsusb | grep -i bluetooth >> $LOG
}
# ====================================================================

function TEST_DEMON {
 $BLD status 2>/dev/null
 RET=`echo $?`
 [ $RET -ne 0 ]&&[ $RET -eq 3 ]&& $BLD start 2>/dev/null
 [ $RET -ne 0 ]&&[ $RET -ne 3 ]&& { TRACE "$BLD status failed" ; EXIT 1 ; }
}
# ====================================================================

function SCAN_REMOTE {
 RETURN=0
 $HCT scan > ${LOG}.scan
 cat ${LOG}.scan
 if [ `cat ${LOG}.scan | grep -v Scanning | wc -l` -lt 1 ];then
   TRACE "Aucun remote dispos"
   RETURN=1
 fi
 return $RETURN
}
# ====================================================================

function CHANNELS_DISPO_IN_CONF {
 RETURN=0
 # Parcourt le fichier de conf a la recherche d'un device joignable
 for CHA in `cat $RFF | awk 'BEGIN{ID="rfcomm*";}{if ($0 ~ "^" ID && i == 0 ){print substr($1,7,1);}}'`
 do
  TRACE "Channel trouve dans $RFF : $CHA ajout dans channels dispo"
  DEV=`cat $RFF | tr -d ';' | awk -v cha="rfcomm${CHA}" 'BEGIN{while($1!=cha)getline;}{while($1!="device")getline;print $2;exit;}'`
  echo $DEV > $CON/channel.$CHA
  RETURN=$((RETURN+1))
 done
 return $RETURN
}
# ====================================================================

function TEST_CONNEXION {
 RETURN=0
 CHA=$1
 DEV=$2
 echo "Test connexion $CHA pour le device $DEV :" >> $LOG
 FIC=${LOG}.show.$CHA.$DEV
 
 TEST_PING $DEV
 
 ret=`echo $?`
 
 if [ $ret -eq 0 ]
 then
  TRACE "ping ok sur $DEV"
  
  $RFB show $CHA 2>/dev/null > $FIC
  TRACE "release de cette connexion closed pour $CHA"
  $RFB release $CHA
  echo "Tentative connexion sur $CHA"
  $RFB connect $CHA &
  
  $RFB show $CHA 2>/dev/null > $FIC
  
  if [ `cat $FIC | egrep "channel $CHA connected" | wc -l` -eq 0 ];then
    TRACE "echec de la connexion sur le channel $CHA pour device $DEV"
    echo "echec de la connexion sur le channel $CHA pour device $DEV"
    RETURN=2
  fi
 fi
 return $RETURN
}
# ====================================================================

function LANCE_BLUETOOTH_AGENT {
 [ `ps aux | grep $BLA | wc -l` -eq 0 ]&&{
   # On kill sans menagement le precedent
   PID=`ps aux | grep $BLA| grep -v grep | awk '{print $2}'`
   kill $PID
   $BLA $CLE & 
 }
}
# ====================================================================
# on a besoin des fichiers suivants pour travailler sinon installer les paquets
# ====================================================================

TESTF $RFB
TESTF $BLA
TESTF $BLD
TESTF $HCT
TESTF $RFF
TESTF $L2P
TESTF $SDP
TESTF $FTP

# ====================================================================
# USAGE
# ====================================================================

[ $# -eq 1 ]&&[ "$1" == "--help" ]||[ "$1" == "-h" ]&&{
  echo "Usage : $0 <channel(fac)>"
  echo "Channel predefini :"
  cat $RFF
  EXIT 0
}

# ====================================================================
# LE PORTABLE EST JOIGNABLE ET N'A JAMAIS ETE CONFIGURE ---------------
# ====================================================================

CHA=""

TRACE "partie I du script "

TEST_HOST
TEST_DEMON
# SCAN_REMOTE 
CHANNELS_DISPO_IN_CONF

RET=0
for FIC in `ls $CON`
do
  CHA=`echo $CON/$FIC | cut -d'.' -f2`
  DEV="`cat $CON/$FIC`"
  TEST_CONNEXION $CHA $DEV
  RET=`echo $?`
done 

echo $RET
exit 1
TRACE "partie II du script "

CONTINUE=0

# ON SAIT QUE LE PORTABLE EST JOIGNABLE OU DEJA CONFIGURE -------------


if [ $CONTINUE -eq 1 ];then
 # a modifier si deja occuper utiliser celui la
 # On extrait les donnees depuis le fichier de conf
 HAD=`cat $RFF | grep device | grep ":" | cut -d';' -f1 | cut -d' ' -f2`
 TRACE "hardware adress trouve = $HAD"
 TEST_PING $HAD
 LANCE_BLUETOOTH_AGENT
 $RFB connect $CHA >>$LOG & 
fi

TRACE "partie III du script "

# ON SAIT QUE LE PORTABLE EST CONFIGURE ET CONNECTE -------------
# ON COMMENCE LES TRANSFERTS

if [ "$CHA" != "" ]||[ $NBA -eq 1 ]||[ $NBA -gt 2 ]&&[ "$2" == "transfert" ]
then
 [ "$CHA" == "" ]&& { CHA=$1 ; }

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


