#ifndef GPIO_H
#define GPIO_H

#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>


#include <astro.h>
#include <config.h>

#define GPIO_FREQ_MAX      1000.0
#define GPIO_VARG_STRING  115
#define GPIO_VARG_INT 100
#define GPIO_VARG_CHAR 99
#define GPIO_MICRO_SEC    1000000.0             // frequence d'une microseconde (pour les usleep et calculs)
#define GPIO_NANO_SEC     1000000000          // frequence d'une nanoseconde (pour les nanosleep et calculs) 
#define GPIO_TIMEOUT      30 
#define GPIO_SIZE         16
#define GPIO_CONTROLEUR_SIZE         4
#define GPIO_FREQ_IR      38000.0
#define GPIO_TEMPORISATION_PERCENT 0.5

#define BUFFER       256
#define GPIO_PATH    "/sys/class/gpio" 
#define GPIO_TAILLE_BUFFER 16

static pthread_mutex_t  mutex_phase_a = PTHREAD_MUTEX_INITIALIZER ;
static pthread_mutex_t  mutex_phase_b = PTHREAD_MUTEX_INITIALIZER ;
  
typedef struct {
 
  unsigned long  incrA ;
  unsigned long  incrB ;
  int phase ;
  int phaseA ;
  int phaseB ;
  int gpioA ;
  int gpioB ;
  int gpioApassif;
  int gpioBpassif ;
  double t ;
  int       pas ;
  double *rap[4] ; 
  double rapp[5][4][128] ;
  
  double deltat ;
 
  unsigned long nbdeltat ;
  
  double ra1 ;  // rapport cyclique phase A1
  double ra2 ;  // rapport cyclique phase A2
  double rb1 ;  // rapport cyclique phase B1
  double rb2 ;  // rapport cyclique phase B2
  
  double Fm ;  // frequence moteur
  double Tm ;  // periode moteur
  double Fpwm ;  // frequence PWM
  double Tpwm ; // periode PWM
  double upas ; // mode micro pas (exemple upas = 64)
  int gpioa1 ;
  int gpioa2 ;
  int gpiob1 ;
  int gpiob2 ;
} 
GPIO_PWM_PHASES_MOTEUR ;

int GPIO_OPEN_STATUT ;

int gpio_fd 	[ GPIO_SIZE ] ;
int gpio_in	[ GPIO_SIZE]  ;
int gpio_out[GPIO_SIZE] ; 
 
int gpio_alt  [ GPIO_CONTROLEUR_SIZE ]  ; // phases du moteur (utilse si CONTROLEUR vaut 0 dans config.txt)
int gpio_azi [ GPIO_CONTROLEUR_SIZE ] ;  // phases du moteur (utilse si CONTROLEUR vaut 0 dans config.txt)

//==========================================================================
// broches pour l'injection directe d'une frequence depuis 2 ports GPIO dans le cas 
// ou la carte electronique de division par etage de la frequence de ref (32768 Hz)
// n'est pas utilise ni le diviseur puissance de deux ni le diviseur 4059
// ( division 2^N puis par Division decimal _ utilisation de D dans param _ )
// On definit egalement les autres broches utiles a savoir :
// - sens = sens de rotation du moteur, en cas d'erreur lors du montage, on utilise
//          le flag ERR_SENS par defaut egal a 1
//==========================================================================

void   GPIO_RAQUETTE_READ (int GPIO_KEY_L[4],int GPIO_KEY_C[4], char KEYBOARD[4][4][GPIO_TAILLE_BUFFER], CLAVIER* clavier)  ;
void   GPIO_RAQUETTE_CONFIG (int GPIO_KEY_L[4],int GPIO_KEY_C[4]) ;
void   GPIO_RAQUETTE_MAJ_SUIVI(int GPIO_KEY_L[4],int GPIO_KEY_C[4], char raquette[4][4][GPIO_TAILLE_BUFFER], SUIVI *suivi) ;

void   GPIO_READ(char datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) ;
void   GPIO_TRAP(int sig) ;
int    GPIO_GET(int gpio) ;
int    GPIO_SET(int gpio,int val)  ;
void   GPIO_STATUT(void) ;
int    GPIO_OPEN(int gpio_in[GPIO_SIZE],int gpio_out[GPIO_SIZE]) ;
int    GPIO_CLOSE(int gpio_in[GPIO_SIZE],int gpio_out[GPIO_SIZE]) ;
void   GPIO_SET_ALT(int dir, int slp, int clk, int rst, int mmm, int ena) ; 
void   GPIO_SET_AZI(int dir, int slp, int clk, int rst, int mmm, int ena) ;

void   GPIO_MOVE_1(int sens, double periode,double nb_pulse, int gpio_dir, int gpio_clk) ;
void   GPIO_MOVE_2(int sens, double periode,unsigned  long nb_pulse)  ;

long   GPIO_ACCELERATION_1(int gpio, double f_deb,double f_fin, double delai,long nano_moins) ;
long   GPIO_ACCELERATION_2(int alt, int azi, double f_deb,double f_fin, double delai,long nano_moins) ;

void   Main(int argc, char **argv) ;

#endif
