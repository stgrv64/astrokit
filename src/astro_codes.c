/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
--------------------------------------------------------------
date           | commentaires 
--------------------------------------------------------------
octobre 2022 :
  * creation
  * recuperation du contenu des fonctions depuis config.c               
-------------------------------------------------------------- */

#include "astro_codes.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

static void CODES_DISPLAY_PREPARE ( STRUCT_CODES * ) ;
static void CODES_DISPLAY        ( STRUCT_CODES * ) ;
static void CODES_UNLOCK         ( STRUCT_CODES * ) ;
static void CODES_LOCK           ( STRUCT_CODES * ) ;
static void CODES_LOG            ( STRUCT_CODES * ) ;

static void CODES_DISPLAY_INDEX_WITH_OUT_ACT (STRUCT_CODES *lp_Cod, const char * ) ;

/*****************************************************************************************
* @fn     : CODES_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_CODES *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void CODES_DISPLAY_PREPARE ( STRUCT_CODES * lp_Cod) {

  int i_pos =0;

  TraceArbo(__func__,2,"astre format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Cod->cod_mutex ) ;

  sprintf( lp_Cod->cod_dis_cmd, STR_COD_FORMAT_0,\

    lp_Cod->cod_in_lirc [ lp_Cod->cod_index ] , \
    lp_Cod->cod_in_term [ lp_Cod->cod_index ] , \
    lp_Cod->cod_out_act [ lp_Cod->cod_index ]  ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Cod->cod_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static CODES_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur la structure dédiée
* @param  : STRUCT_CODES *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void CODES_DISPLAY (STRUCT_CODES *lp_Cod) {

  int i_pos=0 ;

  TraceArbo(__func__,2,"display informations on Codes") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  ASTRO_GLOBAL_LOG_ON ( &  lp_Cod->cod_loglevel ) ;

  for(i_pos=0 ; i_pos < CODES_CODE_NB_CODES ; i_pos++ ) {

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Cod->cod_mutex ) ;

    lp_Cod->cod_index = i_pos ;
    
    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Cod->cod_mutex ) ;

    CODES_DISPLAY_PREPARE( lp_Cod ) ;

    MACRO_ASTRO_GLOBAL_LOG( lp_Cod->cod_loglevel , 1 , "%s", lp_Cod->cod_dis_cmd ) ;
  }

  ASTRO_GLOBAL_LOG_OFF( &  lp_Cod->cod_loglevel ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static CODES_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur la structure dédiée
* @param  : STRUCT_CODES *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void CODES_DISPLAY_INDEX_WITH_OUT_ACT (STRUCT_CODES *lp_Cod, const char * c_out_act) {

  int i_pos=0 ;

  TraceArbo(__func__,2,"display informations on Codes") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  for(i_pos=0 ; i_pos < CODES_CODE_NB_CODES ; i_pos++ ) {

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Cod->cod_mutex ) ;

    lp_Cod->cod_index = i_pos ;
    
    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Cod->cod_mutex ) ;

    if ( ! strcmp( c_out_act, lp_Cod->cod_out_act[i_pos] ) ) {
      ASTRO_GLOBAL_LOG_ON ( &  lp_Cod->cod_loglevel ) ;
      CODES_DISPLAY_PREPARE( lp_Cod ) ;
      MACRO_ASTRO_GLOBAL_LOG( lp_Cod->cod_loglevel , 1 , "%s", lp_Cod->cod_dis_cmd ) ;
      ASTRO_GLOBAL_LOG_OFF( &  lp_Cod->cod_loglevel ) ;
      break ;
    }
  }

  return ;
}

/*****************************************************************************************
* @fn     : CODES_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_CODES *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void CODES_LOCK ( STRUCT_CODES * lp_Cod) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Cod->cod_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CODES_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_CODES *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void CODES_UNLOCK ( STRUCT_CODES * lp_Cod) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Cod->cod_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CODES_LOG
* @author : s.gravois
* @brief  : Log la structure en parametre
* @param  : STRUCT_CODES *
* @date   : 2023-01-11 creation
*****************************************************************************************/

static void CODES_LOG ( STRUCT_CODES * lp_Cod) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Cod->cod_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CODES_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure des codes *STRUCT_CODES
* @brief  : etablie la correspondance entre les KEY du fichier lircd.conf (partie LIRC)
* @brief  : et les codes utilises dans le programme principal (car pas de hachage en c)
* @brief  : etablie aussi la correspondance entre les KEY du clavier termios (partie TERMIOS)
* @brief  : et les codes utilises dans le programme principal (code_action)
* @param  : STRUCT_CODES *lp_Cod
* @date   : 2022-03-21 creation entete
* @todo   : modifier : completer avec une fonction de hachage (regarder si API sur net)
*****************************************************************************************/

void CODES_INIT(STRUCT_CODES *lp_Cod) {

  int i_pos=0 ;
  int j_pos=0 ;

  TraceArbo(__func__,1,"init codes") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Cod->cod_mutex ) ;
                                     lp_Cod->cod_lock        = CODES_LOCK ;
                                     lp_Cod->cod_unlock      = CODES_UNLOCK ;
                                     lp_Cod->cod_log         = CODES_LOG ;
                                     lp_Cod->cod_display     = CODES_DISPLAY ;
                                     lp_Cod->cod_display_act = CODES_DISPLAY_INDEX_WITH_OUT_ACT ;
                                     
                                     lp_Cod->cod_loglevel = 0 ; 
                                     lp_Cod->cod_file     = NULL ;
  gettimeofday ( &                   lp_Cod->cod_tval, NULL ) ;

  for( i_pos=0 ; i_pos<CODES_CODE_NB_CODES ; i_pos++ ) {
    memset( lp_Cod->cod_in_term[i_pos],  CONFIG_ZERO_CHAR, sizeof(lp_Cod->cod_in_term[i_pos]) ) ;
    memset( lp_Cod->cod_in_lirc[i_pos],  CONFIG_ZERO_CHAR, sizeof(lp_Cod->cod_in_lirc[i_pos]) ) ;
    memset( lp_Cod->cod_out_act[i_pos],  CONFIG_ZERO_CHAR, sizeof(lp_Cod->cod_out_act[i_pos]) ) ;

    for( j_pos=0 ; j_pos<CODES_NB_IN_OUT ; j_pos++ ) {
      memset( lp_Cod->cod_hach_codes[i_pos][j_pos], CONFIG_ZERO_CHAR, sizeof(lp_Cod->cod_hach_codes[i_pos][j_pos] )) ;
      strcpy( lp_Cod->cod_hach_codes[i_pos][j_pos], gc_hach_codes[i_pos][j_pos] ) ;
    }
  }
  
  for( i_pos=0 ; i_pos<CODES_CODE_NB_CODES ; i_pos++ ) {

    strcpy( lp_Cod->cod_in_term[ i_pos ], lp_Cod->cod_hach_codes[i_pos][ CODES_0_POS_IN_TERM ] ) ;
    strcpy( lp_Cod->cod_in_lirc[ i_pos ], lp_Cod->cod_hach_codes[i_pos][ CODES_1_POS_IN_LIRC] ) ;
    strcpy( lp_Cod->cod_out_act[ i_pos ], lp_Cod->cod_hach_codes[i_pos][ CODES_2_POS_OUT_ACT] ) ;
  }
  // FIXME :  ATTENTION !!! 
  // le nom des lp_Cod codes en MAJUSCULE servent a definir des actions dans config.c et le reste du programme

  return ;
}