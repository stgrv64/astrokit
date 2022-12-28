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
# 21/01/2022  | * ajout CALCULS_ASCENSION_DROITE
#                 pour inverser les calculs quand on a azi et alt
# 29/09/2022  | * correction buffer overfow => 
#     deplacement et correction code de recuperation type et nom de astre
#     dans CALCULS_RECUP_MODE_ET_ASTRE_TYPE()
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_CALCULS_H
#define ASTRO_CALCULS_H

#include <pthread.h>

struct STR_ANGLE {

  pthread_mutex_t ang_mutex ;
  void          (*ang_lock)   (void) ;
  void          (*ang_unlock) (void) ;  
  unsigned long   ang_tempo ;
  char            ang_sig ;     /* signe sous forme charactere '+' ou '-' */
  int             ang_si ;      /* signe sous forme entiere '-1' ou '+1' */
  double          ang_dec_rad ; 
  double          ang_dec_deg ; 
  int             ang_dd ;      
  int             ang_mm ;      
  int             ang_ss ; 
} ;
typedef struct STR_ANGLE STRUCT_ANGLE ;

#include "astro_global.h"

#define  CALCULS_ZERO_CHAR            0
#define  CALCULS_DUREE_JOUR_SIDERAL   86164.0   /* duree du jour sideral en seconde */
#define  CALCULS_PI_FOIS_DEUX         6.283185307179586476925
#define  CALCULS_OMEGA                0.000072921235169903748
#define  CALCULS_SIN45                0.70710678118654
#define  CALCULS_COS45                0.70710678118654
#define  CALCULS_UN_RADIAN_EN_DEGRES  57.29577951308232
#define  CALCULS_DIVISEUR_SEPCIFIQUE  206264.8062470964   // 60 * 60 * CALCULS_UN_RADIAN_EN_DEGRES
#define  CALCULS_NB_SECARC_PAR_S_EQU  15.04108444361914   // angle en secondes parcouru pendant une seconde a l'equateur

#define  CALCULS_VIT_ROT_EARTH_RAD_PER_SEC          0.000072921235170   // vitesse de rotation de la Terre (radians par seconde)
#define  CALCULS_VIT_ROT_EARTH_DEG_PER_SEC          0.004178079012116   // vitesse de rotation de la Terre (degres par seconde)

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
  CALCULS_INDETERMINE=0,  
  CALCULS_AZIMUTAL_VERS_EQUATORIAL,
  CALCULS_EQUATORIAL_VERS_AZIMUTAL
}
t_en_Calculs_Mode ;

typedef enum t_en_Calculs_Mode ENUM_CALCULS_MODE ;

/*---------------------------------------------------*/
/* Parametres de fichier config (modif octobre 2022) */ 
/*---------------------------------------------------*/

struct STR_CALCULS_PARAMS {

  pthread_mutex_t cal_par_mutex ;
  void          (*cal_lock)   (void) ;
  void          (*cal_unlock) (void) ;  
  FILE           *cal_file ; 
  // ------ PARAMETRES DE altitude   ------------

  unsigned long cal_par_alt_f ;        // frequence de reference (utile si on utilise CALCULS_DIVISEUR_FREQUENCE)
  unsigned int  cal_par_alt_n ;        // prediviseur de frequence si il existe (2 puissance N : 1 2 4 16 32 ..)

  double        cal_par_alt_red_tot ;  // reduction totale
  double        cal_par_alt_red_1 ;    // reduction liee a la monture
  double        cal_par_alt_red_2 ;    // reducteur du moteur
  double        cal_par_alt_red_3 ;    // nombre de pas du moteur en azimut
  double        cal_par_alt_red_4 ;    // mode micro pas utilisee (1/R4)
  double        cal_par_alt_red_5 ;    // reduction liee a la poulie
  double        cal_par_alt_red_6 ;    // reduction liee au cpu
  double        cal_par_alt_red_7 ;    // reduction non decrite plus haut
  int           cal_par_alt_inv ;      // Flag de inversione du sens de rotation (en cas d'erreur)
  double        cal_par_alt_acc ;      // Facteur de multiplication en mo

  // ------ PARAMETRES DE azimut   ------------

  unsigned long cal_par_azi_f ;    // frequence de reference (utile si on utilise CALCULS_DIVISEUR_FREQUENCE)
  unsigned int  cal_par_azi_n ;    // prediviseur de frequence si il existe (2 puissance N : 1 2 4 16 32 ..)
  
  double        cal_par_azi_red_tot ;    // reduction totale
  double        cal_par_azi_red_1 ;   // reduction liee\A0l axe en azimut
  double        cal_par_azi_red_2 ;   // reducteur du moteur
  double        cal_par_azi_red_3 ;   // nombre de pas du moteur en azimut
  double        cal_par_azi_red_4 ;   // mode micro pas utilisee (1/R4)
  double        cal_par_azi_red_5 ;   // reduction liee a la poulie
  double        cal_par_azi_red_6 ;   // reduction liee au cpu
  double        cal_par_azi_red_7 ;   // reduction non decrite plus haut
  int           cal_par_azi_inv ;  // Flag de inversione du sens de rotation (en cas d'erreur)
  double        cal_par_azi_acc ;  // Facteur de multiplication en mode MANUEL_0

  // ------ PARAMETRES DE ALT et AZI   ------------

  double        cal_par_altaz_slow_forward ; // Facteur de multiplication des vitesses pour rattrapages tres tres lent 
  double        cal_par_altaz_slow_rewind ;  // Facteur de multiplication des vitesses pour rattrapages tres tres lent 
  double        cal_par_altaz_fast_forward ; // Facteur de multiplication des vitesses pour rattrapages tres rapides 
  double        cal_par_altaz_fast_rewind ;  // Facteur de multiplication des vitesses pour rattrapages tres rapides
} ;

typedef struct STR_CALCULS_PARAMS STRUCT_CALCULS_PARAMS ;

/*---------------------------------------------------*/
/* Fin parametres de fichier config                  */ 
/*---------------------------------------------------*/

struct STR_CALCULS {
  void (*mut_lock)   (void) ;
  void (*mut_unlock) (void) ;
  pthread_mutex_t           cal_mutex ;
  STRUCT_SUIVI_FREQUENCES * cal_p_Fre ;
  STRUCT_STATS      * cal_p_Sta ;
  STRUCT_SUIVI_PAS        * cal_p_Pas ;
  STRUCT_ASTRE            * cal_p_Ast ;
  STRUCT_CALCULS          * cal_p_Cal ;
  STRUCT_VOUTE            * cal_p_Vou ;
  STRUCT_LIEU             * cal_p_Lie ;
  STRUCT_DEVICES          * cal_p_Dev ;
  STRUCT_MUTEXS           * cal_p_Mut ;
  STRUCT_TIME             * cal_p_Tim ;
  STRUCT_SUIVI            * cal_p_Sui ;
  int                       cal_mode ;
} ;
typedef struct STR_CALCULS STRUCT_CALCULS ;

static const char * gc_hach_reduction_type[] = {
  "REDUCTION_INDETERMINE",
  "REDUCTION_MONTURE_NB_DENTS",
  "REDUCTION_POULIE_MONTURE_NB_DENTS",
  "REDUCTION_POULIE_MOTEUR_NB_DENTS",
  "REDUCTION_REDUCTEUR_PLANETAIRE",
  "REDUCTION_MOTEUR_NB_PAS",
  "REDUCTION_MOTEUR_NB_MICROPAS",
  "REDUCTION_CPU_CORRECTION"
} ;

static const char * gc_hach_calcul_mode[] = {
  "CALCULS_INDETERMINE",  
  "CALCULS_AZIMUTAL_VERS_EQUATORIAL",
  "CALCULS_EQUATORIAL_VERS_AZIMUTAL"
} ;

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

void CALCULS_INIT            ( STRUCT_CALCULS * ) ;
void CALCULS_PARAMS_INIT     ( STRUCT_CALCULS_PARAMS * ) ;
void CALCULS_PARAMS_DISPLAY  ( STRUCT_CALCULS_PARAMS * ) ;

void CALCULS_LOCK            ( STRUCT_CALCULS * ) ;
void CALCULS_UNLOCK          ( STRUCT_CALCULS * ) ;

void CALCULS_INIT_PTR        ( STRUCT_CALCULS * )  ;
void CALCULS_INIT_ANGLE      ( STRUCT_ANGLE * ) ;
void CALCULS_VOUTE                   (void) ;
void CALCULS_COORD_R3                (void) ;
void CALCULS_AZIMUT                  (void) ;
void CALCULS_EQUATEUR                (void) ;
void CALCULS_VITESSES_EQUATORIAL     (void) ;
void CALCULS_PERIODE                 (void) ;
void CALCULS_PERIODES_SUIVI_MANUEL   (void) ;
void CALCULS_VITESSES                (void) ;
void CALCULS_ANGLE_HORAIRE           (void) ;
void CALCULS_ASCENSION_DROITE        (STRUCT_ASTRE *) ; 
void CALCULS_DIVISEUR_FREQUENCE      (void) ;   
void CALCULS_CONVERSIONS_ANGLES      (STRUCT_ASTRE *) ;
void CALCULS_TOUT                    (void) ;
void CALCULS_RECUP_MODE_ET_ASTRE_TYPE(void) ;
void CALCULS_ANGLE_VERS_DMS          (STRUCT_ANGLE *) ;
/* TODO : decommenter
void   SET_ASTRE ( STRUCT_ASTRE *gp_Ast,char *parametre, double valeur) ;
void   SET_LIEU  ( STRUCT_LIEU *gp_Lie,char *parametre, double valeur) ;
void   SET_VOUTE ( STRUCT_VOUTE *gp_Vou,char *parametre, double valeur) ;
*/

#endif

//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================

