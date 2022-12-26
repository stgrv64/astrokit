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
 PTHREAD_CONFIGURED=0,
 PTHREAD_RUNNING=0,
 PTHREAD_CANCELLED,
 PTHREAD_JOINED,
 PTHREAD_KILLED
}
t_en_Pthreads_State ;

typedef enum {
 PTHREAD_MOTOR_0=0,
 PTHREAD_MOTOR_1
} 
t_en_Pthreads_Motors ;

typedef enum {
 PTHREAD_PHASE_0=0,
 PTHREAD_PHASE_1,
 PTHREAD_PHASE_2,
 PTHREAD_PHASE_3,
} 
t_en_Pthreads_Phases ;

typedef enum {
  PTHREAD_POLICY_0=0,
  PTHREAD_POLICY_1,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_5,
  PTHREAD_POLICY_10,
  PTHREAD_POLICY_50,
  PTHREAD_POLICY_99
}
t_en_Pthreads_Sched_Priority ;

typedef enum {
  PTHREAD_TYPE_PWM_PHASE = 0,
  PTHREAD_TYPE_PWM_MOTOR   ,
  PTHREAD_TYPE_MENU       ,
  PTHREAD_TYPE_VOUTE      ,
  PTHREAD_TYPE_INFRARED      ,
  PTHREAD_TYPE_LCD        ,
  PTHREAD_TYPE_CLAVIER    ,
  PTHREAD_TYPE_CAPTEURS    ,
  PTHREAD_TYPE_MAIN
} 
t_en_Pthreads_Type ;

typedef enum {
  PTHREAD_T_MOT_ALT_PHASE_0 = 0,
  PTHREAD_T_MOT_ALT_PHASE_1 ,
  PTHREAD_T_MOT_ALT_PHASE_2 ,
  PTHREAD_T_MOT_ALT_PHASE_3 ,
  PTHREAD_T_MOT_AZI_PHASE_0 ,
  PTHREAD_T_MOT_AZI_PHASE_1 ,
  PTHREAD_T_MOT_AZI_PHASE_2 ,
  PTHREAD_T_MOT_AZI_PHASE_3 ,              
  PTHREAD_T_MOT_ALT    ,
  PTHREAD_T_MOT_AZI    ,
  PTHREAD_T_MENU       ,
  PTHREAD_T_VOUTE      ,
  PTHREAD_T_INFRA      ,
  PTHREAD_T_LCD        ,
  PTHREAD_T_CLAVIER    ,
  PTHREAD_T_CAPTEUR    ,
  PTHREAD_T_MAIN
} 
t_en_Pthreads_T ;

typedef enum {
  PTHREAD_USLEEP_BEFORE_START_SUIVI_PWM_PHASES = 250000,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_PWM_MAIN   = 500000,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_MENU       = 750000,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_VOUTE      = 1000000,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_INFRA      = 1250000,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_LCD        = 1500000,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_CLAVIER    = 1750000,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_CAPTEUR    = 2000000,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_MAIN       = 2250000
}
t_en_Pthreads_Usleep_Before_Start ;

typedef enum {
  PTHREAD_SCHED_PARAM_SUIVI_PWM_PHASES = SCHED_RR,
  PTHREAD_SCHED_PARAM_SUIVI_PWM_MAIN   = SCHED_RR,
  PTHREAD_SCHED_PARAM_SUIVI_MENU       = SCHED_FIFO,
  PTHREAD_SCHED_PARAM_SUIVI_VOUTE      = SCHED_FIFO,
  PTHREAD_SCHED_PARAM_SUIVI_INFRA      = SCHED_FIFO,
  PTHREAD_SCHED_PARAM_SUIVI_LCD        = SCHED_FIFO,
  PTHREAD_SCHED_PARAM_SUIVI_CLAVIER    = SCHED_FIFO,
  PTHREAD_SCHED_PARAM_SUIVI_CAPTEUR    = SCHED_FIFO,
  PTHREAD_SCHED_PARAM_SUIVI_MAIN       = SCHED_FIFO
}
t_en_Pthreads_Sched_Param_OLD ;

struct STR_PTHREADS_ATTRIBUTS {
  
  int                att_sta ;
  pthread_t          att_pid  ;
  int                att_ord ;
  int                att_nbc ;
  struct sched_param att_pri ; 
  int                att_bef ;
  char               att_c_nam[ CONFIG_TAILLE_BUFFER_32 ] ;
  char               att_c_ord[ CONFIG_TAILLE_BUFFER_32 ] ;
  char               att_c_sta[ CONFIG_TAILLE_BUFFER_32 ] ;
} ;

typedef struct STR_PTHREADS_ATTRIBUTS STRUCT_PTHREADS_ATTRIBUTS ;

/* All concerning STRUCT_PTHREADS */ 

struct STR_MUTEXS {
  pthread_mutex_t  mut_mutex ; /* protege les membres de la structure */
  
  pthread_mutex_t  mut_dat ;
  pthread_mutex_t  mut_glo_alt ;
  pthread_mutex_t  mut_glo_azi ;
  pthread_mutex_t  mut_cal ;
  pthread_mutex_t  mut_pth ;
} ;
typedef struct STR_MUTEXS STRUCT_MUTEXS ;

struct STR_PTHREADS {
  STRUCT_PTHREADS_ATTRIBUTS pth_att [ PTHREADS_MAX_THREADS ]  ; 
  pthread_mutex_t           pth_mutex ;
  void                    (*pth_lock)   (void) ;
  void                    (*pth_unlock) (void) ;  
  pthread_t                 pth_t   [ PTHREADS_MAX_THREADS ]  ;  
  unsigned int              pth_i_nb ; 
} ;
typedef struct STR_PTHREADS STRUCT_PTHREADS ;

void PTHREADS_INIT_MUTEXS     (void) ;
void PTHREADS_INIT            ( STRUCT_PTHREADS * , pthread_t ) ;
void PTHREADS_CONFIG          ( STRUCT_PTHREADS * , pthread_t, int  ) ;
void PTHREADS_INFOS           ( STRUCT_PTHREADS * ) ;
void PTHREADS_DISPLAY_ETAT   ( STRUCT_PTHREADS * ) ;
void PTHREADS_CANCEL_OR_KILL  ( STRUCT_PTHREADS * ) ;

#endif