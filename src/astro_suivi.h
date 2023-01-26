/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-10-07  | * creation
# 2022-10-07  | * rapatriement code depuis astro_global.h et astro.c et autres
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_SUIVI_H
#define ASTRO_SUIVI_H

#include "astro_global.h"

#define SUIVI_MAIN_TA_RESET         0.1 
#define SUIVI_MAIN_TH_RESET         0.1 
#define SUIVI_MAIN_FA_RESET         10 
#define SUIVI_MAIN_FH_RESET         10
#define SUIVI_MAIN_TA_TRANSITOIRE   0.25
#define SUIVI_MAIN_TH_TRANSITOIRE   0.25
#define SUIVI_MAIN_FA_TRANSITOIRE   4
#define SUIVI_MAIN_FH_TRANSITOIRE   4

typedef enum { 

  MENU_AZIMUTAL=0,
  MENU_EQUATORIAL,
  MENU_MANUEL_BRUT,
  MENU_MANUEL_NON_ASSERVI,
  MENU_MANUEL_ASSERVI,
  MENU_GOTO,
  MENU_RESEAU_UP,
  MENU_RESEAU_DOWN,
  MENU_PROGRAMME_DOWN,
  MENU_DOWN
}
t_en_Menus ;

/*------------------------------------------------*/
/* Structure de suivi des Pas                     */
/*------------------------------------------------*/

typedef struct STR_SUIVI_PAS STRUCT_SUIVI_PAS ;

struct STR_SUIVI_PAS {

  pthread_mutex_t   pas_mutex ;
  STR_EXT_TIMEVAL   pas_tval ; 
  FILE             *pas_file ;
  void            (*pas_log)     ( STRUCT_SUIVI_PAS * ) ;
  void            (*pas_display) ( STRUCT_SUIVI_PAS * ) ;
  void            (*pas_lock)    ( STRUCT_SUIVI_PAS * ) ;
  void            (*pas_unlock)  ( STRUCT_SUIVI_PAS * ) ;  
  int               pas_loglevel ;
  char              pas_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char              pas_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;
  int               pas_rst ; 
  double            pas_acc_azi ;   // acceleration volontaire en azimut   
  double            pas_acc_alt ;   // acceleration volontaire en altitude 
  long              pas_azi ;
  long              pas_alt ;
  long              pas_azi_old ;
  long              pas_alt_old ;
  long              pas_asc ;
  long              pas_dec ;
  long              pas_ouest ;     // flag d'appui sur le touche OUEST
  long              pas_est ;       // flag d'appui sur le touche EST
  long              pas_nord ;      // flag d'appui sur le touche NORD
  long              pas_sud ;       // flag d'appui sur le touche SUD
  int               pas_acc_plus ;
  int               pas_acc_moins ;
  int               pas_forward ;
  int               pas_rewind ;
  int               pas_forward_fast ;
  int               pas_rewind_fast ;
} ;

/*------------------------------------------------*/
/* Structure de suivi des Frequences              */
/*------------------------------------------------*/

typedef struct STR_SUIVI_FREQUENCES STRUCT_SUIVI_FRE ;

struct STR_SUIVI_FREQUENCES {

  pthread_mutex_t   fre_mutex ;
  STR_EXT_TIMEVAL   fre_tval ; 
  FILE             *fre_file ;
  void            (*fre_log)     ( STRUCT_SUIVI_FRE * ) ;
  void            (*fre_display) ( STRUCT_SUIVI_FRE * ) ;
  void            (*fre_lock)    ( STRUCT_SUIVI_FRE * ) ;
  void            (*fre_unlock)  ( STRUCT_SUIVI_FRE * ) ;  
  int               fre_loglevel ;
  char              fre_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char              fre_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;

  double            fre_ta_mic ; // periode de la frequence a injecter directement
  double            fre_th_mic ; // periode de la frequence a injecter directement
  double            fre_fa_mic ; // frequence a injecter directement (tient compte des micro pas)
  double            fre_fh_mic ; // frequence a injecter directement (tient compte des micro pas)

  double            fre_ta_bru ; // periode brute avant corrections (accelerations et micro pas)
  double            fre_th_bru ; // periode brute avant corrections (accelerations et micro pas)
  double            fre_fa_bru ; // frequence brute avant corrections (accelerations et micro pas)
  double            fre_fh_bru ; // frequence brute avant corrections (accelerations et micro pas)

  double            fre_ta_mot ; // periode de la frequence moteur (ne tient compte des micro pas)
  double            fre_th_mot ; // periode de la frequence moteur (ne tient compte des micro pas)
  double            fre_fa_mot ; // frequence du moteur deduite      (ne tient compte des micro pas)
  double            fre_fh_mot ; // frequence du moteur deduite      (ne tient compte des micro pas)
  
  int               fre_sa ;      // signe de la vitesse (direction), tenir compte du flag de inversion
  int               fre_sh ;      // signe de la vitesse (direction), tenir compte du flag de inversion
  int               fre_sa_old ;  // flag de comparaison pour raffraichir ou non les ecritures
  int               fre_sh_old ;  // flag de comparaison pour raffraichir ou non les ecritures

} ;

/*------------------------------------------------------------*/
/* Structure de suivi (split dans pas et frequences (2023)    */
/*------------------------------------------------------------*/

typedef struct STR_SUIVI STRUCT_SUIVI ;

struct STR_SUIVI {

  STR_EXT_TIMEVAL   sui_tval ; 
  pthread_mutex_t   sui_mutex ;
  FILE             *sui_file ;
  void            (*sui_log)     ( STRUCT_SUIVI * ) ;
  void            (*sui_display) ( STRUCT_SUIVI * ) ;
  void            (*sui_lock)    ( STRUCT_SUIVI * ) ;
  void            (*sui_unlock)  ( STRUCT_SUIVI * ) ;  
  int               sui_loglevel ;
  char              sui_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char              sui_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;
  /* obsolete ?
  double            sui_plus ;    // multiplicateur ajustement des periodes si besoin
  double            sui_moins ;   // multiplicateur ajustement des periodes si besoin
  */
  double            sui_Da ;  // nombre a injecter dans les diviseurs de frequence
  double            sui_Dh ;  // nombre a injecter dans les diviseurs de frequence
  t_en_Menus        sui_menu;      // valeur du menu courant 
  t_en_Menus        sui_menu_old ; // sauvegarde du menu precedent
  int               sui_alarme ; // si different de 0 provoque une alarm(sui_alarme) au debut de main
  double            sui_tempo_percent ;
  double            sui_dth ;
  double            sui_dta ;
    
}  ;

static const char * gc_hach_suivi_menus[] = {
  "MENU_AZIMUTAL",
  "MENU_EQUATORIAL",
  "MENU_MANUEL_BRUT",
  "MENU_MANUEL_NON_ASSERVI",
  "MENU_MANUEL_ASSERVI",
  "MENU_GOTO",
  "MENU_RESEAU_UP",
  "MENU_RESEAU_DOWN",
  "MENU_PROGRAMME_DOWN",
  "MENU_DOWN"
} ;

void SUIVI_INIT                    ( STRUCT_SUIVI * ) ;
void SUIVI_PAS_INIT                ( STRUCT_SUIVI_PAS * ) ;
void SUIVI_FRE_INIT                ( STRUCT_SUIVI_FRE * ) ;

void SUIVI_TRAITEMENT_MOT          ( STRUCT_SUIVI * ) ;
void SUIVI_MANUEL_BRUT             ( STRUCT_SUIVI * ) ;
void SUIVI_MANUEL_ASSERVI          ( STRUCT_SUIVI * ) ;
void SUIVI_OLD_0                   ( STRUCT_SUIVI * ) ;
void SUIVI_MENU_BEFORE_WHILE       ( STRUCT_SUIVI * ) ;
/* old */
void   SUIVI_TEMPORISATION_ALTITUDE  ( STRUCT_SUIVI * , struct timeval * )  ;
void   SUIVI_TEMPORISATION_AZIMUT    ( STRUCT_SUIVI * , struct timeval * ) ;
#endif