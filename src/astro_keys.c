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
* @fn     : KEYS_INPUTS_GESTION_APPUIS
* @author : s.gravois
* @brief  : Cette fonction gere les appuis sur les "touches" (clavier, ir, ..)
* @brief  : et en deduit le contenu de la structure clavier (mot, action, etc..)
* @param  : STRUCT_SUIVI * lp_Sui
* @param  : STRUCT_KEYS  * lp_Key
* @date   : 2022-03-22 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
* @date   : 2022-04-12 protection zone de code dat_inf
*****************************************************************************************/

void KEYS_INPUTS_GESTION_APPUIS(STRUCT_SUIVI * lp_Sui, STRUCT_KEYS *lp_Key) {

  int  i = 0 ;
  char val[255]={0} ;
  char s_buffer[CONFIG_TAILLE_BUFFER_32]={0} ;

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset(val, 0, sizeof(val)) ;     

  pthread_mutex_lock(& gp_Mut->mut_dat );
  strcpy( val, gp_Dat->dat_inf ) ;
  pthread_mutex_unlock(& gp_Mut->mut_dat );
  
  if ( strcmp( val, "") ) {
    
    //printf("val = %s\n", val ) ;
    strcpy( lp_Key->mot, val ) ; 
    lp_Key->appui_en_cours = 1 ;
    lp_Key->mot_en_cours = 1 ;    
  }
  else lp_Key->appui_en_cours = 0 ; 
  
  // =======================================================================
  // Quand la touche est relacheee, on traite
  // ensuite appui en cours est remis a ZERO pour qu'on ne passe qu'une fois
  // dans cette partie de code
  // =======================================================================
  
  if ( lp_Key->mot_en_cours && lp_Key->appui_en_cours == 0 ) {
  
    Trace1("mot trouver = %s", lp_Key->mot ) ;
		
		GPIO_CLIGNOTE(gp_Gpi_Par_Pwm->par_led_etat, 1, 5) ;

    //------------------------------------------------------------
    // On incremente la phrase avec le mot et
    // On incremente le nombre avec le mot si premier n'est pas vide
    //------------------------------------------------------------
    
    if ( strcmp( lp_Key->mot, lp_Key->valider) != 0 ) { 
      
      Trace1("Si mot != valider : on incremente la phrase avec le mot !!\n" ) ;

      if ( strlen(lp_Key->phrase) + strlen(lp_Key->mot) < CONFIG_TAILLE_BUFFER_32) {

        Trace1("on incremente la phrase %s avec le mot %s\n",lp_Key->phrase,  lp_Key->mot ) ;
        // correction bug 28/12/2017 : recopie de la chaine dans un buffer avant le sprintf

        memset( s_buffer, CONFIG_ZERO_CHAR, sizeof( s_buffer )) ;
        strcpy( s_buffer, lp_Key->phrase ) ;

        sprintf(lp_Key->phrase,"%s%s",s_buffer, lp_Key->mot);
      }
      Trace1("resultat => phrase = %s \n",lp_Key->phrase ) ;
      
      Trace1("Si mot != valider et premier non vide => on met nombre + mot dans le nombre !!\n" ) ;

      if ( strcmp( lp_Key->premier, "")) {
        
	      if ( strlen(lp_Key->nombre) + strlen(lp_Key->mot) < CONFIG_TAILLE_BUFFER_32) {

          Trace1("on incremente le nombre %s avec le mot %s\n",lp_Key->nombre,  lp_Key->mot ) ;
          // correction bug 28/12/2017 : recopie de la chaine dans un buffer avant le sprintf

          memset( s_buffer, CONFIG_ZERO_CHAR, sizeof( s_buffer )) ;
          strcpy( s_buffer, lp_Key->nombre ) ;

          sprintf(lp_Key->nombre,"%s%s", s_buffer, lp_Key->mot);
        }
      }
      Trace1("resultat => nombre = %s \n",lp_Key->nombre ) ;
    }    
    //------------------------------------------------------------
    // On met le mot dans premier si il est vide 
    //------------------------------------------------------------
    
    if ( ! strcmp( lp_Key->premier, "")){ 
      if ( strlen(lp_Key->mot) < CONFIG_TAILLE_BUFFER_32)
      strcpy( lp_Key->premier, lp_Key->mot);
    }
    //------------------------------------------------------------
    // Si le mot en cours est une VALIDATION 
    // Exemple : valider
    // Alors on VALIDE la phrase en cours
    //  - en mettant lp_Key->phrase_lu à 1
    // on efface tout sauf SYMBOLE et NOMBRE qui sont determiner plus tot
    //------------------------------------------------------------

    for( i=0 ; i < KEYS_VALIDATIONS_SIZE ; i++ )
    if ( ! strcmp( lp_Key->mot,    lp_Key->valider )  ) {
      Trace1("Appui sur valider => on met premier dans symbole, phrase dans nombre, et NULL dans phrase et mot, phrase_lue a 1" ) ; 

      strcpy(lp_Key->premier,"") ;
      strcpy(lp_Key->phrase,"")  ;
      strcpy(lp_Key->mot,"") ;
      lp_Key->phrase_lue=1 ;
    }
    //------------------------------------------------------------
    // Si le mot est une ACTION, on efface la phrase en cours    
    // on met le mot dans PREMIER (premier mot de la phrase) 
    // on met le mot dans SYMBOLE (symbole utilise dans prog main) 
    //------------------------------------------------------------
    
    for( i=0 ; i < KEYS_ACTIONS_SIZE ; i++ )
    if ( ! strcmp( lp_Key->mot, lp_Key->actions[i] )) {
        Trace1("Si le mot est une ACTION, alors on efface la phrase en cours et on met mot dans premier et symbole") ;
        strcpy(lp_Key->premier,lp_Key->mot) ;
        strcpy(lp_Key->symbole,lp_Key->mot)  ;

        strcpy(lp_Key->nombre,"")  ;
        strcpy(lp_Key->phrase,"")  ;
        strcpy(lp_Key->mot,"") ;
        lp_Key->phrase_lue=0 ;
    }
    
    lp_Key->mot_en_cours = 0 ;
    lp_Key->appui_en_cours = 0 ;
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
  
  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1("") ;

  memset( lp_Key->phrase,  CALCULS_ZERO_CHAR, sizeof( lp_Key->phrase ) );
  memset( lp_Key->valider, CALCULS_ZERO_CHAR, sizeof( lp_Key->valider ) );
  memset( lp_Key->symbole, CALCULS_ZERO_CHAR, sizeof( lp_Key->symbole ) );
  memset( lp_Key->menu,    CALCULS_ZERO_CHAR, sizeof( lp_Key->menu ) );
  memset( lp_Key->premier, CALCULS_ZERO_CHAR, sizeof( lp_Key->premier ) );
  memset( lp_Key->nombre,  CALCULS_ZERO_CHAR, sizeof( lp_Key->nombre ) );
  memset( lp_Key->mot,     CALCULS_ZERO_CHAR, sizeof( lp_Key->mot ) );
  
  strcpy( lp_Key->valider, "valider" ) ;
  strcpy( lp_Key->menu,    "MENU" ) ;
  
  strcpy( lp_Key->symbole,  "" ) ;
  strcpy( lp_Key->phrase,   "" ) ;
  strcpy( lp_Key->premier,  "" ) ;
  strcpy( lp_Key->nombre,   "" ) ;
  strcpy( lp_Key->mot,      "" ) ;

  lp_Key->mot_en_cours   = 0 ;
  lp_Key->phrase_lue     = 0 ;
  lp_Key->appui_en_cours = 0 ;
  /* lp_Key->tempo_clavier = gp_Tim_Par->par_tpo_Clavier ; */ 
 
  // FIXME : definitions des actions : 
  // Les actions servent a 
  
  for( i=0 ; i < KEYS_ACTIONS_SIZE ; i++ ) {
    memset( lp_Key->actions[i], CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_32);
  }
  // for( i=0 ; i < KEYS_VALIDATIONS_SIZE ; i++ )  memset( lp_Key->validations[i], CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_32);

  strcpy( lp_Key->actions[0], "MENU" ) ;
  strcpy( lp_Key->actions[1], "SETUP" ) ;
  strcpy( lp_Key->actions[2], "MES" ) ;
  strcpy( lp_Key->actions[3], "NGC" ) ;
  strcpy( lp_Key->actions[4], "ETO" ) ;
  strcpy( lp_Key->actions[5], "PLA" ) ;
  strcpy( lp_Key->actions[6], "TIME" ) ;
   
} 

/*****************************************************************************************
* @fn     : KEYS_AFFICHER
* @author : s.gravois
* @brief  : Cette fonction affiche les informations d entrees input ("lp_Key")
* @param  : STRUCT_KEYS *lp_Key
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void KEYS_AFFICHER(STRUCT_KEYS *lp_Key) {
  
  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  sprintf( c_cmd , "(phr) %-5s (mot) %-5s (sym) %-5s (nom) %-5s (pre) %-5s (val) %-5s (menu) %-10s",\
    lp_Key->phrase,\
    lp_Key->mot,\
    lp_Key->symbole,\
    lp_Key->nombre,\
    lp_Key->premier,\
    lp_Key->valider,\
    lp_Key->menu) ;

  
  Trace1( "%s", c_cmd) ;

  Trace2("lp_Key->mot         = %s",lp_Key->mot) ;
  Trace2("lp_Key->premier     = %s",lp_Key->premier) ;
  Trace2("lp_Key->phrase      = %s",lp_Key->phrase) ;
  Trace2("lp_Key->nombre      = %s",lp_Key->nombre) ;
  Trace2("lp_Key->symbole     = %s",lp_Key->symbole) ;
  Trace2("lp_Key->phrase_lue  = %d",lp_Key->phrase_lue) ;

  TraceArbo(__func__,1,c_cmd) ; /* MACRO_DEBUG_ARBO_FONCTIONS */
}

