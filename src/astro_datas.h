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

#define DATAS_ZERO_CHAR ""

#ifndef CONFIG_TAILLE_BUFFER_256
#define CONFIG_TAILLE_BUFFER_256 256
#endif

typedef struct STR_DATAS STRUCT_DATAS ;

struct STR_DATAS {
  pthread_mutex_t dat_mutex ;
  STR_EXT_TIMEVAL dat_tval ; 
  FILE           *dat_file ; 
  char            dat_loglevel ;
  void          (*dat_lock)       ( STRUCT_DATAS *) ;
  void          (*dat_unlock)     ( STRUCT_DATAS *) ;  
  void          (*dat_log)        ( STRUCT_DATAS *) ;
  void          (*dat_display)    ( STRUCT_DATAS *) ;
  char            dat_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char            dat_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;
  
  char dat_act [ CONFIG_TAILLE_BUFFER_256 ] ;
  char dat_acc [ CONFIG_TAILLE_BUFFER_256 ] ;
  char dat_bou [ CONFIG_TAILLE_BUFFER_256 ] ;
} ;

void DATAS_INIT               ( STRUCT_DATAS * ) ;
void DATAS_RESET              ( STRUCT_DATAS * )  ;
void DATAS_ACTION_RESET       ( STRUCT_DATAS * )  ;
void DATAS_ACTION_BUF_TO_DAT  ( STRUCT_DATAS * , const char * )  ;
void DATAS_ACTION_DAT_TO_KEY  ( STRUCT_DATAS * , STRUCT_KEYS * ) ;

#endif

/*****************************************************************************************
 *
 * END FILE - END FILE - END FILE - END FILE - END FILE - END FILE - END FILE - END FILE - 
 * 
 * ************************************************************************************* */ 
