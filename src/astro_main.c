/* #define _GNU_SOURCE */
/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# Fichier     | astro.c
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | * ajout entete
#               * suite a ajout variables CONFIG_REP et CONFIG_FIC
#                 il faut d'abord le fichier config.txt en priorite
# 17/01/2022  | * ajout gestion touches gp_Key (*_CLAVIER)
#               * nouveaux fichiers astro_keyboard.h / .c
# 18/01/2022  | * test de fonctions SUIVI_CLAVIER faisant inervenir
#  (issue)        les appels ncurses : KO quand la fonction SUIVI_CLAVIER
#                 est appelle apres la fonction close dans GPIO_CLOSE (???)
#               * creation entete de la fonction au format doxygen des fonctions suivant doxygen
# 19/01/2002  | * suppression passage arguments CONGI_AFFICHER_TOUT
# (issue)         cela est justifie par le fait que les variables sont globales
# 
# 11/03/2022  | * prise en compte option -p (facon classique)
#               * renseignement gp_Con_Par->par_rep_home par -p <path>
# 11/03/2022  | * ajout appel fonction ARGUMENTS_GERER_REP_HOME
#
# 21/03/2022  | * ajout appel CODES_INIT dasn main 
#                 en remaplacement de IR_INIT_CODES
# avril 2002  | * debut codage fonctionnalites LCD1602
#               * suppressione appel GPIO_CLAVIER_MATRICIEL_READ
#               * correction BUG ( repetition if ( i_indice_code < CODES_CODE_NB_CODES ))
# mai 2022      * petites corrections mineures
#               * reprise intégralité code pour utilisation pointeur fct (gp_Lcd->display_xxx)
#               -> display / display_default pour affichage par defaut
#               -> display_xxx pour affichage autre que default
#               => affichage par default parametrable
#
# octobre 2022  * refonte complete du code : deplacemements de code
#                 dans des fichiers *.c dedies 
# -------------------------------------------------------------- 
*/

#include "astro_main.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/*****************************************************************************************
* @fn     : ASTRO_TRAP_MAIN
* @author : s.gravois
* @brief  : fonction appelle quand un signal est trape dans main
* @param  : int     sig
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 remaniement fonction avec nom et id de thread 
* @date   : 2022-09-29 ajout test de retour des fcts pthread
* @todo   : voir s il est preferable de faire un kill(getpid(),SIGKILL) 
*           plutot qu un exit
*****************************************************************************************/

void ASTRO_TRAP_MAIN(int sig) {

  int l_nb_threads = g_nb_threads ;
  int i_num_thread=0 ;
  long id_thread =0;
  char c_thread_name [ 16 ] ; 
  char c_cmd[ 64 ] ;
  int i ;

  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  TRACE("Signal trappe de valeur (sig) %d",sig) ;
  TRACE("Envoi d'un signal %d (SIGTERM) aux threads",SIGTERM) ;
  
  GPIO_CLIGNOTE(gp_Gpi_Par_Pwm->par_led_etat, 1, 100) ;

  //Trace("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;

  /*--------------------------------------------------------*/
  /* Si sig > positif on abandonne les threads et on quitte */
  /*--------------------------------------------------------*/

  Trace("pthread_cancel : %d threads a abandonner", g_nb_threads ) ;

  for( i_num_thread = l_nb_threads ; i_num_thread > 0 ; i_num_thread-- )   {
    
    g_nb_threads-- ;
    id_thread = gp_Pth->pth_att[i_num_thread].att_pid ;

    if ( id_thread >0 ) {
      memset( c_thread_name, 0, sizeof(c_thread_name) ) ;

      if ( pthread_getname_np( id_thread , c_thread_name, 16 ) ) {
        Trace("pthread_getname_np : %ld : error", id_thread) ;
      }
      
      Trace("Abandon thread : num %d id %ld nom %s" ,i_num_thread,id_thread,c_thread_name) ;
      
      if ( pthread_cancel( id_thread ) != 0 ) {
        Trace("pthread_cancel : %s : error", c_thread_name) ;
      }
    }
  }
  Trace("fin des abandons de threads") ;
  
  /*----------------------------------*/
  /* Rendre le terminal propre (sane) */
  /*----------------------------------*/

  memset( c_cmd, 0, sizeof(c_cmd)) ;
  sprintf(c_cmd,"%s sane", g_Path_Cmd_Stty ) ;

  if ( system( c_cmd ) < 0 ) {

    SyslogErr("Probleme avec stty sane\n") ;
    Trace("Probleme avec stty sane") ;

    gp_Lcd->display_str_int(1000000,"exit with return", 2) ;
    exit(2) ;
  } 

  /*----------------------------------*/
  /* Si sig == 0 on arrete le systeme */
  /*----------------------------------*/

  if (sig==0) {

    gp_Lcd->display_str_int(0,"Halt with sig :", sig) ;

    GPIO_CLIGNOTE(gp_Gpi_Par_Pwm->par_led_etat, 1, 100) ;

    if ( system("/sbin/halt") < 0 ) {
      SyslogErr("Probleme avec /sbin/halt\n") ;
      Trace("Probleme avec /sbin/halt") ;

      gp_Lcd->display_str_int(1000000,"exit with return", 2) ;
      exit(2) ;
    } 
  }
  GPIO_SET( gp_Gpi_Par_Pwm->par_led_etat, 0 ) ;

  closelog();
/*
  if ( sig == SIGINT ){
		kill(getpid(),SIGKILL) ;
	}
	if ( sig == SIGKILL ){
		kill(getpid(),SIGKILL) ;
	} */

  Trace("appel LCD_DISPLAY_STRING_INT") ;
  gp_Lcd->display_str_int(0,"Exit with pid :", getpid()) ;

  Trace("appel exit(0)") ;
  
  gp_Lcd->display_str_int(1000000,"exit with return", 0) ;
  exit(0) ;
}

/*****************************************************************************************
* @fn     : ASTRO_TRAP_SUIVI_MENU
* @author : s.gravois
* @brief  : fonctions appellees quand un signal est trape dans leur fonctions respectives
* @param  : int     sig
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-04-27 ajout affichage LCD
* @date   : 2022-04-27 ajout ASTRO_TRAP_MAIN a la fin des fonctions
* @date   : 2022-05-20 correction pthread_cancel  gp_Pth->pth_lcd ) ;
* @date   : 2022-05-20 suppression ASTRO_TRAP_SUIVI_TERMIOS
* @todo   : (completer)
*****************************************************************************************/

void ASTRO_TRAP_SUIVI_MENU(int sig)  {
  
  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  Trace("Signal trappe depuis thread suivi_menu = %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_men ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_VOUTE(int sig)  {
  
  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  Trace("Signal trappe depuis thread suivi_voute= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_vou ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_INFRAROUGE(int sig)  {
  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  Trace("Signal trappe depuis thread suivi_infrarouge= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_inf ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_CAPTEURS(int sig)  {
  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  Trace("Signal trappe depuis thread suivi_capteurs= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_cap ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_CLAVIER(int sig)  {
  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  Trace("Signal trappe depuis thread suivi_clavier= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_cla ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_LCD(int sig)  {
  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  Trace("Signal trappe depuis thread suivi_termios= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_lcd ) ;
  ASTRO_TRAP_MAIN(1) ;
}

/*****************************************************************************************
* @fn     : SUIVI_MENU
* @author : s.gravois
* @brief  : Ce mode permet de gerer les menus .
* @param  : STRUCT_SUIVI   *gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_nb_threads++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : 
*****************************************************************************************/

void * SUIVI_MENU(STRUCT_SUIVI * gp_Sui) {
  
  long long ll_inrc=0 ;
  struct sched_param param;
  void * (*ptr_fct)(STRUCT_SUIVI *)  = SUIVI_MENU ;

  Trace("ptr_fct : %p", ptr_fct ) ;
  
   // La temporisation dans les boucles du thread SUIVI_MENU depend de  gp_Sui->temporisation_menu
   
  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  usleep( PTHREADS_USLEEP_BEFORE_START_SUIVI_MENU ) ;
  Trace1("start") ;

  /* ---------------------------------------------------------------------------- */
  /* Configuration du thread et des attributs de tread                            */
  /* ---------------------------------------------------------------------------- */
/*
  if ( guit_user_getuid == PTHREADS_USERID_ROOT ) {

    param.sched_priority = PTHREADS_POLICY_1 ;
    
    if (pthread_setschedparam( pthread_self(), PTHREADS_SCHED_PARAM_SUIVI_MENU, & param) != 0) { 
      perror("setschedparam SUIVI_MENU");
      exit(EXIT_FAILURE); 
    }
  }
  else { 
    Trace("guit_user_getuid not PTHREADS_USERID_ROOT => cannot pthread_setschedparam(args)") ;
  }

  pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
  pthread_setname_np( pthread_self(), "SUIVI_MENU" ) ;
  gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
  pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;
*/
  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREADS_SUIVI_MENU ) ;

  /* ---------------------------------------------------------------------------- */
  /* Configuration siganxu pour trapper les evenements                            */
  /* ---------------------------------------------------------------------------- */

  signal(SIGTERM,ASTRO_TRAP_SUIVI_MENU) ;
  
  SUIVI_MENU_PREALABLE ( gp_Sui) ;

  gp_Sui->reset = 1 ;

  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread SUIVI_MENU
  //-------------------------------------------------------------------------------
  
  /* Debut boucle SUIVI_MENU */
  while(TRUE) {

    Trace("temporisation %ld", gp_Sui->temporisation_menu ) ;
    usleep( gp_Sui->temporisation_menu ) ;

    /* GPIO_CLAVIER_MATRICIEL_READ( gpio_key_l, gpio_key_c, gp_Key) ; */
    KEYS_INPUTS_GESTION_APPUIS( gp_Sui, gp_Key) ;

    /* SUIVI_OLD_0( gp_Sui) ; */
    SUIVI_TRAITEMENT_MOT( gp_Sui, gp_Key ) ;

    CONFIG_AFFICHER_CHANGEMENTS() ;

    KEYS_AFFICHER( gp_Key ) ;	


    switch( gp_Sui->menu ) {

      // ------------------------------- ALIGNEMENT - MODE AZIMUTAL ----------------------------
     
      case MENU_AZIMUTAL :
        
        Trace("MENU_AZIMUTAL") ;
        
        // a modifier / completer : TEMPS_CALCUL_TEMPS_SIDERAL et CALCUL_ANGLE_HORAIRE
        // sont a supprimer car deja calculer dans SUIVI_

        Trace("appel : %d : MENU_AZIMUTAL" , gp_Sui->menu) ;

        pthread_mutex_lock(& gp_Mut->mut_azi );   
        pthread_mutex_lock(& gp_Mut->mut_alt );

        gp_Sui->acc_alt          = 1 ;
        gp_Sui->acc_azi          = 1 ;

        Trace(""); pthread_mutex_unlock(& gp_Mut->mut_azi );   
        Trace(""); pthread_mutex_unlock(& gp_Mut->mut_alt );

        gp_Sui->SUIVI_EQUATORIAL = 0 ;
        gp_Sui->SUIVI_VOUTE      = 1 ; 

				/* CALCUL_TOUT() ; */

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ; 

      break ;

      // ------------------------------  MODE EQUATORIAL --------------------------------------
     
      case MENU_EQUATORIAL :
        
        Trace("MENU_EQUATORIAL") ;
        
        pthread_mutex_lock(& gp_Mut->mut_azi );   
        pthread_mutex_lock(& gp_Mut->mut_alt );

        gp_Sui->acc_alt          = 0 ;
        gp_Sui->acc_azi          = 1 ;

        Trace(""); pthread_mutex_unlock(& gp_Mut->mut_azi );   
        Trace(""); pthread_mutex_unlock(& gp_Mut->mut_alt );

        gp_Sui->SUIVI_EQUATORIAL = 1 ;
        gp_Sui->SUIVI_VOUTE      = 0 ; 

				/* CALCUL_TOUT() ; */

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ;

      break ;

      // -------------------------- STRUCT_SUIVI MANUEL PAR DEFAUT ------------------
      
      case MENU_MANUEL_BRUT :
        
        Trace("MENU_MANUEL_BRUT") ;
        
        // le but de ce suivi est de deduire des actions N-S-O-E de l'utilisateur 
        // simplement une acceleration / ralentissement / changement de direction
        // sur le N-S-E-O (pas de recalcul des periodes) 
        // FIXME : les periodes sont conservees , ainsi que le mode azimutal ou equatorial

        if( strcmp( gp_Sui->sui_dat.dat_inf, "") != 0 ) {
          Trace("1 gp_Sui->sui_dat.dat_inf = %s", gp_Sui->sui_dat.dat_inf ) ;
        }

         /* SUIVI_MANUEL_BRUT(gp_Sui, gp_Key) ; */ 
        
        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ; 

      break ;

      /* -------------------------- MENU_MANUEL_NON_ASSERVI : LE PLUS SIMPLE ------------------
          TODO : a coder : juste appui des touches provoque le mouvement
          TODO : mode le plus simple a priori
          TODO : acc_azi_ et acc_alt doivent etre initialises a zero
      */
      
      case MENU_MANUEL_NON_ASSERVI :
        
        Trace("MENU_MANUEL_NON_ASSERVI") ;
        
        // Suivi le plus simple : seules les touches est nord sud ouest et reset sont prises en compte
        // TODO : verifier

        Trace("appel : %d : MENU_MANUEL_NON_ASSERVI" , gp_Sui->menu) ;

        pthread_mutex_lock(& gp_Mut->mut_azi );   
        pthread_mutex_lock(& gp_Mut->mut_alt );

        gp_Sui->acc_alt          = 0 ;
        gp_Sui->acc_azi          = 0 ;

        Trace(""); pthread_mutex_unlock(& gp_Mut->mut_azi );   
        Trace(""); pthread_mutex_unlock(& gp_Mut->mut_alt );

				/* CALCUL_TOUT() ; */

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_NON_ASSERVI ; 

      break ;

      // -------------------------- STRUCT_SUIVI MANUEL AVEC RE-CALCUL DES PERIODES  -----------------
      case MENU_MANUEL_ASSERVI :

        Trace("MENU_MANUEL_ASSERVI") ;       

        // TODO : a modifier car cela marche pas tres bien (interraction avec le thread SUIVI_VOUTE)
        // TODO : le but de ce gp_Sui est de deduire des actions N-S-O-E de l'utilisateur 
        // TODO : les periodes / frequences en azimut et altitude

        SUIVI_MANUEL_ASSERVI(gp_Sui, gp_Key) ; 
        CALCUL_PERIODES_SUIVI_MANUEL(gp_Ast,gp_Sui,gp_Vou)  ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_ASSERVI ; 

      break ;

      // ------------------------------  MODE GOTO --------------------------------------

      case MENU_GOTO :

        Trace("appel : %d : MENU_GOTO" , gp_Sui->menu) ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ;

      break ; 

      // ------------------------------  MODE INFO --------------------------------------

      case MENU_INFO :

        Trace("appel : %d : MENU_INFO" , gp_Sui->menu) ;

        CONFIG_AFFICHER_TOUT() ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ;

      break ; 

      // ------------------------------ MODE ACTIVATION DU RESEAU ----------------------------------
     
      case MENU_RESEAU_UP :

        Trace("appel : %d : MENU_RESEAU_UP" , gp_Sui->menu) ;

        if ( system("/etc/init.d/OLD/S40network start")) {
         perror("Probleme avec system(/etc/init.d/OLD/S40network start)"); 
         Trace("Probleme avec system(/etc/init.d/OLD/S40network start)\n") ;
        }

        gp_Sui->menu = gp_Sui->menu_old ;
        gp_Sui->menu = MENU_MANUEL_BRUT ;

      break ;

      // ------------------------------ ARRET DU RESEAU ---------------------------------------
     
      case MENU_RESEAU_DOWN :
     
        Trace("appel : %d : MENU_RESEAU_DOWN" , gp_Sui->menu) ;

        if ( system("/etc/init.d/OLD/S40network stop")) {
          perror("Probleme avec system(/etc/init.d/OLD/S40network stop)"); 
          Trace("Probleme avec system(/etc/init.d/OLD/S40network stop)\n") ;
        }

        gp_Sui->menu = gp_Sui->menu_old ;
        gp_Sui->menu = MENU_MANUEL_BRUT ;

      break ;
      // ------------------------------ ARRET DU PROGRAMME ------------------------------------
      case MENU_PROGRAMME_DOWN :
     
        Trace("appel : %d : MENU_PROGRAMME_DOWN" , gp_Sui->menu) ;

        ASTRO_TRAP_MAIN(1) ;
        break ;
     
      // ------------------------------ ARRET DU SYSTEME EMBARQUE -----------------------------
      case MENU_DOWN :

        Trace("appel : %d : MENU_DOWN" , gp_Sui->menu) ;

        ASTRO_TRAP_MAIN(0) ;

      break ;
    }
    Trace("fin while") ;
  }
  Trace("Stop") ;

  return NULL ;
}
/*****************************************************************************************
* @fn     : SUIVI_VOUTE
* @author : s.gravois
* @brief  : Ce mode permet de gerer la voute c'est a dire le rafraichissement des calculs
* @param  : STRUCT_SUIVI   *gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_nb_threads++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : 
*****************************************************************************************/

/*
   SUIVI_VOUTE :
   le but de la fonction est de rafraichir a intervalles reguliers (1 seconde)
   tous les calculs relatifs a la vitesse de l'as suivi
*/

void * SUIVI_VOUTE(STRUCT_SUIVI * gp_Sui) {
  
  long long ll_inrc=0 ;
  char c_l0[16] ={0};
  char c_l1[16] ={0};

  unsigned long  incr ;
  struct timeval t00 ;
  struct sched_param param;

  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  sleep(2) ;
  /*
  char   c_Vitesses[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure] ;
  */
  // La temporisation dans la boucle du thread SUIVI_VOUTE depend de gp_Vou->DT (en us)
  // a completer / modifier :
  // il FAUT calculer / mettre a jour a,h,A,H de l'as quand est utliser les menus
  // suivi manuel et suivi capteurs , qui renvoient l'azimut et l'altitude
  // ==> CALCUL_EQUATEUR pour les devices altitude et azimut venant du capteur
  // ==> autre calcul plus complique quand on a les devices de vitesses et periodes provenant de la raquette !!!!!
  
  memset( c_l0, CALCUL_ZERO_CHAR, sizeof( c_l0 )) ;
  memset( c_l1, CALCUL_ZERO_CHAR, sizeof( c_l1 )) ;

  Trace2("sleeping..") ;
  usleep( PTHREADS_USLEEP_BEFORE_START_SUIVI_VOUTE ) ;
  Trace1("start") ;

  /* ---------------------------------------------------------------------------- */
  /* Configuration du thread et des attributs de tread                            */
  /* ---------------------------------------------------------------------------- */
/*
  if ( guit_user_getuid == PTHREADS_USERID_ROOT ) {
    param.sched_priority = PTHREADS_POLICY_1 ;

    if (pthread_setschedparam( pthread_self(), PTHREADS_SCHED_PARAM_SUIVI_VOUTE, & param) != 0) {
      perror("setschedparam SUIVI_VOUTE"); 
      exit(EXIT_FAILURE);
    }
  }
  else { 
    Trace("guit_user_getuid not PTHREADS_USERID_ROOT => cannot pthread_setschedparam(args)") ;
  }

  pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
  pthread_setname_np( pthread_self(), "SUIVI_VOUTE" ) ;
  gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
  pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;
*/
  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREADS_SUIVI_VOUTE ) ;

  signal( SIGTERM, ASTRO_TRAP_SUIVI_VOUTE) ;
  
  gp_Sui->d_temps = 0 ;
  incr =0 ;
  
  gettimeofday(&t00,NULL) ;
  
  VOUTE_CONFIG( gp_Vou, 1, 1, 0.985 ) ;
  
  TRACE1("gp_Sui->SUIVI_VOUTE=%d",gp_Sui->SUIVI_VOUTE) ;

  // FIXME : 
  // en mode equatorial, pas besoin de SUIVI_VOUTE, en effet la vitesse ne change pas
  // Va=15"/s et Vh=0 (le moteur en ascension droite est commande par azimut)
  
  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread SUIVI_VOUTE
  //-------------------------------------------------------------------------------
  
  gp_Sui->SUIVI_ALIGNEMENT=1 ;

  /* Debut boucle SUIVI_VOUTE */
  while(TRUE) {
    
    Trace("while") ;

    if ( gp_Sui->SUIVI_VOUTE ) {
      
      /* FIXME : modification 20121225 : tous les calculs generiques dans CALCUL_TOUT */
      
      /* CALCUL_TOUT() ; */
      /* Exceptionnellement , utilisation variables globales */ 
      /* LCD_DISPLAY_TEMPS_LIEU(0,gp_Lie,gp_Tim) ;*/

      if ( gp_Sui->SUIVI_ALIGNEMENT ) { 

        ASTRE_FORMATE_DONNEES_AFFICHAGE(gp_Ast) ;
        CONFIG_AFFICHER_MODE_LONG() ; 
        ASTRE_AFFICHER_MODE_STELLARIUM(gp_Ast) ;
        
        gp_Lcd->display_ast_vit(2000000) ;

        gp_Lcd->refresh_default() ;
        
        gp_Sui->SUIVI_ALIGNEMENT = 0 ;
      }
/*
      if ( gp_Sui->menu_old != gp_Sui->menu  ) {
        CONFIG_AFFICHER_TOUT() ;
      }
*/
      gp_Ast->AGH   += gp_Vou->pas ;
      gp_Vou->deb += gp_Vou->pas ;

      gp_Sui->d_temps += VOUTE_TEMPORISATION( gp_Vou, t00 ) ; 
      gettimeofday(&t00,NULL) ;

			gp_Vou->num ++ ;
      incr++ ;
      // attention cet appel systeme genere une interuption
      // uniquement utiliser pour les tests
      // system("/bin/date >> /root/astrokit.begin.log") ;
    }
    
    // TRES IMPORTANT !!
    // permet d eviter au thread de consommer toute la CPU
    // si jamais la condition SUIVI_VOUTE = 1 n est pas realisee
    
    else {
      // attention cet appel systeme genere une interuption
      // uniquement utiliser pour les tests
      // system("/bin/date >> /root/astrokit.begin.log") ;
      //TRACE("La voute ne tourne pas") ;
      usleep( gp_Vou->DT );
    }
  }
  Trace("Stop") ;

  return NULL ; 
}
/*****************************************************************************************
* @fn     : SUIVI_INFRAROUGE
* @author : s.gravois
* @brief  : fonction de callback du thread suivi infrarouge
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_nb_threads++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qi est variable globale
*****************************************************************************************/

void * SUIVI_INFRAROUGE(STRUCT_SUIVI * gp_Sui) {

  long long ll_inrc=0 ;
  int i_ret=0 ; 
  struct sched_param param;
    
  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  usleep( PTHREADS_USLEEP_BEFORE_START_SUIVI_INFRA ) ;
  Trace1("start") ;
  
  if ( gp_Dev->use_infrarouge ) {

    /* ---------------------------------------------------------------------------- */
    /* Configuration du thread et des attributs de tread                            */
    /* ---------------------------------------------------------------------------- */
/*
    if ( guit_user_getuid == PTHREADS_USERID_ROOT ) {
      param.sched_priority = PTHREADS_POLICY_1  ;
      
      if (pthread_setschedparam( pthread_self(), PTHREADS_SCHED_PARAM_SUIVI_INFRA, & param) != 0) { 
        perror("setschedparam SUIVI_INFRAROUGE"); 
        exit(EXIT_FAILURE);
      }
    }
    else { 
      Trace("guit_user_getuid not PTHREADS_USERID_ROOT => cannot pthread_setschedparam(args)") ;
    } 

    pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
    pthread_setname_np( pthread_self(), "SUIVI_IR" ) ;
    gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
    pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;
*/
    PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREADS_SUIVI_INFRA ) ;

    signal( SIGTERM, ASTRO_TRAP_SUIVI_INFRAROUGE) ;
  
    i_ret = INFRARED_OPEN( gp_LircConfig ) ;
    Trace("INFRARED_OPEN : retour = %d" , i_ret ) ;
    // ATTENTION !!! la fonction INFRARED_READ est bloquante (voir ir.c) !!!!!
    
    INFRARED_READ( gp_Sui ) ;
    INFRARED_CLOSE(gp_LircConfig) ;
  }

  Trace("Stop") ;

  return NULL ;
}
/*****************************************************************************************
* @fn     : SUIVI_LCD
* @author : s.gravois
* @brief  : fonction de callback du thread suivi ecran LCD
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-04-12 creation 
* @date   : 2022-04-27 mise en commentaire de LCD_DISPLAY_TEMPS_LIEU
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_nb_threads++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : TODO : reflechir a ce qui doit etre rafraichi
*****************************************************************************************/

void * SUIVI_LCD(STRUCT_SUIVI * gp_Sui) {

  long long ll_inrc=0 ;
  int i_duree_us ;
  struct sched_param param;
  char c_l0[16] ={0};
  char c_l1[16] ={0};
  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  memset( c_l0, CALCUL_ZERO_CHAR, sizeof( c_l0 )) ;
  memset( c_l1, CALCUL_ZERO_CHAR, sizeof( c_l1 )) ;
    
  Trace2("sleeping..") ;
  usleep( PTHREADS_USLEEP_BEFORE_START_SUIVI_LCD ) ;
  Trace1("start") ;
  
  if ( gp_Dev->use_lcd ) {

    /* ---------------------------------------------------------------------------- */
    /* Configuration du thread et des attributs de tread                            */
    /* ---------------------------------------------------------------------------- */
/*
    if ( guit_user_getuid == PTHREADS_USERID_ROOT ) {

      param.sched_priority = PTHREADS_POLICY_1  ; 

      if (pthread_setschedparam( pthread_self(), PTHREADS_SCHED_PARAM_SUIVI_LCD, & param) != 0) { 
        perror("setschedparam SUIVI_LCD"); 
        exit(EXIT_FAILURE);
      }
    }
    else { 
      Trace("guit_user_getuid not PTHREADS_USERID_ROOT => cannot pthread_setschedparam(args)") ;
    } 

    pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
    pthread_setname_np( pthread_self(), "SUIVI_LCD" ) ;
    gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
    pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;
*/
    PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREADS_SUIVI_LCD ) ;

    signal( SIGTERM, ASTRO_TRAP_SUIVI_LCD) ;

    gp_Lcd->display_default() ;

    /* Debut boucle SUIVI_LCD */
    while(TRUE) {
      
      Trace("while") ;

      usleep( gp_Sui->temporisation_lcd_loop );
      
      /* Si un changement de lignes a ete effectue dans une partie du programme */

      if ( gp_Lcd->i_change_current == TRUE ) {
       
        Trace("gp_Lcd->i_change_current == TRUE");
        /* on additionne la duree affichage de base et
           la duree d'affichage i_duree_us envoyee par les fcts  */
        
        usleep( gp_Lcd->i_duree_us );

        gp_Lcd->display_default() ;

        gp_Lcd->i_change_current = FALSE ;
      }
      else {
        Trace("gp_Lcd->i_change_current == TRUE");
        gp_Lcd->refresh_default() ;
      }
      gp_Sui->SUIVI_ALIGNEMENT = 0 ;
      
    }
  }

  Trace("Stop") ;

  return NULL ;
}
/*****************************************************************************************
* @fn     : SUIVI_CLAVIER_TERMIOS
* @author : s.gravois
* @brief  : fonction de callback du thread suivi clavier 
*           en mode termios
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-04-12 correction code ( repetition if ( i_indice_code < CODES_CODE_NB_CODES ))
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_nb_threads++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CLAVIER_TERMIOS( STRUCT_SUIVI * gp_Sui ) {

  long long ll_inrc=0 ;
  int i_indice_code=0 ;
  int c_char =0 ;
  int i_sum_ascii =0 ; 
  char c_sum_ascii[8] ;
  char ch_chaine[TERMIOS_KBHIT_SIZE_BUFFER_READ] ;
  struct sched_param param;
  struct timeval t00,t01  ;
  
  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  usleep( PTHREADS_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
  Trace1("start") ;
    
  if ( gp_Dev->use_keyboard ) {

    /* ---------------------------------------------------------------------------- */
    /* Configuration du thread et des attributs de tread                            */
    /* ---------------------------------------------------------------------------- */
/*
    if ( guit_user_getuid == PTHREADS_USERID_ROOT ) {

      param.sched_priority = PTHREADS_POLICY_1  ;
      if (pthread_setschedparam( pthread_self(), PTHREADS_SCHED_PARAM_SUIVI_CLAVIER, & param) != 0) { 
        perror("setschedparam SUIVI_CLAVIER_TERMIOS"); exit(EXIT_FAILURE);
      }
    }
    else { 
      Trace("guit_user_getuid not PTHREADS_USERID_ROOT => cannot pthread_setschedparam(args)") ;
    } 

    pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
    pthread_setname_np( pthread_self(), "SUIVI_TERMIOS" ) ;
    gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
    pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;
*/
    PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREADS_SUIVI_CLAVIER ) ;

    signal( SIGTERM, ASTRO_TRAP_SUIVI_CLAVIER) ;
  
    KEYBOARD_TERMIOS_INIT() ;

    /* Debut boucle SUIVI_CLAVIER_TERMIOS */
    while( c_char != 'q' ) {
      
      memset(&c_char,0,sizeof(c_char)) ;
      memset(ch_chaine, 0, sizeof(ch_chaine)) ;

      if ( KEYBOARD_TERMIOS_KBHIT_NEW(ch_chaine,&i_sum_ascii)) {

        c_char=ch_chaine[0] ;

        Trace1("keycode %d %s", i_sum_ascii, ch_chaine) ;

        if ( i_sum_ascii == 27 ) Trace("exit detecte %d", i_sum_ascii) ;
        memset(c_sum_ascii,0, sizeof(c_sum_ascii));
        sprintf(c_sum_ascii,"%d",i_sum_ascii);
        i_indice_code=0 ;
        while(strcmp(gp_Cod->in_term[i_indice_code],c_sum_ascii) && ( i_indice_code < CODES_CODE_NB_CODES ) ){ 
           Trace1("%s = %s ?", c_sum_ascii, gp_Cod->in_term[i_indice_code]) ;  
           i_indice_code++ ; 
        }
        pthread_mutex_lock(& gp_Mut->mut_dat );
        memset( gp_Sui->sui_dat.dat_inf, CONFIG_ZERO_CHAR, strlen( gp_Sui->sui_dat.dat_inf ) ) ;
        Trace(""); pthread_mutex_unlock(& gp_Mut->mut_dat );

        if ( i_indice_code < CODES_CODE_NB_CODES ) {
          pthread_mutex_lock(& gp_Mut->mut_dat );
          Trace2("A maj gp_Sui->sui_dat.dat_inf") ;
          strcpy( gp_Sui->sui_dat.dat_inf, gp_Cod->out_act[i_indice_code] ) ;
          Trace(""); pthread_mutex_unlock(& gp_Mut->mut_dat );

          GPIO_CLIGNOTE(gp_Gpi_Par_Pwm->par_led_etat, 1, 10) ;
        }
        // tres important !!
        // le usleep suivant permet de garder l information !!!!!!
        // gp_Sui->sui_dat.dat_inf fonctionne comme un TAMPON
        // il va etre lu par les threads du programme principal

        Trace(" -> datas lus = %s", gp_Sui->sui_dat.dat_inf ) ;
        Trace("gp_Sui->temporisation_termios = %ld", gp_Sui->temporisation_termios ) ;

        usleep( gp_Sui->temporisation_termios ) ;

        pthread_mutex_lock(& gp_Mut->mut_dat );
        memset( gp_Sui->sui_dat.dat_inf, 0, strlen( gp_Sui->sui_dat.dat_inf ) ) ;
        Trace("remise a zero gp_Sui->sui_dat.dat_inf") ;
        strcpy( gp_Sui->sui_dat.dat_inf, "") ;
        Trace(""); pthread_mutex_unlock(& gp_Mut->mut_dat );
      }
    }

    KEYBOARD_TERMIOS_EXIT() ;

    if ( c_char == 'q' ) {
      ASTRO_TRAP_MAIN(1) ;
    }
  }
  Trace("Stop") ;

  return NULL ;
}
/*****************************************************************************************
* @fn     : SUIVI_CLAVIER_getchar
* @author : s.gravois
* @brief  : fonction de callback du thread suivi clavier 
*           qui utilise directement getchar (aucun effet)
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_nb_threads++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CLAVIER_getchar( STRUCT_SUIVI * gp_Sui ) {

  long long ll_inrc=0 ;
  int c_char = 0 ;
  struct sched_param param;

  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  usleep( PTHREADS_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
  Trace1("start") ;

  if ( gp_Dev->use_keyboard ) {

    /* ---------------------------------------------------------------------------- */
    /* Configuration du thread et des attributs de tread                            */
    /* ---------------------------------------------------------------------------- */

    PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREADS_SUIVI_CLAVIER ) ;

    signal( SIGTERM, ASTRO_TRAP_SUIVI_CLAVIER) ;

    /* Debut boucle SUIVI_CLAVIER_getchar */
    while( ( c_char = getchar () ) != 'q' ) {
      usleep(100000) ;
      TRACE("%c %d entre au clavier", c_char,c_char  ) ; 
    }
    if ( c_char == 'q' ) {
      ASTRO_TRAP_MAIN(1) ;
    }
    /* appels LIRC pour memoire : 
    INFRARED_OPEN( gp_LircConfig ) ;
    INFRARED_READ( gp_Sui ) ;
    INFRARED_CLOSE(gp_LircConfig) ;*/
  }
  Trace("Stop") ;

  return NULL ;
}

/*****************************************************************************************
* @fn     : SUIVI_CLAVIER_NCURSES
* @author : s.gravois
* @brief  : fonction de callback du thread suivi clavier en mode ncurses
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_nb_threads++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CLAVIER_NCURSES(STRUCT_SUIVI * gp_Sui ) {

  long long ll_inrc=0 ;
  int ch = 0 ;
  unsigned long l_incr=0 ;
  struct sched_param param;

  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  usleep( PTHREADS_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
  Trace1("start") ;
  
  if ( gp_Dev->use_keyboard ) {

    /* ---------------------------------------------------------------------------- */
    /* Configuration du thread et des attributs de tread                            */
    /* ---------------------------------------------------------------------------- */

    PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREADS_SUIVI_CLAVIER ) ;

    signal( SIGTERM, ASTRO_TRAP_SUIVI_CLAVIER) ;
    
    sleep(1) ;

    initscr() ;
    if (newterm(0, stdout, stdin) == 0) {
    fprintf(stderr, "Cannot initialize terminal\n");
    TRACE("Cannot initialize terminal") ;
    exit(2);
    }
    cbreak();		    
    echo();		     
    scrollok(stdscr, TRUE);
    move(0,0);
    printw("Fin initialisation\n");
    refresh();
    l_incr=0;

    Trace2("debut while") ;

    /* Debut boucle SUIVI_CLAVIER_ncurses */
    while ((ch = getch())) {
      l_incr++;
      //refresh();
      if ( ch != ERR ) {
        printw("%-5d Keycode %d\n",l_incr,ch);
        
      }
      //refresh();
      // usleep( 100000 );
    }

    sleep(2) ;
    printw("appel endwin") ; sleep(1) ;
    endwin();
    ASTRO_TRAP_MAIN(1) ;
  }
  Trace("Stop") ;

  return NULL ;
}

/*****************************************************************************************
* @fn     : SUIVI_CAPTEURS
* @author : s.gravois
* @brief  : fonction de callback du thread suivi capteurs (non utilisee)
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_nb_threads++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CAPTEURS(STRUCT_SUIVI * gp_Sui) {
   
  long long ll_inrc=0 ;
  struct sched_param param;
  int ret ;
  
  STRUCT_I2C_DEVICE   exemple, *ex ;
  STRUCT_I2C_ACC_MAG  accmag,  *am ;

  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  usleep( PTHREADS_USLEEP_BEFORE_START_SUIVI_CAPTEUR ) ;
  Trace1("start") ;

  ex = & exemple ;
  am = & accmag ;
  ex->fd = 0 ;
  
  if ( gp_Dev->use_capteurs ) {
      
    // La temporisation dans la boucle du thread SUIVI_CAPTEURS depend de gp_Sui->temporisation_capteurs (en us)
    // present dans la fonction bloquante INFRARED_READ 
    
    // on laisse le temps aux autres threads de demarrer
    // notamment pour arriver dans la fonction SUIVI_TRAITEMENT_MENU
    // qui initialise a 1 SUIVI_CAPTEURS

    /* ---------------------------------------------------------------------------- */
    /* Configuration du thread et des attributs de tread                            */
    /* ---------------------------------------------------------------------------- */

    PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREADS_SUIVI_CAPTEUR ) ;

    signal( SIGTERM, ASTRO_TRAP_SUIVI_CAPTEURS) ;
    
    // a modifier pour definir le choix de l'infrarouge ou pas (config.txt ? .h ? )
    
    /* Debut boucle SUIVI_CAPTEURS */
    while(TRUE) {
      
      Trace("while") ;

      if ( ! gp_Dev->init_capteurs ) {
        
        ret = I2C_INIT(ex, DEVICE_RASPI_2, DEVICE_LSM_ADRESS ) ;

        if ( ! ret ) {
          Trace("Pas de capteur disponible") ;
          gp_Dev->use_capteurs = 0 ;
          gp_Dev->init_capteurs = 0 ;
          break ;
        }
        else {
          I2C_SET( ex, REG_CTRL1, "0x57" ) ;
          I2C_SET( ex, REG_CTRL2, "0x00" ) ;
          I2C_SET( ex, REG_CTRL5, "0x64" ) ;
          I2C_SET( ex, REG_CTRL6, "0x20" ) ;
          I2C_SET( ex, REG_CTRL7, "0x00" ) ;
    
          gp_Dev->init_capteurs = 1 ;
        }
      }
      if ( gp_Dev->init_capteurs ) {
      
        I2C_GET_ACC( ex, am )   ;
        I2C_CALCULS_ACCMAG( am ) ;
      
        gp_Sui->roll    =  am->roll  ; 
        gp_Sui->pitch   =  am->pitch ;
        gp_Sui->heading =  am->heading ;
      
        TRACE1("%.0f\t%.0f\t%.0f\n", am->pitch * I2C_DEGRAD, am->roll * I2C_DEGRAD, am->heading * I2C_DEGRAD) ;
      }	
      
      usleep( gp_Sui->temporisation_capteurs );
    }
  }
  Trace("Stop") ;

  return NULL ;
}
/*****************************************************************************************
* @fn     : main
* @author : s.gravois
* @brief  : point entree su programme
* @param  : int     argc
* @param  : char ** argv
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : revoir ordre appels des fonctions
*****************************************************************************************/

/* TODO : decommenter pour pouvoir compiler modules a part 
  (deplacement fonctions main ailleurs que dans astro.c )
  int mainAstro(int argc, char ** argv) {
*/
int main(int argc, char ** argv) {
  
  int i ;
  int devFD, board;
  struct sched_param param;

  pthread_t p_thread_pha_alt[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  pthread_t p_thread_pha_azi[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  pthread_t p_thread_mot_alt ;
  pthread_t p_thread_mot_azi ;
/*
  STRUCT_GPIO_PWM_MOTEUR *gp_Mot_Alt , g_mot_alt ; 
  STRUCT_GPIO_PWM_MOTEUR *gp_Mot_Azi , g_mot_azi ;
*/
  
  ARBO(__func__,0 , "") ;

  int i2cDev = 1;

  /* Attention, pas de TRACE avant d avoir ouvert le fichier de log */ 
  /* car TRACE utilise ce fichier */
  /* Alternative : Trace */ 

  ASTRO_GLOBAL_INIT() ;


  ARGUMENTS_GERER_REP_HOME(argc, argv) ;
  /* Pour permettre acces a STRUCT_SUIVI* via struct STRUCT_GPIO_PWM_MOTEUR* */

  CONFIG_PATH_FIND( g_Path_Cmd_Stty, "stty") ;
  
  // -----------------------------------------------------------------
  // Initialisations diverses et variees
  // -----------------------------------------------------------------

  CONFIG_READ       ( g_Char_Params ) ;
  GPIO_READ         ( g_Char_Params ) ; 

  CONFIG_AFFICHER_DATAS ( g_Char_Params ) ;
  CONFIG_PARAMETRES_CONFIG       ( g_Char_Params ) ;

  // CONFIG_PARAMETRES_AFFICHER() ;   
  LOG_INIT(); 

  /* GPIO_CLAVIER_MATRICIEL_CONFIG( gpio_key_l, gpio_key_c ) ; */
  
  VOUTE_INIT     ( gp_Vou ) ; /* soit etre place avant SUIVI_INIT */
  CODES_INIT     ( gp_Cod ) ;
  KEYS_INIT      ( gp_Key ) ;   
  ASTRE_INIT     ( gp_Ast ) ;
  LIEU_INIT      ( gp_Lie  ) ;
  TEMPS_INIT     ( gp_Tim ) ;

  TEMPS_CALCUL_TEMPS_SIDERAL  ( gp_Lie, gp_Tim) ;
  
  DEVICES_INIT   ( gp_Dev ) ;
  SUIVI_INIT     ( gp_Sui ) ;


  LCD_INIT              ( gp_Lcd ) ;
  PID_INIT              ( gp_Pid, gp_Pid_Par->par_pid_ech, gp_Pid_Par->par_pid_kp, gp_Pid_Par->par_pid_ki, gp_Pid_Par->par_pid_kd ) ;

  DEVICES_AFFICHER_UTILISATION() ;

  Trace("pthread_self = %ld", pthread_self()) ;

  /* ---------------------------------------------------------------------------- */
  /* Initialisation des strcutures necessaires aux attributs de tread             */
  /* ---------------------------------------------------------------------------- */

  PTHREADS_INIT         ( pthread_self()) ;

  /* ---------------------------------------------------------------------------- */
  /* Configuration du thread et des attributs de tread du MAIN                    */
  /* ---------------------------------------------------------------------------- */

  PTHREADS_CONFIG       ( gp_Pth, pthread_self(), PTHREADS_SUIVI_MAIN  ) ; 
  /* pthread init a mettre ici */

  signal(SIGINT,ASTRO_TRAP_MAIN) ;
  signal(SIGALRM,ASTRO_TRAP_MAIN) ;

  Trace("gi_gpio_alt         : %d %d %d %d", gi_gpio_alt[0], gi_gpio_alt[1], gi_gpio_alt[2], gi_gpio_alt[3] ) ;
  Trace("gi_gpio_azi         : %d %d %d %d", gi_gpio_azi[0], gi_gpio_azi[1], gi_gpio_azi[2], gi_gpio_azi[3] ) ;
  Trace("gi_gpio_mas         : %d %d %d %d", gi_gpio_mas[0], gi_gpio_mas[1], gi_gpio_mas[2], gi_gpio_mas[3] ) ;
  Trace("gp_Gpi_Par_Pwm->par_led_etat    : %d", gp_Gpi_Par_Pwm->par_led_etat );
  Trace("gp_Ast_Par->par_default_object : %s", gp_Ast_Par->par_default_object) ;
  
  Trace("gp_Pid_Par->par_pid_ech = %f",  gp_Pid_Par->par_pid_ech);
  
  // -----------------------------------------------------------------
  // reglages variables particulieres
  // ----------------------------------------------------------------- 
  /* TODO : fusionner g_c_cat et g_c_cat_eto */
  CAT_READ            ( CAT_NGC_TXT, g_c_cat) ;                     
  /* CAT_AFFICHER ( g_c_cat ) ;  */
  CAT_FORMAT_DECIMAL_NGC  ( "ngc.csv.dec" , g_c_cat, g_c_cat_dec) ;         
  /* CAT_AFFICHER ( g_c_cat_dec ) ; */

  CAT_READ            ( CAT_ETO_TXT, g_c_cat_eto) ;                 // CAT_AFFICHER ( g_c_cat_eto ) ;
  CAT_FORMAT_DECIMAL_ETO( "eto.csv.dec" , g_c_cat_eto, g_c_cat_eto_dec) ; // CAT_AFFICHER ( g_c_cat_eto_dec ) ;
  // -----------------------------------------------------------------
  
  if ( strcmp(gp_Ast_Par->par_default_object,"") != 0 ) {

    memset( gp_Ast->nom, 0, sizeof(gp_Ast->nom)) ;
    strcpy( gp_Ast->nom, gp_Ast_Par->par_default_object ) ;

    if ( strstr( gp_Ast->nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_dec) ;
    if ( strstr( gp_Ast->nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_dec) ;
    if ( strstr( gp_Ast->nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_eto_dec) ;

    gp_Sui->SUIVI_ALIGNEMENT = 1 ;
  }

  // -----------------------------------------------------------------

  ARGUMENTS_HELP                  ( argc, argv ) ;
  ARGUMENTS_GERER_FACON_CLASSIQUE ( argc, argv  ) ;
  
  if ( gp_Sui->alarme != 0 ) {
    alarm( gp_Sui->alarme) ;
  }  

  // ouverture led etat ----------------------------------------------

  if ( gp_Gpi_Par_Pwm->par_led_etat != 0 ) {

    GPIO_CLOSE_BROCHE( gp_Gpi_Par_Pwm->par_led_etat) ;
    GPIO_OPEN_BROCHE( gp_Gpi_Par_Pwm->par_led_etat, 1) ;
    GPIO_SET( gp_Gpi_Par_Pwm->par_led_etat, 0 ) ;
  }
  
  // -----------------------------------------------------------------
  
  GPIO_INIT_PWM_MOTEUR(\
    gp_Mot_Alt,\
    gi_gpio_alt,\
    gi_gpio_mas,\
    gp_Cal_Par->par_alt_red_4 ,\
    100,\
    gd_gpio_frequence_pwm, \
    0, \
    GPIO_CURRENT_FONCTION,\
    GPIO_CURRENT_FONCTION_PARAM0,\
    GPIO_CURRENT_FONCTION_PARAM1 ) ;

  GPIO_INIT_PWM_MOTEUR(\
    gp_Mot_Azi,\
    gi_gpio_azi,\
    gi_gpio_mas,\
    gp_Cal_Par->par_azi_red4 ,\
    100,\
    gd_gpio_frequence_pwm, \
    1, \
    GPIO_CURRENT_FONCTION,\
    GPIO_CURRENT_FONCTION_PARAM0,\
    GPIO_CURRENT_FONCTION_PARAM1 ) ;
  
  Trace("gp_Mot_Azi->Fm %f gp_Mot_Azi->periode_mot = %f", gp_Mot_Azi->Fm, gp_Mot_Azi->periode_mot ) ;
  Trace("gp_Mot_Alt->Fm %f gp_Mot_Alt->periode_mot = %f", gp_Mot_Alt->Fm, gp_Mot_Alt->periode_mot ) ;

  for( i=0 ; i< gp_Cal_Par->par_alt_red_4 ; i++ ) {
    TRACE1("%d\t%f\t%f\t%f\t%f",i,\
      gp_Mot_Azi->p_pha[0]->rap[i],\
      gp_Mot_Azi->p_pha[1]->rap[i],\
      gp_Mot_Azi->p_pha[2]->rap[i],\
      gp_Mot_Azi->p_pha[3]->rap[i]) ;
  }

  // ============================== gestion des threads  ===================================

  pthread_create( &p_thread_pha_azi[0],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Mot_Azi->p_pha[0] ) ;
  pthread_create( &p_thread_pha_azi[1],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Mot_Azi->p_pha[1] ) ;
  pthread_create( &p_thread_pha_azi[2],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Mot_Azi->p_pha[2] ) ;
  pthread_create( &p_thread_pha_azi[3],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Mot_Azi->p_pha[3] ) ;

  pthread_create( &p_thread_pha_alt[0],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Mot_Alt->p_pha[0] ) ;
  pthread_create( &p_thread_pha_alt[1],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Mot_Alt->p_pha[1] ) ;
  pthread_create( &p_thread_pha_alt[2],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Mot_Alt->p_pha[2] ) ;
  pthread_create( &p_thread_pha_alt[3],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Mot_Alt->p_pha[3] ) ;

  pthread_create( &p_thread_mot_azi,           NULL, (void*)suivi_main_M, gp_Mot_Azi ) ;
  pthread_create( &p_thread_mot_alt,           NULL, (void*)suivi_main_M, gp_Mot_Alt ) ;

  pthread_create( &gp_Pth->pth_men, NULL, (void*)SUIVI_MENU,      gp_Sui ) ;
  pthread_create( &gp_Pth->pth_vou, NULL, (void*)SUIVI_VOUTE,     gp_Sui ) ;
  pthread_create( &gp_Pth->pth_inf, NULL, (void*)SUIVI_INFRAROUGE, gp_Sui ) ;
  pthread_create( &gp_Pth->pth_cap, NULL, (void*)SUIVI_CAPTEURS,  gp_Sui ) ;
  pthread_create( &gp_Pth->pth_cla, NULL, (void*)SUIVI_CLAVIER_TERMIOS,  gp_Sui ) ;
  pthread_create( &gp_Pth->pth_lcd, NULL, (void*)SUIVI_LCD,  gp_Sui ) ;

  /* Affichage d'informations sur les threads lancés */

  PTHREADS_INFOS(gp_Pth) ;

  // ============================== join des threads  ===================================

  if ( gp_Dev->use_lcd )         pthread_join( gp_Pth->pth_lcd, NULL) ;
  if ( gp_Dev->use_keyboard )    pthread_join( gp_Pth->pth_cla, NULL) ;
  if ( gp_Dev->use_capteurs )    pthread_join( gp_Pth->pth_cap, NULL) ;
  if ( gp_Dev->use_infrarouge )  pthread_join( gp_Pth->pth_inf, NULL) ;

  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_join( p_thread_pha_azi[i], NULL) ; 
  }
  pthread_join( p_thread_mot_azi, NULL) ;
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_join( p_thread_pha_alt[i], NULL) ; 
  }
  pthread_join( p_thread_mot_alt, NULL) ;
  
  pthread_join( gp_Pth->pth_men, NULL) ;
  pthread_join( gp_Pth->pth_vou, NULL) ;  

  return 0;
}
// #######################################################################################
// ####### FIN PROGRAMME - FIN PROGRAMME - FIN PROGRAMME - FIN PROGRAMME         - #######
// #######################################################################################
