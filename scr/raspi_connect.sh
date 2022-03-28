#!/bin/bash
INT=/etc/network/interfaces
IFUP=/sbin/ifup
SSH=/usr/bin/ssh
ETH0=eth0
IP=192.168.1.15
echo "MOntage interface eth0 avec caracteristiques suivantes"
[ ! -f $INT ]&&  { echo "$INT inexistante" ; exit 1 ; } 
[ ! -f $IFUP ]&& { echo "$IFUP inexistant" ; exit 1 ; } 
[ ! -f $SSH ]&&  { echo "$SSH inexistant" ; exit 1 ; } 
cat $INT
echo "$IFUP $ETH0" 
$IFUP $ETH0
$SSH $IP /bin/date -s `date "+%Y%m%d"` "+%Y%m%d"
$SSH $IP /bin/date -s `date "+%H%M"`   "+%H%M"
echo "Date sur $IP : verification"
$SSH $IP /bin/date
echo "Connexion sur raspi avec le protocole X11"
$SSH -X $IP
