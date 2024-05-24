/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 07/10/2022  | * creation
# -------------------------------------------------------------- 
*/

#include "astro_keys.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/* Static HERE is entended as private method for struct STRUCT_KEYS */
/* These are pointer fct used as private methods inside STRUCT_KEYS */

static void KEYS_LOG            ( STRUCT_KEYS * ) ;
static void KEYS_DISPLAY        ( STRUCT_KEYS * ) ;
static void KEYS_DISPLAY_PREPARE ( STRUCT_KEYS * ) ;
static void KEYS_LOCK           ( STRUCT_KEYS * )  ;
static void KEYS_UNLOCK         ( STRUCT_KEYS * ) ;

/*****************************************************************************************
* @fn     : KEYS_LOG
* @author : s.gravois
* @brief  : Fonction qui gere les logs de la structure STRUCT_KEYS
* @param  : STRUCT_KEYS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void KEYS_LOG ( STRUCT_KEYS * lp_Key) {

  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,2,"keys log") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;
  ASTRO_GLOBAL_LOG_ROTATE( & lp_Key->key_loglevel ) ;
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex) ;

  sprintf( c_cmd , STR_KEY_FORMAT_0,\
    lp_Key->key_phrase,\
    lp_Key->key_mot,\
    lp_Key->key_symbole,\
    lp_Key->key_nombre,\
    lp_Key->key_premier,\
    lp_Key->key_valider,\
    lp_Key->key_menu, \
    lp_Key->key_mot_en_cours) ;

  MACRO_ASTRO_GLOBAL_LOG(lp_Key->key_loglevel,1,"%s", c_cmd) ;

  return ;
}

/*****************************************************************************************
* @fn     : KEYS_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_KEYS *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void KEYS_DISPLAY_PREPARE ( STRUCT_KEYS * lp_Key) {

  TraceArbo(__func__,2,"keys format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;

  sprintf( lp_Key->key_dis_cmd , STR_KEY_FORMAT_0,\
    lp_Key->key_phrase,\
    lp_Key->key_mot,\
    lp_Key->key_symbole,\
    lp_Key->key_nombre,\
    lp_Key->key_premier,\
    lp_Key->key_valider,\
    lp_Key->key_menu, \
    lp_Key->key_mot_en_cours) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex ) ;

  return ;
}

/*****************************************************************************************
* @fn     : KEYS_DISPLAY
* @author : s.gravois
* @brief  : Fonction qui gere les logs de la structure STRUCT_KEYS
* @param  : STRUCT_KEYS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void KEYS_DISPLAY ( STRUCT_KEYS * lp_Key) {

  TraceArbo(__func__,2,"keys display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
/*
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;
  ASTRO_GLOBAL_LOG_ROTATE(  & lp_Key->key_loglevel ) ;
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex) ;
*/
  KEYS_DISPLAY_PREPARE       ( lp_Key ) ;
  ASTRO_GLOBAL_LOG_ON ( &  lp_Key->key_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Key->key_loglevel , 1 , "%s" , lp_Key->key_dis_cmd ) ;
  ASTRO_GLOBAL_LOG_OFF( &  lp_Key->key_loglevel ) ;

  return ;
}
/*****************************************************************************************
* @fn     : KEYS_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_KEYS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void KEYS_LOCK ( STRUCT_KEYS * lp_Key) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : KEYS_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_KEYS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void KEYS_UNLOCK ( STRUCT_KEYS * lp_Key) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex ) ;

  return ;
}

/*****************************************************************************************
* @fn     : KEYS_INPUTS_GESTION_APPUIS
* @author : s.gravois
* @brief  : Cette fonction gere les appuis sur les "touches" (clavier, ir, ..)
* @brief  : et en deduit le contenu de la structure clavier (mot, action, etc..)
* @param  : STRUCT_KEYS  * lp_Key
* @date   : 2022-03-22 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
* @date   : 2022-04-12 protection zone de code dat_inf
*****************************************************************************************/

void KEYS_INPUTS_GESTION_APPUIS(STRUCT_KEYS *lp_Key) {

  int  i = 0 ;
  char c_mot[255]={0} ;
  char s_buffer[CONFIG_TAILLE_BUFFER_32]={0} ;

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  DATAS_ACTION_DAT_TO_KEY  ( gp_Dat, lp_Key ) ;
  DATAS_ACTION_RESET       ( gp_Dat );

  // =======================================================================
  // Quand la touche est relacheee, on traite
  // ensuite appui en cours est remis a ZERO pour qu'on ne passe qu'une fois
  // dans cette partie de code
  // =======================================================================
  
  if ( lp_Key->key_mot_en_cours && lp_Key->key_appui_en_cours == 0 ) {
  
    Trace1("mot trouver = %s", lp_Key->key_mot ) ;
		
		GPIO_LED_ETAT_CLIGNOTE(1, 5) ;

    //------------------------------------------------------------
    // On incremente la phrase avec le mot et
    // On incremente le nombre avec le mot si premier n'est pas vide
    //------------------------------------------------------------
    
    if ( strcmp( lp_Key->key_mot, lp_Key->key_valider) != 0 ) { 
      
      Trace1("Si mot != valider : on incremente la phrase avec le mot !!\n" ) ;

      if ( strlen(lp_Key->key_phrase) + strlen(lp_Key->key_mot) < CONFIG_TAILLE_BUFFER_32) {

        Trace1("on incremente la phrase %s avec le mot %s\n",lp_Key->key_phrase,  lp_Key->key_mot ) ;
        // correction bug 28/12/2017 : recopie de la chaine dans un buffer avant le sprintf

        memset( s_buffer, CONFIG_ZERO_CHAR, sizeof( s_buffer )) ;
        strcpy( s_buffer, lp_Key->key_phrase ) ;

        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;

        sprintf(lp_Key->key_phrase,"%s%s",s_buffer, lp_Key->key_mot);

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex ) ;
      }
      Trace1("resultat => phrase = %s \n",lp_Key->key_phrase ) ;
      
      Trace1("Si mot != valider et premier non vide => on met nombre + mot dans le nombre !!\n" ) ;

      if ( strcmp( lp_Key->key_premier, "")) {
        
	      if ( strlen(lp_Key->key_nombre) + strlen(lp_Key->key_mot) < CONFIG_TAILLE_BUFFER_32) {

          Trace1("on incremente le nombre %s avec le mot %s\n",lp_Key->key_nombre,  lp_Key->key_mot ) ;
          // correction bug 28/12/2017 : recopie de la chaine dans un buffer avant le sprintf

          memset( s_buffer, CONFIG_ZERO_CHAR, sizeof( s_buffer )) ;
          strcpy( s_buffer, lp_Key->key_nombre ) ;

          HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;

          sprintf(lp_Key->key_nombre,"%s%s", s_buffer, lp_Key->key_mot);

          HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex ) ;
        }
      }
      Trace("resultat => nombre = %s \n",lp_Key->key_nombre ) ;
    }    
    //------------------------------------------------------------
    // On met le mot dans premier si il est vide 
    //------------------------------------------------------------
    
    if ( ! strcmp( lp_Key->key_premier, "")){ 
      if ( strlen(lp_Key->key_mot) < CONFIG_TAILLE_BUFFER_32) {

        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;

        strcpy( lp_Key->key_premier, lp_Key->key_mot);

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex ) ;
      }
    }
    //------------------------------------------------------------
    // Si le mot en cours est une VALIDATION 
    // Exemple : valider
    // Alors on VALIDE la phrase en cours
    //  - en mettant lp_Key->phrase_lu à 1
    // on efface tout sauf SYMBOLE et NOMBRE qui sont determiner plus tot
    //------------------------------------------------------------

    for( i=0 ; i < KEYS_VALIDATIONS_SIZE ; i++ ) {

      if ( ! strcmp( lp_Key->key_mot,    lp_Key->key_valider )  ) {

        Trace1("Appui sur valider => on met premier dans symbole, phrase dans nombre, et NULL dans phrase et mot, phrase_lue a 1" ) ; 

        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;

        strcpy(lp_Key->key_premier,"") ;
        strcpy(lp_Key->key_phrase,"")  ;
        strcpy(lp_Key->key_mot,"") ;
        lp_Key->key_phrase_lue=1 ;

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex ) ;
      }
    }
    //------------------------------------------------------------
    // Si le mot est une ACTION, on efface la phrase en cours    
    // on met le mot dans PREMIER (premier mot de la phrase) 
    // on met le mot dans SYMBOLE (symbole utilise dans prog main) 
    //------------------------------------------------------------
    
    for( i=0 ; i < KEYS_ACTIONS_SIZE ; i++ ) {

      if ( ! strcmp( lp_Key->key_mot, lp_Key->key_actions[i] )) {
          
          MACRO_COD_TRC(lp_Key->key_mot) ;

          Trace("Si le mot est une ACTION, alors on efface la phrase en cours et on met mot dans premier et symbole") ;

          HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;

          strcpy(lp_Key->key_premier,lp_Key->key_mot) ;
          strcpy(lp_Key->key_symbole,lp_Key->key_mot)  ;

          strcpy(lp_Key->key_nombre,"")  ;
          strcpy(lp_Key->key_phrase,"")  ;
          strcpy(lp_Key->key_mot,"") ;

          lp_Key->key_phrase_lue=0 ;

          HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex ) ;

          lp_Key->key_display( lp_Key ) ; 
      }
    }
    
    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;

    lp_Key->key_mot_en_cours = 0 ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex ) ;
    // lp_Key->key_appui_en_cours = 0 ;
  }

  Trace1("lp_Key->key_mot = %s", lp_Key->key_mot ) ;

  return ;
}

/*****************************************************************************************
* @fn     : KEYS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_KEYS * as
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-20 ajout membres de la structure dans la fonction
* @todo   : supprimer STRUCT_ASTRE *gp_Ast (var blog) et remplacer par void
*           passer par des pointeurs de fonctions dans un structure
*****************************************************************************************/

void KEYS_INIT(STRUCT_KEYS * lp_Key) {

  int i ;
  
  TraceArbo(__func__,0,"init keys") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Key->key_mutex ) ;
                                     lp_Key->key_log      = KEYS_LOG ;
                                     lp_Key->key_lock     = KEYS_LOCK ;
                                     lp_Key->key_unlock   = KEYS_UNLOCK ;
                                     lp_Key->key_display  = KEYS_DISPLAY ;
                                     lp_Key->key_reset    = KEYS_RESET ;
                                     lp_Key->key_loglevel = 0 ; 
                                     lp_Key->key_file     = NULL ;
  gettimeofday (                   & lp_Key->key_tval, NULL ) ;

  memset( lp_Key->key_phrase,  CONFIG_ZERO_CHAR, sizeof( lp_Key->key_phrase ) );
  memset( lp_Key->key_valider, CONFIG_ZERO_CHAR, sizeof( lp_Key->key_valider ) );
  memset( lp_Key->key_symbole, CONFIG_ZERO_CHAR, sizeof( lp_Key->key_symbole ) );
  memset( lp_Key->key_menu,    CONFIG_ZERO_CHAR, sizeof( lp_Key->key_menu ) );
  memset( lp_Key->key_premier, CONFIG_ZERO_CHAR, sizeof( lp_Key->key_premier ) );
  memset( lp_Key->key_nombre,  CONFIG_ZERO_CHAR, sizeof( lp_Key->key_nombre ) );
  memset( lp_Key->key_mot,     CONFIG_ZERO_CHAR, sizeof( lp_Key->key_mot ) );
  
  strcpy( lp_Key->key_valider, "valider" ) ;
  strcpy( lp_Key->key_menu,    "MENU" ) ;
  
  strcpy( lp_Key->key_symbole,  "" ) ;
  strcpy( lp_Key->key_phrase,   "" ) ;
  strcpy( lp_Key->key_premier,  "" ) ;
  strcpy( lp_Key->key_nombre,   "" ) ;
  strcpy( lp_Key->key_mot,      "" ) ;

  lp_Key->key_mot_en_cours   = 0 ;
  lp_Key->key_phrase_lue     = 0 ;
  lp_Key->key_appui_en_cours = 0 ;

  /* lp_Key->tempo_clavier = gp_Tim_Par->tim_par_tpo_Clavier ; */ 
 
  /* FIXME : definitions des actions : 

     Les actions servent a initier une valeur en particulier 
     en suivant l ordre suivant :

       * appui sur la touche action : definit l action   exemple TIME
       * lit PLUSIEURS touches pour ajouter a une phrase exemple 1204
       * la touche VALIDER permet de definir une nouvelle heure 
          I.E. 12h04 
  */

  //------------------------------------------------------------
  // Si le mot est une ACTION, on efface la phrase en cours    
  // on met le mot dans PREMIER (premier mot de la phrase) 
  // on met le mot dans SYMBOLE (symbole utilise dans prog main) 
  //------------------------------------------------------------
  
  for( i=0 ; i < KEYS_ACTIONS_SIZE ; i++ ) {
    memset( lp_Key->key_actions[i], CONFIG_ZERO_CHAR, sizeof( lp_Key->key_actions[i] ));
  }
  // for( i=0 ; i < KEYS_VALIDATIONS_SIZE ; i++ )  memset( lp_Key->key_validations[i], CONFIG_ZERO_CHAR, CONFIG_TAILLE_BUFFER_32);

  strcpy( lp_Key->key_actions[0], "MENU" ) ;
  strcpy( lp_Key->key_actions[1], "SETUP" ) ;
  strcpy( lp_Key->key_actions[2], "MES" ) ;
  strcpy( lp_Key->key_actions[3], "NGC" ) ;
  strcpy( lp_Key->key_actions[4], "ETO" ) ;
  strcpy( lp_Key->key_actions[5], "PLA" ) ;
  strcpy( lp_Key->key_actions[6], "TIME" ) ;
  strcpy( lp_Key->key_actions[7], "NET" ) ; /* ajout 2023 : network */
  strcpy( lp_Key->key_actions[8], "LON" ) ; /* ajout 2023 : longitude */
  strcpy( lp_Key->key_actions[9], "LAT" ) ; /* ajout 2023 : latitude */
  

  return ;
} 

/*****************************************************************************************
* @fn     : KEYS_RESET_MOT
* @author : s.gravois
* @brief  : Cette fonction reset proprement le mot en cours et le mot retenu
* @param  : STRUCT_KEYS *lp_Key
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void KEYS_RESET_MOT(STRUCT_KEYS *lp_Key) {
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;

  lp_Key->key_mot_en_cours = 0 ;

  strcpy( lp_Key->key_mot, "" ) ;
  strcpy( lp_Key->key_phrase, "" ) ;

  lp_Key->key_mot_en_cours   = 0 ;
  lp_Key->key_phrase_lue     = 0 ;
  lp_Key->key_appui_en_cours = 0 ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : KEYS_RESET
* @author : s.gravois
* @brief  : Cette fonction reset la structure STRUCT_KEYS
* @param  : STRUCT_KEYS *lp_Key
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

static void KEYS_RESET(STRUCT_KEYS *lp_Key) {
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Key->key_mutex ) ;

  strcpy( lp_Key->key_symbole,  "" ) ;
  strcpy( lp_Key->key_phrase,   "" ) ;
  strcpy( lp_Key->key_premier,  "" ) ;
  strcpy( lp_Key->key_nombre,   "" ) ;
  strcpy( lp_Key->key_mot,      "" ) ;

  lp_Key->key_mot_en_cours   = 0 ;
  lp_Key->key_phrase_lue     = 0 ;
  lp_Key->key_appui_en_cours = 0 ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex ) ;

  return ;
}