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

int gi_pth_numero ;  
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
  PTHREAD_POLICY_5,
  PTHREAD_POLICY_5,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_1
} ;

/* Les  structures suivantes definissent la politique d ordonnancement */
/* par defaut tout le temps ROUND ROBIN */

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

static const char * gc_Pth_Sched_Param[] = {
  "SCHED_RR",
  "SCHED_RR",
  "SCHED_RR",
  "SCHED_RR",
  "SCHED_RR",
  "SCHED_RR",
  "SCHED_RR",
  "SCHED_RR",
  "SCHED_RR"
} ;

/*****************************************************************************************
* @fn     : PTHREADS_INIT_MUTEXS
* @author : s.gravois
* @brief  : Cette fonction initialise les mutexs specifiques a la structure STRUCT_MUTEX independante
* @param  : void
* @date   : 2022-11-04 creation
* @todo   : 
*****************************************************************************************/

void PTHREADS_INIT_MUTEXS(void) {
  int i_error=0 ;
  int i_errno=0 ;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( ( i_error = pthread_mutex_init( & gp_Mut->mut_glo_alt, NULL )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_mutex_init mut_glo_alt");
    exit(EXIT_FAILURE); 
  }
  if ( ( i_error = pthread_mutex_init( & gp_Mut->mut_glo_azi, NULL )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_mutex_init mut_glo_azi");
    exit(EXIT_FAILURE); 
  }
  if ( ( i_error = pthread_mutex_init( & gp_Mut->mut_cal, NULL )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_mutex_init mut_cal");
    exit(EXIT_FAILURE); 
  }
  if ( ( i_error = pthread_mutex_init( & gp_Mut->mut_dat , NULL )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_mutex_init mut_dat");
    exit(EXIT_FAILURE); 
  }
  if ( ( i_error = pthread_mutex_init( & gp_Mut->mut_pth , NULL )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_mutex_init mut_pth");
    exit(EXIT_FAILURE); 
  }
}
/*****************************************************************************************
* @fn     : PTHREADS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise les threads et le temps reel
* @param  : STRUCT_PTHREADS *lp_Pth
* @param  : pthread_t i_pth_self
* @date   : 2022-06-01 creation
* @date   : 2022-09-30 ajout argument pthread_t i_pth_self 
* @date   : split partie mutexs dans fct dediee
* @todo   :
*****************************************************************************************/

void PTHREADS_INIT( STRUCT_PTHREADS *lp_Pth, pthread_t i_pth_self ) {

  int i_error=0 ;
  int i_errno=0 ;
  struct sched_param param; 
  char c_rlogin[ 16] = {0} ;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  lp_Pth->pth_i_nb = 0 ;
  
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
    /*
    param.sched_priority = PTHREAD_POLICY_1 ;

    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_MAIN, & param) != 0) {
        perror("setschedparam main");
        exit(EXIT_FAILURE);
    }
    // PTHREADS_CONFIG( gp_Pth, i_pth_self, PTHREAD_TYPE_MAIN ) ;
    */
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

void PTHREADS_CONFIG( STRUCT_PTHREADS* lp_pth, pthread_t i_pth_self, int l_en_thread ) {

  int   i_num_thread = 0 ;
  int   i_error=0 ;
  int   i_errno=0 ;
  int   i_pri=0, i_ord=0 ;
  char  c_name[PTHREADS_BUFFER_MAX_LENGTH] = {0} ; 
  char  c_ord[PTHREADS_BUFFER_MAX_LENGTH] = {0} ; 

  struct sched_param param ;
  cpu_set_t cpu_set ;
  pthread_attr_t attr ;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  /*-------------------------------------------------*/
  /* Recuperation des valeurs d 'attributs de treads */
  /*-------------------------------------------------*/

  i_ord = gi_Pth_Sched_Param[ (int) l_en_thread ] ;
  i_pri = gi_Pth_Sched_Priority[ (int) l_en_thread ] ;

  memset( c_name, 0, sizeof(c_name)) ;
  strcpy( c_name, gi_Pth_Name[(int)l_en_thread]) ;

  memset( c_ord, 0, sizeof(c_ord)) ;
  strcpy( c_ord, gc_Pth_Sched_Param[(int)l_en_thread]) ;

  param.sched_priority = i_pri ;

  /*----------------------------------------------------------*/
  /* Configuration et attribution des CPUs pour chaque thread */
  /*----------------------------------------------------------*/

  CPU_ZERO(&cpu_set) ;

  /* TODO : FIXME : parametrer les CPUS a utiliser avec un masque */
  /* pour l instant 2022-11 , chaque thread est en ROUND ROBIN sur tous les CPUs */

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
  if ( (i_error=pthread_attr_setschedpolicy (&attr, i_ord )) != 0 ) {
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

  if ( gi_pthread_getuid == PTHREADS_USERID_ROOT ) {

    if ( (i_error= pthread_setschedparam( i_pth_self, i_ord, & param )) != 0) { 
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

  if ( (i_error=pthread_setname_np( i_pth_self, c_name )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_setname_np");
    exit(EXIT_FAILURE); 
  }

  if ( i_error == 0 ) {
    pthread_mutex_lock( & gp_Mut->mut_pth) ;

    i_num_thread = gi_pth_numero ;
    gi_pth_numero++ ;
    
    strcpy( lp_pth->pth_att[ i_num_thread ].att_c_sta , "PTHREAD_RUNNING" ) ;
    strcpy( lp_pth->pth_att[ i_num_thread ].att_c_nam , c_name ) ;
    strcpy( lp_pth->pth_att[ i_num_thread ].att_c_ord , c_ord ) ;
            lp_pth->pth_att[ i_num_thread ].att_pid                = i_pth_self ;
            lp_pth->pth_att[ i_num_thread ].att_pri.sched_priority = i_pri ;
            lp_pth->pth_att[ i_num_thread ].att_ord                = i_ord ; 
            lp_pth->pth_att[ i_num_thread ].att_nbc                = gi_pthread_nb_cpu ; 
            lp_pth->pth_att[ i_num_thread ].att_sta                = (int) PTHREAD_RUNNING ;
    
    pthread_mutex_unlock( & gp_Mut->mut_pth) ;
  }
  if ( i_error == 0 ) {
    Trace("thread %-16s : id %ld ord %s prio %d sta %s num %d", c_name, i_pth_self, c_ord, i_pri, "PTHREAD_RUNNING", i_num_thread ) ;
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

void PTHREADS_INFOS(STRUCT_PTHREADS* lp_pth) {

  int i=0;
  int l_nb_threads = 0 ;
  int i_num_thread = 0 ;
  int i_ord =0 ;
  int i_pri=0 ;
  int i_sta=0 ;
  pthread_t i_id=0 ;
  char c_nam[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char c_ord[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char c_sta[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;

  pthread_mutex_lock( & gp_Mut->mut_pth) ;
  l_nb_threads = gi_pth_numero ;
  pthread_mutex_unlock( & gp_Mut->mut_pth) ;

  for( i_num_thread = 0 ; i_num_thread < l_nb_threads ; i_num_thread++ ) {

    pthread_mutex_lock( & gp_Mut->mut_pth) ;

    strcpy( c_nam, lp_pth->pth_att[ i_num_thread ].att_c_nam ) ;
    strcpy( c_ord, lp_pth->pth_att[ i_num_thread ].att_c_ord ) ;
    strcpy( c_sta, lp_pth->pth_att[ i_num_thread ].att_c_sta ) ;
           i_id  = lp_pth->pth_att[ i_num_thread ].att_pid  ;
           i_pri = lp_pth->pth_att[ i_num_thread ].att_pri.sched_priority  ;
           i_ord = lp_pth->pth_att[ i_num_thread ].att_ord  ; 
           i_sta = lp_pth->pth_att[ i_num_thread ].att_sta ;

    pthread_mutex_unlock( & gp_Mut->mut_pth) ;

    Trace("thread %-16s : id %ld ord %s prio %d sta %s num %d", c_nam, i_id, c_ord, i_pri, c_sta, i_num_thread ) ;
  }
  return ;
}

  /*
  if ( gi_pthread_getuid == PTHREADS_USERID_ROOT ) {

    param.sched_priority = PTHREAD_POLICY_1 ;

    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_VOUTE, & param) != 0) {
      perror("setschedparam SUIVI_VOUTE"); 
      exit(EXIT_FAILURE);
    }
  }
  else { 
    Trace("gi_pthread_getuid not PTHREADS_USERID_ROOT => cannot pthread_setschedparam(args)") ;
  }

  pthread_mutex_lock( & gp_Mut->mut_pth) ;
  pthread_setname_np( pthread_self(), "SUIVI_VOUTE" ) ;
  gp_Sui->p_pthpth_att->att_pid[ gi_pth_numero++ ] = pthread_self() ;
  pthread_mutex_unlock( & gp_Mut->mut_pth) ;
  */

 /*****************************************************************************************
* @fn     : PTHREADS_AFFICHER_ETAT
* @author : s.gravois
* @brief  : Cette fonction affiche l etat des threads en cours 
* @param  : void
* @date   : 2022-05-24 creation
* @todo   : a finir
*****************************************************************************************/

void   PTHREADS_AFFICHER_ETAT(STRUCT_PTHREADS* lp_pth) {

  int i_num_thread=0 ;
  char c_thread_name [ 16 ] ; 
  int i=0;
  int l_nb_threads = 0 ;
  int i_ord =0 ;
  int i_pri=0 ;
  int i_sta=0 ;
  char c_nam[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char c_ord[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char c_sta[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  pthread_t i_id = 0 ;

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  for (i_num_thread=0; i_num_thread < PTHREADS_MAX_THREADS; i_num_thread++) {

    pthread_mutex_lock( & gp_Mut->mut_pth) ;

    strcpy( c_nam,  lp_pth->pth_att[ i_num_thread ].att_c_nam ) ;
    strcpy( c_ord,  lp_pth->pth_att[ i_num_thread ].att_c_ord ) ;
    strcpy( c_sta,  lp_pth->pth_att[ i_num_thread ].att_c_sta ) ;
            i_id  = lp_pth->pth_att[ i_num_thread ].att_pid  ;
            i_pri = lp_pth->pth_att[ i_num_thread ].att_pri.sched_priority  ;
            i_sta = lp_pth->pth_att[ i_num_thread ].att_sta ;

    pthread_mutex_unlock( & gp_Mut->mut_pth) ;

    Trace("thread %-16s : id %ld ord %s prio %d sta %s", c_nam, i_id, c_ord, i_pri, c_sta ) ;

  }
  return ;
}

 /*****************************************************************************************
* @fn     : PTHREADS_CANCEL
* @author : s.gravois
* @brief  : Cette fonction organise le code pour annuler tous les threads 
* @brief  :   (il doit etre JOINABLE & CANCELLABLE)
* @param  : void
* @date   : 2022-11-05 creation
* @todo   : 
*****************************************************************************************/

void PTHREADS_CANCEL ( STRUCT_PTHREADS *lp_pth) {

  int i_num_thread=0 ;
  char c_thread_name [ 16 ] ; 
  int i=0;
  int l_nb_threads = 0 ;
  int i_ord =0 ;
  int i_pri=0 ;
  int i_abandon=0 ;
  int i_sta=0 ;
  char c_nam[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char c_ord[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char c_sta[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  pthread_t i_id = 0 ;

  for( i_num_thread = PTHREADS_MAX_THREADS-1 ; i_num_thread >=0  ; i_num_thread-- )   {

    i_abandon=0 ;

    // pthread_mutex_lock( & gp_Mut->mut_pth) ;

    strcpy( c_nam,  lp_pth->pth_att[ i_num_thread ].att_c_nam ) ;
    strcpy( c_ord,  lp_pth->pth_att[ i_num_thread ].att_c_ord ) ;
    strcpy( c_sta,  lp_pth->pth_att[ i_num_thread ].att_c_sta ) ;
            i_id  = lp_pth->pth_att[ i_num_thread ].att_pid  ;
            i_pri = lp_pth->pth_att[ i_num_thread ].att_pri.sched_priority  ;
            i_sta = lp_pth->pth_att[ i_num_thread ].att_sta ;

    // pthread_mutex_unlock( & gp_Mut->mut_pth) ;

    if ( i_sta == PTHREAD_RUNNING ) {
      memset( c_thread_name, 0, sizeof(c_thread_name) ) ;
/*
      if ( pthread_getname_np( i_id , c_thread_name, 16 ) ) {
        Trace1("pthread_getname_np : %ld : error", i_id) ;
      }
  */    
      if ( pthread_cancel( i_id ) != 0 ) { 
        i_abandon = 0 ; 
      }
      else {
        i_abandon = 1 ; 
        // pthread_mutex_lock( & gp_Mut->mut_pth) ;

        lp_pth->pth_att[ i_num_thread ].att_sta                = (int) PTHREAD_CANCELLED ;    
        strcpy( lp_pth->pth_att[ i_num_thread ].att_c_sta , "PTHREAD_CANCELLED" ) ;
        
        // pthread_mutex_unlock( & gp_Mut->mut_pth) ;
      }
    }
    else {
      i_abandon = 2 ;
    }
    
    if ( i_abandon == 0 ) {
      Trace("canc KO %-15s : id %ld ord %s prio %d sta %s num %d", c_nam, i_id, c_ord, i_pri, c_sta, i_num_thread ) ;
    }
    if ( i_abandon == 1 ) {
      Trace("canc OK %-15s : id %ld ord %s prio %d sta %s num %d", c_nam, i_id, c_ord, i_pri, c_sta, i_num_thread ) ;
    }
    if ( i_abandon == 2 ) {
      Trace("not run %-15s : id %ld ord %s prio %d sta %s num %d", c_nam, i_id, c_ord, i_pri, c_sta, i_num_thread ) ;
    }
    usleep(10000) ;
  }
  return ;
}