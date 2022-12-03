/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 07/10/2022  | * creation
# -------------------------------------------------------------- 
*/

#include "astro_datas.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/*****************************************************************************************
* @fn     : DATAS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise les champs de la structure STRUCT_DATAS
* @param  : STRUCT_DATAS * lp_Dat
* @date   : 2022-10-08 creation 
* @todo   : 
*****************************************************************************************/

void DATAS_INIT ( STRUCT_DATAS * lp_Dat ) {

  int i =0 ;
  
  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Dat->dat_mutex ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Dat->dat_mutex ) ; ;

  memset( lp_Dat, 0, sizeof (STRUCT_DATAS ) ) ;
  strcpy( lp_Dat->dat_acc, ""  ) ;
  strcpy( lp_Dat->dat_bou, ""  ) ;
  strcpy( lp_Dat->dat_act, ""  ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Dat->dat_mutex ) ; ;

  return ;
}

/*****************************************************************************************
* @fn     : DATAS_RESET
* @author : s.gravois
* @brief  : Cette fonction reset les champs de la structure STRUCT_DATAS
* @param  : STRUCT_DATAS * lp_Dat
* @date   : 2022-12-01 creation 
* @todo   : ras
*****************************************************************************************/

void DATAS_RESET( STRUCT_DATAS * lp_Dat ) {

  TraceArbo(__func__,2,"reset") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Dat->dat_mutex ) ; ;

  memset( lp_Dat, 0, sizeof (STRUCT_DATAS ) ) ;

  strcpy( lp_Dat->dat_acc, ""  ) ;
  strcpy( lp_Dat->dat_bou, ""  ) ;
  strcpy( lp_Dat->dat_act, ""  ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Dat->dat_mutex ) ; ;

  return ;
}

/*****************************************************************************************
* @fn     : DATAS_ACTION_RESET
* @author : s.gravois
* @brief  : Cette fonction reset le champs dat_act de STRUCT_DATAS
* @param  : STRUCT_DATAS *lp_Dat
* @date   : 2022-12-01 creation 
* @todo   : utiliser memset ?
*****************************************************************************************/

void DATAS_ACTION_RESET( STRUCT_DATAS * lp_Dat ) {

  TraceArbo(__func__,2,"reset action field") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1("reset : lp_Dat->dat_act") ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Dat->dat_mutex ) ; ;

  strcpy( lp_Dat->dat_act, ""  ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Dat->dat_mutex ) ; ;

  return ;
}

/*****************************************************************************************
* @fn     : DATAS_ACTION_COPY
* @author : s.gravois
* @brief  : Cette fonction copie dans le champs dat_act de STRUCT_DATAS
* @param  : STRUCT_DATAS *lp_Dat
* @date   : 2022-12-01 creation 
* @todo   : 
*****************************************************************************************/

void DATAS_ACTION_COPY( STRUCT_DATAS * lp_Dat, const char * c_char ) {

  TraceArbo(__func__,2,"copy action field") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Dat->dat_mutex ) ; ;

  strcpy( lp_Dat->dat_act, c_char) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Dat->dat_mutex ) ; ;

  GPIO_LED_ETAT_CLIGNOTE(1,10) ;
  
  return ;
}

/*****************************************************************************************
 *
 * END FILE - END FILE - END FILE - END FILE - END FILE - END FILE - END FILE - END FILE - 
 * 
 * ************************************************************************************* */ 