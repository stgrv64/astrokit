#!/bin/bash
/sbin/loadkmap < /etc/fr.kmap 
c_lin0=$(date +%Y%b%d-%Hh%M)
c_lin1="AstKit-2022-v2.1"
i_lcd_add=20
export LD_LIBRARY_PATH="/home/stef/libtarget"
/sbin/insmod /lib/modules/5.10.31-rt38-v7/kernel/drivers/i2c/i2c-dev.ko
#/bin/lsmod | grep i2c
#/usr/sbin/i2cdetect -y 1 | grep $i_lcd_add
/usr/local/bin/astrokit-lcd1602.targ ${c_lin0} ${c_lin1} 1 1 5000
