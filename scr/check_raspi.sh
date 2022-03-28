#!/bin/bash
function testIp {
 ip=$1
 ping -w 1 $ip >/dev/null 2>&1
 res=`echo $?`
 [ $res -eq 0 ]&& { echo $ip >> ip_ok ;}
}
rm -f ip_ok
touch ip_ok
for i in {1..255}
do
 ip="192.168.0."$i
 testIp $ip &
done
wait
for i in {1..255}
do
 ip="192.168.1."$i
 testIp $ip &
done
wait
cat ip_ok
