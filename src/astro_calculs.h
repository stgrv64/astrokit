/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# Fichier     | astro_arguments.h
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 19/01/2022  | * ajout entete
#               * ajouts fonctions utilisant getopt
# 21/01/2022  | * ajout CALCUL_ASCENSION_DROITE
#                 pour inverser les calculs quand on a azi et alt
# 29/09/2022  | * correction buffer overfow => 
#     deplacement et correction code de recuperation type et nom de astre
#     dans CALCUL_ASTRE_RECUP_TYPE_ET_NOM
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_CALCULS_H
#define ASTRO_CALCULS_H

static const char * c_Reduction_Type[] = {
  "REDUCTION_INDETERMINE",
  "REDUCTION_MONTURE_NB_DENTS",
  "REDUCTION_POULIE_MONTURE_NB_DENTS",
  "REDUCTION_POULIE_MOTEUR_NB_DENTS",
  "REDUCTION_REDUCTEUR_PLANETAIRE",
  "REDUCTION_MOTEUR_NB_PAS",
  "REDUCTION_MOTEUR_NB_MICROPAS",
  "REDUCTION_CPU_CORRECTION"
} ;

static const char * c_Calculs_Mode[] = {
  "CALCUL_INDETERMINE",  
  "CALCUL_AZIMUTAL_VERS_EQUATORIAL",
  "CALCUL_EQUATORIAL_VERS_AZIMUTAL"
} ;

#include "astro_global.h"

#define  CALCUL_ZERO_CHAR            0
#define  CALCUL_DUREE_JOUR_SIDERAL   86164.0   /* duree du jour sideral en seconde */
#define  CALCUL_PI_FOIS_DEUX         6.283185307179586476925
#define  CALCUL_OMEGA                0.000072921235169903748
#define  CALCUL_SIN45                0.70710678118654
#define  CALCUL_COS45                0.70710678118654
#define  CALCUL_UN_RADIAN_EN_DEGRES  57.29577951308232
#define  CALCUL_DIVISEUR_SEPCIFIQUE  206264.8062470964   // 60 * 60 * CALCUL_UN_RADIAN_EN_DEGRES
#define  CALCUL_NB_SECARC_PAR_S_EQU  15.04108444361914   // angle en secondes parcouru pendant une seconde a l'equateur

#define  CALCUL_VIT_ROT_EARTH_RAD_PER_SEC          0.000072921235170   // vitesse de rotation de la Terre (radians par seconde)
#define  CALCUL_VIT_ROT_EARTH_DEG_PER_SEC          0.004178079012116   // vitesse de rotation de la Terre (degres par seconde)

typedef enum {

  REDUCTION_INDETERMINE=0,
  REDUCTION_MONTURE_NB_DENTS,
  REDUCTION_POULIE_MONTURE_NB_DENTS,
  REDUCTION_POULIE_MOTEUR_NB_DENTS,
  REDUCTION_REDUCTEUR_PLANETAIRE,
  REDUCTION_MOTEUR_NB_PAS,
  REDUCTION_MOTEUR_NB_MICROPAS,
  REDUCTION_CPU_CORRECTION
}
t_en_Reduction_Type ;

typedef enum t_en_Reduction_Type ENUM_CALCULS_REDUCTION_TYPE ;

typedef enum {
  CALCUL_INDETERMINE=0,  
  CALCUL_AZIMUTAL_VERS_EQUATORIAL,
  CALCUL_EQUATORIAL_VERS_AZIMUTAL
}
t_en_Calculs_Mode ;

typedef enum t_en_Calculs_Mode ENUM_CALCULS_MODE ;

/*---------------------------------------------------*/
/* Parametres de fichier config (modif octobre 2022) */ 
/*---------------------------------------------------*/

struct STR_CALCULS_PARAMS {

  /* TODO : verifier OBSOLETE */
  double        par_alt_red_x[ REDUCTION_CPU_CORRECTION + 1 ] ; /* tableau des reductions en altittue */
  double        par_azi_red_x[ REDUCTION_CPU_CORRECTION + 1 ] ; /* tableau des reductions en azimut */
  /* fin TODO */

  // ------ PARAMETRES DE altitude   ------------

  unsigned long par_alt_f ;        // frequence de reference (utile si on utilise CALCUL_DIVISEUR_FREQUENCE)
  unsigned int  par_alt_n ;        // prediviseur de frequence si il existe (2 puissance N : 1 2 4 16 32 ..)
  double        par_alt_red_tot ;  // reduction totale
  double        par_alt_red_1 ;    // reduction liee a la monture
  double        par_alt_red_2 ;    // reducteur du moteur
  double        par_alt_red_3 ;    // nombre de pas du moteur en azimut
  double        par_alt_red_4 ;    // mode micro pas utilisee (1/R4)
  double        par_alt_red_5 ;    // reduction liee a la poulie
  double        par_alt_red_6 ;    // reduction liee au cpu
  double        par_alt_red_7 ;    // reduction non decrite plus haut
  int           par_alt_rev ;      // Flag de reversibilitee du sens de rotation (en cas d'erreur)
  double        par_alt_acc ;      // Facteur de multiplication en mo

  // ------ PARAMETRES DE azimut   ------------

  unsigned long par_azi_f ;    // frequence de reference (utile si on utilise CALCUL_DIVISEUR_FREQUENCE)
  unsigned int  par_azi_n ;    // prediviseur de frequence si il existe (2 puissance N : 1 2 4 16 32 ..)
  double        par_azi_red_tot ;    // reduction totale
  double        par_azi_red1 ;   // reduction liee\A0l axe en azimut
  double        par_azi_red2 ;   // reducteur du moteur
  double        par_azi_red3 ;   // nombre de pas du moteur en azimut
  double        par_azi_red4 ;   // mode micro pas utilisee (1/R4)
  double        par_azi_red5 ;   // reduction liee a la poulie
  double        par_azi_red6 ;   // reduction liee au cpu
  double        par_azi_red7 ;   // reduction non decrite plus haut
  int           par_azi_rev ;  // Flag de reversibilitee du sens de rotation (en cas d'erreur)
  double        par_azi_acc ;  // Facteur de multiplication en mode MANUEL_0

  // ------ PARAMETRES DE ALT et AZI   ------------

  double        par_altaz_slow_forward ; // Facteur de multiplication des vitesses pour rattrapages tres tres lent 
  double        par_altaz_slow_rewind ;  // Facteur de multiplication des vitesses pour rattrapages tres tres lent 
  double        par_altaz_fast_forward ; // Facteur de multiplication des vitesses pour rattrapages tres rapides 
  double        par_altaz_fast_rewind ;  // Facteur de multiplication des vitesses pour rattrapages tres rapides
} ;

typedef struct STR_CALCULS_PARAMS STRUCT_CALCULS_PARAMS ;

/*---------------------------------------------------*/
/* Fin parametres de fichier config                  */ 
/*---------------------------------------------------*/

// ------------------------------------------------------------------------
// macros de calcul 
// ------------------------------------------------------------------------

double sqr(double x)                                    ;
double max(double x1,double x2)                         ;
double SGN(double x)                                    ;
double DEC  (double LAT, double a, double h)            ;
double ASC  (double H,   double a, double h)            ;
double AGH1 (double LAT, double H, double a, double h)  ;
double AGH2 (double LAT, double H, double h)            ;
double ALT  (double LAT, double A, double H)            ;
double AZI  (double A, double H, double h)              ;
double AZI1 (double LAT, double A, double H, double h)  ;
double RAD  (int degres, int minutes )                  ;
double DEG  (int degres, int minutes )                  ;

// Ces deux fonctions permettent une temporisation precises (microsedondes) 
// en profitant d'un sleep parametrable (eviter une consommation CPU
// avant de rentrer en boucle active d'attente

void   CALCUL_GEODE                   ( STRUCT_ASTRE *gp_Ast) ;
void   CALCUL_AZIMUT                  ( STRUCT_LIEU *gp_Lie, STRUCT_ASTRE *gp_Ast) ;
void   CALCUL_EQUATEUR                ( STRUCT_LIEU *gp_Lie, STRUCT_ASTRE *gp_Ast) ;

void   CALCUL_VITESSES_EQUATORIAL     ( STRUCT_ASTRE *gp_Ast) ;
void   CALCUL_VITESSES                ( STRUCT_LIEU *gp_Lie, STRUCT_ASTRE *gp_Ast, STRUCT_SUIVI * gp_Sui) ;

void   CALCUL_ANGLE_HORAIRE           ( STRUCT_LIEU *gp_Lie, STRUCT_ASTRE *gp_Ast) ;
void   CALCUL_ASCENSION_DROITE        ( STRUCT_LIEU *gp_Lie, STRUCT_ASTRE *gp_Ast) ; 
void   CALCUL_DIVISEUR_FREQUENCE      ( STRUCT_ASTRE *gp_Ast, STRUCT_SUIVI * gp_Sui) ;   
void   CALCUL_PERIODE                 ( STRUCT_ASTRE *gp_Ast, STRUCT_SUIVI * gp_Sui,STRUCT_VOUTE *gp_Vou)  ;
void   CALCUL_PERIODES_SUIVI_MANUEL   ( STRUCT_ASTRE *gp_Ast, STRUCT_SUIVI * gp_Sui, STRUCT_VOUTE *gp_Vou) ;

void   CALCUL_CONVERSIONS_ANGLES      ( STRUCT_ASTRE *gp_Ast) ;
void   CALCUL_AFFICHER_MSG_ANGLE          ( char * mesg, STRUCT_ANGLE *angle ) ;

/* TODO : decommenter
void   SET_ASTRE ( STRUCT_ASTRE *gp_Ast,char *parametre, double valeur) ;
void   SET_LIEU  ( STRUCT_LIEU *gp_Lie,char *parametre, double valeur) ;
void   SET_VOUTE ( STRUCT_VOUTE *gp_Vou,char *parametre, double valeur) ;
*/

void   CALCUL_TOUT                    (void) ;
void   CALCUL_ASTRE_RECUP_TYPE_ET_NOM (void) ;

#endif

//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================

