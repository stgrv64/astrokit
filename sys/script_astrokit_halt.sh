#!/bin/bash
# ==========================================================================

c_lin0=$(date +%Y%b%d-%Hh%M)
c_lin1="System halting"

export LD_LIBRARY_PATH="/home/stef/libtarget"

/usr/local/bin/astrokit-lcd1602.targ "${c_lin0}" "${c_lin1}" 1 1 5000
