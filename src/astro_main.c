#define _GNU_SOURCE
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
MACRO_ASTRO_GLOBAL_EXTERN_PTHREADS ;
MACRO_ASTRO_GLOBAL_EXTERN_CONFIG ;
MACRO_ASTRO_GLOBAL_EXTERN_INFRARED ;

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

  int l_nb_threads = gi_pth_numero ;
  int i_num_thread=0 ;
  long id_thread =0;
  char c_thread_name [ 16 ] ; 
  char c_cmd[ 64 ] ;
  int i ;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1("Signal trappe de valeur (sig) %d",sig) ;
  Trace1("Envoi d'un signal %d (SIGTERM) aux threads",SIGTERM) ;
  
  GPIO_CLIGNOTE(gp_Gpi_Par_Pwm->par_led_etat, 1, 100) ;

  //Trace1("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;

  /*--------------------------------------------------------*/
  /* Si sig > positif on abandonne les threads et on quitte */
  /*--------------------------------------------------------*/

  Trace("pthread_cancel : %d threads a abandonner", gi_pth_numero ) ;

  for( i_num_thread = PTHREADS_MAX_THREADS-1 ; i_num_thread >=0  ; i_num_thread-- )   {

    PTHREADS_CANCEL_OR_KILL ( gp_Pth ) ;
  }
  Trace("fin des abandons de threads") ;
  
  /*----------------------------------*/
  /* Rendre le terminal propre (sane) */
  /*----------------------------------*/

  memset( c_cmd, 0, sizeof(c_cmd)) ;
  sprintf(c_cmd,"%s sane", gc_config_path_cmd_stty ) ;

  if ( system( c_cmd ) < 0 ) {

    SyslogErr("Probleme avec stty sane\n") ;
    Trace1("Probleme avec stty sane") ;

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
      Trace1("Probleme avec /sbin/halt") ;

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
* @todo   : (obsolete) (appel 2 fois ASTRO_TRAP_MAIN = contre productif)
*****************************************************************************************/
/*
void ASTRO_TRAP_SUIVI_MENU(int sig)  {
  
  TraceArbo(__func__,0,"--------------") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_menu = %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_MENU] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_VOUTE(int sig)  {
  
  TraceArbo(__func__,0,"--------------") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_voute= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_VOUTE] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_INFRAROUGE(int sig)  {
  TraceArbo(__func__,0,"--------------") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_infrarouge= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_INFRA] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_CAPTEURS(int sig)  {
  TraceArbo(__func__,0,"--------------") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_capteurs= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_CAPTEUR] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_CLAVIER(int sig)  {
  TraceArbo(__func__,0,"--------------") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_clavier= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_CLAVIER] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_LCD(int sig)  {
  TraceArbo(__func__,0,"--------------") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_termios= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_LCD] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
*/
/*****************************************************************************************
* @fn     : SUIVI_MENU
* @author : s.gravois
* @brief  : Ce mode permet de gerer les menus .
* @param  : STRUCT_SUIVI   *gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : 
*****************************************************************************************/

void * SUIVI_MENU(STRUCT_SUIVI * gp_Sui) {
  
  long long ll_inrc=0 ;
  struct sched_param param;
  void * (*ptr_fct)(STRUCT_SUIVI *)  = SUIVI_MENU ;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL) ;
  pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL ) ;  
  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_MENU ) ;
   
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_MENU ) ;
  
  SUIVI_MENU_BEFORE_WHILE ( gp_Sui) ;

  gp_Sui->reset = 1 ;

  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread SUIVI_MENU
  //-------------------------------------------------------------------------------
  
  /* Debut boucle SUIVI_MENU */
  
  sleep(1 );

  while(TRUE) {

    Trace1("temporisation %ld", gp_Sui->sui_tpo->tempo_menu ) ;
    usleep( gp_Sui->sui_tpo->tempo_menu ) ;

    /* GPIO_CLAVIER_MATRICIEL_READ( gpio_key_l, gpio_key_c, gp_Key) ; */
    KEYS_INPUTS_GESTION_APPUIS( gp_Sui, gp_Key) ;

    /* SUIVI_OLD_0( gp_Sui) ; */
    SUIVI_TRAITEMENT_MOT( gp_Sui, gp_Key ) ;

    CONFIG_MENU_CHANGE_DETECT() ;

    KEYS_AFFICHER( gp_Key ) ;	


    switch( gp_Sui->menu ) {

      // ------------------------------- ALIGNEMENT - MODE AZIMUTAL ----------------------------
     
      case MENU_AZIMUTAL :
        
        Trace1("MENU_AZIMUTAL") ;
        
        // a modifier / completer : TEMPS_CALCUL_TEMPS_SIDERAL et CALCUL_ANGLE_HORAIRE
        // sont a supprimer car deja calculer dans SUIVI_

        Trace1("appel : %d : MENU_AZIMUTAL" , gp_Sui->menu) ;

        pthread_mutex_lock(& gp_Mut->mut_glo_azi );   
        pthread_mutex_lock(& gp_Mut->mut_glo_alt );

        gp_Sui->sui_pas->pas_acc_alt          = 1 ;
        gp_Sui->sui_pas->pas_acc_azi          = 1 ;

        pthread_mutex_unlock(& gp_Mut->mut_glo_azi );   
        pthread_mutex_unlock(& gp_Mut->mut_glo_alt );

        gp_Sui->sui_mode_equatorial = 0 ;
        gp_Sui->sui_mode_voute      = 1 ; 

        CALCUL_ASTRE_RECUP_TYPE_ET_NOM() ;

				CALCUL_TOUT() ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ; 

      break ;

      // ------------------------------  MODE EQUATORIAL --------------------------------------
     
      case MENU_EQUATORIAL :
        
        Trace1("MENU_EQUATORIAL") ;
        
        pthread_mutex_lock(& gp_Mut->mut_glo_azi );   
        pthread_mutex_lock(& gp_Mut->mut_glo_alt );

        gp_Sui->sui_pas->pas_acc_alt          = 0 ;
        gp_Sui->sui_pas->pas_acc_azi          = 1 ;

        pthread_mutex_unlock(& gp_Mut->mut_glo_azi );   
        pthread_mutex_unlock(& gp_Mut->mut_glo_alt );

        gp_Sui->sui_mode_equatorial = 1 ;
        gp_Sui->sui_mode_voute      = 0 ; 

        CALCUL_ASTRE_RECUP_TYPE_ET_NOM() ;

				CALCUL_TOUT() ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ;

      break ;

      // -------------------------- STRUCT_SUIVI MANUEL PAR DEFAUT ------------------
      
      case MENU_MANUEL_BRUT :
        
        Trace1("MENU_MANUEL_BRUT") ;
        
        // le but de ce suivi est de deduire des actions N-S-O-E de l'utilisateur 
        // simplement une acceleration / ralentissement / changement de direction
        // sur le N-S-E-O (pas de recalcul des periodes) 
        // FIXME : les periodes sont conservees , ainsi que le mode azimutal ou equatorial

        if( strcmp( gp_Sui->sui_dat->dat_inf, "") != 0 ) {
          Trace1("1 gp_Sui->sui_dat->dat_inf = %s", gp_Sui->sui_dat->dat_inf ) ;
        }

        SUIVI_MANUEL_BRUT(gp_Sui, gp_Key) ;
  
        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ; 

      break ;

      /* -------------------------- MENU_MANUEL_NON_ASSERVI : LE PLUS SIMPLE ------------------
          TODO : a coder : juste appui des touches provoque le mouvement
          TODO : mode le plus simple a priori
          TODO : acc_azi_ et pas_acc_alt doivent etre initialises a zero
      */
      
      case MENU_MANUEL_NON_ASSERVI :
        
        Trace1("MENU_MANUEL_NON_ASSERVI") ;
        
        // Suivi le plus simple : seules les touches est nord sud ouest et reset sont prises en compte
        // TODO : verifier

        Trace1("appel : %d : MENU_MANUEL_NON_ASSERVI" , gp_Sui->menu) ;

        pthread_mutex_lock(& gp_Mut->mut_glo_azi );   
        pthread_mutex_lock(& gp_Mut->mut_glo_alt );

        gp_Sui->sui_pas->pas_acc_alt          = 0 ;
        gp_Sui->sui_pas->pas_acc_azi          = 0 ;

        pthread_mutex_unlock(& gp_Mut->mut_glo_azi );   
        pthread_mutex_unlock(& gp_Mut->mut_glo_alt );

        CALCUL_ASTRE_RECUP_TYPE_ET_NOM() ;

				CALCUL_TOUT() ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_NON_ASSERVI ; 

      break ;

      // -------------------------- STRUCT_SUIVI MANUEL AVEC RE-CALCUL DES PERIODES  -----------------
      case MENU_MANUEL_ASSERVI :

        Trace1("MENU_MANUEL_ASSERVI") ;       

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

        Trace1("appel : %d : MENU_GOTO" , gp_Sui->menu) ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ;

      break ; 

      // ------------------------------  MODE INFO --------------------------------------

      case MENU_INFO :

        Trace1("appel : %d : MENU_INFO" , gp_Sui->menu) ;

        CONFIG_AFFICHER_TOUT() ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ;

      break ; 

      // ------------------------------ MODE ACTIVATION DU RESEAU ----------------------------------
     
      case MENU_RESEAU_UP :

        Trace1("appel : %d : MENU_RESEAU_UP" , gp_Sui->menu) ;

        if ( system("/etc/init.d/OLD/S40network start")) {
         perror("Probleme avec system(/etc/init.d/OLD/S40network start)"); 
         Trace1("Probleme avec system(/etc/init.d/OLD/S40network start)\n") ;
        }

        gp_Sui->menu = gp_Sui->menu_old ;
        gp_Sui->menu = MENU_MANUEL_BRUT ;

      break ;

      // ------------------------------ ARRET DU RESEAU ---------------------------------------
     
      case MENU_RESEAU_DOWN :
     
        Trace1("appel : %d : MENU_RESEAU_DOWN" , gp_Sui->menu) ;

        if ( system("/etc/init.d/OLD/S40network stop")) {
          perror("Probleme avec system(/etc/init.d/OLD/S40network stop)"); 
          Trace1("Probleme avec system(/etc/init.d/OLD/S40network stop)\n") ;
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
    Trace1("fin while") ;
  }
  Trace1("Stop") ;

  return NULL ;
}

/*****************************************************************************************
* @fn     : SUIVI_VOUTE
* @author : s.gravois
* @brief  : Ce mode permet de gerer la voute c'est a dire le rafraichissement des calculs
* @param  : STRUCT_SUIVI   *gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
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

  unsigned long  ul_vou_while_incr ;
  struct timeval t00 ;
  struct sched_param param;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL) ;
  pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL ) ;  
  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_VOUTE ) ;

  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_VOUTE ) ;

  sleep(1) ;
  
  memset( c_l0, CALCUL_ZERO_CHAR, sizeof( c_l0 )) ;
  memset( c_l1, CALCUL_ZERO_CHAR, sizeof( c_l1 )) ;
  
  ul_vou_while_incr =0 ;
  
  gettimeofday(&t00,NULL) ;
  
  VOUTE_CONFIG( gp_Vou, 1, 1, 0.985 ) ;
  
  // FIXME : 
  // en mode equatorial, pas besoin de SUIVI_VOUTE, en effet la vitesse ne change pas
  // Va=15"/s et Vh=0 (le moteur en ascension droite est commande par azimut)
  
  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread SUIVI_VOUTE
  //-------------------------------------------------------------------------------
  
  gp_Ast->ast_new = TRUE ;

  /* Debut boucle SUIVI_VOUTE */
  while(TRUE) {
    
    Trace1("while") ;

    if ( gp_Sui->sui_mode_voute ) {
      
      /* FIXME : modification 20121225 : tous les calculs generiques dans CALCUL_TOUT */
      
      CALCUL_TOUT() ;
      
      /* Exceptionnellement , utilisation variables globales */ 
      /* LCD_DISPLAY_TEMPS_LIEU(0,gp_Lie,gp_Tim) ;*/

      if ( gp_Ast->ast_new ) { 

        ASTRE_FORMATE_DONNEES_AFFICHAGE(gp_Ast) ;
        CONFIG_AFFICHER_MODE_LONG() ; 
        ASTRE_AFFICHER_MODE_STELLARIUM(gp_Ast) ;
        
        gp_Lcd->display_ast_vit(2000000) ;

        gp_Lcd->refresh_default() ;
        
        gp_Ast->ast_new = FALSE ;
      }
/*
      if ( gp_Sui->menu_old != gp_Sui->menu  ) {
        CONFIG_AFFICHER_TOUT() ;
      }
*/
      gp_Ast->AGH += gp_Vou->vou_pas ;
      gp_Vou->vou_begin += gp_Vou->vou_pas ;

      gp_Vou->vou_temps_ecoule += VOUTE_TEMPORISATION( gp_Vou, t00 ) ; 
      gettimeofday(&t00,NULL) ;

			gp_Vou->vou_num ++ ;
      ul_vou_while_incr++ ;
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
      //Trace1("La voute ne tourne pas") ;
      usleep( gp_Vou->vou_dt_us );
    }
  }
  Trace1("Stop") ;

  return NULL ; 
}
/*****************************************************************************************
* @fn     : SUIVI_INFRAROUGE
* @author : s.gravois
* @brief  : fonction de callback du thread suivi infrarouge
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qi est variable globale
*****************************************************************************************/

void * SUIVI_INFRAROUGE(STRUCT_SUIVI * gp_Sui) {

  long long ll_inrc=0 ;
  int i_ret=0 ; 
  struct sched_param param;
    
  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL) ;
  pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL ) ;
  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_INFRARED ) ;
  
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_INFRA ) ;
  
  if ( gp_Dev->use_infrarouge ) {
  
    i_ret = INFRARED_OPEN( gp_LircConfig ) ;
    Trace1("INFRARED_OPEN : retour = %d" , i_ret ) ;
    // ATTENTION !!! la fonction INFRARED_READ est bloquante (voir ir.c) !!!!!
    
    INFRARED_READ( gp_Sui ) ;
    INFRARED_CLOSE(gp_LircConfig) ;
  }

  Trace1("Stop") ;

  return NULL ;
}
/*****************************************************************************************
* @fn     : SUIVI_LCD
* @author : s.gravois
* @brief  : fonction de callback du thread suivi ecran LCD
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-04-12 creation 
* @date   : 2022-04-27 mise en commentaire de LCD_DISPLAY_TEMPS_LIEU
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : TODO : reflechir a ce qui doit etre rafraichi
*****************************************************************************************/

void * SUIVI_LCD(STRUCT_SUIVI * gp_Sui) {

  long long ll_inrc=0 ;
  int i_duree_us ;
  struct sched_param param;
  char c_l0[16] ={0};
  char c_l1[16] ={0};

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL) ;
  pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL ) ;  
  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_LCD ) ;

  memset( c_l0, CALCUL_ZERO_CHAR, sizeof( c_l0 )) ;
  memset( c_l1, CALCUL_ZERO_CHAR, sizeof( c_l1 )) ;
    
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_LCD ) ;
  
  if ( gp_Dev->use_lcd ) {

    gp_Lcd->display_default() ;

    /* Debut boucle SUIVI_LCD */
    while(TRUE) {
      
      Trace1("while") ;

      usleep( gp_Sui->sui_tpo->tempo_lcd_loop );
      
      /* Si un changement de lignes a ete effectue dans une partie du programme */

      if ( gp_Lcd->i_change_current == TRUE ) {
       
        Trace1("gp_Lcd->i_change_current == TRUE");
        /* on additionne la duree affichage de base et
           la duree d'affichage i_duree_us envoyee par les fcts  */
        
        usleep( gp_Lcd->i_duree_us );

        gp_Lcd->display_default() ;

        gp_Lcd->i_change_current = FALSE ;
      }
      else {
        Trace1("gp_Lcd->i_change_current == TRUE");
        gp_Lcd->refresh_default() ;
      }
      gp_Ast->ast_new = FALSE ;
      
    }
  }

  Trace1("Stop") ;

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
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
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
  
  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL) ;
  pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL ) ;
  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_CLAVIER ) ;

  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
    
  if ( gp_Dev->use_keyboard ) {
  
    KEYBOARD_TERMIOS_INIT() ;

    /* Debut boucle SUIVI_CLAVIER_TERMIOS */
    while( c_char != 'q' ) {
      
      memset(&c_char,0,sizeof(c_char)) ;
      memset(ch_chaine, 0, sizeof(ch_chaine)) ;

      if ( KEYBOARD_TERMIOS_KBHIT_NEW(ch_chaine,&i_sum_ascii)) {

        c_char=ch_chaine[0] ;

        Trace2("keycode %d %s", i_sum_ascii, ch_chaine) ;

        if ( i_sum_ascii == 27 ) Trace1("exit detecte %d", i_sum_ascii) ;
        memset(c_sum_ascii,0, sizeof(c_sum_ascii));
        sprintf(c_sum_ascii,"%d",i_sum_ascii);
        i_indice_code=0 ;
        while(strcmp(gp_Cod->in_term[i_indice_code],c_sum_ascii) && ( i_indice_code < CODES_CODE_NB_CODES ) ){ 
           Trace2("%s = %s ?", c_sum_ascii, gp_Cod->in_term[i_indice_code]) ;  
           i_indice_code++ ; 
        }
        pthread_mutex_lock(& gp_Mut->mut_dat );
        memset( gp_Sui->sui_dat->dat_inf, CONFIG_ZERO_CHAR, strlen( gp_Sui->sui_dat->dat_inf ) ) ;
        pthread_mutex_unlock(& gp_Mut->mut_dat );

        if ( i_indice_code < CODES_CODE_NB_CODES ) {
          pthread_mutex_lock(& gp_Mut->mut_dat );
          strcpy( gp_Sui->sui_dat->dat_inf, gp_Cod->out_act[i_indice_code] ) ;
          pthread_mutex_unlock(& gp_Mut->mut_dat );

          GPIO_CLIGNOTE(gp_Gpi_Par_Pwm->par_led_etat, 1, 10) ;
        }
        // tres important !!
        // le usleep suivant permet de garder l information !!!!!!
        // gp_Sui->sui_dat->dat_inf fonctionne comme un TAMPON
        // il va etre lu par les threads du programme principal

        Trace1(" -> datas lus = %s", gp_Sui->sui_dat->dat_inf ) ;
        Trace1("gp_Sui->sui_tpo->tempo_termios = %ld", gp_Sui->sui_tpo->tempo_termios ) ;

        usleep( gp_Sui->sui_tpo->tempo_termios ) ;

        pthread_mutex_lock(& gp_Mut->mut_dat );
        memset( gp_Sui->sui_dat->dat_inf, 0, strlen( gp_Sui->sui_dat->dat_inf ) ) ;
        strcpy( gp_Sui->sui_dat->dat_inf, "") ;
        pthread_mutex_unlock(& gp_Mut->mut_dat );
      }
    }

    KEYBOARD_TERMIOS_EXIT() ;
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
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CLAVIER_getchar( STRUCT_SUIVI * gp_Sui ) {

  long long ll_inrc=0 ;
  int c_char = 0 ;
  struct sched_param param;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL) ;
  pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL ) ;
  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_CLAVIER ) ;

  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
  
  if ( gp_Dev->use_keyboard ) {

    /* Debut boucle SUIVI_CLAVIER_getchar */

    while( ( c_char = getchar () ) != 'q' ) {
      usleep(100000) ;
      Trace1("%c %d entre au clavier", c_char,c_char  ) ; 
    }
    if ( c_char == 'q' ) {
      ASTRO_TRAP_MAIN(1) ;
    }
    /* appels LIRC pour memoire : 
    INFRARED_OPEN( gp_LircConfig ) ;
    INFRARED_READ( gp_Sui ) ;
    INFRARED_CLOSE(gp_LircConfig) ;*/
  }
  Trace1("Stop") ;

  return NULL ;
}

/*****************************************************************************************
* @fn     : SUIVI_CLAVIER_NCURSES
* @author : s.gravois
* @brief  : fonction de callback du thread suivi clavier en mode ncurses
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CLAVIER_NCURSES(STRUCT_SUIVI * gp_Sui ) {

  long long ll_inrc=0 ;
  int ch = 0 ;
  unsigned long l_incr=0 ;
  struct sched_param param;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL) ;
  pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL ) ;
  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_CLAVIER ) ;

  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
  
  if ( gp_Dev->use_keyboard ) {

    // signal( SIGTERM, ASTRO_TRAP_SUIVI_CLAVIER) ;
    
    sleep(1) ;

    initscr() ;
    if (newterm(0, stdout, stdin) == 0) {
    fprintf(stderr, "Cannot initialize terminal\n");
    Trace1("Cannot initialize terminal") ;
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
  Trace1("Stop") ;

  return NULL ;
}

/*****************************************************************************************
* @fn     : SUIVI_CAPTEURS
* @author : s.gravois
* @brief  : fonction de callback du thread suivi capteurs (non utilisee)
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CAPTEURS(STRUCT_SUIVI * gp_Sui) {
   
  long long ll_inrc=0 ;
  struct sched_param param;
  int ret ;
  
  STRUCT_I2C_DEVICE   exemple, *lp_dev ;
  STRUCT_I2C_ACC_MAG  accmag,  *lp_acc ;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL) ;
  pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL ) ;
  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_CAPTEURS ) ;

  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CAPTEUR ) ;

  lp_dev = & exemple ;
  lp_acc = & accmag ;

  lp_dev->fd = 0 ;
  
  if ( gp_Dev->use_capteurs ) {

    
    /* Debut boucle SUIVI_CAPTEURS */
    
    while(TRUE) {
      
      Trace1("while") ;

      if ( ! gp_Dev->init_capteurs ) {
        
        ret = I2C_INIT(lp_dev, DEVICE_RASPI_2, DEVICE_LSM_ADRESS ) ;

        if ( ! ret ) {
          Trace1("Pas de capteur disponible") ;
          gp_Dev->use_capteurs = 0 ;
          gp_Dev->init_capteurs = 0 ;
          break ;
        }
        else {
          I2C_SET( lp_dev, REG_CTRL1, "0x57" ) ;
          I2C_SET( lp_dev, REG_CTRL2, "0x00" ) ;
          I2C_SET( lp_dev, REG_CTRL5, "0x64" ) ;
          I2C_SET( lp_dev, REG_CTRL6, "0x20" ) ;
          I2C_SET( lp_dev, REG_CTRL7, "0x00" ) ;
    
          gp_Dev->init_capteurs = 1 ;
        }
      }
      if ( gp_Dev->init_capteurs ) {
      
        I2C_GET_ACC( lp_dev, lp_acc )   ;
        I2C_CALCULS_ACCMAG( lp_acc ) ;
      /*
        gp_Sui->roll    =  lp_acc->roll  ; 
        gp_Sui->pitch   =  lp_acc->pitch ;
        gp_Sui->heading =  lp_acc->heading ;
      */
        Trace1("%.0f\t%.0f\t%.0f\n", lp_acc->pitch * I2C_DEGRAD, lp_acc->roll * I2C_DEGRAD, lp_acc->heading * I2C_DEGRAD) ;
      }	
      
      usleep( gp_Sui->sui_tpo->tempo_capteurs );
    }
  }
  Trace1("Stop") ;

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
  STRUCT_GPIO_PWM_MOTEUR *gp_Gpio_Pwm_Mot_Alt , g_mot_alt ; 
  STRUCT_GPIO_PWM_MOTEUR *gp_Gpio_Pwm_Mot_Azi , g_mot_azi ;
*/
  
  TraceArbo(__func__,0 , "") ;

  int i2cDev = 1;

  /* Attention, pas de Trace1 avant d avoir ouvert le fichier de log */ 
  /* car Trace1 utilise ce fichier */
  /* Alternative : Trace1 */ 

  ASTRO_GLOBAL_INIT() ;

  ARGUMENTS_GERER_REP_HOME(argc, argv) ;
  /* Pour permettre acces a STRUCT_SUIVI* via struct STRUCT_GPIO_PWM_MOTEUR* */

  CONFIG_PATH_FIND( gc_config_path_cmd_stty, "stty") ;
  
  // -----------------------------------------------------------------
  // Initialisations diverses et variees
  // -----------------------------------------------------------------

  CONFIG_FIC_READ            ( gp_Con ) ;
  GPIO_CONFIG_FIC_READ       ( gp_Con ) ; 
  CONFIG_FIC_DISPLAY         ( gp_Con ) ;
  CONFIG_FIC_VERIFY          ( gp_Con);
  CONFIG_PARAMETRES_CONFIG   ( gp_Con ) ;
  CONFIG_PARAMETRES_AFFICHER () ;

  // CONFIG_PARAMETRES_AFFICHER() ;   
  /* LOG_INIT ouvre le fichier en ecriture pour pouvoir avoir les traces en mode ecriture sur disque
     et donc a besoin de lire au prealable le fichier de CONFIGURATION (config.txt) */

  LOG_INIT        (gp_Log); 

  TEMPS_INIT     ( gp_Tim, gp_Tim_Tem ) ;
  
  /* GPIO_CLAVIER_MATRICIEL_CONFIG( gpio_key_l, gpio_key_c ) ; */
  
  VOUTE_INIT     ( gp_Vou ) ; /* soit etre place avant SUIVI_INIT */
  CODES_INIT     ( gp_Cod ) ;
  KEYS_INIT      ( gp_Key ) ;   
  ASTRE_INIT     ( gp_Ast ) ;
  LIEU_INIT      ( gp_Lie  ) ;
  
  TEMPS_CALCUL_TEMPS_SIDERAL  ( gp_Lie, gp_Tim) ;
  
  DEVICES_INIT   ( gp_Dev ) ;
  SUIVI_INIT     ( gp_Sui ) ;
  LCD_INIT       ( gp_Lcd ) ;
  PID_INIT       ( gp_Pid, gp_Pid_Par->par_pid_ech, gp_Pid_Par->par_pid_kp, gp_Pid_Par->par_pid_ki, gp_Pid_Par->par_pid_kd ) ;

  DEVICES_AFFICHER_UTILISATION() ;

  Trace1("pthread_self = %ld", pthread_self()) ;

  /* ---------------------------------------------------------------------------- */
  /* Initialisation des strcutures necessaires aux attributs de tread             */
  /* configuration du thread et des attributs de tread du MAIN                    */
  /* ---------------------------------------------------------------------------- */

  PTHREADS_INIT_MUTEXS () ;
  PTHREADS_INIT        ( gp_Pth, pthread_self()) ;
  PTHREADS_CONFIG      ( gp_Pth, pthread_self(), PTHREAD_TYPE_MAIN  ) ; 

  signal(SIGINT,ASTRO_TRAP_MAIN) ;
  signal(SIGALRM,ASTRO_TRAP_MAIN) ;

  Trace1("gi_gpio_alt         : %d %d %d %d", gi_gpio_alt[0], gi_gpio_alt[1], gi_gpio_alt[2], gi_gpio_alt[3] ) ;
  Trace1("gi_gpio_azi         : %d %d %d %d", gi_gpio_azi[0], gi_gpio_azi[1], gi_gpio_azi[2], gi_gpio_azi[3] ) ;
  Trace1("gi_gpio_mas         : %d %d %d %d", gi_gpio_mas[0], gi_gpio_mas[1], gi_gpio_mas[2], gi_gpio_mas[3] ) ;
  Trace1("gp_Gpi_Par_Pwm->par_led_etat    : %d", gp_Gpi_Par_Pwm->par_led_etat );
  Trace1("gp_Ast_Par->par_default_object : %s", gp_Ast_Par->par_default_object) ;
  
  Trace1("gp_Pid_Par->par_pid_ech = %f",  gp_Pid_Par->par_pid_ech);
  
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

    gp_Ast->ast_new = TRUE ;
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
    gp_Gpio_Pwm_Mot_Alt,\
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
    gp_Gpio_Pwm_Mot_Azi,\
    gi_gpio_azi,\
    gi_gpio_mas,\
    gp_Cal_Par->par_azi_red4 ,\
    100,\
    gd_gpio_frequence_pwm, \
    1, \
    GPIO_CURRENT_FONCTION,\
    GPIO_CURRENT_FONCTION_PARAM0,\
    GPIO_CURRENT_FONCTION_PARAM1 ) ;
  
  Trace1("gp_Gpio_Pwm_Mot_Azi->Fm %f gp_Gpio_Pwm_Mot_Azi->periode_mot = %f", gp_Gpio_Pwm_Mot_Azi->Fm, gp_Gpio_Pwm_Mot_Azi->periode_mot ) ;
  Trace1("gp_Gpio_Pwm_Mot_Alt->Fm %f gp_Gpio_Pwm_Mot_Alt->periode_mot = %f", gp_Gpio_Pwm_Mot_Alt->Fm, gp_Gpio_Pwm_Mot_Alt->periode_mot ) ;

  for( i=0 ; i< gp_Cal_Par->par_alt_red_4 ; i++ ) {
    Trace1("%d\t%f\t%f\t%f\t%f",i,\
      gp_Gpio_Pwm_Mot_Azi->mot_pha[0]->rap[i],\
      gp_Gpio_Pwm_Mot_Azi->mot_pha[1]->rap[i],\
      gp_Gpio_Pwm_Mot_Azi->mot_pha[2]->rap[i],\
      gp_Gpio_Pwm_Mot_Azi->mot_pha[3]->rap[i]) ;
  }

  // ============================== gestion des threads  ===================================
/*
  pthread_create( &p_thread_pha_azi[0],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Azi->mot_pha[0] ) ;
  pthread_create( &p_thread_pha_azi[1],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Azi->mot_pha[1] ) ;
  pthread_create( &p_thread_pha_azi[2],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Azi->mot_pha[2] ) ;
  pthread_create( &p_thread_pha_azi[3],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Azi->mot_pha[3] ) ;

  pthread_create( &p_thread_pha_alt[0],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Alt->mot_pha[0] ) ;
  pthread_create( &p_thread_pha_alt[1],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Alt->mot_pha[1] ) ;
  pthread_create( &p_thread_pha_alt[2],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Alt->mot_pha[2] ) ;
  pthread_create( &p_thread_pha_alt[3],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Alt->mot_pha[3] ) ;

  pthread_create( &p_thread_mot_azi,           NULL, (void*)suivi_main_M, gp_Gpio_Pwm_Mot_Azi ) ;
  pthread_create( &p_thread_mot_alt,           NULL, (void*)suivi_main_M, gp_Gpio_Pwm_Mot_Alt ) ;

*/
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_0], NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Azi->mot_pha[0] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_1], NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Azi->mot_pha[1] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_2], NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Azi->mot_pha[2] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_3], NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Azi->mot_pha[3] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_AZI],         NULL, (void*)suivi_main_M,         gp_Gpio_Pwm_Mot_Azi ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_0], NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Alt->mot_pha[0] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_1], NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Alt->mot_pha[1] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_2], NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Alt->mot_pha[2] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_3], NULL, (void*)GPIO_SUIVI_PWM_PHASE, gp_Gpio_Pwm_Mot_Alt->mot_pha[3] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_ALT],         NULL, (void*)suivi_main_M,         gp_Gpio_Pwm_Mot_Alt ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MENU],            NULL, (void*)SUIVI_MENU,           gp_Sui ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_VOUTE],           NULL, (void*)SUIVI_VOUTE,          gp_Sui ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_INFRA],           NULL, (void*)SUIVI_INFRAROUGE,     gp_Sui ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_CAPTEUR],         NULL, (void*)SUIVI_CAPTEURS,       gp_Sui ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_CLAVIER],         NULL, (void*)SUIVI_CLAVIER_TERMIOS,gp_Sui ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_LCD],             NULL, (void*)SUIVI_LCD,            gp_Sui ) ;

  /* Affichage d'informations sur les threads lancés */

  sleep(5) ;

  PTHREADS_INFOS(gp_Pth) ;

  sleep(1) ;

  // ============================== join des threads  ===================================
  /* TODO : statut cancellable implique de ne pas forcement mettre en join */

  if ( gp_Dev->use_lcd )         pthread_join( gp_Pth->pth_t[PTHREAD_T_LCD], NULL) ;
  if ( gp_Dev->use_keyboard )    pthread_join( gp_Pth->pth_t[PTHREAD_T_CLAVIER], NULL) ;
  if ( gp_Dev->use_capteurs )    pthread_join( gp_Pth->pth_t[PTHREAD_T_CAPTEUR], NULL) ;
  if ( gp_Dev->use_infrarouge )  pthread_join( gp_Pth->pth_t[PTHREAD_T_INFRA], NULL) ;

  pthread_join( gp_Pth->pth_t[PTHREAD_T_VOUTE], NULL) ;
  pthread_join( gp_Pth->pth_t[PTHREAD_T_MENU],  NULL) ;
  pthread_join( gp_Pth->pth_t[PTHREAD_T_MOT_ALT], NULL) ;
  pthread_join( gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_3], NULL) ;
  pthread_join( gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_2], NULL) ;
  pthread_join( gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_1], NULL) ;
  pthread_join( gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_0], NULL) ;
  pthread_join( gp_Pth->pth_t[PTHREAD_T_MOT_AZI], NULL) ;
  pthread_join( gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_3], NULL) ;
  pthread_join( gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_2], NULL) ;
  pthread_join( gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_1], NULL) ;
  pthread_join( gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_0], NULL) ;

  return 0;
}
// #######################################################################################
// ####### FIN PROGRAMME - FIN PROGRAMME - FIN PROGRAMME - FIN PROGRAMME         - #######
// #######################################################################################
