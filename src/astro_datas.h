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

#include "astro_global.h"

#ifndef CONFIG_TAILLE_BUFFER_256
#define CONFIG_TAILLE_BUFFER_256 256
#endif

struct STR_DATAS {
  pthread_mutex_t dat_mutex ;
  char dat_act [ CONFIG_TAILLE_BUFFER_256 ] ;
  char dat_acc [ CONFIG_TAILLE_BUFFER_256 ] ;
  char dat_bou [ CONFIG_TAILLE_BUFFER_256 ] ;
} ;
typedef struct STR_DATAS STRUCT_DATAS ;

void DATAS_INIT         ( STRUCT_DATAS * ) ;
void DATAS_RESET        ( STRUCT_DATAS * )  ;
void DATAS_ACTION_RESET ( STRUCT_DATAS * )  ;
void DATAS_ACTION_COPY  ( STRUCT_DATAS * , const char * ) ;

#endif