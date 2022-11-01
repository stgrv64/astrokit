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

#ifndef ASTRO_DATAS_H
#define ASTRO_DATAS_H

#ifndef CONFIG_TAILLE_BUFFER_256
#define CONFIG_TAILLE_BUFFER_256 256
#endif

struct STR_DATAS {
  char dat_inf [ CONFIG_TAILLE_BUFFER_256 ] ;
  char dat_acc [ CONFIG_TAILLE_BUFFER_256 ] ;
  char dat_bou [ CONFIG_TAILLE_BUFFER_256 ] ;
} ;

#include "astro_global.h"

void   DATAS_INIT ( STRUCT_DATAS * ) ;

#endif