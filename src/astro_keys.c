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


/*****************************************************************************************
* @fn     : KEYS_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_KEYS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void KEYS_LOCK ( STRUCT_KEYS * lp_Key) {

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

void KEYS_UNLOCK ( STRUCT_KEYS * lp_Key) {

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

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  DATAS_ACTION_PUT  ( gp_Dat, lp_Key ) ;
  DATAS_ACTION_RESET( gp_Dat );

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

        sprintf(lp_Key->key_phrase,"%s%s",s_buffer, lp_Key->key_mot);
      }
      Trace1("resultat => phrase = %s \n",lp_Key->key_phrase ) ;
      
      Trace1("Si mot != valider et premier non vide => on met nombre + mot dans le nombre !!\n" ) ;

      if ( strcmp( lp_Key->key_premier, "")) {
        
	      if ( strlen(lp_Key->key_nombre) + strlen(lp_Key->key_mot) < CONFIG_TAILLE_BUFFER_32) {

          Trace1("on incremente le nombre %s avec le mot %s\n",lp_Key->key_nombre,  lp_Key->key_mot ) ;
          // correction bug 28/12/2017 : recopie de la chaine dans un buffer avant le sprintf

          memset( s_buffer, CONFIG_ZERO_CHAR, sizeof( s_buffer )) ;
          strcpy( s_buffer, lp_Key->key_nombre ) ;

          sprintf(lp_Key->key_nombre,"%s%s", s_buffer, lp_Key->key_mot);
        }
      }
      Trace1("resultat => nombre = %s \n",lp_Key->key_nombre ) ;
    }    
    //------------------------------------------------------------
    // On met le mot dans premier si il est vide 
    //------------------------------------------------------------
    
    if ( ! strcmp( lp_Key->key_premier, "")){ 
      if ( strlen(lp_Key->key_mot) < CONFIG_TAILLE_BUFFER_32)
      strcpy( lp_Key->key_premier, lp_Key->key_mot);
    }
    //------------------------------------------------------------
    // Si le mot en cours est une VALIDATION 
    // Exemple : valider
    // Alors on VALIDE la phrase en cours
    //  - en mettant lp_Key->phrase_lu à 1
    // on efface tout sauf SYMBOLE et NOMBRE qui sont determiner plus tot
    //------------------------------------------------------------

    for( i=0 ; i < KEYS_VALIDATIONS_SIZE ; i++ )
    if ( ! strcmp( lp_Key->key_mot,    lp_Key->key_valider )  ) {
      Trace1("Appui sur valider => on met premier dans symbole, phrase dans nombre, et NULL dans phrase et mot, phrase_lue a 1" ) ; 

      strcpy(lp_Key->key_premier,"") ;
      strcpy(lp_Key->key_phrase,"")  ;
      strcpy(lp_Key->key_mot,"") ;
      lp_Key->key_phrase_lue=1 ;
    }
    //------------------------------------------------------------
    // Si le mot est une ACTION, on efface la phrase en cours    
    // on met le mot dans PREMIER (premier mot de la phrase) 
    // on met le mot dans SYMBOLE (symbole utilise dans prog main) 
    //------------------------------------------------------------
    
    for( i=0 ; i < KEYS_ACTIONS_SIZE ; i++ )
    if ( ! strcmp( lp_Key->key_mot, lp_Key->key_actions[i] )) {
        Trace1("Si le mot est une ACTION, alors on efface la phrase en cours et on met mot dans premier et symbole") ;
        strcpy(lp_Key->key_premier,lp_Key->key_mot) ;
        strcpy(lp_Key->key_symbole,lp_Key->key_mot)  ;

        strcpy(lp_Key->key_nombre,"")  ;
        strcpy(lp_Key->key_phrase,"")  ;
        strcpy(lp_Key->key_mot,"") ;
        lp_Key->key_phrase_lue=0 ;
    }
    
    lp_Key->key_mot_en_cours = 0 ;
    lp_Key->key_appui_en_cours = 0 ;
  }
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

  Trace1("") ;

  memset( lp_Key->key_phrase,  CALCULS_ZERO_CHAR, sizeof( lp_Key->key_phrase ) );
  memset( lp_Key->key_valider, CALCULS_ZERO_CHAR, sizeof( lp_Key->key_valider ) );
  memset( lp_Key->key_symbole, CALCULS_ZERO_CHAR, sizeof( lp_Key->key_symbole ) );
  memset( lp_Key->key_menu,    CALCULS_ZERO_CHAR, sizeof( lp_Key->key_menu ) );
  memset( lp_Key->key_premier, CALCULS_ZERO_CHAR, sizeof( lp_Key->key_premier ) );
  memset( lp_Key->key_nombre,  CALCULS_ZERO_CHAR, sizeof( lp_Key->key_nombre ) );
  memset( lp_Key->key_mot,     CALCULS_ZERO_CHAR, sizeof( lp_Key->key_mot ) );
  
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
 
  // FIXME : definitions des actions : 
  // Les actions servent a 
  
  for( i=0 ; i < KEYS_ACTIONS_SIZE ; i++ ) {
    memset( lp_Key->key_actions[i], CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_32);
  }
  // for( i=0 ; i < KEYS_VALIDATIONS_SIZE ; i++ )  memset( lp_Key->key_validations[i], CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_32);

  strcpy( lp_Key->key_actions[0], "MENU" ) ;
  strcpy( lp_Key->key_actions[1], "SETUP" ) ;
  strcpy( lp_Key->key_actions[2], "MES" ) ;
  strcpy( lp_Key->key_actions[3], "NGC" ) ;
  strcpy( lp_Key->key_actions[4], "ETO" ) ;
  strcpy( lp_Key->key_actions[5], "PLA" ) ;
  strcpy( lp_Key->key_actions[6], "TIME" ) ;
   
} 

/*****************************************************************************************
* @fn     : KEYS_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations d entrees input ("lp_Key")
* @param  : STRUCT_KEYS *lp_Key
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void KEYS_DISPLAY(STRUCT_KEYS *lp_Key) {
  
  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  sprintf( c_cmd , "(phr) %-5s (mot) %-5s (sym) %-5s (nom) %-5s (pre) %-5s (c_mot) %-5s (menu) %-10s",\
    lp_Key->key_phrase,\
    lp_Key->key_mot,\
    lp_Key->key_symbole,\
    lp_Key->key_nombre,\
    lp_Key->key_premier,\
    lp_Key->key_valider,\
    lp_Key->key_menu) ;

  
  Trace1( "%s", c_cmd) ;

  Trace2("lp_Key->key_mot         = %s",lp_Key->key_mot) ;
  Trace2("lp_Key->key_premier     = %s",lp_Key->key_premier) ;
  Trace2("lp_Key->key_phrase      = %s",lp_Key->key_phrase) ;
  Trace2("lp_Key->key_nombre      = %s",lp_Key->key_nombre) ;
  Trace2("lp_Key->key_symbole     = %s",lp_Key->key_symbole) ;
  Trace2("lp_Key->key_phrase_lue  = %d",lp_Key->key_phrase_lue) ;

  TraceArbo(__func__,1,c_cmd) ; /* MACRO_DEBUG_ARBO_FONCTIONS */
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

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Key->key_mutex ) ;

  return ;

}