#include <astro_gpios.h>

void main(int argc, char **argv)
{
  int         i, pid, nbcpus, priority ;
  double      t ;
  double      Fm1, Fm2 , Fpwm ;
  double      periode ;
  double      upas ;
  int         gpiosM1[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  int         gpiosM2[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  int         masque[  GPIO_NB_PHASES_PAR_MOTEUR ] ;
  
  struct sched_param param ;
  cpu_set_t          cpuset ;
  
  pthread_t p_thread_p0[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  pthread_t p_thread_p1[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  
  pthread_t p_thread_m0 ;
  pthread_t p_thread_m1 ;
  
  GPIO_PWM_MOTEUR *pm0 , m0 ; 
  GPIO_PWM_MOTEUR *pm1 , m1 ;
  
  // Debut programme --------------------------
  
  pm0 = &m0 ;
  pm1 = &m1 ;
  gp_Sui = &su ;
  
  pm0->p_sui = (ASTRO_TRACK*)gp_Sui ;   // pour permettre l'acces des membres de ASTRO_TRACK dans GPIO_PWM_MOTEUR
  pm1->p_sui = (ASTRO_TRACK*)gp_Sui ;   // pour permettre l'acces des membres de ASTRO_TRACK dans GPIO_PWM_MOTEUR
  
  if( argc < 8 ) { 
    printf("Usage : %s <gpios moteur1 (ex 5,6,7,8)> <gpios moteur 2> <masque (ex 3,0,2,1)> <fr MOTEUR1 = alt (Hz)> <fr MOTEUR2 = azi (Hz)> <micro pas> <priority (0-99)>\n",argv[0]) ;
    printf("Commande DEUX moteurs sur les pin GPIOS [ gpio1 - gpio1+3 ][ gpio2 - gpio2+3 ] en utilisant une modulation PWM\n") ;
    printf("La frequence PWM est calculee automatiquement par le programme apres calcul optimisations\n") ;
    exit(1) ;
  }
  signal(SIGINT,GPIO_TRAP) ;
  
  GPIO_TAB_TOKEN( gpiosM1, argv[1] , "," ) ;
  GPIO_TAB_TOKEN( gpiosM2, argv[2] , "," ) ;
  GPIO_TAB_TOKEN( masque,  argv[3] , "," ) ;
    
  for(i=0;i<4;i++) TRACE("%d",gpiosM1[i]) ;
  for(i=0;i<4;i++) TRACE("%d",gpiosM2[i]) ;
  for(i=0;i<4;i++) TRACE("%d",masque[i]) ;
  
  Fm1        = atof(argv[4]) ;
  Fm2        = atof(argv[5]) ;
  upas       = atof(argv[6]) ;
  priority   = atoi(argv[7]) ;    
  
  pm0->p_sui->Th_mic = 1 / (Fm1*upas) ;
  pm1->p_sui->Ta_mic = 1 / (Fm2*upas) ;
  
  Fpwm       = GPIO_FREQUENCE_PWM ;
    
  pid=getpid() ;
  nbcpus = sysconf(_SC_NPROCESSORS_ONLN) ;
  
  TRACE("pid processus = %d", pid) ; 
  TRACE("nb cppus      = %d", nbcpus ) ; 

  if ( priority > 0)  {
    mlockall(MCL_CURRENT | MCL_FUTURE);
    TRACE("mise en place temps reel : echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ;
    system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ;
    param.sched_priority = priority ;
    TRACE("mise en place temps reel : param.sched_priority = %d", priority) ;
    if ( sched_setscheduler( pid, SCHED_RR, & param) != 0) { perror("setschedparam"); exit(EXIT_FAILURE);  }
    else printf("modification du processus avec priorite = %d\n", priority) ;
  } 
  pm0 = &m0 ;
  pm1 = &m1 ;
 
  GPIO_INIT_PWM_MOTEUR(pm0,gpiosM1,masque,upas,Fm1, Fpwm,0) ;
  GPIO_INIT_PWM_MOTEUR(pm1,gpiosM2,masque,upas,Fm2, Fpwm,1) ;
  
  sleep(1) ;
  
  // lancement des pthreads -------------------------------------------------------------
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ )
  pthread_create( &p_thread_p0[i], NULL,(void*)GPIO_SUIVI_PWM_PHASE, pm0->phase[i] ) ;
  pthread_create( &p_thread_m0, NULL, (void*)suivi_main_M, pm0 ) ;
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ )
  pthread_create( &p_thread_p1[i], NULL,(void*)GPIO_SUIVI_PWM_PHASE, pm1->phase[i] ) ;
  pthread_create( &p_thread_m1, NULL, (void*)suivi_main_M, pm1 ) ;
  
  // Join des threads -------------------------------------------------------------------
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ )
  pthread_join( p_thread_p0[i], NULL) ; 
  pthread_join( p_thread_m0, NULL) ;
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ )
  pthread_join( p_thread_p1[i], NULL) ; 
  pthread_join( p_thread_m1, NULL) ;
  
}
// ##########################################################################################################
