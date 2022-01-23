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
# --------------------------------------------------------------  
*/

#include <ir.h>

//====================================================================================
// etablie la correspondance entre les KEY du fichier lircd.conf
// et les codes utilises dans le programme principal
// (car pas de hachage en c) modifier : completer avec une fonction de hachage
//------------------------------------------------------------------------------------

void LIRC_CONFIG_CODES(IR_LIRC_CODES *irc) {

  int i ;
  
  for( i=0 ; i<IR_NB_CODES ; i++ ) memset( irc->code[i], IR_ZERO_CHAR, strlen(irc->code[i]) ) ;
  for( i=0 ; i<IR_NB_CODES ; i++ ) memset( irc->key[i],  IR_ZERO_CHAR, strlen(irc->key[i]) ) ;

  // FIXME :  ATTENTION !!! 
  // le nom des irc codes en MAJUSCULE servent a definir des actions dans config.c et le reste du programme

  strcpy( irc->key[0], "KEY_0") ;           strcpy( irc->code[0], "0") ;
  strcpy( irc->key[1], "KEY_1") ;           strcpy( irc->code[1], "1") ;
  strcpy( irc->key[2], "KEY_2") ;           strcpy( irc->code[2], "2") ;
  strcpy( irc->key[3], "KEY_3") ;           strcpy( irc->code[3], "3") ;
  strcpy( irc->key[4], "KEY_4") ;           strcpy( irc->code[4], "4") ;
  strcpy( irc->key[5], "KEY_5") ;           strcpy( irc->code[5], "5") ;
  strcpy( irc->key[6], "KEY_6") ;           strcpy( irc->code[6], "6") ;
  strcpy( irc->key[7], "KEY_7") ;           strcpy( irc->code[7], "7") ;
  strcpy( irc->key[8], "KEY_8") ;           strcpy( irc->code[8], "8") ;
  strcpy( irc->key[9], "KEY_9") ;           strcpy( irc->code[9], "9") ;

  strcpy( irc->key[10],"KEY_M") ;           strcpy( irc->code[10],"MES") ;
  strcpy( irc->key[11],"KEY_N") ;           strcpy( irc->code[11],"NGC") ;
  strcpy( irc->key[12],"KEY_E") ;           strcpy( irc->code[12],"ETO") ;
  strcpy( irc->key[13],"KEY_P") ;           strcpy( irc->code[13],"PLA") ;

  strcpy( irc->key[14],"KEY_PREVIOUS");     strcpy( irc->code[14],"previous") ;
  strcpy( irc->key[15],"KEY_PLAY") ;        strcpy( irc->code[15],"valider") ;
  strcpy( irc->key[16],"KEY_NEXT") ;        strcpy( irc->code[16],"plus") ;
  strcpy( irc->key[17],"KEY_STOP") ;        strcpy( irc->code[17],"stop") ;
  strcpy( irc->key[18],"KEY_MENU") ;        strcpy( irc->code[18],"MENU") ;
  strcpy( irc->key[19],"KEY_PAUSE") ;       strcpy( irc->code[19],"pause") ;
  strcpy( irc->key[20],"KEY_OK") ;          strcpy( irc->code[20],"reset") ;
  strcpy( irc->key[21],"KEY_UP") ;          strcpy( irc->code[21],"n") ;
  strcpy( irc->key[22],"KEY_RIGHT") ;       strcpy( irc->code[22],"e") ;
  strcpy( irc->key[23],"KEY_DOWN") ;        strcpy( irc->code[23],"s") ;
  strcpy( irc->key[24],"KEY_LEFT") ;        strcpy( irc->code[24],"o") ;
  strcpy( irc->key[25],"KEY_SETUP") ;       strcpy( irc->code[25],"SETUP") ;
  // strcpy( irc->key[26],"KEY_TIME") ;        strcpy( irc->code[26],"TIME") ;    // ajout du 10/10/2016
  strcpy( irc->key[26],"KEY_SOUND") ;        strcpy( irc->code[26],"TIME") ;    // ajout du 10/10/2016

  strcpy( irc->key[27],"KEY_FORWARD") ;     strcpy( irc->code[27],"forward") ;
  strcpy( irc->key[28],"KEY_REWIND") ;      strcpy( irc->code[28],"rewind") ;    // ajout du 26/11/2017

  strcpy( irc->key[29],"KEY_RED") ;         strcpy( irc->code[29],"red") ;
  strcpy( irc->key[30],"KEY_BLUE") ;        strcpy( irc->code[30],"blue") ;
  strcpy( irc->key[31],"KEY_YELLOW") ;      strcpy( irc->code[31],"yellow") ;
  strcpy( irc->key[32],"KEY_GREEN") ;       strcpy( irc->code[32],"green") ;   // ajout ulterieurs.. pour etre idem que reel 25 et superieur ...

  strcpy( irc->key[33],"KEY_POWER") ;       strcpy( irc->code[33],"key_power") ;

  strcpy( irc->key[34],"KEY_CHANNELUP") ;   strcpy( irc->code[34],"forwardfast") ;
  strcpy( irc->key[35],"KEY_CHANNELDOWN") ; strcpy( irc->code[35],"rewindfast") ;
  strcpy( irc->key[36],"KEY_VOLUMEUP") ;    strcpy( irc->code[36],"forward") ;
  strcpy( irc->key[37],"KEY_VOLUMEDOWN") ;  strcpy( irc->code[37],"rewind") ;

  strcpy( irc->key[38],"KEY_MUTE") ;        strcpy( irc->code[38],"TIME") ;

  strcpy( irc->key[39],"KEY_SCREEN") ;      strcpy( irc->code[39],"key_screen") ; 
  strcpy( irc->key[40],"KEY_TV") ;          strcpy( irc->code[40],"key_tv") ;
  strcpy( irc->key[41],"KEY_INFO") ;        strcpy( irc->code[41],"key_info") ;
  strcpy( irc->key[42],"KEY_ZOOM") ;        strcpy( irc->code[42],"key_zoom") ;
  strcpy( irc->key[43],"KEY_LIST") ;        strcpy( irc->code[43],"key_list") ;
  strcpy( irc->key[44],"KEY_MODE") ;        strcpy( irc->code[44],"key_mode") ; 
  strcpy( irc->key[45],"KEY_EXIT") ;        strcpy( irc->code[45],"key_exit") ;
  
  // codes VOUTE de la telecommande - joue sur la vitesse globale
}
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
         while(strcmp(irc->key[i],token) && ( i < IR_NB_CODES ) ){ 
           TRACE1("%s = %s ?", token, irc->key[i]) ;  
           i++ ; 
         }
       }
    }  
    TRACE1("i j k = %d %d %d", i,j,k); 
    memset( suivi->datas_infrarouge, '\0', strlen( suivi->datas_infrarouge ) ) ;
       
    if ( k == 0 && i < IR_NB_CODES ) strcpy( suivi->datas_infrarouge, irc->code[i] ) ;
    if ( k > 0 && i < IR_NB_CODES && i <= IR_CODE_REPETE_AUTORISE_MAX && i >= IR_CODE_REPETE_AUTORISE_MIN )  
         strcpy( suivi->datas_infrarouge, irc->code[i] ) ;
    
    // tres important !!
    // le usleep suivant permet de garder l information !!!!!!
    // suivi->datas_infrarouge fonctionne comme un TAMPON
    // il va etre lu par les threads du programme principal
    
    TRACE1("suivi->datas_infrarouge = %s", suivi->datas_infrarouge ) ;
    TRACE1("suivi->temporisation_ir = %ld", suivi->temporisation_ir ) ;
    
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
  
  irc   = &ir_codes ;
  suivi = &su ;
  
  suivi->temporisation_ir = 10000 ;
  
  LIRC_CONFIG_CODES( irc ) ;
  LIRC_OPEN( lircconfig ) ;
  LIRC_READ( suivi ) ;
  LIRC_CLOSE( lircconfig ) ;
}
//====================================================================================
