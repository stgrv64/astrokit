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
   double    DT ; 
   double    pourcentage_tempo ;
   double    calibration_voute ;
   double    deb ;
   double    fin ;
   double    pas ;
   double    dt ;
   double    acc ;
   double    acc_old ;
   long long num ; 
} ;

void  VOUTE_INIT            ( STRUCT_VOUTE * ) ;
void  VOUTE_CONFIG          ( STRUCT_VOUTE *, double , double , double  ) ;
long  VOUTE_TEMPORISATION   ( STRUCT_VOUTE *, struct timeval ) ;
void  VOUTE_AFFICHER        ( STRUCT_VOUTE * ) ;

#endif