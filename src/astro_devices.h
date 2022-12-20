/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-10-07  | * creation
# 2022-10-07  | * rapatriement code depuis astro_config.h
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_DEVICES_H
#define ASTRO_DEVICES_H

#include "astro_global.h"

struct STR_DEVICES_PARAMS {
  pthread_mutex_t dev_par_mutex ;
  int dev_par_use_capteurs    ;
  int dev_par_use_raquette     ;
  int dev_par_use_bluetooth    ;
  int dev_par_use_infrared ;
  int dev_par_use_controler ;
  int dev_par_use_keyboard  ;
  int dev_par_use_lcd ; 
} ;

struct STR_DEVICES {
  pthread_mutex_t dev_mutex ;
  void          (*dev_lock)   (void) ;
  void          (*dev_unlock) (void) ;  
  FILE           *dev_file ;
  int             dev_use_bluetooth ;
  int             dev_use_capteurs ;
  int             dev_use_infrarouge ;
  int             dev_use_raquette ;
  int             dev_use_controler ; 
  int             dev_use_keyboard ;
  int             dev_use_lcd ; 
  int             dev_init_capteurs ;
} ;

typedef struct STR_DEVICES_PARAMS STRUCT_DEVICES_PARAMS ;
typedef struct STR_DEVICES STRUCT_DEVICES ;

void   DEVICES_INIT                 ( STRUCT_DEVICES * ) ;
void   DEVICES_PARAMS_INIT          ( STRUCT_DEVICES_PARAMS * ) ;
void   DEVICES_PARAMS_DISPLAY       ( STRUCT_DEVICES_PARAMS * ) ;

void   DEVICES_DISPLAY_UTILISATION ( STRUCT_DEVICES * ) ;  

#endif