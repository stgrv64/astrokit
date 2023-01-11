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

typedef struct STR_INFRARED STRUCT_INFRARED ;

struct STR_INFRARED {
  pthread_mutex_t  inf_mutex ;
  STR_EXT_TIMEVAL  inf_tval ; 
  FILE            *inf_file ; 
  char             inf_loglevel ;
  void           (*inf_lock)       ( STRUCT_INFRARED * ) ;
  void           (*inf_unlock)     ( STRUCT_INFRARED * ) ;  
  void           (*inf_log)        ( STRUCT_INFRARED * ) ;
  void           (*inf_display)    ( STRUCT_INFRARED * ) ;
  char             inf_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char             inf_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;
} ;

void  INFRARED_INIT  ( STRUCT_INFRARED * ) ;

/* TODO : inserer gp_LircConfig dans STR_INFRARED */

int   INFRARED_OPEN  ( STR_EXT_LIRC_CONFIG *gp_LircConfig) ;
void  INFRARED_CLOSE ( STR_EXT_LIRC_CONFIG *gp_LircConfig) ;
void  INFRARED_READ  ( void) ;

#endif
