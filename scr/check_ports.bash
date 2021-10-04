#!/bin/bash
function testIp {
 ip=$1
 ping -w 1 $ip >/dev/null 2>&1
 res=`echo $?`
 [ $res -eq 0 ]&& { echo $ip >> ip_ok ;}
}
fucntion testPort {
 ip=$1
 for port in {1..32768}; do
   echo "Test du port $port"
   telnet $ip $port  2>/dev/null
 done
}
testPort $1
