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

typedef struct STR_PID STRUCT_PID ;

struct STR_PID {
  pthread_mutex_t  pid_mutex ;
  STR_EXT_TIMEVAL  pid_tval ; 
  FILE            *pid_file ; 
  void           (*pid_log)     ( STRUCT_PID * ) ;
  void           (*pid_display) ( STRUCT_PID * ) ;
  void           (*pid_lock)    ( STRUCT_PID * ) ;
  void           (*pid_unlock)  ( STRUCT_PID * ) ;  
  void           (*pid_run)     ( STRUCT_PID *, double,double) ; 
  void           (*pid_reset)   ( STRUCT_PID * ) ; 
  void           (*pid_test)    ( STRUCT_PID * ) ;
  int              pid_loglevel ;
  char             pid_dis_for [ CONFIG_TAILLE_BUFFER_256 ] ;
  char             pid_dis_cmd [ CONFIG_TAILLE_BUFFER_256 ] ;

  double           pid_acc_azi ;  // acceleration deduite en azimut   pour asservissement
  double           pid_acc_alt ;  // acceleration deduite en altitude pour asservissement

  /* Entree / sortie de l algorithme : input = consigne */ 

  unsigned long    pid_long_incr ;      /* numero d increment du calcul */ 
  double           pid_input ; /* consigne */ 
  double           pid_output ;         /* sortie */ 
  double           pid_ech ;            /* echantillonage */
  double           pid_err ;            /* erreur  = out - inp */
  double           pid_err_sum ;        /* calcul de la somme des erreurs */ 
  double           pid_err_kp ;         /* calcul de la partie proportionnelle */  
  double           pid_err_ki ;         /* calcul de la partie integrale */  
  double           pid_err_kd ;         /* calcul de la partie derivee */

  /* resultat du calcul complet =
      Kp * (terme proportionnel ) + Ki * (terme integral) + Kd * (terme derivee) 
  */
  double          pid_result ;          /* resultat du calcul complet */
  double          pid_err_pre ;         /* sauvegarde de erreur precedente */ 
  /* Parametres de regalges du PID proportionnel integral derive*/

  double          pid_kp ;
  double          pid_ki ;
  double          pid_kd ;

  /*------------------------------------------------*/
  /* Pointeurs de fonctions associes a la structure */
  /*------------------------------------------------*/
  

} ;

typedef struct STR_PID_PARAMS STRUCT_PID_PARAMS ;

       void PID_PARAMS_INIT      ( STRUCT_PID_PARAMS * ) ;
       void PID_PARAMS_DISPLAY   ( STRUCT_PID_PARAMS * ) ;
       void PID_INIT             ( STRUCT_PID * ) ;

static void PID_RUN              ( STRUCT_PID *, double, double ) ;
static void PID_RESET            ( STRUCT_PID * ) ;
static void PID_TEST             ( STRUCT_PID * ) ;
static void PID_ACTIVATE         ( STRUCT_PID * ) ;
static int  PID_IS_IN_CONDITION  ( STRUCT_PID * ) ;

#endif