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
* @param  : LIEU *gp_Lie
* @date   : 2022-06-01 creation
* @date   : 2022-09-30 ajout argument pthread_t pth_self 
*****************************************************************************************/

void PTHREADS_INIT( pthread_t pth_self ) {

  struct sched_param param; 
  char c_rlogin[ 16] = {0} ;

  pthread_mutex_init( & gp_Pth->pth_mut.mut_alt, NULL ) ;
  pthread_mutex_init( & gp_Pth->pth_mut.mut_azi, NULL ) ;
  pthread_mutex_init( & gp_Pth->pth_mut.mut_cal, NULL ) ;
  pthread_mutex_init( & gp_Pth->pth_mut.mut_dat , NULL ) ;
  pthread_mutex_init( & gp_Pth->pth_mut.mut_pth , NULL ) ;

  getlogin_r( c_rlogin, strlen(c_rlogin));

  Trace("user is : %s (getuid) %d ", c_rlogin, getuid()) ;
  Trace("pthread_self = %ld", pthread_self()) ;

  guit_user_getuid = getuid() ;

  // -----------------------------------------------------------------
  // Mise en place du temps reel et du parallelisme (parallelisme, priorites, ..)
  // -----------------------------------------------------------------
  
  Trace("Mise en place temps reel et parallelisme ") ;
  /* La commande suivante avec sudo et tee provoque un souvi sur la cible */
  /* visible avec strace -x  */
  /*
  system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ; 
  */
  if ( guit_user_getuid == USERID_ROOT ) {

    system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ; 
    mlockall(MCL_CURRENT | MCL_FUTURE);
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
* @param  : LIEU *gp_Lie
* @date   : 2022-06-01 creation
*****************************************************************************************/

void PTHREADS_CONFIG( PTHREADS* p_pth, pthread_t pth_self, t_en_Pthreads_Sched_Param l_en_thread ) {

  int   i_error=0 ;
  int   i_pri=0, i_ord=0 ;
  char  c_name[64] = {0} ; 

  pthread_mutex_lock( & p_pth->pth_mut.mut_pth) ;

  i_ord = gi_Pth_Sched_Param[ (int) l_en_thread ] ;
  i_pri = gi_Pth_Sched_Priority[ (int) l_en_thread ] ;
  memset( c_name, 0, sizeof(c_name)) ;
  strcpy( c_name, gi_Pth_Name[(int)l_en_thread]) ;

  Trace("nom %-64s policy %d ordonn %d ", c_name, i_pri, i_ord) ;

  pthread_mutex_unlock( & p_pth->pth_mut.mut_pth) ;

  if ( guit_user_getuid == USERID_ROOT ) {

    pthread_mutex_lock( & p_pth->pth_mut.mut_pth) ;
    p_pth->pth_att[ g_nb_threads ].att_pri.sched_priority = i_pri ;
    pthread_mutex_unlock( & p_pth->pth_mut.mut_pth) ;

    pthread_mutex_lock( & p_pth->pth_mut.mut_pth) ;

    if (pthread_setschedparam( \
        pth_self, \
        i_ord, 
      & p_pth->pth_att[ g_nb_threads ].att_pri ) != 0) { 

      i_error=1;        
      perror("setschedparam SUIVI_MENU");
      pthread_mutex_unlock( & p_pth->pth_mut.mut_pth) ;
      exit(EXIT_FAILURE); 
    }
    pthread_mutex_unlock( & p_pth->pth_mut.mut_pth) ;
  }
  else { 
    Trace("guit_user_getuid not USERID_ROOT => cannot pthread_setschedparam(args)") ;
  }

  pthread_mutex_lock( & p_pth->pth_mut.mut_pth) ;

  if ( pthread_setname_np( pth_self, c_name ) == 0 ) {

    p_pth->pth_att[ g_nb_threads ].att_pid= pth_self ;
  }
  else {
    i_error=1 ;
  }

  pthread_mutex_unlock( & p_pth->pth_mut.mut_pth) ;

  if ( i_error == 0 ) {
    pthread_mutex_lock( & p_pth->pth_mut.mut_pth) ;
    g_nb_threads++ ;
    pthread_mutex_unlock( & p_pth->pth_mut.mut_pth) ;
  }
  return ;
}


  /*
  if ( guit_user_getuid == USERID_ROOT ) {

    param.sched_priority = PTHREAD_POLICY_1 ;

    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_VOUTE, & param) != 0) {
      perror("setschedparam SUIVI_VOUTE"); 
      exit(EXIT_FAILURE);
    }
  }
  else { 
    Trace("guit_user_getuid not USERID_ROOT => cannot pthread_setschedparam(args)") ;
  }

  pthread_mutex_lock( & gp_Sui->p_pth->pth_mut.mut_pth) ;
  pthread_setname_np( pthread_self(), "SUIVI_VOUTE" ) ;
  gp_Sui->p_pthpth_att->att_pid[ g_nb_threads++ ] = pthread_self() ;
  pthread_mutex_unlock( & gp_Sui->p_pth->pth_mut.mut_pth) ;
  */