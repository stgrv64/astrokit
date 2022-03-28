#!/bin/bash

BIN=/etc/init.d/lirc
MODPROBE=/sbin/modprobe
DPKG=/usr/bin/dpkg

echo "On arrete le demaon lirc :"

[ -f $BIN ]&& { $BIN stop ; }

echo "On enleve les modules lirc_rpi et lirc_dev :"

[ -f $MODPROBE ]&& { $MODPROBE -r lirc_rpi ; }
[ -f $MODPROBE ]&& { $MODPROBE -r lirc_dev ; }

echo "Les paquetages lirc sont :"

[ -f $DPKG ]&& { $DPKG -l | grep lirc ; }
# find /etc/*rc*d -type l -name "*apache*" -exec unlink {} \;
