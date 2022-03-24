/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 22/03/2022  | ajout entete
# 22/03/2022  | deplacement de 2 fonctions IR_xxx_
#  dans config.c / .h (aucun lien avec infrarouge)
# -------------------------------------------------------------- 
*/

#ifndef IR_H
#define IR_H

// INCLUDES =====================================
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <types.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <linux/i2c-dev.h>
#include <lirc_client.h>

#include <arguments.h>
#include <astro.h>
#include <calculs.h>
#include <cat.h>
#include <config.h>
#include <gpio.h>
#include <i2c.h>
//#include <ir.h>
#include <stat.h>
#include <types.h>

// FIN INCLUDES =====================================

#define IR_GAP          120000
#define IR_SEPARATEUR   '='
#define IR_ZERO_CHAR    '\0'

// c est le code min et max des touches autorisees a etre repetees 
// pour que ces touches soient prises en compte rapidement et de 
// maniere repetitive  = touches de deplacement N S E O 

#define IR_CODE_REPETE_AUTORISE_MIN 21 
#define IR_CODE_REPETE_AUTORISE_MAX 24


//------------------------------------------------------------------------------

struct lirc_config *lircconfig;

//------------------------------------------------------------------------------
// Prototype des fonctions
//------------------------------------------------------------------------------

int   LIRC_OPEN(struct lirc_config *lircconfig) ;
void  LIRC_CLOSE(struct lirc_config *lircconfig) ;
void  LIRC_READ(SUIVI *suivi) ;

void  CONFIG_INIT_CODES(t_st_Codes *gp_Codes) ;

void  IR_ACTIONS_PARTICULIERES( SUIVI *suivi) ;

#endif
