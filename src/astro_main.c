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
# 19/01/2002  | * suppression passage arguments CONGI_DISPLAY_TOUT
# (issue)         cela est justifie par le fait que les variables sont globales
# 
# 11/03/2022  | * prise en compte option -p (facon classique)
#               * renseignement gp_Con_Par->con_par_rep_home par -p <path>
# 11/03/2022  | * ajout appel fonction ARGUMENTS_MANAGE_REP_HOME
#
# 21/03/2022  | * ajout appel CODES_INIT dasn main 
#                 en remaplacement de IR_INIT_CODES
# avril 2002  | * debut codage fonctionnalites LCD1602
#               * suppressione appel GPIO_CLAVIER_MATRICIEL_READ
#               * correction BUG ( repetition if ( i_indice_code < CODES_CODE_NB_CODES ))
# mai 2022      * petites corrections mineures
#               * reprise intégralité code pour utilisation pointeur fct (gp_Lcd->display_xxx)
#               -> display / default_display pour affichage par defaut
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

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1("Signal trappe de valeur (sig) %d",sig) ;
  Trace1("Envoi d'un signal %d (SIGTERM) aux threads",SIGTERM) ;
  
  GPIO_LED_ETAT_CLIGNOTE(1,100) ;

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

    GPIO_LED_ETAT_CLIGNOTE(1,100) ;

    if ( system("/sbin/halt") < 0 ) {
      SyslogErr("Probleme avec /sbin/halt\n") ;
      Trace1("Probleme avec /sbin/halt") ;

      gp_Lcd->display_str_int(1000000,"exit with return", 2) ;
      exit(2) ;
    } 
  }
  GPIO_SET( gp_Pwm_Par->gpi_pwm_par_led_etat, 0 ) ;

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
  
  TraceArbo(__func__,0,"") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_menu = %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_MENU] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_VOUTE(int sig)  {
  
  TraceArbo(__func__,0,"") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_voute= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_VOUTE] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_INFRAROUGE(int sig)  {
  TraceArbo(__func__,0,"") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_infrarouge= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_INFRA] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_CAPTEURS(int sig)  {
  TraceArbo(__func__,0,"") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_capteurs= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_CAPTEUR] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_CLAVIER(int sig)  {
  TraceArbo(__func__,0,"") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_clavier= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_CLAVIER] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
void ASTRO_TRAP_SUIVI_LCD(int sig)  {
  TraceArbo(__func__,0,"") ; // MACRO_DEBUG_ARBO_FONCTIONS
  Trace1("Signal trappe depuis thread suivi_termios= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pth->pth_t[ PTHREAD_T_LCD] ) ;
  ASTRO_TRAP_MAIN(1) ;
}
*/
/*****************************************************************************************
* @fn     : _SUIVI_MENU
* @author : s.gravois
* @brief  : Ce mode permet de gerer les menus .
* @param  : STRUCT_SUIVI   *gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : 
*****************************************************************************************/

void * _SUIVI_MENU(STRUCT_SUIVI * gp_Sui) {
  
  long long ll_inrc=0 ;
  struct sched_param param;
  void * (*ptr_fct)(STRUCT_SUIVI *)  = _SUIVI_MENU ;

  TraceArbo(__func__,1,"pthread_create_callback_fct") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
 
  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_MENU ) ;
   
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_MENU ) ;
  
  SUIVI_MENU_BEFORE_WHILE ( gp_Sui) ;

  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread _SUIVI_MENU
  //-------------------------------------------------------------------------------
  
  /* Debut boucle _SUIVI_MENU */
  
  while(TRUE) {

    /* Creee un point d 'annulation pour la fonction pthread_cancel */
    pthread_testcancel() ;

    Trace1("temporisation %ld", gp_Tpo->tpo_menu ) ;
    usleep( gp_Tpo->tpo_menu ) ;

    /* GPIO_CLAVIER_MATRICIEL_READ( gpio_key_l, gpio_key_c, gp_Key) ; */
    KEYS_INPUTS_GESTION_APPUIS( gp_Key) ;
    
    /* SUIVI_OLD_0( gp_Sui) ; */
    SUIVI_TRAITEMENT_MOT( gp_Sui ) ;

    CONFIG_MENU_CHANGE_DETECT() ;

    switch( gp_Sui->sui_menu ) {

      // ------------------------------- ALIGNEMENT - MODE AZIMUTAL ----------------------------
     
      case MENU_AZIMUTAL :
        
        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Cal->cal_mutex ) ;

        gp_Cal->cal_mode             = CALCULS_MODE_AZIMUTAL ;
        
        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Cal->cal_mutex ) ;

        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Pas->pas_mutex ) ;

        gp_Pas->pas_acc_alt          = 1 ;
        gp_Pas->pas_acc_azi          = 1 ;

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Pas->pas_mutex ) ;
        
        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Vou->vou_mutex ) ;

        gp_Vou->vou_run              = 1 ; 

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Vou->vou_mutex ) ;

        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Sui->sui_mutex ) ;

        gp_Sui->sui_menu_old         = gp_Sui->sui_menu ;
        gp_Sui->sui_menu             = MENU_MANUEL_BRUT ; 

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Sui->sui_mutex ) ;

      break ;

      // ------------------------------  MODE EQUATORIAL --------------------------------------
     
      case MENU_EQUATORIAL :
        
        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Cal->cal_mutex ) ;

        gp_Cal->cal_mode             = CALCULS_MODE_EQUATORIAL ;
        
        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Cal->cal_mutex ) ;
        
        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Pas->pas_mutex ) ;

        gp_Pas->pas_acc_alt          = 0 ;
        gp_Pas->pas_acc_azi          = 1 ;

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Pas->pas_mutex ) ;

        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Sui->sui_mutex ) ;

        gp_Sui->sui_menu_old         = gp_Sui->sui_menu ;
        gp_Sui->sui_menu             = MENU_MANUEL_BRUT ;

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Sui->sui_mutex ) ;

        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Vou->vou_mutex ) ;

        gp_Vou->vou_run      = 0 ; 

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Vou->vou_mutex ) ;

      break ;

      /* -------------------------- SUIVI MANUEL PAR DEFAUT -------------------------- */
      /* 
        Le but de ce suivi est de deduire des actions N-S-O-E de l'utilisateur 
        simplement une acceleration / ralentissement / changement de direction
        sur le N-S-E-O (pas de recalcul des periodes) 
        FIXME : les periodes sont conservees , ainsi que le mode azimutal ou equatorial    
      */
      case MENU_MANUEL_BRUT :
        
        Trace1("MENU_MANUEL_BRUT") ;
      
        SUIVI_MANUEL_BRUT(gp_Sui) ;
  
      break ;

      /* -------------------------- MENU_MANUEL_NON_ASSERVI : LE PLUS SIMPLE ------------------
          TODO : a coder : juste appui des touches provoque le mouvement
          TODO : mode le plus simple a priori
          TODO : acc_azi_ et pas_acc_alt doivent etre initialises a zero
      */
      
      case MENU_MANUEL_NON_ASSERVI :
        
        Trace1("MENU_MANUEL_NON_ASSERVI") ;
        
        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Pas->pas_mutex ) ;

        gp_Pas->pas_acc_alt          = 0 ;
        gp_Pas->pas_acc_azi          = 0 ;

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Pas->pas_mutex ) ;

      break ;

      /* -------------------------- Suivi MANUEL AVEC RE-CALCUL DES PERIODES  ----------------- */
      /*
        TODO : a modifier car cela marche pas tres bien (interraction avec le thread _SUIVI_VOUTE)
        TODO : le but de ce gp_Sui est de deduire des actions N-S-O-E de l'utilisateur 
        TODO : les periodes / frequences en azimut et altitude
      */
      case MENU_MANUEL_ASSERVI :

        Trace1("MENU_MANUEL_ASSERVI") ;       

        SUIVI_MANUEL_ASSERVI(gp_Sui) ; 
        CALCULS_PERIODES_SUIVI_MANUEL(gp_Ast,gp_Pas,gp_Fre)  ;

        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Sui->sui_mutex ) ;

        gp_Sui->sui_menu_old         = gp_Sui->sui_menu ;
        gp_Sui->sui_menu             = MENU_MANUEL_ASSERVI ; 

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Sui->sui_mutex ) ;

      break ;

      // ------------------------------  MODE GOTO --------------------------------------

      case MENU_GOTO :

        Trace1("appel : %d : MENU_GOTO" , gp_Sui->sui_menu) ;

        gp_Sui->sui_menu_old         = gp_Sui->sui_menu ;
        gp_Sui->sui_menu             = MENU_MANUEL_BRUT ;

      break ; 

      // ------------------------------ MODE ACTIVATION DU RESEAU ----------------------------------
     
      case MENU_RESEAU_UP :

        Trace1("appel : %d : MENU_RESEAU_UP" , gp_Sui->sui_menu) ;

        if ( system("/etc/init.d/OLD/S40network start")) {
         perror("Probleme avec system(/etc/init.d/OLD/S40network start)"); 
         Trace1("Probleme avec system(/etc/init.d/OLD/S40network start)\n") ;
        }

        gp_Sui->sui_menu = gp_Sui->sui_menu_old ;
        gp_Sui->sui_menu = MENU_MANUEL_BRUT ;

      break ;

      // ------------------------------ ARRET DU RESEAU ---------------------------------------
     
      case MENU_RESEAU_DOWN :
     
        Trace1("appel : %d : MENU_RESEAU_DOWN" , gp_Sui->sui_menu) ;

        if ( system("/etc/init.d/OLD/S40network stop")) {
          perror("Probleme avec system(/etc/init.d/OLD/S40network stop)"); 
          Trace1("Probleme avec system(/etc/init.d/OLD/S40network stop)\n") ;
        }

        gp_Sui->sui_menu = gp_Sui->sui_menu_old ;
        gp_Sui->sui_menu = MENU_MANUEL_BRUT ;

      break ;
      // ------------------------------ ARRET DU PROGRAMME ------------------------------------
      case MENU_PROGRAMME_DOWN :
     
        Trace("appel : %d : MENU_PROGRAMME_DOWN" , gp_Sui->sui_menu) ;

        ASTRO_TRAP_MAIN(1) ;
        break ;
     
      // ------------------------------ ARRET DU SYSTEME EMBARQUE -----------------------------
      case MENU_DOWN :

        Trace("appel : %d : MENU_DOWN" , gp_Sui->sui_menu) ;

        ASTRO_TRAP_MAIN(0) ;

      break ;
    }
    Trace1("fin while") ;
  }
  Trace1("Stop") ;

  return NULL ;
}

/*****************************************************************************************
* @fn     : _SUIVI_INFRAROUGE
* @author : s.gravois
* @brief  : fonction de callback du thread suivi infrarouge
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qi est variable globale
*****************************************************************************************/

void * _SUIVI_INFRAROUGE(STRUCT_SUIVI * gp_Sui) {

  long long ll_inrc=0 ;
  int i_ret=0 ; 
  struct sched_param param;
    
  TraceArbo(__func__,1,"pthread_create_callback_fct") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_INFRARED ) ;
  
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_INFRA ) ;
  
  if ( gp_Dev->dev_use_infrarouge ) {
  
    i_ret = INFRARED_OPEN( gp_LircConfig ) ;
    Trace1("INFRARED_OPEN : retour = %d" , i_ret ) ;
    
    // ATTENTION !!! la fonction INFRARED_READ est bloquante (voir ir.c) !!!!!
    
    INFRARED_READ() ;

    INFRARED_CLOSE(gp_LircConfig) ;
  }

  Trace1("Stop") ;

  return NULL ;
}
/*****************************************************************************************
* @fn     : _SUIVI_LCD
* @author : s.gravois
* @brief  : fonction de callback du thread suivi ecran LCD
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-04-12 creation 
* @date   : 2022-04-27 mise en commentaire de LCD_DISPLAY_TEMPS_LIEU
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : TODO : reflechir a ce qui doit etre rafraichi
*****************************************************************************************/

void * _SUIVI_LCD(STRUCT_SUIVI * gp_Sui) {

  long long ll_inrc=0 ;
  int i_duree_us ;
  struct sched_param param;
  char c_l0[LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure] ={0};
  char c_l1[LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure] ={0};

  TraceArbo(__func__,1,"pthread_create_callback_fct") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_LCD ) ;

  memset( c_l0, CALCULS_ZERO_CHAR, sizeof( c_l0 )) ;
  memset( c_l1, CALCULS_ZERO_CHAR, sizeof( c_l1 )) ;
    
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_LCD ) ;
  
  if ( gp_Dev->dev_use_lcd ) {

    gp_Lcd->default_display() ;

    /*-------------------------*/
    /* Debut boucle _SUIVI_LCD */
    /*-------------------------*/

    while(TRUE) {

      /* Creee un point d 'annulation pour la fonction pthread_cancel */
      pthread_testcancel() ;

      Trace1("while") ;

      usleep( gp_Tpo->tpo_lcd_loop );
      
      /* Si un changement de lignes a ete effectue dans une partie du programme */

      if ( gp_Lcd->lcd_change_current == TRUE ) {
       
        Trace1("gp_Lcd->lcd_change_current == TRUE");
        /* on additionne la duree affichage de base et
           la duree d'affichage i_duree_us envoyee par les fcts  */
        
        usleep( gp_Lcd->lcd_change_display_us );

        gp_Lcd->default_display() ;

        gp_Lcd->lcd_change_current = FALSE ;
      }
      else {
        Trace1("gp_Lcd->lcd_change_current == TRUE");
        gp_Lcd->default_refresh() ;
      }
      gp_Ast->ast_new = FALSE ;
      
    }
  }

  Trace1("Stop") ;

  return NULL ;
}
/*****************************************************************************************
* @fn     : _SUIVI_CLAVIER_TERMIOS
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

void * _SUIVI_CLAVIER_TERMIOS( STRUCT_TERMIOS * lp_Ter ) {

  unsigned long u_sleep_termios_demi_periode = 0 ; 
  long long ll_inrc=0 ;
  int i_indice_code=0 ;
  int c_char =0 ;
  int i_nread = 0 ;
  int i_sum_ascii =0 ; 
  char c_sum_ascii[8] ;
  char ch_chaine[TERMIOS_KBHIT_SIZE_BUFFER_READ+1] ;
  struct sched_param param;
  struct timeval t00,t01  ;
  
  u_sleep_termios_demi_periode = (unsigned long)((double)gp_Tpo->tpo_termios / 2.0) ;

  TraceArbo(__func__,1,"pthread_create_callback_fct") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_CLAVIER ) ;

  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
    
  if ( gp_Dev->dev_use_keyboard ) {

    /* Debut boucle _SUIVI_CLAVIER_TERMIOS */
    /* !!! 
       Eviter les traces dans la boucle TERMIOS 
       pour eviter interraction avec 
    */
    while( c_char != 'q' ) {

      /* Creee un point d 'annulation pour la fonction pthread_cancel */
      pthread_testcancel() ;

      usleep( gp_Tpo->tpo_termios ) ;

      memset(&c_char,0,sizeof(c_char)) ;
      memset(ch_chaine, 0, sizeof(ch_chaine)) ;

      KEYBOARD_TERMIOS_KBHIT_READ_CHARS(lp_Ter) ;

      lp_Ter->ter_lock(lp_Ter) ;

      i_nread = lp_Ter->ter_nread  ;

      lp_Ter->ter_unlock(lp_Ter) ;

      if ( i_nread  > 0) {

        lp_Ter->ter_lock(lp_Ter) ;

        strcpy( ch_chaine, lp_Ter->ter_buffer ) ;
        i_sum_ascii      = lp_Ter->ter_sum_ascii ;

        lp_Ter->ter_unlock(lp_Ter) ;

        c_char=ch_chaine[0] ;

        memset(c_sum_ascii,0, sizeof(c_sum_ascii));
        sprintf(c_sum_ascii,"%d",i_sum_ascii);
        i_indice_code=0 ;

        while( strcmp(gp_Cod->cod_in_term[i_indice_code],c_sum_ascii) \
               && ( i_indice_code < CODES_CODE_NB_CODES ) ) { 
           Trace2("%s = %s ?", c_sum_ascii, gp_Cod->cod_in_term[i_indice_code]) ;  
           i_indice_code++ ; 
        }
        
        if ( i_indice_code < CODES_CODE_NB_CODES ) {

          DATAS_ACTION_BUF_TO_DAT( gp_Dat, gp_Cod->cod_out_act[i_indice_code] ) ; 

          Trace("chaine %s nread %d ascii %d indice code %d code %s", \
            ch_chaine, \
            i_nread , \
            i_sum_ascii, \
            i_indice_code, \
            gp_Cod->cod_out_act[i_indice_code] ) ;

        }
        else {
          Trace("chaine %s nread %d ascii %d indice code %d >= CODES_CODE_NB_CODES (%d) : aucune correspondance trouvee", \
            ch_chaine, \
            i_nread , \
            i_sum_ascii, \
            i_indice_code, \
            CODES_CODE_NB_CODES ) ;          
        }        
      }    
    }
  }

  KEYBOARD_TERMIOS_EXIT(lp_Ter) ;
  
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

  TraceArbo(__func__,1,"pthread_create_callback_fct") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_CLAVIER ) ;

  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
  
  if ( gp_Dev->dev_use_keyboard ) {

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
    INFRARED_READ() ;
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

  TraceArbo(__func__,1,"pthread_create_callback_fct") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_CLAVIER ) ;

  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
  
  if ( gp_Dev->dev_use_keyboard ) {

    // signal( SIGTERM, ASTRO_TRAP_SUIVI_CLAVIER) ;
    
    

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
    printw("appel endwin") ; 
    endwin();
    ASTRO_TRAP_MAIN(1) ;
  }
  Trace1("Stop") ;

  return NULL ;
}

/*****************************************************************************************
* @fn     : _SUIVI_CAPTEURS
* @author : s.gravois
* @brief  : fonction de callback du thread suivi capteurs (non utilisee)
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * _SUIVI_CAPTEURS(STRUCT_SUIVI * gp_Sui) {
   
  long long ll_inrc=0 ;
  struct sched_param param;
  int ret ;
  
  STRUCT_I2C   l_I2c, *lp_I2c ;
  STRUCT_I2C_ACC_MAG  l_Acc, *lp_Acc ;

  TraceArbo(__func__,1,"pthread_create_callback_fct") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_CAPTEURS ) ;

  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CAPTEUR ) ;

  lp_I2c = & l_I2c ;
  lp_Acc = & l_Acc ;

  lp_I2c->i2c_dev_fd = 0 ;
  
  if ( gp_Dev->dev_use_capteurs ) {

    
    /* Debut boucle _SUIVI_CAPTEURS */
    
    while(TRUE) {

      /* Creee un point d 'annulation pour la fonction pthread_cancel */
      pthread_testcancel() ;

      Trace1("while") ;

      if ( ! gp_Dev->dev_init_capteurs ) {
        
        ret = I2C_INIT(lp_I2c, DEVICE_RASPI_2, DEVICE_LSM_ADRESS ) ;

        if ( ! ret ) {
          Trace1("Pas de capteur disponible") ;
          gp_Dev->dev_use_capteurs = 0 ;
          gp_Dev->dev_init_capteurs = 0 ;
          break ;
        }
        else {
          I2C_WRITE( lp_I2c, REG_CTRL1, "0x57" ) ;
          I2C_WRITE( lp_I2c, REG_CTRL2, "0x00" ) ;
          I2C_WRITE( lp_I2c, REG_CTRL5, "0x64" ) ;
          I2C_WRITE( lp_I2c, REG_CTRL6, "0x20" ) ;
          I2C_WRITE( lp_I2c, REG_CTRL7, "0x00" ) ;
    
          gp_Dev->dev_init_capteurs = 1 ;
        }
      }
      if ( gp_Dev->dev_init_capteurs ) {
      
        I2C_ACC_READ( lp_I2c, lp_Acc )   ;
        I2C_CALCULS_ACCMAG( lp_Acc ) ;
      /*
        gp_Sui->_roll    =  lp_Acc->acc_roll  ; 
        gp_Sui->_pitch   =  lp_Acc->acc_pitch ;
        gp_Sui->_heading =  lp_Acc->acc_heading ;
      */
        Trace1("%.0f\t%.0f\t%.0f\n", \
          lp_Acc->acc_pitch * I2C_DEGRAD, \
          lp_Acc->acc_roll * I2C_DEGRAD, \
          lp_Acc->acc_heading * I2C_DEGRAD ) ;
      }	
      
      usleep( gp_Tpo->tpo_capteurs );
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
  
  TraceArbo(__func__,0 , "") ;

  /* Attention, pas de Trace1 avant d avoir ouvert le fichier de log */ 
  /* car Trace1 utilise ce fichier */
  /* Alternative : Trace1 */ 

  ASTRO_GLOBAL_INIT          () ;
  ARGUMENTS_MANAGE_REP_HOME  ( argc, argv) ;
  CONFIG_PATH_FIND           ( gc_config_path_cmd_stty, "stty") ;
  
  ASTRE_PARAMS_INIT          ( gp_Ast_Par ) ;
  CALCULS_PARAMS_INIT        ( gp_Cal_Par ) ;
  CONFIG_PARAMS_INIT         ( gp_Con_Par ) ;
  DEVICES_PARAMS_INIT        ( gp_Dev_Par ) ;
  LIEU_PARAMS_INIT           ( gp_Lie_Par ) ;
  PID_PARAMS_INIT            ( gp_Pid_Par ) ;
  TIME_PARAMS_INIT           ( gp_Tim_Par ) ;
  GPIO_PWM_PARAMS_INIT       ( gp_Pwm_Par ) ;
  
  CONFIG_INIT                ( gp_Con ) ;
  CONFIG_FIC_READ            ( gp_Con ) ;
  GPIO_CONFIG_FIC_READ       ( gp_Con ) ; 
  CONFIG_FIC_DISPLAY         ( gp_Con ) ;
  CONFIG_FIC_VERIFY          ( gp_Con ) ;
  CONFIG_PARAMETRES_CONFIG   ( gp_Con ) ;

  CONFIG_PARAMS_DISPLAY  () ;  

  TIME_INIT                  ( gp_Tim ) ;
  LIEU_INIT                  ( gp_Lie ) ;
  TIME_INIT_TEMPOS           ( gp_Tpo ) ;
  TIME_TEMPOS_CONFIG         ( gp_Tpo ) ;
  TIME_TEMPOS_DISPLAY        ( gp_Tpo ) ;
  LOG_INIT                   ( gp_Log ); 

  TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;
  
  CALCULS_INIT               ( gp_Cal ) ;
  CAT_INIT                   ( gp_Ngc ) ;
  CAT_INIT                   ( gp_Eto ) ;
  CAT_READ                   ( gp_Ngc, CAT_NGC_TXT ) ;    
  CAT_READ                   ( gp_Eto, CAT_ETO_TXT ) ;                  
  CAT_FORMAT_DECIMAL_NGC     ( gp_Ngc, CAT_NGC_DEC_TXT ) ;         
  CAT_FORMAT_DECIMAL_ETO     ( gp_Eto, CAT_ETO_DEC_TXT ) ; 
  CODES_INIT                 ( gp_Cod ) ;
  VOUTE_INIT                 ( gp_Vou ) ; /* soit etre place avant SUIVI_INIT */
  ASTRE_INIT                 ( gp_Ast ) ;
  KEYS_INIT                  ( gp_Key ) ;
  DEVICES_INIT               ( gp_Dev ) ;
  DEVICES_DISPLAY_UTILISATION( gp_Dev) ;
  SUIVI_INIT                 ( gp_Sui ) ;
  SUIVI_PAS_INIT             ( gp_Pas ) ;
  SUIVI_FRE_INIT             ( gp_Fre ) ;
  LCD_INIT                   ( gp_Lcd ) ;
  PID_INIT                   ( gp_Pid ) ;
  STATS_INIT                 ( gp_Sta ) ;
  KEYBOARD_TERMIOS_INIT      (gp_Ter) ;
  
  DEVICES_DISPLAY_UTILISATION( gp_Dev) ;

  /* ---------------------------------------------------------------------------- */
  /* Initialisation des strcutures necessaires aux attributs de tread             */
  /* configuration du thread et des attributs de tread du MAIN                    */
  /* ---------------------------------------------------------------------------- */

  PTHREADS_INIT_MUTEXS () ;
  
  PTHREADS_INIT        ( gp_Pth, pthread_self()) ;
  
  PTHREADS_CONFIG      ( gp_Pth, pthread_self(), PTHREAD_TYPE_MAIN  ) ; 
  
  signal(SIGINT,ASTRO_TRAP_MAIN) ;
  
  signal(SIGALRM,ASTRO_TRAP_MAIN) ;

  Trace("gi_alt_gpios   : %d %d %d %d", gi_alt_gpios[0], gi_alt_gpios[1], gi_alt_gpios[2], gi_alt_gpios[3] ) ;
  Trace("gi_azi_gpios   : %d %d %d %d", gi_azi_gpios[0], gi_azi_gpios[1], gi_azi_gpios[2], gi_azi_gpios[3] ) ;
  Trace("gi_alt_masque  : %d %d %d %d", gi_alt_masque[0], gi_alt_masque[1], gi_alt_masque[2], gi_alt_masque[3] ) ;
  Trace("gi_azi_masque  : %d %d %d %d", gi_azi_masque[0], gi_azi_masque[1], gi_azi_masque[2], gi_azi_masque[3] ) ;
  
  
  gp_Pwm_Par->gpi_pwm_par_led_etat=0;
  printf( "LED : %d\n", gp_Pwm_Par->gpi_pwm_par_led_etat ) ;
  Trace("led_etat    : %d", gp_Pwm_Par->gpi_pwm_par_led_etat );
  
  Trace1("gp_Ast_Par->ast_par_default_object : %s", gp_Ast_Par->ast_par_default_object) ;
  
  Trace1("gp_Pid_Par->par_pid_ech = %f",  gp_Pid_Par->par_pid_ech);
  
  // -----------------------------------------------------------------
  // reglages variables particulieres
  // ----------------------------------------------------------------- 
  /* TODO : fusionner g_c_cat et g_c_cat_eto */

  // -----------------------------------------------------------------
  
  if ( strcmp(gp_Ast_Par->ast_par_default_object,"") != 0 ) {

    memset( gp_Ast->ast_nom, 0, sizeof(gp_Ast->ast_nom)) ;
    strcpy( gp_Ast->ast_nom, gp_Ast_Par->ast_par_default_object ) ;

    CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;

    if ( strstr( gp_Ast->ast_nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Ngc, gp_Ast) ; ;
    if ( strstr( gp_Ast->ast_nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Ngc, gp_Ast) ; ;
    if ( strstr( gp_Ast->ast_nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Eto, gp_Ast) ;

    gp_Ast->ast_new = TRUE ;
  }

  // -----------------------------------------------------------------

  ARGUMENTS_HELP                  ( argc, argv ) ;
  ARGUMENTS_MANAGE_FACON_CLASSIQUE ( argc, argv  ) ;
  
  if ( gp_Sui->sui_alarme != 0 ) {
    alarm( gp_Sui->sui_alarme) ;
  }  

  // ouverture led etat ----------------------------------------------

  if ( gp_Pwm_Par->gpi_pwm_par_led_etat != 0 ) {

    GPIO_CLOSE_BROCHE( gp_Pwm_Par->gpi_pwm_par_led_etat) ;
    GPIO_OPEN_BROCHE( gp_Pwm_Par->gpi_pwm_par_led_etat, 1) ;
    GPIO_SET( gp_Pwm_Par->gpi_pwm_par_led_etat, 0 ) ;
  }
  
  // -----------------------------------------------------------------
  
  GPIO_INIT_PWM_MOTEUR(\
    gp_Alt_Mot,\
    gi_alt_gpios,\
    gi_alt_masque,\
    gp_Cal_Par->cal_par_alt_red_4 ,\
    100,\
    gd_alt_fpwm, \
    0, \
    GPIO_CURRENT_FONCTION,\
    GPIO_CURRENT_FONCTION_PARAM0,\
    GPIO_CURRENT_FONCTION_PARAM1 ) ;

  GPIO_INIT_PWM_MOTEUR(\
    gp_Azi_Mot,\
    gi_azi_gpios,\
    gi_azi_masque,\
    gp_Cal_Par->cal_par_azi_red_4 ,\
    100,\
    gd_azi_fpwm, \
    1, \
    GPIO_CURRENT_FONCTION,\
    GPIO_CURRENT_FONCTION_PARAM0,\
    GPIO_CURRENT_FONCTION_PARAM1 ) ;
  
  Trace1("gp_Azi_Mot->mot_fm %f gp_Azi_Mot->mot_per_mot = %f", gp_Azi_Mot->mot_fm, gp_Azi_Mot->mot_per_mot ) ;
  Trace1("gp_Alt_Mot->mot_fm %f gp_Alt_Mot->mot_per_mot = %f", gp_Alt_Mot->mot_fm, gp_Alt_Mot->mot_per_mot ) ;

  for( i=0 ; i< gp_Cal_Par->cal_par_alt_red_4 ; i++ ) {
    Trace1("%d\t%f\t%f\t%f\t%f",i,\
      gp_Azi_Mot->mot_pha[0]->pha_rap[i],\
      gp_Azi_Mot->mot_pha[1]->pha_rap[i],\
      gp_Azi_Mot->mot_pha[2]->pha_rap[i],\
      gp_Azi_Mot->mot_pha[3]->pha_rap[i]) ;
  }


  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_0], NULL, (void*)_GPIO_PWM_PHASE, gp_Azi_Mot->mot_pha[0] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_1], NULL, (void*)_GPIO_PWM_PHASE, gp_Azi_Mot->mot_pha[1] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_2], NULL, (void*)_GPIO_PWM_PHASE, gp_Azi_Mot->mot_pha[2] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_AZI_PHASE_3], NULL, (void*)_GPIO_PWM_PHASE, gp_Azi_Mot->mot_pha[3] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_AZI],         NULL, (void*)_GPIO_PWM_MOT,         gp_Azi_Mot ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_0], NULL, (void*)_GPIO_PWM_PHASE, gp_Alt_Mot->mot_pha[0] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_1], NULL, (void*)_GPIO_PWM_PHASE, gp_Alt_Mot->mot_pha[1] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_2], NULL, (void*)_GPIO_PWM_PHASE, gp_Alt_Mot->mot_pha[2] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_ALT_PHASE_3], NULL, (void*)_GPIO_PWM_PHASE, gp_Alt_Mot->mot_pha[3] ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MOT_ALT],         NULL, (void*)_GPIO_PWM_MOT,         gp_Alt_Mot ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_MENU],            NULL, (void*)_SUIVI_MENU,           gp_Sui ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_VOUTE],           NULL, (void*)_SUIVI_VOUTE,          gp_Vou ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_INFRA],           NULL, (void*)_SUIVI_INFRAROUGE,     gp_Sui ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_CAPTEUR],         NULL, (void*)_SUIVI_CAPTEURS,       gp_Sui ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_CLAVIER],         NULL, (void*)_SUIVI_CLAVIER_TERMIOS,gp_Ter ) ;
  pthread_create( &gp_Pth->pth_t[PTHREAD_T_LCD],             NULL, (void*)_SUIVI_LCD,            gp_Sui ) ;

  /* Affichage d'informations sur les threads lancés */

  // ============================== join des threads  ===================================
  /* TODO : statut cancellable implique de ne pas forcement mettre en join */

  if ( gp_Dev->dev_use_lcd )         pthread_join( gp_Pth->pth_t[PTHREAD_T_LCD], NULL) ;
  if ( gp_Dev->dev_use_keyboard )    pthread_join( gp_Pth->pth_t[PTHREAD_T_CLAVIER], NULL) ;
  if ( gp_Dev->dev_use_capteurs )    pthread_join( gp_Pth->pth_t[PTHREAD_T_CAPTEUR], NULL) ;
  if ( gp_Dev->dev_use_infrarouge )  pthread_join( gp_Pth->pth_t[PTHREAD_T_INFRA], NULL) ;

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
