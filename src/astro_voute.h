/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-10-08  | * creation
# 2022-10-08  | * rapatriement code depuis autres fichiers
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_VOUTE_H
#define ASTRO_VOUTE_H

#include "astro_global.h"

/*  dt effectif en microsec en tenant compte de acceleration et de tous les autres parametres */

typedef struct   STR_VOUTE STRUCT_VOUTE ;

struct STR_VOUTE {
   pthread_mutex_t vou_mutex ;
   unsigned long   vou_tempo ;
   unsigned long   vou_dt_us ; 
   long long       vou_num ; 
   double          vou_tempo_percent ;
   double          vou_calibration_delta_t ;
   double          vou_begin ;
   double          vou_end ;
   double          vou_pas ;
   double          vou_dt ;
   double          vou_acc ;
   double          vou_temps_ecoule ;   
   int             vou_run ;
} ;

void  VOUTE_INIT            ( STRUCT_VOUTE * ) ;
void  VOUTE_CONFIG          ( STRUCT_VOUTE *, double , double , double  ) ;
long  VOUTE_TEMPORISATION   ( STRUCT_VOUTE *, struct timeval ) ;
void  VOUTE_AFFICHER        ( STRUCT_VOUTE * ) ;

#endif