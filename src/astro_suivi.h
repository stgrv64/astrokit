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
	SUIVI_MANUEL=0,
	SUIVI_EQUATORIAL,
	SUIVI_AZIMUTAL,
}
t_en_Suivis ;

typedef enum { 

  MENU_AZIMUTAL=0,
  MENU_EQUATORIAL,
  MENU_MANUEL_BRUT,
  MENU_MANUEL_NON_ASSERVI,
  MENU_MANUEL_ASSERVI,
  MENU_GOTO,
  MENU_INFO,
  MENU_RESEAU_UP,
  MENU_RESEAU_DOWN,
  MENU_PROGRAMME_DOWN,
  MENU_DOWN
}
t_en_Menus ;

struct STR_SUIVI {

  STRUCT_PTHREADS     * p_pth ;
  STRUCT_DATAS          sui_dat ;

  int          SUIVI_MANUEL ;
  int          SUIVI_ALIGNEMENT ;
  int          SUIVI_GOTO ;
  int          SUIVI_VOUTE ;
  int          SUIVI_EQUATORIAL ;

  long         t_diff ;
  double       t_diff_sec ;
  
  double       d_temps ;   
  double       d_appui_raq_azi ;
  double       d_appui_raq_alt ;
  
  double       roll ;
  double       pitch ;
  double       heading ;
     
  long         l_NANO_MOINS ;     // a retirer sur les temporisations pour les tests
  
  unsigned int i_statut_suivi ;
  
  /* les variables suivantes compensent le calcul des */
  /* frequences et periodes (par defaut sont egales à 1) */

  double       acc_azi_pid ;  // acceleration deduite en azimut   pour asservissement
  double       acc_alt_pid ;  // acceleration deduite en altitude pour asservissement

  double       acc_azi ;      // acceleration volontaire en azimut   
  double       acc_alt ;      // acceleration volontaire en altitude 

  int          sgn_azi ;      // signe de la frequence en azimut  a modifier au besoin
  int          sgn_alt ;      // signe de la frequence en altitude a modifier au besoin

  // Variables utilisees en mode MANUEL EVOLUE
  int          pas_acc_plus ;
  int          pas_acc_moins ;
  int          pas_forward ;
  int          pas_rewind ;
  int          pas_forward_fast ;
  int          pas_rewind_fast ;
  long         pas_azi ;
  long         pas_alt ;
  long         pas_azi_old ;
  long         pas_alt_old ;
  long         pas_asc ;
  long         pas_dec ;
  long         pas_ouest ;     // flag d'appui sur le touche OUEST
  long         pas_est ;       // flag d'appui sur le touche EST
  long         pas_nord ;      // flag d'appui sur le touche NORD
  long         pas_sud ;       // flag d'appui sur le touche SUD
  
  double       Ta_mic ;       // periode de la frequence a injecter directement
  double       Th_mic ;       // periode de la frequence a injecter directement
  double       Ta_bru ;   // periode brute avant corrections (accelerations et micro pas)
  double       Th_bru ;   // periode brute avant corrections (accelerations et micro pas)
  double       Ta_mot ;   // periode de la frequence moteur (ne tient compte des micro pas)
  double       Th_mot ;   // periode de la frequence moteur (ne tient compte des micro pas)

  double       Fa_bru ;   // frequence brute avant corrections (accelerations et micro pas)
  double       Fh_bru ;   // frequence brute avant corrections (accelerations et micro pas)
  double       Fa_mot ;   // frequence du moteur deduite      (ne tient compte des micro pas)
  double       Fh_mot ;   // frequence du moteur deduite      (ne tient compte des micro pas)
  double       Fa_mic ;       // frequence a injecter directement (tient compte des micro pas)
  double       Fh_mic ;       // frequence a injecter directement (tient compte des micro pas)

  double        Tac ;             // correcteur de periode, pour corriger les effets des latences du systeme, calculer par suivi voute
  double        Thc ;             // correcteur de periode, pour corriger les effets des latences du systeme, calculer par suivi voute

  double        Tacc ;            // correcteur de correcteur de periode, pour corriger les insuffisances du correcteur de base 
  double        Thcc ;            // correcteur de correcteur de periode, pour corriger les insuffisances du correcteur de base 

  unsigned long Ia ;              // nombre d'impulsions mesurees sur azimut
  unsigned long Ih ;              // nombre d'impulsions mesureees sur altitude

  unsigned long Ia_prec ;         // nombre d'impulsions mesurees sur azimut
  unsigned long Ih_prec ;         // nombre d'impulsions mesureees sur altitude 

  double        Ias ;             // somme sur nombres d'impulsions mesurees 
  double        Ihs ;             // somme sur nombres d'impulsions mesurees 

  unsigned long Iat[STATS_ASS] ; // tableau des nombres d'impulsions mesurees
  unsigned long Iht[STATS_ASS] ; // tableau des nombres d'impulsions mesurees
  
  double       plus ;    // multiplicateur ajustement des periodes si besoin
  double       moins ;   // multiplicateur ajustement des periodes si besoin
  
  int          Sa ;      // signe de la vitesse (direction), tenir compte du flag de reversibilite
  int          Sh ;      // signe de la vitesse (direction), tenir compte du flag de reversibilite
  int          Sa_old ;  // flag de comparaison pour raffraichir ou non les ecritures
  int          Sh_old ;  // flag de comparaison pour raffraichir ou non les ecritures
  
  double       Da ;  // nombre a injecter dans les diviseurs de frequence
  double       Dh ;  // nombre a injecter dans les diviseurs de frequence
  
  t_en_Menus   menu;      // valeur du menu courant 
  t_en_Menus   menu_old ; // sauvegarde du menu precedent

  int          alarme ; // si different de 0 provoque une alarm(alarme) au debut de main
  int          reset ;  // sert de reset pour remettre les compteeurs a zero dans diverses parties du programme
  
  double       pourcentage_tempo ;
  double       DTh ;
  double       DTa ;
  
  unsigned long temporisation_menu      ;
  unsigned long temporisation_voute     ;
  unsigned long temporisation_raq       ;
  unsigned long temporisation_ir        ; 
  unsigned long temporisation_termios   ; 
  unsigned long temporisation_capteurs  ; 
  unsigned long temporisation_clavier   ; 
  unsigned long temporisation_lcd_loop  ;
  unsigned long temporisation_lcd_disp  ;
  unsigned long temporisation_pid_loop  ;

  struct timeval tval ; 
}  ;

typedef struct STR_SUIVI STRUCT_SUIVI ;

void   SUIVI_INIT           ( STRUCT_SUIVI * ) ;
void   SUIVI_OLD_0          ( STRUCT_SUIVI * ) ;
void   SUIVI_TRAITEMENT_MOT ( STRUCT_SUIVI *, STRUCT_KEYS * ) ;
void   SUIVI_MENU_PREALABLE ( STRUCT_SUIVI * ) ;
void   SUIVI_MANUEL_BRUT    ( STRUCT_SUIVI * , STRUCT_KEYS *) ;
void   SUIVI_MANUEL_ASSERVI ( STRUCT_SUIVI * , STRUCT_KEYS *) ;
/* old */
void   SUIVI_TEMPORISATION_ALTITUDE  ( STRUCT_SUIVI * , struct timeval * )  ;
void   SUIVI_TEMPORISATION_AZIMUT    ( STRUCT_SUIVI * , struct timeval * ) ;
#endif