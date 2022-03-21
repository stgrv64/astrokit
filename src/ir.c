/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | * ajout entete
#               * remplacement key_mute par TIME
# --------------------------------------------------------------
# 15/11/2021  | * modification nom fct mainXxxx() pour compilation
#                 => mainIr <=> main
# 20/01/2022  | * ajout KEY_ZOOM (oubli) et action associee
#
# 21/03/2022  | * mise en place structure g_Codes (t_st_Codes)
#               * deport du code dans fichier config.c /.h
#               * renommage fonction LIRC_CODE => COPNFIG_INIT_CODE
# --------------------------------------------------------------  
*/

#include <ir.h>

//====================================================================================
int LIRC_OPEN(struct lirc_config *lircconfig) {

  int retour = 0 ;
  
  if( lirc_init("lirc",1)==-1)                          retour = 1 ;
  if( lirc_readconfig( NULL, &lircconfig, NULL) != 0 )  retour = 2 ;  
  
  return retour ;
}
//====================================================================================
  
void LIRC_CLOSE(struct lirc_config *lircconfig) {
  
  // Frees the data structures associated with config.
  lirc_freeconfig(lircconfig);
  
  // lirc_deinit() closes the connection to lircd and does some internal clean-up stuff.
  lirc_deinit();
}
//====================================================================================
void LIRC_READ(SUIVI *suivi) {

  char *code;
  char *str1, *token, *sptr ;
  int i,j,k ;  
  
  // ATTENTION la fonction lirc_nextcode est bloquante
  // de plus elle conserve dans un tampon les touches lues
  // au fil de l'eau 
  
  TRACE("start\n");

  while(lirc_nextcode(&code)==0) {
    
    TRACE1("on a lu quelquechose ..") ;
    //GPIO_CLIGNOTE(GPIO_LED_ETAT, 2, 2) ;

    if ( code == NULL ) { 
      TRACE1("code NULL lu ..") ;
      continue; 
    }
    TRACE1("code = %s", code);
   
    i=k=0 ; 
    for (j = 0, str1 = code ; ; j++, str1 = NULL) {
     
       token = strtok_r(str1, " ", &sptr);
       if (token == NULL) break ;
       TRACE1("token i j k = %s %d %d %d",token, i,j,k); 
       if ( j==1 ) k=atoi(token) ;
       if ( j==2 ) {  
         while(strcmp(gp_Codes->in_lirc[i],token) && ( i < CONFIG_CODE_NB_CODES ) ){ 
           TRACE1("%s = %s ?", token, gp_Codes->in_lirc[i]) ;  
           i++ ; 
         }
       }
    }  
    TRACE1("i j k = %d %d %d", i,j,k); 
    memset( suivi->datas_infrarouge, '\0', strlen( suivi->datas_infrarouge ) ) ;
       
    if ( k == 0 && i < CONFIG_CODE_NB_CODES ) {
      strcpy( suivi->datas_infrarouge, gp_Codes->out_act[i] ) ;
    }

    if ( k > 0 && i < CONFIG_CODE_NB_CODES && i <= IR_CODE_REPETE_AUTORISE_MAX && i >= IR_CODE_REPETE_AUTORISE_MIN ) {
      strcpy( suivi->datas_infrarouge, gp_Codes->out_act[i] ) ;
    }
    
    // tres important !!
    // le usleep suivant permet de garder l information !!!!!!
    // suivi->datas_infrarouge fonctionne comme un TAMPON
    // il va etre lu par les threads du programme principal
    
    Trace("suivi->datas_infrarouge = %s", suivi->datas_infrarouge ) ;
    Trace("suivi->temporisation_ir = %ld", suivi->temporisation_ir ) ;
    
    usleep( suivi->temporisation_ir ) ;
    
    free(code);

    memset( suivi->datas_infrarouge, 0, strlen( suivi->datas_infrarouge ) ) ;
    strcpy( suivi->datas_infrarouge, "") ;
  }
}
//====================================================================================
void IR_KEYBOARD_READ(SUIVI *suivi, CLAVIER *clavier) {

  int  i = 0 ;
  char val[255] ;
  char s_buffer[CONFIG_TAILLE_BUFFER_32] ;

  memset(val, 0, strlen(val)) ;     
  strcpy( val, suivi->datas_infrarouge ) ;
  
  Trace1("val = %s\n", val ) ;
  
  if ( strcmp( val, "") ) {
    
    //printf("val = %s\n", val ) ;
    strcpy( clavier->mot, val ) ; 
    clavier->appui_en_cours = 1 ;
    clavier->mot_en_cours = 1 ;    
  }
  else clavier->appui_en_cours = 0 ; 
  
  // =======================================================================
  // Quand la touche est relacheee, on traite
  // ensuite appui en cours est remis a ZERO pour qu'on ne passe qu'une fois
  // dans cette partie de code
  // =======================================================================
  
  if ( clavier->mot_en_cours && clavier->appui_en_cours == 0 ) {
  
    Trace1("mot trouver = %s", clavier->mot ) ;
		
		GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 5) ;

    //------------------------------------------------------------
    // On incremente la phrase avec le mot et
    // On incremente le nombre avec le mot si premier n'est pas vide
    //------------------------------------------------------------
    
    if ( strcmp( clavier->mot, clavier->valider) != 0 ) { 
      
      Trace1("Si mot != valider : on incremente la phrase avec le mot !!\n" ) ;

      if ( strlen(clavier->phrase) + strlen(clavier->mot) < CONFIG_TAILLE_BUFFER_32) {

        Trace1("on incremente la phrase %s avec le mot %s\n",clavier->phrase,  clavier->mot ) ;
        // correction bug 28/12/2017 : recopie de la chaine dans un buffer avant le sprintf

        memset( s_buffer, IR_ZERO_CHAR, strlen( s_buffer )) ;
        strcpy( s_buffer, clavier->phrase ) ;

        sprintf(clavier->phrase,"%s%s",s_buffer, clavier->mot);
      }
      Trace1("resultat => phrase = %s \n",clavier->phrase ) ;
      
      Trace1("Si mot != valider et premier non vide => on met nombre + mot dans le nombre !!\n" ) ;

      if ( strcmp( clavier->premier, "")) {
        
	      if ( strlen(clavier->nombre) + strlen(clavier->mot) < CONFIG_TAILLE_BUFFER_32) {

          Trace1("on incremente le nombre %s avec le mot %s\n",clavier->nombre,  clavier->mot ) ;
          // correction bug 28/12/2017 : recopie de la chaine dans un buffer avant le sprintf

          memset( s_buffer, IR_ZERO_CHAR, strlen( s_buffer )) ;
          strcpy( s_buffer, clavier->nombre ) ;

          sprintf(clavier->nombre,"%s%s", s_buffer, clavier->mot);
        }
      }
      Trace1("resultat => nombre = %s \n",clavier->nombre ) ;
    }    
    //------------------------------------------------------------
    // On met le mot dans premier si il est vide 
    //------------------------------------------------------------
    
    if ( ! strcmp( clavier->premier, "")){ 
      if ( strlen(clavier->mot) < CONFIG_TAILLE_BUFFER_32)
      strcpy( clavier->premier, clavier->mot);
    }
    //------------------------------------------------------------
    // Si le mot en cours est une VALIDATION 
    // Exemple : valider
    // Alors on VALIDE la phrase en cours
    //  - en mettant clavier->phrase_lu à 1
    // on efface tout sauf SYMBOLE et NOMBRE qui sont determiner plus tot
    //------------------------------------------------------------

    for( i=0 ; i < CONFIG_VALIDATIONS_SIZE ; i++ )
    if ( ! strcmp( clavier->mot,    clavier->valider )  ) {
      Trace1("Appui sur valider => on met premier dans symbole, phrase dans nombre, et NULL dans phrase et mot, phrase_lue a 1" ) ; 

      strcpy(clavier->premier,"") ;
      strcpy(clavier->phrase,"")  ;
      strcpy(clavier->mot,"") ;
      clavier->phrase_lue=1 ;
    }
    //------------------------------------------------------------
    // Si le mot est une ACTION, on efface la phrase en cours    
    // on met le mot dans PREMIER (premier mot de la phrase) 
    // on met le mot dans SYMBOLE (symbole utilise dans prog main) 
    //------------------------------------------------------------
    
    for( i=0 ; i < CONFIG_ACTIONS_SIZE ; i++ )
    if ( ! strcmp( clavier->mot, clavier->actions[i] )) {
        Trace1("Si le mot est une ACTION, alors on efface la phrase en cours et on met mot dans premier et symbole") ;
        strcpy(clavier->premier,clavier->mot) ;
        strcpy(clavier->symbole,clavier->mot)  ;

        strcpy(clavier->nombre,"")  ;
        strcpy(clavier->phrase,"")  ;
        strcpy(clavier->mot,"") ;
        clavier->phrase_lue=0 ;
    }
    
    clavier->mot_en_cours = 0 ;
    clavier->appui_en_cours = 0 ;
    
    CONFIG_AFFICHER_CLAVIER( clavier ) ;	
  }
}

//==========================================================
void IR_ACTIONS_PARTICULIERES( SUIVI *suivi) {
    
  if ( ! strcmp( suivi->datas_infrarouge, "plus" ) )  {
    suivi->Ta *=  suivi->plus  ; suivi->Fa = 1 / suivi->Ta ;
    suivi->Th *=  suivi->plus  ; suivi->Fh = 1 / suivi->Th ;
  }
  if ( ! strcmp( suivi->datas_infrarouge, "moins" ) ) {
    suivi->Ta *=  suivi->moins  ; suivi->Fa = 1 / suivi->Ta ;
    suivi->Th *=  suivi->moins  ; suivi->Fh = 1 / suivi->Th ;
  }
}
//====================================================================================
/* 
 TODO : modifier pour avoir le meme comportement que appel thread SUIVI_INFRAROUGE
*/
void mainIr(int argc, char *argv[])  // void main(int argc, char *argv[]) 
{	
  
  SUIVI *suivi ;
  SUIVI su ;   
  
  gp_Codes   = &g_Codes ;
  suivi = &su ;
  
  suivi->temporisation_ir = 10000 ;
  
  CONFIG_INIT_CODES( gp_Codes ) ;
  LIRC_OPEN( lircconfig ) ;
  LIRC_READ( suivi ) ;
  LIRC_CLOSE( lircconfig ) ;
}
//====================================================================================
