/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-10-07  | * creation
# 2022-10-07  | * rapatriement code depuis un peu partout
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_LIEU_H
#define ASTRO_LIEU_H

#include "astro_global.h"

#ifndef  CALCULS_UN_RADIAN_EN_DEGRES
#define  CALCULS_UN_RADIAN_EN_DEGRES  57.29577951308232
#endif

typedef struct STR_LIEU STRUCT_LIEU ;

/*---------------------------------------------------*/
/* Parametres de fichier config (modif octobre 2022) */ 
/*---------------------------------------------------*/

struct STR_LIEU_PARAMS {
  pthread_mutex_t lie_par_mutex ;
  double par_longitude ;
  double par_latitude ;
  double par_altitude ;
} ;

typedef struct STR_LIEU_PARAMS STRUCT_LIEU_PARAMS ;

/*---------------------------------------------------*/
/* Fin parametres de fichier config                  */ 
/*---------------------------------------------------*/

struct STR_LIEU {
 
 pthread_mutex_t lie_mutex ;
 double          lie_lat ;  // latitude
 double          lie_lon ;  // longitude
 double          lie_alt ;
 double          lie_jj ;  // jour julien
 double          lie_ts ;   // temps sideral
 double          lie_tsr ;  // temps sideral ramene en radians
 double          lie_jd ;   // jour decimal
} ;

void   LIEU_INIT    ( STRUCT_LIEU    * ) ;
void   LIEU_AFFICHER( STRUCT_LIEU *gp_Lie) ;

#endif