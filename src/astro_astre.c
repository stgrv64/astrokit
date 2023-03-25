/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-10-07  | * creation
# 2022-10-07  | * rapatriement code depuis partout
# -------------------------------------------------------------- 
*/

#include "astro_astre.h"

/* Les macros suivantes incluent des eelements extern depuis global.c/.h */
/* pour faire apparaitre les variables globales ; Il est necessaire de la declarer dans le .c */

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

static void ASTRE_DISPLAY_PREPARE ( STRUCT_ASTRE * ) ;
static void ASTRE_DISPLAY        ( STRUCT_ASTRE * ) ;
static void ASTRE_LOCK           ( STRUCT_ASTRE * ) ;
static void ASTRE_UNLOCK         ( STRUCT_ASTRE * ) ;

/*****************************************************************************************
* @fn     : static ASTRE_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_ASTRE *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void ASTRE_DISPLAY_PREPARE ( STRUCT_ASTRE * lp_Ast) {

  TraceArbo(__func__,2,"astre format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  ASTRE_FORMATE_DONNEES_AFFICHAGE(lp_Ast) ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex ) ;

  sprintf( lp_Ast->ast_dis_cmd , STR_AST_FORMAT_0,\
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

  ASTRO_GLOBAL_LOG_ON ( &  lp_Ast->ast_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Ast->ast_loglevel , 1 , "%s", lp_Ast->ast_dis_cmd ) ;
  ASTRO_GLOBAL_LOG_OFF( &  lp_Ast->ast_loglevel ) ;

  return ;
}

/*****************************************************************************************
* @fn     : ASTRE_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_ASTRE *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void ASTRE_LOCK ( STRUCT_ASTRE * lp_Ast) {

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
* @fn     : ASTRE_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_ASTRE * lp_Ast
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-20 ajout membres de la structure dans la fonction
* @date   : 2022-10-07 deplacement depuis config.c dans ce fichier
* @todo   : supprimer STRUCT_ASTRE *lp_Ast (var blog) et remplacer par void
*           passer par des pointeurs de fonctions dans un structure
*****************************************************************************************/

void ASTRE_INIT(STRUCT_ASTRE *lp_Ast ) {

  int C ;
  
  TraceArbo(__func__,0,"init astre") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Ast->ast_mutex ) ;
                                     lp_Ast->ast_lock     = ASTRE_LOCK ;
                                     lp_Ast->ast_unlock   = ASTRE_UNLOCK ;
                                     lp_Ast->ast_log      = ASTRE_LOG ;
                                     lp_Ast->ast_display  = ASTRE_DISPLAY ;
                        lp_Ast->ast_display_format_datas  = ASTRE_FORMATE_DONNEES_AFFICHAGE ;                 
                                     lp_Ast->ast_loglevel = 0 ;
                                     lp_Ast->ast_file     = NULL ;
  gettimeofday ( &                   lp_Ast->ast_tval, NULL ) ;

  for(C=0; C< ASTRE_NB_COLONNES;C++) {
    memset( lp_Ast->ast_plus_proche[C], CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);
    strcpy( lp_Ast->ast_plus_proche[C], "") ;
  }
  memset( lp_Ast->ast_nom,   CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  memset( lp_Ast->ast_infos, CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  memset( lp_Ast->ast_plus_proche, CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  
  lp_Ast->ast_azi     = 0 ;
  lp_Ast->ast_alt     = 0 ;
  lp_Ast->ast_agh     = 0 ; 
  lp_Ast->ast_asc     = 0 ;
  lp_Ast->ast_dec     = 0 ;
  lp_Ast->ast_asc_sav = 0 ;
  lp_Ast->ast_dec_sav = 0 ;
  lp_Ast->ast_azi_vit = 0 ;
  lp_Ast->ast_alt_vit = 0 ;
  lp_Ast->ast_r3_x    = 0 ;
  lp_Ast->ast_r3_xx   = 0 ;
  lp_Ast->ast_r3_y    = 0 ;
  lp_Ast->ast_r3_yy   = 0 ;
  lp_Ast->ast_r3_z    = 0 ;
  lp_Ast->ast_r3_zz   = 0 ;
  
  lp_Ast->ast_typ = ASTRE_INDETERMINE ;
  lp_Ast->ast_num = 0 ;
  lp_Ast->ast_new = TRUE ;

  TIME_INIT( & lp_Ast->ast_at     ) ;
  TIME_INIT( & lp_Ast->ast_ht     ) ;
  TIME_INIT( & lp_Ast->ast_azi_t  ) ;
  TIME_INIT( & lp_Ast->ast_alt_t  ) ;
  TIME_INIT( & lp_Ast->ast_dec_t  ) ;
  TIME_INIT( & lp_Ast->ast_asc_t  ) ;   
  TIME_INIT( & lp_Ast->ast_agh_t  ) ;
  TIME_INIT( & lp_Ast->ast_agh0_t ) ;  
  TIME_INIT( & lp_Ast->ast_agh1_t ) ; 
  TIME_INIT( & lp_Ast->ast_agh2_t ) ;  
  TIME_INIT( & lp_Ast->ast_azi0_t ) ;
  TIME_INIT( & lp_Ast->ast_azi1_t ) ;
  TIME_INIT( & lp_Ast->ast_azi2_t ) ;

  CALCULS_INIT_ANGLE ( & lp_Ast->ast_azi_a ) ; 
  CALCULS_INIT_ANGLE ( & lp_Ast->ast_alt_a ) ;
  CALCULS_INIT_ANGLE ( & lp_Ast->ast_dec_a ) ; 
  CALCULS_INIT_ANGLE ( & lp_Ast->ast_asc_a ) ;
  CALCULS_INIT_ANGLE ( & lp_Ast->ast_agh_a ) ;

  return ;
}
/*****************************************************************************************
* @fn     : ASTRE_PARAMS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_ASTRE_PARAMS *
* @param  : void
* @date   : 2022-12-11 creation 
* @todo   : 
*****************************************************************************************/

void ASTRE_PARAMS_INIT(STRUCT_ASTRE_PARAMS *lp_Ast_Par ) {
  
  TraceArbo(__func__,1,"init astre params") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Ast_Par->ast_par_mutex ) ;


  memset( lp_Ast_Par->ast_par_default_object, CALCULS_ZERO_CHAR, sizeof( lp_Ast_Par->ast_par_default_object ) ) ;
  
  /* TODO : finalize */
}
/*****************************************************************************************
* @fn     : ASTRE_PARAMS_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_ASTRE_PARAMS *
* @param  : void
* @date   : 2022-12-11 creation 
* @todo   : 
*****************************************************************************************/

void ASTRE_PARAMS_DISPLAY(STRUCT_ASTRE_PARAMS *lp_Ast_Par ) {
  
  TraceArbo(__func__,1,"astre params display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %s","lp_Ast_Par->ast_par_default_object", lp_Ast_Par->ast_par_default_object );  

  return ;
}

/*****************************************************************************************
* @fn     : ASTRE_RESET
* @author : s.gravois
* @brief  : Cette fonction reset la structure STRUCT_ASTRE * lp_Ast
* @param  : STRUCT_ASTRE *
* @date   : 2022-12-01 creation (a l identique de ASTRE_INIT)
* @todo   : 
*****************************************************************************************/

void ASTRE_RESET(STRUCT_ASTRE * lp_Ast) {

  int C ;
  
  TraceArbo(__func__,0,"reset") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  lp_Ast->ast_lock(lp_Ast) ;
  // HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Ast->ast_mutex ) ; ;

  for(C=0; C< ASTRE_NB_COLONNES;C++) {
    memset( lp_Ast->ast_plus_proche[C], CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);
    strcpy( lp_Ast->ast_plus_proche[C], "") ;
  }
  memset( lp_Ast->ast_nom,   CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  memset( lp_Ast->ast_infos, CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  memset( lp_Ast->ast_plus_proche, CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);

  lp_Ast->ast_typ = ASTRE_INDETERMINE ;
  lp_Ast->ast_num = 0 ;
  lp_Ast->ast_new = TRUE ;
  lp_Ast->ast_azi     = 0  ;
  lp_Ast->ast_alt     = 0  ;
  lp_Ast->ast_agh     = 0 ; 
  lp_Ast->ast_asc     = 0 ;
  lp_Ast->ast_dec     = 0 ;
  lp_Ast->ast_azi_vit = 0 ;
  lp_Ast->ast_alt_vit = 0 ;
  lp_Ast->ast_r3_x    = 0 ;
  lp_Ast->ast_r3_xx   = 0 ;
  lp_Ast->ast_r3_y    = 0 ;
  lp_Ast->ast_r3_yy   = 0 ;
  lp_Ast->ast_r3_z    = 0 ;
  lp_Ast->ast_r3_zz   = 0 ;

  lp_Ast->ast_unlock( lp_Ast ) ;
}

/*****************************************************************************************
* @fn     : ASTRE_FORMATE_DONNEES_AFFICHAGE
* @author : s.gravois
* @brief  : Cette fonction formate divers string en vue d un affichage pertinent
* @param  : STRUCT_ASTRE *lp_Ast
* @date   : 2022-04-12 creation
* @date   : 2022-04-21 remplacement ° par o (affichage LCD ° impossible)
* @date   : 2022-04-21 ajout de 2 resolutions plus simple (affichage contraint par LCD) :
* @date   : 2022-04-21 - c_hhmm_*
* @date   : 2022-04-21 - c_dd_*  
*****************************************************************************************/

void ASTRE_FORMATE_DONNEES_AFFICHAGE(STRUCT_ASTRE *lp_Ast) {

  char  c_hhmmss_agh[ CONFIG_TAILLE_BUFFER_32] ;
  char  c_hhmmss_asc[ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_hhmmss_azi[ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_hhmmss_alt[ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_hhmmss_dec[ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_hhmm_agh  [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_hhmm_asc  [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_hhmm_azi  [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_hhmm_alt  [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_hhmm_dec  [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_ddmm_agh  [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_ddmm_asc  [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_ddmm_azi  [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_ddmm_alt  [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_ddmm_dec  [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_dd_agh    [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_dd_asc    [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_dd_azi    [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_dd_alt    [ CONFIG_TAILLE_BUFFER_32 ] ;
  char  c_dd_dec    [ CONFIG_TAILLE_BUFFER_32 ] ;

  TraceArbo(__func__,2,"format astre display datas") ;

  memset( c_hhmmss_agh, 0, sizeof(c_hhmmss_agh) ) ;
  memset( c_hhmmss_asc, 0, sizeof(c_hhmmss_asc) ) ;
  memset( c_hhmmss_azi, 0, sizeof(c_hhmmss_azi) ) ;
  memset( c_hhmmss_alt, 0, sizeof(c_hhmmss_alt) ) ;
  memset( c_hhmmss_dec, 0, sizeof(c_hhmmss_dec) ) ;

  memset( c_hhmm_agh, 0, sizeof(c_hhmm_agh) ) ;
  memset( c_hhmm_asc, 0, sizeof(c_hhmm_asc) ) ;
  memset( c_hhmm_azi, 0, sizeof(c_hhmm_azi) ) ;
  memset( c_hhmm_alt, 0, sizeof(c_hhmm_alt) ) ;
  memset( c_hhmm_dec, 0, sizeof(c_hhmm_dec) ) ;

  memset( c_ddmm_agh, 0, sizeof(c_ddmm_agh) ) ;
  memset( c_ddmm_asc, 0, sizeof(c_ddmm_asc) ) ;
  memset( c_ddmm_azi, 0, sizeof(c_ddmm_azi) ) ;
  memset( c_ddmm_alt, 0, sizeof(c_ddmm_alt) ) ;
  memset( c_ddmm_dec, 0, sizeof(c_ddmm_dec) ) ;

  memset( c_dd_agh, 0, sizeof(c_dd_agh) ) ;
  memset( c_dd_asc, 0, sizeof(c_dd_asc) ) ;
  memset( c_dd_azi, 0, sizeof(c_dd_azi) ) ;
  memset( c_dd_alt, 0, sizeof(c_dd_alt) ) ;
  memset( c_dd_dec, 0, sizeof(c_dd_dec) ) ;

  /* traitement des donnees en heures / minutes / secondes */

  sprintf( c_hhmmss_agh, " %3dh%2dm%2ds",  lp_Ast->ast_agh_t.tim_HH, lp_Ast->ast_agh_t.tim_MM, lp_Ast->ast_agh_t.tim_SS  ) ;
  sprintf( c_hhmmss_asc, " %3dh%2dm%2ds",  lp_Ast->ast_asc_t.tim_HH, lp_Ast->ast_asc_t.tim_MM, lp_Ast->ast_asc_t.tim_SS  ) ;
  sprintf( c_hhmmss_azi, " %3dh%2dm%2ds",  lp_Ast->ast_azi_t.tim_HH, lp_Ast->ast_azi_t.tim_MM, lp_Ast->ast_azi_t.tim_SS  ) ;
  sprintf( c_hhmmss_alt, " %3dh%2dm%2ds",  lp_Ast->ast_alt_t.tim_HH, lp_Ast->ast_alt_t.tim_MM, lp_Ast->ast_alt_t.tim_SS  ) ;
  sprintf( c_hhmmss_dec, " %3dh%2dm%2ds",  lp_Ast->ast_dec_t.tim_HH, lp_Ast->ast_dec_t.tim_MM, lp_Ast->ast_dec_t.tim_SS  ) ;

  sprintf( c_hhmm_agh,   " %3d h %2d m",   lp_Ast->ast_agh_t.tim_HH, lp_Ast->ast_agh_t.tim_MM ) ;
  sprintf( c_hhmm_asc,   " %3d h %2d m",   lp_Ast->ast_asc_t.tim_HH, lp_Ast->ast_asc_t.tim_MM ) ;
  sprintf( c_hhmm_azi,   " %3d h %2d m",   lp_Ast->ast_azi_t.tim_HH, lp_Ast->ast_azi_t.tim_MM ) ;
  sprintf( c_hhmm_alt,   " %3d h %2d m",   lp_Ast->ast_alt_t.tim_HH, lp_Ast->ast_alt_t.tim_MM ) ;
  sprintf( c_hhmm_dec,   " %3d h %2d m",   lp_Ast->ast_dec_t.tim_HH, lp_Ast->ast_dec_t.tim_MM ) ;

  /* traitement des donnees en degres / minutes / secondes */
  /* est inclus dans l affichage le signe */

  sprintf( c_ddmm_agh, "%c %-3d d %d'", lp_Ast->ast_agh_a.ang_sig, lp_Ast->ast_agh_a.ang_dd, lp_Ast->ast_agh_a.ang_mm ) ;
  sprintf( c_ddmm_asc, "%c %-3d d %d'", lp_Ast->ast_asc_a.ang_sig, lp_Ast->ast_asc_a.ang_dd, lp_Ast->ast_asc_a.ang_mm ) ;
  sprintf( c_ddmm_azi, "%c %-3d d %d'", lp_Ast->ast_azi_a.ang_sig, lp_Ast->ast_azi_a.ang_dd, lp_Ast->ast_azi_a.ang_mm ) ;
  sprintf( c_ddmm_alt, "%c %-3d d %d'", lp_Ast->ast_alt_a.ang_sig, lp_Ast->ast_alt_a.ang_dd, lp_Ast->ast_alt_a.ang_mm ) ;
  sprintf( c_ddmm_dec, "%c %-3d d %d'", lp_Ast->ast_dec_a.ang_sig, lp_Ast->ast_dec_a.ang_dd, lp_Ast->ast_dec_a.ang_mm ) ;

  sprintf( c_dd_agh,   "%c %-3d deg",   lp_Ast->ast_agh_a.ang_sig, lp_Ast->ast_agh_a.ang_dd ) ;
  sprintf( c_dd_asc,   "%c %-3d deg",   lp_Ast->ast_asc_a.ang_sig, lp_Ast->ast_asc_a.ang_dd ) ;
  sprintf( c_dd_azi,   "%c %-3d deg",   lp_Ast->ast_azi_a.ang_sig, lp_Ast->ast_azi_a.ang_dd ) ;
  sprintf( c_dd_alt,   "%c %-3d deg",   lp_Ast->ast_alt_a.ang_sig, lp_Ast->ast_alt_a.ang_dd ) ;
  sprintf( c_dd_dec,   "%c %-3d deg",   lp_Ast->ast_dec_a.ang_sig, lp_Ast->ast_dec_a.ang_dd ) ;

  /* Sauvegarde des donnees formatees dans la structure astre */
  
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Ast->ast_mutex ) ;

  strcpy( lp_Ast->ast_hhmmss_agh, c_hhmmss_agh)  ;
  strcpy( lp_Ast->ast_hhmmss_asc, c_hhmmss_asc)  ;
  strcpy( lp_Ast->ast_hhmmss_azi, c_hhmmss_azi)  ;
  strcpy( lp_Ast->ast_hhmmss_alt, c_hhmmss_alt)  ;
  strcpy( lp_Ast->ast_hhmmss_dec, c_hhmmss_dec)  ;

  strcpy( lp_Ast->ast_hhmm_agh, c_hhmm_agh)  ;
  strcpy( lp_Ast->ast_hhmm_asc, c_hhmm_asc)  ;
  strcpy( lp_Ast->ast_hhmm_azi, c_hhmm_azi)  ;
  strcpy( lp_Ast->ast_hhmm_alt, c_hhmm_alt)  ;
  strcpy( lp_Ast->ast_hhmm_dec, c_hhmm_dec)  ;

  strcpy( lp_Ast->ast_ddmm_agh, c_ddmm_agh)  ;
  strcpy( lp_Ast->ast_ddmm_asc, c_ddmm_asc)  ;
  strcpy( lp_Ast->ast_ddmm_azi, c_ddmm_azi)  ;
  strcpy( lp_Ast->ast_ddmm_alt, c_ddmm_alt)  ;
  strcpy( lp_Ast->ast_ddmm_dec, c_ddmm_dec)  ;

  strcpy( lp_Ast->ast_dd_agh, c_dd_agh)  ;
  strcpy( lp_Ast->ast_dd_asc, c_dd_asc)  ;
  strcpy( lp_Ast->ast_dd_azi, c_dd_azi)  ;
  strcpy( lp_Ast->ast_dd_alt, c_dd_alt)  ;
  strcpy( lp_Ast->ast_dd_dec, c_dd_dec)  ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Ast->ast_mutex ) ;

  return ;
}

/*****************************************************************************************
* @fn     : ASTRE_LOG
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_ASTRE *
* @date   : 2023-01-07 creation 
* @todo   : (a modifier , c une recopie depuis ast_disp)
*****************************************************************************************/

void ASTRE_LOG(STRUCT_ASTRE *lp_Ast) {

  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,2,"keys log") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK  ( & lp_Ast->ast_mutex ) ;
  ASTRO_GLOBAL_LOG_ROTATE    ( &  lp_Ast->ast_loglevel ) ;
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex) ;

  Trace("Va / Vh    : %3.2f / %3.2f" , lp_Ast->ast_azi_vit    , lp_Ast->ast_alt_vit ) ;
  Trace("AD / Dec   : %s / %s"       , lp_Ast->ast_hhmmss_asc , lp_Ast->ast_ddmm_dec ) ;
  Trace("AH / Dec   : %s / %s"       , lp_Ast->ast_hhmmss_agh , lp_Ast->ast_ddmm_dec ) ;
  Trace("AZ./ Haut. : %s / %s"       , lp_Ast->ast_ddmm_azi   , lp_Ast->ast_ddmm_alt ) ;

  sprintf( c_cmd , "(nom) %-15s (asc) %-15s (dec) %-15s (agh) %-15s (azi) %-15s (alt) %-5s (Va) %3.2f (Vh) %3.2f",\
    lp_Ast->ast_nom, \
    lp_Ast->ast_hhmmss_asc, \
    lp_Ast->ast_ddmm_dec, \
    lp_Ast->ast_hhmmss_agh, \
    lp_Ast->ast_ddmm_azi, \
    lp_Ast->ast_ddmm_alt, \
    lp_Ast->ast_azi_vit, \
    lp_Ast->ast_alt_vit ) ;

  MACRO_ASTRO_GLOBAL_LOG(lp_Ast->ast_loglevel,1,"%s", c_cmd) ;

  return ;
}

/*****************************************************************************************
* @fn     : ASTRE_STELLARIUM_VIEW
* @author : s.gravois
* @brief  : Cette fonction affiche les informations a la sauce stellarium
* @param  : 
* @date   : 2022-03-18 creation
* @date   : 2022-03-28 simplification
*****************************************************************************************/

void ASTRE_STELLARIUM_VIEW(STRUCT_ASTRE *lp_Ast) {

  TraceArbo(__func__,1,"display mode stellarium") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("Va / Vh    : %3.2f / %3.2f" , lp_Ast->ast_azi_vit    , lp_Ast->ast_alt_vit ) ;
  Trace("AD / Dec   : %s / %s"       , lp_Ast->ast_hhmmss_asc , lp_Ast->ast_ddmm_dec ) ;
  Trace("AH / Dec   : %s / %s"       , lp_Ast->ast_hhmmss_agh , lp_Ast->ast_ddmm_dec ) ;
  Trace("AZ./ Haut. : %s / %s"       , lp_Ast->ast_ddmm_azi   , lp_Ast->ast_ddmm_alt ) ;

  return ;
}

