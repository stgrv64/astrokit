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

#ifndef  CALCUL_UN_RADIAN_EN_DEGRES
#define  CALCUL_UN_RADIAN_EN_DEGRES  57.29577951308232
#endif

typedef struct STR_LIEU STRUCT_LIEU ;

struct STR_LIEU {
 
 double       lat ;  // latitude
 double       lon ;  // longitude
 double       alt ;
 double       JJ  ;  // jour julien
 double       TS ;   // temps sideral
 double       TSR ;  // temps sideral ramene en radians
 double       JD ;   // jour decimal
} ;

void   LIEU_INIT    ( STRUCT_LIEU    * ) ;
void   LIEU_AFFICHER( STRUCT_LIEU *gp_Lie) ;

#endif