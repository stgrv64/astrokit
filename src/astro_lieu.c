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

/*---------------------------------------------------*/
/* Parametres de fichier config (modif octobre 2022) */ 
/*---------------------------------------------------*/

double LIEU_LONGITUDE ;
double LIEU_LATITUDE ;
double LIEU_ALTITUDE ;

/*---------------------------------------------------*/
/* FIN Parametres de fichier config                  */ 
/*---------------------------------------------------*/

STRUCT_LIEU g_Lieu, *gp_Lie ;

/*****************************************************************************************
* @fn     : LIEU_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_LIEU *gp_Lie
* @param  : STRUCT_LIEU *gp_Lie
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-10-07 deplacement depuis config.c dans ce fichier
* @todo   : cf si JJ etc.. sont utiles dans la structure (parametres de temps)
*****************************************************************************************/

void LIEU_INIT(STRUCT_LIEU *gp_Lie) {
 
  /* ARBO(__func__,1,"") ; */ /* MACRO_DEBUG_ARBO_FONCTIONS */

  gp_Lie->JJ  = 0 ; // jour julien
  gp_Lie->TS  = 0 ;  // temps sideral
  gp_Lie->TSR = 0 ;  // temps sideral en radians
  gp_Lie->JD  = 0 ;  // jour decimal
  gp_Lie->lat = LIEU_LATITUDE / CALCUL_UN_RADIAN_EN_DEGRES ;
  gp_Lie->lon = LIEU_LONGITUDE / CALCUL_UN_RADIAN_EN_DEGRES ;
  gp_Lie->alt = LIEU_ALTITUDE ;
}

/*****************************************************************************************
* @fn     : LIEU_AFFICHER
* @author : s.gravois
* @brief  : Cette fonction affiche les informations du lieu d observation
* @param  : STRUCT_LIEU *gp_Lie
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void LIEU_AFFICHER(STRUCT_LIEU *gp_Lie) {

  ARBO(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1(" latitude   :  %.2f", gp_Lie->lat * CALCUL_UN_RADIAN_EN_DEGRES ) ; 
  Trace1(" longitude  :  %.2f", gp_Lie->lon * CALCUL_UN_RADIAN_EN_DEGRES ) ;
  Trace1("gp_Lie->JD    : %f",gp_Lie->JD) ;
  Trace1("gp_Lie->JJ    : %f",gp_Lie->JJ) ;
  Trace1("gp_Lie->TS    : %f",gp_Lie->TS) ;
  Trace1("gp_Lie->TSR   : %f",gp_Lie->TSR) ;

  Trace1("gp_Lie->alt = %f",gp_Lie->alt ) ;


  Trace1("gp_Lie->lat (deg) = %f",gp_Lie->lat * CALCUL_UN_RADIAN_EN_DEGRES ) ;
  Trace1("gp_Lie->lon (deg) = %f",gp_Lie->lon * CALCUL_UN_RADIAN_EN_DEGRES ) ;

  Trace1("----------------------------") ;
}