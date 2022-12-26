/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
--------------------------------------------------------------
date        | commentaires 
--------------------------------------------------------------
07/10/2022  | * creation a partir du code de astro.c et autre
-------------------------------------------------------------- 
*/

#include "astro_suivi.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;
MACRO_ASTRO_GLOBAL_EXTERN_PID ;

/*****************************************************************************************
* @fn     : SUIVI_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_SUIVI *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void SUIVI_LOCK ( STRUCT_SUIVI * lp_Sui) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Sui->sui_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : SUIVI_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_SUIVI *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void SUIVI_UNLOCK ( STRUCT_SUIVI * lp_Sui) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Sui->sui_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : SUIVI_PAS_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_SUIVI_PAS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void SUIVI_PAS_LOCK ( STRUCT_SUIVI_PAS * lp_Pas) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pas->pas_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : SUIVI_PAS_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_SUIVI_PAS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void SUIVI_PAS_UNLOCK ( STRUCT_SUIVI_PAS * lp_Pas) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pas->pas_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : SUIVI_FRE_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_SUIVI_FREQUENCES *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void SUIVI_FRE_LOCK ( STRUCT_SUIVI_FREQUENCES * lp_Fre) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Fre->fre_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : SUIVI_FRE_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_SUIVI_FREQUENCES *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void SUIVI_FRE_UNLOCK ( STRUCT_SUIVI_FREQUENCES * lp_Fre) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Fre->fre_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : SUIVI_TEMPORISATION_AZIMUT
* @author : s.gravois
* @brief  : Cette fonction permet de faire une temporisation dans le thread SUIVI_AZIMUT de astro.c
* @param  : STRUCT_SUIVI * lp_Sui
* @param  : struct timeval * pt00
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
* @date   : 2022-10-11 deplacement dans astro_suivi.c
* @todo   : TODO : supprimer (obsolete) 
*****************************************************************************************/

void SUIVI_TEMPORISATION_AZIMUT(STRUCT_SUIVI * lp_Sui, struct timeval * pt00) {
  
  struct timeval t01 ;
  double t_diff ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  usleep ( (long)(lp_Sui->sui_dta * lp_Sui->sui_tempo_percent) ) ;
  
  t_diff=0;
  while( t_diff < lp_Sui->sui_dta ) {
     
   gettimeofday(&t01,NULL) ;
   t_diff = (( t01.tv_sec - pt00->tv_sec)  * TIME_MICRO_SEC ) + t01.tv_usec - pt00->tv_usec;
  }
  gettimeofday(pt00,NULL) ;
}

/*****************************************************************************************
* @fn     : SUIVI_TEMPORISATION_AZIMUT
* @author : s.gravois
* @brief  : Cette fonction permet de faire une temporisation dans le thread SUIVI_ALTITUDE de astro.c
* @param  : STRUCT_SUIVI * lp_Sui
* @param  : struct timeval * pt00
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
* @date   : 2022-10-11 deplacement dans astro_suivi.c
* @todo   : TODO : supprimer (obsolete) 
*****************************************************************************************/

void SUIVI_TEMPORISATION_ALTITUDE(STRUCT_SUIVI * lp_Sui, struct timeval * pt00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  usleep ( (long)(lp_Sui->sui_dth * lp_Sui->sui_tempo_percent) ) ;
  
  t_diff=0;
  while( t_diff < lp_Sui->sui_dth ) {
     
   gettimeofday(&t01,NULL) ;
   t_diff = (( t01.tv_sec - pt00->tv_sec)  * TIME_MICRO_SEC ) + t01.tv_usec - pt00->tv_usec;
  }
  gettimeofday(pt00,NULL) ;
}

/*****************************************************************************************
* @fn     : SUIVI_MAJ_PAS
* @author : s.gravois
* @brief  : Cette fonction met a jour les valeurs de lp_Sui->pas* en fonction
* @brief  : du contenu de gp_Dat->dat_act
* @param  : STRUCT_SUIVI * lp_Sui
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
*****************************************************************************************/

void SUIVI_MAJ_PAS( STRUCT_SUIVI_PAS * lp_Pas ) {

  int i = 0 ;

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /* TODO : supprimer : obsolete ----------
  if ( ! strcmp( gp_Dat->dat_act, "plus" ) )  {
    gp_Fre->fre_ta_mic *=  lp_Sui->sui_plus  ; gp_Fre->fre_fa_mic = 1 / gp_Fre->fre_ta_mic ;
    gp_Fre->fre_th_mic *=  lp_Sui->sui_plus  ; gp_Fre->fre_fh_mic = 1 / gp_Fre->fre_th_mic ;
  }
  if ( ! strcmp( gp_Dat->dat_act, "moins" ) ) {
    gp_Fre->fre_ta_mic *=  lp_Sui->sui_moins  ; gp_Fre->fre_fa_mic = 1 / gp_Fre->fre_ta_mic ;
    gp_Fre->fre_th_mic *=  lp_Sui->sui_moins  ; gp_Fre->fre_fh_mic = 1 / gp_Fre->fre_th_mic ;
  }
  */
  /* TODO : supprimer : obsolete ----------
    touche + et - utilise pour PLA et ETO sur le clavier 
  */
 /*
  if ( ! strcmp( gp_Dat->dat_act, "plus" ) )         { i++; gp_Pas->pas_acc_plus  = 1 ; }
  if ( ! strcmp( gp_Dat->dat_act, "moins" ) )        { i++; gp_Pas->pas_acc_moins = 1 ; }
*/
/*
  if ( ! strcmp( gp_Dat->dat_act, "forward" ) )      { i++; gp_Pas->pas_forward  = 1 ; }
  if ( ! strcmp( gp_Dat->dat_act, "rewind" ) )       { i++; gp_Pas->pas_rewind = 1 ; }
  if ( ! strcmp( gp_Dat->dat_act, "forwardfast" ) )  { i++; gp_Pas->pas_forward_fast  = 1 ; }
  if ( ! strcmp( gp_Dat->dat_act, "rewindfast" ) )   { i++; gp_Pas->pas_rewind_fast = 1 ; }
  if ( ! strcmp( gp_Dat->dat_act, "ne" ) )           { i++; gp_Pas->pas_nord=1 ; gp_Pas->pas_est=1   ; }
  if ( ! strcmp( gp_Dat->dat_act, "no" ) )           { i++; gp_Pas->pas_nord=1 ; gp_Pas->pas_ouest=1 ; }
  if ( ! strcmp( gp_Dat->dat_act, "se" ) )           { i++; gp_Pas->pas_sud=1  ; gp_Pas->pas_est=1   ; }
  if ( ! strcmp( gp_Dat->dat_act, "so" ) )           { i++; gp_Pas->pas_sud=1  ; gp_Pas->pas_ouest=1 ; }
  if ( ! strcmp( gp_Dat->dat_act, "n" ) )            { i++; gp_Pas->pas_nord  = 1 ; }
  if ( ! strcmp( gp_Dat->dat_act, "o" ) )            { i++; gp_Pas->pas_ouest = 1 ; }
  if ( ! strcmp( gp_Dat->dat_act, "e" ) )            { i++; gp_Pas->pas_est   = 1 ; }
  if ( ! strcmp( gp_Dat->dat_act, "s" ) )            { i++; gp_Pas->pas_sud   = 1 ; }
  if ( ! strcmp( gp_Dat->dat_act, "reset" ) )        { i++; gp_Pas->pas_rst   = 1 ; }
*/

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pas->pas_mutex ) ;

  KEYS_IF_MOT_IS("n" )             { i++ ; lp_Pas->pas_nord          = 1 ; }
  KEYS_IF_MOT_IS("o" )             { i++ ; lp_Pas->pas_ouest         = 1 ; }
  KEYS_IF_MOT_IS("e" )             { i++ ; lp_Pas->pas_est           = 1 ; }
  KEYS_IF_MOT_IS("s" )             { i++ ; lp_Pas->pas_sud           = 1 ; }
  KEYS_IF_MOT_IS("forward" )       { i++ ; lp_Pas->pas_forward       = 1 ; }
  KEYS_IF_MOT_IS("rewind" )        { i++ ; lp_Pas->pas_rewind        = 1 ; }
  KEYS_IF_MOT_IS("forwardfast" )   { i++ ; lp_Pas->pas_forward_fast  = 1 ; }
  KEYS_IF_MOT_IS("rewindfast" )    { i++ ; lp_Pas->pas_rewind_fast   = 1 ; }
  KEYS_IF_MOT_IS("ne" )            { i++ ; lp_Pas->pas_nord          = 1 ; lp_Pas->pas_est   = 1 ; }
  KEYS_IF_MOT_IS("no" )            { i++ ; lp_Pas->pas_nord          = 1 ; lp_Pas->pas_ouest = 1 ; }
  KEYS_IF_MOT_IS("se" )            { i++ ; lp_Pas->pas_sud           = 1 ; lp_Pas->pas_est   = 1 ; }
  KEYS_IF_MOT_IS("so" )            { i++ ; lp_Pas->pas_sud           = 1 ; lp_Pas->pas_ouest = 1 ; }
  KEYS_IF_MOT_IS("reset" )         { i++ ; lp_Pas->pas_rst             = 1 ; }

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pas->pas_mutex ) ;
  /* Si gp_Dat->dat_act a ete utilise, il peut etre remis a zero */

  if (i>0) { 
    KEYS_RESET_MOT(gp_Key) ; 
  }

  /* TODO : remplacer par SUIVI_PAS_DISPLAY */

  Trace2("%ld %ld %ld %ld %d %d\n", \
    lp_Pas->pas_ouest, \
    lp_Pas->pas_est, \
    lp_Pas->pas_nord, \
    lp_Pas->pas_sud, \
    lp_Pas->pas_acc_plus, \
    lp_Pas->pas_acc_moins );
  
  TraceArbo(__func__,2,"fin") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
}

/*****************************************************************************************
* @fn     : SUIVI_OLD_0
* @author : s.gravois
* @brief  : realise certaines actions particulieres apres appui sur une touche
* @param  : STRUCT_SUIVI   *lp_Sui
* @param  : STRUCT_KEYS *clavier 
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-30 amelioration et visibilite du contenu fct 
* @date   : 2022-05-30 ajout des mots concernant les touches F1->F12 du gp_Key
* @todo   : analyser pour simplification <=> avec suivi menu etc..
*****************************************************************************************/

void SUIVI_OLD_0( STRUCT_SUIVI * lp_Sui) {
    

}

/*****************************************************************************************
* @fn     : SUIVI_MENU_BEFORE_WHILE
* @author : s.gravois
* @brief  : fonction qui initialise des attributs 
* @param  : STRUCT_SUIVI * lp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-10-07 deplacement code depuis astro.c 
* @date   : 2022-11-03 renommage SUIVI_MENU_PREALABLE -> SUIVI_MENU_BEFORE_WHILE
* @date   : 2022-11-03 suppression de tous les champs suivi_xxx (inutile)
* @todo   : (obsolete) 
*****************************************************************************************/

void SUIVI_MENU_BEFORE_WHILE (STRUCT_SUIVI * lp_Sui) {

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  switch ( lp_Sui->sui_menu ) {
    case MENU_AZIMUTAL            : break ;
    case MENU_EQUATORIAL          : break ;
    case MENU_MANUEL_BRUT         : break ;
    case MENU_MANUEL_NON_ASSERVI  : break ;
    case MENU_MANUEL_ASSERVI      : break ;
    case MENU_GOTO                : break ;
    case MENU_INFO                : break ;
    case MENU_RESEAU_UP           : break ;
    case MENU_RESEAU_DOWN         : break ; 
    case MENU_PROGRAMME_DOWN      : break ; 
    case MENU_DOWN                : break ; 
  }
}

/*****************************************************************************************
* @fn     : SUIVI_TRAITEMENT_MOT
* @author : s.gravois
* @brief  : realise certaines actions particulieres apres appui sur une touche
* @param  : STRUCT_SUIVI   *lp_Sui
* @param  : STRUCT_KEYS *clavier 
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-30 amelioration et visibilite du contenu fct 
* @date   : 2022-05-30 ajout des mots concernant les touches F1->F12 du gp_Key
* @todo   : analyser pour simplification <=> avec suivi menu etc..
*****************************************************************************************/

void SUIVI_TRAITEMENT_MOT( STRUCT_SUIVI * lp_Sui ) {

  int i=0;
  char cmd[256]={0} ;
  char s_buffer4[ CONFIG_TAILLE_BUFFER_32 * 4 ]={0} ;
  char c_l0[16] ={0};
  char c_l1[16] ={0};

  TraceArbo(__func__,1,"start") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  memset( c_l0, CALCULS_ZERO_CHAR, sizeof( c_l0 )) ;
  memset( c_l1, CALCULS_ZERO_CHAR, sizeof( c_l1 )) ;

  memset( cmd,       CALCULS_ZERO_CHAR, sizeof( cmd )) ;
  memset( s_buffer4, CALCULS_ZERO_CHAR, sizeof( s_buffer4 )) ;
  
  //---------------------------------------------------------------------------------------------------------
  // TRAITEMENT DES ACTIONS SANS NECESSAIREMENT UNE VALIDATION
  // exemple : on appuie sur la touche ouest est sud nors : appel MENU_MANUEL_BRUT
  //---------------------------------------------------------------------------------------------------------
  
  // On change de MENU si on appuie sur EST OUEST NORD ou SUD => 
  // on passe en MENU 0 

  lp_Sui->sui_menu_old = lp_Sui->sui_menu ;

  /*  touche OUEST */

  /* Si on a un mot non vide */
  if ( strcmp( gp_Key->key_mot, "" ) != 0 ) {

    Trace("gp_Key->key_mot non vide : %s", gp_Key->key_mot) ;

    ASTRE_FORMATE_DONNEES_AFFICHAGE(gp_Ast);

    /* Renvoi sur MANUEL_BRUT */

    KEYS_IF_MOT_IS("n" )             { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("o" )             { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("e" )             { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("s" )             { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("forward" )       { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("rewind" )        { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("forwardfast" )   { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("rewindfast" )    { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("ne" )            { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("no" )            { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("se" )            { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("so" )            { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }
    KEYS_IF_MOT_IS("reset" )         { lp_Sui->sui_menu = MENU_MANUEL_BRUT ; }

    /* Fin Renvoi sur MANUEL_BRUT */

    KEYS_IF_MOT_IS("stop")  { gp_Vou->vou_run = 0 ; i=1;}
    KEYS_IF_MOT_IS("play")  { gp_Vou->vou_run = 1 ; i=1;}
    
    /* Quelques actions d 'affichage a l'ecran  */

    KEYS_IF_MOT_IS("aff_variables")       { CONFIG_PARAMETRES_DISPLAY() ; i=1 ; } ;

    /* Quelques actions d 'affichage a l'ecran LCD */

    KEYS_IF_MOT_IS("aff_tps_lie")         { gp_Lcd->display_tps_lie( 2000000 ) ;i=1; }  
    KEYS_IF_MOT_IS("aff_ast_vit")         { gp_Lcd->display_ast_vit( 2000000 ) ; i=1;}  
    KEYS_IF_MOT_IS("aff_ast_fre")         { gp_Lcd->display_ast_fre( 2000000 ) ; i=1;}
    KEYS_IF_MOT_IS("aff_ast_per")         { gp_Lcd->display_ast_per( 2000000 ) ;i=1; }
    KEYS_IF_MOT_IS("aff_azi_alt")         { gp_Lcd->display_azi_alt( 2000000 ) ; i=1;}
    KEYS_IF_MOT_IS("aff_agh_dec")         { gp_Lcd->display_agh_dec( 2000000 ) ;i=1; }
    KEYS_IF_MOT_IS("aff_asc_dec")         { gp_Lcd->display_asc_dec( 2000000 ) ; i=1;}
    KEYS_IF_MOT_IS("aff_mod_ste")         { gp_Lcd->display_mod_ste( 2000000 ) ;i=1; }
    KEYS_IF_MOT_IS("cfg_gpios_alt_azi")   { gp_Lcd->display_cfg_gpios_alt_azi( 2000000 ) ; i=1;}
    KEYS_IF_MOT_IS("cfg_gpios_mas_fre")   { gp_Lcd->display_cfg_gpios_mas_fre( 2000000 ) ; i=1;}
    KEYS_IF_MOT_IS("cfg_gpios_leds")      { gp_Lcd->display_cfg_gpios_leds( 2000000 ) ;i=1; }
    KEYS_IF_MOT_IS("cfg_reduction")       { gp_Lcd->display_cfg_reduction( 2000000 ) ; i=1;}

    /* Activation algorithme PID de regulation des periodes / frequences moteurs */

    KEYS_IF_MOT_IS("cfg_log_tps_reel_up") { 
      i=1 ; 
      if ( gi_pid_trace == 0 ){
        gi_pid_trace = gp_Tpo->tpo_pid_loop ; 
        sprintf(c_l0,"PID:activation") ;
        sprintf(c_l1,"echant. = %d pas", gi_pid_trace) ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace1("%s : %s", c_l0, c_l1) ;
      } 
      else {
        gi_pid_trace = 0 ; 
        sprintf(c_l0,"PID:") ;
        sprintf(c_l1,"desactivation") ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace1("%s : %s", c_l0, c_l1) ;        
      }
    }

    /* Activation traces PID pour le moteur ALT */

    KEYS_IF_MOT_IS("cfg_log_tps_reel_trace_alt") { 
      i=1 ; 
      if ( gi_pid_trace_alt == 0 ) {
        gi_pid_trace_alt=1;      
        sprintf(c_l0,"PID:log alt") ;
        sprintf(c_l1,"echant. = %d pas", gi_pid_trace) ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace1("%s : %s", c_l0, c_l1) ;
      }
      else {
        gi_pid_trace_alt=0;      
        sprintf(c_l0,"PID:log alt") ;
        sprintf(c_l1,"desactivation") ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace1("%s : %s", c_l0, c_l1) ;        
      }
    }

    /* Activation traces PID pour le moteur AZI */

    KEYS_IF_MOT_IS("cfg_log_tps_reel_trace_azi") { 
      i=1 ;
      if ( gi_pid_trace_azi == 0 ) {
        gi_pid_trace_azi=1 ; 
        sprintf(c_l0,"PID:log azi") ;
        sprintf(c_l1,"echant. = %d pas", gi_pid_trace) ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace1("%s : %s", c_l0, c_l1) ;
      }
      else {
        gi_pid_trace_azi=0 ; 
        sprintf(c_l0,"PID:log azi") ;
        sprintf(c_l1,"desactivation") ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace1("%s : %s", c_l0, c_l1) ;        
      }
    }

    KEYS_IF_MOT_IS("aff_acc_alt_azi")     { gp_Lcd->display_acc_alt_azi( 2000000 ) ;i=1; }

    KEYS_IF_MOT_IS("aff_infos") { 

      Trace1("Mot_Is aff_infos") ;
      
      CONFIG_DISPLAY_TOUT() ; 
      
      /*gp_Lcd->display_informations ( 2000000 ) ;*/
      lp_Sui->sui_menu = MENU_INFO ; 
      
      i=1;
      
    }     // mode info

    /*  touche mode equatorial */
    
    KEYS_IF_MOT_IS("key_equ")      { 
      gp_Lcd->display_str_str( 2000000, "Mode equatorial", (char*)gc_hach_suivi_menus[ MENU_EQUATORIAL ] ) ;
      lp_Sui->sui_menu = MENU_EQUATORIAL ; 
      i=1;
    }    

    /*  touche mode azimutal */

    KEYS_IF_MOT_IS("key_azi")      { 
      gp_Lcd->display_str_str( 2000000, "Mode azimutal", (char*)gc_hach_suivi_menus[ MENU_AZIMUTAL ] ) ;
      lp_Sui->sui_menu = MENU_AZIMUTAL ; 
      i=1;
    }       // mode azimutal

    /* touche POWER : arret su systeme */

    KEYS_IF_MOT_IS("key_power")     { 
      gp_Lcd->display_str_str( 2000000, "Mode azimutal", (char*)gc_hach_suivi_menus[ MENU_AZIMUTAL ] ) ;
      lp_Sui->sui_menu = MENU_DOWN ; 
      i=1;
    } 

    /* touche EXIT : arret su programme */

    KEYS_IF_MOT_IS("key_exit")      { 
      lp_Sui->sui_menu = MENU_PROGRAMME_DOWN ; 
      i=1;
    } 

    /* touche reseau : arret du reseau */
    /* TODO : non implemente : a definir et coder */

    KEYS_IF_MOT_IS("key_reseau_up") { 
      
      lp_Sui->sui_menu = MENU_RESEAU_UP ; 
      /* TODO : a terminer */
      i=1;
      
    }     
    /* remise a jour du mot a vide */ 

    KEYS_IF_MOT_IS("aff_thread_infos") {
      i=1;
      PTHREADS_INFOS(gp_Pth) ;
    }
    if (i>0) { 
      KEYS_RESET_MOT(gp_Key) ; 
    }
  }
  else {
    Trace1("gp_Key->key_mot vide") ;
  }
  //-----------------------------------------------------------------
  // Si un  mot est lu sur le clavier (appui sur menu->key_valider) necessaire)
  // on effectue une action
  // Si autre chose qu'un menu est lu sur le clavier , on peut aussi changer
  // de menu (pour plus de rapidite)
  //-----------------------------------------------------------------

  if ( gp_Key->key_phrase_lue ) {
    
    //---------------------------------
    // Si un MENU est entrer au clavier
    // => gestion des MENUS en fonction du clavier
    // ou de toute autre entree exterieure
    // autre qu'un switch electronique
    //---------------------------------
    
    Trace("symbole = %s nombre = %s\n", gp_Key->key_symbole, gp_Key->key_nombre ) ;
    
    //--------------------------------------------------------------------
    // ON CHANGE DE MENU VOLONTAIREMENT
    //--------------------------------------------------------------------
    
    if ( ! strcmp( gp_Key->key_symbole, gp_Key->key_menu )) {
      lp_Sui->sui_menu_old = lp_Sui->sui_menu ;
      lp_Sui->sui_menu     = atoi( gp_Key->key_nombre ) ; 
    }
    
    //---------------------------------------------------------------------------------------------------------
    // Si un nouvel as a ete pris en compte
    //---------------------------------------------------------------------------------------------------------
    
    sprintf( s_buffer4, "%s%s%s%s", CONFIG_MES, CONFIG_NGC, CONFIG_ETO, CONFIG_PLA) ;

    if ( strstr( s_buffer4, gp_Key->key_symbole ) != NULL ) {

      memset( gp_Ast->ast_nom, CALCULS_ZERO_CHAR, sizeof(gp_Ast->ast_nom)) ;
      sprintf( gp_Ast->ast_nom, "%s%s", gp_Key->key_symbole, gp_Key->key_nombre) ;
    
      Trace("== %s ==",gp_Ast->ast_nom) ;
      
      CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;

      /* Recherche de l'as dans les catalgues */
      
      if ( strstr( gp_Ast->ast_nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Ngc, gp_Ast) ; ;
      if ( strstr( gp_Ast->ast_nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Ngc, gp_Ast) ; ;
      if ( strstr( gp_Ast->ast_nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Eto, gp_Ast) ;
      
      /* Pour les planetes, le calcul est fait dans SOLAR_SYSTEM dans CALCULS_TOUT */

      gp_Ast->ast_new = TRUE ;
      //lp_Sui->sui_menu = MENU_AZIMUTAL ;

      GPIO_LED_ETAT_CLIGNOTE(1, 10) ; 
    }
    //---------------------------------------------------------------------------------------------------------
    // Si une DEMANDE de MISE A JOUR relative au temps (sauf mois et jour)
    //---------------------------------------------------------------------------------------------------------
    
    if(  ! strcmp( gp_Key->key_symbole, "TIME" ) )  {

      GPIO_LED_ETAT_CLIGNOTE(strlen( gp_Key->key_nombre ), 10) ;

      switch ( strlen( gp_Key->key_nombre ) ) {

        case 2 : // Si longueur = 2 cela ne peut etre qu'un changement de mois
          
          Trace1("demande changement heure minutes : %s" , gp_Key->key_nombre) ;

          /* CONFIG_SET_MONTH( gp_Key->key_nombre ) ; */

        case 4 : // Si longueur = 4 cela ne peut etre qu'un changement d'heure et de minutes
          
          Trace1("demande changement heure minutes : %s" , gp_Key->key_nombre) ;
    
          TIME_SET_HOUR_AND_MINUTES( gp_Key->key_nombre ) ;
          
        break ;
          
          // FIXME : 20190822 : modification configuration de la date

        case 8 : // Si longueur = 5 cela est un changement de mois jour avec verif de l'annee

          Trace1("demande changement annee : %s" , gp_Key->key_nombre) ;
          
          TIME_SET_YEAR_MONTH_AND_DAY( gp_Key->key_nombre ) ;
      
          break ;
      }
    }
    //---------------------------------------------------------------------------------------------------------
    // Si une DEMANDE de SETUP a lieu 
    // RAPPEL : l'annee par defaut est declaree en DUR dans le CODE de telle sorte que le code ne soit valable
    // que pour annee en COURS
    //---------------------------------------------------------------------------------------------------------
    
    if(  ! strcmp( gp_Key->key_symbole, "SETUP" ) )  {
      switch ( strlen( gp_Key->key_nombre ) ) {
        
        case 0 : break ; // a completer
        case 1 : break ; // a completer 

        case 2 : // TODO : exemple d'une demande de capteur : a modifier / completer
          TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;

          if ( gp_Dev->dev_use_capteurs ) { 
            /* TODO : FIXME :  use global var for pitch & heading as member of STR_I2C_xx */
            /*
            gp_Ast->ast_azi = lp_Sui->_pitch ;
            gp_Ast->ast_alt = lp_Sui->_heading ; 
            */
          }
          CALCULS_EQUATEUR () ;

          lp_Sui->sui_menu_old = lp_Sui->sui_menu ;
          lp_Sui->sui_menu = MENU_AZIMUTAL ;
          break ;
      }
      //---------------------------------------------------------------------------------------------------------
    }
    gp_Key->key_phrase_lue=0 ;

    strcpy( gp_Key->key_mot, "" ) ;
    strcpy( gp_Key->key_nombre, "" ) ;
  }
}
/*****************************************************************************************
* @fn     : SUIVI_MANUEL_BRUT
* @author : s.gravois
* @brief  : Ce mode permet le centrage / recentrage de l'objet tout en ayant le suivi.
* @param  : STRUCT_SUIVI   *lp_Sui
* @param  : STRUCT_KEYS *gp_Key 
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-31 correction code dans flag_nord_sud_est_ouest : les accelrations sont les pas
* @todo   : 
*****************************************************************************************/
/* FIXME :
  SUIVI_MANUEL_BRUT : 
    le suivi etant effectue sur un axe (calcul des vitesses et periodes par suivi_voute)
    l'appui sur les touches N-S-E-O provoque une suspension de suivi_voute jusqua touche OK
    avec une multiplication des vitesses N-S-E-O par un facteur gp_Cal_Par->cal_par_alt_acc
    Ce mode permet le centrage / recentrage de l'objet tout en ayant le suivi.
*/

void SUIVI_MANUEL_BRUT(STRUCT_SUIVI * lp_Sui) {
  
  int  flag_nord_sud_est_ouest = 0 ;
  int  flag_calcul = 0 ;
  char c_action[ CONFIG_TAILLE_BUFFER_32] = {0} ;

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  /* GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, lp_Sui) ; */

  SUIVI_MAJ_PAS(gp_Pas) ;

  if( strcmp( gp_Dat->dat_act, "") != 0 ) {
    Trace1("0 gp_Dat->dat_act = %s", gp_Dat->dat_act ) ;
  }
  else {
    Trace1("dat_inf vide" ) ;
  }
  // -----------------------------------------------------------
  // reset = remise a zero des compteurs
  //------------------------------------------------------------
  
  if ( gp_Pas->pas_rst ) {

    strcpy(c_action,"reset") ;

    Trace1("prise en compte : gp_Pas->pas_rst" ) ;

    // pthread_mutex_lock(& gp_Mut->mut_glo_azi );
    // pthread_mutex_lock(& gp_Mut->mut_glo_azi );

    if ( gp_Fre->fre_sh != 0 ) gp_Pas->pas_alt = gp_Fre->fre_sh ; else gp_Pas->pas_alt = 1 ;
    if ( gp_Fre->fre_sa != 0 ) gp_Pas->pas_azi = gp_Fre->fre_sa ; else gp_Pas->pas_azi = 1 ;

    gp_Pas->pas_acc_azi = 1 ;

    if ( lp_Sui->sui_mode_equatorial )  gp_Pas->pas_acc_alt = 0.0 ;
    else                             gp_Pas->pas_acc_alt = 1.0 ;

    gp_Pas->pas_rst = 0 ;
           
    // pthread_mutex_unlock(& gp_Mut->mut_glo_azi );
    // pthread_mutex_unlock(& gp_Mut->mut_glo_alt );
           
    flag_calcul = 1 ;
  }
  // -----------------------------------------------------------
  // actions a faire suivant appui sur NORD / SUD / OUEST /EST
  //------------------------------------------------------------

  if ( gp_Pas->pas_nord || gp_Pas->pas_sud || gp_Pas->pas_est || gp_Pas->pas_ouest ) {
    flag_nord_sud_est_ouest = 1 ;
  }

  // flag_nord_sud_est_ouest = 1 = on a appuye sur N-S-E-O

  if (flag_nord_sud_est_ouest ) { 
  
    strcpy(c_action,"fleches") ;

    // pthread_mutex_lock(& gp_Mut->mut_glo_azi );
    // pthread_mutex_lock(& gp_Mut->mut_glo_alt );

    // On utilise les memes touches que dans SUIVI_MANUEL_ASSERVI

    if ( gp_Pas->pas_nord )  gp_Pas->pas_alt++ ; if ( gp_Pas->pas_alt == 0 ) gp_Pas->pas_alt = 1 ;
    if ( gp_Pas->pas_sud )   gp_Pas->pas_alt-- ; if ( gp_Pas->pas_alt == 0 ) gp_Pas->pas_alt = -1 ;
    if ( gp_Pas->pas_ouest ) gp_Pas->pas_azi++;  if ( gp_Pas->pas_azi == 0 ) gp_Pas->pas_azi = 1 ;
    if ( gp_Pas->pas_est )   gp_Pas->pas_azi-- ; if ( gp_Pas->pas_azi == 0 ) gp_Pas->pas_azi = -1 ;

    if ( gp_Pas->pas_nord  || gp_Pas->pas_sud ) {
      /*
      if ( abs(gp_Pas->pas_alt) <= 2) gp_Pas->pas_acc_alt = gp_Pas->pas_alt  ;
      else                           gp_Pas->pas_acc_alt = gp_Pas->pas_alt * gp_Cal_Par->cal_par_alt_acc ;
      */ /* correction mai 2022 */

      gp_Pas->pas_acc_alt = gp_Pas->pas_alt  ;

    }
    if ( gp_Pas->pas_ouest || gp_Pas->pas_est ) {
      /*
      if ( abs(gp_Pas->pas_azi) <= 2) gp_Pas->pas_acc_azi = gp_Pas->pas_azi ;
      else                           gp_Pas->pas_acc_azi = gp_Pas->pas_azi * gp_Cal_Par->cal_par_azi_acc ;
      */ /* correction mai 2022 */

      gp_Pas->pas_acc_azi = gp_Pas->pas_azi ;
    }
    gp_Pas->pas_est     = 0 ;
    gp_Pas->pas_ouest   = 0 ; 
    gp_Pas->pas_nord    = 0 ;
    gp_Pas->pas_sud     = 0 ;
    gp_Pas->pas_rst     = 0 ;

    // pthread_mutex_unlock(& gp_Mut->mut_glo_azi );
    // pthread_mutex_unlock(& gp_Mut->mut_glo_alt );

    flag_calcul = 1 ;
  }
  // ======================================================
  // Gestion des accelerations ou decelerations tres lentes
  // (reglage du suivi precis)
  // ======================================================

  if ( gp_Pas->pas_forward ) {

    strcpy(c_action,"forward") ;

    // pthread_mutex_lock(& gp_Mut->mut_glo_azi );
    // pthread_mutex_lock(& gp_Mut->mut_glo_alt );

    gp_Pas->pas_acc_azi *= gp_Cal_Par->cal_par_altaz_slow_forward ;
    gp_Pas->pas_acc_alt *= gp_Cal_Par->cal_par_altaz_slow_forward ; 

    // pthread_mutex_unlock(& gp_Mut->mut_glo_azi );
    // pthread_mutex_unlock(& gp_Mut->mut_glo_alt );
/*
    Trace1("acc*gp_Cal_Par->cal_par_altaz_slow_forward %.4f pas_acc_azi %.4f pas_acc_alt %.4f", gp_Cal_Par->cal_par_altaz_slow_forward, gp_Pas->pas_acc_azi, gp_Pas->pas_acc_alt ) ;
*/
    Trace("%-15s : fre_sh %d fre_sa %d pas_azi = %ld pas_alt = %ld pas_acc_azi = %f pas_acc_alt = %f", "forward.." , \
           gp_Fre->fre_sh , gp_Fre->fre_sa, gp_Pas->pas_azi, gp_Pas->pas_alt, gp_Pas->pas_acc_azi , gp_Pas->pas_acc_alt) ;

    gp_Pas->pas_forward = 0 ;
    flag_calcul = 1 ;
  }
  
  if ( gp_Pas->pas_rewind ) {

    strcpy(c_action,"rewind") ;

    // pthread_mutex_lock(& gp_Mut->mut_glo_azi );
    // pthread_mutex_lock(& gp_Mut->mut_glo_alt );

    gp_Pas->pas_acc_azi /= gp_Cal_Par->cal_par_altaz_slow_rewind ;
    gp_Pas->pas_acc_alt /= gp_Cal_Par->cal_par_altaz_slow_rewind ; 

    // pthread_mutex_unlock(& gp_Mut->mut_glo_azi );
    // pthread_mutex_unlock(& gp_Mut->mut_glo_alt );

    gp_Pas->pas_rewind  = 0 ;
    flag_calcul = 1 ;
  }
  // ======================================================
  // Gestion des accelerations ou decelerations tres rapides
  // (reglage du suivi precis)
  // ======================================================

  if ( gp_Pas->pas_forward_fast ) {

    strcpy(c_action,"forward fast") ;

    // pthread_mutex_lock(& gp_Mut->mut_glo_azi );
    // pthread_mutex_lock(& gp_Mut->mut_glo_alt );

    gp_Pas->pas_acc_azi *= gp_Cal_Par->cal_par_altaz_fast_forward ;          
    gp_Pas->pas_acc_alt *= gp_Cal_Par->cal_par_altaz_fast_forward ; 

    // pthread_mutex_unlock(& gp_Mut->mut_glo_azi );
    // pthread_mutex_unlock(& gp_Mut->mut_glo_alt );

    gp_Pas->pas_forward_fast = 0 ;
    flag_calcul = 1 ;
  }

  if ( gp_Pas->pas_rewind_fast ) {

    strcpy(c_action,"rewind fast") ;
    
    // pthread_mutex_lock(& gp_Mut->mut_glo_azi );   
    // pthread_mutex_lock(& gp_Mut->mut_glo_alt );

    gp_Pas->pas_acc_azi /= gp_Cal_Par->cal_par_altaz_fast_rewind ;
    gp_Pas->pas_acc_alt /= gp_Cal_Par->cal_par_altaz_fast_rewind ; 

    // pthread_mutex_unlock(& gp_Mut->mut_glo_azi );
    // pthread_mutex_unlock(& gp_Mut->mut_glo_alt );
            
    gp_Pas->pas_rewind_fast  = 0 ;
    flag_calcul = 1 ;
  }
  // ======================================================
  // Rafraichissement des vitesses et periodes
  // ======================================================

  if ( flag_calcul ) {
  
    Trace("fre_sh %d fre_sa %d pas_azi = %ld pas_alt = %ld pas_acc_azi = %f pas_acc_alt = %f (%-15s)",
        gp_Fre->fre_sh, \
        gp_Fre->fre_sa, \
        gp_Pas->pas_azi, \
        gp_Pas->pas_alt, \
        gp_Pas->pas_acc_azi, \
        gp_Pas->pas_acc_alt, \
        c_action) ;

    /* pthread_mutex_lock( & gp_Mut->mut_cal ); */
    
    CALCULS_VITESSES() ;
    CALCULS_PERIODE() ;
    
    /* pthread_mutex_unlock( & gp_Mut->mut_cal ); */
  }
}

//==========================================================
// MODE MANUEL : le suivi d'effectue directement en appuyant sur les touches 
//==========================================================

void SUIVI_MANUEL_ASSERVI(STRUCT_SUIVI * lp_Sui) {

  double  t_appui_raq_azi ;
  double  t_appui_raq_alt ;
  double  tempo_raq_alt ;
  double  tempo_raq_azi ;

  struct timeval t00 ;
  struct timeval t01 ;
  int azi,alt ;
  long    t_diff =0;
  double  t_diff_sec=0 ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  gp_Pas->pas_azi_old = gp_Pas->pas_azi ;
  gp_Pas->pas_alt_old = gp_Pas->pas_alt ;
  
  gettimeofday(&t00,NULL) ;
  
  azi = 0 ;
  alt = 0 ;
  
  /* GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, lp_Sui) ; */
  SUIVI_MAJ_PAS(gp_Pas) ;
  
  // La determination de tempo_raq est tres importante
  // Elle varie suivant la reduction total du moteur
  /* TODO : a expliquer */
  tempo_raq_alt = (double)gp_Tim_Par->tim_par_tpo_raq * 1000000 / gp_Cal_Par->cal_par_alt_red_tot ;
  tempo_raq_azi = (double)gp_Tim_Par->tim_par_tpo_raq * 1000000 / gp_Cal_Par->cal_par_azi_red_tot ;
  
  // -----------------------------------------------------------
  // reset = remise a zero des compteurs
  //------------------------------------------------------------
  
  if ( gp_Pas->pas_rst ) {

    gp_Pas->pas_alt = 0 ;
    gp_Pas->pas_azi = 0 ;
    gp_Pas->pas_rst = 0 ;

    t_appui_raq_azi = 0 ;
    t_appui_raq_alt = 0 ; 

    gp_Fre->fre_ta_mic = SUIVI_MAIN_TA_RESET ;
    gp_Fre->fre_th_mic = SUIVI_MAIN_TH_RESET ;
    gp_Fre->fre_fa_mic = SUIVI_MAIN_FA_RESET ;
    gp_Fre->fre_fh_mic = SUIVI_MAIN_FH_RESET ;
    
  }
  // -----------------------------------------------------------
  // TANT QUE J'APPUIE SUR UN BOUTON DE LA RAQUETTE ===>
  //------------------------------------------------------------
  
  while( gp_Pas->pas_ouest || gp_Pas->pas_est || gp_Pas->pas_sud || gp_Pas->pas_nord || gp_Pas->pas_acc_plus || gp_Pas->pas_acc_moins )
  { 
    // On remet une periode inferieure a la seconde pour eviter aux deux autres threads d'attendre
    
    if ( gp_Fre->fre_ta_mic == SUIVI_MAIN_TA_RESET && gp_Fre->fre_th_mic == SUIVI_MAIN_TH_RESET ) {
      gp_Fre->fre_ta_mic = SUIVI_MAIN_TA_TRANSITOIRE ;
      gp_Fre->fre_th_mic = SUIVI_MAIN_TH_TRANSITOIRE ;
      gp_Fre->fre_fa_mic = SUIVI_MAIN_FA_TRANSITOIRE ;
      gp_Fre->fre_fh_mic = SUIVI_MAIN_FH_TRANSITOIRE ;
    }
    
    // ----------------------------------------------------------------------------
    // On ne fait qu'une fois dans la boucle les actions suivantes (d'ou ce test..)
    // ----------------------------------------------------------------------------
    
    if ( ! azi )  if ( gp_Pas->pas_ouest || gp_Pas->pas_est || gp_Pas->pas_acc_moins || gp_Pas->pas_acc_plus ) { 
      pthread_mutex_lock(& gp_Mut->mut_glo_azi );
      azi=1 ;
    }
    if ( ! alt )  if ( gp_Pas->pas_nord  || gp_Pas->pas_sud || gp_Pas->pas_acc_moins || gp_Pas->pas_acc_plus ) { 
      pthread_mutex_lock(& gp_Mut->mut_glo_alt ); 
      alt=1 ;
    }
    
    if ( azi ) {
     
     if ( gp_Pas->pas_ouest || gp_Pas->pas_est ) 
       gp_Pas->pas_azi += gp_Pas->pas_ouest - gp_Pas->pas_est  ; 
     
     if ( gp_Pas->pas_acc_plus)  gp_Pas->pas_azi = (long) gp_Pas->pas_azi * 1.5 ; 
     if ( gp_Pas->pas_acc_moins) gp_Pas->pas_azi = (long) gp_Pas->pas_azi / 1.5 ; 
    }
    
    if ( alt ) {
     
     if ( gp_Pas->pas_nord || gp_Pas->pas_sud)
       gp_Pas->pas_alt += gp_Pas->pas_nord  - gp_Pas->pas_sud ;
     
     if ( gp_Pas->pas_acc_plus)  gp_Pas->pas_alt = (long) gp_Pas->pas_alt * 1.5 ; 
     if ( gp_Pas->pas_acc_moins) gp_Pas->pas_alt = (long) gp_Pas->pas_alt / 1.5 ; 
    }
    
    if ( azi ) usleep( (long)( tempo_raq_azi ) ) ;
    if ( alt ) usleep( (long)( tempo_raq_alt ) ) ;

    //-------------------------------------------------------------
    // ACCELERATION ou DECELERATION lors de l'APPUI sur la RAQUETTE
    // a modifier pour optimiser
    //-------------------------------------------------------------
    
    gp_Pas->pas_acc_plus  = 0 ;
    gp_Pas->pas_acc_moins = 0 ;
    gp_Pas->pas_est   = 0 ;
    gp_Pas->pas_ouest = 0 ; 
    gp_Pas->pas_nord  = 0 ;
    gp_Pas->pas_sud   = 0 ;
    gp_Pas->pas_rst = 0 ;
    
    // on relit sur les claviers en mode manuel 

    /* GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, lp_Sui) ; */
    SUIVI_MAJ_PAS(gp_Pas) ;
  }
  // =================================================================
  // FIN DE LA BOUCLE : TANT QUE J'APPUIE SUR UN BOUTON DE LA RAQUETTE
  // =================================================================

  if ( azi ) { pthread_mutex_unlock( & gp_Mut->mut_glo_azi ); }
  if ( alt ) { pthread_mutex_unlock( & gp_Mut->mut_glo_alt ); }
  
  gettimeofday(&t01,NULL) ;
  /* t_diff a definir en local */
  t_diff     = (( t01.tv_sec - t00.tv_sec ) * TIME_MICRO_SEC) + t01.tv_usec - t00.tv_usec ;
  t_diff_sec = (double)t_diff / (double)TIME_MICRO_SEC ;
	 
  if ( gp_Pas->pas_azi_old != gp_Pas->pas_azi ) t_appui_raq_azi += t_diff_sec ;
  if ( gp_Pas->pas_alt_old != gp_Pas->pas_alt ) t_appui_raq_alt += t_diff_sec ;
	 
  if ( gp_Pas->pas_azi_old != gp_Pas->pas_azi || gp_Pas->pas_alt_old != gp_Pas->pas_alt ) {

   Trace1(" : Va = %2.4f Vh = %2.4f fre_ta_mic = %2.4f fre_th_mic = %2.4f fre_fa_mic = %2.4f fre_fh_mic = %2.4f Fam = %ld Fhm = %ld",\
    gp_Ast->ast_azi_vit, \
    gp_Ast->ast_alt_vit, \
    gp_Fre->fre_ta_mic, \
    gp_Fre->fre_th_mic, \
    gp_Fre->fre_fa_mic, \
    gp_Fre->fre_fh_mic, \
    (gp_Sta->sta_Ia - gp_Sta->sta_Ia_prec), \
    (gp_Sta->sta_Ih - gp_Sta->sta_Ih_prec )) ;  
  }

}

/*****************************************************************************************
* @fn     : SUIVI_INIT
* @author : s.gravois
* @brief  : Cette fonction configure la structure STRUCT_SUIVI * lp_Sui
* @param  : STRUCT_SUIVI * lp_Sui
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-20 passage d une partie de code dans DEVICES_INIT
* @date   : 2022-05-24 ajout mutex_thread suivi pour proteger section crotique threads
* @date   : 2022-06-17 ajout gp_Pid_Par->par_pid_ech gp_Pid_Par->par_pid_kp gp_Pid_Par->par_pid_ki gp_Pid_Par->par_pid_kd 
* @todo   : voir si les mutex peuvent etre dispatches / eclates dans les autres struct
*****************************************************************************************/

void SUIVI_INIT(STRUCT_SUIVI * lp_Sui) {
  
  int i ;
  
  TraceArbo(__func__,0,"init suivi") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  lp_Sui->sui_mode_equatorial = 0 ;

  // a modifier  : instancier ces variables a l aide du fichier de config

  gp_Pas->pas_rst           = 1 ;
  gp_Pas->pas_azi           = 1 ;
  gp_Pas->pas_alt           = 1 ;

  gp_Pas->pas_acc_plus      = 0 ;
  gp_Pas->pas_acc_moins     = 0 ;
  gp_Pas->pas_azi_old       = 0 ;
  gp_Pas->pas_alt_old       = 0 ;
  gp_Pas->pas_est           = 0 ;
  gp_Pas->pas_ouest         = 0 ; 
  gp_Pas->pas_nord          = 0 ;
  gp_Pas->pas_sud           = 0 ;
  gp_Pas->pas_forward       = 0 ;
  gp_Pas->pas_rewind        = 0 ;
  gp_Pas->pas_forward_fast  = 0 ;
  gp_Pas->pas_rewind_fast   = 0 ;
  gp_Pas->pas_acc_azi       = 1.0 ; // acceleration volontaire des vitesses brutes
  gp_Pas->pas_acc_alt       = 1.0 ; // acceleration volontaire des vitesses brutes
  gp_Pas->pas_asc           = 0 ;
  gp_Pas->pas_dec           = 0  ;

  gp_Fre->fre_fa_mic        = 30 ;
  gp_Fre->fre_fh_mic        = 30 ;
  gp_Fre->fre_sa            = 0 ; // signe de la vitesse (direction), tenir compte du flag FLAG_SENS_ALT
  gp_Fre->fre_sh            = 0 ; // signe de la vitesse (direction), tenir compte du flag FLAG_SENS_AZI
  gp_Fre->fre_sa_old        = 0 ; // flag de comparaison pour raffraichir ou non les ecritures
  gp_Fre->fre_sh_old        = 0 ; // flag de comparaison pour raffraichir ou non les ecritures
  gp_Fre->fre_ta_mic        = 1 / gp_Fre->fre_fa_mic ;
  gp_Fre->fre_th_mic        = 1 / gp_Fre->fre_fh_mic ;

  gp_Sta->sta_Tac        = 1.0 ;
  gp_Sta->sta_Thc        = 1.0 ;
  
  gp_Sta->sta_Tacc = 0.97 ;
  gp_Sta->sta_Thcc = 0.97 ;
  
  gp_Sta->sta_Ia = 0 ;
  gp_Sta->sta_Ih = 0 ;
  
  gp_Sta->sta_Ias = 0 ;
  gp_Sta->sta_Ihs = 0 ;
  
  gp_Sta->sta_Ia_prec = 0 ;
  gp_Sta->sta_Ih_prec = 0 ;
  
  for(i=0;i<STATS_ASS;i++) gp_Sta->sta_Iat[i] = 0 ;
  for(i=0;i<STATS_ASS;i++) gp_Sta->sta_Iht[i] = 0 ;
           
  lp_Sui->sui_Da            = 0  ;      // nombre a injecter dans les diviseurs de frequence
  lp_Sui->sui_Dh            = 0 ;       // nombre a injecter dans les diviseurs de frequence
  lp_Sui->sui_alarme            = 0 ;
  lp_Sui->sui_tempo_percent = 0.99 ; 
  lp_Sui->sui_dth               = gp_Fre->fre_th_mic * TIME_MICRO_SEC ;
  lp_Sui->sui_dta               = gp_Fre->fre_ta_mic * TIME_MICRO_SEC ;
  lp_Sui->sui_menu          = gp_Con_Par->con_par_default_menu  ;    // menu par defaut
  lp_Sui->sui_menu_old      = gp_Con_Par->con_par_default_menu  ;

  gettimeofday(&lp_Sui->sui_tval,NULL) ;

  return ;
}