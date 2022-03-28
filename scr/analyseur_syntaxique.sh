#!/bin/bash

path_file_host=`/usr/bin/which $0`
path_dir_target=$PWD
path_dir_host=`/usr/bin/dirname $path_file_host`
path_name_host=`/usr/bin/basename $path_file_host  | cut -d'.' -f1`
name_dir_target=`/usr/bin/basename $path_dir_target`
dir_work=${path_dir_host}/$path_name_host/${name_dir_target}

echo $path_file_host
echo $path_dir_target
echo $path_dir_host
echo $name_dir_target
echo $dir_work
mkdir -p $dir_work 2>/dev/null

pointc="*.c"
pointh="*.h"

for fic in `ls $pointc`
do
name_fic=`/usr/bin/basename $fic  | cut -d'.' -f1`

cat $fic | grep "(" | gawk '{\
  if( $1 == "double" || $1 == "int" ){\
    s=$1;
    for(i=2;i<NF+1;i++)
      if(i==2)    s = sprintf("%s\t%s",s,$i);
      else        s = sprintf("%s,%s",s,$i);print s;
  }}' | sort -n -k 2 > ${dir_work}/${name_fic}.fun

done
