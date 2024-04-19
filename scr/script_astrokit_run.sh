#!/bin/bash
# Ce script demarre astrokit depuis astrokit-run service
# juin 2022
# -------------------------------------------------------------------- 

# Remplacement des lignes suivantes par une extension de service (override.conf)

#echo "export ld library path"
#export LD_LIBRARY_PATH=/home/stef/libtarget

echo "lancement astrokit en background"

cd /home/stef
./astrokit 
