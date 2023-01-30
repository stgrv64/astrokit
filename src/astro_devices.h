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
  STR_EXT_TIMEVAL dev_tval ; 
  FILE           *dev_file ; 
  int             dev_loglevel ;
  void          (*dev_lock)       ( STRUCT_DEVICES *) ;
  void          (*dev_unlock)     ( STRUCT_DEVICES *) ;  
  void          (*dev_log)        ( STRUCT_DEVICES *) ;
  void          (*dev_display)    ( STRUCT_DEVICES *) ;
  char            dev_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char            dev_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;

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