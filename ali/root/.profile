# -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-05-01  | * creation
# 2022-06-01  | * ajout entete , alias et certaines fonctionalites
# -------------------------------------------------------------- 

pro=astrokit

rep_ste=/home/stef
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

alias astrokit-cd-rep-ste='cd ${rep_ste}'
alias astrokit-pssansker='ps aux | grep -v "\["'

alias astrokit-mnt-boo='umount ${rep_boo} 2>/dev/null ; mkdir ${rep_boo} 2>/dev/null ; mount ${dev_boo} $rep_boo ;'
alias astrokit-mnt-dat='umount ${rep_dat} 2>/dev/null ; mkdir ${rep_dat} 2>/dev/null ; mount ${dev_dat} $rep_dat ;'

alias astrokit-lcd-verif-0='${rep_ulb}/astrokit-lcd1602.targ Astrokit Welcome 1 1 5000'

alias astrokit-boo-recup-led-ir='astrokit-mnt-boo ; cat $rep_boo/config.txt | grep overlay | grep gpio_pin | cut -d "=" -f3' 
alias astrokit-sysd-verif-units='systemctl list-units | grep -i fail'

alias astrokit-gpio-0='/home/stef/gpio -g $g0 -G $g1 -m $m -n $n0 -L $l0 -f $f0 -F $f1 -u u0'

#--------------------------------------
# Action a realiser au prompt
#--------------------------------------

echo "=> verification des units systemd"
astrokit-sysd-verif-units

echo "=> montage $dev_dat sur $rep_dat"	
astrokit-mnt-dat

echo "=> montage $dev_boo sur $rep_boo"	
astrokit-mnt-boo

echo "=> affichage contenu $rep_dat"
ls /datas 

echo "=> chargement module i2c-dev"
${scr_i2c}

echo "=> affichage message sur ecran LCD"
astrokit-lcd-verif-0

echo "=> affichage des alias"
alias | grep astrokit 

echo "=> deplacement dans ${rep_ste}"
astrokit-cd-rep-ste

