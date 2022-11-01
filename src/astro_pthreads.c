#define _GNU_SOURCE
/* pour utilisation de pthread_getname_np / pthread_setname_np */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# juin 2022   * creation
# juin 2022   * deport du code concernant temps teel et threads ici
# -------------------------------------------------------------- 
*/

#include "astro_pthreads.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

int gi_pthread_nb_threads ;  
int gi_pthread_getuid ;
int gi_pthread_nb_cpu ;

static const char *  gi_Pth_Name[] = {
  "pth_phases" ,
  "pth_moteurs"   ,
  "pth_menu"       ,
  "pth_voute"      ,
  "pth_inf"      ,
  "pth_lcd"        ,
  "pth_clavier"    ,
  "pth_capteurs"    ,
  "pth_main"
} ;

static const int gi_Pth_Sched_Priority[] = {
  PTHREADS_POLICY_5,
  PTHREADS_POLICY_5,
  PTHREADS_POLICY_2,
  PTHREADS_POLICY_2,
  PTHREADS_POLICY_2,
  PTHREADS_POLICY_2,
  PTHREADS_POLICY_2,
  PTHREADS_POLICY_2,
  PTHREADS_POLICY_1
} ;

static const int gi_Pth_Sched_Param[] = {
  SCHED_RR,
  SCHED_RR,
  SCHED_RR,
  SCHED_RR,
  SCHED_RR,
  SCHED_RR,
  SCHED_RR,
  SCHED_RR,
  SCHED_RR
} ;

/*****************************************************************************************
* @fn     : PTHREADS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise les threads et le temps reel
* @param  : STRUCT_LIEU *gp_Lie
* @date   : 2022-06-01 creation
* @date   : 2022-09-30 ajout argument pthread_t pth_self 
*****************************************************************************************/

void PTHREADS_INIT( pthread_t pth_self ) {

  int i_error=0 ;
  int i_errno=0 ;
  struct sched_param param; 
  char c_rlogin[ 16] = {0} ;

  if ( ( i_error = pthread_mutex_init( & gp_Mut->mut_alt, NULL )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE); 
  }
  if ( ( i_error = pthread_mutex_init( & gp_Mut->mut_azi, NULL )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE); 
  }
  if ( ( i_error = pthread_mutex_init( & gp_Mut->mut_cal, NULL )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE); 
  }
  if ( ( i_error = pthread_mutex_init( & gp_Mut->mut_dat , NULL )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE); 
  }
  if ( ( i_error = pthread_mutex_init( & gp_Mut->mut_pth , NULL )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE); 
  }

  getlogin_r( c_rlogin, strlen(c_rlogin));

  Trace("user is : %s (getuid) %d ", c_rlogin, getuid()) ;
  Trace("pthread_self = %ld", pthread_self()) ;

  gi_pthread_getuid = getuid() ;

  // -----------------------------------------------------------------
  // Mise en place du temps reel et du parallelisme (parallelisme, priorites, ..)
  // -----------------------------------------------------------------
  
  Trace("Mise en place temps reel et parallelisme ") ;
  /* La commande suivante avec sudo et tee provoque un souvi sur la cible */
  /* visible avec strace -x  */
  /*
  system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ; 
  */
  if ( gi_pthread_getuid == PTHREADS_USERID_ROOT ) {

    system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ; 
    mlockall(MCL_CURRENT | MCL_FUTURE);

    /* Code a executer avec compte rroot uniqumeent */ 
    
    param.sched_priority = PTHREADS_POLICY_1 ;

    if (pthread_setschedparam( pthread_self(), PTHREADS_SCHED_PARAM_SUIVI_MAIN, & param) != 0) {
        perror("setschedparam main");
        exit(EXIT_FAILURE);
    }
    // PTHREADS_CONFIG( gp_Pth, pth_self, PTHREADS_SUIVI_MAIN ) ;
    
  }
  else {
    perror("Execute with root privilege (because of threads -> sched_priority used)") ;
    exit(EXIT_FAILURE) ;
  }
}

/*****************************************************************************************
* @fn     : PTHREADS_CONFIG
* @author : s.gravois
* @brief  : Cette fonction configure un thread, est appelle par un pt de fct
* @param  : STRUCT_LIEU *gp_Lie
* @date   : 2022-06-01 creation
*****************************************************************************************/

void PTHREADS_CONFIG( STRUCT_PTHREADS* p_pth, pthread_t pth_self, t_en_Pthreads_Sched_Param l_en_thread ) {

  int   i_error=0 ;
  int   i_errno=0 ;
  int   i_pri=0, i_ord=0 ;
  char  c_name[PTHREADS_BUFFER_MAX_LENGTH] = {0} ; 
  struct sched_param param ;
  cpu_set_t cpu_set ;
  pthread_attr_t attr ;

  /*-------------------------------------------------*/
  /* Recuperation des valeurs d 'attributs de treads */
  /*-------------------------------------------------*/

  i_ord = gi_Pth_Sched_Param[ (int) l_en_thread ] ;
  i_pri = gi_Pth_Sched_Priority[ (int) l_en_thread ] ;
  memset( c_name, 0, sizeof(c_name)) ;
  strcpy( c_name, gi_Pth_Name[(int)l_en_thread]) ;
  param.sched_priority = i_pri ;

  /*----------------------------------------------------------*/
  /* Configuration et attribution des CPUs pour chaque thread */
  /*----------------------------------------------------------*/

  CPU_ZERO(&cpu_set) ;

  for(int i = 0 ; i< gi_pthread_nb_cpu; i++ ) {
    CPU_SET(i,&cpu_set); 
  }
  
  /*----------------------------------------------------------*/
  /* Initialisation et attribution des attributs de thread    */
  /*----------------------------------------------------------*/

  pthread_attr_init (&attr);

  if ( (i_error=pthread_attr_setstacksize   (&attr, PTHREADS_ATTR_STACKSIZE)) != 0 ) {
      i_errno=errno;        
      Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
      perror("pthread_attr_setstacksize");
      exit(EXIT_FAILURE); 
  }
  if ( (i_error=pthread_attr_setaffinity_np (&attr, sizeof(cpu_set_t), &cpu_set )) != 0 ) {
      i_errno=errno;        
      Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
      perror("pthread_attr_setaffinity_np");
      exit(EXIT_FAILURE); 
  }
  if ( (i_error=pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE ) ) != 0 ) {
      i_errno=errno;        
      Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
      perror("pthread_attr_setdetachstate");
      exit(EXIT_FAILURE); 
  }
  if ( (i_error=pthread_attr_setschedpolicy (&attr, SCHED_RR)) != 0 ) {
      i_errno=errno;        
      Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
      perror("pthread_attr_setschedpolicy");
      exit(EXIT_FAILURE); 
  }
  if ( (i_error=pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) != 0 ) {
      i_errno=errno;        
      Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
      perror("pthread_attr_setinheritsched");
      exit(EXIT_FAILURE); 
  }
  
  if ( (i_error=pthread_attr_setschedparam(&attr, &param)) != 0 ) {
      i_errno=errno;        
      Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
      perror("pthread_attr_setinheritsched");
      exit(EXIT_FAILURE); 
  }
  
  Trace("nom %-64s policy %d ordonn %d ", c_name, i_pri, i_ord) ;

  if ( gi_pthread_getuid == PTHREADS_USERID_ROOT ) {

    if ( (i_error= pthread_setschedparam( pth_self, i_ord, & param )) != 0) { 
      i_errno=errno;        
      Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
      perror("pthread_setschedparam");
      exit(EXIT_FAILURE); 
    } 
  }
  else { 
    Trace("gi_pthread_getuid not PTHREADS_USERID_ROOT => cannot pthread_setschedparam(args)") ;
  }
  /* erreur la plus courante de pthread_setname_np */
  /* ERANGE 34 Numerical result out of range */
  if ( (i_error=pthread_setname_np( pth_self, c_name )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_setname_np");
    exit(EXIT_FAILURE); 
  }

  if ( i_error == 0 ) {
    pthread_mutex_lock( & gp_Mut->mut_pth) ;
    strcpy( p_pth->pth_att[ gi_pthread_nb_threads ].att_nam , c_name ) ;
    p_pth->pth_att[ gi_pthread_nb_threads ].att_pid                = pth_self ;
    p_pth->pth_att[ gi_pthread_nb_threads ].att_pri.sched_priority = i_pri ;
    p_pth->pth_att[ gi_pthread_nb_threads ].att_ord                = i_ord ; 
    
    gi_pthread_nb_threads++ ;
    
    Trace(""); pthread_mutex_unlock( & gp_Mut->mut_pth) ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : PTHREADS_CONFIG
* @author : s.gravois
* @brief  : Cette fonction affiche toute l info concernant les threads (id, attr, ..)
* @param  : 
* @date   : 2022-10-03 creation
*****************************************************************************************/

void PTHREADS_INFOS(STRUCT_PTHREADS* p_pth) {

  int i=0;
  int l_nb_threads = 0 ;
  int i_ord =0 ;
  int i_pri=0 ;
  int i_id=0 ;
  char c_name[16] ;

  pthread_mutex_lock( & gp_Mut->mut_pth) ;
  l_nb_threads = gi_pthread_nb_threads ;
  Trace(""); pthread_mutex_unlock( & gp_Mut->mut_pth) ;
  
  Trace(""); pthread_mutex_unlock( & gp_Mut->mut_pth) ;

  for( i = 0 ; i < l_nb_threads ; i++ ) {

    pthread_mutex_lock( & gp_Mut->mut_pth) ;

    strcpy( c_name, p_pth->pth_att[ i ].att_nam ) ;
    i_id  = p_pth->pth_att[ i ].att_pid  ;
    i_pri = p_pth->pth_att[ i ].att_pri.sched_priority  ;
    i_ord = p_pth->pth_att[ i ].att_ord  ; 
    
    Trace(""); pthread_mutex_unlock( & gp_Mut->mut_pth) ;

    Trace("thread %-16s : id %d ordonnancement (SCHED_FIFO/SCHED_RR ?) %d priority %d", c_name, i_id, i_ord, i_pri ) ;

  }
  return ;
}

  /*
  if ( gi_pthread_getuid == PTHREADS_USERID_ROOT ) {

    param.sched_priority = PTHREADS_POLICY_1 ;

    if (pthread_setschedparam( pthread_self(), PTHREADS_SCHED_PARAM_SUIVI_VOUTE, & param) != 0) {
      perror("setschedparam SUIVI_VOUTE"); 
      exit(EXIT_FAILURE);
    }
  }
  else { 
    Trace("gi_pthread_getuid not PTHREADS_USERID_ROOT => cannot pthread_setschedparam(args)") ;
  }

  pthread_mutex_lock( & gp_Mut->mut_pth) ;
  pthread_setname_np( pthread_self(), "SUIVI_VOUTE" ) ;
  gp_Sui->p_pthpth_att->att_pid[ gi_pthread_nb_threads++ ] = pthread_self() ;
  Trace(""); pthread_mutex_unlock( & gp_Mut->mut_pth) ;
  */

 /*****************************************************************************************
* @fn     : PTHREADS_AFFICHER_ETAT
* @author : s.gravois
* @brief  : Cette fonction affiche l etat des threads en cours 
* @param  : void
* @date   : 2022-05-24 creation
* @todo   : a finir
*****************************************************************************************/

void   PTHREADS_AFFICHER_ETAT(STRUCT_PTHREADS* p_pth) {

  char c_thread_name [ 16 ] ;
  int i_num_thread=0 ;
  long id_thread ;

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  for (i_num_thread=0; i_num_thread < PTHREADS_MAX_THREADS; i_num_thread++) {

    id_thread = p_pth->pth_att[i_num_thread].att_pid ;
    
    memset( c_thread_name, 0, sizeof(c_thread_name) ) ;
    
    if ( id_thread > 0) pthread_getname_np( id_thread , c_thread_name, 16 ) ;
    else break ;
    
    Trace1("Thread %d : id %ld nom %s ", i_num_thread, id_thread, c_thread_name )  ;
  }
  return ;
}