#!/bin/bash
fdisk -l | grep -i ntfs > /tmp/toto
cat /tmp/toto
for dev in `cat /tmp/toto | awk '{print $1}'`;do
  mnt=`echo $dev | cut -d'/' -f3`
  echo $mnt
  mkdir /mnt/$mnt 2>/dev/null
  mount /dev/$mnt /mnt/$mnt 2>/dev/null
done
rm -f /tmp/toto 2>/dev/null
df -k | grep -i mnt
for rep in `df -k | grep mnt | awk '{print $NF}'`;do
  echo "\======== $rep"
  find $rep -maxdepth 1 -type d | sort -k 1
done
if [ $# -eq 2 ];then
 ARG=$1
 MAXDEPTH=$2
 for rep in `df -k | grep mnt | awk '{print $NF}'`;do
  echo "\======== $rep"
  find $rep -maxdepth $MAXDEPTH -type d -name "${ARG}*" 
 done
fi