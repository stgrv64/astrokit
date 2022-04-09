#!/bin/sh
# =======================================================================================
# Rappel du code en C
# =======================================================================================
#  sprintf(exp,"%s/export",GPIO_PATH) ;
#  sprintf(dir,"%s/gpio%d/direction",GPIO_PATH,gpio) ;
#  sprintf(val,"%s/gpio%d/value",GPIO_PATH,gpio) ;
#  if ( output ) {
#    TRACE("gpio ouverture en OUTPUT : dir= %s exp=%s val=%s",dir,exp,val);
#    if ((f=fopen(exp,"w")) == NULL )                      { GPIO_OPEN_STATUT = -1  ; }
#    else if ((fprintf(f,"%d\n",gpio))<=0)                 { GPIO_OPEN_STATUT = -2  ; }
#    else if ((fclose(f)) != 0 )                           { GPIO_OPEN_STATUT = -3  ; }
#    else if ((f=fopen(dir,"w")) == NULL)                  { GPIO_OPEN_STATUT = -4  ; }
#    else if ((fprintf(f,"out\n")<=0))                     { GPIO_OPEN_STATUT = -5  ; }
#    else if ((fclose(f)!=0))                              { GPIO_OPEN_STATUT = -6  ; }
#    else if ((gpio_fd[ gpio ]=open(val, O_WRONLY))<0)     { GPIO_OPEN_STATUT = -7  ; }
# =======================================================================================
# Donc il faut faire en script shell ceci ===>
# =======================================================================================
#     echo 18  > /sys/class/gpio/export
#     echo out > /sys/class/gpio/gpio18/direction
#     echo 1   > /sys/class/gpio/gpio18/value
# =======================================================================================

REP_LOG=/root/log
REP_CONF=/root/conf
REP_CLASS=/sys/class/gpio
AWK=/usr/bin/awk

mkdir -p $REP_LOG 2>/dev/null 

FIC_CONF=$REP_CONF/config.txt
FIC_LOG=$REP_LOG/active_gpio.sh.log

[ ! -d $REP_CLASS ]&& { echo "Repertoire $REP_CLASS inexistant" > $FIC_LOG ; exit 1 ; }
[ ! -d $REP_CONF ]&& { echo "Repertoire $REP_CONF inexistant" > $FIC_LOG ; exit 1 ; }
[ ! -f $FIC_CONF ]&& { echo "Fichier $FIC_CONF inexistant" > $FIC_LOG ; exit 1 ; }
[ ! -f $AWK ]&& { echo "Fichier $AWK inexistant" > $FIC_LOG ; exit 1 ; }

GPIO=`cat $FIC_CONF | grep -v "#" | grep GPIO_LED_ETAT | $AWK '{print $2}'`
echo "Activation gpio${GPIO}"
echo $GPIO > $REP_CLASS/unexport 2>/dev/null
echo $GPIO > $REP_CLASS/export 2>/dev/null
echo out > /sys/class/gpio/gpio${GPIO}/direction
echo 1   > /sys/class/gpio/gpio${GPIO}/value

