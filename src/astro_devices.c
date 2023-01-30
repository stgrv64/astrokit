/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | * ajout entete
#               * remplacement key_mute par TIME
# --------------------------------------------------------------
# 15/11/2021  | * modification nom fct mainXxxx() pour compilation
#                 => mainIr <=> main
# 20/01/2022  | * ajout KEY_ZOOM (oubli) et action associee
#
# 21/03/2022  | * mise en place structure g_Cod (STRUCT_CODES)
#               * deport du code dans fichier config.c /.h
#               * renommage fonction LIRC_CODE => CONFIG_INIT_CODE
#
# 22/03/2022  | * deplacement de 2 fonctions dans config.c/.h
# --------------------------------------------------------------  
*/

#include "astro_devices.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/*****************************************************************************************
* @fn     : DEVICES_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_DEVICES *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void DEVICES_DISPLAY_PREPARE ( STRUCT_DEVICES * lp_Dev) {

  TraceArbo(__func__,2,"devices format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Dev->dev_mutex ) ;
/*
 "(cla) %d (inf) %d (lcd) %d (cap) %d (ctl) %d (blu) %d (raq) %d"
*/
  sprintf( lp_Dev->dev_dis_cmd , STR_DEV_FORMAT_0,\
    lp_Dev->dev_use_keyboard, \
    lp_Dev->dev_use_infrarouge, \
    lp_Dev->dev_use_lcd, \
    lp_Dev->dev_use_capteurs, \
    lp_Dev->dev_use_controler, \
    lp_Dev->dev_use_bluetooth, \
    lp_Dev->dev_use_raquette ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Dev->dev_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static DEVICES_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_DEVICES *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void DEVICES_DISPLAY(STRUCT_DEVICES *lp_Dev) {

  TraceArbo(__func__,2,"display informations on I/O") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  DEVICES_DISPLAY_PREPARE( lp_Dev ) ;

  ASTRO_GLOBAL_LOG_ON ( &  lp_Dev->dev_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Dev->dev_loglevel , 1 , "%s", lp_Dev->dev_dis_cmd ) ;
  ASTRO_GLOBAL_LOG_OFF( &  lp_Dev->dev_loglevel ) ;

  return ;
}
/*****************************************************************************************
* @fn     : DEVICES_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_DEVICES *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void DEVICES_LOCK ( STRUCT_DEVICES * lp_Dev) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Dev->dev_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : DEVICES_LOG
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_DEVICES *
* @date   : 2023-01-24 creation
* @todo   : (a completer )
*****************************************************************************************/

void DEVICES_LOG ( STRUCT_DEVICES * lp_Dev) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Dev->dev_mutex ) ;
  
  /* TODO : complete */ 

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Dev->dev_mutex ) ;
  
  return ;
}
/*****************************************************************************************
* @fn     : DEVICES_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_DEVICES *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void DEVICES_UNLOCK ( STRUCT_DEVICES * lp_Dev) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Dev->dev_mutex ) ;

  return ;
}

/*****************************************************************************************
* @fn     : DEVICES_INIT
* @author : s.gravois
* @brief  : Cette fonction configure la structure STRUCT_DEVICES *lp_Dev
* @param  : STRUCT_DEVICES *lp_Dev
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : (obsolete) les devices sont lues depuis le fichier de configuration
*****************************************************************************************/

void DEVICES_INIT(STRUCT_DEVICES *lp_Dev) {

  TraceArbo(__func__,0,"init devices") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Dev->dev_mutex ) ;
                                     lp_Dev->dev_log      = DEVICES_LOG ;
                                     lp_Dev->dev_lock     = DEVICES_LOCK ;
                                     lp_Dev->dev_unlock   = DEVICES_UNLOCK ;
                                     lp_Dev->dev_display  = DEVICES_DISPLAY ;
                                     lp_Dev->dev_loglevel = 0 ; 
                                     lp_Dev->dev_file     = NULL ;
  gettimeofday ( &                   lp_Dev->dev_tval, NULL ) ;

  lp_Dev->dev_use_capteurs    = gp_Dev_Par->dev_par_use_capteurs ;
  lp_Dev->dev_use_raquette    = gp_Dev_Par->dev_par_use_raquette ;
  lp_Dev->dev_use_bluetooth   = gp_Dev_Par->dev_par_use_bluetooth ;
  lp_Dev->dev_use_infrarouge  = gp_Dev_Par->dev_par_use_infrared ;
  lp_Dev->dev_use_controler   = gp_Dev_Par->dev_par_use_controler ;
  lp_Dev->dev_use_keyboard    = gp_Dev_Par->dev_par_use_keyboard ;
  lp_Dev->dev_use_lcd         = gp_Dev_Par->dev_par_use_lcd ; 
  lp_Dev->dev_init_capteurs = 0 ; 

  return ;
}
/*****************************************************************************************
* @fn     : CONFIG_PARAMS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_CONFIG_PARAMS *
* @param  : STRUCT_DEVICES_PARAMS *lp_Dev_Par
* @date   : 2022-12-11 creation 
* @todo   : 
*****************************************************************************************/

void DEVICES_PARAMS_INIT(STRUCT_DEVICES_PARAMS *lp_Dev_Par ) {
  
  TraceArbo(__func__,1,"init devicdes params") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Dev_Par->dev_par_mutex ) ;

  lp_Dev_Par->dev_par_use_capteurs  = 0 ;
  lp_Dev_Par->dev_par_use_raquette  = 0 ;
  lp_Dev_Par->dev_par_use_bluetooth = 0 ;
  lp_Dev_Par->dev_par_use_infrared  = 0 ;
  lp_Dev_Par->dev_par_use_controler = 0 ;
  lp_Dev_Par->dev_par_use_keyboard  = 0 ;
  lp_Dev_Par->dev_par_use_lcd       = 0 ; 

  return ;
}
/*****************************************************************************************
* @fn     : DEVICES_PARAMS_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche la structure STRUCT_DEVICES_PARAMS *
* @param  : STRUCT_DEVICES_PARAMS *lp_Dev_Par
* @date   : 2022-12-11 creation 
* @todo   : 
*****************************************************************************************/

void DEVICES_PARAMS_DISPLAY(STRUCT_DEVICES_PARAMS *lp_Dev_Par ) {
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %d", "lp_Dev_Par->dev_par_use_controler", lp_Dev_Par->dev_par_use_controler);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %d", "lp_Dev_Par->dev_par_use_capteurs ", lp_Dev_Par->dev_par_use_capteurs);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %d", "lp_Dev_Par->dev_par_use_bluetooth", lp_Dev_Par->dev_par_use_bluetooth);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %d", "lp_Dev_Par->dev_par_use_infrared ", lp_Dev_Par->dev_par_use_infrared);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %d", "lp_Dev_Par->dev_par_use_raquette ", lp_Dev_Par->dev_par_use_raquette);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %d", "lp_Dev_Par->dev_par_use_keyboard ", lp_Dev_Par->dev_par_use_keyboard);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %d", "lp_Dev_Par->dev_par_use_lcd      ", lp_Dev_Par->dev_par_use_lcd);

  return ;
}

/*****************************************************************************************
* @fn     : DEVICES_DISPLAY_UTILISATION
* @author : s.gravois
* @brief  : Cette fonction affiche le parametrage utilisation des devices 
* @param  : void
* @date   : 2022-03-12 creation
* @date   : 2022-10-08 rapatriement et changement de nom depuis config.c
*****************************************************************************************/

void DEVICES_DISPLAY_UTILISATION (STRUCT_DEVICES *lp_Dev) {

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1("lp_Dev->dev_use_infrarouge = %d",lp_Dev->dev_use_infrarouge) ;
  Trace1("lp_Dev->dev_use_capteurs   = %d",lp_Dev->dev_use_capteurs) ;
  Trace1("lp_Dev->dev_use_raquette   = %d",lp_Dev->dev_use_raquette) ;
  Trace1("lp_Dev->dev_use_bluetooth  = %d",lp_Dev->dev_use_bluetooth) ;
  Trace1("lp_Dev->dev_use_keyboard   = %d",lp_Dev->dev_use_keyboard) ;
  Trace1("lp_Dev->dev_use_controler  = %d",lp_Dev->dev_use_controler) ;
  Trace1("lp_Dev->dev_use_lcd        = %d",lp_Dev->dev_use_lcd) ;

  return ;
}

