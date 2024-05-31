/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 07/10/2022  | * creation
# 27/05/2024  | * correction bug (suppression memset)
# -------------------------------------------------------------- 
*/

#include "astro_datas.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

static void DATAS_DISPLAY_PREPARE ( STRUCT_DATAS * ) ;
static void DATAS_DISPLAY        ( STRUCT_DATAS * ) ;
static void DATAS_LOCK           ( STRUCT_DATAS * ) ;
static void DATAS_UNLOCK         ( STRUCT_DATAS * ) ;

/*****************************************************************************************
* @fn     : DATAS_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_DATAS *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void DATAS_DISPLAY_PREPARE ( STRUCT_DATAS * lp_Dat) {

  TraceArbo(__func__,2,"astre format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Dat->dat_mutex ) ;

  sprintf( lp_Dat->dat_dis_cmd , STR_DAT_FORMAT_0,\
    lp_Dat->dat_act, \
    lp_Dat->dat_acc, \
    lp_Dat->dat_bou ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Dat->dat_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static DATAS_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_DATAS *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void DATAS_DISPLAY(STRUCT_DATAS *lp_Dat) {

  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,2,"display informations on Astre") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("") ; usleep(50000) ; 
  DATAS_DISPLAY_PREPARE( lp_Dat ) ;
  Trace("") ; usleep(50000) ; 
  ASTRO_GLOBAL_LOG_ON ( &  lp_Dat->dat_loglevel ) ;
  Trace("") ; usleep(50000) ; 
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Dat->dat_loglevel , 0 , "%s", lp_Dat->dat_dis_cmd ) ;
  Trace("") ; usleep(50000) ; 
  ASTRO_GLOBAL_LOG_OFF( &  lp_Dat->dat_loglevel ) ;
  Trace("") ; usleep(50000) ; 

  return ;
}
/*****************************************************************************************
* @fn     : DATAS_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_DATAS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void DATAS_LOCK ( STRUCT_DATAS * lp_Dat) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Dat->dat_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : DATAS_LOG
* @author : s.gravois
* @brief  : Log la structure en parametre 
* @param  : STRUCT_DATAS *
* @date   : 2023-01-24 creation
* @todo   : (a completer )
*****************************************************************************************/

static void DATAS_LOG ( STRUCT_DATAS * lp_Dat) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Dat->dat_mutex ) ;

  /* TODO : complete */ 

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Dat->dat_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : DATAS_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_DATAS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void DATAS_UNLOCK ( STRUCT_DATAS * lp_Dat) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Dat->dat_mutex ) ;

  return ;
}
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
  
  TraceArbo(__func__,0,"init datas") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Dat->dat_mutex ) ;
                                     lp_Dat->dat_log      = DATAS_LOG ;
                                     lp_Dat->dat_lock     = DATAS_LOCK ;
                                     lp_Dat->dat_unlock   = DATAS_UNLOCK ;
                                     lp_Dat->dat_display  = DATAS_DISPLAY ;
                                     lp_Dat->dat_loglevel = 0 ; 
                                     lp_Dat->dat_file     = NULL ;
  gettimeofday ( &                   lp_Dat->dat_tval, NULL ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Dat->dat_mutex ) ; ;

  // FIXME : correction bug 2024 : ne pas faire le memset ici !! 
  // memset( lp_Dat, 0, sizeof ( STRUCT_DATAS ) ) ;

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

  memset( lp_Dat, 0, sizeof ( STRUCT_DATAS ) ) ;

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

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Dat->dat_mutex ) ;

  strcpy( lp_Dat->dat_act, ""  ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Dat->dat_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : DATAS_ACTION_BUF_TO_DAT
* @author : s.gravois
* @brief  : Cette fonction copie dans le champs dat_act de STRUCT_DATAS la valeur en arg
* @param  : STRUCT_DATAS *lp_Dat
* @date   : 2022-12-01 creation 
* @todo   : (voir si on garde GPIO_LED_ETAT_CLIGNOTE )
*****************************************************************************************/

void DATAS_ACTION_BUF_TO_DAT( STRUCT_DATAS * lp_Dat, const char * c_char ) {

  TraceArbo(__func__,2,"copy action field") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  Trace1("cp datas %s on gp_Dat->dat_act", c_char ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Dat->dat_mutex ) ; ;

  strcpy( lp_Dat->dat_act, c_char) ;
  
  Trace("code / action : %s",  lp_Dat->dat_act ) ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Dat->dat_mutex ) ; ;

  // GPIO_LED_ETAT_CLIGNOTE(1,10) ;
  
  return ;
}
/*****************************************************************************************
* @fn     : DATAS_ACTION_DAT_TO_KEY
* @author : s.gravois
* @brief  : Cette fonction copie dans la valeur en arg , le champs dat_act de STRUCT_DATAS
* @param  : STRUCT_DATAS *lp_Dat
* @date   : 2022-12-01 creation 
* @todo   : (voir si on garde GPIO_LED_ETAT_CLIGNOTE )
*****************************************************************************************/

void DATAS_ACTION_DAT_TO_KEY( STRUCT_DATAS * lp_Dat, STRUCT_KEYS * lp_Key ) {

  TraceArbo(__func__,2,"copy action field") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  /* Si la chaine n'est pas vide , on traite */

  if ( strcmp(lp_Dat->dat_act,DATAS_ZERO_CHAR) != 0 ) {

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Key->key_mutex ) ;

    Trace("cp lp_Dat->dat_act %s to lp_Key->key_mot", lp_Dat->dat_act) ;

    memset( lp_Key->key_mot, 0, sizeof(lp_Key->key_mot)) ;   
    strcpy( lp_Key->key_mot, lp_Dat->dat_act) ;
      
    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Key->key_mutex ) ;

    lp_Key->key_mot_en_cours = 1 ;

    Trace("key_mot = %s",  lp_Key->key_mot ) ;
  }
  else {
    Trace1("lp_Dat->dat_act vide") ;
  }
  // GPIO_LED_ETAT_CLIGNOTE(1,10) ;
  
  return ;
}
/*****************************************************************************************
 *
 * END FILE - END FILE - END FILE - END FILE - END FILE - END FILE - END FILE - END FILE - 
 * 
 * ************************************************************************************* */ 