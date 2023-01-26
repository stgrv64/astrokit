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
   STR_EXT_TIMEVAL vou_tval ; 
   FILE           *vou_file ;
   void          (*vou_log)     ( STRUCT_VOUTE * ) ;
   void          (*vou_display) ( STRUCT_VOUTE * ) ;
   void          (*vou_lock)    ( STRUCT_VOUTE * ) ;
   void          (*vou_unlock)  ( STRUCT_VOUTE * ) ;  
   char            vou_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
   char            vou_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;
   int             vou_loglevel ;

   unsigned long   vou_tempo ; 
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

void * _SUIVI_VOUTE           ( STRUCT_VOUTE * ) ;

static void VOUTE_DISPLAY_PREPARE ( STRUCT_VOUTE * ) ;
static void VOUTE_DISPLAY        ( STRUCT_VOUTE * ) ;
static void VOUTE_UNLOCK         ( STRUCT_VOUTE * ) ;
static void VOUTE_LOCK           ( STRUCT_VOUTE * ) ;
static void VOUTE_LOG            ( STRUCT_VOUTE * ) ;

#endif

/* =====================================================================================
 *
 * FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - 
 * 
 * ===================================================================================== */