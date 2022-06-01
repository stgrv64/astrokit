#define _GNU_SOURCE

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 01/05/2021  | ajout entete
# 01/05/2021  | creation entete de la fonction au format doxygen #define GPIO_FREQUENCE_PWM 
#   suite a ajout de la variable du meme nom dans types.h
# mai 2022    | ajout / modifications sur les threads  
#               ajout / renommage membres champs struct PMW phases et moteurs
# -------------------------------------------------------------- 
*/

#ifndef GPIO_H
#define GPIO_H

// INCLUDES =====================================
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <types.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <linux/i2c-dev.h>
#include <lirc_client.h>

#include <arguments.h>
#include <astro.h>
#include <calculs.h>
#include <cat.h>
#include <config.h>
//#include <gpio.h>
#include <i2c.h>
#include <ir.h>
#include <stat.h>
#include <types.h>

// FIN INCLUDES =====================================


#define GPIO_DEFAULT_ALT_MOTOR_GPIOS  "26,19,13,6"
#define GPIO_DEFAULT_AZI_MOTOR_GPIOS  "5,7,11,10"
#define GPIO_DEFAULT_MAS_MOTOR_ORDER  "3,0,2,1"

/*
#define GPIO_DEFAULT_ALT_MOTOR_GPIOS  "21,26,19,13"
#define GPIO_DEFAULT_AZI_MOTOR_GPIOS  "6,5,7,11"
#define GPIO_DEFAULT_MAS_MOTOR_ORDER  "3,0,2,1"
*/
#define GPIO_CURRENT_FONCTION          3       // 0 : triangle - 1 : sinus - 2 : mixte - 3  : sinus arrondi (best)
#define GPIO_CURRENT_FONCTION_PARAM0   0.65    // ce parametre deforme la fonction 2 et 3
#define GPIO_CURRENT_FONCTION_PARAM1   1.0     // ce parametre multiplie la fonction pour l'ajuster a 1 (5v)

#define GPIO_SUIVI_PWM_PHASE_PRIORITY  10
#define GPIO_SUIVI_MAIN_PRIORITY       5
#define GPIO_SUIVI_MAIN_ATTENTE_MAX    10     // attente maximum exprimee en seconde pour non blocage

#define GPIO_SUIVI_PWM_PHASE_SCHED   SCHED_RR
#define GPIO_SUIVI_MAIN_SCHED        SCHED_RR

#define GPIO_MICROPAS_MAX           500
// FIXME : creation entete de la fonction au format doxygen de la ligne suivante (2021)
// #define GPIO_FREQUENCE_PWM          750 
#define GPIO_NB_PHASES_PAR_MOTEUR   4
#define GPIO_FREQ_MAX               1000.0
#define GPIO_VARG_STRING            115
#define GPIO_VARG_INT               100
#define GPIO_VARG_CHAR              99
#define GPIO_MICRO_SEC              1000000.0 // frequence d'une microseconde (pour les usleep et calculs)
#define GPIO_NANO_SEC               1000000000 // frequence d'une nanoseconde (pour les nanosleep et calculs) 
#define GPIO_TIMEOUT                30 
#define GPIO_SIZE                   16
#define GPIO_FREQ_IR                38000.0
#define GPIO_TEMPORISATION_PERCENT  0.5

#define BUFFER       256
#define GPIO_PATH    "/sys/class/gpio" 
#define GPIO_TAILLE_BUFFER 16

//==========================================================================
/* mai 2022 : ajout / renommage champs  
  - micropas
  - periode_mic
  - periode_mot
  - SUIVI * suivi 
*/
typedef struct {
  
  pthread_mutex_t mutex ; 

  int     id ;  
  double  Tpwm ;  

  int     pas ;
  int     micropas ;
  
  double  rap[ GPIO_MICROPAS_MAX ] ;
  double  rc ;

  int     gpio ;
  int     gpio_open_statut ;
  int     gpio_fd ;
  
  double  periode_mic ;
  double  periode_mot ;

  struct timeval tval ;

  SUIVI  * suivi ;
}
GPIO_PWM_PHASE ;

//==========================================================================

typedef struct {
  
  pthread_mutex_t mutex ; 
  
  SUIVI          * suivi ;
  GPIO_PWM_PHASE * phase[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  
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

  // FIXME : la difference entre temps et  temps_reel : voir fonction *suivi_main_M
  //         *  temps      : on utilise periode , qui sert pour le usleep 
  //         *  temps_reel : on utilise gettimeofday , qui sert a calculer la duree de la boucle while(en entier)

  double  temps ;      // temps ecoule sur azimut ou altitude, deduit des calculs gpio : suivi_main_M, 
                       // recopie dans SUIVI* temps_a ou temps_h suivant que GPIO_PWM_MOTEUR est moteur azimut ou altitude

  double  temps_reel ; // temps REEL ecoule sur azimut ou altitude, deduit des calculs gpio : suivi_main_M, 
                       // recopie dans SUIVI* temps_a ou temps_h suivant que GPIO_PWM_MOTEUR est moteur azimut ou altitude

  double  temps_moyen ;       // temps ecoule moyen sur azimut ou altitude, deduit des calculs gpio : suivi_main_M, 
                              // recopie dans SUIVI* temps_a ou temps_h suivant que GPIO_PWM_MOTEUR est moteur azimut ou altitude

  double  temps_reel_moyen ;  // temps REEL ecoule moyen sur azimut ou altitude, deduit des calculs gpio : suivi_main_M, 
                              // recopie dans SUIVI* temps_a ou temps_h suivant que GPIO_PWM_MOTEUR est moteur azimut ou altitude

  struct timeval tval ;

}
GPIO_PWM_MOTEUR ;

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
/*
GPIO_PWM_MOTEUR *pm_alt , m_alt ; 
GPIO_PWM_MOTEUR *pm_azi , m_azi ;
*/
int gpio_key_l[4] ;
int gpio_key_c[4] ;  

int GPIO_OPEN_STATUT ;

int gpio_fd     [ GPIO_SIZE ] ;
int gpio_in   	[ GPIO_SIZE]  ;
int gpio_out    [ GPIO_SIZE] ; 

int gpio_alt    [ GPIO_NB_PHASES_PAR_MOTEUR ]  ;  // phases du moteur (utilise si CONTROLEUR vaut 0 dans config.txt)
int gpio_azi    [ GPIO_NB_PHASES_PAR_MOTEUR ] ;   // phases du moteur (utilise si CONTROLEUR vaut 0 dans config.txt)
int gpio_mas    [ GPIO_NB_PHASES_PAR_MOTEUR ] ;   // masque de redefinition des gpios des phases : evite soucis de branchement

double gpio_frequence_pwm ;

int priority ;
int g_i_trace ;
int g_i_timeout ;
int g_i_max_nb_pas ;
int g_i_max_nb_micropas ;

sem_t           gpio_sem  ;
struct timespec gpio_tm_now;
struct timespec gpio_tm_nxt;

int        g_incrlog ;
int        g_id_thread ;  

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

void   GPIO_CLIGNOTE         (int , int , int ) ;
void   GPIO_READ         (char [DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) ;
void   GPIO_READ2        (char [DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) ;
void   GPIO_RAQUETTE_CONFIG  (int [4],int [4]) ;
void   GPIO_RAQUETTE_READ    (int [4],int [4],CLAVIER* ) ;

void   GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS(int [4],int [4], SUIVI *) ;
void   GPIO_TRAP(int ) ;
void   GPIO_STATUT(void) ;
int    GPIO_GET(int ) ;
int    GPIO_SET(int ,int ) ;
int    GPIO_OPEN_BROCHE(int ,int ) ;
void   /* GPIO_OPEN_BROCHE */_PWM(GPIO_PWM_PHASE *) ;
int    GPIO_OPEN(int gpio_in[GPIO_SIZE],int [GPIO_SIZE]) ;
int    GPIO_CLOSE_BROCHE(int gpio) ;
int    GPIO_CLOSE(int gpio_in[GPIO_SIZE],int gpio_out[GPIO_SIZE]) ;
void   GPIO_SET_ALT(int dir, int slp, int clk, int rst, int mmm, int ena) ;
void   GPIO_SET_AZI(int dir, int slp, int clk, int rst, int mmm, int ena) ;
void   GPIO_MOVE_1(int sens, double periode,double nb_pulse, int gpio_dir, int gpio_clk) ;
void   GPIO_MOVE_2(int sens, double periode,unsigned  long nb_pulse) ;
void   GPIO_MOVE_3(int sens, double periode,double nb_pulse, int gpio_dir, int gpio_clk) ;
long   GPIO_ACCELERATION_1(int gpio, double f_deb,double f_fin, double delai,long nano_moins) ;
long   GPIO_ACCELERATION_2(int alt, int azi, double f_deb,double f_fin, double delai,long nano_moins) ;

// PWM
void   GPIO_INIT_PWM_MOTEUR(GPIO_PWM_MOTEUR *pm, int gpios[ GPIO_NB_PHASES_PAR_MOTEUR ], int masque[ GPIO_NB_PHASES_PAR_MOTEUR ],  double upas, double fm, double fpwm, int id, int type_fonction, double param0, double param1) ;
void   GPIO_CALCUL_PWM_RAPPORTS_CYCLIQUES(GPIO_PWM_MOTEUR *pm) ;

// Fonction de threads

void * GPIO_SUIVI_PWM_PHASE(GPIO_PWM_PHASE *ph ) ;
void * suivi_main_M(GPIO_PWM_MOTEUR *pm) ;

#endif
