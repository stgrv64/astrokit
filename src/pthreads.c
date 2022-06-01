#define _GNU_SOURCE

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# juin 2022   * creation
# juin 2022   * deport du code concernant temps teel et threads ici
# -------------------------------------------------------------- 
*/

#include <pthreads.h>

/*****************************************************************************************
* @fn     : PTHREADS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise les threads et le temps reel
* @param  : LIEU *gp_Lieu
* @date   : 2022-06-01 creation
*****************************************************************************************/

void PTHREADS_INIT( void ) {

  struct sched_param param; 
  char c_rlogin[ 16] = {0} ;

  pthread_mutex_init( & gp_Pthr->mutex_alt, NULL ) ;
  pthread_mutex_init( & gp_Pthr->mutex_azi, NULL ) ;
  pthread_mutex_init( & gp_Pthr->mutex_cal, NULL ) ;
  pthread_mutex_init( & gp_Pthr->mutex_infrarouge , NULL ) ;
  pthread_mutex_init( & gp_Pthr->mutex_pthread , NULL ) ;

  getlogin_r( c_rlogin, strlen(c_rlogin));
  Trace("user is : %s (getuid) %d ", c_rlogin, getuid()) ;

  gc_user_id = getuid() ;

  // -----------------------------------------------------------------
  // Mise en place du temps reel et du parallelisme (parallelisme, priorites, ..)
  // -----------------------------------------------------------------
  
  Trace("Mise en place temps reel et parallelisme ") ;
  /* La commande suivante avec sudo et tee provoque un souvi sur la cible */
  /* visible avec strace -x  */
  /*
  system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ; 
  */
  if ( gc_user_id == USERID_ROOT ) {

    system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ; 
    mlockall(MCL_CURRENT | MCL_FUTURE);
    
    param.sched_priority = PTHREAD_POLICY_1 ;

    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_MAIN, & param) != 0) {
        perror("setschedparam main");
        exit(EXIT_FAILURE);
    }
  }
  else {
    system("sudo echo -1 | sudo tee -a /proc/sys/kernel/sched_rt_runtime_us") ; 
    mlockall(MCL_CURRENT | MCL_FUTURE);
    /* Code a executer avec compte rroot uniqumeent */ 
    /*
    param.sched_priority = PTHREAD_POLICY_1 ;

    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_MAIN, & param) != 0) {
        perror("setschedparam main");
        exit(EXIT_FAILURE);
    }
    */
  }
}

/*****************************************************************************************
* @fn     : PTHREADS_CONFIG
* @author : s.gravois
* @brief  : Cette fonction configure un thread, est appelle par un pt de fct
* @param  : LIEU *gp_Lieu
* @date   : 2022-06-01 creation
*****************************************************************************************/

void PTHREADS_CONFIG( void ) {

  return ;
}