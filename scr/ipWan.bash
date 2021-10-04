#!/bin/bash
DIR=/root/SCRIPTS
DEST=stgrv64mail@gmail.com
rm -f $DIR/index.html*
wget -O $DIR/index.html http://192.168.1.1 2>/dev/null
ipWan="`grep '[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}' $DIR/index.html | grep value | cut -d '>' -f2 | cut -d'<' -f1`"
ipWanOld="`cat $DIR/ipWan.txt`" 2>/dev/null
[ "$ipWanOld" != "$ipWan" ]&& {
  echo $ipWan | mail -s "mon ip wan" $DEST 
  echo $ipWan >  $DIR/ipWan.txt
}
