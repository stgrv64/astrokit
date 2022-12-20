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
* @fn     : ASTRE_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_ASTRE *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void ASTRE_LOCK ( STRUCT_ASTRE * lp_Ast) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : ASTRE_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_ASTRE *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void ASTRE_UNLOCK ( STRUCT_ASTRE * lp_Ast) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex ) ;

  return ;
}

/*****************************************************************************************
* @fn     : LIEU_PARAMS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_LIEU_PARAMS *
* @param  : STRUCT_LIEU_PARAMS * lp_Lie_Par
* @date   : 2022-12-12 creation
*****************************************************************************************/

void LIEU_PARAMS_INIT(STRUCT_LIEU_PARAMS * lp_Lie_Par ) {

  char c_path_file_out[ CONFIG_TAILLE_BUFFER_256] ; 

  TraceArbo(__func__,1,"init lieu params") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Lie_Par->lie_par_mutex ) ;

  lp_Lie_Par->par_altitude = 0 ;
  lp_Lie_Par->par_latitude = 0 ;
  lp_Lie_Par->par_longitude = 0 ;

  return ;
}

/*****************************************************************************************
* @fn     : LIEU_PARAMS_DISPLAY
* @author : s.gravois
* @brief  : Cette affiche la structure STRUCT_LIEU_PARAMS *
* @param  : STRUCT_LIEU_PARAMS *lp_Lie_Par
* @date   : 2022-12-15 creation 
* @todo   : 
*****************************************************************************************/

void LIEU_PARAMS_DISPLAY(STRUCT_LIEU_PARAMS *lp_Lie_Par ) {
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("%-50s = %.2f" , "lp_Lie_Par->par_altitude ", lp_Lie_Par->par_altitude); 
  Trace("%-50s = %.2f" , "lp_Lie_Par->par_latitude ", lp_Lie_Par->par_latitude); 
  Trace("%-50s = %.2f" , "lp_Lie_Par->par_longitude", lp_Lie_Par->par_longitude); 

  return ;
}

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
 
  TraceArbo(__func__,0,"init lieu") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

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
* @fn     : LIEU_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations du lieu d observation
* @param  : STRUCT_LIEU *lp_Lie
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void LIEU_DISPLAY(STRUCT_LIEU *lp_Lie) {

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