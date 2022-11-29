/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-06-01  | * creation
# 2022-06-01  | * rapatriement code depuis astro_config.h
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_PID_H
#define ASTRO_PID_H

#include "astro_global.h"

#define PID_ERR_PRE_INIT 56.245856

/*--------------------------------------------*/
/* Parametres du regulateur PID en frequences */
/*--------------------------------------------*/

struct STR_PID_PARAMS {
  pthread_mutex_t pid_par_mutex ;
  double par_pid_ech ; /* echantillonage en nombre de pas moteurs */
  double par_pid_kp ;
  double par_pid_ki ;
  double par_pid_kd ;
} ;

/*--------------------------------------------*/
/* Parametres du regulateur PID en frequences */
/*--------------------------------------------*/

struct STR_PID {

  FILE          * pid_f_out ; 
  pthread_mutex_t pid_mutex ;

  double          pid_acc_azi ;  // acceleration deduite en azimut   pour asservissement
  double          pid_acc_alt ;  // acceleration deduite en altitude pour asservissement

  /* Entree / sortie de l algorithme : input = consigne */ 

  unsigned long incr ; /* numero d increment du calcul */ 

  double inp ;     /* consigne */ 
  double out ;     /* sortie */ 
  double ech ;     /* echantillonage */
  double err ;     /* erreur  = out - inp */

  double err_sum ; /* calcul de la somme des erreurs */ 

  double err_pro_Kp ; /* calcul de la partie proportionnelle */  
  double err_int_Ki ; /* calcul de la partie integrale */  
  double err_der_Kd ; /* calcul de la partie derivee */

  /* resultat du calcul complet =
      Kp * (terme proportionnel ) + Ki * (terme integral) + Kd * (terme derivee) 
  */
  double pid ; /* resultat du calcul complet */
  double err_pre ; /* sauvegarde de erreur precedente */ 
  /* Parametres de regalges du PID proportionnel integral derive*/

  double Kp ;
  double Ki ;
  double Kd ;

  /*------------------------------------------------*/
  /* Pointeurs de fonctions associes a la structure */
  /*------------------------------------------------*/
  
  void (* pid_init)(void) ; 
  void (* pid_reset)(void) ; 
  void (* pid_run)(double,double) ; 
  /* void (* pid_calcul)(void) ;  */
  void (* pid_test)(void) ;
} ;

typedef struct STR_PID STRUCT_PID ;
typedef struct STR_PID_PARAMS STRUCT_PID_PARAMS ;

void   PID_INIT             (STRUCT_PID *, double, double, double, double) ;

void   PID_RESET            ( void)  ;
void   PID_TEST             ( void)  ;
void   PID_ACTIVATE         ( void) ;
int    PID_IS_IN_CONDITION  ( void) ;
void   PID_CALCULATE        ( double, double ) ;

#endif