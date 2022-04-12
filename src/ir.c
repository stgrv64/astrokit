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
# 21/03/2022  | * mise en place structure g_Codes (CODES)
#               * deport du code dans fichier config.c /.h
#               * renommage fonction LIRC_CODE => CONFIG_INIT_CODE
#
# 22/03/2022  | * deplacement de 2 fonctions dans config.c/.h
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
/*****************************************************************************************
* @fn     : LIRC_READ
* @author : s.gravois
* @brief  : lecture de donnees sur le port infrarouge, doit etre appelle depuis un thread
* @brief  :  car on utilise la fonction bloquante suivante : lirc_nextcode()
* @param  : SUIVI * suivi
* @date   : 2022-04-12 creation entete de la fonction au format doxygen
* @date   : 2022-04-12 mutex_lock sur suivi->datas_infrarouge
* @todo   : 
*****************************************************************************************/

void LIRC_READ(SUIVI *suivi) {

  char *code;
  char *str1, *token, *sptr ;
  int i_indice_code,j,k ;  
  
  // ATTENTION la fonction lirc_nextcode est bloquante
  // de plus elle conserve dans un tampon les touches lues
  // au fil de l'eau 
  
  Trace("start");

  while(lirc_nextcode(&code)==0) {
    
    Trace1("on a lu quelquechose ..") ;
    //GPIO_CLIGNOTE(GPIO_LED_ETAT, 2, 2) ;

    if ( code == NULL ) { 
      Trace1("code NULL lu ..") ;
      continue; 
    }
    Trace1("code = %s", code);
   
    i_indice_code=k=0 ; 
    for (j = 0, str1 = code ; ; j++, str1 = NULL) {
     
       token = strtok_r(str1, " ", &sptr);
       if (token == NULL) break ;
       Trace1("token i_indice_code j k = %s %d %d %d",token, i_indice_code,j,k); 
       if ( j==1 ) k=atoi(token) ;
       if ( j==2 ) {  
         while(strcmp(gp_Codes->in_lirc[i_indice_code],token) && ( i_indice_code < CONFIG_CODE_NB_CODES ) ){ 
           Trace1("%s = %s ?", token, gp_Codes->in_lirc[i_indice_code]) ;  
           i_indice_code++ ; 
         }
       }
    }  
    Trace1("i_indice_code j k = %d %d %d", i_indice_code,j,k); 

    pthread_mutex_lock(& suivi->mutex_infrarouge );
    memset( suivi->datas_infrarouge, '\0', strlen( suivi->datas_infrarouge ) ) ;
    pthread_mutex_unlock(& suivi->mutex_infrarouge );

    if ( k == 0 && i_indice_code < CONFIG_CODE_NB_CODES ) {
      
      pthread_mutex_lock(& suivi->mutex_infrarouge );
      Trace2("AA maj suivi->datas_infrarouge") ;
      strcpy( suivi->datas_infrarouge, gp_Codes->out_act[i_indice_code] ) ;
      pthread_mutex_unlock(& suivi->mutex_infrarouge );

      GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 10) ;
    }

    if ( k > 0 && \
         i_indice_code < CONFIG_CODE_NB_CODES && \
         i_indice_code <= IR_CODE_REPETE_AUTORISE_MAX && \
         i_indice_code >= IR_CODE_REPETE_AUTORISE_MIN ) {

      pthread_mutex_lock(& suivi->mutex_infrarouge );
      Trace2("BB maj suivi->datas_infrarouge") ;
      strcpy( suivi->datas_infrarouge, gp_Codes->out_act[i_indice_code] ) ;
      pthread_mutex_unlock(& suivi->mutex_infrarouge );

      GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 10) ;
    }
    
    // tres important !!
    // le usleep suivant permet de garder l information !!!!!!
    // suivi->datas_infrarouge fonctionne comme un TAMPON
    // il va etre lu par les threads du programme principal
    
    Trace("suivi->datas_infrarouge = %s", suivi->datas_infrarouge ) ;
    Trace("suivi->temporisation_ir = %ld", suivi->temporisation_ir ) ;
    
    usleep( suivi->temporisation_ir ) ;
    
    free(code);

    pthread_mutex_lock(& suivi->mutex_infrarouge );
    Trace2("h remise a zero de suivi->datas_infrarouge") ;
    memset( suivi->datas_infrarouge, 0, strlen( suivi->datas_infrarouge ) ) ;
    strcpy( suivi->datas_infrarouge, "") ;
    pthread_mutex_unlock(& suivi->mutex_infrarouge );
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
