/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
--------------------------------------------------------------
date        | commentaires 
--------------------------------------------------------------
07/10/2022  | * creation a partir du code de astro.c et autre
-------------------------------------------------------------- 
*/

#include "astro_lieu.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/*****************************************************************************************
* @fn     : LIEU_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_LIEU *lp_Lie
* @param  : STRUCT_LIEU *lp_Lie
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-10-07 deplacement depuis config.c dans ce fichier
* @todo   : cf si JJ etc.. sont utiles dans la structure (parametres de temps)
*****************************************************************************************/

void LIEU_INIT(STRUCT_LIEU *lp_Lie) {
 
  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  pthread_mutex_init( & lp_Lie->lie_mutex, NULL ) ;

  lp_Lie->lie_jj  = 0 ; // jour julien
  lp_Lie->lie_ts  = 0 ;  // temps sideral
  lp_Lie->lie_tsr = 0 ;  // temps sideral en radians
  lp_Lie->lie_jd  = 0 ;  // jour decimal
  lp_Lie->lie_lat = gp_Lie_Par->par_latitude / CALCULS_UN_RADIAN_EN_DEGRES ;
  lp_Lie->lie_lon = gp_Lie_Par->par_longitude / CALCULS_UN_RADIAN_EN_DEGRES ;
  lp_Lie->lie_alt = gp_Lie_Par->par_altitude ;
}

/*****************************************************************************************
* @fn     : LIEU_AFFICHER
* @author : s.gravois
* @brief  : Cette fonction affiche les informations du lieu d observation
* @param  : STRUCT_LIEU *lp_Lie
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void LIEU_AFFICHER(STRUCT_LIEU *lp_Lie) {

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1(" latitude   :  %.2f", lp_Lie->lie_lat * CALCULS_UN_RADIAN_EN_DEGRES ) ; 
  Trace1(" longitude  :  %.2f", lp_Lie->lie_lon * CALCULS_UN_RADIAN_EN_DEGRES ) ;
  Trace1("lp_Lie->lie_jd    : %f",lp_Lie->lie_jd) ;
  Trace1("lp_Lie->lie_jj    : %f",lp_Lie->lie_jj) ;
  Trace1("lp_Lie->lie_ts    : %f",lp_Lie->lie_ts) ;
  Trace1("lp_Lie->lie_tsr   : %f",lp_Lie->lie_tsr) ;

  Trace1("lp_Lie->lie_alt = %f",lp_Lie->lie_alt ) ;


  Trace1("lp_Lie->lie_lat (deg) = %f",lp_Lie->lie_lat * CALCULS_UN_RADIAN_EN_DEGRES ) ;
  Trace1("lp_Lie->lie_lon (deg) = %f",lp_Lie->lie_lon * CALCULS_UN_RADIAN_EN_DEGRES ) ;

  Trace1("----------------------------") ;
}