#define _GNU_SOURCE
/* pour utilisation de pthread_getname_np / pthread_setname_np */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# juin 2022     * creation
# juin 2022     * deport du code concernant temps teel et threads ici
# octobre 2023  * correction bug priorite pthreads + cpu_set (a analyser)
# -------------------------------------------------------------- 
*/

#include "astro_pthreads.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

int  gi_pth_numero ;  
int  gi_pthread_getuid ;
int  gi_pthread_nb_cpu ;
char gc_hostname [ CONFIG_TAILLE_BUFFER_256 ] ; 

static const char *  gc_hach_pth_name[] = {
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
  PTHREAD_POLICY_99,
  PTHREAD_POLICY_99,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_2,
  PTHREAD_POLICY_1
} ;

static const int gi_Pth_Sleep_Before_While[] = {
  PTHREAD_USLEEP_BEFORE_START_SUIVI_PWM_PHASES,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_PWM_MOTOR ,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_MENU      ,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_VOUTE     ,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_INFRA     ,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_LCD       ,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_CLAVIER   ,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_CAPTEUR   ,
  PTHREAD_USLEEP_BEFORE_START_SUIVI_MAIN      
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

static const char * gc_hach_pth_sched_param[] = {
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
* @fn     : PTHREADS_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_PTHREADS *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void PTHREADS_DISPLAY_PREPARE ( STRUCT_PTHREADS * lp_Pth) {

  pthread_t i_id=0 ;
  pthread_t i_id_pth=0 ;
  int       i=0;
  int       l_nb_threads = 0 ;
  int       i_ord =0 ;
  int       i_pri=0 ;
  int       i_sta=0 ;
  char      c_nam[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char      c_ord[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char      c_sta[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char      c_cmd[ CONFIG_TAILLE_BUFFER_256] = {0} ;

  TraceArbo(__func__,2,"display informations on Thread") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pth->pth_mutex ) ;

  strcpy( c_nam, lp_Pth->pth_att[ lp_Pth->pth_index ].att_c_nam ) ;
  strcpy( c_ord, lp_Pth->pth_att[ lp_Pth->pth_index ].att_c_ord ) ;
  strcpy( c_sta, lp_Pth->pth_att[ lp_Pth->pth_index ].att_c_sta ) ;
         i_id  = lp_Pth->pth_att[ lp_Pth->pth_index ].att_pid  ;
         i_pri = lp_Pth->pth_att[ lp_Pth->pth_index ].att_pri.sched_priority  ;
         i_ord = lp_Pth->pth_att[ lp_Pth->pth_index ].att_ord  ; 
         i_sta = lp_Pth->pth_att[ lp_Pth->pth_index ].att_sta ;
      i_id_pth = lp_Pth->pth_t  [ lp_Pth->pth_index ] ;

  sprintf( lp_Pth->pth_dis_cmd , STR_PTH_FORMAT_0, \
    c_nam, \
    i_id, \
    i_id_pth, \
    c_ord, \
    i_pri, \
    c_sta, \
    lp_Pth->pth_index ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pth->pth_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static PTHREADS_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_PTHREADS *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void PTHREADS_DISPLAY ( STRUCT_PTHREADS *lp_Pth) {

  int i_num_thread = 0 ;
  int l_nb_threads = 0 ;

  TraceArbo(__func__,2,"display informations on Threads") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pth->pth_mutex ) ;

  l_nb_threads = gi_pth_numero ; 

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pth->pth_mutex ) ;

  ASTRO_GLOBAL_LOG_ON ( &  lp_Pth->pth_loglevel ) ;

  for( i_num_thread = 0 ; i_num_thread <= l_nb_threads ; i_num_thread++ ) {

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pth->pth_mutex ) ;

    lp_Pth->pth_index = i_num_thread ;
    
    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pth->pth_mutex ) ;

    PTHREADS_DISPLAY_PREPARE( lp_Pth ) ;

    MACRO_ASTRO_GLOBAL_LOG ( lp_Pth->pth_loglevel , 1 , "%s", lp_Pth->pth_dis_cmd ) ;
  }

  ASTRO_GLOBAL_LOG_OFF( &  lp_Pth->pth_loglevel ) ;

  return ;
}
/*****************************************************************************************
* @fn     : PTHREADS_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_PTHREADS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void PTHREADS_LOCK ( STRUCT_PTHREADS * lp_Pth) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pth->pth_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : PTHREADS_LOG_new
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_PTHREADS *
* @date   : 2023-01-26 creation
* @todo   : (a completer )
*****************************************************************************************/

static void PTHREADS_LOG_new ( STRUCT_PTHREADS * lp_Pth) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pth->pth_mutex ) ;
  
  /* TODO : complete */ 

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pth->pth_mutex ) ;
  
  return ;
}
/*****************************************************************************************
* @fn     : PTHREADS_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_PTHREADS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void PTHREADS_UNLOCK ( STRUCT_PTHREADS * lp_Pth) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pth->pth_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : PTHREADS_LOG
* @author : s.gravois
* @brief  : Fonction qui gere les logs de la structure STRUCT_PTHREADS
* @param  : STRUCT_PTHREADS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

 static void PTHREADS_LOG ( STRUCT_PTHREADS * lp_Pth) {

  pthread_t i_id=0 ;
  pthread_t i_id_pth=0 ;
  int       i=0;
  int       l_nb_threads = 0 ;
  int       i_num_thread = 0 ;
  int       i_ord =0 ;
  int       i_pri=0 ;
  int       i_sta=0 ;
  char      c_nam[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char      c_ord[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char      c_sta[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char      c_cmd[ CONFIG_TAILLE_BUFFER_256] = {0} ;

  TraceArbo(__func__,2,"pthreads log") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pth->pth_mutex ) ;

  l_nb_threads = gi_pth_numero ; 

  switch ( lp_Pth->pth_loglevel )
  {
    case 0 :
      lp_Pth->pth_loglevel = 1 ;
      break;
    case 1 :
      lp_Pth->pth_loglevel = 0 ;
      break;
    default:
      lp_Pth->pth_loglevel = 0 ;
      break;
  }

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pth->pth_mutex) ;

  for( i_num_thread = 0 ; i_num_thread <= l_nb_threads ; i_num_thread++ ) {

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pth->pth_mutex ) ;

    strcpy( c_nam, lp_Pth->pth_att[ i_num_thread ].att_c_nam ) ;
    strcpy( c_ord, lp_Pth->pth_att[ i_num_thread ].att_c_ord ) ;
    strcpy( c_sta, lp_Pth->pth_att[ i_num_thread ].att_c_sta ) ;
           i_id  = lp_Pth->pth_att[ i_num_thread ].att_pid  ;
           i_pri = lp_Pth->pth_att[ i_num_thread ].att_pri.sched_priority  ;
           i_ord = lp_Pth->pth_att[ i_num_thread ].att_ord  ; 
           i_sta = lp_Pth->pth_att[ i_num_thread ].att_sta ;
        i_id_pth = lp_Pth->pth_t[ i_num_thread ] ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pth->pth_mutex ) ;

    memset( c_cmd, 0, sizeof(c_cmd)) ;

    sprintf( c_cmd , "(pth) %-16s (id) %ld (id2) %ld (ord) %s (prio) %d (sta) %s (num) %d", \
      c_nam, \
      i_id, \
      i_id_pth, \
      c_ord, \
      i_pri, \
      c_sta, \
      i_num_thread ) ;

      MACRO_ASTRO_GLOBAL_LOG(lp_Pth->pth_loglevel,1,"%s", c_cmd) ;
  }

  return ;
}
/*****************************************************************************************
* @fn     : PTHREADS_INIT_MUTEXS
* @author : s.gravois
* @brief  : Cette fonction initialise les mutexs specifiques a la structure STRUCT_MUTEX independante
* @param  : void
* @date   : 2022-11-04 creation
* @todo   : (obsolete : struct -> mutex are used in place of these)
*****************************************************************************************/

void PTHREADS_INIT_MUTEXS(void) {

  int i_error=0 ;
  int i_errno=0 ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & gp_Mut->mut_glo_alt ) ;
  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & gp_Mut->mut_glo_azi ) ;
  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & gp_Mut->mut_cal ) ;
  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & gp_Mut->mut_dat ) ;
  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & gp_Mut->mut_dat ) ;
  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & gp_Mut->mut_pth ) ;

  return ;
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

  TraceArbo(__func__,0,"init pthreads") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Pth->pth_mutex ) ;
                                     lp_Pth->pth_log      = PTHREADS_LOG ;
                                     lp_Pth->pth_lock     = PTHREADS_LOCK ;
                                     lp_Pth->pth_unlock   = PTHREADS_UNLOCK ;
                                     lp_Pth->pth_display  = PTHREADS_DISPLAY ;
                                     lp_Pth->pth_loglevel = 0 ; 
                                     lp_Pth->pth_file     = NULL ;
  gettimeofday (                   & lp_Pth->pth_tval, NULL ) ;  

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
  }
  else {
    perror("Execute with root privilege (because of threads -> sched_priority used)") ;
    exit(EXIT_FAILURE) ;
  }
  
  return ;
}
/*****************************************************************************************
* @fn     : PTHREADS_CONFIG
* @author : s.gravois
* @brief  : Cette fonction configure un thread, est appelle par un pt de fct
* @param  : STRUCT_LIEU *gp_Lie
* @date   : 2022-06-01 creation
*****************************************************************************************/

void PTHREADS_CONFIG( STRUCT_PTHREADS* lp_Pth, pthread_t i_pth_self, int l_en_thread ) {

  int   i_num_thread = 0 ;
  int   i_error=0 ;
  int   i_errno=0 ;
  int   i_slp=0 ;
  int   i_pri=0, i_ord=0 ;
  char  c_name[PTHREADS_BUFFER_MAX_LENGTH] = {0} ; 
  char  c_ord[PTHREADS_BUFFER_MAX_LENGTH] = {0} ; 

  struct sched_param param ;
  cpu_set_t cpu_set ;
  pthread_attr_t attr ;

  pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL) ;
  pthread_setcanceltype ( PTHREAD_CANCEL_DEFERRED, NULL ) ;  

  TraceArbo(__func__,1,"----------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  /*-------------------------------------------------*/
  /* Recuperation des valeurs d 'attributs de treads */
  /*-------------------------------------------------*/

  i_ord = gi_Pth_Sched_Param[        (int) l_en_thread ] ;
  i_pri = gi_Pth_Sched_Priority[     (int) l_en_thread ] ;
  i_slp = gi_Pth_Sleep_Before_While[ (int) l_en_thread ] ;

  memset( c_name, 0, sizeof(c_name)) ;
  strcpy( c_name, gc_hach_pth_name[(int)l_en_thread]) ;

  memset( c_ord, 0, sizeof(c_ord)) ;
  strcpy( c_ord, gc_hach_pth_sched_param[(int)l_en_thread]) ;

  param.sched_priority = i_pri ;


  /*----------------------------------------------------------*/
  /* Configuration et attribution des CPUs pour chaque thread */
  /*----------------------------------------------------------*/

  // CPU_ZERO(&cpu_set) ;

  /* TODO : FIXME : parametrer les CPUS a utiliser avec un masque */
  /* pour l instant 2022-11 , chaque thread est en ROUND ROBIN sur tous les CPUs */

  // for(int i = 0 ; i< gi_pthread_nb_cpu; i++ ) {
  //   CPU_SET(i,&cpu_set); 
  // }

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
    
    i_num_thread = gi_pth_numero ;
    gi_pth_numero++ ;
    
    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pth->pth_mutex ) ;

    strcpy( lp_Pth->pth_att[ i_num_thread ].att_c_sta , "PTHREAD_RUNNING" ) ;
    strcpy( lp_Pth->pth_att[ i_num_thread ].att_c_nam , c_name ) ;
    strcpy( lp_Pth->pth_att[ i_num_thread ].att_c_ord , c_ord ) ;
            lp_Pth->pth_att[ i_num_thread ].att_pid                = i_pth_self ;
            lp_Pth->pth_att[ i_num_thread ].att_pri.sched_priority = i_pri ;
            lp_Pth->pth_att[ i_num_thread ].att_ord                = i_ord ; 
            lp_Pth->pth_att[ i_num_thread ].att_slp                = i_slp ;
            lp_Pth->pth_att[ i_num_thread ].att_nbc                = gi_pthread_nb_cpu ; 
            lp_Pth->pth_att[ i_num_thread ].att_sta                = (int) PTHREAD_RUNNING ;
    
    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pth->pth_mutex ) ;
  }
  
  if ( i_error == 0 ) {
    Trace("%-16s : enum %d id %lx ord %s prio %d sta %s num %d", \
      c_name, \
      l_en_thread, \
      i_pth_self, \
      c_ord, \
      i_pri, \
      "PTHREAD_RUNNING", \
      i_num_thread ) ;
  }

  /*-------------------------------------------------------------*/
  /* La priorite est l arret du programme en cas de cancellation */
  /* de tous les threads par le thread principal appellant (main)*/
  /* Ne doit pas attendre une requete d annulation correlle a un */
  /* d annulation du thread                                      */
  /* Etat de cancel approprie = PTHREAD_CANCEL_ASYNCHRONOUS      */
  /*-------------------------------------------------------------*/
/*
  pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL) ;
  pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL ) ;
*/ 
  /* FIXME : ajout 2023 mars */
  
  usleep( i_slp ) ;

  TraceArbo(__func__,0,"fin usleep") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  return ;
}

/*****************************************************************************************
* @fn     : PTHREADS_CONFIG
* @author : s.gravois
* @brief  : Cette fonction affiche toute l info concernant les threads (id, attr, ..)
* @param  : 
* @date   : 2022-10-03 creation
*****************************************************************************************/

void PTHREADS_INFOS(STRUCT_PTHREADS* lp_Pth) {

  int i=0;
  int l_nb_threads = 0 ;
  int i_num_thread = 0 ;
  int i_ord =0 ;
  int i_pri=0 ;
  int i_sta=0 ;
  pthread_t i_id=0 ;
  pthread_t i_id_pth=0 ;
  char c_nam[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char c_ord[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char c_sta[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Mut->mut_pth) ;
  l_nb_threads = gi_pth_numero ;
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Mut->mut_pth) ;

  for( i_num_thread = 0 ; i_num_thread <= l_nb_threads ; i_num_thread++ ) {

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Mut->mut_pth) ;

    strcpy( c_nam, lp_Pth->pth_att[ i_num_thread ].att_c_nam ) ;
    strcpy( c_ord, lp_Pth->pth_att[ i_num_thread ].att_c_ord ) ;
    strcpy( c_sta, lp_Pth->pth_att[ i_num_thread ].att_c_sta ) ;
           i_id  = lp_Pth->pth_att[ i_num_thread ].att_pid  ;
           i_pri = lp_Pth->pth_att[ i_num_thread ].att_pri.sched_priority  ;
           i_ord = lp_Pth->pth_att[ i_num_thread ].att_ord  ; 
           i_sta = lp_Pth->pth_att[ i_num_thread ].att_sta ;
           i_id_pth = lp_Pth->pth_t[ i_num_thread ] ;
    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Mut->mut_pth) ;

    Trace("thread %-16s : id %ld id2 %ld ord %s prio %d sta %s num %d", c_nam, i_id, i_id_pth,  c_ord, i_pri, c_sta, i_num_thread ) ;
  }

  return ;
}

  /*
  if ( gi_pthread_getuid == PTHREADS_USERID_ROOT ) {

    param.sched_priority = PTHREAD_POLICY_1 ;

    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_VOUTE, & param) != 0) {
      perror("setschedparam _SUIVI_VOUTE"); 
      exit(EXIT_FAILURE);
    }
  }
  else { 
    Trace("gi_pthread_getuid not PTHREADS_USERID_ROOT => cannot pthread_setschedparam(args)") ;
  }

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Mut->mut_pth) ;
  pthread_setname_np( pthread_self(), "_SUIVI_VOUTE" ) ;
  gp_Sui->p_pthpth_att->att_pid[ gi_pth_numero++ ] = pthread_self() ;
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Mut->mut_pth) ;
  */

 /*****************************************************************************************
* @fn     : PTHREADS_DISPLAY_ETAT
* @author : s.gravois
* @brief  : Cette fonction affiche l etat des threads en cours 
* @param  : void
* @date   : 2022-05-24 creation
* @todo   : a finir
*****************************************************************************************/

void   PTHREADS_DISPLAY_ETAT(STRUCT_PTHREADS* lp_Pth) {

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

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Mut->mut_pth) ;

    strcpy( c_nam,  lp_Pth->pth_att[ i_num_thread ].att_c_nam ) ;
    strcpy( c_ord,  lp_Pth->pth_att[ i_num_thread ].att_c_ord ) ;
    strcpy( c_sta,  lp_Pth->pth_att[ i_num_thread ].att_c_sta ) ;
            i_id  = lp_Pth->pth_att[ i_num_thread ].att_pid  ;
            i_pri = lp_Pth->pth_att[ i_num_thread ].att_pri.sched_priority  ;
            i_sta = lp_Pth->pth_att[ i_num_thread ].att_sta ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Mut->mut_pth) ;

    Trace("PTHREAD %-16s : id %lx ord %s prio %d sta %s", c_nam, i_id, c_ord, i_pri, c_sta ) ;

  }
  return ;
}

 /*****************************************************************************************
* @fn     : PTHREADS_CANCEL_OR_KILL
* @author : s.gravois
* @brief  : Cette fonction organise le code pour annuler tous les threads 
* @brief  :   (il doit etre JOINABLE & CANCELLABLE)
* @param  : void
* @date   : 2022-11-05 creation
* @todo   : 
*****************************************************************************************/

void PTHREADS_CANCEL_OR_KILL ( STRUCT_PTHREADS *lp_Pth) {

  int i_num_thread=0 ;
  char c_thread_name [ 16 ] ; 
  int i=0;
  int l_nb_threads = 0 ;
  int i_ord =0 ;
  int i_pri=0 ;
  int i_err=0 ;
  int i_abandon=0 ;
  int i_sta=0 ;
  char c_nam[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char c_ord[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  char c_sta[ CONFIG_TAILLE_BUFFER_32 ] = {0} ;
  pthread_t i_id = 0 ;

  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_LCD] ) != 0 )             {  Trace("pthread_cancel error PTHREAD_T_LCD") ; }
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_CLAVIER] ) != 0 )         {  Trace("pthread_cancel error PTHREAD_T_CLAVIER") ; }
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_CAPTEUR] ) != 0 )         {  Trace("pthread_cancel error PTHREAD_T_CAPTEUR") ; }
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_INFRA] ) != 0 )           {  Trace("pthread_cancel error PTHREAD_T_INFRA") ; }
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_VOUTE] ) != 0 )           {  Trace("pthread_cancel error PTHREAD_T_VOUTE") ; }
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_MENU] ) != 0 )            {  Trace("pthread_cancel error PTHREAD_T_MENU") ; } 
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_MOT_ALT] ) != 0 )         {  Trace("pthread_cancel error PTHREAD_T_MOT_ALT") ; }
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_MOT_AZI] ) != 0 )         {  Trace("pthread_cancel error PTHREAD_T_MOT_AZI") ; }              
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_0] ) != 0 ) {  Trace("pthread_cancel error PTHREAD_T_MOT_AZI_PHASE_0") ; }  
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_1] ) != 0 ) {  Trace("pthread_cancel error PTHREAD_T_MOT_AZI_PHASE_1") ; }  
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_2] ) != 0 ) {  Trace("pthread_cancel error PTHREAD_T_MOT_AZI_PHASE_2") ; }  
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_3] ) != 0 ) {  Trace("pthread_cancel error PTHREAD_T_MOT_AZI_PHASE_3") ; }
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_0] ) != 0 ) {  Trace("pthread_cancel error PTHREAD_T_MOT_ALT_PHASE_0") ; }  
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_1] ) != 0 ) {  Trace("pthread_cancel error PTHREAD_T_MOT_ALT_PHASE_1") ; }  
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_2] ) != 0 ) {  Trace("pthread_cancel error PTHREAD_T_MOT_ALT_PHASE_2") ; }  
  if ( pthread_cancel ( lp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_3] ) != 0 ) {  Trace("pthread_cancel error PTHREAD_T_MOT_ALT_PHASE_3") ; }

  for( i_num_thread = PTHREADS_MAX_THREADS ; i_num_thread >=0  ; i_num_thread-- )   {

    i_abandon=0 ;

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Mut->mut_pth) ;

    strcpy( c_nam,  lp_Pth->pth_att[ i_num_thread ].att_c_nam ) ;
    strcpy( c_ord,  lp_Pth->pth_att[ i_num_thread ].att_c_ord ) ;
    strcpy( c_sta,  lp_Pth->pth_att[ i_num_thread ].att_c_sta ) ;
            i_id  = lp_Pth->pth_att[ i_num_thread ].att_pid  ;
            i_pri = lp_Pth->pth_att[ i_num_thread ].att_pri.sched_priority  ;
            i_sta = lp_Pth->pth_att[ i_num_thread ].att_sta ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Mut->mut_pth) ;

    if ( i_sta == PTHREAD_RUNNING ) {
      memset( c_thread_name, 0, sizeof(c_thread_name) ) ;
/*
      if ( pthread_getname_np( i_id , c_thread_name, 16 ) ) {
        Trace1("pthread_getname_np : %ld : error", i_id) ;
      }
  */    
      // if ( pthread_cancel( i_id ) != 0 ) { 

      if ( ( i_err = pthread_cancel ( i_id )) != 0 ) { 
        i_abandon = 1 ; 
/*      
        if ( pthread_kill ( i_id, SIGTERM ) != 0 ) { 
          i_abandon = 2 ; 
        }
        else {
          i_abandon = 3 ; 
          
          i_sta = (int)PTHREAD_KILLED ;
          strcpy( c_sta, "PTHREAD_KILLED");

          HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Mut->mut_pth) ;

          lp_Pth->pth_att[ i_num_thread ].att_sta = (int) i_sta ;    
          strcpy( lp_Pth->pth_att[ i_num_thread ].att_c_sta , c_sta ) ;
          
          HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Mut->mut_pth) ;
        }
*/
      }
      else {
          i_abandon = 4 ; 

          i_sta = (int)PTHREAD_CANCELLED ;
          strcpy( c_sta, "PTHREAD_CANCELLED");

          HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Mut->mut_pth) ;

          lp_Pth->pth_att[ i_num_thread ].att_sta = i_sta ;    
          strcpy( lp_Pth->pth_att[ i_num_thread ].att_c_sta , c_sta ) ;
          
          HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Mut->mut_pth) ;
      }
    }
    else {
      i_abandon = 2 ;
    }
    
    if ( i_abandon == 0 ) {
      Trace("not run %-15s : id %ld ord %s prio %d sta %s num %d", c_nam, i_id, c_ord, i_pri, c_sta, i_num_thread ) ;
    }
    if ( i_abandon == 1 ) {
      Trace("canc KO %-15s : id %ld ord %s prio %d sta %s num %d err %d", c_nam, i_id, c_ord, i_pri, c_sta, i_num_thread, i_err ) ;
    }
    if ( i_abandon == 2 ) {
      Trace("kill KO %-15s : id %ld ord %s prio %d sta %s num %d", c_nam, i_id, c_ord, i_pri, c_sta, i_num_thread ) ;
    }
    if ( i_abandon == 3 ) {
      Trace("kill ok %-15s : id %ld ord %s prio %d sta %s num %d", c_nam, i_id, c_ord, i_pri, c_sta, i_num_thread ) ;
    }
    if ( i_abandon == 3 ) {
      Trace("canc ok %-15s : id %ld ord %s prio %d sta %s num %d", c_nam, i_id, c_ord, i_pri, c_sta, i_num_thread ) ;
    }
    // sleep(1) ;
  }
  return ;
}