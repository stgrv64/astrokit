#!/bin/bash

MODPROBE=/sbin/modprobe
MODE2=/usr/bin/mode2
LIRC=/etc/init.d/lirc
LSMOD=/sbin/lsmod
LIRCDCONF=/etc/lirc/lircd.conf
LIRCRC=/root/.lircrc

[ $# -lt 1 ]&& { echo "Usage : $0 <port gpio pour la led infrarouge> <1=mode demon(facultatif)>" ; }
[ $# -lt 1 ]&& { echo "Usage : $0 <.lircrc> (generer le fichier .lircrc utiliser par les commandes ir* _ I.E irexec _)" ; }

if [ $# -eq 1 ]&&[ "$1" == ".lircrc" ]
then
  [ ! -f $LIRCDCONF ]&& { echo "$LIRCDCONF inexistant" ; exit 1 ; }
  for ligne in `cat $LIRCDCONF | grep name | grep KEY | cut -d' ' -f2`
  do
    echo $ligne
  done
  exit 1
fi

NBA=$#
PORT=$1


[ ! -f $MODPROBE ]&& { echo "$MODPROBE inexistant" ; exit 1 ; }
[ ! -f $MODE2 ]&& { echo "$MODE2 inexistant" ; exit 1 ; }
[ ! -f $LSMOD ]&& { echo "$LSMOD inexistant" ; exit 1 ; }

[ -f $LIRC ]&& { $LIRC stop 2>/dev/null ; }

$MODPROBE -r lirc_rpi 2>/dev/null
$MODPROBE -r lirc_dev 2>/dev/null
$MODPROBE lirc_rpi gpio_in_pin=$PORT

if [ $NBA -lt 2 ]||[ $NBA -gt 1 ]&&[ "$2" != "1" ]
then
 echo "test de reception de la telecommande infrarouge sur le gpio $PORT"
 echo "appuyer sur une touche"
 echo "CTRL^c pour terminer"
 $MODE2 -d /dev/lirc0
fi
