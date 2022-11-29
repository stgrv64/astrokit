/* #define _GNU_SOURCE */

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

#ifndef ASTRO_INFRARED_H
#define ASTRO_INFRARED_H

#include "astro_global.h"

#define IR_GAP          120000
#define IR_SEPARATEUR   '='

// c est le code min et max des touches autorisees a etre repetees 
// pour que ces touches soient prises en compte rapidement et de 
// maniere repetitive  = touches de deplacement N S E O 

#define IR_CODE_REPETE_AUTORISE_MIN 21 
#define IR_CODE_REPETE_AUTORISE_MAX 24

struct STR_INFRARED {
 
} ;

typedef struct STR_INFRARED STRUCT_INFRARED ;

int   INFRARED_OPEN  ( INFRARED_LIRC_CONFIG *gp_LircConfig) ;
void  INFRARED_CLOSE ( INFRARED_LIRC_CONFIG *gp_LircConfig) ;
void  INFRARED_READ  ( STRUCT_SUIVI         * gp_Sui) ;

#endif
