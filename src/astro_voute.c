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
* @brief  : Cette fonction initialise la structure STRUCT_VOUTE *gp_Vou
* @param  : STRUCT_VOUTE *gp_Vou
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           pourcentage_tempo et calibration_voute
*****************************************************************************************/

void VOUTE_INIT(STRUCT_VOUTE *gp_Vou) {
  
  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  gp_Vou->dt                = 0  ;
  gp_Vou->pourcentage_tempo = 0.96 ;   
  gp_Vou->calibration_voute = 0.97 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
  gp_Vou->num               = 0 ;
  gp_Vou->deb               = 0 ;
  gp_Vou->fin               = 0 ;
  gp_Vou->acc_old           = 1 ;
  gp_Vou->acc               = 1 ;
  gp_Vou->pas               = gp_Vou->dt * CALCUL_VIT_ROT_EARTH_RAD_PER_SEC ;
  /* dt en micro-sec */

  gp_Vou->DT = (unsigned long)( gp_Vou->dt * TEMPS_MICRO_SEC / gp_Vou->acc ) ;   
}


/*****************************************************************************************
* @fn     : VOUTE_CONFIG
* @author : s.gravois
* @brief  : Cette fonction configure la structure STRUCT_VOUTE *gp_Vou
* @param  : STRUCT_VOUTE *gp_Vou
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           pourcentage_tempo et calibration_voute
*****************************************************************************************/

void VOUTE_CONFIG( STRUCT_VOUTE *gp_Vou, double dt, double acc, double percent ) {
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  gp_Vou->dt                 = dt  ;
  gp_Vou->pourcentage_tempo  = percent ; 
  gp_Vou->calibration_voute  = 0.99 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
  gp_Vou->deb                = 0 ;
  gp_Vou->num                = 0 ;
  gp_Vou->fin                = CALCUL_PI_FOIS_DEUX ;
  gp_Vou->acc_old            = gp_Vou->acc ;
  gp_Vou->acc                = acc ;
  gp_Vou->pas                = gp_Vou->dt * CALCUL_VIT_ROT_EARTH_RAD_PER_SEC ;
  /* dt en micro-sec */
  gp_Vou->DT = (unsigned long)( gp_Vou->dt * TEMPS_MICRO_SEC / gp_Vou->acc ) ;
}


/*****************************************************************************************
* @fn     : VOUTE_TEMPORISATION
* @author : s.gravois
* @brief  : permet de faire une temporisation (1s) dans la boucle infinie de SUIVI_VOUTE
* @brief  : utilise un usleep() sur 99% du temps de la temporisation (cf calibration_voute)
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
* @todo   : verifier si on peut augmenter la valeur de calibration_voute en augmentant
*           la priorite du thread egalement
*****************************************************************************************/

long VOUTE_TEMPORISATION(STRUCT_VOUTE *gp_Vou, struct timeval t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  usleep ( (long)(gp_Vou->DT * gp_Vou->pourcentage_tempo) ) ;
  
  t_diff=0;
  
  // FIXME : rappel : gp_Vou->calibration_voute = 0.99 normalement
  // FIXME : pour eviter de faire appel a la boucle while 99% du temps
  
  while( t_diff < ( gp_Vou->DT * gp_Vou->calibration_voute ) ) {
     
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

void VOUTE_AFFICHER( STRUCT_VOUTE * gp_Vou) {
	
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

	Trace1("gp_Vou->num %lld", gp_Vou->num) ;
  Trace1("gp_Vou->pas %lld", gp_Vou->num) ;

  Trace1("----------------------------") ;

}