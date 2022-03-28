#!/bin/bash

PIDSHELL=$$
DEBUG=1

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

CLE=1234
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

# ====================================================================
# fichiers sur le remote portable -------------------------------------
# ====================================================================

REP_AND="/sensortrack"

REP_MAG="${REP_AND}/magnetic"
REP_ORI="${REP_AND}/orientation"
REP_ACC="${REP_AND}/accelerometer"

# ====================================================================
# Numero des champs CSV
# ====================================================================

AND_DATE=0

AND_ACCX=3
AND_ACCY=4
AND_ACCZ=5

AND_MAGX=3
AND_MAGY=4
AND_MAGZ=5

AND_LAT=18
AND_LON=19

# ====================================================================
# FONCTIONS
# ====================================================================

function EXIT { echo "\---------------------------" ; cat $LOG ; exit $1 ; }
function TESTF { [ ! -f $1 ]&&{ echo "$1 inexistant" >> $LOG ; EXIT 1 ;} }
function TRACE { echo "$1" >> $LOG ; [ "$DEBUG" == "1" ]&& { echo $1 ; } }
function RETKO { RET=`echo $?` ; echo $RET ;[ $RET -ne 0 ] && { echo "$1" >> $LOG ; EXIT 1 ; } }

# ====================================================================
function TEST_PING {
 
 HADD=$1
 $L2P -c 1 -t 2 $HADD >>$LOG 2>&1
 RET=`echo $?`
 [ $RET -ne 0 ]&& { TRACE "Impossible de pinger $HADD" ; return 2 ; }
 [ $RET -eq 0 ]&& { TRACE "ping protocole L2CAP sur $HADD ok" ; return 0 ; }
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

 CHA=$1
 DEV=$2
 TRACE "Test connexion $CHA pour le device $DEV :"
 FIC=${LOG}.show.$CHA.$DEV
 RES=${LOG}.connections.etablies
 
 TEST_PING $DEV
 
 ret=`echo $?`
 
 if [ $ret -eq 0 ]
 then
  $RFB show $CHA 2>/dev/null > $FIC
  TRACE "Contenu de la connexion pour channel $CHA"
  cat $FIC 
  if [ `cat $FIC | grep "channel $CHA connected" | wc -l` -eq 0 ]
  then
    $RFB release $CHA
    $RFB connect $CHA &
    sleep 2
    $RFB show $CHA 2>/dev/null > $FIC
    
    if [ `cat $FIC | egrep "channel $CHA connected" | wc -l` -eq 0 ];then
      TRACE "echec de la connexion sur le channel $CHA pour device $DEV"
      return 2
    else
      TRACE "Connexion etablie sur channel $CHA pour portable $DEV"
      echo $CHA $DEV >> $RES
      return 0
    fi
  else
    TRACE "Connexion deja etablie sur channel $CHA pour portable $DEV"
    echo $CHA $DEV >> $RES
    return 0
  fi
 else
   TRACE "Connexion channel $CHA impossible : portable $DEV non joignable avec un ping"
   return 2    
 fi
}
# ====================================================================

function LANCE_BLUETOOTH_AGENT {
  ps aux | grep $BLA
  [ `ps aux | grep $BLA | wc -l` -gt 0 ]&&{
    # On kill sans menagement le precedent
    PID=`ps aux | grep $BLA| grep -v grep | awk '{print $2}'`
    kill $PID
  }
  echo "On lance $BLA avec la cle $CLE" 
  TRACE "On lance $BLA avec la cle $CLE"  
  $BLA $CLE & 
}
# ====================================================================

function TRANSFERT_FTP {
 
 DAT=""
 
 [ $NBA -eq 2 ]&&{ DAT=$2 ; }
 [ $NBA -lt 2 ]&&{ DAT=`date +%Y%m%d_%H%M` ; }
 
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
 # DAT=`date --date '+1hour' +%Y%m%d_%H%M`
 
 
 FIC=${DAT}.txt
  
 echo "$FTP -b $HAD -g $REP_ACC/$FIC"
 $FTP -b $HAD -g $REP_ACC/$FIC >/dev/null 2>&1
 TESTF $WRK/$FIC
 mv $WRK/$FIC $WRK/ACC.${DAT}.csv
 
 echo "$FTP -b $HAD -g $REP_MAG/$FIC"
 $FTP -b $HAD -g $REP_MAG/$FIC >/dev/null 2>&1
 TESTF $WRK/$FIC
 mv $WRK/$FIC $WRK/MAG.${DAT}.csv
 
 echo "$FTP -b $HAD -g $REP_ORI/$FIC"
 $FTP -b $HAD -g $REP_ORI/$FIC >/dev/null 2>&1
 TESTF $WRK/$FIC
 mv $WRK/$FIC $WRK/ORI.${DAT}.csv
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

[ "$1" == "transfert" ]&& { TRANSFERT_FTP ; exit 0 ; }

TEST_HOST
TEST_DEMON
# SCAN_REMOTE 
LANCE_BLUETOOTH_AGENT
CHANNELS_DISPO_IN_CONF

RET=0
for FIC in `ls $CON`
do
  CHA=`echo $CON/$FIC | cut -d'.' -f2`
  DEV="`cat $CON/$FIC`"
  TEST_CONNEXION $CHA $DEV &
done 

TRANSFERT_FTP 
 

####################################################################


