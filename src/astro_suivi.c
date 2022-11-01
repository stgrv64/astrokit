/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
--------------------------------------------------------------
date        | commentaires 
--------------------------------------------------------------
07/10/2022  | * creation a partir du code de astro.c et autre
-------------------------------------------------------------- 
*/

#include "astro_suivi.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

const char * gc_const_menus[] = {
  "MENU_AZIMUTAL",
  "MENU_EQUATORIAL",
  "MENU_MANUEL_BRUT",
  "MENU_MANUEL_NON_ASSERVI",
  "MENU_MANUEL_ASSERVI",
  "MENU_GOTO",
  "MENU_INFO",
  "MENU_RESEAU_UP",
  "MENU_RESEAU_DOWN",
  "MENU_PROGRAMME_DOWN",
  "MENU_DOWN"
} ;

/*****************************************************************************************
* @fn     : SUIVI_TEMPORISATION_AZIMUT
* @author : s.gravois
* @brief  : Cette fonction permet de faire une temporisation dans le thread SUIVI_AZIMUT de astro.c
* @param  : STRUCT_SUIVI * gp_Sui
* @param  : struct timeval * pt00
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
* @date   : 2022-10-11 deplacement dans astro_suivi.c
* @todo   : TODO : supprimer (obsolete) 
*****************************************************************************************/

void SUIVI_TEMPORISATION_AZIMUT(STRUCT_SUIVI * gp_Sui, struct timeval * pt00) {
  
  struct timeval t01 ;
  double t_diff ;
  
  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // temporisation pour ne pas consommer de CPU
  // on sommeille = pourcentage_tempo du usleep 
  
  usleep ( (long)(gp_Sui->DTa * gp_Sui->pourcentage_tempo) ) ;
  
  t_diff=0;
  while( t_diff < gp_Sui->DTa ) {
     
   gettimeofday(&t01,NULL) ;
   t_diff = (( t01.tv_sec - pt00->tv_sec)  * TEMPS_MICRO_SEC ) + t01.tv_usec - pt00->tv_usec;
  }
  gettimeofday(pt00,NULL) ;
}

/*****************************************************************************************
* @fn     : SUIVI_TEMPORISATION_AZIMUT
* @author : s.gravois
* @brief  : Cette fonction permet de faire une temporisation dans le thread SUIVI_ALTITUDE de astro.c
* @param  : STRUCT_SUIVI * gp_Sui
* @param  : struct timeval * pt00
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
* @date   : 2022-10-11 deplacement dans astro_suivi.c
* @todo   : TODO : supprimer (obsolete) 
*****************************************************************************************/

void SUIVI_TEMPORISATION_ALTITUDE(STRUCT_SUIVI * gp_Sui, struct timeval * pt00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // temporisation pour ne pas consommer de CPU
  // on sommeille = pourcentage_tempo du usleep 
  
  usleep ( (long)(gp_Sui->DTh * gp_Sui->pourcentage_tempo) ) ;
  
  t_diff=0;
  while( t_diff < gp_Sui->DTh ) {
     
   gettimeofday(&t01,NULL) ;
   t_diff = (( t01.tv_sec - pt00->tv_sec)  * TEMPS_MICRO_SEC ) + t01.tv_usec - pt00->tv_usec;
  }
  gettimeofday(pt00,NULL) ;
}

/*****************************************************************************************
* @fn     : SUIVI_MAJ_PAS
* @author : s.gravois
* @brief  : Cette fonction met a jour les valeurs de gp_Sui->pas* en fonction
* @brief  : du contenu de gp_Sui->sui_dat.dat_inf
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
*****************************************************************************************/

void SUIVI_MAJ_PAS( STRUCT_SUIVI * gp_Sui) {

  char c_act = '0' ;

  ARBO(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->use_infrarouge ) {
    
    pthread_mutex_lock(& gp_Mut->mut_dat );

    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "plus" ) )         { c_act='1'; gp_Sui->pas_acc_plus  = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "moins" ) )        { c_act='1'; gp_Sui->pas_acc_moins = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "forward" ) )      { c_act='1'; gp_Sui->pas_forward  = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "rewind" ) )       { c_act='1'; gp_Sui->pas_rewind = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "forwardfast" ) )  { c_act='1'; gp_Sui->pas_forward_fast  = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "rewindfast" ) )   { c_act='1'; gp_Sui->pas_rewind_fast = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "ne" ) )           { c_act='1'; gp_Sui->pas_nord=1 ; gp_Sui->pas_est=1   ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "no" ) )           { c_act='1'; gp_Sui->pas_nord=1 ; gp_Sui->pas_ouest=1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "se" ) )           { c_act='1'; gp_Sui->pas_sud=1  ; gp_Sui->pas_est=1   ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "so" ) )           { c_act='1'; gp_Sui->pas_sud=1  ; gp_Sui->pas_ouest=1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "n" ) )            { c_act='1'; gp_Sui->pas_nord  = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "o" ) )            { c_act='1'; gp_Sui->pas_ouest = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "e" ) )            { c_act='1'; gp_Sui->pas_est   = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "s" ) )            { c_act='1'; gp_Sui->pas_sud   = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "reset" ) )        { c_act='1'; gp_Sui->reset   = 1 ; }

    /* Si gp_Sui->sui_dat.dat_inf a ete utilise, il peut etre remis a zero */

    if ( c_act == '1' ) {
      Trace("raz de gp_Sui->sui_dat.dat_inf") ;
      memset( gp_Sui->sui_dat.dat_inf, 0, strlen( gp_Sui->sui_dat.dat_inf ) ) ;
      strcpy( gp_Sui->sui_dat.dat_inf, "") ;
    }
    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_dat );

    TRACE2("%ld %ld %ld %ld %d %d\n", \
      gp_Sui->pas_ouest, \
      gp_Sui->pas_est, \
      gp_Sui->pas_nord, \
      gp_Sui->pas_sud, \
      gp_Sui->pas_acc_plus, \
      gp_Sui->pas_acc_moins );
  }
  ARBO(__func__,2,"fin") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
}

/*****************************************************************************************
* @fn     : SUIVI_OLD_0
* @author : s.gravois
* @brief  : realise certaines actions particulieres apres appui sur une touche
* @param  : STRUCT_SUIVI   *gp_Sui
* @param  : STRUCT_KEYS *clavier 
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-30 amelioration et visibilite du contenu fct 
* @date   : 2022-05-30 ajout des mots concernant les touches F1->F12 du gp_Key
* @todo   : analyser pour simplification <=> avec suivi menu etc..
*****************************************************************************************/

void SUIVI_OLD_0( STRUCT_SUIVI * gp_Sui) {
    
  if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "plus" ) )  {
    gp_Sui->Ta_mic *=  gp_Sui->plus  ; gp_Sui->Fa_mic = 1 / gp_Sui->Ta_mic ;
    gp_Sui->Th_mic *=  gp_Sui->plus  ; gp_Sui->Fh_mic = 1 / gp_Sui->Th_mic ;
  }
  if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "moins" ) ) {
    gp_Sui->Ta_mic *=  gp_Sui->moins  ; gp_Sui->Fa_mic = 1 / gp_Sui->Ta_mic ;
    gp_Sui->Th_mic *=  gp_Sui->moins  ; gp_Sui->Fh_mic = 1 / gp_Sui->Th_mic ;
  }
}

/*****************************************************************************************
* @fn     : SUIVI_MENU_PREALABLE
* @author : s.gravois
* @brief  : fonction qui initialise des attributs 
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-10-07 deplacement code depuis astro.c 
* @todo   : TODO : analyser la reelle utilite de cette fonction
            en diminuant le bombre de variable gp_Sui->var
*****************************************************************************************/

void SUIVI_MENU_PREALABLE (STRUCT_SUIVI * gp_Sui) {

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  switch ( gp_Sui->menu ) {
    case MENU_AZIMUTAL            : gp_Sui->SUIVI_EQUATORIAL = 0 ; break ;
    case MENU_EQUATORIAL          : gp_Sui->SUIVI_EQUATORIAL = 1 ; break ;
    case MENU_MANUEL_BRUT         : gp_Sui->SUIVI_MANUEL     = 1 ; break ;
    case MENU_MANUEL_NON_ASSERVI  : gp_Sui->SUIVI_MANUEL     = 1 ; break ;
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
* @param  : STRUCT_SUIVI   *gp_Sui
* @param  : STRUCT_KEYS *clavier 
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-30 amelioration et visibilite du contenu fct 
* @date   : 2022-05-30 ajout des mots concernant les touches F1->F12 du gp_Key
* @todo   : analyser pour simplification <=> avec suivi menu etc..
*****************************************************************************************/

void SUIVI_TRAITEMENT_MOT( STRUCT_SUIVI * gp_Sui, STRUCT_KEYS *gp_Key ) {
  int i=0;
  char cmd[256]={0} ;
  char s_buffer4[ CONFIG_TAILLE_BUFFER_32 * 4 ]={0} ;
  char c_l0[16] ={0};
  char c_l1[16] ={0};

  ARBO(__func__,1,"start") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  Trace("start") ;

  memset( c_l0, CALCUL_ZERO_CHAR, sizeof( c_l0 )) ;
  memset( c_l1, CALCUL_ZERO_CHAR, sizeof( c_l1 )) ;

  memset( cmd,       CALCUL_ZERO_CHAR, sizeof( cmd )) ;
  memset( s_buffer4, CALCUL_ZERO_CHAR, sizeof( s_buffer4 )) ;
  
  //---------------------------------------------------------------------------------------------------------
  // TRAITEMENT DES ACTIONS SANS NECESSAIREMENT UNE VALIDATION
  // exemple : on appuie sur la touche ouest est sud nors : appel MENU_MANUEL_BRUT
  //---------------------------------------------------------------------------------------------------------
  
  // On change de MENU si on appuie sur EST OUEST NORD ou SUD => 
  // on passe en MENU 0 

  gp_Sui->menu_old = gp_Sui->menu ;

  /*  touche OUEST */

  /* Si on a un mot non vide */
  if ( strcmp( gp_Key->mot, "" ) != 0 ) {

    Trace("gp_Key->mot non vide") ;

    ASTRE_FORMATE_DONNEES_AFFICHAGE(gp_Ast);

    If_Mot_Is("o")     { gp_Sui->menu = MENU_MANUEL_BRUT ;i=1; }
    If_Mot_Is("e" )    { gp_Sui->menu = MENU_MANUEL_BRUT ; i=1;}
    If_Mot_Is("s")     { gp_Sui->menu = MENU_MANUEL_BRUT ; i=1;}
    If_Mot_Is("n")     { gp_Sui->menu = MENU_MANUEL_BRUT ;i=1; }
    If_Mot_Is("reset") { gp_Sui->menu = MENU_MANUEL_BRUT ;i=1; }
    If_Mot_Is("stop")  { gp_Sui->SUIVI_VOUTE = 0 ; i=1;}
    If_Mot_Is("play")  { gp_Sui->SUIVI_VOUTE = 1 ; i=1;}
    
    /* Quelques actions d 'affichage a l'ecran  */

    If_Mot_Is("aff_variables")       { CONFIG_PARAMETRES_AFFICHER() ; i=1 ; } ;

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
      Trace("Mot_Is aff_infos") ;
      CONFIG_AFFICHER_TOUT() ; 
      gp_Lcd->display_informations ( 2000000 ) ;
      gp_Sui->menu = MENU_INFO ; 
      i=1;
      
    }     // mode info

    /*  touche mode equatorial */
    
    If_Mot_Is("key_equ")      { 
      gp_Lcd->display_str_str( 2000000, "Mode equatorial", (char*)gc_const_menus[ MENU_EQUATORIAL ] ) ;
      gp_Sui->menu = MENU_EQUATORIAL ; 
      i=1;
    }    

    /*  touche mode azimutal */

    If_Mot_Is("key_azi")      { 
      gp_Lcd->display_str_str( 2000000, "Mode azimutal", (char*)gc_const_menus[ MENU_AZIMUTAL ] ) ;
      gp_Sui->menu = MENU_AZIMUTAL ; 
      i=1;
    }       // mode azimutal

    /* touche POWER : arret su systeme */

    If_Mot_Is("key_power")     { 
      gp_Lcd->display_str_str( 2000000, "Mode azimutal", (char*)gc_const_menus[ MENU_AZIMUTAL ] ) ;
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

    if (i) strcpy(gp_Key->mot,"") ; 
  }
  else {
    Trace("gp_Key->mot vide") ;
  }
  //-----------------------------------------------------------------
  // Si un  mot est lu sur le clavier (appui sur menu->valider) necessaire)
  // on effectue une action
  // Si autre chose qu'un menu est lu sur le clavier , on peut aussi changer
  // de menu (pour plus de rapidite)
  //-----------------------------------------------------------------

  if ( gp_Key->phrase_lue ) {
    
    //---------------------------------
    // Si un MENU est entrer au clavier
    // => gestion des MENUS en fonction du clavier
    // ou de toute autre entree exterieure
    // autre qu'un switch electronique
    //---------------------------------
    
    Trace("symbole = %s nombre = %s\n", gp_Key->symbole, gp_Key->nombre ) ;
    
    //--------------------------------------------------------------------
    // ON CHANGE DE MENU VOLONTAIREMENT
    //--------------------------------------------------------------------
    
    if ( ! strcmp( gp_Key->symbole, gp_Key->menu )) {
      gp_Sui->menu_old = gp_Sui->menu ;
      gp_Sui->menu     = atoi( gp_Key->nombre ) ; 
    }
    
    //---------------------------------------------------------------------------------------------------------
    // Si un nouvel as a ete pris en compte
    //---------------------------------------------------------------------------------------------------------
    
    sprintf( s_buffer4, "%s%s%s%s", CONFIG_MES, CONFIG_NGC, CONFIG_ETO, CONFIG_PLA) ;

    if ( strstr( s_buffer4, gp_Key->symbole ) != NULL ) {

      memset( gp_Ast->nom, CALCUL_ZERO_CHAR, sizeof(gp_Ast->nom)) ;
      sprintf( gp_Ast->nom, "%s%s", gp_Key->symbole, gp_Key->nombre) ;
    
      Trace1("== %s ==",gp_Ast->nom) ;
      
      /* Recherche de l'as dans les catalgues */
      
      if ( strstr( gp_Ast->nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_dec) ;
      if ( strstr( gp_Ast->nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_dec) ;
      if ( strstr( gp_Ast->nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_eto_dec) ;
      
      /* Pour les planetes, le calcul est fait dans SOLAR_SYSTEM dans CALCUL_TOUT */

      gp_Sui->SUIVI_ALIGNEMENT = 1 ;
      //gp_Sui->menu = MENU_AZIMUTAL ;

      GPIO_CLIGNOTE(gp_Gpi_Par_Pwm->par_led_etat, 1, 10) ; 
    }
    //---------------------------------------------------------------------------------------------------------
    // Si une DEMANDE de MISE A JOUR relative au temps (sauf mois et jour)
    //---------------------------------------------------------------------------------------------------------
    
    if(  ! strcmp( gp_Key->symbole, "TIME" ) )  {

      GPIO_CLIGNOTE(gp_Gpi_Par_Pwm->par_led_etat, strlen( gp_Key->nombre ), 10) ;

      switch ( strlen( gp_Key->nombre ) ) {

        case 2 : // Si longueur = 2 cela ne peut etre qu'un changement de mois
          
          Trace1("demande changement heure minutes : %s" , gp_Key->nombre) ;

          /* CONFIG_SET_MONTH( gp_Key->nombre ) ; */

        case 4 : // Si longueur = 4 cela ne peut etre qu'un changement d'heure et de minutes
          
          Trace1("demande changement heure minutes : %s" , gp_Key->nombre) ;
    
          TEMPS_SET_HOUR_AND_MINUTES( gp_Key->nombre ) ;
          
        break ;
          
          // FIXME : 20190822 : modification configuration de la date

        case 8 : // Si longueur = 5 cela est un changement de mois jour avec verif de l'annee

          Trace1("demande changement annee : %s" , gp_Key->nombre) ;
          
          TEMPS_SET_YEAR_MONTH_AND_DAY( gp_Key->nombre ) ;
      
          break ;
      }
    }
    //---------------------------------------------------------------------------------------------------------
    // Si une DEMANDE de SETUP a lieu 
    // RAPPEL : l'annee par defaut est declaree en DUR dans le CODE de telle sorte que le code ne soit valable
    // que pour annee en COURS
    //---------------------------------------------------------------------------------------------------------
    
    if(  ! strcmp( gp_Key->symbole, "SETUP" ) )  {
      switch ( strlen( gp_Key->nombre ) ) {
        
        case 0 : break ; // a completer
        case 1 : break ; // a completer 

        case 2 : // TODO : exemple d'une demande de capteur : a modifier / completer
          TEMPS_CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;

          if ( gp_Dev->use_capteurs ) { 
            gp_Ast->a = gp_Sui->pitch ;
            gp_Ast->h = gp_Sui->heading ; 
          }
          CALCUL_EQUATEUR ( gp_Lie, gp_Ast) ;

          gp_Sui->menu_old = gp_Sui->menu ;
          gp_Sui->menu = MENU_AZIMUTAL ;
          break ;
      }
      //---------------------------------------------------------------------------------------------------------
    }
    gp_Key->phrase_lue=0 ;
    strcpy( gp_Key->mot, "" ) ;
    strcpy( gp_Key->nombre, "" ) ;
  }
  Trace("end") ;
}
/*****************************************************************************************
* @fn     : SUIVI_MANUEL_BRUT
* @author : s.gravois
* @brief  : Ce mode permet le centrage / recentrage de l'objet tout en ayant le suivi.
* @param  : STRUCT_SUIVI   *gp_Sui
* @param  : STRUCT_KEYS *gp_Key 
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-31 correction code dans flag_nord_sud_est_ouest : les accelrations sont les pas
* @todo   : 
*****************************************************************************************/
/* FIXME :
  SUIVI_MANUEL_BRUT : 
    le suivi etant effectue sur un as (calcul des vitesses et periodes par suivi_voute)
    l'appui sur les touches N-S-E-O provoque une suspension de suivi_voute jusqua touche OK
    avec une multiplication des vitesses N-S-E-O par un facteur gp_Cal_Par->par_alt_acc
    Ce mode permet le centrage / recentrage de l'objet tout en ayant le suivi.
*/

void SUIVI_MANUEL_BRUT(STRUCT_SUIVI * gp_Sui, STRUCT_KEYS *gp_Key) {
  
  int flag_nord_sud_est_ouest = 0 ;
  int flag_calcul = 0 ;

  ARBO(__func__,0,"start") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  Trace("start") ;

  /* GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, gp_Sui) ; */

  SUIVI_MAJ_PAS( gp_Sui) ;

  if( strcmp( gp_Sui->sui_dat.dat_inf, "") != 0 ) {
    Trace("0 gp_Sui->sui_dat.dat_inf = %s", gp_Sui->sui_dat.dat_inf ) ;
  }
  else {
    Trace("dat_inf vide" ) ;
  }
  // -----------------------------------------------------------
  // reset = remise a zero des compteurs
  //------------------------------------------------------------
  
  if ( gp_Sui->reset ) {

    pthread_mutex_lock(& gp_Mut->mut_azi );
    pthread_mutex_lock(& gp_Mut->mut_azi );

    if ( gp_Sui->Sh != 0 ) gp_Sui->pas_alt = gp_Sui->Sh ; else gp_Sui->pas_alt = 1 ;
    if ( gp_Sui->Sa != 0 ) gp_Sui->pas_azi = gp_Sui->Sa ; else gp_Sui->pas_azi = 1 ;

    gp_Sui->acc_azi = 1 ;

    if ( gp_Sui->SUIVI_EQUATORIAL )  gp_Sui->acc_alt = 0.0 ;
    else                             gp_Sui->acc_alt = 1.0 ;

    gp_Sui->reset = 0 ;

    TRACE("%-10s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "reset.." , \
           gp_Sui->Sh , gp_Sui->Sa, gp_Sui->pas_azi, gp_Sui->pas_alt, gp_Sui->acc_azi , gp_Sui->acc_alt) ;
           
    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_azi );
    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_alt );

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
    pthread_mutex_lock(& gp_Mut->mut_azi );
    pthread_mutex_lock(& gp_Mut->mut_alt );

    // On utilise les memes touches que dans SUIVI_MANUEL_ASSERVI

    if ( gp_Sui->pas_nord )  gp_Sui->pas_alt++ ; if ( gp_Sui->pas_alt == 0 ) gp_Sui->pas_alt = 1 ;
    if ( gp_Sui->pas_sud )   gp_Sui->pas_alt-- ; if ( gp_Sui->pas_alt == 0 ) gp_Sui->pas_alt = -1 ;
    if ( gp_Sui->pas_ouest ) gp_Sui->pas_azi++;  if ( gp_Sui->pas_azi == 0 ) gp_Sui->pas_azi = 1 ;
    if ( gp_Sui->pas_est )   gp_Sui->pas_azi-- ; if ( gp_Sui->pas_azi == 0 ) gp_Sui->pas_azi = -1 ;

    if ( gp_Sui->pas_nord  || gp_Sui->pas_sud ) {
      /*
      if ( abs(gp_Sui->pas_alt) <= 2) gp_Sui->acc_alt = gp_Sui->pas_alt  ;
      else                           gp_Sui->acc_alt = gp_Sui->pas_alt * gp_Cal_Par->par_alt_acc ;
      */ /* correction mai 2022 */

      gp_Sui->acc_alt = gp_Sui->pas_alt  ;

    }
    if ( gp_Sui->pas_ouest || gp_Sui->pas_est ) {
      /*
      if ( abs(gp_Sui->pas_azi) <= 2) gp_Sui->acc_azi = gp_Sui->pas_azi ;
      else                           gp_Sui->acc_azi = gp_Sui->pas_azi * gp_Cal_Par->par_azi_acc ;
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

    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_azi );
    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_alt );

    flag_calcul = 1 ;
  }
  // ======================================================
  // Gestion des accelerations ou decelerations tres lentes
  // (reglage du suivi precis)
  // ======================================================

  if ( gp_Sui->pas_forward ) {

    pthread_mutex_lock(& gp_Mut->mut_azi );
    pthread_mutex_lock(& gp_Mut->mut_alt );

    gp_Sui->acc_azi *= gp_Cal_Par->par_altaz_slow_forward ;
    gp_Sui->acc_alt *= gp_Cal_Par->par_altaz_slow_forward ; 

    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_azi );
    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_alt );
/*
    TRACE("acc*gp_Cal_Par->par_altaz_slow_forward %.4f acc_azi %.4f acc_alt %.4f", gp_Cal_Par->par_altaz_slow_forward, gp_Sui->acc_azi, gp_Sui->acc_alt ) ;
*/
    TRACE("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "forward.." , \
           gp_Sui->Sh , gp_Sui->Sa, gp_Sui->pas_azi, gp_Sui->pas_alt, gp_Sui->acc_azi , gp_Sui->acc_alt) ;

    gp_Sui->pas_forward = 0 ;
    flag_calcul = 1 ;
  }
  if ( gp_Sui->pas_rewind ) {

    pthread_mutex_lock(& gp_Mut->mut_azi );
    pthread_mutex_lock(& gp_Mut->mut_alt );

    gp_Sui->acc_azi /= gp_Cal_Par->par_altaz_slow_rewind ;
    gp_Sui->acc_alt /= gp_Cal_Par->par_altaz_slow_rewind ; 

    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_azi );
    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_alt );
/*
    TRACE("acc/gp_Cal_Par->par_altaz_slow_rewind %.4f acc_azi %.4f acc_alt %.4f ", gp_Cal_Par->par_altaz_slow_rewind, gp_Sui->acc_azi, gp_Sui->acc_alt ) ;
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

    pthread_mutex_lock(& gp_Mut->mut_azi );
    pthread_mutex_lock(& gp_Mut->mut_alt );

    gp_Sui->acc_azi *= gp_Cal_Par->par_altaz_fast_forward ;          
    gp_Sui->acc_alt *= gp_Cal_Par->par_altaz_fast_forward ; 

    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_azi );
    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_alt );

/*
    TRACE("acc*gp_Cal_Par->par_altaz_fast_forward %.4f acc_azi %.4f acc_alt %.4f", gp_Cal_Par->par_altaz_fast_forward, gp_Sui->acc_azi, gp_Sui->acc_alt ) ;
*/
    TRACE("%-15s : Sh %d Sa %d pas_azi = %ld pas_alt = %ld acc_azi = %f acc_alt = %f", "forward fast.." , \
           gp_Sui->Sh , gp_Sui->Sa, gp_Sui->pas_azi, gp_Sui->pas_alt, gp_Sui->acc_azi , gp_Sui->acc_alt) ;

    gp_Sui->pas_forward_fast = 0 ;
    flag_calcul = 1 ;
  }
  if ( gp_Sui->pas_rewind_fast ) {

    pthread_mutex_lock(& gp_Mut->mut_azi );   
    pthread_mutex_lock(& gp_Mut->mut_alt );

    gp_Sui->acc_azi /= gp_Cal_Par->par_altaz_fast_rewind ;
    gp_Sui->acc_alt /= gp_Cal_Par->par_altaz_fast_rewind ; 

    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_azi );
    Trace(""); pthread_mutex_unlock(& gp_Mut->mut_alt );

/*
    TRACE("acc/gp_Cal_Par->par_altaz_fast_rewind  %.4f acc_azi %.4f acc_alt %.4f ", gp_Cal_Par->par_altaz_fast_rewind, gp_Sui->acc_azi, gp_Sui->acc_alt ) ;
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
  
    Trace("flag_calcul") ;

    pthread_mutex_lock( & gp_Mut->mut_cal );
    
    CALCUL_VITESSES( gp_Lie, gp_Ast, gp_Sui) ;
    CALCUL_PERIODE ( gp_Ast, gp_Sui, gp_Vou) ;
    
    Trace(""); pthread_mutex_unlock( & gp_Mut->mut_cal );
  }
}

//==========================================================
// MODE MANUEL : le suivi d'effectue directement en appuyant sur les touches 
//==========================================================

void SUIVI_MANUEL_ASSERVI(STRUCT_SUIVI * gp_Sui, STRUCT_KEYS *gp_Key) {
    
  double  tempo_raq_alt ;
  double  tempo_raq_azi ;
  struct timeval t00 ;
  struct timeval t01 ;
  int azi,alt ;

  ARBO(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  gp_Sui->pas_azi_old = gp_Sui->pas_azi ;
  gp_Sui->pas_alt_old = gp_Sui->pas_alt ;
  
  gettimeofday(&t00,NULL) ;
  
  azi = 0 ;
  alt = 0 ;
  
  /* GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, gp_Sui) ; */
  SUIVI_MAJ_PAS  ( gp_Sui) ;
  
  // La determination de tempo_raq est tres importante
  // Elle varie suivant la reduction total du moteur
  /* TODO : a expliquer */
  tempo_raq_alt = (double)gp_Tim_Par->par_tempo_Raq * 1000000 / gp_Cal_Par->par_alt_red_tot ;
  tempo_raq_azi = (double)gp_Tim_Par->par_tempo_Raq * 1000000 / gp_Cal_Par->par_azi_red_tot ;
  
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
      pthread_mutex_lock(& gp_Mut->mut_azi );
      azi=1 ;
    }
    if ( ! alt )  if ( gp_Sui->pas_nord  || gp_Sui->pas_sud || gp_Sui->pas_acc_moins || gp_Sui->pas_acc_plus ) { 
      pthread_mutex_lock(& gp_Mut->mut_alt ); 
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

    /* GPIO_CLAVIER_MATRICIEL_MAJ_SUIVI_PAS( gpio_key_l, gpio_key_c, gp_Sui) ; */
    SUIVI_MAJ_PAS( gp_Sui) ;
  }
  // =================================================================
  // FIN DE LA BOUCLE : TANT QUE J'APPUIE SUR UN BOUTON DE LA RAQUETTE
  // =================================================================

  if ( azi ) { Trace(""); pthread_mutex_unlock( & gp_Mut->mut_azi ); }
  if ( alt ) { Trace(""); pthread_mutex_unlock( & gp_Mut->mut_alt ); }
  
  gettimeofday(&t01,NULL) ;

  gp_Sui->t_diff     = (( t01.tv_sec - t00.tv_sec ) * TEMPS_MICRO_SEC) + t01.tv_usec - t00.tv_usec ;
  gp_Sui->t_diff_sec = (double)gp_Sui->t_diff / (double)TEMPS_MICRO_SEC ;
	 
  if ( gp_Sui->pas_azi_old != gp_Sui->pas_azi ) gp_Sui->d_appui_raq_azi += gp_Sui->t_diff_sec ;
  if ( gp_Sui->pas_alt_old != gp_Sui->pas_alt ) gp_Sui->d_appui_raq_alt += gp_Sui->t_diff_sec ;
	 
  if ( gp_Sui->pas_azi_old != gp_Sui->pas_azi || gp_Sui->pas_alt_old != gp_Sui->pas_alt ) {

   TRACE(" : Va = %2.4f Vh = %2.4f Ta_mic = %2.4f Th_mic = %2.4f Fa_mic = %2.4f Fh_mic = %2.4f Fam = %ld Fhm = %ld",\
        gp_Ast->Va,gp_Ast->Vh,gp_Sui->Ta_mic,gp_Sui->Th_mic,gp_Sui->Fa_mic,gp_Sui->Fh_mic, \
	(gp_Sui->Ia - gp_Sui->Ia_prec),(gp_Sui->Ih - gp_Sui->Ih_prec )) ;  
  }

}



/*****************************************************************************************
* @fn     : SUIVI_INIT
* @author : s.gravois
* @brief  : Cette fonction configure la structure STRUCT_SUIVI * gp_Sui
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-20 passage d une partie de code dans DEVICES_INIT
* @date   : 2022-05-24 ajout mutex_thread suivi pour proteger section crotique threads
* @date   : 2022-06-17 ajout gp_Pid_Par->par_pid_ech gp_Pid_Par->par_pid_kp gp_Pid_Par->par_pid_ki gp_Pid_Par->par_pid_kd 
* @todo   : voir si les mutex peuvent etre dispatches / eclates dans les autres struct
*****************************************************************************************/

void SUIVI_INIT(STRUCT_SUIVI * gp_Sui) {
  
  int i ;
  
  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  gp_Sui->SUIVI_MANUEL     = 0 ;
  gp_Sui->SUIVI_ALIGNEMENT = 0 ;
  gp_Sui->SUIVI_GOTO       = 0 ;
  gp_Sui->SUIVI_VOUTE      = 1 ;
  gp_Sui->SUIVI_EQUATORIAL = 0 ;

  // a modifier  : instancier ces variables a l aide du fichier de config

  gp_Sui->reset = 0 ;
  
  gp_Sui->pas_azi        = 1 ;
  gp_Sui->pas_alt        = 1 ;

  gp_Sui->pas_acc_plus   = 0 ;
  gp_Sui->pas_acc_moins  = 0 ;
  gp_Sui->pas_azi_old    = 0 ;
  gp_Sui->pas_alt_old    = 0 ;

  gp_Sui->pas_est        = 0 ;
  gp_Sui->pas_ouest      = 0 ; 
  gp_Sui->pas_nord       = 0 ;
  gp_Sui->pas_sud        = 0 ;

  gp_Sui->pas_forward       = 0 ;
  gp_Sui->pas_rewind        = 0 ;
  gp_Sui->pas_forward_fast  = 0 ;
  gp_Sui->pas_rewind_fast   = 0 ;

  /* Initialisation des accelerations */

  gp_Sui->acc_azi       = 1.0 ; // acceleration volontaire des vitesses brutes
  gp_Sui->acc_alt       = 1.0 ; // acceleration volontaire des vitesses brutes
  gp_Sui->acc_azi_pid   = 1.0 ; // acceleration PID par retour boucle des vitesses brutes
  gp_Sui->acc_alt_pid   = 1.0 ; // acceleration PID par retour boucle des vitesses brutes

  gp_Sui->sgn_azi   = 1 ;
  gp_Sui->sgn_alt   = 1 ;

  gp_Sui->t_diff = 0 ;
  gp_Sui->t_diff_sec = 0 ;
  gp_Sui->d_temps          = 0 ;   
  gp_Sui->d_appui_raq_azi  = 0 ;
  gp_Sui->d_appui_raq_alt  = 0 ;
  
  gp_Sui->Fa_mic        = 30 ;
  gp_Sui->Fh_mic        = 30 ;

  gp_Sui->Ta_mic        = 1 / gp_Sui->Fa_mic ;
  gp_Sui->Th_mic        = 1 / gp_Sui->Fh_mic ;
  
  gp_Sui->Tac        = 1.0 ;
  gp_Sui->Thc        = 1.0 ;
  
  gp_Sui->Tacc = 0.97 ;
  gp_Sui->Thcc = 0.97 ;
  
  gp_Sui->Ia = 0 ;
  gp_Sui->Ih = 0 ;
  
  gp_Sui->Ias = 0 ;
  gp_Sui->Ihs = 0 ;
  
  gp_Sui->Ia_prec = 0 ;
  gp_Sui->Ih_prec = 0 ;
  
  for(i=0;i<STATS_ASS;i++) gp_Sui->Iat[i] = 0 ;
  for(i=0;i<STATS_ASS;i++) gp_Sui->Iht[i] = 0 ;
  
  //gp_Sui->plus      =  1.02 ;
  //gp_Sui->moins     =  1.0 / gp_Sui->plus ;   
  
  gp_Sui->l_NANO_MOINS     = 0  ;     // a retirer sur les temporisations pour les tests
   
  gp_Sui->pas_asc = 0 ;
  gp_Sui->pas_dec = 0  ;
    
  gp_Sui->Sa = 0 ;       // signe de la vitesse (direction), tenir compte du flag FLAG_SENS_ALT
  gp_Sui->Sh = 0 ;       // signe de la vitesse (direction), tenir compte du flag FLAG_SENS_AZI
  gp_Sui->Sa_old = 0 ;   // flag de comparaison pour raffraichir ou non les ecritures
  gp_Sui->Sh_old = 0 ;   // flag de comparaison pour raffraichir ou non les ecritures
  
  gp_Sui->Da = 0  ;      // nombre a injecter dans les diviseurs de frequence
  gp_Sui->Dh = 0 ;       // nombre a injecter dans les diviseurs de frequence
  
  gp_Sui->menu = gp_Con_Par->par_default_menu  ;    // menu par defaut
  gp_Sui->menu_old = gp_Con_Par->par_default_menu  ;

  gp_Sui->alarme = 0 ;
  
  gp_Sui->pourcentage_tempo = 0.99 ; 
  
  gp_Sui->temporisation_menu     = gp_Tim_Par->par_tempo_Menu ;
  gp_Sui->temporisation_raq      = gp_Tim_Par->par_tempo_Raq ;
  gp_Sui->temporisation_ir       = gp_Tim_Par->par_tempo_Ir ;  
  gp_Sui->temporisation_termios  = gp_Tim_Par->par_tempo_Termios ;
  gp_Sui->temporisation_clavier  = gp_Tim_Par->par_tempo_Clavier ; 
  gp_Sui->temporisation_capteurs = gp_Tim_Par->par_tempo_Capteurs ;
  gp_Sui->temporisation_lcd_loop = gp_Tim_Par->par_tempo_Lcd_Loop ;
  gp_Sui->temporisation_lcd_disp = gp_Tim_Par->par_tempo_Lcd_Disp ;

  gp_Sui->temporisation_pid_loop = gp_Pid_Par->par_pid_ech ;
  gp_Sui->temporisation_voute    = gp_Vou->DT ;

  gp_Sui->DTh = gp_Sui->Th_mic * TEMPS_MICRO_SEC ;
  gp_Sui->DTa = gp_Sui->Ta_mic * TEMPS_MICRO_SEC ;

  gettimeofday(&gp_Sui->tval,NULL) ;
}