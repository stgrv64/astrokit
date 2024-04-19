#!/bin/bash
# -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-06-01  | * creation
# 2022-06-15  | * mise a jour montages et alias reprise code .profile
# --------------------------------------------------------------

/sbin/loadkmap < /etc/fr.kmap 

pro=astrokit

i_sleep=5

c_lin0_clear="-"
c_lin1_clear="-"

c_lin1="Prompt"
c_lin0="Astrokit-v2.1"

# root @ yocto-hardknott-systemd-bindateYbm : ~  #  ip addr | grep "192.168" | awk '{print $1" "$2}'
# inet 192.168.43.27/24
# root @ yocto-hardknott-systemd-bindateYbm : ~  #  ip addr | grep "192.168" | awk '{print $1" "$4}'
# brd 192.168.43.255

c_lin0_ip=$(/sbin/ip addr | grep "192.168" | awk '{print $2}')
c_lin1_ip=$(/sbin/ip addr | grep "192.168" | awk '{print $4}')

rep_ste=/home/stef
rep_sys=/etc/systemd/system
rep_lir=/etc/lirc

rep_boo=/bootfs
rep_roo=/rootfs
rep_dat=/datas
rep_ulb=/usr/local/bin
rep_scr=/usr/local/scr

scr_i2c=${rep_scr}/script_insmod_i2c-dev.sh

dev_boo=/dev/mmcblk0p1
dev_roo=/dev/mmcblk0p2
dev_dat=/dev/mmcblk0p3

g0="21,26,19,13" # Masque GPIOS moteur altitude
g1="6,5,7,11"    # Masque GPIOS moteur azimut
f0=1             # Frequence moteur alt
f1=1             # Frequence moteur azi
m0="0,1,2,3"     # Masque moteurs
n0=1             # Nb mot a utiliser (1=2) 
l0=60            # rafraichissement des traces
u0=128           # Nb micropas

export PATH=$PATH:/usr/local/scr:$rep_ste
export LD_LIBRARY_PATH=${rep_ste}/libtarget

# mise en commentaire au travers d une fonction

function astrokit-myalias() {

alias cdstef='cd ${rep_ste}'
alias cdsysd='cd ${rep_sys}'
alias cdlirc='cd ${rep_lir}'
alias cdusrbin='cd ${rep_ulb}'
alias cdusrscr='cd ${rep_scr}'
alias astrokit-pssansker='ps aux | grep -v "\["'
alias astrokit-mnt-boo='umount ${rep_boo} 2>/dev/null ; mkdir ${rep_boo} 2>/dev/null ; mount ${dev_boo} $rep_boo ;'
alias astrokit-mnt-dat='umount ${rep_dat} 2>/dev/null ; mkdir ${rep_dat} 2>/dev/null ; mount ${dev_dat} $rep_dat ;'
alias astrokit-lcd-verif-0='${rep_ulb}/astrokit-lcd1602.targ "${c_lin0}"       "${c_lin1}" 1 1 5000'
alias astrokit-lcd-clear-0='${rep_ulb}/astrokit-lcd1602.targ "${c_lin0_clear}" "${c_lin1_clear}" 1 1 5000'
alias astrokit-lcd-ips-0='${rep_ulb}/astrokit-lcd1602.targ   "${c_lin0_ip}"    "${c_lin1_ip}" 1 1 5000'
alias astrokit-boo-recup-led-ir='cat ${rep_boo}/config.txt | grep overlay | grep gpio_pin | cut -d "=" -f3' 
alias astrokit-sysd-verif-units='systemctl list-units | grep -i fail'
alias astrokit-gpio-0='/home/stef/gpio -g $g0 -G $g1 -m $m -n $n0 -L $l0 -f $f0 -F $f1 -u u0'
alias astrokit-sysd-status='systemctl status astrokit-run'
alias astrokit-sysd-start='systemctl start astrokit-run'
alias astrokit-sysd-stop='systemctl stop astrokit-run'
alias astrokit-getty-show='for g in $(seq 0 7); do echo $g ; systemctl status getty@tty${g}.service |egrep "Active:"  ; done ;'
alias astrokit-askit-show='systemctl status astrokit* | egrep "Active:" '

}

# mise en commentaire au travers d une fonction

function astrokit-myprompt() {

echo "=> systemd units failed listing"
astrokit-sysd-verif-units
echo "=> mount $dev_dat on $rep_dat"
astrokit-mnt-dat
echo "=> mount $dev_boo on $rep_boo"
astrokit-mnt-boo
echo "=> display $rep_dat contents"
ls /datas 
echo "=> load module i2c-dve (if not)"
${scr_i2c}
echo "=> clear LCD I2C screen"
astrokit-lcd-clear-0
echo "=> display IPS on LCD I2C screen"
astrokit-lcd-ips-0
echo "=> sleep ${i_sleep}"
sleep ${i_sleep} 
echo "=> display prompt on LCD I2C screen"
astrokit-lcd-verif-0
echo "=> recup led IR info config in /boot/config.txt and dtoverlay"
astrokit-mnt-boo
astrokit-boo-recup-led-ir
}

# MAIN

umount ${rep_boo} 2>/dev/null ;
mkdir ${rep_boo} 2>/dev/null ; 
mount ${dev_boo} $rep_boo ;

cat ${rep_boo}/config.txt | grep overlay | grep gpio_pin | cut -d "=" -f3 > /tmp/astrokit-boo-recup-led-ir.txt




