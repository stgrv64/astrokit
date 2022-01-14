/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 01/05/2021  | ajout entete
# 01/05/2021  | mise en commentaire #define GPIO_FREQUENCE_PWM 
#   suite a ajout de la variable du meme nom dans types.h
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

#define GPIO_CURRENT_FONCTION          3       // 0 : triangle - 1 : sinus - 2 : mixte - 3  : sinus arrondi (best)
#define GPIO_CURRENT_FONCTION_PARAM0   0.65    // ce parametre deforme la fonction 2 et 3
#define GPIO_CURRENT_FONCTION_PARAM1   1.0     // ce parametre multiplie la fonction pour l'ajuster a 1 (5v)

#define GPIO_SUIVI_PWM_PHASE_PRIORITY  5
#define GPIO_SUIVI_MAIN_PRIORITY       2
#define GPIO_SUIVI_MAIN_ATTENTE_MAX    0.3     // attente maximum exprimee en seconde pour non blocage

#define GPIO_SUIVI_PWM_PHASE_SCHED   SCHED_RR
#define GPIO_SUIVI_MAIN_SCHED        SCHED_FIFO

#define GPIO_MICROPAS_MAX           500
// FIXME : mise en commentaire de la ligne suivante (2021)
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

typedef struct {
  
  pthread_mutex_t mutex ; 

  int     id ;  
  double  Tpwm ;  
  int     pas ;

  double  rap[ GPIO_MICROPAS_MAX ] ;
  double  rc ;

  int     gpio ;
  int     gpio_open_statut ;
  int     gpio_fd ;
  
  double  deltat_m ;

  struct timeval tval ;
}
GPIO_PWM_PHASE ;

//==========================================================================

typedef struct {
  
  pthread_mutex_t mutex ; 
  
  SUIVI          * suivi ;
  GPIO_PWM_PHASE * phase[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  
  int     id ;
  int     pas ;
  double  deltat ;
  double  t ;

  double  Fm ;  
  double  Tm ;  
  double  Fpwm ;  
  double  Tpwm ; 

  double  upas ; 
  long    nbdeltat ;
  int     type_fonction ; 

  double  param0 ;
  double  param1 ;

  // FIXME : la difference entre temps et  temps_reel : voir fonction *suivi_main_M
  //         *  temps      : on utilise deltat , qui sert pour le usleep 
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

//==========================================================================

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

GPIO_PWM_MOTEUR *pm_alt , m_alt ; 
GPIO_PWM_MOTEUR *pm_azi , m_azi ;

int priority ;

//==========================================================================
// broches pour l'injection directe d'une frequence depuis 2 ports GPIO dans le cas 
// ou la carte electronique de division par etage de la frequence de ref (32768 Hz)
// n'est pas utilise ni le diviseur puissance de deux ni le diviseur 4059
// ( division 2^N puis par Division decimal _ utilisation de D dans param _ )
// On definit egalement les autres broches utiles a savoir :
// - sens = sens de rotation du moteur, en cas d'erreur lors du montage, on utilise
//          le flag ERR_SENS par defaut egal a 1
//==========================================================================


void   GPIO_CLIGNOTE(int gpio, int nombre_clignotement, int duree_clignotement) ;
void   GPIO_INIT_VAR(char datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) ;
void   GPIO_INIT_VAR2(char datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) ;
void   GPIO_KEYBOARD_CONFIG (int GPIO_KEY_L[4],int GPIO_KEY_C[4]) ;
void   GPIO_KEYBOARD_READ (int GPIO_KEY_L[4],int GPIO_KEY_C[4], char KEYBOARD[4][4][GPIO_TAILLE_BUFFER], CLAVIER* clavier) ;
void   GPIO_KEYBOARD_RAQUETTE_READ(int GPIO_KEY_L[4],int GPIO_KEY_C[4], char raquette[4][4][GPIO_TAILLE_BUFFER], SUIVI *suivi) ;
void   GPIO_TRAP(int sig) ;
void   GPIO_STATUT(void) ;
int    GPIO_GET(int gpio) ;
int    GPIO_SET(int gpio,int val) ;
int    GPIO_OPEN_BROCHE(int gpio,int output) ;
void   GPIO_OPEN_BROCHE_PWM(GPIO_PWM_PHASE *gpwm) ;
int    GPIO_OPEN(int gpio_in[GPIO_SIZE],int gpio_out[GPIO_SIZE]) ;
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
void   GPIO_INIT_PWM_MOTEUR(GPIO_PWM_MOTEUR *pm, int gpio, double upas, double fm, double fpwm, int id)  ;
void   GPIO_INIT_PWM_MOTEUR_2(GPIO_PWM_MOTEUR *pm, int gpios[ GPIO_NB_PHASES_PAR_MOTEUR ], int masque[ GPIO_NB_PHASES_PAR_MOTEUR ],  double upas, double fm, double fpwm, int id, int type_fonction, double param0, double param1) ;
void   GPIO_CALCUL_PWM_RAPPORTS_CYCLIQUES(GPIO_PWM_MOTEUR *pm) ;
void   GPIO_INIT_PWM_MOTEUR(GPIO_PWM_MOTEUR *pm0, int gpio, double upas, double fm, double fpwm,int id) ;

// Fonction de threads

void * GPIO_SUIVI_PWM_PHASE(GPIO_PWM_PHASE *ph ) ;
void * suivi_main_M(GPIO_PWM_MOTEUR *pm) ;

#endif
