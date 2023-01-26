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
 
  pthread_mutex_t  lie_mutex ;
  STR_EXT_TIMEVAL  lie_tval ; 
  FILE            *lie_file ; 
  void           (*lie_log)     ( STRUCT_LIEU * ) ;
  void           (*lie_display) ( STRUCT_LIEU * ) ;
  void           (*lie_lock)    ( STRUCT_LIEU * ) ;
  void           (*lie_unlock)  ( STRUCT_LIEU * ) ;  
  int              lie_loglevel ;
  char             lie_dis_for [ CONFIG_TAILLE_BUFFER_256 ] ;
  char             lie_dis_cmd [ CONFIG_TAILLE_BUFFER_256 ] ;

  double           lie_lat ;  // latitude
  double           lie_lon ;  // longitude
  double           lie_alt ;
  double           lie_jj ;  // jour julien
  double           lie_ts ;   // temps sideral
  double           lie_tsr ;  // temps sideral ramene en radians
  double           lie_jd ;   // jour decimal
} ;

void LIEU_INIT           ( STRUCT_LIEU    * ) ;
void LIEU_PARAMS_INIT    ( STRUCT_LIEU_PARAMS * ) ;
void LIEU_PARAMS_DISPLAY ( STRUCT_LIEU_PARAMS * ) ;

void LIEU_DISPLAY_old    ( STRUCT_LIEU *)  ;

#endif