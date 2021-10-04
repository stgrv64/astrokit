#!/bin/bash
DATE="`date +%Y%m%d`.`date +%H%M`"
LOG=/root/LOG/rtc.log.${DATE}
modprobe rtc-ds1307
/bin/echo ds1307 0x68 > /sys/class/i2c-adapter/i2c-1/new_device 
/sbin/hwclock -s
/sbin/hwclock -r --show
