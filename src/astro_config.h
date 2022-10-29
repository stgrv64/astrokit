/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | ajout entete
# 01/05/2021  | ajout fonction CONFIG_GETCWD
# 21/03/2022  | ajout fonctions initialisations des codes 
# 22/03/2022  | ajout fonction SUIVI_MAJ_PAS (anciennement 
#               dans ir.c / .h)
# 2022-04-28  | ajout de 2 resolutions plus simple (affichage contraint par LCD) :
# 2022-04-28  | - c_hhmm_*
# 2022-04-28  | - c_dd_*  
# 2022-05-02  | ajout fonction CONFIG_PATH_FIND
# 2022-05-24  | ajout fonction PTHREADS_AFFICHER_ETAT
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_CONFIG_H
#define ASTRO_CONFIG_H

#include "astro_global.h"

/*---------------------------------------------------*/
/* Parametres de fichier config                      */ 
/*---------------------------------------------------*/

struct STR_CONFIG_PARAMS {

  int  par_default_menu ;
  char par_rep_home   [ CONFIG_TAILLE_BUFFER_64 ] ;
  char par_rep_cat    [ CONFIG_TAILLE_BUFFER_64 ] ;
  char par_rep_cfg    [ CONFIG_TAILLE_BUFFER_64 ] ;      
  char par_rep_log    [ CONFIG_TAILLE_BUFFER_64 ] ;      
  char par_rep_in     [ CONFIG_TAILLE_BUFFER_64 ] ;    
  char par_rep_scr    [ CONFIG_TAILLE_BUFFER_64 ] ; 
  char par_fic_pid    [ CONFIG_TAILLE_BUFFER_64 ] ;
  char par_fic_log    [ CONFIG_TAILLE_BUFFER_64 ] ;            
  char par_fic_dat    [ CONFIG_TAILLE_BUFFER_64 ] ;            
  char par_fic_hhm    [ CONFIG_TAILLE_BUFFER_64 ] ;        
  char par_fic_led    [ CONFIG_TAILLE_BUFFER_64 ] ; 
  char par_src_ker    [ CONFIG_TAILLE_BUFFER_64 ] ;
} ;

/*---------------------------------------------------*/
/* Fin parametres de fichier config                  */ 
/*---------------------------------------------------*/

struct STR_CONFIG {
 char g_Char_Params  [CONFIG_DATAS_NB_LIGNES] [CONFIG_DATAS_NB_COLONNES] [CONFIG_TAILLE_BUFFER_256] ;
} ;

int    CONFIG_FORMAT_ADMIS(char c) ;
int    CONFIG_FIN_LIGNE(char c) ;
int    CONFIG_DEB_COM(char c);
int    CONFIG_FIN_MOT(char c) ;
int    CONFIG_FIN_FICHIER(char c) ;
void   CONFIG_PARAMETRES_CONFIG(char g_Char_Params[CONFIG_DATAS_NB_LIGNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]) ;
void   CONFIG_PARAMETRES_AFFICHER(void) ;
void   CONFIG_AFFICHER_DATAS(char g_Char_Params[CONFIG_DATAS_NB_LIGNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]) ;
int    CONFIG_READ(char g_Char_Params[CONFIG_DATAS_NB_LIGNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]) ;
void   CONFIG_AFFICHER_MODE_LONG(void) ;
void   CONFIG_AFFICHER_TOUT(void) ;          /* FIXME ajout 20191228 */
void   CONFIG_AFFICHER_CHANGEMENTS (void) ;  /* FIXME ajout 20200102 */
void   DEVICES_AFFICHER_UTILISATION (void) ;  /* FIXME ajout 20220312 */

int    CONFIG_PATH_FIND                    ( char * , char *) ;

#endif

//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================

