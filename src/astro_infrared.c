/* #define _GNU_SOURCE */

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
# 21/03/2022  | * mise en place structure g_Cod (STRUCT_CODES)
#               * deport du code dans fichier config.c /.h
#               * renommage fonction LIRC_CODE => CONFIG_INIT_CODE
#
# 22/03/2022  | * deplacement de 2 fonctions dans config.c/.h
# --------------------------------------------------------------  
*/

#include "astro_infrared.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;
MACRO_ASTRO_GLOBAL_EXTERN_INFRARED ;

/*****************************************************************************************
* @fn     : INFRARED_OPEN
* @author : s.gravois
* @brief  : Ouverture des flux IR a l aide du paquet LIRC
* @param  : struct lirc_config *gp_LircConfig
* @date   : 2022-04-12 creation entete de la fonction au format doxygen
* @date   : 2022-04-12 mutex_lock sur gp_Dat->dat_act
* @todo   : 
*****************************************************************************************/

int INFRARED_OPEN(INFRARED_LIRC_CONFIG *gp_LircConfig) {

  int retour = 0 ;
  
  if( lirc_init("lirc",1)==-1)                      retour = 1 ;
  if( lirc_readconfig( NULL, &gp_LircConfig, NULL) != 0 )  retour = 2 ;  
  
  return retour ;
}
//====================================================================================
  
void INFRARED_CLOSE(INFRARED_LIRC_CONFIG *gp_LircConfig) {
  
  // Frees the data structures associated with config.
  lirc_freeconfig(gp_LircConfig);
  
  // lirc_deinit() closes the connection to lircd and does some internal clean-up stuff.
  lirc_deinit();
}
/*****************************************************************************************
* @fn     : INFRARED_READ
* @author : s.gravois
* @brief  : lecture de donnees sur le port infrarouge, doit etre appelle depuis un thread
* @brief  :  car on utilise la fonction bloquante suivante : lirc_nextcode()
* @param  : 
* @date   : 2022-04-12 creation entete de la fonction au format doxygen
* @date   : 2022-04-12 mutex_lock sur gp_Dat->dat_act
* @todo   : 
*****************************************************************************************/

void INFRARED_READ(void) {

  char *code  = NULL ;
  char *str1  = NULL ;
  char *token = NULL ;
  char *sptr  = NULL ;
  int i_indice_code,j,k ;  
  
  // ATTENTION la fonction lirc_nextcode est bloquante
  // de plus elle conserve dans un tampon les touches lues
  // au fil de l'eau 
  
  Trace("start");

  while(lirc_nextcode(&code)==0) {
        
    Trace("on a lu quelquechose ..") ;
    //GPIO_LED_ETAT_CLIGNOTE(2, 2) ;

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
         while(strcmp( gp_Cod->cod_in_lirc[i_indice_code],token) && ( i_indice_code < CODES_CODE_NB_CODES ) ){ 
           Trace1("%s = %s ?", token, gp_Cod->cod_in_lirc[i_indice_code]) ;  
           i_indice_code++ ; 
         }
       }
    }  
    Trace1("i_indice_code j k = %d %d %d", i_indice_code,j,k); 

    if ( k == 0 && i_indice_code < CODES_CODE_NB_CODES ) {
      
      DATAS_ACTION_SET( gp_Dat, gp_Cod->cod_out_act[i_indice_code] ) ;
    }

    if ( k > 0 && \
         i_indice_code < CODES_CODE_NB_CODES && \
         i_indice_code <= IR_CODE_REPETE_AUTORISE_MAX && \
         i_indice_code >= IR_CODE_REPETE_AUTORISE_MIN ) {

      DATAS_ACTION_SET( gp_Dat, gp_Cod->cod_out_act[i_indice_code] ) ;
    }
    
    // tres important !!
    // le usleep suivant permet de garder l information !!!!!!
    // gp_Dat->dat_act fonctionne comme un TAMPON
    // il va etre lu par les threads du programme principal
    
    Trace1("gp_Dat->dat_act = %s", gp_Dat->dat_act ) ;
    Trace("gp_Tpo->tpo_ir = %ld", gp_Tpo->tpo_ir ) ;
  
    usleep( gp_Tpo->tpo_ir ) ;
    
    DATAS_ACTION_RESET( gp_Dat ) ;
  }
}

//====================================================================================
/* 
 TODO : modifier pour avoir le meme comportement que appel thread _SUIVI_INFRAROUGE
*/
void mainIr(int argc, char *argv[])  // void main(int argc, char *argv[]) 
{	
  
  STRUCT_SUIVI * gp_Sui ;
  STRUCT_SUIVI su ;   
  
  gp_Cod   = & g_Codes ;
  gp_Sui   = & g_Suivi ;
  
  gp_Tpo->tpo_ir = 10000 ;
  
  CODES_INIT( gp_Cod ) ;
  INFRARED_OPEN( gp_LircConfig ) ;
  INFRARED_READ() ;
  INFRARED_CLOSE( gp_LircConfig ) ;
}
//====================================================================================
