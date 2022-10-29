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
  int par_use_Capteurs    ;
  int par_use_Raquette     ;
  int par_use_Bluetooth    ;
  int par_use_Infrared ;
  int par_use_Controler ;
  int par_use_Keyboard  ;
  int par_use_Lcd ; 
} ;

struct STR_DEVICES {

  int  use_bluetooth ;
  int  use_capteurs ;
  int  use_infrarouge ;
  int  use_raquette ;
  int  use_controler ; 
  int  use_keyboard ;
  int  use_lcd ; 
  
  int  init_capteurs ;
} ;

typedef struct STR_DEVICES_PARAMS STRUCT_DEVICES_PARAMS ;
typedef struct STR_DEVICES STRUCT_DEVICES ;

void   DEVICES_INIT ( STRUCT_DEVICES * ) ;

#endif