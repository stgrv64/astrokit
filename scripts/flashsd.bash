#!/bin/bash

USR_LOCAL_BIN=/usr/local/bin
BASE_DIR=$PWD
HOME=""
if [ -d "/home/root" ];then
 HOME="/home/root"
else
 HOME="/root"
fi

KERNEL_DIR=${HOME}/kernel
IMG_DIR=${KERNEL_DIR}/noyaux
ROOTFS_DIR=${KERNEL_DIR}/rootfs
ROOT_ASTROKIT=$HOME/astrokit
PARTSD=${USR_LOCAL_BIN}/partsd

OUTIL=`basename $BASE_DIR`
DATE=`date +%Y%m%d`

BOOT=/Boot
ROOT=/Root

#=====================================================================================
function PARTITIONNER_DEVICE() {

  device=$1
  tt=`sudo fdisk -l | grep $device | wc -l`
  [ $tt -eq 0 ]&& { echo "error : $device non trouve dans fdisk" ; exit 2 ; }

  tt=`sudo df -k | grep $device | wc -l`
  [ $tt -ne 0 ]&& { echo "error : $device est deja monte" ; exit 2 ; }

  $PARTSD $device
}
#=====================================================================================
function FORCER_UMOUNT() {

  sudo umount -f $BOOT 
  sudo umount -f $ROOT
}
#=====================================================================================
function VOIR_DEVICE() {
 
  sudo fdisk -l | grep "/dev"
}
#=====================================================================================
function MONTER_DEVICE() {
  
  device=$1
  pointm=$2
  typ=$3

  tt=`sudo fdisk -l | grep $device | wc -l`
  [ $tt -eq 0 ]&& { echo "error : $device non trouve dans fdisk" ; exit 2 ; }
  
  tt=`sudo df -k | grep $device | wc -l`
  [ $tt -ne 0 ]&& { echo "error : $device est deja monte" ; FORCER_UMOUNT ; exit 2 ; }
  
  mm=`sudo fdisk -l | grep $device | cut -d' ' -f1`
  
  sudo mount -t $typ $mm $pointm 
}
#=====================================================================================
# GEstion des cas particuliers 
#=====================================================================================

[ $# -eq 1 ]&&[ "$1" == "voir" ]&& {
  
  VOIR_DEVICE
  exit 0 
}
#=====================================================================================
[ $# -eq 1 ]&&[ "$1" == "demonter" ]&& {
  
  sudo umount $BOOT
  sudo umount $ROOT
  exit 0 
}
#=====================================================================================
[ $# -eq 2 ]&&[ "$1" == "part" ]&& {
  
  [ ! -f $PARTSD ]&&   { echo "$PARTSD inexistant" ; exit 1 ; }
  PARTITIONNER_DEVICE $2
  exit 0 
}
#=====================================================================================
[ $# -eq 3 ]&&[ "$1" == "monter" ]&& {
  
  echo "Creation si inexistant des repertoires de $BOOT et $ROOT "

  mkdir $BOOT 2>/dev/null
  mkdir $ROOT 2>/dev/null
  
  echo "On monte les devices $2 et $3"
  
  MONTER_DEVICE $2 $BOOT "vfat"
  MONTER_DEVICE $3 $ROOT "ext4"
  
  find $BOOT -maxdepth 1 -ls
  find $ROOT -maxdepth 1 -ls
  
  exit 0 
}
#=====================================================================================
[ $# -eq 0 ]&& {
 echo "Usage : $0 <cible>  <options> <device boot (exemple = /dev/sdc1)> <device root (exemple = /dev/sdc2)>"
 echo "Usage : $0 <monter>           <device boot (exemple = /dev/sdc1)> <device root (exemple = /dev/sdc2)>"
 echo "Usage : $0 <demonter>"
 echo "Usage : $0 <part>  <device>"
 echo "Usage : $0 <voir>"
 echo "\==================================================================="
 echo "cible    : exemple : armv6 ou raspi2.. a titre information seulement"
 echo "options  : exemple : non-rt .. a titre information seulement"
 echo "monter   : uniquement monter les devices et lister le contenu"
 echo "demonter : demonter $BOOT et $ROOT"
 echo "part     : uniquement partitionner le device <device>"
 echo "voir     : voir les devices" 
 exit 1
}
#=====================================================================================

ARCH=$1
OPTIONS=$2
DEVICE_BOOT=$3
DEVICE_ROOT=$4

#=====================================================================================

echo "On verifie l existence des repertoires de travail"

[ ! -d $KERNEL_DIR ]&& { echo "$KERNEL_DIR inexistant" ; exit 1 ; }
[ ! -d $IMG_DIR ]&&    { echo "$IMG_DIR inexistant" ; exit 1 ; }
[ ! -d $BASE_DIR ]&&   { echo "$BASE_DIR inexistant" ; exit 1 ; }
[ ! -d $ROOTFS_DIR ]&&   { echo "$ROOTFS_DIR inexistant" ; exit 1 ; }
[ ! -f $PARTSD ]&&   { echo "$PARTSD inexistant" ; exit 1 ; }
[ ! -d $ROOT_ASTROKIT ]&&   { echo "$ROOT_ASTROKIT inexistant" ; exit 1 ; }

echo "Creation si inexistant des repertoires de boot et root "

mkdir $BOOT 2>/dev/null
mkdir $ROOT 2>/dev/null

# --------------------------------------------------------------------------------------------------------
echo "On monte les devices"
# --------------------------------------------------------------------------------------------------------

MONTER_DEVICE $DEVICE_BOOT $BOOT "vfat"
MONTER_DEVICE $DEVICE_ROOT $ROOT "ext4"

# --------------------------------------------------------------------------------------------------------
echo "Sauvegarde de l image"
# --------------------------------------------------------------------------------------------------------

cp ${BASE_DIR}/zImage $IMG_DIR/$OUTIL.$ARCH.$OPTIONS.$DATE.kernel.img 2>/dev/null

# --------------------------------------------------------------------------------------------------------
echo "Copies de fichiers vers boot"
# --------------------------------------------------------------------------------------------------------

sudo cp -pf ${BASE_DIR}/rpi-firmware/bootcode.bin $BOOT
sudo cp -pf ${BASE_DIR}/rpi-firmware/fixup.dat    $BOOT
sudo cp -pf ${BASE_DIR}/rpi-firmware/start.elf    $BOOT
sudo cp -pf ${BASE_DIR}/zImage                    $BOOT
sudo cp -pf ${BASE_DIR}/*.dtb                     $BOOT
sudo mv     $BOOT/zImage                          $BOOT/kernel.img

find $BOOT -maxdepth 1 -ls

# --------------------------------------------------------------------------------------------------------
echo "Sauvegarde du rootfs"
echo "sudo cp -pf ${BASE_DIR}/rootfs.tar $ROOTFS_DIR/$OUTIL.$ARCH.$OPTIONS.$DATE.rootfs.tar 2>/dev/null"
# --------------------------------------------------------------------------------------------------------

sudo cp -pf ${BASE_DIR}/rootfs.tar $ROOTFS_DIR/$OUTIL.$ARCH.$OPTIONS.$DATE.rootfs.tar 2>/dev/null

# --------------------------------------------------------------------------------------------------------
echo "Detarrage de rootfs.tar vers root"
echo "sudo tar -xpsf ${BASE_DIR}/rootfs.tar -C $ROOT"
# --------------------------------------------------------------------------------------------------------

sudo tar -xpsf ${BASE_DIR}/rootfs.tar -C $ROOT

if [ $? -ne 0 ]
then
 echo "error : Le detarrage de rootfs.tar vers root est sorti en erreur"
fi

# --------------------------------------------------------------------------------------------------------
echo "Copie du projet present $ROOT_ASTROKIT dans $ROOT"
echo "sudo cp -prf $ROOT_ASTROKIT $ROOT/root"
# --------------------------------------------------------------------------------------------------------

sudo cp -prf $ROOT_ASTROKIT $ROOT/root

find $ROOT -maxdepth 1 -ls

if [ $? -ne 0 ]
then
 echo "error : Copie du projet present $ROOT_ASTROKIT dans $ROOT"
fi

sync

sudo umount $BOOT
sudo umount $ROOT

# --------------------------------------------------------------------------------------------------------
# FIN FICHIER
# --------------------------------------------------------------------------------------------------------
