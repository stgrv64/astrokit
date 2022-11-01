/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 01/05/2021  | ajout entete
# 01/05/2021  | creation entete de la fonction au format doxygen #define gp_Gpi_Par_Pwm->par_fre_pwm 
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

  unsigned char par_alt_dir ;  // numero de port GPIO pour le sens de rotation en altitude
  unsigned char par_alt_clk ;  // numero de port GPIO pour l'horloge de l'altitude
  unsigned char par_alt_slp ;  // numero de port GPIO pour le sleep en altitude
  unsigned char par_alt_rst ;  // numero de port GPIO pour le reset en altitude
  unsigned char par_alt_mmm ;  // numero de port GPIO pour le mmm en altitude
  unsigned char par_alt_ena ;  // numero de port GPIO pour le enable en altitude
  unsigned char par_alt_m2  ;  // numero de port GPIO pour le choix du micro pas en altitude
  unsigned char par_alt_m1  ;  // numero de port GPIO pour le choix du micro pas en altitude
  unsigned char par_alt_m0  ;  // numero de port GPIO pour le choix du micro pas en altitude

  unsigned char par_azi_dir ;  // numero de port GPIO pour le sens de rotation en azimut
  unsigned char par_azi_clk ;  // numero de port GPIO pour l'horloge de l'azimut
  unsigned char par_azi_slp ;  // numero de port GPIO pour le sleep en azimut
  unsigned char par_azi_rst ;  // numero de port GPIO pour le reset en azimut
  unsigned char par_azi_mmm ;  // numero de port GPIO pour le mmm en azimut
  unsigned char par_azi_ena ;  // numero de port GPIO pour le enable en azimut
  unsigned char par_azi_m2  ;  // numero de port GPIO pour le choix du micro pas en azimut
  unsigned char par_azi_m1  ;  // numero de port GPIO pour le choix du micro pas en azimut
  unsigned char par_azi_m0  ;  // numero de port GPIO pour le choix du micro pas en azimut
} ;

/* 
  les params suivants servent pour utilisation d'une raquette O/N/E/S
  reliee directement aux gpios (OBSOLETE)
*/

struct STR_GPIO_PARAMS_RAQUETTE {

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
  les params suivants servent pour utilisation d'une modulation PWM
  sur les sorties representees par altitude (4 broches) et azimut
  un masque d 'attaque est utilise
*/

struct STR_GPIO_PARAMS_PWM {
  int  par_led_etat ;
  char par_fre_pwm [ CONFIG_TAILLE_BUFFER_64 ] ;
  char par_alt     [ CONFIG_TAILLE_BUFFER_64 ] ;
  char par_azi     [ CONFIG_TAILLE_BUFFER_64 ] ;
  char par_mas     [ CONFIG_TAILLE_BUFFER_64 ] ;
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

struct STR_GPIO_PWM_PHASE {

  STRUCT_SUIVI         * p_sui ;
  STRUCT_PTHREADS      * p_pth ;
  pthread_mutex_t mutex ; 

  struct timeval tval ;

  int     id ;  
  double  Tpwm ;  

  int     pas ;
  int     micropas ;
  
  double  rap[ GPIO_MICROPAS_MAX ] ;
  double  rc ;

  int     gpio ;
  int     gpio_open_statut ;
  int     gi_gpio_fd ;
  
  double  periode_mic ;
  double  periode_mot ;  
} ;

struct STR_GPIO_PWM_MOTEUR {

  STRUCT_SUIVI          * p_sui ;
  STRUCT_PTHREADS       * p_pth ;
  STRUCT_GPIO_PWM_PHASE * p_pha[ GPIO_NB_PHASES_PAR_MOTEUR ] ;

  pthread_mutex_t mutex ; 
  
  int     id ;

  long long  pas ;
  int        micropas ;
  
  double  periode_mic ;
  double  periode_mot ;
  double  t ;

  double  Fm ;  
  double  Tm ;  
  double  Fpwm ;  
  double  Tpwm ; 

  double  nbmicropas ; 
  long    nbdeltat ;
  int     type_fonction ; 

  double  param0 ;
  double  param1 ;

  // FIXME : la difference entre temps et  tps_ree : voir fonction *suivi_main_M
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
  double  tps_bru ; /* somme des periodes T<x>_bru brutes calculees dans CALCUL_PERIODE */
  double  tps_mot ; /* somme des periodes T<x>_mot (T<x>_bru * accelerations) */ 
  double  temps_moyen ;       

  double  temps_reel_moyen ;  

  struct timeval tval ;

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


void   GPIO_TEST_MOTEURS      (void ) ;
void   GPIO_CLIGNOTE         (int , int , int ) ;
void   GPIO_READ         (char [CONFIG_DATAS_NB_LIGNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]) ;
void   GPIO_READ2        (char [CONFIG_DATAS_NB_LIGNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]) ;
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
void   GPIO_INIT_PWM_MOTEUR(STRUCT_GPIO_PWM_MOTEUR *pm, int gpios[ GPIO_NB_PHASES_PAR_MOTEUR ], int masque[ GPIO_NB_PHASES_PAR_MOTEUR ],  double upas, double fm, double fpwm, int id, int type_fonction, double param0, double param1) ;
void   GPIO_CALCUL_PWM_RAPPORTS_CYCLIQUES(STRUCT_GPIO_PWM_MOTEUR *pm) ;

// Fonction de threads

void * GPIO_SUIVI_PWM_PHASE(STRUCT_GPIO_PWM_PHASE *ph ) ;
void * suivi_main_M(STRUCT_GPIO_PWM_MOTEUR *pm) ;

#endif
