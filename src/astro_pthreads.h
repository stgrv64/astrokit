#define _GNU_SOURCE

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-06-01  | * creation
# 2022-06-01  | * rapatriement code depuis astro_config.h
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_PTHREADS_H
#define ASTRO_PTHREADS_H

#include "astro_global.h"

#define PTHREADS_MAX_THREADS       20
#define PTHREADS_ATTR_STACKSIZE    65536
#define PTHREADS_BUFFER_MAX_LENGTH 16

typedef enum {
  PTHREADS_USERID_ROOT=0,
  PTHREADS_USERID_NO_ROOT=1000
} 
t_en_Pthreads_User_Ids ;

typedef enum {
  PTHREADS_POLICY_0=0,
  PTHREADS_POLICY_1,
  PTHREADS_POLICY_2,
  PTHREADS_POLICY_5,
  PTHREADS_POLICY_10,
  PTHREADS_POLICY_50,
  PTHREADS_POLICY_99
}
t_en_Pthreads_Sched_Priority ;

typedef enum {
  PTHREADS_SUIVI_PWM_PHA = 0,
  PTHREADS_SUIVI_PWM_MOT   ,
  PTHREADS_SUIVI_MENU       ,
  PTHREADS_SUIVI_VOUTE      ,
  PTHREADS_SUIVI_INFRA      ,
  PTHREADS_SUIVI_LCD        ,
  PTHREADS_SUIVI_CLAVIER    ,
  PTHREADS_SUIVI_CAPTEUR    ,
  PTHREADS_SUIVI_MAIN
} 
t_en_Pthreads_Sched_Param ;

typedef enum {
  PTHREADS_USLEEP_BEFORE_START_SUIVI_PWM_PHASES = 250000,
  PTHREADS_USLEEP_BEFORE_START_SUIVI_PWM_MAIN   = 500000,
  PTHREADS_USLEEP_BEFORE_START_SUIVI_MENU       = 750000,
  PTHREADS_USLEEP_BEFORE_START_SUIVI_VOUTE      = 1000000,
  PTHREADS_USLEEP_BEFORE_START_SUIVI_INFRA      = 1250000,
  PTHREADS_USLEEP_BEFORE_START_SUIVI_LCD        = 1500000,
  PTHREADS_USLEEP_BEFORE_START_SUIVI_CLAVIER    = 1750000,
  PTHREADS_USLEEP_BEFORE_START_SUIVI_CAPTEUR    = 2000000,
  PTHREADS_USLEEP_BEFORE_START_SUIVI_MAIN       = 2250000
}
t_en_Pthreads_Usleep_Before_Start ;

typedef enum {
  PTHREADS_SCHED_PARAM_SUIVI_PWM_PHASES = SCHED_RR,
  PTHREADS_SCHED_PARAM_SUIVI_PWM_MAIN   = SCHED_RR,
  PTHREADS_SCHED_PARAM_SUIVI_MENU       = SCHED_FIFO,
  PTHREADS_SCHED_PARAM_SUIVI_VOUTE      = SCHED_FIFO,
  PTHREADS_SCHED_PARAM_SUIVI_INFRA      = SCHED_FIFO,
  PTHREADS_SCHED_PARAM_SUIVI_LCD        = SCHED_FIFO,
  PTHREADS_SCHED_PARAM_SUIVI_CLAVIER    = SCHED_FIFO,
  PTHREADS_SCHED_PARAM_SUIVI_CAPTEUR    = SCHED_FIFO,
  PTHREADS_SCHED_PARAM_SUIVI_MAIN       = SCHED_FIFO
}
t_en_Pthreads_Sched_Param_OLD ;

struct STR_PTHREADS_ATTRIBUTS {
  
  pthread_t          att_pid  ;
  int                att_ord ;
  struct sched_param att_pri ; 
  int                att_bef ;
  char               att_nam[ 16 ] ;
} ;

typedef struct STR_PTHREADS_ATTRIBUTS STRUCT_PTHREADS_ATTRIBUTS ;

/* All concerning STRUCT_PTHREADS */ 

struct STR_MUTEXS {
  pthread_mutex_t  mut_dat ;
  pthread_mutex_t  mut_alt ;
  pthread_mutex_t  mut_azi ;
  pthread_mutex_t  mut_cal ;
  pthread_mutex_t  mut_pth ;
} ;

typedef struct STR_MUTEXS STRUCT_MUTEXS ;

struct STR_PTHREADS {

  // Pthreads utilises comme pointeurs dans la structure passee en argument des fonctions de thread

  pthread_t     pth_inf ;
  pthread_t     pth_cap ;
  pthread_t     pth_vou ;
  pthread_t     pth_cla ;
  pthread_t     pth_ter ;
  pthread_t     pth_lcd ;
  pthread_t     pth_alt ;
  pthread_t     pth_azi ;
  pthread_t     pth_men ;
  pthread_t     pth_tst ;
  
  STRUCT_PTHREADS_ATTRIBUTS pth_att[ PTHREADS_MAX_THREADS ]  ; 
} ;

typedef struct STR_PTHREADS STRUCT_PTHREADS ;

void PTHREADS_INIT          ( pthread_t ) ;
void PTHREADS_CONFIG        ( STRUCT_PTHREADS * , pthread_t, t_en_Pthreads_Sched_Param  ) ;
void PTHREADS_INFOS         ( STRUCT_PTHREADS * ) ;
void PTHREADS_AFFICHER_ETAT ( STRUCT_PTHREADS *) ;

#endif