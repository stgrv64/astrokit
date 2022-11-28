/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
--------------------------------------------------------------
date        | commentaires 
--------------------------------------------------------------
07/10/2022  | * creation a partir du code de astro.c et autre
-------------------------------------------------------------- 
*/

#include "astro_voute.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/*****************************************************************************************
* @fn     : VOUTE_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_VOUTE *lp_Vou
* @param  : STRUCT_VOUTE *lp_Vou
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           vou_tempo_percent et vou_calibration_delta_t
*****************************************************************************************/

void VOUTE_INIT(STRUCT_VOUTE *lp_Vou) {
  
  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  lp_Vou->vou_temps_ecoule        = 0 ;
  lp_Vou->vou_dt                  = 0  ;
  lp_Vou->vou_tempo_percent       = 0.96 ;   
  lp_Vou->vou_calibration_delta_t = 0.97 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
  lp_Vou->vou_num                 = 0 ;
  lp_Vou->vou_begin               = 0 ;
  lp_Vou->vou_end                 = 0 ;
  lp_Vou->vou_acc        = 1 ;
  lp_Vou->vou_pas                 = lp_Vou->vou_dt * CALCULS_VIT_ROT_EARTH_RAD_PER_SEC ;
  /* dt en micro-sec */

  lp_Vou->vou_dt_us = (unsigned long)( lp_Vou->vou_dt * TEMPS_MICRO_SEC / lp_Vou->vou_acc ) ;   
}

/*****************************************************************************************
* @fn     : VOUTE_CONFIG
* @author : s.gravois
* @brief  : Cette fonction configure la structure STRUCT_VOUTE *lp_Vou
* @param  : STRUCT_VOUTE *lp_Vou
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           vou_tempo_percent et vou_calibration_delta_t
*****************************************************************************************/

void VOUTE_CONFIG( STRUCT_VOUTE *lp_Vou, double dt, double acc, double percent ) {
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  lp_Vou->vou_temps_ecoule        = 0 ;
  lp_Vou->vou_dt                  = dt  ;
  lp_Vou->vou_tempo_percent       = percent ; 
  lp_Vou->vou_calibration_delta_t = 0.99 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
  lp_Vou->vou_begin               = 0 ;
  lp_Vou->vou_end                 = CALCULS_PI_FOIS_DEUX ;
  lp_Vou->vou_num                     = 0 ;
  lp_Vou->vou_acc        = acc ;
  lp_Vou->vou_pas                 = lp_Vou->vou_dt * CALCULS_VIT_ROT_EARTH_RAD_PER_SEC ;
  /* dt en micro-sec */
  lp_Vou->vou_dt_us                      = (unsigned long)( lp_Vou->vou_dt * TEMPS_MICRO_SEC / lp_Vou->vou_acc ) ;
}

/*****************************************************************************************
* @fn     : VOUTE_TEMPORISATION
* @author : s.gravois
* @brief  : permet de faire une temporisation (1s) dans la boucle infinie de SUIVI_VOUTE
* @brief  : utilise un usleep() sur 99% du temps de la temporisation (cf vou_calibration_delta_t)
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
* @todo   : verifier si on peut augmenter la valeur de vou_calibration_delta_t en augmentant
*           la priorite du thread egalement
*****************************************************************************************/

long VOUTE_TEMPORISATION(STRUCT_VOUTE *lp_Vou, struct timeval t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  usleep ( (long)(lp_Vou->vou_dt_us * lp_Vou->vou_tempo_percent) ) ;
  
  t_diff=0;
  
  // FIXME : rappel : lp_Vou->vou_calibration_delta_t = 0.99 normalement
  // FIXME : pour eviter de faire appel a la boucle while 99% du temps
  
  while( t_diff < ( lp_Vou->vou_dt_us * lp_Vou->vou_calibration_delta_t ) ) {
     
   gettimeofday(&t01,NULL) ;
   t_diff = (double)(( t01.tv_sec - t00.tv_sec)  * TEMPS_MICRO_SEC ) + t01.tv_usec - t00.tv_usec;
  }
  return t_diff ;
}

/*****************************************************************************************
* @fn     : VOUTE_AFFICHER
* @author : s.gravois
* @brief  : Cette fonction affiche les infos liees a la voute STRUCT_VOUTE *
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : a completer eventuellement
*****************************************************************************************/

void VOUTE_AFFICHER( STRUCT_VOUTE * lp_Vou) {
	
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

	Trace1("lp_Vou->vou_num %lld", lp_Vou->vou_num) ;
  Trace1("lp_Vou->vou_pas %lld", lp_Vou->vou_num) ;

  Trace1("----------------------------") ;

}