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

#include <astro.h>

char  keyboard[4][4][GPIO_TAILLE_BUFFER] = \
  {{"1","4",   "7", "MENU"},\
  {"2" ,"5",   "8", "0"},\
  {"3" ,"6",   "9", "."},\
  {"MES" ,"NGC", "ETO", "valider"}} ;
  
char  raquette[4][4][GPIO_TAILLE_BUFFER] = \
  {{"no" ,"o"    ,"so"     ,"MENU"},\
  { "n"  ,"reset","s"      ,"0" },\
  { "ne" ,"e"    ,"se"     ,"."},\
  { "MES"  ,"NGC"  , "ETO", "valider"}} ;
  
char  keyboard_ir[4][4][GPIO_TAILLE_BUFFER] = \
  {{"KEY_1" ,"KEY_4" , "KEY_7" , "KEY_MENU"}, \
  { "KEY_2" ,"KEY_5" , "KEY_8" , "KEY_0" },\
  { "KEY_3" ,"KEY_6" , "KEY_9" , "KEY_P"}, \
  { "KEY_M" ,"KEY_N" , "KEY_E",  "KEY_OK"}} ;
  
char  raquette_ir[4][4][GPIO_TAILLE_BUFFER] = \
  {{"KEY_1"  ,"KEY_LEFT"  , "KEY_7"  , "KEY_MENU"}, \
  { "KEY_UP" ,"KEY_OK"    , "KEY_DOWN" , "KEY_0" },\
  { "KEY_3"  ,"KEY_RIGHT" , "KEY_9" , "KEY_P"}, \
  { "KEY_M" ,"KEY_N" , "KEY_E",  "KEY_OK"}} ;

int        incrlog ;
int        id_thread ;  
int        retour ;
int        incr ;
int        alarme ;

//==========================================================
void TRAP_MAIN(int sig) {
  int i ;
  
  TRACE("Signal trappe depuis main = %d",sig) ;
  TRACE("Envoi d'un signal %d (SIGTERM) aux threads",SIGTERM) ;
  
  GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 100) ;

  //printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;
  
  for(i=0;i<id_thread;i++)  {
    TRACE("Abandon thread numero %d de p_thread_t = %d",i,(int)suivi->p_threads_id[i]) ;
    pthread_cancel(suivi->p_threads_id[i]);
  }
  
  if (sig==0) {
   GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 100) ;

   if ( system("/sbin/halt") < 0 ) {
     printf("Probleme avec /sbin/halt\n") ;
     exit(2) ;
   } 
  }
  GPIO_SET( GPIO_LED_ETAT, 0 ) ;
  exit(0) ;
}
//========================================================================================
// FIXME : TRAP_SUIVI_* : 
// FIXME : * ce sont les fonctions de recuperation des signaux definis dans les threads
//========================================================================================

void TRAP_SUIVI_MENU(int sig)  {
  
  printf("Signal trappe depuis thread suivi_menu = %d\n",sig) ;
  pthread_cancel( suivi->p_menu ) ;
}
void TRAP_SUIVI_VOUTE(int sig)  {
  
  printf("Signal trappe depuis thread suivi_voute= %d\n",sig) ;
  pthread_cancel( suivi->p_suivi_calculs ) ;
}
void TRAP_SUIVI_INFRAROUGE(int sig)  {
  
  printf("Signal trappe depuis thread suivi_infrarouge= %d\n",sig) ;
  pthread_cancel( suivi->p_suivi_infrarouge ) ;
  TRAP_MAIN(1) ;
}
void TRAP_SUIVI_CAPTEURS(int sig)  {
  
  printf("Signal trappe depuis thread suivi_capteurs= %d\n",sig) ;
  pthread_cancel( suivi->p_suivi_capteurs ) ;
}

// #######################################################################################

void SUIVI_MENU_PREALABLE (SUIVI *suivi) {
/*
  MENU_MANUEL_0,   
  MENU_MANUEL_1,
  MENU_AZIMUTAL,
  MENU_EQUATORIAL,
  MENU_GOTO,
  MENU_INFO,
  MENU_RESEAU_UP,
  MENU_RESEAU_DOWN,
  MENU_PROGRAMME_DOWN,
  MENU_DOWN,
  MENU_MANUEL_BRUT
*/
// TODO : voir la reelle utilite de SUIVI_MENU_PREALABLE et des variables suivi->SUIVI_MANUEL , suivi->SUIVI_EQUATORIAL , etc ..

  switch ( suivi->menu ) {
    case MENU_MANUEL_BRUT : ; break ;
    case MENU_MANUEL_0   : suivi->SUIVI_MANUEL     = 1 ; break ;
    case MENU_MANUEL_1   : suivi->SUIVI_MANUEL     = 1 ; break ;
    case MENU_AZIMUTAL   : suivi->SUIVI_EQUATORIAL = 0 ; break ;
    case MENU_EQUATORIAL : suivi->SUIVI_EQUATORIAL = 1 ; break ;
    case MENU_GOTO       : suivi->SUIVI_GOTO       = 1 ; break ;
    case MENU_INFO       : break ;
    case MENU_RESEAU_UP  : break ;
    case MENU_RESEAU_DOWN  : break ; 
    case MENU_PROGRAMME_DOWN  : break ; 
    case MENU_DOWN  : break ; 
  }
}
// #######################################################################################

void SUIVI_TRAITEMENT_MOT( SUIVI *suivi, CLAVIER *clavier ) {
  
  char cmd[256] ;
  char s_buffer4[ CONFIG_TAILLE_BUFFER_32 * 4 ] ;

  memset( cmd,       ZERO_CHAR, strlen( cmd )) ;
  memset( s_buffer4, ZERO_CHAR, strlen( s_buffer4 )) ;
  
  //---------------------------------------------------------------------------------------------------------
  // TRAITEMENT DES ACTIONS SANS NECESSAIREMENT UNE VALIDATION
  // exemple : on appuie sur la touche ouest est sud nors : appel
  //---------------------------------------------------------------------------------------------------------
  
  // On change de MENU si on appuie sur EST OUEST NORD ou SUD => 
  // on passe en MENU 0 

  suivi->menu_old = suivi->menu ;

  if ( ! strcmp( clavier->mot, "o" ))         { suivi->menu = MENU_MANUEL_0 ; strcpy(clavier->mot,"") ; }
  if ( ! strcmp( clavier->mot, "e" ))         { suivi->menu = MENU_MANUEL_0 ; strcpy(clavier->mot,"") ; }
  if ( ! strcmp( clavier->mot, "s" ))         { suivi->menu = MENU_MANUEL_0 ; strcpy(clavier->mot,"") ; }
  if ( ! strcmp( clavier->mot, "n" ))         { suivi->menu = MENU_MANUEL_0 ; strcpy(clavier->mot,"") ; }
  if ( ! strcmp( clavier->mot, "reset" ))     { suivi->menu = MENU_MANUEL_0 ; strcpy(clavier->mot,"") ; }
  if ( ! strcmp( clavier->mot, "stop" ))      { suivi->SUIVI_VOUTE = 0 ; strcpy(clavier->mot,"") ; }
  if ( ! strcmp( clavier->mot, "play" ))      { suivi->SUIVI_VOUTE = 1 ; strcpy(clavier->mot,"") ; }

  //  appel a la volee

  if ( ! strcmp( clavier->mot, "key_screen"))     { suivi->menu = MENU_MANUEL_BRUT ; strcpy(clavier->mot,"") ; }  
  //if ( ! strcmp( clavier->mot, "key_tv"))         { suivi->menu = MENU_MANUEL_BRUT ; strcpy(clavier->mot,"") ; }  
  if ( ! strcmp( clavier->mot, "key_info" ))      { suivi->menu = MENU_INFO ; strcpy(clavier->mot,"") ; }     // mode info

  if ( ! strcmp( clavier->mot, "key_mode" ))      { suivi->menu = MENU_EQUATORIAL ; strcpy(clavier->mot,"") ; }     // mode equaotorial
  if ( ! strcmp( clavier->mot, "key_list" ))      { suivi->menu = MENU_AZIMUTAL ; strcpy(clavier->mot,"") ; }       // mode azimutal

  if ( ! strcmp( clavier->mot, "key_power" ))     { suivi->menu = MENU_DOWN ; strcpy(clavier->mot,"") ;  }          // arret complet
  if ( ! strcmp( clavier->mot, "key_exit" ))      { suivi->menu = MENU_PROGRAMME_DOWN ; strcpy(clavier->mot,"") ; } // arret du programme
  if ( ! strcmp( clavier->mot, "key_reseau_up"))  { suivi->menu = MENU_RESEAU_UP ; strcpy(clavier->mot,"") ; }      // activation du reseau
  
  
  //-----------------------------------------------------------------
  // Si un  mot est lu sur le clavier (appui sur menu->valider) necessaire)
  // on effectue une action
  // Si autre chose qu'un menu est lu sur le clavier , on peut aussi changer
  // de menu (pour plus de rapidite)
  //-----------------------------------------------------------------
  
  if ( clavier->phrase_lue ) {
    
    //---------------------------------
    // Si un MENU est entrer au clavier
    // => gestion des MENUS en fonction du clavier
    // ou de toute autre entree exterieure
    // autre qu'un switch electronique
    //---------------------------------
    
    TRACE("symbole = %s nombre = %s\n", clavier->symbole, clavier->nombre ) ;
    
    //--------------------------------------------------------------------
    // ON CHANGE DE MENU VOLONTAIREMENT
    //--------------------------------------------------------------------
    
    if ( ! strcmp( clavier->symbole, clavier->menu )) {
      suivi->menu_old = suivi->menu ;
      suivi->menu     = atoi( clavier->nombre ) ; 
    }
    
    //---------------------------------------------------------------------------------------------------------
    // Si un nouvel astre a ete pris en compte
    //---------------------------------------------------------------------------------------------------------
    
    sprintf( s_buffer4, "%s%s%s%s", CONFIG_MES, CONFIG_NGC, CONFIG_ETO, CONFIG_PLA) ;

    if ( strstr( s_buffer4, clavier->symbole ) != NULL ) {

      memset( astre->nom, ZERO_CHAR, strlen(astre->nom)) ;
      sprintf( astre->nom, "%s%s", clavier->symbole, clavier->nombre) ;
    
      TRACE("%s a ete pris en compte",astre->nom) ;
        LOG("%s a ete pris en compte",astre->nom) ;
      
      if ( strstr( astre->nom, CONFIG_MES ) != NULL ) CAT_FIND( astre, cat_dec) ;
      if ( strstr( astre->nom, CONFIG_NGC ) != NULL ) CAT_FIND( astre, cat_dec) ;
      if ( strstr( astre->nom, CONFIG_ETO ) != NULL ) CAT_FIND( astre, etoiles_dec) ;
      
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

        case 4 : // Si longueur = 4 cela ne peut etre qu'un changement d'heure et de minutes
          
          TRACE("demande changement heure minutes : %s" , clavier->nombre) ;
          LOG("demande changement heure minutes : %s" , clavier->nombre) ;
    
          CONFIG_SET_HOUR_AND_MINUTES( clavier->nombre ) ;
          
        break ;
          
          // FIXME : 20190822 : modification configuration de la date

        case 8 : // Si longueur = 5 cela est un changement de mois jour avec verif de l'annee

          TRACE("demande changement annee : %s" , clavier->nombre) ;
          LOG("demande changement annee : %s" , clavier->nombre) ;
          
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
          if ( suivi->DONNEES_CAPTEURS ) { astre->a = suivi->pitch ;astre->h = suivi->heading ; }
          CALCUL_EQUATEUR ( lieu, astre) ;
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
//==========================================================
// MODE MANUEL 0 : le suivi etant effectue sur un astre (calcul des vitesses et periodes par suivi_voute)
// l'appui sur les touches N-S-E-O provoque une suspension de suivi_voute jusqua touche OK
// avec une multiplication des vitesses N-S-E-O par un facteur ALT_ACC
//==========================================================
// Ce mode permet le centrage / recentrage de l'objet tout en ayant le suivi.
//==========================================================

void SUIVI_MANUEL_0(SUIVI * suivi, CLAVIER *clavier) {
  
  int flag = 0 ;
  int flag_calcul = 0 ;

  TRACE1("start") ;

  if ( suivi->DONNEES_RAQUETTE )   GPIO_KEYBOARD_RAQUETTE_READ( gpio_key_l, gpio_key_c, raquette, suivi) ;
  if ( suivi->DONNEES_INFRAROUGE ) IR_KEYBOARD_RAQUETTE_READ( suivi) ;

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

    TRACE("reset : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f",\
           suivi->Sh , suivi->Sa, suivi->pas_azi, suivi->pas_alt, suivi->acc_azi , suivi->acc_alt) ;
           
      LOG("reset : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f",\
           suivi->Sh , suivi->Sa, suivi->pas_azi, suivi->pas_alt, suivi->acc_azi , suivi->acc_alt) ;
           
    pthread_mutex_unlock(& suivi->mutex_azi );
    pthread_mutex_unlock(& suivi->mutex_alt );

    flag_calcul = 1 ;
  }
  // -----------------------------------------------------------
  // actions a faire suivant appui sur NORD / SUD / OUEST /EST
  //------------------------------------------------------------

  if ( suivi->pas_nord || suivi->pas_sud || suivi->pas_est || suivi->pas_ouest ) flag = 1 ;

  // flag = 1 = on a appuye sur N-S-E-O

  if (flag ) { 
  
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

    TRACE("pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", suivi->pas_azi, suivi->pas_alt, suivi->acc_azi , suivi->acc_alt) ;
      LOG("pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", suivi->pas_azi, suivi->pas_alt, suivi->acc_azi , suivi->acc_alt) ;

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

    TRACE("acc*ALTAZ_FORWARD %.4f acc_azi %.4f acc_alt %.4f", ALTAZ_FORWARD, suivi->acc_azi, suivi->acc_alt ) ;
      LOG("acc*ALTAZ_FORWARD %.4f acc_azi %.4f acc_alt %.4f", ALTAZ_FORWARD, suivi->acc_azi, suivi->acc_alt ) ;
    
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

    TRACE("acc/ALTAZ_REWIND %.4f acc_azi %.4f acc_alt %.4f ", ALTAZ_REWIND, suivi->acc_azi, suivi->acc_alt ) ;
      LOG("acc/ALTAZ_REWIND %.4f acc_azi %.4f acc_alt %.4f ", ALTAZ_REWIND, suivi->acc_azi, suivi->acc_alt ) ;
    
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

    TRACE("acc*ALTAZ_FORWARD_FAST %.4f acc_azi %.4f acc_alt %.4f", ALTAZ_FORWARD_FAST, suivi->acc_azi, suivi->acc_alt ) ;
      LOG("acc*ALTAZ_FORWARD_FAST %.4f acc_azi %.4f acc_alt %.4f", ALTAZ_FORWARD_FAST, suivi->acc_azi, suivi->acc_alt ) ;
    
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

    TRACE("acc/ALTAZ_REWIND_FAST  %.4f acc_azi %.4f acc_alt %.4f ", ALTAZ_REWIND_FAST, suivi->acc_azi, suivi->acc_alt ) ;
      LOG("acc/ALTAZ_REWIND_FAST  %.4f acc_azi %.4f acc_alt %.4f ", ALTAZ_REWIND_FAST, suivi->acc_azi, suivi->acc_alt ) ;
      
    suivi->pas_rewind_fast  = 0 ;
    flag_calcul = 1 ;
  }
  // ======================================================
  // Rafraichissement des vitesses et periodes
  // ======================================================

  if ( flag_calcul ) {
  
    pthread_mutex_lock( & suivi->mutex_cal );
    
    CALCUL_VITESSES( lieu, astre, suivi) ;
    CALCUL_PERIODE ( astre, suivi, voute) ;
    
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
  
  TRACE1("start") ;
  
  azi = 0 ;
  alt = 0 ;
  
  if ( suivi->DONNEES_RAQUETTE )   GPIO_KEYBOARD_RAQUETTE_READ( gpio_key_l, gpio_key_c, raquette, suivi) ;
  if ( suivi->DONNEES_INFRAROUGE ) IR_KEYBOARD_RAQUETTE_READ( suivi) ;
  
  // La determination de tempo_raq est tres importante
  // Elle varie suivant la reduction total du moteur
  
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

    if ( suivi->DONNEES_RAQUETTE   ) GPIO_KEYBOARD_RAQUETTE_READ( gpio_key_l, gpio_key_c, raquette, suivi) ;
    if ( suivi->DONNEES_INFRAROUGE ) IR_KEYBOARD_RAQUETTE_READ( suivi) ;
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
        astre->Va,astre->Vh,suivi->Ta,suivi->Th,suivi->Fa,suivi->Fh, \
	(suivi->Ia - suivi->Ia_prec),(suivi->Ih - suivi->Ih_prec )) ;  
  }

}
// #######################################################################################
// ####### GESTION DU MENU - GESTION DU MENU - GESTION DU MENU - GESTION DU MENU - #######
// #######################################################################################

void * SUIVI_MENU(SUIVI * suivi) {
  
  struct sched_param param;
  
   // La temporisation dans les boucles du thread SUIVI_MENU depend de  suivi->temporisation_menu
   
  TRACE("start") ;
    
  param.sched_priority = 1 ;

  if (pthread_setschedparam( pthread_self(), SCHED_FIFO, & param) != 0) { 
    perror("setschedparam SUIVI_MENU");
    exit(EXIT_FAILURE); 
  }

  signal(SIGTERM,TRAP_SUIVI_MENU) ;
  
  SUIVI_MENU_PREALABLE ( suivi) ;

  usleep( suivi->temporisation_menu ) ;
  suivi->reset = 1 ;

  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread SUIVI_MENU
  //-------------------------------------------------------------------------------
  
  
  while(1) {

    usleep( suivi->temporisation_menu ) ;

    if ( suivi->DONNEES_RAQUETTE )   GPIO_KEYBOARD_READ( gpio_key_l, gpio_key_c, keyboard, clavier) ;
    if ( suivi->DONNEES_INFRAROUGE ) IR_KEYBOARD_READ( suivi, clavier) ;

    // IR_ACTIONS_PARTICULIERES( suivi) ;

    SUIVI_TRAITEMENT_MOT( suivi, clavier ) ;

    CONFIG_AFFICHER_CHANGEMENTS( suivi ) ;

    switch( suivi->menu ) {

      // -------------------------- SUIVI MANUEL PAR DEFAUT ------------------
      case MENU_MANUEL_0 :

        // le but de ce suivi est de deduire des actions N-S-O-E de l'utilisateur 
        // simplement une acceleration / ralentissement / changement de direction
        // sur le N-S-E-O (pas de recalcul des periodes) 
        // FIXME : les periodes sont conservees , ainsi que le mode azimutal ou equatorial

        SUIVI_MANUEL_0(suivi, clavier) ;

        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_0 ; 

      break ;
      // -------------------------- SUIVI MANUEL AVEC RE-CALCUL DES PERIODES  -----------------
      case MENU_MANUEL_1 :
       
        // TODO : a modifier car cela marche pas tres bien (interraction avec le thread SUIVI_VOUTE)
        // TODO : le but de ce suivi est de deduire des actions N-S-O-E de l'utilisateur 
        // TODO : les periodes / frequences en azimut et altitude

        SUIVI_MANUEL_1(suivi, clavier) ; 
        CALCUL_PERIODES_SUIVI_MANUEL(astre,suivi,voute)  ;

        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_1 ; 

      break ;
      // ------------------------------- ALIGNEMENT - MODE AZIMUTAL ----------------------------
     
      case MENU_AZIMUTAL :

        // a modifier / completer : CALCUL_TEMPS_SIDERAL et CALCUL_ANGLE_HORAIRE
        // sont a supprimer car deja calculer dans SUIVI_

        LOG("appel : %d : MENU_AZIMUTAL" , suivi->menu) ;

        pthread_mutex_lock(& suivi->mutex_azi );   
        pthread_mutex_lock(& suivi->mutex_alt );
        suivi->acc_alt          = 1 ;
        suivi->acc_azi          = 1 ;
        pthread_mutex_unlock(& suivi->mutex_azi );   
        pthread_mutex_unlock(& suivi->mutex_alt );

        suivi->SUIVI_EQUATORIAL = 0 ;
        //suivi->SUIVI_VOUTE      = 1 ; // TODO : verifier utilite SUIVI_VOUTE
        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_0 ; 

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
        //suivi->SUIVI_VOUTE      = 1 ; // TODO : verifier utilite SUIVI_VOUTE
        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_0 ;

      break ;
      // -------------------------- SMENU_MANUEL_BRUT : LE PLUS SIMPLE ------------------
      
      case MENU_MANUEL_BRUT :

        // Suivi le plus simple : seules les touches est nord sud ouest et reset sont prises en compte
        // TODO : verifier

        TRACE("appel : %d : MENU_MANUEL_BRUT" , suivi->menu) ;
        LOG("appel : %d : MENU_MANUEL_BRUT" , suivi->menu) ;

        pthread_mutex_lock(& suivi->mutex_azi );   
        pthread_mutex_lock(& suivi->mutex_alt );
        suivi->acc_alt          = 0 ;
        suivi->acc_azi          = 0 ;
        pthread_mutex_unlock(& suivi->mutex_azi );   
        pthread_mutex_unlock(& suivi->mutex_alt );

        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_0 ; 

      break ;
      // ------------------------------  MODE GOTO --------------------------------------

      case MENU_GOTO :

        TRACE("appel : %d : MENU_GOTO" , suivi->menu) ;
        LOG("appel : %d : MENU_GOTO" , suivi->menu) ;

        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_0 ;

      break ; 

      // ------------------------------  MODE INFO --------------------------------------

      case MENU_INFO :

        TRACE("appel : %d : MENU_INFO" , suivi->menu) ;
        LOG("appel : %d : MENU_INFO" , suivi->menu) ;

        CONFIG_AFFICHER_TOUT(clavier,temps, lieu, astre) ;

        suivi->menu_old         = suivi->menu ;
        suivi->menu             = MENU_MANUEL_0 ;

      break ; 

      // ------------------------------ MODE ACTIVATION DU RESEAU ----------------------------------
     
      case MENU_RESEAU_UP :

        TRACE("appel : %d : MENU_RESEAU_UP" , suivi->menu) ;
        LOG("appel : %d : MENU_RESEAU_UP" , suivi->menu) ;

        if ( system("/etc/init.d/OLD/S40network start")) {
         perror("Probleme avec system(/etc/init.d/OLD/S40network start)"); 
         printf("Probleme avec system(/etc/init.d/OLD/S40network start)\n") ;
        }

        suivi->menu = suivi->menu_old ;
        suivi->menu = MENU_MANUEL_0 ;

      break ;

      // ------------------------------ ARRET DU RESEAU ---------------------------------------
     
      case MENU_RESEAU_DOWN :
     
        TRACE("appel : %d : MENU_RESEAU_DOWN" , suivi->menu) ;
        LOG("appel : %d : MENU_RESEAU_DOWN" , suivi->menu) ;

        if ( system("/etc/init.d/OLD/S40network stop")) {
          perror("Probleme avec system(/etc/init.d/OLD/S40network stop)"); 
          printf("Probleme avec system(/etc/init.d/OLD/S40network stop)\n") ;
        }

        suivi->menu = suivi->menu_old ;
        suivi->menu = MENU_MANUEL_0 ;

      break ;
      // ------------------------------ ARRET DU PROGRAMME ------------------------------------
      case MENU_PROGRAMME_DOWN :
     
        TRACE("appel : %d : MENU_PROGRAMME_DOWN" , suivi->menu) ;
        LOG("appel : %d : MENU_PROGRAMME_DOWN" , suivi->menu) ;


        TRAP_MAIN(1) ;
        break ;
     
      // ------------------------------ ARRET DU SYSTEME EMBARQUE -----------------------------
      case MENU_DOWN :

        TRACE("appel : %d : MENU_DOWN" , suivi->menu) ;
        LOG("appel : %d : MENU_DOWN" , suivi->menu) ;

        TRAP_MAIN(0) ;

      break ;
    }
  }
}
//=============================================================================
// SUIVI_VOUTE :
// le but de la fonction est de rafraichir a intervalles reguliers (1 seconde)
// tous les calculs relatifs a la vitesse de l'astre suivi
//=============================================================================

void * SUIVI_VOUTE(SUIVI * suivi) {
  
  unsigned long  incr ;
  struct timeval t00 ;
  struct sched_param param;
  
  // La temporisation dans la boucle du thread SUIVI_VOUTE depend de voute->DT (en us)
  // a completer / modifier :
  // il FAUT calculer / mettre a jour a,h,A,H de l'astre quand est utliser les menus
  // suivi manuel et suivi capteurs , qui renvoient l'azimut et l'altitude
  // ==> CALCUL_EQUATEUR pour les donnees altitude et azimut venant du capteur
  // ==> autre calcul plus complique quand on a les donnees de vitesses et periodes provenant de la raquette !!!!!
  
  TRACE("start") ;
  
  param.sched_priority = 1 ;
  
  if (pthread_setschedparam( pthread_self(), SCHED_FIFO, & param) != 0) {perror("setschedparam SUIVI_VOUTE"); exit(EXIT_FAILURE);}
  suivi->p_threads_id[ id_thread++ ] = pthread_self() ;
  signal( SIGTERM, TRAP_SUIVI_VOUTE) ;
  
  suivi->d_temps = 0 ;
  incr =0 ;
  
  gettimeofday(&t00,NULL) ;
  
  CONFIG_VOUTE( voute, 1, 1, 0.985 ) ;
  
  TRACE("suivi->SUIVI_VOUTE=%d",suivi->SUIVI_VOUTE) ;

  // FIXME : 
  // en mode equatorial, pas besoin de SUIVI_VOUTE, en effet la vitesse ne change pas
  // Va=15"/s et Vh=0 (le moteur en ascension droite est commande par azimut)
  
  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread SUIVI_VOUTE
  //-------------------------------------------------------------------------------
  
  while(1) {
    if ( suivi->SUIVI_VOUTE ) {
      
      /* FIXME : modification 20121225 : tous les calculs generiques dans CALCUL_TOUT */
      
      CALCUL_TOUT(lieu, temps, astre, suivi, clavier ) ;
    
      if ( suivi->SUIVI_ALIGNEMENT )          CONFIG_AFFICHER_ASTRE(astre) ;
      if ( suivi->menu_old != suivi->menu  )  CONFIG_AFFICHER_TOUT(clavier,temps, lieu, astre) ;

      astre->A += voute->pas ;
      
      suivi->d_temps += CALCUL_TEMPORISATION_VOUTE( voute, t00 ) ; 
      gettimeofday(&t00,NULL) ;
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
}
//==========================================================
void * SUIVI_INFRAROUGE(SUIVI * suivi) {
   
  struct sched_param param;
  
  TRACE("start") ;
  
  // La temporisation dans la boucle du thread SUIVI_INFRAROUGE depend de suivi->temporisation_ir (en us)
  // present dans la fonction bloquante LIRC_READ 
  
  // on laisse le temps aux autres threads de demarrer
  // notamment pour arriver dans la fonction SUIVI_TRAITEMENT_MENU
  // qui initialise a 1 SUIVI_INFRAROUGE
  
  sleep(1) ;
  
  param.sched_priority = 1  ;
  
  if (pthread_setschedparam( pthread_self(), SCHED_FIFO, & param) != 0) { perror("setschedparam SUIVI_INFRAROUGE"); exit(EXIT_FAILURE);}
  suivi->p_threads_id[ id_thread++ ] = pthread_self() ;
  signal( SIGTERM, TRAP_SUIVI_INFRAROUGE) ;
  
  if ( suivi->DONNEES_INFRAROUGE ) {
    
    LIRC_CONFIG_CODES( irc) ;
    LIRC_OPEN( lircconfig ) ;
  
    // ATTENTION !!! la fonction LIRC_READ est bloquante (voir ir.c) !!!!!
    
    LIRC_READ( suivi ) ;
    LIRC_CLOSE(lircconfig) ;
  }
  return NULL ;
}
//==========================================================
void * SUIVI_CAPTEURS(SUIVI * suivi) {
   
  struct sched_param param;
  int ret ;
  
  I2C_DEVICE   exemple, *ex ;
  I2C_ACC_MAG  accmag,  *am ;
  
  TRACE("start") ;
  
  ex = & exemple ;
  am = & accmag ;
  ex->fd = 0 ;
  
  // La temporisation dans la boucle du thread SUIVI_CAPTEURS depend de suivi->temporisation_capteurs (en us)
  // present dans la fonction bloquante LIRC_READ 
  
  // on laisse le temps aux autres threads de demarrer
  // notamment pour arriver dans la fonction SUIVI_TRAITEMENT_MENU
  // qui initialise a 1 SUIVI_CAPTEURS
  
  param.sched_priority = 2  ;
  
  if (pthread_setschedparam( pthread_self(), SCHED_FIFO, & param) != 0) {perror("setschedparam SUIVI_CAPTEURS"); exit(EXIT_FAILURE);}

  suivi->p_threads_id[ id_thread++ ] = pthread_self() ;
  
  signal( SIGTERM, TRAP_SUIVI_CAPTEURS) ;
  
  // a modifier pour definir le choix de l'infrarouge ou pas (config.txt ? .h ? )
  
  while(1) {
    if ( suivi->DONNEES_CAPTEURS ) {
      if ( ! suivi->init_capteurs ) {
        
        ret = I2C_INIT(ex, DEVICE_RASPI_2, DEVICE_LSM_ADRESS ) ;
	
        if ( ! ret ) {
          printf("Pas de capteur disponible\n") ;
          suivi->DONNEES_CAPTEURS = 0 ;
          suivi->init_capteurs = 0 ;
          break ;
        }
        else {
          I2C_SET( ex, REG_CTRL1, "0x57" ) ;
          I2C_SET( ex, REG_CTRL2, "0x00" ) ;
          I2C_SET( ex, REG_CTRL5, "0x64" ) ;
          I2C_SET( ex, REG_CTRL6, "0x20" ) ;
          I2C_SET( ex, REG_CTRL7, "0x00" ) ;
	  
          suivi->init_capteurs = 1 ;
        }
      }
      if ( suivi->init_capteurs ) {
      
        I2C_GET_ACC( ex, am )   ;
        I2C_CALCULS_ACCMAG( am ) ;
      
        suivi->roll    =  am->roll  ; 
        suivi->pitch   =  am->pitch ;
        suivi->heading =  am->heading ;
      
        TRACE1("%.0f\t%.0f\t%.0f\n", am->pitch * I2C_DEGRAD, am->roll * I2C_DEGRAD, am->heading * I2C_DEGRAD) ;
      }	
    }
    usleep( suivi->temporisation_capteurs );
  }
  TRACE("stop") ;
  return NULL ;
}
// #######################################################################################
// ####### DEBUT PROGRAMME - DEBUT PROGRAMME - DEBUT PROGRAMME - DEBUT PROGRAMME - #######
// #######################################################################################

int main(int argc, char ** argv) {
  
  int i ;
  struct sched_param param;

  pthread_t p_thread_p_alt[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  pthread_t p_thread_p_azi[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  pthread_t p_thread_m_alt ;
  pthread_t p_thread_m_azi ;
  // -----------------------------------------------------------------
  
  system("/bin/date > /root/astrokit.begin.date.log") ;
  
  incrlog=0 ;
  
  astre = &as ;
  lieu  = &li;
  voute = &vo ;
  suivi = &su ;
  temps = &te ;
  clavier = &cl ;
  irc = &ir_codes ;
  
  // -----------------------------------------------------------------
  // MISE EN PLACE DES PARAMETRES SYSTEMES (parallelisme, priorites, ..)
  // -----------------------------------------------------------------
 
  system("echo -1 > /proc/sys/kernel/sched_rt_runtime_us") ; 
  mlockall(MCL_CURRENT | MCL_FUTURE);
   
  param.sched_priority = 1 ;
  if (pthread_setschedparam( pthread_self(), SCHED_RR, & param) != 0) {perror("setschedparam main");exit(EXIT_FAILURE);}

  signal(SIGINT,TRAP_MAIN) ;
  signal(SIGALRM,TRAP_MAIN) ;

  id_thread=0 ;
  memset( suivi->p_threads_id, 0 , MAX_THREADS*sizeof(pthread_t)) ;
  
  // -----------------------------------------------------------------
  // LECTURE DES FICHIERS DE CONFIG
  // -----------------------------------------------------------------
  
  CONFIG_INIT_LOG(); 
  LOG( "=== debut astrokit ===" ) ;
    
  CONFIG_READ       ( datas ) ;
  CONFIG_INIT_VAR   ( datas ) ;   
  
  //GPIO_INIT_VAR2     ( datas) ;    // impacte les tableaux gpio_in[] et gpio_out[]

  if ( suivi->DONNEES_RAQUETTE ) GPIO_KEYBOARD_CONFIG( gpio_key_l, gpio_key_c ) ;
  
  // -----------------------------------------------------------------
  
  // CONFIG_AFFICHER_DATAS( datas ) ;
  // CONFIG_AFFICHER_VARIABLES() ;

  printf("==> GPIO_LED_ETAT     = %d\n", GPIO_LED_ETAT );
  printf("==> ASTRE_PAR_DEFAUT, = %s\n", ASTRE_PAR_DEFAUT );

  LOG("==> ASTRE_PAR_DEFAUT : %s", ASTRE_PAR_DEFAUT) ;
  
  // -----------------------------------------------------------------
  // fonctions initialisations des structures de donnees
  // -----------------------------------------------------------------

  CONFIG_INIT_CLAVIER ( clavier ) ;   
  CONFIG_INIT_ASTRE   ( astre ) ;
  CONFIG_INIT_LIEU    ( lieu  ) ;
  CONFIG_INIT_VOUTE   ( voute ) ;
  CONFIG_INIT_SUIVI   ( suivi ) ;
  CONFIG_INIT_TEMPS   ( temps ) ;
  
  // -----------------------------------------------------------------
  // reglages variables particulieres
  // ----------------------------------------------------------------- 

  if ( strcmp(ASTRE_PAR_DEFAUT,"") != 0 ) {

    memset( astre->nom, 0, sizeof(astre->nom)) ;
    strcpy( astre->nom, ASTRE_PAR_DEFAUT ) ;
  }
  // -----------------------------------------------------------------
  
  CAT_READ            ( CAT_NGC_TXT, cat) ;                     // CAT_AFFICHER ( cat ) ; 
  CAT_FORMAT_DECIMAL  ( "ngc.csv.dec" , cat, cat_dec) ;         // CAT_AFFICHER ( cat_dec ) ;
  CAT_READ            ( CAT_ETO_TXT, etoiles) ;                 // CAT_AFFICHER ( etoiles ) ;
  CAT_FORMAT_DECIMAL_2( "eto.csv.dec" , etoiles, etoiles_dec) ; // CAT_AFFICHER ( etoiles_dec ) ;
  
  // -----------------------------------------------------------------

  ARGUMENTS_HELP    ( argc, argv) ;
  ARGUMENTS_GERER   ( argc, argv, lieu, astre, voute, suivi) ;
  
  // ouverture led etat ----------------------------------------------

  if ( GPIO_LED_ETAT != 0 ) {

    GPIO_CLOSE_BROCHE( GPIO_LED_ETAT) ;
    GPIO_OPEN_BROCHE( GPIO_LED_ETAT, 1) ;
    GPIO_SET( GPIO_LED_ETAT, 0 ) ;
  }

  // -----------------------------------------------------------------
  
  TRACE("==> astre->nom     = %s", astre->nom );

  if ( strcmp(astre->nom,"") != 0 ) {

    if ( strstr( astre->nom, CONFIG_MES ) != NULL ) CAT_FIND( astre, cat_dec) ;
    if ( strstr( astre->nom, CONFIG_NGC ) != NULL ) CAT_FIND( astre, cat_dec) ;
    if ( strstr( astre->nom, CONFIG_ETO ) != NULL ) CAT_FIND( astre, etoiles_dec) ;

    CALCUL_TEMPS_SIDERAL  ( lieu, temps ) ;
    CALCUL_ANGLE_HORAIRE  ( lieu, astre ) ;
    CALCUL_AZIMUT         ( lieu, astre) ;
    
    CONFIG_AFFICHER_ASTRE ( astre) ;

  }
  if ( suivi->alarme != 0 ) alarm( suivi->alarme) ;
      
  pm_azi = &m_azi ;
  pm_alt = &m_alt ;
  
  pm_azi->suivi = (SUIVI*)suivi ;   // pour permettre l'acces des membres de SUIVI dans GPIO_PWM_MOTEUR
  pm_alt->suivi = (SUIVI*)suivi ;   // pour permettre l'acces des membres de SUIVI dans GPIO_PWM_MOTEUR
  
  GPIO_INIT_VAR( datas ) ; // impacte les tableaux gpio_alt[], gpio_azi[], gpio_masque[] et gpio_frequence_pwm[]
  
  printf("gpio_alt[x]=") ;    for(i=0;i<4;i++) printf("%d ",gpio_alt[i]) ; printf("\n") ;
  printf("gpio_azi[x]=") ;    for(i=0;i<4;i++) printf("%d ",gpio_azi[i]) ; printf("\n") ;
  printf("gpio_masque[x]=") ; for(i=0;i<4;i++) printf("%d ",gpio_masque[i]) ; printf("\n") ;
  
  LOG("gpio_alt[x]=") ;    for(i=0;i<4;i++) LOG("%d ",gpio_alt[i]) ; LOG("\n") ;
  
  GPIO_INIT_PWM_MOTEUR_2(\
    pm_alt,\
    gpio_alt,\
    gpio_masque,\
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
    gpio_masque,\
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
  
  TRACE("suivi->DONNEES_INFRAROUGE = %d",suivi->DONNEES_INFRAROUGE) ;
  TRACE("suivi->DONNEES_CAPTEURS   = %d",suivi->DONNEES_CAPTEURS) ;
  TRACE("suivi->DONNEES_RAQUETTE   = %d",suivi->DONNEES_RAQUETTE) ;
  TRACE("suivi->DONNEES_BLUETOOTH  = %d",suivi->DONNEES_BLUETOOTH) ;

  TRACE("MAIN avant THREADS = Ta=%2.6f Th=%2.6f Fa=%2.6f Fh=%2.6f\n",suivi->Ta,suivi->Th,suivi->Fa,suivi->Fh) ;
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_create( &p_thread_p_azi[i], NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_azi->phase[i] ) ;
  }
  pthread_create( &p_thread_m_azi,    NULL, (void*)suivi_main_M, pm_azi ) ;
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_create( &p_thread_p_alt[i], NULL, (void*)GPIO_SUIVI_PWM_PHASE, pm_alt->phase[i] ) ;
  }
  pthread_create( &p_thread_m_alt,    NULL, (void*)suivi_main_M, pm_alt ) ;
  
  pthread_create( &suivi->p_suivi_calculs,   NULL, (void*)SUIVI_VOUTE,     suivi ) ;
  pthread_create( &suivi->p_menu,            NULL, (void*)SUIVI_MENU,      suivi ) ;

  if ( suivi->DONNEES_INFRAROUGE ) pthread_create( &suivi->p_suivi_infrarouge,NULL, (void*)SUIVI_INFRAROUGE, suivi ) ;
  if ( suivi->DONNEES_CAPTEURS )   pthread_create( &suivi->p_suivi_capteurs,  NULL, (void*)SUIVI_CAPTEURS,  suivi ) ;
  
  // ============================== join des threads  ===================================
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ )
  pthread_join( p_thread_p_azi[i], NULL) ; 
  pthread_join( p_thread_m_azi, NULL) ;
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ )
  pthread_join( p_thread_p_alt[i], NULL) ; 
  pthread_join( p_thread_m_alt, NULL) ;
  
  pthread_join( suivi->p_menu, NULL) ;
  pthread_join( suivi->p_suivi_calculs, NULL) ;

  if ( suivi->DONNEES_INFRAROUGE ) pthread_join( suivi->p_suivi_infrarouge, NULL) ;
  if ( suivi->DONNEES_CAPTEURS )   pthread_join( suivi->p_suivi_capteurs, NULL) ;
  
  return 0;
}
// #######################################################################################
// ####### FIN PROGRAMME - FIN PROGRAMME - FIN PROGRAMME - FIN PROGRAMME         - #######
// #######################################################################################
