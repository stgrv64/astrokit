/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 01/05/2021  | ajout entete
# 01/05/2021  | creation entete de la fonction au format doxygen #define gp_Pwm_Par->par_alt_fpwm 
#   suite a ajout de la variable du meme nom dans types.h
# mai 2022    | ajout / modifications sur les threads  
#               ajout / renommage membres champs struct PMW phases et moteurs
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_GPIO_H
#define ASTRO_GPIO_H

#include "astro_global.h"

/*---------------------------------------------------*/
/* Parametres de fichier config (modif octobre 2022) */ 
/*---------------------------------------------------*/

/* ---------------------------------------------------------------
  les params suivants servent pour utilisation d un controleur externe
  pour les pas et micro pas et l'amplification des signaux (OBSOLETE)
  -------------------------------------------------------------------- */

struct STR_GPIO_PARAMS_CONTROLER {

  pthread_mutex_t par_mutex ;
  unsigned char   par_alt_dir ;  // numero de port GPIO pour le sens de rotation en altitude
  unsigned char   par_alt_clk ;  // numero de port GPIO pour l'horloge de l'altitude
  unsigned char   par_alt_slp ;  // numero de port GPIO pour le sleep en altitude
  unsigned char   par_alt_rst ;  // numero de port GPIO pour le reset en altitude
  unsigned char   par_alt_mmm ;  // numero de port GPIO pour le mmm en altitude
  unsigned char   par_alt_ena ;  // numero de port GPIO pour le enable en altitude
  unsigned char   par_alt_m2  ;  // numero de port GPIO pour le choix du micro pas en altitude
  unsigned char   par_alt_m1  ;  // numero de port GPIO pour le choix du micro pas en altitude
  unsigned char   par_alt_m0  ;  // numero de port GPIO pour le choix du micro pas en altitude
  unsigned char   par_azi_dir ;  // numero de port GPIO pour le sens de rotation en azimut
  unsigned char   par_azi_clk ;  // numero de port GPIO pour l'horloge de l'azimut
  unsigned char   par_azi_slp ;  // numero de port GPIO pour le sleep en azimut
  unsigned char   par_azi_rst ;  // numero de port GPIO pour le reset en azimut
  unsigned char   par_azi_mmm ;  // numero de port GPIO pour le mmm en azimut
  unsigned char   par_azi_ena ;  // numero de port GPIO pour le enable en azimut
  unsigned char   par_azi_m2  ;  // numero de port GPIO pour le choix du micro pas en azimut
  unsigned char   par_azi_m1  ;  // numero de port GPIO pour le choix du micro pas en azimut
  unsigned char   par_azi_m0  ;  // numero de port GPIO pour le choix du micro pas en azimut
} ;

/* 
  les params suivants servent pour utilisation d'une raquette O/N/E/S
  reliee directement aux gpios (OBSOLETE)
*/

struct STR_GPIO_PARAMS_RAQUETTE {

  pthread_mutex_t par_mutex ;
  int par_raq_ouest  ; 
  int par_raq_est  ;
  int par_raq_sud  ;
  int par_raq_nord  ;
  int par_raq_v  ;
} ;

/* 
  les params suivants servent pour utilisation d'un clavier matriciel 4*4
  reliee directement aux 8 gpios par multiplexage (OBSOLETE)
*/

struct STR_GPIO_PARAMS_MATRICIEL {

  pthread_mutex_t par_mutex ;
  int par_l1  ;
  int par_l2  ;
  int par_l3  ;
  int par_l4  ;
  int par_c1  ;
  int par_c2  ;
  int par_c3  ;
  int par_c4  ;
} ;

/* 
  Les params suivants servent pour utilisation d'une modulation PWM
  sur les sorties representees par altitude (4 broches) et azimut
  un masque d 'attaque est utilise

  Ces parametres sont bufferisés dans cette structure , mais lus directement
  dans les variables globales gi_* du fichier gpio.h / global.h
*/

struct STR_GPIO_PARAMS_PWM {

  pthread_mutex_t par_mutex ;

  int             par_led_etat ; /* TODO : mettre ailleurs */
  char            par_alt_fpwm [ CONFIG_TAILLE_BUFFER_64 ] ;
  char            par_alt_gpio     [ CONFIG_TAILLE_BUFFER_64 ] ;
  char            par_alt_mask     [ CONFIG_TAILLE_BUFFER_64 ] ;
  char            par_azi_gpio     [ CONFIG_TAILLE_BUFFER_64 ] ;
  char            par_azi_mask     [ CONFIG_TAILLE_BUFFER_64 ] ;
  char            par_azi_fpwm [ CONFIG_TAILLE_BUFFER_64 ] ;
} ;

/*---------------------------------------------------*/
/* Fin parametres de fichier config                  */ 
/*---------------------------------------------------*/

typedef enum {

  GPIO_RED_INDETERMINE=0,
  GPIO_RED_MONTURE_NB_DENTS,
  GPIO_RED_POULIE_MONTURE_NB_DENTS,
  GPIO_RED_POULIE_MOTEUR_NB_DENTS,
  GPIO_RED_REDUCTEUR_PLANETAIRE,
  GPIO_RED_MOTEUR_NB_PAS,
  GPIO_RED_MOTEUR_NB_MICROPAS,
  GPIO_RED_CPU_CORRECTION
}
t_en_Gpio_Red_Type ;

/*---------------------------------------------------*/
/* Structure servant a gerer la phase d un moteur    */ 
/*---------------------------------------------------*/

struct STR_GPIO_PWM_PHASE {

  pthread_mutex_t   pha_mutex ;    

  /* STRUCT_SUIVI    * p_sui ; */
  /* STRUCT_PTHREADS * p_pth ; */

  struct timeval    pha_tval ;

  double            pha_rap[ GPIO_MICROPAS_MAX ] ; 
  double            pha_per_pwm ;  
  double            pha_per_mic ;
  double            pha_per_mot ;  
  double            pha_rc ;
  int               pha_id ;  
  int               pha_pas ;
  int               pha_upas ;
  int               pha_gpio ;
  int               pha_gpio_open_statut ;
  int               pha_gpio_fd ;

} ;

/*---------------------------------------------------*/
/* Structure servant a gerer la bobine d un moteur   */ 
/* Un moteur PAP = 2 bobines de 2 phases (4 fils)    */
/*---------------------------------------------------*/

struct STR_GPIO_PWM_MOTEUR {

  pthread_mutex_t         mot_mutex ; 

  STRUCT_GPIO_PWM_PHASE * mot_pha[ GPIO_NB_PHASES_PAR_MOTEUR ] ;

  struct timeval          mot_timeval ;
  /* STRUCT_SUIVI          * p_sui ; */
  /* STRUCT_PTHREADS       * p_pth ; */
  
  int                     mot_id ;
  long long               mot_pas ;
  int                     mot_upas ;
  double                  mot_per_mic ;
  double                  mot_per_mot ;
  double                  mot_t ;
  double                  mot_fm ;  
  double                  mot_tm ;  
  double                  mot_fre_pwm ;  
  double                  mot_per_pwm ; 
  double                  mot_nb_upas ; 
  long                    nbdeltat ;
  int                     type_fonction ; 
  double                  param0 ;
  double                  param1 ;

  // FIXME : la difference entre temps et  tps_ree : voir fonction *_GPIO_PWM_MOT
  //         *  temps      : on utilise periode , qui sert pour le usleep 
  //         *  tps_ree : on utilise gettimeofday , qui sert a calculer la duree de la boucle while(en entier)

  /* Les 3 champs suivants permettent d avoir une indication sur le temps rell consomm et le temps reel moteur
     du au calcul des periodes et frequences brutes, l ecart correspond aux latences entre chaque usleep ou a la 
    somme de la l incertitude sur les usleep (dans la fonction SUIVI_MAIN)
    On a les inegalites suivantes : 

    tps_ree > tps_mic
    
    En asservissant , on souhaite avoir : 
    
    tps_ree = tps_bru
  */
  double  tps_mic ; /* somme des usleep dans la fonction de thread void * SUIVI_MAIN */
  double  tps_ree ; /* somme des temps reels consomme entre les usleep (en utilisant gettimeofday )*/
  double  tps_bru ; /* somme des periodes T<x>_bru brutes calculees dans CALCULS_PERIODE */
  double  tps_mot ; /* somme des periodes T<x>_mot (T<x>_bru * accelerations) */ 
  double  temps_moyen ;       

  double  temps_reel_moyen ;  
} ;

/* ------------------------------------------------------------------------
  - ajout definition "raquettes de commande" (obsolete : pour memoire)
  - passage des const en static const car deplaces du .c dans le .h
  ------------------------------------------------------------------------ */

static const char  keyboard[4][4][GPIO_TAILLE_BUFFER] = \
  {{"1","4",   "7", "MENU"},\
  {"2" ,"5",   "8", "0"},\
  {"3" ,"6",   "9", "."},\
  {"MES" ,"NGC", "ETO", "valider"}} ;
  
static const char  raquette[4][4][GPIO_TAILLE_BUFFER] = \
  {{"no" ,"o"    ,"so"     ,"MENU"},\
  { "n"  ,"reset","s"      ,"0" },\
  { "ne" ,"e"    ,"se"     ,"."},\
  { "MES"  ,"NGC"  , "ETO", "valider"}} ;
  
static const char  keyboard_ir[4][4][GPIO_TAILLE_BUFFER] = \
  {{"KEY_1" ,"KEY_4" , "KEY_7" , "KEY_MENU"}, \
  { "KEY_2" ,"KEY_5" , "KEY_8" , "KEY_0" },\
  { "KEY_3" ,"KEY_6" , "KEY_9" , "KEY_P"}, \
  { "KEY_M" ,"KEY_N" , "KEY_E",  "KEY_OK"}} ;
  
static const char  raquette_ir[4][4][GPIO_TAILLE_BUFFER] = \
  {{"KEY_1"  ,"KEY_LEFT"  , "KEY_7"  , "KEY_MENU"}, \
  { "KEY_UP" ,"KEY_OK"    , "KEY_DOWN" , "KEY_0" },\
  { "KEY_3"  ,"KEY_RIGHT" , "KEY_9" , "KEY_P"}, \
  { "KEY_M" ,"KEY_N" , "KEY_E",  "KEY_OK"}} ;

//==========================================================================
// broches pour l'injection directe d'une frequence depuis 2 ports GPIO dans le cas 
// ou la carte electronique de division par etage de la frequence de ref (32768 Hz)
// n'est pas utilise ni le diviseur puissance de deux ni le diviseur 4059
// ( division 2^N puis par Division decimal _ utilisation de D dans param _ )
// On definit egalement les autres broches utiles a savoir :
// - sens = sens de rotation du moteur, en cas d'erreur lors du montage, on utilise
//          le flag ERR_SENS par defaut egal a 1
//==========================================================================

void   GPIO_GETOPT(int argc, char ** argv) ;


void   GPIO_TEST_CONTROLER (void ) ;
void   GPIO_LED_ETAT_CLIGNOTE     (int , int ) ;
void   GPIO_CONFIG_FIC_READ         (STRUCT_CONFIG *) ;
void   GPIO_READ2        (STRUCT_CONFIG *) ;
/*
void   GPIO_CLAVIER_MATRICIEL_CONFIG  (int [4],int [4]) ;
void   GPIO_CLAVIER_MATRICIEL_READ    (int [4],int [4],STRUCT_KEYS* ) ;
*/
void   GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS(int [4],int [4], STRUCT_SUIVI *) ;
void   GPIO_TRAP(int ) ;
void   GPIO_STATUT(void) ;
int    GPIO_GET(int ) ;
int    GPIO_SET(int ,int ) ;
int    GPIO_OPEN_BROCHE(int ,int ) ;
void   /* GPIO_OPEN_BROCHE */_PWM(STRUCT_GPIO_PWM_PHASE *) ;
int    GPIO_OPEN(int gi_gpio_in[GPIO_SIZE],int [GPIO_SIZE]) ;
int    GPIO_CLOSE_BROCHE(int gpio) ;
int    GPIO_CLOSE(int gi_gpio_in[GPIO_SIZE],int gi_gpio_out[GPIO_SIZE]) ;
void   GPIO_SET_ALT(int dir, int slp, int clk, int rst, int mmm, int ena) ;
void   GPIO_SET_AZI(int dir, int slp, int clk, int rst, int mmm, int ena) ;
void   GPIO_MOVE_1(int sens, double periode,double nb_pulse, int gpio_dir, int gpio_clk) ;
void   GPIO_MOVE_2(int sens, double periode,unsigned  long nb_pulse) ;
void   GPIO_MOVE_3(int sens, double periode,double nb_pulse, int gpio_dir, int gpio_clk) ;
long   GPIO_ACCELERATION_1(int gpio, double f_deb,double f_fin, double delai,long nano_moins) ;
long   GPIO_ACCELERATION_2(int alt, int azi, double f_deb,double f_fin, double delai,long nano_moins) ;

// PWM
void   GPIO_PWM_PARAMS_INIT    ( STRUCT_GPIO_PARAMS_PWM * ) ;
void   GPIO_PWM_PARAMS_DISPLAY ( STRUCT_GPIO_PARAMS_PWM * ) ;

void   GPIO_INIT_PWM_MOTEUR(STRUCT_GPIO_PWM_MOTEUR *lp_Mot, int gpios[ GPIO_NB_PHASES_PAR_MOTEUR ], int masque[ GPIO_NB_PHASES_PAR_MOTEUR ],  double upas, double fm, double fpwm, int id, int type_fonction, double param0, double param1) ;
void   GPIO_CALCULS_PWM_RAPPORTS_CYCLIQUES(STRUCT_GPIO_PWM_MOTEUR *lp_Mot) ;

// Fonction de threads

void * _GPIO_PWM_PHASE(STRUCT_GPIO_PWM_PHASE *lp_Pha ) ;
void * _GPIO_PWM_MOT(STRUCT_GPIO_PWM_MOTEUR *lp_Mot) ;

#endif
