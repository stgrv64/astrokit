#define _GNU_SOURCE

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
# 21/03/2022  | * mise en place structure g_Cod (CODES)
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
* @param  : SUIVI * gp_Sui
* @date   : 2022-04-12 creation entete de la fonction au format doxygen
* @date   : 2022-04-12 mutex_lock sur gp_Sui->sui_dat.dat_inf
* @todo   : 
*****************************************************************************************/

void LIRC_READ(SUIVI * gp_Sui) {

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
         while(strcmp(gp_Cod->in_lirc[i_indice_code],token) && ( i_indice_code < CONFIG_CODE_NB_CODES ) ){ 
           Trace1("%s = %s ?", token, gp_Cod->in_lirc[i_indice_code]) ;  
           i_indice_code++ ; 
         }
       }
    }  
    Trace1("i_indice_code j k = %d %d %d", i_indice_code,j,k); 

    pthread_mutex_lock(& gp_Pth->pth_mut.mut_dat );
    memset( gp_Sui->sui_dat.dat_inf, '\0', strlen( gp_Sui->sui_dat.dat_inf ) ) ;
    pthread_mutex_unlock(& gp_Pth->pth_mut.mut_dat );

    if ( k == 0 && i_indice_code < CONFIG_CODE_NB_CODES ) {
      
      pthread_mutex_lock(& gp_Pth->pth_mut.mut_dat );
      Trace2("AA maj gp_Sui->sui_dat.dat_inf") ;
      strcpy( gp_Sui->sui_dat.dat_inf, gp_Cod->out_act[i_indice_code] ) ;
      pthread_mutex_unlock(& gp_Pth->pth_mut.mut_dat );

      GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 10) ;
    }

    if ( k > 0 && \
         i_indice_code < CONFIG_CODE_NB_CODES && \
         i_indice_code <= IR_CODE_REPETE_AUTORISE_MAX && \
         i_indice_code >= IR_CODE_REPETE_AUTORISE_MIN ) {

      pthread_mutex_lock(& gp_Pth->pth_mut.mut_dat );
      Trace2("BB maj gp_Sui->sui_dat.dat_inf") ;
      strcpy( gp_Sui->sui_dat.dat_inf, gp_Cod->out_act[i_indice_code] ) ;
      pthread_mutex_unlock(& gp_Pth->pth_mut.mut_dat );

      GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 10) ;
    }
    
    // tres important !!
    // le usleep suivant permet de garder l information !!!!!!
    // gp_Sui->sui_dat.dat_inf fonctionne comme un TAMPON
    // il va etre lu par les threads du programme principal
    
    Trace("gp_Sui->sui_dat.dat_inf = %s", gp_Sui->sui_dat.dat_inf ) ;
    Trace("gp_Sui->temporisation_ir = %ld", gp_Sui->temporisation_ir ) ;
    
    usleep( gp_Sui->temporisation_ir ) ;
    
    free(code);

    pthread_mutex_lock(& gp_Pth->pth_mut.mut_dat );
    Trace("raz de gp_Sui->sui_dat.dat_inf") ;
    memset( gp_Sui->sui_dat.dat_inf, 0, strlen( gp_Sui->sui_dat.dat_inf ) ) ;
    strcpy( gp_Sui->sui_dat.dat_inf, "") ;
    pthread_mutex_unlock(& gp_Pth->pth_mut.mut_dat );
  }
}

//==========================================================
void IR_ACTIONS_PARTICULIERES( SUIVI * gp_Sui) {
    
  if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "plus" ) )  {
    gp_Sui->Ta_mic *=  gp_Sui->plus  ; gp_Sui->Fa_mic = 1 / gp_Sui->Ta_mic ;
    gp_Sui->Th_mic *=  gp_Sui->plus  ; gp_Sui->Fh_mic = 1 / gp_Sui->Th_mic ;
  }
  if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "moins" ) ) {
    gp_Sui->Ta_mic *=  gp_Sui->moins  ; gp_Sui->Fa_mic = 1 / gp_Sui->Ta_mic ;
    gp_Sui->Th_mic *=  gp_Sui->moins  ; gp_Sui->Fh_mic = 1 / gp_Sui->Th_mic ;
  }
}
//====================================================================================
/* 
 TODO : modifier pour avoir le meme comportement que appel thread SUIVI_INFRAROUGE
*/
void mainIr(int argc, char *argv[])  // void main(int argc, char *argv[]) 
{	
  
  SUIVI * gp_Sui ;
  SUIVI su ;   
  
  gp_Cod   = &g_Cod ;
  gp_Sui = &su ;
  
  gp_Sui->temporisation_ir = 10000 ;
  
  CONFIG_INIT_CODES( gp_Cod ) ;
  LIRC_OPEN( lircconfig ) ;
  LIRC_READ( gp_Sui ) ;
  LIRC_CLOSE( lircconfig ) ;
}
//====================================================================================
