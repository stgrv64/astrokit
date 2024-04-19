#!/bin/bash
# Ce script charge le module i2c-dev - appel depuis unit astrokit-init
# mai 2022
# -------------------------------------------------------------------- 

/sbin/insmod /lib/modules/5.10.31-rt38-v7/kernel/drivers/i2c/i2c-dev.ko
/bin/lsmod | grep i2c
/usr/sbin/i2cdetect -y 1
