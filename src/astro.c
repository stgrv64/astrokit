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
# 17/01/2022  | * ajout gestion touches clavier (*_CLAVIER)
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
# avril 2002   | * debut codage fonctionnalites LCD1602
#                * suppressione appel GPIO_RAQUETTE_READ
#                * correction BUG ( repetition if ( i_indice_code < CONFIG_CODE_NB_CODES ))
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
#include <keyboard.h>

int        g_incrlog ;
int        g_id_thread ;  

/*****************************************************************************************
* @fn     : TRAP_MAIN
* @author : s.gravois
* @brief  : fonction appelle quand un signal est trape dans main
* @param  : int     sig
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : voir s il est preferable de faire un kill(getpid(),SIGKILL) 
*           plutot qu un exit
*****************************************************************************************/

void TRAP_MAIN(int sig) {

  char c_cmd[ 64 ] ;
  int i ;

  TRACE("Signal trappe de valeur (sig) %d",sig) ;
  TRACE("Envoi d'un signal %d (SIGTERM) aux threads",SIGTERM) ;
  
  GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 100) ;

  //Trace("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;

  /*--------------------------------------------------------*/
  /* Si sig > positif on abandonne les threads et on quitte */
  /*--------------------------------------------------------*/

  for(i=0;i<g_id_thread;i++)  {
    TRACE("Abandon thread numero %d de p_thread_t = %d",i,(int)suivi->p_threads_id[i]) ;
    pthread_cancel(suivi->p_threads_id[i]);
  }

  /*----------------------------------*/
  /* Rendre le terminal propre (sane) */
  /*----------------------------------*/

  memset( c_cmd, 0, sizeof(c_cmd)) ;
  sprintf(c_cmd,"%s sane", PATH_CMD_STTY ) ;

  if ( system( c_cmd ) < 0 ) {

    SyslogErr("Probleme avec stty sane\n") ;
    exit(2) ;
  } 

  /*----------------------------------*/
  /* Si sig == 0 on arrete le systeme */
  /*----------------------------------*/

  if (sig==0) {

    CONFIG_LCD_AFFICHER_STRING_INT(gp_Lcd,0,"Halt with sig :", sig) ;

    GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 100) ;

    if ( system("/sbin/halt") < 0 ) {
      SyslogErr("Probleme avec /sbin/halt\n") ;
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

  CONFIG_LCD_AFFICHER_STRING_INT(gp_Lcd,0,"Exit with pid :", getpid()) ;

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
* @todo   : (completer)
*****************************************************************************************/

void TRAP_SUIVI_MENU(int sig)  {
  
  Trace("Signal trappe depuis thread suivi_menu = %d\n",sig) ;
  CONFIG_LCD_AFFICHER_STRING_INT(gp_Lcd,0,"Signal trappe", sig) ;
  pthread_cancel( suivi->p_menu ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_VOUTE(int sig)  {
  
  Trace("Signal trappe depuis thread suivi_voute= %d\n",sig) ;
  CONFIG_LCD_AFFICHER_STRING_INT(gp_Lcd,0,"Signal trappe", sig) ;
  pthread_cancel( suivi->p_suivi_voute ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_INFRAROUGE(int sig)  {
  Trace("Signal trappe depuis thread suivi_infrarouge= %d\n",sig) ;
  CONFIG_LCD_AFFICHER_STRING_INT(gp_Lcd,0,"Signal trappe", sig) ;
  pthread_cancel( suivi->p_suivi_infrarouge ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_CAPTEURS(int sig)  {
  
  Trace("Signal trappe depuis thread suivi_capteurs= %d\n",sig) ;
  CONFIG_LCD_AFFICHER_STRING_INT(gp_Lcd,0,"Signal trappe", sig) ;
  pthread_cancel( suivi->p_suivi_capteurs ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_CLAVIER(int sig)  {
  
  Trace("Signal trappe depuis thread suivi_clavier= %d\n",sig) ;
  CONFIG_LCD_AFFICHER_STRING_INT(gp_Lcd,0,"Signal trappe", sig) ;
  pthread_cancel( suivi->p_suivi_clavier ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_TERMIOS(int sig)  {
  
  Trace("Signal trappe depuis thread suivi_termios= %d\n",sig) ;
  CONFIG_LCD_AFFICHER_STRING_INT(gp_Lcd,0,"Signal trappe", sig) ;
  pthread_cancel( suivi->p_suivi_clavier ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_LCD(int sig)  {
  
  Trace("Signal trappe depuis thread suivi_termios= %d\n",sig) ;
  CONFIG_LCD_AFFICHER_STRING_INT(gp_Lcd,0,"Signal trappe", sig) ;
  pthread_cancel( suivi->p_suivi_clavier ) ;
  TRAP_MAIN(1) ;
}
/*****************************************************************************************
* @fn     : SUIVI_MENU_PREALABLE
* @author : s.gravois
* @brief  : fonction qui initialise des attributs 
* @param  : SUIVI *suivi
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : analyser la reelle utilite de cette fonction
            en diminuant le bombre de variable suivi->var
*****************************************************************************************/

void SUIVI_MENU_PREALABLE (SUIVI *suivi) {

  switch ( suivi->menu ) {
    case MENU_AZIMUTAL            : suivi->SUIVI_EQUATORIAL = 0 ; break ;
    case MENU_EQUATORIAL          : suivi->SUIVI_EQUATORIAL = 1 ; break ;
    case MENU_MANUEL_BRUT         : suivi->SUIVI_MANUEL     = 1 ; break ;
    case MENU_MANUEL_NON_ASSERVI : suivi->SUIVI_MANUEL     = 1 ; break ;
    case MENU_MANUEL_ASSERVI      : suivi->SUIVI_MANUEL     = 1 ; break ;
    case MENU_GOTO                : suivi->SUIVI_GOTO       = 1 ; break ;
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
* @param  : SUIVI   *suivi
* @param  : CLAVIER *clavier 
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : analyser pour simplification <=> avec suivi menu etc..
*****************************************************************************************/

void SUIVI_TRAITEMENT_MOT( SUIVI *suivi, CLAVIER *clavier ) {
  
  char cmd[256] ;
  char s_buffer4[ CONFIG_TAILLE_BUFFER_32 * 4 ] ;

  memset( cmd,       ZERO_CHAR, strlen( cmd )) ;
  memset( s_buffer4, ZERO_CHAR, strlen( s_buffer4 )) ;
  
  //---------------------------------------------------------------------------------------------------------
  // TRAITEMENT DES ACTIONS SANS NECESSAIREMENT UNE VALIDATION
  // exemple : on appuie sur la touche ouest est sud nors : appel MENU_MANUEL_BRUT
  //---------------------------------------------------------------------------------------------------------
  
  // On change de MENU si on appuie sur EST OUEST NORD ou SUD => 
  // on passe en MENU 0 

  suivi->menu_old = suivi->menu ;

  /*  touche OUEST */

  if ( ! strcmp( clavier->mot, "o" )) { 
    suivi->menu = MENU_MANUEL_BRUT ; 
    strcpy(clavier->mot,"") ; 
  }

  /*  touche EST */

  if ( ! strcmp( clavier->mot, "e" )) { 
    suivi->menu = MENU_MANUEL_BRUT ; 
    strcpy(clavier->mot,"") ; 
  }

  /*  touche SUD */

  if ( ! strcmp( clavier->mot, "s" )) { 
    suivi->menu = MENU_MANUEL_BRUT ; 
    strcpy(clavier->mot,"") ; 
  }

  /*  touche NORD */

  if ( ! strcmp( clavier->mot, "n" )) { 
    suivi->menu = MENU_MANUEL_BRUT ; 
    strcpy(clavier->mot,"") ; 
  }

  /*  touche RESET */

  if ( ! strcmp( clavier->mot, "reset" ))     { 
    suivi->menu = MENU_MANUEL_BRUT ; 
    strcpy(clavier->mot,"") ; 
  }

  /*  touche STOP */

  if ( ! strcmp( clavier->mot, "stop" ))      { 
    suivi->SUIVI_VOUTE = 0 ; 
    strcpy(clavier->mot,"") ; 
  }

  /*  touche PLAY */

  if ( ! strcmp( clavier->mot, "play" ))      { 
    suivi->SUIVI_VOUTE = 1 ; 
    strcpy(clavier->mot,"") ; 
  }

  /*  touche aff_azi_alt */

  if ( ! strcmp( clavier->mot, "aff_azi_alt")) { 
    CONFIG_FORMATE_DONNEES_AFFICHAGE(as);
    CONFIG_LCD_AFFICHER_AZIMUT_ALTITUDE(gp_Lcd, 2, as ) ;
    /* suivi->menu = MENU_MANUEL_BRUT ; */
    strcpy(clavier->mot,"") ; 
  }

  /*  touche aff_azi_alt */

  if ( ! strcmp( clavier->mot, "aff_agh_dec")) { 
    CONFIG_FORMATE_DONNEES_AFFICHAGE(as);
    CONFIG_LCD_AFFICHER_ANGLE_HORAIRE_DECLINAISON(gp_Lcd, 2, as ) ;
    /* suivi->menu = MENU_MANUEL_BRUT ; */
    strcpy(clavier->mot,"") ; 
  }

  /*  touche aff_time */

  if ( ! strcmp( clavier->mot, "aff_time")) { 
    CONFIG_FORMATE_DONNEES_AFFICHAGE(as);
    CONFIG_LCD_AFFICHER_TEMPS_LIEU(gp_Lcd, 2 , lieu, temps ) ; 
    suivi->menu = MENU_MANUEL_BRUT ; 
    strcpy(clavier->mot,"") ; 
  }  
  if ( ! strcmp( clavier->mot, "aff_info" )) { 
    CONFIG_LCD_AFFICHER_INFORMATIONS(gp_Lcd, 2 ) ;
    suivi->menu = MENU_INFO ; 
    strcpy(clavier->mot,"") ; 
  }     // mode info
  /*  mode equatorial */
  if ( ! strcmp( clavier->mot, "key_equ" ))      { 
    CONFIG_LCD_AFFICHER_STRINGS(gp_Lcd, 2, "Mode equatorial", (char*)c_Menus[ MENU_EQUATORIAL ] ) ;
    suivi->menu = MENU_EQUATORIAL ; 
    strcpy(clavier->mot,"") ; 
  }    
  if ( ! strcmp( clavier->mot, "key_azi" ))      { 
    CONFIG_LCD_AFFICHER_STRINGS(gp_Lcd, 2, "Mode azimutal", (char*)c_Menus[ MENU_AZIMUTAL ] ) ;
    suivi->menu = MENU_AZIMUTAL ; 
    strcpy(clavier->mot,"") ; 
  }       // mode azimutal

  if ( ! strcmp( clavier->mot, "key_power" ))     { 
    CONFIG_LCD_AFFICHER_STRINGS(gp_Lcd, 1, "Mode azimutal", (char*)c_Menus[ MENU_AZIMUTAL ] ) ;
    suivi->menu = MENU_DOWN ; 
    strcpy(clavier->mot,"") ;  
  } // arret complet
  if ( ! strcmp( clavier->mot, "key_exit" ))      { 
    suivi->menu = MENU_PROGRAMME_DOWN ; 
    strcpy(clavier->mot,"") ; 
  } // arret du programme

  if ( ! strcmp( clavier->mot, "key_reseau_up"))  { 
    
    suivi->menu = MENU_RESEAU_UP ; strcpy(clavier->mot,"") ; 
  }      // activation du reseau
  
  
  //-----------------------------------------------------------------
  // Si un  mot est lu sur le clavier (appui sur menu->valider) necessaire)
  // on effectue une action
  // Si autre chose qu'un menu est lu sur le clavier , on peut aussi changer
  // de menu (pour plus de rapidite)
  //-----------------------------------------------------------------
  
  Trace1("clavier->phrase_lue = %d", clavier->phrase_lue) ;

  if ( clavier->phrase_lue ) {
    
    //---------------------------------
    // Si un MENU est entrer au clavier
    // => gestion des MENUS en fonction du clavier
    // ou de toute autre entree exterieure
    // autre qu'un switch electronique
    //---------------------------------
    
    Trace("symbole = %s nombre = %s\n", clavier->symbole, clavier->nombre ) ;
    
    //--------------------------------------------------------------------
    // ON CHANGE DE MENU VOLONTAIREMENT
    //--------------------------------------------------------------------
    
    if ( ! strcmp( clavier->symbole, clavier->menu )) {
      suivi->menu_old = suivi->menu ;
      suivi->menu     = atoi( clavier->nombre ) ; 
    }
    
    //---------------------------------------------------------------------------------------------------------
    // Si un nouvel as a ete pris en compte
    //---------------------------------------------------------------------------------------------------------
    
    sprintf( s_buffer4, "%s%s%s%s", CONFIG_MES, CONFIG_NGC, CONFIG_ETO, CONFIG_PLA) ;

    if ( strstr( s_buffer4, clavier->symbole ) != NULL ) {

      memset( as->nom, ZERO_CHAR, strlen(as->nom)) ;
      sprintf( as->nom, "%s%s", clavier->symbole, clavier->nombre) ;
    
      Trace1("== %s ==",as->nom) ;
      
      /* Recherche de l'as dans les catalgues */
      
      if ( strstr( as->nom, CONFIG_MES ) != NULL ) CAT_FIND( as, cat_dec) ;
      if ( strstr( as->nom, CONFIG_NGC ) != NULL ) CAT_FIND( as, cat_dec) ;
      if ( strstr( as->nom, CONFIG_ETO ) != NULL ) CAT_FIND( as, etoiles_dec) ;
      
      suivi->SUIVI_ALIGNEMENT = 1 ;
      //suivi->menu = MENU_AZIMUTAL ;

      GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 10) ;
      
    }
    //---------------------------------------------------------------------------------------------------------
    // Si une DEMANDE de MISE A JOUR relative au temps (sauf mois et jour)
    //---------------------------------------------------------------------------------------------------------
    
    if(  ! strcmp( clavier->symbole, "TIME" ) )  {

      GPIO_CLIGNOTE(GPIO_LED_ETAT, strlen( clavier->nombre ), 10) ;

      switch ( strlen( clavier->nombre ) ) {

        case 2 : // Si longueur = 2 cela ne peut etre qu'un changement de mois
          
          Trace1("demande changement heure minutes : %s" , clavier->nombre) ;

          /* CONFIG_SET_MONTH( clavier->nombre ) ; */

        case 4 : // Si longueur = 4 cela ne peut etre qu'un changement d'heure et de minutes
          
          Trace1("demande changement heure minutes : %s" , clavier->nombre) ;
    
          CONFIG_SET_HOUR_AND_MINUTES( clavier->nombre ) ;
          
        break ;
          
          // FIXME : 20190822 : modification configuration de la date

        case 8 : // Si longueur = 5 cela est un changement de mois jour avec verif de l'annee

          Trace1("demande changement annee : %s" , clavier->nombre) ;
          
          CONFIG_SET_YEAR_MONTH_AND_DAY( clavier->nombre ) ;
      
          break ;
      }
    }
    //---------------------------------------------------------------------------------------------------------
    // Si une DEMANDE de SETUP a lieu 
    // RAPPEL : l'annee par defaut est declaree en DUR dans le CODE de telle sorte que le code ne soit valable
    // que pour annee en COURS
    //---------------------------------------------------------------------------------------------------------
    
    if(  ! strcmp( clavier->symbole, "SETUP" ) )  {
      switch ( strlen( clavier->nombre ) ) {
        
        case 0 : break ; // a completer
        case 1 : break ; // a completer 

        case 2 : // TODO : exemple d'une demande de capteur : a modifier / completer
          CALCUL_TEMPS_SIDERAL( lieu, temps ) ;

          if ( devices->DEVICE_USE_CAPTEURS ) { 
            as->a = suivi->pitch ;
            as->h = suivi->heading ; 
          }
          CALCUL_EQUATEUR ( lieu, as) ;

          suivi->menu_old = suivi->menu ;
          suivi->menu = MENU_AZIMUTAL ;
          break ;
      }
      //---------------------------------------------------------------------------------------------------------
    }
    clavier->phrase_lue=0 ;
    strcpy( clavier->mot, "" ) ;
    strcpy( clavier->nombre, "" ) ;
  }
}
/*****************************************************************************************
* @fn     : SUIVI_MANUEL_BRUT
* @author : s.gravois
* @brief  : Ce mode permet le centrage / recentrage de l'objet tout en ayant le suivi.
* @param  : SUIVI   *suivi
* @param  : CLAVIER *clavier 
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/
/* FIXME :
 SUIVI_MANUEL_BRUT : le suivi etant effectue sur un as (calcul des vitesses et periodes par suivi_voute)
 l'appui sur les touches N-S-E-O provoque une suspension de suivi_voute jusqua touche OK
 avec une multiplication des vitesses N-S-E-O par un facteur ALT_ACC

 Ce mode permet le centrage / recentrage de l'objet tout en ayant le suivi.
*/

void SUIVI_MANUEL_BRUT(SUIVI * suivi, CLAVIER *clavier) {
  
  int flag_nord_sud_est_ouest = 0 ;
  int flag_calcul = 0 ;

  TRACE2("start") ;

  // GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, suivi) ;

  CONFIG_MAJ_SUIVI_PAS( suivi) ;

  if( strcmp( suivi->datas_infrarouge, "") != 0 ) {
    Trace1("0 suivi->datas_infrarouge = %s", suivi->datas_infrarouge ) ;
  }
  // -----------------------------------------------------------
  // reset = remise a zero des compteurs
  //------------------------------------------------------------
  
  if ( suivi->reset ) {

    pthread_mutex_lock(& suivi->mutex_azi );
    pthread_mutex_lock(& suivi->mutex_alt );

    if ( suivi->Sh != 0 ) suivi->pas_alt = suivi->Sh ; else suivi->pas_alt = 1 ;
    if ( suivi->Sa != 0 ) suivi->pas_azi = suivi->Sa ; else suivi->pas_azi = 1 ;

    suivi->acc_azi = 1 ;

    if ( suivi->SUIVI_EQUATORIAL )  suivi->acc_alt = 0.0 ;
    else                            suivi->acc_alt = 1.0 ;

    suivi->reset = 0 ;

    TRACE("%-10s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "reset.." , \
           suivi->Sh , suivi->Sa, suivi->pas_azi, suivi->pas_alt, suivi->acc_azi , suivi->acc_alt) ;
           
    pthread_mutex_unlock(& suivi->mutex_azi );
    pthread_mutex_unlock(& suivi->mutex_alt );

    flag_calcul = 1 ;
  }
  // -----------------------------------------------------------
  // actions a faire suivant appui sur NORD / SUD / OUEST /EST
  //------------------------------------------------------------

  if ( suivi->pas_nord || suivi->pas_sud || suivi->pas_est || suivi->pas_ouest ) {
    flag_nord_sud_est_ouest = 1 ;
  }

  // flag_nord_sud_est_ouest = 1 = on a appuye sur N-S-E-O

  if (flag_nord_sud_est_ouest ) { 
  
    Trace("flag_nord_sud_est_ouest") ;
    pthread_mutex_lock(& suivi->mutex_azi );
    pthread_mutex_lock(& suivi->mutex_alt );

    // On utilise les memes touches que dans SUIVI_MANUEL_1

    if ( suivi->pas_nord )  suivi->pas_alt++ ; if ( suivi->pas_alt == 0 ) suivi->pas_alt = 1 ;
    if ( suivi->pas_sud )   suivi->pas_alt-- ; if ( suivi->pas_alt == 0 ) suivi->pas_alt = -1 ;
    if ( suivi->pas_ouest ) suivi->pas_azi++;  if ( suivi->pas_azi == 0 ) suivi->pas_azi = 1 ;
    if ( suivi->pas_est )   suivi->pas_azi-- ; if ( suivi->pas_azi == 0 ) suivi->pas_azi = -1 ;

    if ( suivi->pas_nord  || suivi->pas_sud ) {
      if ( abs(suivi->pas_alt) <= 2) suivi->acc_alt = suivi->pas_alt  ;
      else                           suivi->acc_alt = suivi->pas_alt * ALT_ACC ;
    }
    if ( suivi->pas_ouest || suivi->pas_est ) {
      if ( abs(suivi->pas_azi) <= 2) suivi->acc_azi = suivi->pas_azi ;
      else                           suivi->acc_azi = suivi->pas_azi * AZI_ACC ;
    }
    suivi->pas_est     = 0 ;
    suivi->pas_ouest   = 0 ; 
    suivi->pas_nord    = 0 ;
    suivi->pas_sud     = 0 ;
    suivi->reset = 0 ;
/*
    Trace("pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", \
         suivi->pas_azi, suivi->pas_alt, suivi->acc_azi , suivi->acc_alt) ;
*/
    Trace("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "fleches.." , \
           suivi->Sh , suivi->Sa, suivi->pas_azi, suivi->pas_alt, suivi->acc_azi , suivi->acc_alt) ;

    pthread_mutex_unlock(& suivi->mutex_azi );
    pthread_mutex_unlock(& suivi->mutex_alt );

    flag_calcul = 1 ;
  }
  // ======================================================
  // Gestion des accelerations ou decelerations tres lentes
  // (reglage du suivi precis)
  // ======================================================

  if ( suivi->pas_forward ) {

    pthread_mutex_lock(& suivi->mutex_azi );
    pthread_mutex_lock(& suivi->mutex_alt );

    suivi->acc_azi *= ALTAZ_FORWARD ;
    suivi->acc_alt *= ALTAZ_FORWARD ; 

    pthread_mutex_unlock(& suivi->mutex_azi );
    pthread_mutex_unlock(& suivi->mutex_alt );
/*
    TRACE("acc*ALTAZ_FORWARD %.4f acc_azi %.4f acc_alt %.4f", ALTAZ_FORWARD, suivi->acc_azi, suivi->acc_alt ) ;
*/
    TRACE("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "forward.." , \
           suivi->Sh , suivi->Sa, suivi->pas_azi, suivi->pas_alt, suivi->acc_azi , suivi->acc_alt) ;

    suivi->pas_forward = 0 ;
    flag_calcul = 1 ;
  }
  if ( suivi->pas_rewind ) {

    pthread_mutex_lock(& suivi->mutex_azi );
    pthread_mutex_lock(& suivi->mutex_alt );

    suivi->acc_azi /= ALTAZ_REWIND ;
    suivi->acc_alt /= ALTAZ_REWIND ; 

    pthread_mutex_unlock(& suivi->mutex_azi );
    pthread_mutex_unlock(& suivi->mutex_alt );
/*
    TRACE("acc/ALTAZ_REWIND %.4f acc_azi %.4f acc_alt %.4f ", ALTAZ_REWIND, suivi->acc_azi, suivi->acc_alt ) ;
*/
    TRACE("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "rewind.." , \
           suivi->Sh , suivi->Sa, suivi->pas_azi, suivi->pas_alt, suivi->acc_azi , suivi->acc_alt) ;


    suivi->pas_rewind  = 0 ;
    flag_calcul = 1 ;
  }
  // ======================================================
  // Gestion des accelerations ou decelerations tres rapides
  // (reglage du suivi precis)
  // ======================================================

  if ( suivi->pas_forward_fast ) {

    pthread_mutex_lock(& suivi->mutex_azi );
    pthread_mutex_lock(& suivi->mutex_alt );

    suivi->acc_azi *= ALTAZ_FORWARD_FAST ;          
    suivi->acc_alt *= ALTAZ_FORWARD_FAST ; 

    pthread_mutex_unlock(& suivi->mutex_azi );
    pthread_mutex_unlock(& suivi->mutex_alt );

/*
    TRACE("acc*ALTAZ_FORWARD_FAST %.4f acc_azi %.4f acc_alt %.4f", ALTAZ_FORWARD_FAST, suivi->acc_azi, suivi->acc_alt ) ;
*/
    TRACE("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "forward fast.." , \
           suivi->Sh , suivi->Sa, suivi->pas_azi, suivi->pas_alt, suivi->acc_azi , suivi->acc_alt) ;

    suivi->pas_forward_fast = 0 ;
    flag_calcul = 1 ;
  }
  if ( suivi->pas_rewind_fast ) {

    pthread_mutex_lock(& suivi->mutex_azi );   
    pthread_mutex_lock(& suivi->mutex_alt );

    suivi->acc_azi /= ALTAZ_REWIND_FAST ;
    suivi->acc_alt /= ALTAZ_REWIND_FAST ; 

    pthread_mutex_unlock(& suivi->mutex_azi );
    pthread_mutex_unlock(& suivi->mutex_alt );

/*
    TRACE("acc/ALTAZ_REWIND_FAST  %.4f acc_azi %.4f acc_alt %.4f ", ALTAZ_REWIND_FAST, suivi->acc_azi, suivi->acc_alt ) ;
*/
    TRACE("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "forward fast.." , \
           suivi->Sh , suivi->Sa, suivi->pas_azi, suivi->pas_alt, suivi->acc_azi , suivi->acc_alt) ;
               
    suivi->pas_rewind_fast  = 0 ;
    flag_calcul = 1 ;
  }
  // ======================================================
  // Rafraichissement des vitesses et periodes
  // ======================================================

  if ( flag_calcul ) {
  
    pthread_mutex_lock( & suivi->mutex_cal );
    
    CALCUL_VITESSES( lieu, as, suivi) ;
    CALCUL_PERIODE ( as, suivi, voute) ;
    
    pthread_mutex_unlock( & suivi->mutex_cal );
  }
}

//==========================================================
// MODE MANUEL : le suivi d'effectue directement en appuyant sur les touches 
//==========================================================

void SUIVI_MANUEL_1(SUIVI * suivi, CLAVIER *clavier) {
    
  double  tempo_raq_alt ;
  double  tempo_raq_azi ;
  struct timeval t00 ;
  struct timeval t01 ;
  int azi,alt ;
  
  suivi->pas_azi_old = suivi->pas_azi ;
  suivi->pas_alt_old = suivi->pas_alt ;
  
  gettimeofday(&t00,NULL) ;
  
  Trace("start") ;
  
  azi = 0 ;
  alt = 0 ;
  
  GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, suivi) ;
  CONFIG_MAJ_SUIVI_PAS  ( suivi) ;
  
  // La determination de tempo_raq est tres importante
  // Elle varie suivant la reduction total du moteur
  /* TODO : a expliquer */
  tempo_raq_alt = (double)TEMPO_RAQ * 1000000 / ALT_R ;
  tempo_raq_azi = (double)TEMPO_RAQ * 1000000 / AZI_R ;
  
  // -----------------------------------------------------------
  // reset = remise a zero des compteurs
  //------------------------------------------------------------
  
  if ( suivi->reset ) {

    suivi->pas_alt = 0 ;
    suivi->pas_azi = 0 ;

    suivi->d_appui_raq_azi = 0 ;
    suivi->d_appui_raq_alt = 0 ; 

    suivi->Ta = MAIN_TA_RESET ;
    suivi->Th = MAIN_TH_RESET ;
    suivi->Fa = MAIN_FA_RESET ;
    suivi->Fh = MAIN_FH_RESET ;
    suivi->reset = 0 ;
  }
  // -----------------------------------------------------------
  // TANT QUE J'APPUIE SUR UN BOUTON DE LA RAQUETTE ===>
  //------------------------------------------------------------
  
  while( suivi->pas_ouest || suivi->pas_est || suivi->pas_sud || suivi->pas_nord || suivi->pas_acc_plus || suivi->pas_acc_moins )
  { 
    // On remet une periode inferieure a la seconde pour eviter aux deux autres threads d'attendre
    
    if ( suivi->Ta == MAIN_TA_RESET && suivi->Th == MAIN_TH_RESET ) {
      suivi->Ta = MAIN_TA_TRANSITOIRE ;
      suivi->Th = MAIN_TH_TRANSITOIRE ;
      suivi->Fa = MAIN_FA_TRANSITOIRE ;
      suivi->Fh = MAIN_FH_TRANSITOIRE ;
    }
    
    // ----------------------------------------------------------------------------
    // On ne fait qu'une fois dans la boucle les actions suivantes (d'ou ce test..)
    // ----------------------------------------------------------------------------
    
    if ( ! azi )  if ( suivi->pas_ouest || suivi->pas_est || suivi->pas_acc_moins || suivi->pas_acc_plus ) { 
      pthread_mutex_lock(& suivi->mutex_azi );
      azi=1 ;
    }
    if ( ! alt )  if ( suivi->pas_nord  || suivi->pas_sud || suivi->pas_acc_moins || suivi->pas_acc_plus ) { 
      pthread_mutex_lock(& suivi->mutex_alt ); 
      alt=1 ;
    }
    
    if ( azi ) {
     
     if ( suivi->pas_ouest || suivi->pas_est ) 
       suivi->pas_azi += suivi->pas_ouest - suivi->pas_est  ; 
     
     if ( suivi->pas_acc_plus)  suivi->pas_azi = (long) suivi->pas_azi * 1.5 ; 
     if ( suivi->pas_acc_moins) suivi->pas_azi = (long) suivi->pas_azi / 1.5 ; 
    }
    
    if ( alt ) {
     
     if ( suivi->pas_nord || suivi->pas_sud)
       suivi->pas_alt += suivi->pas_nord  - suivi->pas_sud ;
     
     if ( suivi->pas_acc_plus)  suivi->pas_alt = (long) suivi->pas_alt * 1.5 ; 
     if ( suivi->pas_acc_moins) suivi->pas_alt = (long) suivi->pas_alt / 1.5 ; 
    }
    
    if ( azi ) usleep( (long)( tempo_raq_azi ) ) ;
    if ( alt ) usleep( (long)( tempo_raq_alt ) ) ;

    //-------------------------------------------------------------
    // ACCELERATION ou DECELERATION lors de l'APPUI sur la RAQUETTE
    // a modifier pour optimiser
    //-------------------------------------------------------------
    
    suivi->pas_acc_plus  = 0 ;
    suivi->pas_acc_moins = 0 ;
    suivi->pas_est   = 0 ;
    suivi->pas_ouest = 0 ; 
    suivi->pas_nord  = 0 ;
    suivi->pas_sud   = 0 ;
    suivi->reset = 0 ;
    
    // on relit sur les claviers en mode manuel 

    GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, suivi) ;
    CONFIG_MAJ_SUIVI_PAS( suivi) ;
  }
  // =================================================================
  // FIN DE LA BOUCLE : TANT QUE J'APPUIE SUR UN BOUTON DE LA RAQUETTE
  // =================================================================

  if ( azi ) { pthread_mutex_unlock( & suivi->mutex_azi ); }
  if ( alt ) { pthread_mutex_unlock( & suivi->mutex_alt ); }
  
  gettimeofday(&t01,NULL) ;

  suivi->t_diff     = (( t01.tv_sec - t00.tv_sec ) * GPIO_MICRO_SEC) + t01.tv_usec - t00.tv_usec ;
  suivi->t_diff_sec = (double)suivi->t_diff / (double)GPIO_MICRO_SEC ;
	 
  if ( suivi->pas_azi_old != suivi->pas_azi ) suivi->d_appui_raq_azi += suivi->t_diff_sec ;
  if ( suivi->pas_alt_old != suivi->pas_alt ) suivi->d_appui_raq_alt += suivi->t_diff_sec ;
	 
  if ( suivi->pas_azi_old != suivi->pas_azi || suivi->pas_alt_old != suivi->pas_alt ) {

   TRACE(" : Va = %2.4f Vh = %2.4f Ta = %2.4f Th = %2.4f Fa = %2.4f Fh = %2.4f Fam = %ld Fhm = %ld",\
        as->Va,as->Vh,suivi->Ta,suivi->Th,suivi->Fa,suivi->Fh, \
	(suivi->Ia - suivi->Ia_prec),(suivi->Ih - suivi->Ih_prec )) ;  
  }

}
/*****************************************************************************************
* @fn     : SUIVI_MENU
* @author : s.gravois
* @brief  : Ce mode permet de gerer les menus .
* @param  : SUIVI   *suivi
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void * SUIVI_MENU(SUIVI * suivi) {
  
  struct sched_param param;
  
   // La temporisation dans les boucles du thread SUIVI_MENU depend de  suivi->temporisation_menu
   
  Trace("start with tempo %ld", suivi->temporisation_menu) ;
    
  param.sched_priority = PTHREAD_POLICY_1 ;

  if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_MENU, & param) != 0) { 
    perror("setschedparam SUIVI_MENU");
    exit(EXIT_FAILURE); 
  }

  signal(SIGTERM,TRAP_SUIVI_MENU) ;
  
  SUIVI_MENU_PREALABLE ( suivi) ;

  suivi->reset = 1 ;

  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread SUIVI_MENU
  //-------------------------------------------------------------------------------
  
  while(1) {

    usleep( suivi->temporisation_menu ) ;

    if( strcmp( suivi->datas_infrarouge, "") != 0 ) {
      Trace2("A suivi->datas_infrarouge = %s", suivi->datas_infrarouge ) ;
    }

    /* GPIO_RAQUETTE_READ( gpio_key_l, gpio_key_c, clavier) ; */
    CONFIG_INPUTS_GESTION_APPUIS( suivi, clavier) ;

    if( strcmp( suivi->datas_infrarouge, "") != 0 ) {
      Trace2("B suivi->datas_infrarouge = %s", suivi->datas_infrarouge ) ;
    }

    /* IR_ACTIONS_PARTICULIERES( suivi) ; */
    SUIVI_TRAITEMENT_MOT( suivi, clavier ) ;


    if( strcmp( suivi->datas_infrarouge, "") != 0 ) {
      Trace2("C suivi->datas_infrarouge = %s", suivi->datas_infrarouge ) ;
    }

    CONFIG_AFFICHER_CHANGEMENTS() ;


    if( strcmp( suivi->datas_infrarouge, "") != 0 ) {
      Trace2("D suivi->datas_infrarouge = %s", suivi->datas_infrarouge ) ;
    }

    CONFIG_AFFICHER_CLAVIER( clavier ) ;	

    if( strcmp( suivi->datas_infrarouge, "") != 0 ) {
      Trace2("E suivi->datas_infrarouge = %s", suivi->datas_infrarouge ) ;
    }

    switch( suivi->menu ) {

      // ------------------------------- ALIGNEMENT - MODE AZIMUTAL ----------------------------
     
      case MENU_AZIMUTAL :

        // a modifier / completer : CALCUL_TEMPS_SIDERAL et CALCUL_ANGLE_HORAIRE
        // sont a supprimer car deja calculer dans SUIVI_

        TRACE("appel : %d : MENU_AZIMUTAL" , suivi->menu) ;

        pthread_mutex_lock(& suivi->mutex_azi );   
        pthread_mutex_lock(& suivi->mutex_alt );

        suivi->acc_alt          = 1 ;
        suivi->acc_azi          = 1 ;

        pthread_mutex_unlock(& suivi->mutex_azi );   
        pthread_mutex_unlock(& suivi->mutex_alt );

        suivi->SUIVI_EQUATORIAL = 0 ;
        suivi->SUIVI_VOUTE      = 1 ; 

				CALCUL_TOUT() ;

        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_BRUT ; 

      break ;

      // ------------------------------  MODE EQUATORIAL --------------------------------------
     
      case MENU_EQUATORIAL :

        pthread_mutex_lock(& suivi->mutex_azi );   
        pthread_mutex_lock(& suivi->mutex_alt );

        suivi->acc_alt          = 0 ;
        suivi->acc_azi          = 1 ;

        pthread_mutex_unlock(& suivi->mutex_azi );   
        pthread_mutex_unlock(& suivi->mutex_alt );

        suivi->SUIVI_EQUATORIAL = 1 ;
        suivi->SUIVI_VOUTE      = 0 ; 

				CALCUL_TOUT() ;

        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_BRUT ;

      break ;

      // -------------------------- SUIVI MANUEL PAR DEFAUT ------------------
      
      case MENU_MANUEL_BRUT :

        // le but de ce suivi est de deduire des actions N-S-O-E de l'utilisateur 
        // simplement une acceleration / ralentissement / changement de direction
        // sur le N-S-E-O (pas de recalcul des periodes) 
        // FIXME : les periodes sont conservees , ainsi que le mode azimutal ou equatorial

        if( strcmp( suivi->datas_infrarouge, "") != 0 ) {
          Trace2("1 suivi->datas_infrarouge = %s", suivi->datas_infrarouge ) ;
        }

        SUIVI_MANUEL_BRUT(suivi, clavier) ;
        
        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_BRUT ; 

      break ;

      /* -------------------------- MENU_MANUEL_NON_ASSERVI : LE PLUS SIMPLE ------------------
          TODO : a coder : juste appui des touches provoque le mouvement
          TODO : mode le plus simple a priori
          TODO : acc_azi_ et acc_alt doivent etre initialises a zero
      */
      
      case MENU_MANUEL_NON_ASSERVI :

        // Suivi le plus simple : seules les touches est nord sud ouest et reset sont prises en compte
        // TODO : verifier

        TRACE("appel : %d : MENU_MANUEL_NON_ASSERVI" , suivi->menu) ;

        pthread_mutex_lock(& suivi->mutex_azi );   
        pthread_mutex_lock(& suivi->mutex_alt );

        suivi->acc_alt          = 0 ;
        suivi->acc_azi          = 0 ;

        pthread_mutex_unlock(& suivi->mutex_azi );   
        pthread_mutex_unlock(& suivi->mutex_alt );

				CALCUL_TOUT() ;

        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_NON_ASSERVI ; 

      break ;

      // -------------------------- SUIVI MANUEL AVEC RE-CALCUL DES PERIODES  -----------------
      case MENU_MANUEL_ASSERVI :
       
        // TODO : a modifier car cela marche pas tres bien (interraction avec le thread SUIVI_VOUTE)
        // TODO : le but de ce suivi est de deduire des actions N-S-O-E de l'utilisateur 
        // TODO : les periodes / frequences en azimut et altitude

        SUIVI_MANUEL_1(suivi, clavier) ; 
        CALCUL_PERIODES_SUIVI_MANUEL(as,suivi,voute)  ;

        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_ASSERVI ; 

      break ;

      // ------------------------------  MODE GOTO --------------------------------------

      case MENU_GOTO :

        TRACE("appel : %d : MENU_GOTO" , suivi->menu) ;

        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_BRUT ;

      break ; 

      // ------------------------------  MODE INFO --------------------------------------

      case MENU_INFO :

        TRACE("appel : %d : MENU_INFO" , suivi->menu) ;

        CONFIG_AFFICHER_TOUT() ;

        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_BRUT ;

      break ; 

      // ------------------------------ MODE ACTIVATION DU RESEAU ----------------------------------
     
      case MENU_RESEAU_UP :

        TRACE("appel : %d : MENU_RESEAU_UP" , suivi->menu) ;

        if ( system("/etc/init.d/OLD/S40network start")) {
         perror("Probleme avec system(/etc/init.d/OLD/S40network start)"); 
         Trace("Probleme avec system(/etc/init.d/OLD/S40network start)\n") ;
        }

        suivi->menu = suivi->menu_old ;
        suivi->menu = MENU_MANUEL_BRUT ;

      break ;

      // ------------------------------ ARRET DU RESEAU ---------------------------------------
     
      case MENU_RESEAU_DOWN :
     
        TRACE("appel : %d : MENU_RESEAU_DOWN" , suivi->menu) ;

        if ( system("/etc/init.d/OLD/S40network stop")) {
          perror("Probleme avec system(/etc/init.d/OLD/S40network stop)"); 
          Trace("Probleme avec system(/etc/init.d/OLD/S40network stop)\n") ;
        }

        suivi->menu = suivi->menu_old ;
        suivi->menu = MENU_MANUEL_BRUT ;

      break ;
      // ------------------------------ ARRET DU PROGRAMME ------------------------------------
      case MENU_PROGRAMME_DOWN :
     
        TRACE("appel : %d : MENU_PROGRAMME_DOWN" , suivi->menu) ;

        TRAP_MAIN(1) ;
        break ;
     
      // ------------------------------ ARRET DU SYSTEME EMBARQUE -----------------------------
      case MENU_DOWN :

        TRACE("appel : %d : MENU_DOWN" , suivi->menu) ;

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
* @param  : SUIVI   *suivi
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

/*
   SUIVI_VOUTE :
   le but de la fonction est de rafraichir a intervalles reguliers (1 seconde)
   tous les calculs relatifs a la vitesse de l'as suivi
*/

void * SUIVI_VOUTE(SUIVI * suivi) {
  
  unsigned long  incr ;
  struct timeval t00 ;
  struct sched_param param;
  /*
  char   c_Vitesses[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure] ;
  */
  // La temporisation dans la boucle du thread SUIVI_VOUTE depend de voute->DT (en us)
  // a completer / modifier :
  // il FAUT calculer / mettre a jour a,h,A,H de l'as quand est utliser les menus
  // suivi manuel et suivi capteurs , qui renvoient l'azimut et l'altitude
  // ==> CALCUL_EQUATEUR pour les devices altitude et azimut venant du capteur
  // ==> autre calcul plus complique quand on a les devices de vitesses et periodes provenant de la raquette !!!!!
  
  Trace("start with tempo %ld", suivi->temporisation_voute) ;
  
  param.sched_priority = PTHREAD_POLICY_1 ;
  
  if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_VOUTE, & param) != 0) {
     perror("setschedparam SUIVI_VOUTE"); 
     exit(EXIT_FAILURE);
  }
  suivi->p_threads_id[ g_id_thread++ ] = pthread_self() ;
  signal( SIGTERM, TRAP_SUIVI_VOUTE) ;
  
  suivi->d_temps = 0 ;
  incr =0 ;
  
  gettimeofday(&t00,NULL) ;
  
  CONFIG_VOUTE( voute, 1, 1, 0.985 ) ;
  
  TRACE1("suivi->SUIVI_VOUTE=%d",suivi->SUIVI_VOUTE) ;

  // FIXME : 
  // en mode equatorial, pas besoin de SUIVI_VOUTE, en effet la vitesse ne change pas
  // Va=15"/s et Vh=0 (le moteur en ascension droite est commande par azimut)
  
  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread SUIVI_VOUTE
  //-------------------------------------------------------------------------------
  
  suivi->SUIVI_ALIGNEMENT=1 ;

  while(1) {
    
    if ( suivi->SUIVI_VOUTE ) {
      
      /* FIXME : modification 20121225 : tous les calculs generiques dans CALCUL_TOUT */
      
      CALCUL_TOUT() ;
      /* Exceptionnellement , utilisation variables globales */ 
      /* CONFIG_LCD_AFFICHER_TEMPS_LIEU(gp_Lcd,0,lieu,temps) ;*/

      if ( suivi->SUIVI_ALIGNEMENT ) { 

        CONFIG_FORMATE_DONNEES_AFFICHAGE(as) ;
        CONFIG_AFFICHER_MODE_LONG(as) ; 
        CONFIG_AFFICHER_MODE_STELLARIUM(as) ;
        CONFIG_LCD_AFFICHER_ASTRE_VITESSES(gp_Lcd,0,as) ;
        
        suivi->SUIVI_ALIGNEMENT = 0 ;
      }
/*
      if ( suivi->menu_old != suivi->menu  ) {
        CONFIG_AFFICHER_TOUT() ;
      }
*/
      as->AGH   += voute->pas ;
      voute->deb += voute->pas ;

      suivi->d_temps += CALCUL_TEMPORISATION_VOUTE( voute, t00 ) ; 
      gettimeofday(&t00,NULL) ;

			voute->num ++ ;
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
      usleep( voute->DT );
    }
  }
  Trace("Stop") ;

  return NULL ; 
}
/*****************************************************************************************
* @fn     : SUIVI_INFRAROUGE
* @author : s.gravois
* @brief  : fonction de callback du thread suivi infrarouge
* @param  : SUIVI * suivi
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : supprimer argument qi est variable globale
*****************************************************************************************/

void * SUIVI_INFRAROUGE(SUIVI * suivi) {
   
  struct sched_param param;
  
  Trace("start with tempo %ld", suivi->temporisation_ir) ;
  
  // La temporisation dans la boucle du thread SUIVI_INFRAROUGE depend de suivi->temporisation_ir (en us)
  // present dans la fonction bloquante LIRC_READ 
  
  // on laisse le temps aux autres threads de demarrer
  // notamment pour arriver dans la fonction SUIVI_TRAITEMENT_MENU
  // qui initialise a 1 SUIVI_INFRAROUGE
  
  sleep(1) ;
  
  if ( devices->DEVICE_USE_INFRAROUGE ) {

    param.sched_priority = PTHREAD_POLICY_1  ;
    
    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_INFRA, & param) != 0) { 
      perror("setschedparam SUIVI_INFRAROUGE"); 
      exit(EXIT_FAILURE);
    }
    suivi->p_threads_id[ g_id_thread++ ] = pthread_self() ;
    signal( SIGTERM, TRAP_SUIVI_INFRAROUGE) ;
  
    LIRC_OPEN( lircconfig ) ;
  
    // ATTENTION !!! la fonction LIRC_READ est bloquante (voir ir.c) !!!!!
    
    LIRC_READ( suivi ) ;
    LIRC_CLOSE(lircconfig) ;
  }

  Trace("Stop") ;

  return NULL ;
}
/*****************************************************************************************
* @fn     : SUIVI_LCD
* @author : s.gravois
* @brief  : fonction de callback du thread suivi ecran LCD
* @param  : SUIVI * suivi
* @date   : 2022-04-12 creation 
* @date   : 2022-04-27 mise en commentaire de CONFIG_LCD_AFFICHER_TEMPS_LIEU
* @todo   : TODO : reflechir a ce qui doit etre rafraichi
*****************************************************************************************/

void * SUIVI_LCD(SUIVI * suivi) {
   
  struct sched_param param;
  
  Trace("start with tempo %ld", suivi->temporisation_lcd) ;
  
  if ( devices->DEVICE_USE_LCD ) {

    param.sched_priority = PTHREAD_POLICY_1  ;
    
    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_LCD, & param) != 0) { 
      perror("setschedparam SUIVI_LCD"); 
      exit(EXIT_FAILURE);
    }
    suivi->p_threads_id[ g_id_thread++ ] = pthread_self() ;
    signal( SIGTERM, TRAP_SUIVI_LCD) ;

    while(1) {

      /* CONFIG_LCD_AFFICHER_TEMPS_LIEU( gp_Lcd,0 , lieu, temps ) ; */
      usleep( suivi->temporisation_lcd );
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
* @param  : SUIVI * suivi
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-04-12 correction code ( repetition if ( i_indice_code < CONFIG_CODE_NB_CODES ))
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CLAVIER_TERMIOS( SUIVI * suivi ) {

  int i_indice_code=0 ;
  int c_char =0 ;
  int i_sum_ascii =0 ; 
  char c_sum_ascii[8] ;
  char ch_chaine[TERMIOS_KBHIT_SIZE_BUFFER_READ] ;
  struct sched_param param;
  struct timeval t00,t01  ;
  
  Trace("start with tempo %ld", suivi->temporisation_termios) ;
  
  sleep(1) ;
  
  if ( devices->DEVICE_USE_KEYBOARD ) {

    param.sched_priority = PTHREAD_POLICY_1  ;
    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_CLAVIER, & param) != 0) { 
      perror("setschedparam SUIVI_CLAVIER_TERMIOS"); exit(EXIT_FAILURE);
    }
    suivi->p_threads_id[ g_id_thread++ ] = pthread_self() ;
    signal( SIGTERM, TRAP_SUIVI_CLAVIER) ;
  
    KEYBOARD_TERMIOS_INIT() ;

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
        while(strcmp(gp_Codes->in_term[i_indice_code],c_sum_ascii) && ( i_indice_code < CONFIG_CODE_NB_CODES ) ){ 
           Trace("%s = %s ?", c_sum_ascii, gp_Codes->in_term[i_indice_code]) ;  
           i_indice_code++ ; 
        }
        pthread_mutex_lock(& suivi->mutex_infrarouge );
        memset( suivi->datas_infrarouge, '\0', strlen( suivi->datas_infrarouge ) ) ;
        pthread_mutex_unlock(& suivi->mutex_infrarouge );

        if ( i_indice_code < CONFIG_CODE_NB_CODES ) {
          pthread_mutex_lock(& suivi->mutex_infrarouge );
          Trace2("A maj suivi->datas_infrarouge") ;
          strcpy( suivi->datas_infrarouge, gp_Codes->out_act[i_indice_code] ) ;
          pthread_mutex_unlock(& suivi->mutex_infrarouge );

          GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 10) ;
        }
        // tres important !!
        // le usleep suivant permet de garder l information !!!!!!
        // suivi->datas_infrarouge fonctionne comme un TAMPON
        // il va etre lu par les threads du programme principal

        Trace("suivi->datas_infrarouge = %s", suivi->datas_infrarouge ) ;
        Trace1("suivi->temporisation_termios = %ld", suivi->temporisation_termios ) ;

        usleep( suivi->temporisation_termios ) ;

        pthread_mutex_lock(& suivi->mutex_infrarouge );
        memset( suivi->datas_infrarouge, 0, strlen( suivi->datas_infrarouge ) ) ;
        Trace2("F remise a zero suivi->datas_infrarouge") ;
        strcpy( suivi->datas_infrarouge, "") ;
        pthread_mutex_unlock(& suivi->mutex_infrarouge );
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
* @param  : SUIVI * suivi
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CLAVIER_getchar( SUIVI * suivi ) {

  int c_char = 0 ;
  struct sched_param param;
  Trace("start") ;

  sleep(1) ;

  if ( devices->DEVICE_USE_KEYBOARD ) {

    param.sched_priority = PTHREAD_POLICY_1  ;
    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_CLAVIER, & param) != 0) { 
      perror("setschedparam SUIVI_CLAVIER"); exit(EXIT_FAILURE);
    }
    suivi->p_threads_id[ g_id_thread++ ] = pthread_self() ;
    signal( SIGTERM, TRAP_SUIVI_CLAVIER) ;

    while( ( c_char = getchar () ) != 'q' ) {
      usleep(100000) ;
      TRACE("%c %d entre au clavier", c_char,c_char  ) ; 
    }
    if ( c_char == 'q' ) {
      TRAP_MAIN(1) ;
    }
    /* appels LIRC pour memoire : 
    LIRC_OPEN( lircconfig ) ;
    LIRC_READ( suivi ) ;
    LIRC_CLOSE(lircconfig) ;*/
  }
  Trace("Stop") ;

  return NULL ;
}

/*****************************************************************************************
* @fn     : SUIVI_CLAVIER_NCURSES
* @author : s.gravois
* @brief  : fonction de callback du thread suivi clavier en mode ncurses
* @param  : SUIVI * suivi
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CLAVIER_NCURSES(SUIVI* suivi ) {
  int ch = 0 ;
  unsigned long l_incr=0 ;
  struct sched_param param;
  Trace("start") ;
  
  sleep(1) ;
  
  if ( devices->DEVICE_USE_KEYBOARD ) {

    param.sched_priority = PTHREAD_POLICY_1  ;
    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_CLAVIER, & param) != 0) { 
      perror("setschedparam SUIVI_CLAVIER"); exit(EXIT_FAILURE);
    }
    suivi->p_threads_id[ g_id_thread++ ] = pthread_self() ;
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
* @param  : SUIVI * suivi
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : supprimer argument qui est variable globale
*****************************************************************************************/

void * SUIVI_CAPTEURS(SUIVI * suivi) {
   
  struct sched_param param;
  int ret ;
  
  I2C_DEVICE   exemple, *ex ;
  I2C_ACC_MAG  accmag,  *am ;
  
  Trace("start") ;
  
  sleep(1) ;

  ex = & exemple ;
  am = & accmag ;
  ex->fd = 0 ;
  
  if ( devices->DEVICE_USE_CAPTEURS ) {
      
    // La temporisation dans la boucle du thread SUIVI_CAPTEURS depend de suivi->temporisation_capteurs (en us)
    // present dans la fonction bloquante LIRC_READ 
    
    // on laisse le temps aux autres threads de demarrer
    // notamment pour arriver dans la fonction SUIVI_TRAITEMENT_MENU
    // qui initialise a 1 SUIVI_CAPTEURS
    
    param.sched_priority = PTHREAD_POLICY_1  ;
    
    if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_CAPTEUR, & param) != 0) {
      perror("setschedparam SUIVI_CAPTEURS"); exit(EXIT_FAILURE);}

    suivi->p_threads_id[ g_id_thread++ ] = pthread_self() ;
    
    signal( SIGTERM, TRAP_SUIVI_CAPTEURS) ;
    
    // a modifier pour definir le choix de l'infrarouge ou pas (config.txt ? .h ? )
    
    while(1) {
      
      if ( ! devices->init_capteurs ) {
        
        ret = I2C_INIT(ex, DEVICE_RASPI_2, DEVICE_LSM_ADRESS ) ;

        if ( ! ret ) {
          Trace("Pas de capteur disponible") ;
          devices->DEVICE_USE_CAPTEURS = 0 ;
          devices->init_capteurs = 0 ;
          break ;
        }
        else {
          I2C_SET( ex, REG_CTRL1, "0x57" ) ;
          I2C_SET( ex, REG_CTRL2, "0x00" ) ;
          I2C_SET( ex, REG_CTRL5, "0x64" ) ;
          I2C_SET( ex, REG_CTRL6, "0x20" ) ;
          I2C_SET( ex, REG_CTRL7, "0x00" ) ;
    
          devices->init_capteurs = 1 ;
        }
      }
      if ( devices->init_capteurs ) {
      
        I2C_GET_ACC( ex, am )   ;
        I2C_CALCULS_ACCMAG( am ) ;
      
        suivi->roll    =  am->roll  ; 
        suivi->pitch   =  am->pitch ;
        suivi->heading =  am->heading ;
      
        TRACE1("%.0f\t%.0f\t%.0f\n", am->pitch * I2C_DEGRAD, am->roll * I2C_DEGRAD, am->heading * I2C_DEGRAD) ;
      }	
      
      usleep( suivi->temporisation_capteurs );
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
  pthread_t p_thread_p_alt[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  pthread_t p_thread_p_azi[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  pthread_t p_thread_m_alt ;
  pthread_t p_thread_m_azi ;
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

  ARGUMENTS_GERER_REP_HOME(argc, argv) ;
  
  g_incrlog=0 ;
  g_id_thread=0 ;

  // -----------------------------------------------------------------
  // Initialisations des structures de devices
  // -----------------------------------------------------------------

  as      = &ast ;
  lieu    = &lie;
  voute   = &vou ;
  suivi   = &sui ;
  temps   = &tem ;
  clavier = &cla ;
  devices = &dev ;

  gp_Codes  = &g_Codes ;
  gp_Lcd    = &g_Lcd ;

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
  
  CONFIG_INIT_VOUTE     ( voute ) ; /* soit etre place avant CONFIG_INIT_SUIVI */
  CONFIG_INIT_CODES     ( gp_Codes ) ;
  CONFIG_INIT_CLAVIER   ( clavier ) ;   
  CONFIG_INIT_ASTRE     ( as ) ;
  CONFIG_INIT_LIEU      ( lieu  ) ;
  CONFIG_INIT_SUIVI     ( suivi ) ;
  CONFIG_INIT_TEMPS     ( temps ) ;
  CONFIG_INIT_DEVICES   ( devices ) ;
  CALCUL_TEMPS_SIDERAL  ( lieu, temps) ;
  CONFIG_INIT_LCD       ( gp_Lcd ) ;
  CONFIG_LCD_AFFICHER_TEMPS_LIEU( gp_Lcd,0 , lieu, temps ) ; 

  CONFIG_AFFICHER_DEVICES_USE() ;

  // -----------------------------------------------------------------
  // Mise en place du temps reel et du parallelisme (parallelisme, priorites, ..)
  // -----------------------------------------------------------------
  
  Trace1("Mise en place temps reel et parallelisme ") ;
  /* La commande suivante avec sudo et tee provoque un souvi sur la cible */
  /* visible avec strace -x  */
  /*
  system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ; 
  */
  system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ; 
  mlockall(MCL_CURRENT | MCL_FUTURE);
  
  param.sched_priority = PTHREAD_POLICY_1 ;

  if (pthread_setschedparam( pthread_self(), PTHREAD_SCHED_PARAM_SUIVI_MAIN, & param) != 0) {
    perror("setschedparam main");
    exit(EXIT_FAILURE);
  }

  signal(SIGINT,TRAP_MAIN) ;
  signal(SIGALRM,TRAP_MAIN) ;

  memset( suivi->p_threads_id, 0 , MAX_THREADS*sizeof(pthread_t)) ;

  TRACE("gpio_alt         : %d %d %d %d", gpio_alt[0], gpio_alt[1], gpio_alt[2], gpio_alt[3] ) ;
  TRACE("gpio_azi         : %d %d %d %d", gpio_azi[0], gpio_azi[1], gpio_azi[2], gpio_azi[3] ) ;
  TRACE("gpio_mas         : %d %d %d %d", gpio_mas[0], gpio_mas[1], gpio_mas[2], gpio_mas[3] ) ;
  TRACE("GPIO_LED_ETAT    : %d", GPIO_LED_ETAT );
  TRACE("ASTRE_PAR_DEFAUT : %s", ASTRE_PAR_DEFAUT) ;
  
  // -----------------------------------------------------------------
  // reglages variables particulieres
  // ----------------------------------------------------------------- 

  if ( strcmp(ASTRE_PAR_DEFAUT,"") != 0 ) {

    memset( as->nom, 0, sizeof(as->nom)) ;
    strcpy( as->nom, ASTRE_PAR_DEFAUT ) ;
  }
  // -----------------------------------------------------------------
  
  CAT_READ            ( CAT_NGC_TXT, cat) ;                     // CAT_AFFICHER ( cat ) ; 
  CAT_FORMAT_DECIMAL  ( "ngc.csv.dec" , cat, cat_dec) ;         // CAT_AFFICHER ( cat_dec ) ;

  CAT_READ            ( CAT_ETO_TXT, etoiles) ;                 // CAT_AFFICHER ( etoiles ) ;
  CAT_FORMAT_DECIMAL_2( "eto.csv.dec" , etoiles, etoiles_dec) ; // CAT_AFFICHER ( etoiles_dec ) ;
  
  // -----------------------------------------------------------------

  ARGUMENTS_HELP                  ( argc, argv ) ;
  ARGUMENTS_GERER_FACON_CLASSIQUE ( argc, argv  ) ;
  
  if ( suivi->alarme != 0 ) {
    alarm( suivi->alarme) ;
  }  

  // ouverture led etat ----------------------------------------------

  if ( GPIO_LED_ETAT != 0 ) {

    GPIO_CLOSE_BROCHE( GPIO_LED_ETAT) ;
    GPIO_OPEN_BROCHE( GPIO_LED_ETAT, 1) ;
    GPIO_SET( GPIO_LED_ETAT, 0 ) ;
  }
  
  // -----------------------------------------------------------------
  
  pm_azi = &m_azi ;
  pm_alt = &m_alt ;
  
  pm_azi->suivi = (SUIVI*)suivi ;   // pour permettre l'acces des membres de SUIVI dans GPIO_PWM_MOTEUR
  pm_alt->suivi = (SUIVI*)suivi ;   // pour permettre l'acces des membres de SUIVI dans GPIO_PWM_MOTEUR
  

  GPIO_INIT_PWM_MOTEUR_2(\
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

  GPIO_INIT_PWM_MOTEUR_2(\
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
  
  for( i=0 ; i< ALT_R4 ; i++ ) {
    TRACE1("%d\t%f\t%f\t%f\t%f",i,\
      pm_azi->phase[0]->rap[i],\
      pm_azi->phase[1]->rap[i],\
      pm_azi->phase[2]->rap[i],\
      pm_azi->phase[3]->rap[i]) ;
  }

  // ============================== gestion des threads  ===================================

  pthread_create( &p_thread_p_azi[0],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_azi->phase[0] ) ;
  pthread_create( &p_thread_p_azi[1],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_azi->phase[1] ) ;
  pthread_create( &p_thread_p_azi[2],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_azi->phase[2] ) ;
  pthread_create( &p_thread_p_azi[3],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_azi->phase[3] ) ;

  pthread_create( &p_thread_p_alt[0],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_alt->phase[0] ) ;
  pthread_create( &p_thread_p_alt[1],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_alt->phase[1] ) ;
  pthread_create( &p_thread_p_alt[2],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_alt->phase[2] ) ;
  pthread_create( &p_thread_p_alt[3],        NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_alt->phase[3] ) ;
  
  pthread_create( &p_thread_m_azi,           NULL, (void*)suivi_main_M, pm_azi ) ;
  pthread_create( &p_thread_m_alt,           NULL, (void*)suivi_main_M, pm_alt ) ;

  pthread_create( &suivi->p_menu,            NULL, (void*)SUIVI_MENU,      suivi ) ;
  pthread_create( &suivi->p_suivi_voute,     NULL, (void*)SUIVI_VOUTE,     suivi ) ;
  pthread_create( &suivi->p_suivi_infrarouge,NULL, (void*)SUIVI_INFRAROUGE, suivi ) ;
  pthread_create( &suivi->p_suivi_capteurs,  NULL, (void*)SUIVI_CAPTEURS,  suivi ) ;
  pthread_create( &suivi->p_suivi_clavier,   NULL, (void*)SUIVI_CLAVIER_TERMIOS,  suivi ) ;
  pthread_create( &suivi->p_suivi_lcd,       NULL, (void*)SUIVI_LCD,  suivi ) ;

  // ============================== join des threads  ===================================

  if ( devices->DEVICE_USE_LCD )         pthread_join( suivi->p_suivi_lcd, NULL) ;
  if ( devices->DEVICE_USE_KEYBOARD )    pthread_join( suivi->p_suivi_clavier, NULL) ;
  if ( devices->DEVICE_USE_CAPTEURS )    pthread_join( suivi->p_suivi_capteurs, NULL) ;
  if ( devices->DEVICE_USE_INFRAROUGE )  pthread_join( suivi->p_suivi_infrarouge, NULL) ;

  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_join( p_thread_p_azi[i], NULL) ; 
  }
  pthread_join( p_thread_m_azi, NULL) ;
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_join( p_thread_p_alt[i], NULL) ; 
  }
  pthread_join( p_thread_m_alt, NULL) ;
  
  pthread_join( suivi->p_menu, NULL) ;
  pthread_join( suivi->p_suivi_voute, NULL) ;  

  return 0;
}
// #######################################################################################
// ####### FIN PROGRAMME - FIN PROGRAMME - FIN PROGRAMME - FIN PROGRAMME         - #######
// #######################################################################################

/*
void * SUIVI_CLAVIER_1(SUIVI * suivi) {
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
  suivi->p_threads_id[ g_id_thread++ ] = pthread_self() ;
  signal( SIGTERM, TRAP_SUIVI_CLAVIER) ;
  
  if ( devices->DEVICE_USE_KEYBOARD ) {


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
  
    usleep( suivi->temporisation_clavier );

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

void * SUIVI_CLAVIER_0(SUIVI * suivi) {
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
  suivi->p_threads_id[ g_id_thread++ ] = pthread_self() ;
  signal( SIGTERM, TRAP_SUIVI_CLAVIER) ;
  
  if ( devices->DEVICE_USE_KEYBOARD ) {

  if (newterm(0, stdout, stdin) == 0) {
    fprintf(stderr, "Cannot initialize terminal\n");
    SyslogEno("Cannot initialize terminal") ;
    pthread_cancel( suivi->p_suivi_clavier ) ;
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