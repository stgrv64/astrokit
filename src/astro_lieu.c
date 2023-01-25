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
* @fn     : ASTRE_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_ASTRE *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void ASTRE_DISPLAY_PREPARE ( STRUCT_ASTRE * lp_Ast) {

  TraceArbo(__func__,2,"astre format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex ) ;

  sprintf( lp_Ast->ast_dis_cmd , STR_ASTRE_FORMAT,\
    lp_Ast->ast_nom, \
    lp_Ast->ast_hhmmss_asc, \
    lp_Ast->ast_ddmm_dec, \
    lp_Ast->ast_hhmmss_agh, \
    lp_Ast->ast_ddmm_azi, \
    lp_Ast->ast_ddmm_alt, \
    lp_Ast->ast_azi_vit, \
    lp_Ast->ast_alt_vit ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static ASTRE_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_ASTRE *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void ASTRE_DISPLAY(STRUCT_ASTRE *lp_Ast) {

  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,2,"display informations on Astre") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  ASTRE_DISPLAY_PREPARE( lp_Ast ) ;

  MACRO_ASTRO_GLOBAL_LOG_ON ( lp_Ast->ast_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Ast->ast_loglevel , 1 , "%s", lp_Ast->ast_dis_cmd ) ;
  MACRO_ASTRO_GLOBAL_LOG_OFF( lp_Ast->ast_loglevel ) ;

  return ;
}

/*****************************************************************************************
* @fn     : LIEU_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_LIEU *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void LIEU_LOCK ( STRUCT_LIEU * lp_Lie) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Lie->lie_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : LIEU_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_LIEU *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void LIEU_UNLOCK ( STRUCT_LIEU * lp_Lie) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Lie->lie_mutex ) ;

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

  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %.2f" , "lp_Lie_Par->par_altitude ", lp_Lie_Par->par_altitude); 
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %.2f" , "lp_Lie_Par->par_latitude ", lp_Lie_Par->par_latitude); 
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %.2f" , "lp_Lie_Par->par_longitude", lp_Lie_Par->par_longitude); 

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

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Lie->lie_mutex ) ;
                                     lp_Lie->lie_log      = LIEU_LOG ;
                                     lp_Lie->lie_lock     = LIEU_LOCK ;
                                     lp_Lie->lie_unlock   = LIEU_UNLOCK ;
                                     lp_Lie->lie_display  = LIEU_DISPLAY ;
                                     lp_Lie->lie_loglevel = 0 ; 
                                     lp_Lie->lie_file     = NULL ;
  gettimeofday (                   & lp_Lie->lie_tval, NULL ) ;  

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