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
/*
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;
*/


/*****************************************************************************************
* @fn     : ASTRE_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_ASTRE * gp_Ast
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-20 ajout membres de la structure dans la fonction
* @date   : 2022-10-07 deplacement depuis config.c dans ce fichier
* @todo   : supprimer STRUCT_ASTRE *gp_Ast (var blog) et remplacer par void
*           passer par des pointeurs de fonctions dans un structure
*****************************************************************************************/

void ASTRE_INIT(STRUCT_ASTRE *gp_Ast ) {

  int C ;
  
  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & gp_Ast->ast_mutex ) ;

  pthread_mutex_lock( & gp_Ast->ast_mutex ) ;

  for(C=0; C< ASTRE_NB_COLONNES;C++) {
    memset( gp_Ast->plus_proche[C], CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);
    strcpy( gp_Ast->plus_proche[C], "") ;
  }
  memset( gp_Ast->nom,   CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  memset( gp_Ast->infos, CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  memset( gp_Ast->plus_proche, CALCULS_ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  
  gp_Ast->a   = 0  ;
  gp_Ast->h   = 0  ;
  gp_Ast->a0  = 0 ;
  gp_Ast->h0  = 0 ;
  gp_Ast->AGH   = 0  ; 
  gp_Ast->ASC   = 0  ;
  gp_Ast->DEC   = 0   ;
  gp_Ast->A0  = 0 ;
  gp_Ast->H0  = 0 ;
  gp_Ast->da  = 0 ;
  gp_Ast->dh  = 0 ;
  gp_Ast->dA  = 0 ;
  gp_Ast->dH  = 0 ;
  gp_Ast->Va  = 0 ;
  gp_Ast->Vh  = 0 ;
  gp_Ast->dVa = 0 ;
  gp_Ast->dVh = 0 ;
  gp_Ast->dVam= 0 ;
  gp_Ast->dVhm= 0 ;

  gp_Ast->x = 0 ;
  gp_Ast->xx = 0 ;
  gp_Ast->y  = 0 ;
  gp_Ast->yy =0;
  gp_Ast->z =0;
  gp_Ast->zz =0;
  
  gp_Ast->ast_typ = ASTRE_INDETERMINE ;
  gp_Ast->ast_num  = 0 ;
  gp_Ast->ast_new = TRUE ;

  pthread_mutex_unlock( & gp_Ast->ast_mutex ) ;
}


/*****************************************************************************************
* @fn     : ASTRE_FORMATE_DONNEES_AFFICHAGE
* @author : s.gravois
* @brief  : Cette fonction formate divers string en vue d un affichage pertinent
* @param  : STRUCT_ASTRE *gp_Ast
* @date   : 2022-04-12 creation
* @date   : 2022-04-21 remplacement ° par o (affichage LCD ° impossible)
* @date   : 2022-04-21 ajout de 2 resolutions plus simple (affichage contraint par LCD) :
* @date   : 2022-04-21 - c_hhmm_*
* @date   : 2022-04-21 - c_dd_*  
*****************************************************************************************/

void ASTRE_FORMATE_DONNEES_AFFICHAGE(STRUCT_ASTRE *gp_Ast ) {

  char  c_hhmmss_agh[ 16] ;
  char  c_hhmmss_asc[ 16] ;
  char  c_hhmmss_azi[ 16] ;
  char  c_hhmmss_alt[ 16] ;
  char  c_hhmmss_dec[ 16] ;

  char  c_hhmm_agh[ 16] ;
  char  c_hhmm_asc[ 16] ;
  char  c_hhmm_azi[ 16] ;
  char  c_hhmm_alt[ 16] ;
  char  c_hhmm_dec[ 16] ;

  char  c_ddmm_agh[ 16] ;
  char  c_ddmm_asc[ 16] ;
  char  c_ddmm_azi[ 16] ;
  char  c_ddmm_alt[ 16] ;
  char  c_ddmm_dec[ 16] ;

  char  c_dd_agh[ 16] ;
  char  c_dd_asc[ 16] ;
  char  c_dd_azi[ 16] ;
  char  c_dd_alt[ 16] ;
  char  c_dd_dec[ 16] ;

  TraceArbo(__func__,1,"") ;

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

  sprintf( c_hhmmss_agh, " %3dh%2dm%2ds",  gp_Ast->AGHt.tim_HH, gp_Ast->AGHt.tim_MM, gp_Ast->AGHt.tim_SS  ) ;
  sprintf( c_hhmmss_asc, " %3dh%2dm%2ds",  gp_Ast->ASCt.tim_HH, gp_Ast->ASCt.tim_MM, gp_Ast->ASCt.tim_SS  ) ;
  sprintf( c_hhmmss_azi, " %3dh%2dm%2ds",  gp_Ast->AZIt.tim_HH, gp_Ast->AZIt.tim_MM, gp_Ast->AZIt.tim_SS  ) ;
  sprintf( c_hhmmss_alt, " %3dh%2dm%2ds",  gp_Ast->ALTt.tim_HH, gp_Ast->ALTt.tim_MM, gp_Ast->ALTt.tim_SS  ) ;
  sprintf( c_hhmmss_dec, " %3dh%2dm%2ds",  gp_Ast->DECt.tim_HH, gp_Ast->DECt.tim_MM, gp_Ast->DECt.tim_SS  ) ;

  sprintf( c_hhmm_agh,   " %3d h %2d m",   gp_Ast->AGHt.tim_HH, gp_Ast->AGHt.tim_MM ) ;
  sprintf( c_hhmm_asc,   " %3d h %2d m",   gp_Ast->ASCt.tim_HH, gp_Ast->ASCt.tim_MM ) ;
  sprintf( c_hhmm_azi,   " %3d h %2d m",   gp_Ast->AZIt.tim_HH, gp_Ast->AZIt.tim_MM ) ;
  sprintf( c_hhmm_alt,   " %3d h %2d m",   gp_Ast->ALTt.tim_HH, gp_Ast->ALTt.tim_MM ) ;
  sprintf( c_hhmm_dec,   " %3d h %2d m",   gp_Ast->DECt.tim_HH, gp_Ast->DECt.tim_MM ) ;

  /* traitement des donnees en degres / minutes / secondes */
  /* est inclus dans l affichage le signe */

  sprintf( c_ddmm_agh, "%c %-3d d %d'", gp_Ast->AGHa.ang_sig, gp_Ast->AGHa.ang_DD, gp_Ast->AGHa.ang_MM ) ;
  sprintf( c_ddmm_asc, "%c %-3d d %d'", gp_Ast->ASCa.ang_sig, gp_Ast->ASCa.ang_DD, gp_Ast->ASCa.ang_MM ) ;
  sprintf( c_ddmm_azi, "%c %-3d d %d'", gp_Ast->AZIa.ang_sig, gp_Ast->AZIa.ang_DD, gp_Ast->AZIa.ang_MM ) ;
  sprintf( c_ddmm_alt, "%c %-3d d %d'", gp_Ast->ALTa.ang_sig, gp_Ast->ALTa.ang_DD, gp_Ast->ALTa.ang_MM ) ;
  sprintf( c_ddmm_dec, "%c %-3d d %d'", gp_Ast->DECa.ang_sig, gp_Ast->DECa.ang_DD, gp_Ast->DECa.ang_MM ) ;

  sprintf( c_dd_agh,   "%c %-3d deg",   gp_Ast->AGHa.ang_sig, gp_Ast->AGHa.ang_DD ) ;
  sprintf( c_dd_asc,   "%c %-3d deg",   gp_Ast->ASCa.ang_sig, gp_Ast->ASCa.ang_DD ) ;
  sprintf( c_dd_azi,   "%c %-3d deg",   gp_Ast->AZIa.ang_sig, gp_Ast->AZIa.ang_DD ) ;
  sprintf( c_dd_alt,   "%c %-3d deg",   gp_Ast->ALTa.ang_sig, gp_Ast->ALTa.ang_DD ) ;
  sprintf( c_dd_dec,   "%c %-3d deg",   gp_Ast->DECa.ang_sig, gp_Ast->DECa.ang_DD ) ;

  /* Sauvegarde des donnees formatees dans la structure astre */
  
  MUTEX_GLO_AST_LOCK

  strcpy( gp_Ast->c_hhmmss_agh, c_hhmmss_agh)  ;
  strcpy( gp_Ast->c_hhmmss_asc, c_hhmmss_asc)  ;
  strcpy( gp_Ast->c_hhmmss_azi, c_hhmmss_azi)  ;
  strcpy( gp_Ast->c_hhmmss_alt, c_hhmmss_alt)  ;
  strcpy( gp_Ast->c_hhmmss_dec, c_hhmmss_dec)  ;

  strcpy( gp_Ast->c_hhmm_agh, c_hhmm_agh)  ;
  strcpy( gp_Ast->c_hhmm_asc, c_hhmm_asc)  ;
  strcpy( gp_Ast->c_hhmm_azi, c_hhmm_azi)  ;
  strcpy( gp_Ast->c_hhmm_alt, c_hhmm_alt)  ;
  strcpy( gp_Ast->c_hhmm_dec, c_hhmm_dec)  ;

  strcpy( gp_Ast->c_ddmm_agh, c_ddmm_agh)  ;
  strcpy( gp_Ast->c_ddmm_asc, c_ddmm_asc)  ;
  strcpy( gp_Ast->c_ddmm_azi, c_ddmm_azi)  ;
  strcpy( gp_Ast->c_ddmm_alt, c_ddmm_alt)  ;
  strcpy( gp_Ast->c_ddmm_dec, c_ddmm_dec)  ;

  strcpy( gp_Ast->c_dd_agh, c_dd_agh)  ;
  strcpy( gp_Ast->c_dd_asc, c_dd_asc)  ;
  strcpy( gp_Ast->c_dd_azi, c_dd_azi)  ;
  strcpy( gp_Ast->c_dd_alt, c_dd_alt)  ;
  strcpy( gp_Ast->c_dd_dec, c_dd_dec)  ;

  MUTEX_GLO_AST_UNLOCK
}

/*****************************************************************************************
* @fn     : ASTRE_AFFICHER_MODE_STELLARIUM
* @author : s.gravois
* @brief  : Cette fonction affiche les informations a la sauce stellarium
* @param  : 
* @date   : 2022-03-18 creation
* @date   : 2022-03-28 simplification
*****************************************************************************************/

void ASTRE_AFFICHER_MODE_STELLARIUM(STRUCT_ASTRE *gp_Ast) {

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("Va / Vh    : %3.2f / %3.2f" , gp_Ast->Va           , gp_Ast->Vh ) ;
  Trace("AD / Dec   : %s / %s"       , gp_Ast->c_hhmmss_asc , gp_Ast->c_ddmm_dec ) ;
  Trace("AH / Dec   : %s / %s"       , gp_Ast->c_hhmmss_agh , gp_Ast->c_ddmm_dec ) ;
  Trace("AZ./ Haut. : %s / %s"       , gp_Ast->c_ddmm_azi   , gp_Ast->c_ddmm_alt ) ;
}

