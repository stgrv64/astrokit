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
# 17/01/2022  | * ajout gestion touches gp_Clav (*_CLAVIER)
#               * nouveaux fichiers keyboard.h / .c
# 18/01/2022  | * test de fonctions SUIVI_CLAVIER faisant inervenir
#  (issue)        les appels ncurses : KO quand la fonction SUIVI_CLAVIER
#                 est appelle apres la fonction close dans GPIO_CLOSE (???)
#               * creation entete de la fonction au format doxygen des fonctions suivant doxygen
# 19/01/2002  | * suppression passage arguments CONGI_AFFICHER_TOUT
# (issue)         cela est justifie par le fait que les variables sont globales
# 
# 11/03/2022  | * prise en compte option -p (facon classique)
#               * renseignement CONFIG_REP_HOME par -p <path>
# 11/03/2022  | * ajout appel fonction ARGUMENTS_GERER_REP_HOME
#
# 21/03/2022  | * ajout appel CONFIG_INIT_CODES dasn main 
#                 en remaplacement de IR_INIT_CODES
# avril 2002  | * debut codage fonctionnalites LCD1602
#               * suppressione appel GPIO_RAQUETTE_READ
#               * correction BUG ( repetition if ( i_indice_code < CONFIG_CODE_NB_CODES ))
# mai 2022      * petites corrections mineures
#               * reprise intégralité code pour utilisation pointeur fct (gp_Lcd->display_xxx)
#               -> display / display_default pour affichage par defaut
#               -> display_xxx pour affichage autre que default
#               => affichage par default parametrable
# -------------------------------------------------------------- 
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/mman.h>
#include <ncurses.h>

#include <astro.h>


/*****************************************************************************************
* @fn     : TRAP_MAIN
* @author : s.gravois
* @brief  : fonction appelle quand un signal est trape dans main
* @param  : int     sig
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 remaniement fonction avec nom et id de thread 
* @todo   : voir s il est preferable de faire un kill(getpid(),SIGKILL) 
*           plutot qu un exit
*****************************************************************************************/

void TRAP_MAIN(int sig) {

  int i_num_thread=0 ;
  long id_thread =0;
  char c_thread_name [ 16 ] ; 
  char c_cmd[ 64 ] ;
  int i ;

  TRACE("Signal trappe de valeur (sig) %d",sig) ;
  TRACE("Envoi d'un signal %d (SIGTERM) aux threads",SIGTERM) ;
  
  GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 100) ;

  //Trace("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;

  /*--------------------------------------------------------*/
  /* Si sig > positif on abandonne les threads et on quitte */
  /*--------------------------------------------------------*/

  for(i_num_thread=0;i_num_thread<g_id_thread;i_num_thread++)  {
    id_thread = gp_Pthr->p_thread_t_id[i_num_thread] ;
    if ( id_thread >0 ) {
      memset( c_thread_name, 0, sizeof(c_thread_name) ) ;
      pthread_getname_np( id_thread , c_thread_name, 16 ) ;
      Trace("Abandon thread : num %d id %ld nom %s" ,i_num_thread,id_thread,c_thread_name) ;
      pthread_cancel( id_thread );
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

    GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 100) ;

    if ( system("/sbin/halt") < 0 ) {
      SyslogErr("Probleme avec /sbin/halt\n") ;
      Trace("Probleme avec /sbin/halt") ;

      gp_Lcd->display_str_int(1000000,"exit with return", 2) ;
      exit(2) ;
    } 
  }
  GPIO_SET( GPIO_LED_ETAT, 0 ) ;

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
* @fn     : TRAP_SUIVI_MENU
* @author : s.gravois
* @brief  : fonctions appellees quand un signal est trape dans leur fonctions respectives
* @param  : int     sig
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-04-27 ajout affichage LCD
* @date   : 2022-04-27 ajout TRAP_MAIN a la fin des fonctions
* @date   : 2022-05-20 correction pthread_cancel  gp_Pthr->p_suivi_lcd ) ;
* @date   : 2022-05-20 suppression TRAP_SUIVI_TERMIOS
* @todo   : (completer)
*****************************************************************************************/

void TRAP_SUIVI_MENU(int sig)  {
  
  Trace("Signal trappe depuis thread suivi_menu = %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pthr->p_menu ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_VOUTE(int sig)  {
  
  Trace("Signal trappe depuis thread suivi_voute= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pthr->p_suivi_voute ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_INFRAROUGE(int sig)  {
  Trace("Signal trappe depuis thread suivi_infrarouge= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pthr->p_suivi_infrarouge ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_CAPTEURS(int sig)  {
  
  Trace("Signal trappe depuis thread suivi_capteurs= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pthr->p_suivi_capteurs ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_CLAVIER(int sig)  {
  
  Trace("Signal trappe depuis thread suivi_clavier= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pthr->p_suivi_clavier ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_LCD(int sig)  {
  
  Trace("Signal trappe depuis thread suivi_termios= %d\n",sig) ;
  gp_Lcd->display_str_int(0,"Signal trappe", sig) ;
  pthread_cancel( gp_Pthr->p_suivi_lcd ) ;
  TRAP_MAIN(1) ;
}
/*****************************************************************************************
* @fn     : SUIVI_MENU_PREALABLE
* @author : s.gravois
* @brief  : fonction qui initialise des attributs 
* @param  : SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : analyser la reelle utilite de cette fonction
            en diminuant le bombre de variable gp_Sui->var
*****************************************************************************************/

void SUIVI_MENU_PREALABLE (SUIVI * gp_Sui) {

  switch ( gp_Sui->menu ) {
    case MENU_AZIMUTAL            : gp_Sui->SUIVI_EQUATORIAL = 0 ; break ;
    case MENU_EQUATORIAL          : gp_Sui->SUIVI_EQUATORIAL = 1 ; break ;
    case MENU_MANUEL_BRUT         : gp_Sui->SUIVI_MANUEL     = 1 ; break ;
    case MENU_MANUEL_NON_ASSERVI : gp_Sui->SUIVI_MANUEL     = 1 ; break ;
    case MENU_MANUEL_ASSERVI      : gp_Sui->SUIVI_MANUEL     = 1 ; break ;
    case MENU_GOTO                : gp_Sui->SUIVI_GOTO       = 1 ; break ;
    case MENU_INFO                : break ;
    case MENU_RESEAU_UP           : break ;
    case MENU_RESEAU_DOWN         : break ; 
    case MENU_PROGRAMME_DOWN      : break ; 
    case MENU_DOWN                : break ; 
  }
}
/*****************************************************************************************
* @fn     : SUIVI_TRAITEMENT_MOT
* @author : s.gravois
* @brief  : realise certaines actions particulieres apres appui sur une touche
* @param  : SUIVI   *gp_Sui
* @param  : CLAVIER *clavier 
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-30 amelioration et visibilite du contenu fct 
* @date   : 2022-05-30 ajout des mots concernant les touches F1->F12 du gp_Clav
* @todo   : analyser pour simplification <=> avec suivi menu etc..
*****************************************************************************************/

void SUIVI_TRAITEMENT_MOT( SUIVI * gp_Sui, CLAVIER *gp_Clav ) {
  int i=0;
  char cmd[256]={0} ;
  char s_buffer4[ CONFIG_TAILLE_BUFFER_32 * 4 ]={0} ;
  char c_l0[16] ;
  char c_l1[16] ;

  memset( c_l0, ZERO_CHAR, strlen( c_l0 )) ;
  memset( c_l1, ZERO_CHAR, strlen( c_l1 )) ;

  memset( cmd,       ZERO_CHAR, strlen( cmd )) ;
  memset( s_buffer4, ZERO_CHAR, strlen( s_buffer4 )) ;
  
  //---------------------------------------------------------------------------------------------------------
  // TRAITEMENT DES ACTIONS SANS NECESSAIREMENT UNE VALIDATION
  // exemple : on appuie sur la touche ouest est sud nors : appel MENU_MANUEL_BRUT
  //---------------------------------------------------------------------------------------------------------
  
  // On change de MENU si on appuie sur EST OUEST NORD ou SUD => 
  // on passe en MENU 0 

  gp_Sui->menu_old = gp_Sui->menu ;

  /*  touche OUEST */

  /* Si on a un mot non vide */
  if ( strcmp( gp_Clav->mot, "" ) != 0 ) {

    CONFIG_FORMATE_DONNEES_AFFICHAGE(gp_Astr);

    If_Mot_Is("o")     { gp_Sui->menu = MENU_MANUEL_BRUT ;i=1; }
    If_Mot_Is("e" )    { gp_Sui->menu = MENU_MANUEL_BRUT ; i=1;}
    If_Mot_Is("s")     { gp_Sui->menu = MENU_MANUEL_BRUT ; i=1;}
    If_Mot_Is("n")     { gp_Sui->menu = MENU_MANUEL_BRUT ;i=1; }
    If_Mot_Is("reset") { gp_Sui->menu = MENU_MANUEL_BRUT ;i=1; }
    If_Mot_Is("stop")  { gp_Sui->SUIVI_VOUTE = 0 ; i=1;}
    If_Mot_Is("play")  { gp_Sui->SUIVI_VOUTE = 1 ; i=1;}
    
    /* Quelques actions d 'affichage a l'ecran  */

    If_Mot_Is("aff_variables")       { CONFIG_AFFICHER_VARIABLES() ; i=1 ; } ;

    /* Quelques actions d 'affichage a l'ecran LCD */

    If_Mot_Is("aff_tps_lie")         { gp_Lcd->display_tps_lie( 2000000 ) ;i=1; }  
    If_Mot_Is("aff_ast_vit")         { gp_Lcd->display_ast_vit( 2000000 ) ; i=1;}  
    If_Mot_Is("aff_ast_fre")         { gp_Lcd->display_ast_fre( 2000000 ) ; i=1;}
    If_Mot_Is("aff_ast_per")         { gp_Lcd->display_ast_per( 2000000 ) ;i=1; }
    If_Mot_Is("aff_azi_alt")         { gp_Lcd->display_azi_alt( 2000000 ) ; i=1;}
    If_Mot_Is("aff_agh_dec")         { gp_Lcd->display_agh_dec( 2000000 ) ;i=1; }
    If_Mot_Is("aff_asc_dec")         { gp_Lcd->display_asc_dec( 2000000 ) ; i=1;}
    If_Mot_Is("aff_mod_ste")         { gp_Lcd->display_mod_ste( 2000000 ) ;i=1; }
    If_Mot_Is("cfg_gpios_alt_azi")   { gp_Lcd->display_cfg_gpios_alt_azi( 2000000 ) ; i=1;}
    If_Mot_Is("cfg_gpios_mas_fre")   { gp_Lcd->display_cfg_gpios_mas_fre( 2000000 ) ; i=1;}
    If_Mot_Is("cfg_gpios_leds")      { gp_Lcd->display_cfg_gpios_leds( 2000000 ) ;i=1; }
    If_Mot_Is("cfg_reduction")       { gp_Lcd->display_cfg_reduction( 2000000 ) ; i=1;}

    /* Activation algorithme PID de regulation des periodes / frequences moteurs */

    If_Mot_Is("cfg_log_tps_reel_up") { 
      i=1 ; 
      if ( g_i_trace == 0 ){
        g_i_trace = gp_Sui->temporisation_pid_loop ; 
        sprintf(c_l0,"PID:activation") ;
        sprintf(c_l1,"echant. = %d pas", g_i_trace) ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace("%s : %s", c_l0, c_l1) ;
      } 
      else {
        g_i_trace = 0 ; 
        sprintf(c_l0,"PID:") ;
        sprintf(c_l1,"desactivation") ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace("%s : %s", c_l0, c_l1) ;        
      }
    }

    /* Activation traces PID pour le moteur ALT */

    If_Mot_Is("cfg_log_tps_reel_trace_alt") { 
      i=1 ; 
      if ( g_i_trace_alt == 0 ) {
        g_i_trace_alt=1;      
        sprintf(c_l0,"PID:log alt") ;
        sprintf(c_l1,"echant. = %d pas", g_i_trace) ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace("%s : %s", c_l0, c_l1) ;
      }
      else {
        g_i_trace_alt=0;      
        sprintf(c_l0,"PID:log alt") ;
        sprintf(c_l1,"desactivation") ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace("%s : %s", c_l0, c_l1) ;        
      }
    }

    /* Activation traces PID pour le moteur AZI */

    If_Mot_Is("cfg_log_tps_reel_trace_azi") { 
      i=1 ;
      if ( g_i_trace_azi == 0 ) {
        g_i_trace_azi=1 ; 
        sprintf(c_l0,"PID:log azi") ;
        sprintf(c_l1,"echant. = %d pas", g_i_trace) ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace("%s : %s", c_l0, c_l1) ;
      }
      else {
        g_i_trace_azi=0 ; 
        sprintf(c_l0,"PID:log azi") ;
        sprintf(c_l1,"desactivation") ;
        gp_Lcd->display_str_str( 2000000, c_l0, c_l1 ) ; 
        Trace("%s : %s", c_l0, c_l1) ;        
      }
    }

    If_Mot_Is("aff_acc_alt_azi")     { gp_Lcd->display_acc_alt_azi( 2000000 ) ;i=1; }

    If_Mot_Is("aff_infos") { 
      CONFIG_AFFICHER_TOUT() ; 
      gp_Lcd->display_informations ( 2000000 ) ;
      gp_Sui->menu = MENU_INFO ; 
      i=1;
      
    }     // mode info

    /*  touche mode equatorial */
    
    If_Mot_Is("key_equ")      { 
      gp_Lcd->display_str_str( 2000000, "Mode equatorial", (char*)c_Menus[ MENU_EQUATORIAL ] ) ;
      gp_Sui->menu = MENU_EQUATORIAL ; 
      i=1;
    }    

    /*  touche mode azimutal */

    If_Mot_Is("key_azi")      { 
      gp_Lcd->display_str_str( 2000000, "Mode azimutal", (char*)c_Menus[ MENU_AZIMUTAL ] ) ;
      gp_Sui->menu = MENU_AZIMUTAL ; 
      i=1;
    }       // mode azimutal

    /* touche POWER : arret su systeme */

    If_Mot_Is("key_power")     { 
      gp_Lcd->display_str_str( 2000000, "Mode azimutal", (char*)c_Menus[ MENU_AZIMUTAL ] ) ;
      gp_Sui->menu = MENU_DOWN ; 
      i=1;
    } 

    /* touche EXIT : arret su programme */

    If_Mot_Is("key_exit")      { 
      gp_Sui->menu = MENU_PROGRAMME_DOWN ; 
      i=1;
    } 

    /* touche reseau : arret du reseau */
    /* TODO : non implemente : a definir et coder */

    If_Mot_Is("key_reseau_up") { 
      
      gp_Sui->menu = MENU_RESEAU_UP ; 
      /* TODO : a terminer */
      i=1;
      
    }     
    /* remise a jour du mot a vide */ 

    if (i) strcpy(gp_Clav->mot,"") ; 
  }
  
  //-----------------------------------------------------------------
  // Si un  mot est lu sur le clavier (appui sur menu->valider) necessaire)
  // on effectue une action
  // Si autre chose qu'un menu est lu sur le clavier , on peut aussi changer
  // de menu (pour plus de rapidite)
  //-----------------------------------------------------------------

  if ( gp_Clav->phrase_lue ) {
    
    //---------------------------------
    // Si un MENU est entrer au clavier
    // => gestion des MENUS en fonction du clavier
    // ou de toute autre entree exterieure
    // autre qu'un switch electronique
    //---------------------------------
    
    Trace("symbole = %s nombre = %s\n", gp_Clav->symbole, gp_Clav->nombre ) ;
    
    //--------------------------------------------------------------------
    // ON CHANGE DE MENU VOLONTAIREMENT
    //--------------------------------------------------------------------
    
    if ( ! strcmp( gp_Clav->symbole, gp_Clav->menu )) {
      gp_Sui->menu_old = gp_Sui->menu ;
      gp_Sui->menu     = atoi( gp_Clav->nombre ) ; 
    }
    
    //---------------------------------------------------------------------------------------------------------
    // Si un nouvel as a ete pris en compte
    //---------------------------------------------------------------------------------------------------------
    
    sprintf( s_buffer4, "%s%s%s%s", CONFIG_MES, CONFIG_NGC, CONFIG_ETO, CONFIG_PLA) ;

    if ( strstr( s_buffer4, gp_Clav->symbole ) != NULL ) {

      memset( gp_Astr->nom, ZERO_CHAR, strlen(gp_Astr->nom)) ;
      sprintf( gp_Astr->nom, "%s%s", gp_Clav->symbole, gp_Clav->nombre) ;
    
      Trace1("== %s ==",gp_Astr->nom) ;
      
      /* Recherche de l'as dans les catalgues */
      
      if ( strstr( gp_Astr->nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Astr, cat_dec) ;
      if ( strstr( gp_Astr->nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Astr, cat_dec) ;
      if ( strstr( gp_Astr->nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Astr, etoiles_dec) ;
      
      /* Pour les planetes, le calcul est fait dans SOLAR_SYSTEM dans CALCUL_TOUT */

      gp_Sui->SUIVI_ALIGNEMENT = 1 ;
      //gp_Sui->menu = MENU_AZIMUTAL ;

      GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 10) ; 
    }
    //---------------------------------------------------------------------------------------------------------
    // Si une DEMANDE de MISE A JOUR relative au temps (sauf mois et jour)
    //---------------------------------------------------------------------------------------------------------
    
    if(  ! strcmp( gp_Clav->symbole, "TIME" ) )  {

      GPIO_CLIGNOTE(GPIO_LED_ETAT, strlen( gp_Clav->nombre ), 10) ;

      switch ( strlen( gp_Clav->nombre ) ) {

        case 2 : // Si longueur = 2 cela ne peut etre qu'un changement de mois
          
          Trace1("demande changement heure minutes : %s" , gp_Clav->nombre) ;

          /* CONFIG_SET_MONTH( gp_Clav->nombre ) ; */

        case 4 : // Si longueur = 4 cela ne peut etre qu'un changement d'heure et de minutes
          
          Trace1("demande changement heure minutes : %s" , gp_Clav->nombre) ;
    
          CONFIG_SET_HOUR_AND_MINUTES( gp_Clav->nombre ) ;
          
        break ;
          
          // FIXME : 20190822 : modification configuration de la date

        case 8 : // Si longueur = 5 cela est un changement de mois jour avec verif de l'annee

          Trace1("demande changement annee : %s" , gp_Clav->nombre) ;
          
          CONFIG_SET_YEAR_MONTH_AND_DAY( gp_Clav->nombre ) ;
      
          break ;
      }
    }
    //---------------------------------------------------------------------------------------------------------
    // Si une DEMANDE de SETUP a lieu 
    // RAPPEL : l'annee par defaut est declaree en DUR dans le CODE de telle sorte que le code ne soit valable
    // que pour annee en COURS
    //---------------------------------------------------------------------------------------------------------
    
    if(  ! strcmp( gp_Clav->symbole, "SETUP" ) )  {
      switch ( strlen( gp_Clav->nombre ) ) {
        
        case 0 : break ; // a completer
        case 1 : break ; // a completer 

        case 2 : // TODO : exemple d'une demande de capteur : a modifier / completer
          CALCUL_TEMPS_SIDERAL( gp_Lieu, gp_Time ) ;

          if ( gp_Devi->DEVICE_USE_CAPTEURS ) { 
            gp_Astr->a = gp_Sui->pitch ;
            gp_Astr->h = gp_Sui->heading ; 
          }
          CALCUL_EQUATEUR ( gp_Lieu, gp_Astr) ;

          gp_Sui->menu_old = gp_Sui->menu ;
          gp_Sui->menu = MENU_AZIMUTAL ;
          break ;
      }
      //---------------------------------------------------------------------------------------------------------
    }
    gp_Clav->phrase_lue=0 ;
    strcpy( gp_Clav->mot, "" ) ;
    strcpy( gp_Clav->nombre, "" ) ;
  }
}
/*****************************************************************************************
* @fn     : SUIVI_MANUEL_BRUT
* @author : s.gravois
* @brief  : Ce mode permet le centrage / recentrage de l'objet tout en ayant le suivi.
* @param  : SUIVI   *gp_Sui
* @param  : CLAVIER *gp_Clav 
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-31 correction code dans flag_nord_sud_est_ouest : les accelrations sont les pas
* @todo   : 
*****************************************************************************************/
/* FIXME :
  SUIVI_MANUEL_BRUT : 
    le suivi etant effectue sur un as (calcul des vitesses et periodes par suivi_voute)
    l'appui sur les touches N-S-E-O provoque une suspension de suivi_voute jusqua touche OK
    avec une multiplication des vitesses N-S-E-O par un facteur ALT_ACC
    Ce mode permet le centrage / recentrage de l'objet tout en ayant le suivi.
*/

void SUIVI_MANUEL_BRUT(SUIVI * gp_Sui, CLAVIER *gp_Clav) {
  
  int flag_nord_sud_est_ouest = 0 ;
  int flag_calcul = 0 ;

  Trace1("start") ;

  // GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, gp_Sui) ;

  CONFIG_MAJ_SUIVI_PAS( gp_Sui) ;

  if( strcmp( gp_Sui->datas_infrarouge, "") != 0 ) {
    Trace1("0 gp_Sui->datas_infrarouge = %s", gp_Sui->datas_infrarouge ) ;
  }
  // -----------------------------------------------------------
  // reset = remise a zero des compteurs
  //------------------------------------------------------------
  
  if ( gp_Sui->reset ) {

    pthread_mutex_lock(& gp_Pthr->mutex_azi );
    pthread_mutex_lock(& gp_Pthr->mutex_alt );

    if ( gp_Sui->Sh != 0 ) gp_Sui->pas_alt = gp_Sui->Sh ; else gp_Sui->pas_alt = 1 ;
    if ( gp_Sui->Sa != 0 ) gp_Sui->pas_azi = gp_Sui->Sa ; else gp_Sui->pas_azi = 1 ;

    gp_Sui->acc_azi = 1 ;

    if ( gp_Sui->SUIVI_EQUATORIAL )  gp_Sui->acc_alt = 0.0 ;
    else                             gp_Sui->acc_alt = 1.0 ;

    gp_Sui->reset = 0 ;

    TRACE("%-10s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "reset.." , \
           gp_Sui->Sh , gp_Sui->Sa, gp_Sui->pas_azi, gp_Sui->pas_alt, gp_Sui->acc_azi , gp_Sui->acc_alt) ;
           
    pthread_mutex_unlock(& gp_Pthr->mutex_azi );
    pthread_mutex_unlock(& gp_Pthr->mutex_alt );

    flag_calcul = 1 ;
  }
  // -----------------------------------------------------------
  // actions a faire suivant appui sur NORD / SUD / OUEST /EST
  //------------------------------------------------------------

  if ( gp_Sui->pas_nord || gp_Sui->pas_sud || gp_Sui->pas_est || gp_Sui->pas_ouest ) {
    flag_nord_sud_est_ouest = 1 ;
  }

  // flag_nord_sud_est_ouest = 1 = on a appuye sur N-S-E-O

  if (flag_nord_sud_est_ouest ) { 
  
    Trace("flag_nord_sud_est_ouest") ;
    pthread_mutex_lock(& gp_Pthr->mutex_azi );
    pthread_mutex_lock(& gp_Pthr->mutex_alt );

    // On utilise les memes touches que dans SUIVI_MANUEL_ASSERVI

    if ( gp_Sui->pas_nord )  gp_Sui->pas_alt++ ; if ( gp_Sui->pas_alt == 0 ) gp_Sui->pas_alt = 1 ;
    if ( gp_Sui->pas_sud )   gp_Sui->pas_alt-- ; if ( gp_Sui->pas_alt == 0 ) gp_Sui->pas_alt = -1 ;
    if ( gp_Sui->pas_ouest ) gp_Sui->pas_azi++;  if ( gp_Sui->pas_azi == 0 ) gp_Sui->pas_azi = 1 ;
    if ( gp_Sui->pas_est )   gp_Sui->pas_azi-- ; if ( gp_Sui->pas_azi == 0 ) gp_Sui->pas_azi = -1 ;

    if ( gp_Sui->pas_nord  || gp_Sui->pas_sud ) {
      /*
      if ( abs(gp_Sui->pas_alt) <= 2) gp_Sui->acc_alt = gp_Sui->pas_alt  ;
      else                           gp_Sui->acc_alt = gp_Sui->pas_alt * ALT_ACC ;
      */ /* correction mai 2022 */

      gp_Sui->acc_alt = gp_Sui->pas_alt  ;

    }
    if ( gp_Sui->pas_ouest || gp_Sui->pas_est ) {
      /*
      if ( abs(gp_Sui->pas_azi) <= 2) gp_Sui->acc_azi = gp_Sui->pas_azi ;
      else                           gp_Sui->acc_azi = gp_Sui->pas_azi * AZI_ACC ;
      */ /* correction mai 2022 */

      gp_Sui->acc_azi = gp_Sui->pas_azi ;
    }
    gp_Sui->pas_est     = 0 ;
    gp_Sui->pas_ouest   = 0 ; 
    gp_Sui->pas_nord    = 0 ;
    gp_Sui->pas_sud     = 0 ;
    gp_Sui->reset = 0 ;
/*
    Trace("pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", \
         gp_Sui->pas_azi, gp_Sui->pas_alt, gp_Sui->acc_azi , gp_Sui->acc_alt) ;
*/
    Trace("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "fleches.." , \
           gp_Sui->Sh , gp_Sui->Sa, gp_Sui->pas_azi, gp_Sui->pas_alt, gp_Sui->acc_azi , gp_Sui->acc_alt) ;

    pthread_mutex_unlock(& gp_Pthr->mutex_azi );
    pthread_mutex_unlock(& gp_Pthr->mutex_alt );

    flag_calcul = 1 ;
  }
  // ======================================================
  // Gestion des accelerations ou decelerations tres lentes
  // (reglage du suivi precis)
  // ======================================================

  if ( gp_Sui->pas_forward ) {

    pthread_mutex_lock(& gp_Pthr->mutex_azi );
    pthread_mutex_lock(& gp_Pthr->mutex_alt );

    gp_Sui->acc_azi *= ALTAZ_FORWARD ;
    gp_Sui->acc_alt *= ALTAZ_FORWARD ; 

    pthread_mutex_unlock(& gp_Pthr->mutex_azi );
    pthread_mutex_unlock(& gp_Pthr->mutex_alt );
/*
    TRACE("acc*ALTAZ_FORWARD %.4f acc_azi %.4f acc_alt %.4f", ALTAZ_FORWARD, gp_Sui->acc_azi, gp_Sui->acc_alt ) ;
*/
    TRACE("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "forward.." , \
           gp_Sui->Sh , gp_Sui->Sa, gp_Sui->pas_azi, gp_Sui->pas_alt, gp_Sui->acc_azi , gp_Sui->acc_alt) ;

    gp_Sui->pas_forward = 0 ;
    flag_calcul = 1 ;
  }
  if ( gp_Sui->pas_rewind ) {

    pthread_mutex_lock(& gp_Pthr->mutex_azi );
    pthread_mutex_lock(& gp_Pthr->mutex_alt );

    gp_Sui->acc_azi /= ALTAZ_REWIND ;
    gp_Sui->acc_alt /= ALTAZ_REWIND ; 

    pthread_mutex_unlock(& gp_Pthr->mutex_azi );
    pthread_mutex_unlock(& gp_Pthr->mutex_alt );
/*
    TRACE("acc/ALTAZ_REWIND %.4f acc_azi %.4f acc_alt %.4f ", ALTAZ_REWIND, gp_Sui->acc_azi, gp_Sui->acc_alt ) ;
*/
    TRACE("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "rewind.." , \
           gp_Sui->Sh , gp_Sui->Sa, gp_Sui->pas_azi, gp_Sui->pas_alt, gp_Sui->acc_azi , gp_Sui->acc_alt) ;


    gp_Sui->pas_rewind  = 0 ;
    flag_calcul = 1 ;
  }
  // ======================================================
  // Gestion des accelerations ou decelerations tres rapides
  // (reglage du suivi precis)
  // ======================================================

  if ( gp_Sui->pas_forward_fast ) {

    pthread_mutex_lock(& gp_Pthr->mutex_azi );
    pthread_mutex_lock(& gp_Pthr->mutex_alt );

    gp_Sui->acc_azi *= ALTAZ_FORWARD_FAST ;          
    gp_Sui->acc_alt *= ALTAZ_FORWARD_FAST ; 

    pthread_mutex_unlock(& gp_Pthr->mutex_azi );
    pthread_mutex_unlock(& gp_Pthr->mutex_alt );

/*
    TRACE("acc*ALTAZ_FORWARD_FAST %.4f acc_azi %.4f acc_alt %.4f", ALTAZ_FORWARD_FAST, gp_Sui->acc_azi, gp_Sui->acc_alt ) ;
*/
    TRACE("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "forward fast.." , \
           gp_Sui->Sh , gp_Sui->Sa, gp_Sui->pas_azi, gp_Sui->pas_alt, gp_Sui->acc_azi , gp_Sui->acc_alt) ;

    gp_Sui->pas_forward_fast = 0 ;
    flag_calcul = 1 ;
  }
  if ( gp_Sui->pas_rewind_fast ) {

    pthread_mutex_lock(& gp_Pthr->mutex_azi );   
    pthread_mutex_lock(& gp_Pthr->mutex_alt );

    gp_Sui->acc_azi /= ALTAZ_REWIND_FAST ;
    gp_Sui->acc_alt /= ALTAZ_REWIND_FAST ; 

    pthread_mutex_unlock(& gp_Pthr->mutex_azi );
    pthread_mutex_unlock(& gp_Pthr->mutex_alt );

/*
    TRACE("acc/ALTAZ_REWIND_FAST  %.4f acc_azi %.4f acc_alt %.4f ", ALTAZ_REWIND_FAST, gp_Sui->acc_azi, gp_Sui->acc_alt ) ;
*/
    TRACE("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "forward fast.." , \
           gp_Sui->Sh , gp_Sui->Sa, gp_Sui->pas_azi, gp_Sui->pas_alt, gp_Sui->acc_azi , gp_Sui->acc_alt) ;
               
    gp_Sui->pas_rewind_fast  = 0 ;
    flag_calcul = 1 ;
  }
  // ======================================================
  // Rafraichissement des vitesses et periodes
  // ======================================================

  if ( flag_calcul ) {
  
    pthread_mutex_lock( & gp_Pthr->mutex_cal );
    
    CALCUL_VITESSES( gp_Lieu, gp_Astr, gp_Sui) ;
    CALCUL_PERIODE ( gp_Astr, gp_Sui, gp_Vout) ;
    
    pthread_mutex_unlock( & gp_Pthr->mutex_cal );
  }
}

//==========================================================
// MODE MANUEL : le suivi d'effectue directement en appuyant sur les touches 
//==========================================================

void SUIVI_MANUEL_ASSERVI(SUIVI * gp_Sui, CLAVIER *gp_Clav) {
    
  double  tempo_raq_alt ;
  double  tempo_raq_azi ;
  struct timeval t00 ;
  struct timeval t01 ;
  int azi,alt ;
  
  gp_Sui->pas_azi_old = gp_Sui->pas_azi ;
  gp_Sui->pas_alt_old = gp_Sui->pas_alt ;
  
  gettimeofday(&t00,NULL) ;
  
  azi = 0 ;
  alt = 0 ;
  
  GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, gp_Sui) ;
  CONFIG_MAJ_SUIVI_PAS  ( gp_Sui) ;
  
  // La determination de tempo_raq est tres importante
  // Elle varie suivant la reduction total du moteur
  /* TODO : a expliquer */
  tempo_raq_alt = (double)TEMPO_RAQ * 1000000 / ALT_R ;
  tempo_raq_azi = (double)TEMPO_RAQ * 1000000 / AZI_R ;
  
  // -----------------------------------------------------------
  // reset = remise a zero des compteurs
  //------------------------------------------------------------
  
  if ( gp_Sui->reset ) {

    gp_Sui->pas_alt = 0 ;
    gp_Sui->pas_azi = 0 ;

    gp_Sui->d_appui_raq_azi = 0 ;
    gp_Sui->d_appui_raq_alt = 0 ; 

    gp_Sui->Ta_mic = MAIN_TA_RESET ;
    gp_Sui->Th_mic = MAIN_TH_RESET ;
    gp_Sui->Fa_mic = MAIN_FA_RESET ;
    gp_Sui->Fh_mic = MAIN_FH_RESET ;
    gp_Sui->reset = 0 ;
  }
  // -----------------------------------------------------------
  // TANT QUE J'APPUIE SUR UN BOUTON DE LA RAQUETTE ===>
  //------------------------------------------------------------
  
  while( gp_Sui->pas_ouest || gp_Sui->pas_est || gp_Sui->pas_sud || gp_Sui->pas_nord || gp_Sui->pas_acc_plus || gp_Sui->pas_acc_moins )
  { 
    // On remet une periode inferieure a la seconde pour eviter aux deux autres threads d'attendre
    
    if ( gp_Sui->Ta_mic == MAIN_TA_RESET && gp_Sui->Th_mic == MAIN_TH_RESET ) {
      gp_Sui->Ta_mic = MAIN_TA_TRANSITOIRE ;
      gp_Sui->Th_mic = MAIN_TH_TRANSITOIRE ;
      gp_Sui->Fa_mic = MAIN_FA_TRANSITOIRE ;
      gp_Sui->Fh_mic = MAIN_FH_TRANSITOIRE ;
    }
    
    // ----------------------------------------------------------------------------
    // On ne fait qu'une fois dans la boucle les actions suivantes (d'ou ce test..)
    // ----------------------------------------------------------------------------
    
    if ( ! azi )  if ( gp_Sui->pas_ouest || gp_Sui->pas_est || gp_Sui->pas_acc_moins || gp_Sui->pas_acc_plus ) { 
      pthread_mutex_lock(& gp_Pthr->mutex_azi );
      azi=1 ;
    }
    if ( ! alt )  if ( gp_Sui->pas_nord  || gp_Sui->pas_sud || gp_Sui->pas_acc_moins || gp_Sui->pas_acc_plus ) { 
      pthread_mutex_lock(& gp_Pthr->mutex_alt ); 
      alt=1 ;
    }
    
    if ( azi ) {
     
     if ( gp_Sui->pas_ouest || gp_Sui->pas_est ) 
       gp_Sui->pas_azi += gp_Sui->pas_ouest - gp_Sui->pas_est  ; 
     
     if ( gp_Sui->pas_acc_plus)  gp_Sui->pas_azi = (long) gp_Sui->pas_azi * 1.5 ; 
     if ( gp_Sui->pas_acc_moins) gp_Sui->pas_azi = (long) gp_Sui->pas_azi / 1.5 ; 
    }
    
    if ( alt ) {
     
     if ( gp_Sui->pas_nord || gp_Sui->pas_sud)
       gp_Sui->pas_alt += gp_Sui->pas_nord  - gp_Sui->pas_sud ;
     
     if ( gp_Sui->pas_acc_plus)  gp_Sui->pas_alt = (long) gp_Sui->pas_alt * 1.5 ; 
     if ( gp_Sui->pas_acc_moins) gp_Sui->pas_alt = (long) gp_Sui->pas_alt / 1.5 ; 
    }
    
    if ( azi ) usleep( (long)( tempo_raq_azi ) ) ;
    if ( alt ) usleep( (long)( tempo_raq_alt ) ) ;

    //-------------------------------------------------------------
    // ACCELERATION ou DECELERATION lors de l'APPUI sur la RAQUETTE
    // a modifier pour optimiser
    //-------------------------------------------------------------
    
    gp_Sui->pas_acc_plus  = 0 ;
    gp_Sui->pas_acc_moins = 0 ;
    gp_Sui->pas_est   = 0 ;
    gp_Sui->pas_ouest = 0 ; 
    gp_Sui->pas_nord  = 0 ;
    gp_Sui->pas_sud   = 0 ;
    gp_Sui->reset = 0 ;
    
    // on relit sur les claviers en mode manuel 

    GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, gp_Sui) ;
    CONFIG_MAJ_SUIVI_PAS( gp_Sui) ;
  }
  // =================================================================
  // FIN DE LA BOUCLE : TANT QUE J'APPUIE SUR UN BOUTON DE LA RAQUETTE
  // =================================================================

  if ( azi ) { pthread_mutex_unlock( & gp_Pthr->mutex_azi ); }
  if ( alt ) { pthread_mutex_unlock( & gp_Pthr->mutex_alt ); }
  
  gettimeofday(&t01,NULL) ;

  gp_Sui->t_diff     = (( t01.tv_sec - t00.tv_sec ) * GPIO_MICRO_SEC) + t01.tv_usec - t00.tv_usec ;
  gp_Sui->t_diff_sec = (double)gp_Sui->t_diff / (double)GPIO_MICRO_SEC ;
	 
  if ( gp_Sui->pas_azi_old != gp_Sui->pas_azi ) gp_Sui->d_appui_raq_azi += gp_Sui->t_diff_sec ;
  if ( gp_Sui->pas_alt_old != gp_Sui->pas_alt ) gp_Sui->d_appui_raq_alt += gp_Sui->t_diff_sec ;
	 
  if ( gp_Sui->pas_azi_old != gp_Sui->pas_azi || gp_Sui->pas_alt_old != gp_Sui->pas_alt ) {

   TRACE(" : Va = %2.4f Vh = %2.4f Ta_mic = %2.4f Th_mic = %2.4f Fa_mic = %2.4f Fh_mic = %2.4f Fam = %ld Fhm = %ld",\
        gp_Astr->Va,gp_Astr->Vh,gp_Sui->Ta_mic,gp_Sui->Th_mic,gp_Sui->Fa_mic,gp_Sui->Fh_mic, \
	(gp_Sui->Ia - gp_Sui->Ia_prec),(gp_Sui->Ih - gp_Sui->Ih_prec )) ;  
  }

}
/*****************************************************************************************
* @fn     : SUIVI_MENU
* @author : s.gravois
* @brief  : Ce mode permet de gerer les menus .
* @param  : SUIVI   *gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_id_thread++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : 
*****************************************************************************************/

void * SUIVI_MENU(SUIVI * gp_Sui) {
  
  struct sched_param param;
  
   // La temporisation dans les boucles du thread SUIVI_MENU depend de  gp_Sui->temporisation_menu
   
  Trace2("sleeping..") ;
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_MENU ) ;
  Trace("start") ;

  if ( gc_user_id == USERID_ROOT ) {

    param.sched_priority = PTHREAD_POLICY_1 ;
    
    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_MENU, & param) != 0) { 
      perror("setschedparam SUIVI_MENU");
      exit(EXIT_FAILURE); 
    }
  }
  else { 
    Trace("gc_user_id not USERID_ROOT => cannot pthread_setschedparam(args)") ;
  }

  pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
  pthread_setname_np( pthread_self(), "SUIVI_MENU" ) ;
  gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
  pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;

  signal(SIGTERM,TRAP_SUIVI_MENU) ;
  
  SUIVI_MENU_PREALABLE ( gp_Sui) ;

  gp_Sui->reset = 1 ;

  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread SUIVI_MENU
  //-------------------------------------------------------------------------------
  
  Trace2("debut while") ;

  while(1) {

    usleep( gp_Sui->temporisation_menu ) ;

    /* GPIO_RAQUETTE_READ( gpio_key_l, gpio_key_c, gp_Clav) ; */
    CONFIG_INPUTS_GESTION_APPUIS( gp_Sui, gp_Clav) ;
    /* IR_ACTIONS_PARTICULIERES( gp_Sui) ; */
    SUIVI_TRAITEMENT_MOT( gp_Sui, gp_Clav ) ;
    CONFIG_AFFICHER_CHANGEMENTS() ;
    CONFIG_AFFICHER_CLAVIER( gp_Clav ) ;	

    switch( gp_Sui->menu ) {

      // ------------------------------- ALIGNEMENT - MODE AZIMUTAL ----------------------------
     
      case MENU_AZIMUTAL :

        // a modifier / completer : CALCUL_TEMPS_SIDERAL et CALCUL_ANGLE_HORAIRE
        // sont a supprimer car deja calculer dans SUIVI_

        TRACE("appel : %d : MENU_AZIMUTAL" , gp_Sui->menu) ;

        pthread_mutex_lock(& gp_Sui->p_Pth->mutex_azi );   
        pthread_mutex_lock(& gp_Sui->p_Pth->mutex_alt );

        gp_Sui->acc_alt          = 1 ;
        gp_Sui->acc_azi          = 1 ;

        pthread_mutex_unlock(& gp_Sui->p_Pth->mutex_azi );   
        pthread_mutex_unlock(& gp_Sui->p_Pth->mutex_alt );

        gp_Sui->SUIVI_EQUATORIAL = 0 ;
        gp_Sui->SUIVI_VOUTE      = 1 ; 

				CALCUL_TOUT() ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ; 

      break ;

      // ------------------------------  MODE EQUATORIAL --------------------------------------
     
      case MENU_EQUATORIAL :

        pthread_mutex_lock(& gp_Sui->p_Pth->mutex_azi );   
        pthread_mutex_lock(& gp_Sui->p_Pth->mutex_alt );

        gp_Sui->acc_alt          = 0 ;
        gp_Sui->acc_azi          = 1 ;

        pthread_mutex_unlock(& gp_Sui->p_Pth->mutex_azi );   
        pthread_mutex_unlock(& gp_Sui->p_Pth->mutex_alt );

        gp_Sui->SUIVI_EQUATORIAL = 1 ;
        gp_Sui->SUIVI_VOUTE      = 0 ; 

				CALCUL_TOUT() ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ;

      break ;

      // -------------------------- SUIVI MANUEL PAR DEFAUT ------------------
      
      case MENU_MANUEL_BRUT :

        // le but de ce suivi est de deduire des actions N-S-O-E de l'utilisateur 
        // simplement une acceleration / ralentissement / changement de direction
        // sur le N-S-E-O (pas de recalcul des periodes) 
        // FIXME : les periodes sont conservees , ainsi que le mode azimutal ou equatorial

        if( strcmp( gp_Sui->datas_infrarouge, "") != 0 ) {
          Trace2("1 gp_Sui->datas_infrarouge = %s", gp_Sui->datas_infrarouge ) ;
        }

        SUIVI_MANUEL_BRUT(gp_Sui, gp_Clav) ;
        
        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ; 

      break ;

      /* -------------------------- MENU_MANUEL_NON_ASSERVI : LE PLUS SIMPLE ------------------
          TODO : a coder : juste appui des touches provoque le mouvement
          TODO : mode le plus simple a priori
          TODO : acc_azi_ et acc_alt doivent etre initialises a zero
      */
      
      case MENU_MANUEL_NON_ASSERVI :

        // Suivi le plus simple : seules les touches est nord sud ouest et reset sont prises en compte
        // TODO : verifier

        TRACE("appel : %d : MENU_MANUEL_NON_ASSERVI" , gp_Sui->menu) ;

        pthread_mutex_lock(& gp_Sui->p_Pth->mutex_azi );   
        pthread_mutex_lock(& gp_Sui->p_Pth->mutex_alt );

        gp_Sui->acc_alt          = 0 ;
        gp_Sui->acc_azi          = 0 ;

        pthread_mutex_unlock(& gp_Sui->p_Pth->mutex_azi );   
        pthread_mutex_unlock(& gp_Sui->p_Pth->mutex_alt );

				CALCUL_TOUT() ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_NON_ASSERVI ; 

      break ;

      // -------------------------- SUIVI MANUEL AVEC RE-CALCUL DES PERIODES  -----------------
      case MENU_MANUEL_ASSERVI :
       
        // TODO : a modifier car cela marche pas tres bien (interraction avec le thread SUIVI_VOUTE)
        // TODO : le but de ce gp_Sui est de deduire des actions N-S-O-E de l'utilisateur 
        // TODO : les periodes / frequences en azimut et altitude

        SUIVI_MANUEL_ASSERVI(gp_Sui, gp_Clav) ; 
        CALCUL_PERIODES_SUIVI_MANUEL(gp_Astr,gp_Sui,gp_Vout)  ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_ASSERVI ; 

      break ;

      // ------------------------------  MODE GOTO --------------------------------------

      case MENU_GOTO :

        TRACE("appel : %d : MENU_GOTO" , gp_Sui->menu) ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ;

      break ; 

      // ------------------------------  MODE INFO --------------------------------------

      case MENU_INFO :

        TRACE("appel : %d : MENU_INFO" , gp_Sui->menu) ;

        CONFIG_AFFICHER_TOUT() ;

        gp_Sui->menu_old         = gp_Sui->menu ;
        gp_Sui->menu             = MENU_MANUEL_BRUT ;

      break ; 

      // ------------------------------ MODE ACTIVATION DU RESEAU ----------------------------------
     
      case MENU_RESEAU_UP :

        TRACE("appel : %d : MENU_RESEAU_UP" , gp_Sui->menu) ;

        if ( system("/etc/init.d/OLD/S40network start")) {
         perror("Probleme avec system(/etc/init.d/OLD/S40network start)"); 
         Trace("Probleme avec system(/etc/init.d/OLD/S40network start)\n") ;
        }

        gp_Sui->menu = gp_Sui->menu_old ;
        gp_Sui->menu = MENU_MANUEL_BRUT ;

      break ;

      // ------------------------------ ARRET DU RESEAU ---------------------------------------
     
      case MENU_RESEAU_DOWN :
     
        TRACE("appel : %d : MENU_RESEAU_DOWN" , gp_Sui->menu) ;

        if ( system("/etc/init.d/OLD/S40network stop")) {
          perror("Probleme avec system(/etc/init.d/OLD/S40network stop)"); 
          Trace("Probleme avec system(/etc/init.d/OLD/S40network stop)\n") ;
        }

        gp_Sui->menu = gp_Sui->menu_old ;
        gp_Sui->menu = MENU_MANUEL_BRUT ;

      break ;
      // ------------------------------ ARRET DU PROGRAMME ------------------------------------
      case MENU_PROGRAMME_DOWN :
     
        TRACE("appel : %d : MENU_PROGRAMME_DOWN" , gp_Sui->menu) ;

        TRAP_MAIN(1) ;
        break ;
     
      // ------------------------------ ARRET DU SYSTEME EMBARQUE -----------------------------
      case MENU_DOWN :

        TRACE("appel : %d : MENU_DOWN" , gp_Sui->menu) ;

        TRAP_MAIN(0) ;

      break ;
    }
  }
  Trace("Stop") ;

  return NULL ;
}
/*****************************************************************************************
* @fn     : SUIVI_VOUTE
* @author : s.gravois
* @brief  : Ce mode permet de gerer la voute c'est a dire le rafraichissement des calculs
* @param  : SUIVI   *gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_id_thread++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : 
*****************************************************************************************/

/*
   SUIVI_VOUTE :
   le but de la fonction est de rafraichir a intervalles reguliers (1 seconde)
   tous les calculs relatifs a la vitesse de l'as suivi
*/

void * SUIVI_VOUTE(SUIVI * gp_Sui) {
  
  char c_l0[16] ;
  char c_l1[16] ;

  unsigned long  incr ;
  struct timeval t00 ;
  struct sched_param param;
  /*
  char   c_Vitesses[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure] ;
  */
  // La temporisation dans la boucle du thread SUIVI_VOUTE depend de gp_Vout->DT (en us)
  // a completer / modifier :
  // il FAUT calculer / mettre a jour a,h,A,H de l'as quand est utliser les menus
  // suivi manuel et suivi capteurs , qui renvoient l'azimut et l'altitude
  // ==> CALCUL_EQUATEUR pour les devices altitude et azimut venant du capteur
  // ==> autre calcul plus complique quand on a les devices de vitesses et periodes provenant de la raquette !!!!!
  
  memset( c_l0, ZERO_CHAR, strlen( c_l0 )) ;
  memset( c_l1, ZERO_CHAR, strlen( c_l1 )) ;

  Trace2("sleeping..") ;
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_VOUTE ) ;
  Trace("start") ;
  
  if ( gc_user_id == USERID_ROOT ) {
    param.sched_priority = PTHREAD_POLICY_1 ;

    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_VOUTE, & param) != 0) {
      perror("setschedparam SUIVI_VOUTE"); 
      exit(EXIT_FAILURE);
    }
  }
  else { 
    Trace("gc_user_id not USERID_ROOT => cannot pthread_setschedparam(args)") ;
  }

  pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
  pthread_setname_np( pthread_self(), "SUIVI_VOUTE" ) ;
  gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
  pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;

  signal( SIGTERM, TRAP_SUIVI_VOUTE) ;
  
  gp_Sui->d_temps = 0 ;
  incr =0 ;
  
  gettimeofday(&t00,NULL) ;
  
  CONFIG_VOUTE( gp_Vout, 1, 1, 0.985 ) ;
  
  TRACE1("gp_Sui->SUIVI_VOUTE=%d",gp_Sui->SUIVI_VOUTE) ;

  // FIXME : 
  // en mode equatorial, pas besoin de SUIVI_VOUTE, en effet la vitesse ne change pas
  // Va=15"/s et Vh=0 (le moteur en ascension droite est commande par azimut)
  
  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread SUIVI_VOUTE
  //-------------------------------------------------------------------------------
  
  gp_Sui->SUIVI_ALIGNEMENT=1 ;

  Trace2("debut while") ;

  while(1) {
    
    if ( gp_Sui->SUIVI_VOUTE ) {
      
      /* FIXME : modification 20121225 : tous les calculs generiques dans CALCUL_TOUT */
      
      CALCUL_TOUT() ;
      /* Exceptionnellement , utilisation variables globales */ 
      /* LCD_DISPLAY_TEMPS_LIEU(0,gp_Lieu,gp_Time) ;*/

      if ( gp_Sui->SUIVI_ALIGNEMENT ) { 

        CONFIG_FORMATE_DONNEES_AFFICHAGE(gp_Astr) ;
        CONFIG_AFFICHER_MODE_LONG() ; 
        CONFIG_AFFICHER_MODE_STELLARIUM(gp_Astr) ;
        
        gp_Lcd->display_ast_vit(2000000) ;

        sprintf( c_l0, "%d%s%d %-2d:%-2d", \
          gp_Time->yy , \
          c_Lcd_Display_Months[ gp_Time->mm -1  ] , \
          gp_Time->dd, \
          gp_Time->HH, \
          gp_Time->MM ) ;
      
        sprintf( c_l1, "%s", gp_Astr->nom ) ;
        
        gp_Lcd->change_default( 2000000, c_l0 , c_l1 ) ;
        
        gp_Sui->SUIVI_ALIGNEMENT = 0 ;
      }
/*
      if ( gp_Sui->menu_old != gp_Sui->menu  ) {
        CONFIG_AFFICHER_TOUT() ;
      }
*/
      gp_Astr->AGH   += gp_Vout->pas ;
      gp_Vout->deb += gp_Vout->pas ;

      gp_Sui->d_temps += CALCUL_TEMPORISATION_VOUTE( gp_Vout, t00 ) ; 
      gettimeofday(&t00,NULL) ;

			gp_Vout->num ++ ;
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
      usleep( gp_Vout->DT );
    }
  }
  Trace("Stop") ;

  return NULL ; 
}
/*****************************************************************************************
* @fn     : SUIVI_INFRAROUGE
* @author : s.gravois
* @brief  : fonction de callback du thread suivi infrarouge
* @param  : SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_id_thread++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qi est variable globale
*****************************************************************************************/

void * SUIVI_INFRAROUGE(SUIVI * gp_Sui) {
   
  struct sched_param param;
    
  Trace2("sleeping..") ;
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_INFRA ) ;
  Trace("start") ;
  
  if ( gp_Devi->DEVICE_USE_INFRAROUGE ) {

    if ( gc_user_id == USERID_ROOT ) {
      param.sched_priority = PTHREAD_POLICY_1  ;
      
      if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_INFRA, & param) != 0) { 
        perror("setschedparam SUIVI_INFRAROUGE"); 
        exit(EXIT_FAILURE);
      }
    }
    else { 
      Trace("gc_user_id not USERID_ROOT => cannot pthread_setschedparam(args)") ;
    } 

    pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
    pthread_setname_np( pthread_self(), "SUIVI_IR" ) ;
    gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
    pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;
    
    signal( SIGTERM, TRAP_SUIVI_INFRAROUGE) ;
  
    LIRC_OPEN( lircconfig ) ;
  
    // ATTENTION !!! la fonction LIRC_READ est bloquante (voir ir.c) !!!!!
    
    LIRC_READ( gp_Sui ) ;
    LIRC_CLOSE(lircconfig) ;
  }

  Trace("Stop") ;

  return NULL ;
}
/*****************************************************************************************
* @fn     : SUIVI_LCD
* @author : s.gravois
* @brief  : fonction de callback du thread suivi ecran LCD
* @param  : SUIVI * gp_Sui
* @date   : 2022-04-12 creation 
* @date   : 2022-04-27 mise en commentaire de LCD_DISPLAY_TEMPS_LIEU
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_id_thread++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : TODO : reflechir a ce qui doit etre rafraichi
*****************************************************************************************/

void * SUIVI_LCD(SUIVI * gp_Sui) {
  int i_duree_us ;
  struct sched_param param;
  char c_l0[16] ;
  char c_l1[16] ;

  memset( c_l0, ZERO_CHAR, strlen( c_l0 )) ;
  memset( c_l1, ZERO_CHAR, strlen( c_l1 )) ;
    
  Trace2("sleeping..") ;
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_LCD ) ;
  Trace("start") ;
  
  if ( gp_Devi->DEVICE_USE_LCD ) {

    /* param.sched_priority = PTHREAD_POLICY_0  ; :  */
    /* pthread_setschedparam : invalide argument si 0 */

    if ( gc_user_id == USERID_ROOT ) {

      param.sched_priority = PTHREAD_POLICY_2  ; 

      if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_LCD, & param) != 0) { 
        perror("setschedparam SUIVI_LCD"); 
        exit(EXIT_FAILURE);
      }
    }
    else { 
      Trace("gc_user_id not USERID_ROOT => cannot pthread_setschedparam(args)") ;
    } 

    pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
    pthread_setname_np( pthread_self(), "SUIVI_LCD" ) ;
    gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
    pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;

    signal( SIGTERM, TRAP_SUIVI_LCD) ;

    gp_Lcd->display_default() ;

    Trace2("debut while") ;

    while(1) {
      
      usleep( gp_Sui->temporisation_lcd_loop );
      
      /* Si un changement de lignes a ete effectue dans une partie du programme */

      if ( gp_Lcd->i_change_current == TRUE ) {
       
        /* on additionne la duree affichage de base et
           la duree d'affichage i_duree_us envoyee par les fcts  */
        usleep( gp_Sui->temporisation_lcd_disp );
        usleep( gp_Lcd->i_duree_us );

        gp_Lcd->display_default() ;

        gp_Lcd->i_change_current = FALSE ;
      }
      
      /* Si un changement de date a lieu et que cela fait partie du changement par défaut
         alors on effectue un changement de affichage par defaut */
      /* TODO : coder */
      
      if ( FALSE ) { 

        sprintf( c_l0, "%d%s%d %-2d:%-2d", \
          gp_Time->yy , \
          c_Lcd_Display_Months[ gp_Time->mm -1  ] , \
          gp_Time->dd, \
          gp_Time->HH, \
          gp_Time->MM ) ;
      
        sprintf( c_l1, "%s", gp_Astr->nom ) ;
        
        gp_Lcd->change_default( 2000000, c_l0 , c_l1 ) ;
        
        gp_Sui->SUIVI_ALIGNEMENT = 0 ;
      }
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
* @param  : SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-04-12 correction code ( repetition if ( i_indice_code < CONFIG_CODE_NB_CODES ))
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_id_thread++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CLAVIER_TERMIOS( SUIVI * gp_Sui ) {

  int i_indice_code=0 ;
  int c_char =0 ;
  int i_sum_ascii =0 ; 
  char c_sum_ascii[8] ;
  char ch_chaine[TERMIOS_KBHIT_SIZE_BUFFER_READ] ;
  struct sched_param param;
  struct timeval t00,t01  ;
  
  Trace2("sleeping..") ;
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
  Trace("start") ;
    
  if ( gp_Devi->DEVICE_USE_KEYBOARD ) {

    if ( gc_user_id == USERID_ROOT ) {

      param.sched_priority = PTHREAD_POLICY_1  ;
      if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_CLAVIER, & param) != 0) { 
        perror("setschedparam SUIVI_CLAVIER_TERMIOS"); exit(EXIT_FAILURE);
      }
    }
    else { 
      Trace("gc_user_id not USERID_ROOT => cannot pthread_setschedparam(args)") ;
    } 

    pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
    pthread_setname_np( pthread_self(), "SUIVI_TERMIOS" ) ;
    gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
    pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;
    
    signal( SIGTERM, TRAP_SUIVI_CLAVIER) ;
  
    KEYBOARD_TERMIOS_INIT() ;

    Trace2("debut while") ;

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
        while(strcmp(gp_Cod->in_term[i_indice_code],c_sum_ascii) && ( i_indice_code < CONFIG_CODE_NB_CODES ) ){ 
           Trace1("%s = %s ?", c_sum_ascii, gp_Cod->in_term[i_indice_code]) ;  
           i_indice_code++ ; 
        }
        pthread_mutex_lock(& gp_Sui->p_Pth->mutex_infrarouge );
        memset( gp_Sui->datas_infrarouge, '\0', strlen( gp_Sui->datas_infrarouge ) ) ;
        pthread_mutex_unlock(& gp_Sui->p_Pth->mutex_infrarouge );

        if ( i_indice_code < CONFIG_CODE_NB_CODES ) {
          pthread_mutex_lock(& gp_Sui->p_Pth->mutex_infrarouge );
          Trace2("A maj gp_Sui->datas_infrarouge") ;
          strcpy( gp_Sui->datas_infrarouge, gp_Cod->out_act[i_indice_code] ) ;
          pthread_mutex_unlock(& gp_Sui->p_Pth->mutex_infrarouge );

          GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 10) ;
        }
        // tres important !!
        // le usleep suivant permet de garder l information !!!!!!
        // gp_Sui->datas_infrarouge fonctionne comme un TAMPON
        // il va etre lu par les threads du programme principal

        Trace("gp_Sui->datas_infrarouge = %s", gp_Sui->datas_infrarouge ) ;
        Trace1("gp_Sui->temporisation_termios = %ld", gp_Sui->temporisation_termios ) ;

        usleep( gp_Sui->temporisation_termios ) ;

        pthread_mutex_lock(& gp_Sui->p_Pth->mutex_infrarouge );
        memset( gp_Sui->datas_infrarouge, 0, strlen( gp_Sui->datas_infrarouge ) ) ;
        Trace2("F remise a zero gp_Sui->datas_infrarouge") ;
        strcpy( gp_Sui->datas_infrarouge, "") ;
        pthread_mutex_unlock(& gp_Sui->p_Pth->mutex_infrarouge );
      }
    }

    KEYBOARD_TERMIOS_EXIT() ;

    if ( c_char == 'q' ) {
      TRAP_MAIN(1) ;
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
* @param  : SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_id_thread++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CLAVIER_getchar( SUIVI * gp_Sui ) {

  int c_char = 0 ;
  struct sched_param param;

  Trace2("sleeping..") ;
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
  Trace("start") ;

  if ( gp_Devi->DEVICE_USE_KEYBOARD ) {
    
    if ( gc_user_id == USERID_ROOT ) {
      param.sched_priority = PTHREAD_POLICY_1  ;
      
      if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_CLAVIER, & param) != 0) { 
        perror("setschedparam SUIVI_CLAVIER"); exit(EXIT_FAILURE);
      }
    }
    else { 
      Trace("gc_user_id not USERID_ROOT => cannot pthread_setschedparam(args)") ;
    } 

    pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
    pthread_setname_np( pthread_self(), "SUIVI_getchar" ) ;
    gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
    pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;

    signal( SIGTERM, TRAP_SUIVI_CLAVIER) ;

    Trace2("debut while") ;

    while( ( c_char = getchar () ) != 'q' ) {
      usleep(100000) ;
      TRACE("%c %d entre au clavier", c_char,c_char  ) ; 
    }
    if ( c_char == 'q' ) {
      TRAP_MAIN(1) ;
    }
    /* appels LIRC pour memoire : 
    LIRC_OPEN( lircconfig ) ;
    LIRC_READ( gp_Sui ) ;
    LIRC_CLOSE(lircconfig) ;*/
  }
  Trace("Stop") ;

  return NULL ;
}

/*****************************************************************************************
* @fn     : SUIVI_CLAVIER_NCURSES
* @author : s.gravois
* @brief  : fonction de callback du thread suivi clavier en mode ncurses
* @param  : SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_id_thread++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CLAVIER_NCURSES(SUIVI * gp_Sui ) {
  int ch = 0 ;
  unsigned long l_incr=0 ;
  struct sched_param param;

  Trace2("sleeping..") ;
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CLAVIER ) ;
  Trace("start") ;
  
  if ( gp_Devi->DEVICE_USE_KEYBOARD ) {

    if ( gc_user_id == USERID_ROOT ) {

      param.sched_priority = PTHREAD_POLICY_1  ;

      if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_CLAVIER, & param) != 0) { 
        perror("setschedparam SUIVI_CLAVIER"); exit(EXIT_FAILURE);
      }
    }
    else { 
      Trace("gc_user_id not USERID_ROOT => cannot pthread_setschedparam(args)") ;
    } 

    pthread_mutex_lock( & gp_Sui->p_Pth->mutex_pthread) ;
    pthread_setname_np( pthread_self(), "SUIVI_NCURSES" ) ;
    gp_Sui->p_Pth->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
    pthread_mutex_unlock( & gp_Sui->p_Pth->mutex_pthread) ;

    signal( SIGTERM, TRAP_SUIVI_CLAVIER) ;
    
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
    TRAP_MAIN(1) ;
  }
  Trace("Stop") ;

  return NULL ;
}

/*****************************************************************************************
* @fn     : SUIVI_CAPTEURS
* @author : s.gravois
* @brief  : fonction de callback du thread suivi capteurs (non utilisee)
* @param  : SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ g_id_thread++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CAPTEURS(SUIVI * gp_Sui) {
   
  struct sched_param param;
  int ret ;
  
  I2C_DEVICE   exemple, *ex ;
  I2C_ACC_MAG  accmag,  *am ;
  PTHREADS *   lp_Pthr = (PTHREADS *)gp_Sui->p_Pth ;

  Trace2("sleeping..") ;
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_CAPTEUR ) ;
  Trace("start") ;

  ex = & exemple ;
  am = & accmag ;
  ex->fd = 0 ;
  
  if ( gp_Devi->DEVICE_USE_CAPTEURS ) {
      
    // La temporisation dans la boucle du thread SUIVI_CAPTEURS depend de gp_Sui->temporisation_capteurs (en us)
    // present dans la fonction bloquante LIRC_READ 
    
    // on laisse le temps aux autres threads de demarrer
    // notamment pour arriver dans la fonction SUIVI_TRAITEMENT_MENU
    // qui initialise a 1 SUIVI_CAPTEURS
    
    if ( gc_user_id == USERID_ROOT ) {

      param.sched_priority = PTHREAD_POLICY_1  ;
    
      if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_CAPTEUR, & param) != 0) {
        perror("setschedparam SUIVI_CAPTEURS"); exit(EXIT_FAILURE);
      }
    }
    else { 
      Trace("gc_user_id not USERID_ROOT => cannot pthread_setschedparam(args)") ;
    } 
    pthread_mutex_lock( & lp_Pthr->mutex_pthread) ;
    pthread_setname_np( pthread_self(), "SUIVI_CAPTEURS" ) ;
    lp_Pthr->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
    pthread_mutex_unlock( & lp_Pthr->mutex_pthread) ;
    
    signal( SIGTERM, TRAP_SUIVI_CAPTEURS) ;
    
    // a modifier pour definir le choix de l'infrarouge ou pas (config.txt ? .h ? )
    
    Trace2("debut while") ;

    while(1) {
      
      if ( ! gp_Devi->init_capteurs ) {
        
        ret = I2C_INIT(ex, DEVICE_RASPI_2, DEVICE_LSM_ADRESS ) ;

        if ( ! ret ) {
          Trace("Pas de capteur disponible") ;
          gp_Devi->DEVICE_USE_CAPTEURS = 0 ;
          gp_Devi->init_capteurs = 0 ;
          break ;
        }
        else {
          I2C_SET( ex, REG_CTRL1, "0x57" ) ;
          I2C_SET( ex, REG_CTRL2, "0x00" ) ;
          I2C_SET( ex, REG_CTRL5, "0x64" ) ;
          I2C_SET( ex, REG_CTRL6, "0x20" ) ;
          I2C_SET( ex, REG_CTRL7, "0x00" ) ;
    
          gp_Devi->init_capteurs = 1 ;
        }
      }
      if ( gp_Devi->init_capteurs ) {
      
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

  GPIO_PWM_MOTEUR *pm_alt , m_alt ; 
  GPIO_PWM_MOTEUR *pm_azi , m_azi ;

  int i2cDev = 1;

  // -----------------------------------------------------------------
  // recuperer le chemin courant avec getcwd
  // -----------------------------------------------------------------

  /* Attention, pas de TRACE avant d avoir ouvert le fichier de log */ 
  /* car TRACE utilise ce fichier */
  /* Alternative : Trace */ 

  /* ---------------------------------------------------------------
  * Gestion d un chemin externe (option -p <path>) si getcwd KO
  * ---------------------------------------------------------------*/

  g_i_trace = 0 ;
  g_i_trace_alt = 0 ;
  g_i_trace_azi = 0 ;
  g_i_timeout = 0 ;
  g_i_max_nb_pas = 0 ;
  g_i_max_nb_micropas = 0 ;

  ARGUMENTS_GERER_REP_HOME(argc, argv) ;
  
  g_incrlog=0 ;
  g_id_thread=0 ;

  // -----------------------------------------------------------------
  // Initialisations des structures et pointeurs sur structure
  // -----------------------------------------------------------------

  gp_Pthr    = & g_Pth;
  gp_Astr    = & ast ;
  gp_Lieu    = & lie;
  gp_Vout    = & vou ;
  gp_Time    = & tem ;
  gp_Clav    = & cla ;
  gp_Devi    = & dev ;
  gp_Cod     = & g_Cod ;
  gp_Lcd     = & g_Lcd ;
  gp_Pid     = & g_Pid ;
  pm_azi     = & m_azi ;
  pm_alt     = & m_alt ;
  gp_Sui     = & sui ;
  
  /* Pour permettre acces a PTHREADS* via struct SUIVI* */

  gp_Sui->p_Pth = (PTHREADS*) gp_Pthr ; 

  /* Pour permettre acces a SUIVI* via struct GPIO_PWM_MOTEUR* */

  pm_azi->p_Sui = (SUIVI*)gp_Sui ;   
  pm_alt->p_Sui = (SUIVI*)gp_Sui ;   
  
  /* Pour permettre acces a SUIVI* via struct GPIO_PWM_MOTEUR* */

  pm_alt->p_Pth = (PTHREADS*)gp_Pthr ;
  pm_azi->p_Pth = (PTHREADS*)gp_Pthr ; 

  /* Pour permettre acces a SUIVI* via struct GPIO_PWM_MOTEUR* */

  CONFIG_PATH_FIND( g_Path_Cmd_Stty, "stty") ;
  Trace("Chemin retenu : %s", g_Path_Cmd_Stty) ;
  // -----------------------------------------------------------------
  // Initialisations diverses et variees
  // -----------------------------------------------------------------

  CONFIG_READ       ( g_Datas ) ;
  GPIO_READ         ( g_Datas ) ; 

  CONFIG_AFFICHER_DATAS ( g_Datas ) ;
  CONFIG_INIT_VAR       ( g_Datas ) ;

  CONFIG_AFFICHER_VARIABLES() ;   
  CONFIG_INIT_LOG(); 

  GPIO_RAQUETTE_CONFIG( gpio_key_l, gpio_key_c ) ;
  
  CONFIG_INIT_VOUTE     ( gp_Vout ) ; /* soit etre place avant CONFIG_INIT_SUIVI */
  CONFIG_INIT_CODES     ( gp_Cod ) ;
  CONFIG_INIT_CLAVIER   ( gp_Clav ) ;   
  CONFIG_INIT_ASTRE     ( gp_Astr ) ;
  CONFIG_INIT_LIEU      ( gp_Lieu  ) ;
  CONFIG_INIT_TEMPS     ( gp_Time ) ;
  CONFIG_INIT_DEVICES   ( gp_Devi ) ;

  CALCUL_TEMPS_SIDERAL  ( gp_Lieu, gp_Time) ;

  LCD_INIT              ( gp_Lcd ) ;
  PID_INIT              ( gp_Pid, PID_ECH, PID_KP, PID_KI, PID_KD ) ;

  PTHREADS_INIT         () ;

  CONFIG_AFFICHER_DEVICES_USE() ;

  CONFIG_INIT_SUIVI     ( gp_Sui ) ;

  /* pthread init a mettre ici */

  signal(SIGINT,TRAP_MAIN) ;
  signal(SIGALRM,TRAP_MAIN) ;

  Trace("gpio_alt         : %d %d %d %d", gpio_alt[0], gpio_alt[1], gpio_alt[2], gpio_alt[3] ) ;
  Trace("gpio_azi         : %d %d %d %d", gpio_azi[0], gpio_azi[1], gpio_azi[2], gpio_azi[3] ) ;
  Trace("gpio_mas         : %d %d %d %d", gpio_mas[0], gpio_mas[1], gpio_mas[2], gpio_mas[3] ) ;
  Trace("GPIO_LED_ETAT    : %d", GPIO_LED_ETAT );
  Trace("ASTRE_PAR_DEFAUT : %s", ASTRE_PAR_DEFAUT) ;
  
  Trace("PID_ECH = %f",  PID_ECH);
  
  // -----------------------------------------------------------------
  // reglages variables particulieres
  // ----------------------------------------------------------------- 
  
  CAT_READ            ( CAT_NGC_TXT, cat) ;                     // CAT_AFFICHER ( cat ) ; 
  CAT_FORMAT_DECIMAL  ( "ngc.csv.dec" , cat, cat_dec) ;         // CAT_AFFICHER ( cat_dec ) ;

  CAT_READ            ( CAT_ETO_TXT, etoiles) ;                 // CAT_AFFICHER ( etoiles ) ;
  CAT_FORMAT_DECIMAL_2( "eto.csv.dec" , etoiles, etoiles_dec) ; // CAT_AFFICHER ( etoiles_dec ) ;
  // -----------------------------------------------------------------
  
  if ( strcmp(ASTRE_PAR_DEFAUT,"") != 0 ) {

    memset( gp_Astr->nom, 0, sizeof(gp_Astr->nom)) ;
    strcpy( gp_Astr->nom, ASTRE_PAR_DEFAUT ) ;

    if ( strstr( gp_Astr->nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Astr, cat_dec) ;
    if ( strstr( gp_Astr->nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Astr, cat_dec) ;
    if ( strstr( gp_Astr->nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Astr, etoiles_dec) ;

    gp_Sui->SUIVI_ALIGNEMENT = 1 ;
  }

  // -----------------------------------------------------------------

  ARGUMENTS_HELP                  ( argc, argv ) ;
  ARGUMENTS_GERER_FACON_CLASSIQUE ( argc, argv  ) ;
  
  if ( gp_Sui->alarme != 0 ) {
    alarm( gp_Sui->alarme) ;
  }  

  // ouverture led etat ----------------------------------------------

  if ( GPIO_LED_ETAT != 0 ) {

    GPIO_CLOSE_BROCHE( GPIO_LED_ETAT) ;
    GPIO_OPEN_BROCHE( GPIO_LED_ETAT, 1) ;
    GPIO_SET( GPIO_LED_ETAT, 0 ) ;
  }
  
  // -----------------------------------------------------------------
  
  GPIO_INIT_PWM_MOTEUR(\
    pm_alt,\
    gpio_alt,\
    gpio_mas,\
    ALT_R4 ,\
    100,\
    gpio_frequence_pwm, \
    0, \
    GPIO_CURRENT_FONCTION,\
    GPIO_CURRENT_FONCTION_PARAM0,\
    GPIO_CURRENT_FONCTION_PARAM1 ) ;

  GPIO_INIT_PWM_MOTEUR(\
    pm_azi,\
    gpio_azi,\
    gpio_mas,\
    AZI_R4 ,\
    100,\
    gpio_frequence_pwm, \
    1, \
    GPIO_CURRENT_FONCTION,\
    GPIO_CURRENT_FONCTION_PARAM0,\
    GPIO_CURRENT_FONCTION_PARAM1 ) ;
  
  Trace("pm_azi->Fm %f pm_azi->periode_mot = %f", pm_azi->Fm, pm_azi->periode_mot ) ;
  Trace("pm_alt->Fm %f pm_alt->periode_mot = %f", pm_alt->Fm, pm_alt->periode_mot ) ;

  for( i=0 ; i< ALT_R4 ; i++ ) {
    TRACE1("%d\t%f\t%f\t%f\t%f",i,\
      pm_azi->phase[0]->rap[i],\
      pm_azi->phase[1]->rap[i],\
      pm_azi->phase[2]->rap[i],\
      pm_azi->phase[3]->rap[i]) ;
  }

  // ============================== gestion des threads  ===================================

  pthread_create( &p_thread_pha_azi[0],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_azi->phase[0] ) ;
  pthread_create( &p_thread_pha_azi[1],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_azi->phase[1] ) ;
  pthread_create( &p_thread_pha_azi[2],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_azi->phase[2] ) ;
  pthread_create( &p_thread_pha_azi[3],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_azi->phase[3] ) ;

  pthread_create( &p_thread_pha_alt[0],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_alt->phase[0] ) ;
  pthread_create( &p_thread_pha_alt[1],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_alt->phase[1] ) ;
  pthread_create( &p_thread_pha_alt[2],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_alt->phase[2] ) ;
  pthread_create( &p_thread_pha_alt[3],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_alt->phase[3] ) ;

  pthread_create( &p_thread_mot_azi,           NULL, (void*)suivi_main_M, pm_azi ) ;
  pthread_create( &p_thread_mot_alt,           NULL, (void*)suivi_main_M, pm_alt ) ;

  pthread_create( &gp_Sui->p_Pth->p_menu,            NULL, (void*)SUIVI_MENU,      gp_Sui ) ;
  pthread_create( &gp_Sui->p_Pth->p_suivi_voute,     NULL, (void*)SUIVI_VOUTE,     gp_Sui ) ;
  pthread_create( &gp_Sui->p_Pth->p_suivi_infrarouge,NULL, (void*)SUIVI_INFRAROUGE, gp_Sui ) ;
  pthread_create( &gp_Sui->p_Pth->p_suivi_capteurs,  NULL, (void*)SUIVI_CAPTEURS,  gp_Sui ) ;
  pthread_create( &gp_Sui->p_Pth->p_suivi_clavier,   NULL, (void*)SUIVI_CLAVIER_TERMIOS,  gp_Sui ) ;
  pthread_create( &gp_Sui->p_Pth->p_suivi_lcd,       NULL, (void*)SUIVI_LCD,  gp_Sui ) ;

  // ============================== join des threads  ===================================

  if ( gp_Devi->DEVICE_USE_LCD )         pthread_join( gp_Sui->p_Pth->p_suivi_lcd, NULL) ;
  if ( gp_Devi->DEVICE_USE_KEYBOARD )    pthread_join( gp_Sui->p_Pth->p_suivi_clavier, NULL) ;
  if ( gp_Devi->DEVICE_USE_CAPTEURS )    pthread_join( gp_Sui->p_Pth->p_suivi_capteurs, NULL) ;
  if ( gp_Devi->DEVICE_USE_INFRAROUGE )  pthread_join( gp_Sui->p_Pth->p_suivi_infrarouge, NULL) ;

  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_join( p_thread_pha_azi[i], NULL) ; 
  }
  pthread_join( p_thread_mot_azi, NULL) ;
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_join( p_thread_pha_alt[i], NULL) ; 
  }
  pthread_join( p_thread_mot_alt, NULL) ;
  
  pthread_join( gp_Sui->p_Pth->p_menu, NULL) ;
  pthread_join( gp_Sui->p_Pth->p_suivi_voute, NULL) ;  

  return 0;
}
// #######################################################################################
// ####### FIN PROGRAMME - FIN PROGRAMME - FIN PROGRAMME - FIN PROGRAMME         - #######
// #######################################################################################

/*
void * SUIVI_CLAVIER_1(SUIVI * gp_Sui) {
//  char ch = 0 ;
  struct sched_param param; 
  struct timeval previous;
  int n=0;
  int ch=0;   
  Trace("start") ;
  
  param.sched_priority = PTHREAD_POLICY_1  ;
  
  if (pthread_setschedparam( pthread_self(), SCHED_RR, & param) != 0) { 
    perror("setschedparam SUIVI_CLAVIER"); exit(EXIT_FAILURE);
  }
  gp_Pthr->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
  signal( SIGTERM, TRAP_SUIVI_CLAVIER) ;
  
  if ( gp_Devi->DEVICE_USE_KEYBOARD ) {


  // unlink(MY_LOGFILE);

  TRACE("Initialisation newterm") ; 

  if (newterm(0, stdout, stdin) == 0) {
    fprintf(stderr, "Cannot initialize terminal\n");
    TRACE("Cannot initialize terminal") ;
    exit(2);
  }
  TRACE("Initialisation cbreak") ; 
  cbreak();		     
  TRACE("Initialisation noecho") ; 
  noecho();		    
  TRACE("Initialisation scrollok") ; 
  scrollok(stdscr, FALSE);
  TRACE("Initialisation keypad") ; 
  keypad(stdscr, TRUE );
  TRACE("Initialisation move") ; 
  move(10, 10);

  refresh();

  // we do the define_key() calls after keypad(), since the first call to
  // keypad() initializes the corresponding data.
  //
  
  TRACE("Initialisation define_key") ; 

  for (n = 0; n < 255; ++n) {
    char temp[10];
    _nc_SPRINTF(temp, _nc_SLIMIT(sizeof(temp)) "\033%c", n);
    define_key(temp, n + MY_KEYS);
  }
  
  TRACE("Initialisation define_key (suite)") ; 

  for (n = KEY_MIN; n < KEY_MAX; ++n) {

    char *value;
    
    if ((value = keybound(n, 0)) != 0) {
        size_t need = strlen(value) + 2;
        char *temp = typeMalloc(char, need);
        _nc_SPRINTF(temp, _nc_SLIMIT(need) "\033%s", value);
        define_key(temp, n + MY_KEYS);
        free(temp);
        free(value);
    }
  }
  
 
  // gettimeofday(&previous, 0);
 
  TRACE("Initialisation while()") ; 

  sleep(1) ;
  clear() ;
  refresh();

  while ((ch = getch()) != 0 ) {
  
    usleep( gp_Sui->temporisation_clavier );

    bool         escaped = (ch >= MY_KEYS);
    const char * name = keyname(escaped ? (ch - MY_KEYS) : ch);
    int          secs, msecs;
     struct timeval current; 

    gettimeofday(&current, 0);
    secs = (int) (current.tv_sec - previous.tv_sec);
    msecs = (int) ((current.tv_usec - previous.tv_usec) / 1000);
    if (msecs < 0) {
      msecs += 1000;
      --secs;
    }
    if (msecs >= 1000) {
      secs += msecs / 1000;
      msecs %= 1000;
    }
    
    printw("%6d.%03d ", secs, msecs);
    previous = current;


    printw("Keycode %d, name %s%s\n",
      ch,
      escaped ? "ESC-" : "",
      name != 0 ? name : "<null>");
      KEYBOARD_LOG_LAST_LINE(stdscr);


    clear() ;

    if ( ch != ERR ) {
      TRACE("ch = %c", ch) ;
    }

    printw("Keycode %d, name %s%s\n",
      ch,
      escaped ? "ESC-" : "",
      name != 0 ? name : "<null>");
    
    refresh() ;

    // KEYBOARD_LOG_LAST_LINE(stdscr);

    // TRACE("appel clrtoeol") ; sleep(1) ; 
    clrtoeol();
    
  }
  TRACE("appel endwin") ; sleep(1) ;
  endwin();
  TRACE("appel exit(0)") ; sleep(1) ;
  exit(0) ;
  }
  return NULL ;
}
//==========================================================

void * SUIVI_CLAVIER_0(SUIVI * gp_Sui) {
//  char ch = 0 ;
  struct sched_param param; 
  struct timeval previous;
  int n=0;
  int ch=0;   
  Trace("start") ;
  
  param.sched_priority = PTHREAD_POLICY_1  ;
  
  if (pthread_setschedparam( pthread_self(), SCHED_RR, & param) != 0) { 
    perror("setschedparam SUIVI_CLAVIER"); exit(EXIT_FAILURE);
  }
  gp_Pthr->p_thread_t_id[ g_id_thread++ ] = pthread_self() ;
  signal( SIGTERM, TRAP_SUIVI_CLAVIER) ;
  
  if ( gp_Devi->DEVICE_USE_KEYBOARD ) {

  if (newterm(0, stdout, stdin) == 0) {
    fprintf(stderr, "Cannot initialize terminal\n");
    SyslogEno("Cannot initialize terminal") ;
    pthread_cancel( gp_Pthr->p_suivi_clavier ) ;
  }

  initscr() ;
  cbreak();		     
  noecho();		     
  scrollok(stdscr, TRUE);
  //keypad(stdscr, TRUE );
  move(0,0);
  sleep(1) ;
  printw("Fin initialisation\n");
  sleep(1) ;
  refresh();
  
  while ((ch = getch()) != ERR ) {
  
    usleep( 500000 );

    bool         escaped = (ch >= MY_KEYS);
    const char * name = keyname(escaped ? (ch - MY_KEYS) : ch);

    printw("Keycode %d, name %s%s\n",ch,escaped ? "ESC-" : "",name != 0 ? name : "<null>");
    
    refresh() ;

  }
  printw("appel endwin") ; sleep(1) ;
  endwin();
  TRACE("appel exit(0)") ; sleep(1) ;
  exit(0) ;
  }
  return NULL ;
}
*/