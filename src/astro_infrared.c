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
* @fn     : ASTRE_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_ASTRE *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void ASTRE_DISPLAY_PREPARE ( STRUCT_ASTRE * lp_Ast) {

  TraceArbo(__func__,2,"astre format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex ) ;

  sprintf( lp_Ast->ast_dis_cmd , STR_ASTRE_FORMAT,\
    lp_Ast->ast_nom, \
    lp_Ast->ast_hhmmss_asc, \
    lp_Ast->ast_ddmm_dec, \
    lp_Ast->ast_hhmmss_agh, \
    lp_Ast->ast_ddmm_azi, \
    lp_Ast->ast_ddmm_alt, \
    lp_Ast->ast_azi_vit, \
    lp_Ast->ast_alt_vit ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static ASTRE_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_ASTRE *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void ASTRE_DISPLAY(STRUCT_ASTRE *lp_Ast) {

  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,2,"display informations on Astre") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  ASTRE_DISPLAY_PREPARE( lp_Ast ) ;

  MACRO_ASTRO_GLOBAL_LOG_ON ( lp_Ast->ast_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Ast->ast_loglevel , 1 , "%s", lp_Ast->ast_dis_cmd ) ;
  MACRO_ASTRO_GLOBAL_LOG_OFF( lp_Ast->ast_loglevel ) ;

  return ;
}


/*****************************************************************************************
* @fn     : INFRARED_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_INFRARED *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void INFRARED_LOCK ( STRUCT_INFRARED * lp_Inf) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Inf->inf_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : INFRARED_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_INFRARED *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void INFRARED_UNLOCK ( STRUCT_INFRARED * lp_Inf) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Inf->inf_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : INFRARED_INIT
* @author : s.gravois
* @brief  : Initialise la structure STRUCT_INFRARED
* @param  : STRUCT_INFRARED *
* @date   : 2022-12-20 creation entete 
* @todo   : (inserer dans STRUCT_INFRARED le contenu de STR_EXT_LIRC_CONFIG)
*****************************************************************************************/

void  INFRARED_INIT ( STRUCT_INFRARED * lp_Inf ) {

  TraceArbo(__func__,0,"init infrared") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Inf->inf_mutex ) ;
                                     lp_Inf->inf_log      = INFRARED_LOG ;
                                     lp_Inf->inf_lock     = INFRARED_LOCK ;
                                     lp_Inf->inf_unlock   = INFRARED_UNLOCK ;
                                     lp_Inf->inf_display  = INFRARED_DISPLAY ;
                                     lp_Inf->inf_loglevel = 0 ; 
                                     lp_Inf->inf_file     = NULL ;
  gettimeofday (                   & lp_Inf->inf_tval, NULL ) ;

  return ;
}
/*****************************************************************************************
* @fn     : INFRARED_OPEN
* @author : s.gravois
* @brief  : Ouverture des flux IR a l aide du paquet LIRC
* @param  : struct lirc_config *gp_LircConfig
* @date   : 2022-04-12 creation entete de la fonction au format doxygen
* @date   : 2022-04-12 mutex_lock sur gp_Dat->dat_act
* @todo   : 
*****************************************************************************************/

int INFRARED_OPEN(STR_EXT_LIRC_CONFIG *gp_LircConfig) {

  int retour = 0 ;
  
  if( lirc_init("lirc",1)==-1)                      retour = 1 ;
  if( lirc_readconfig( NULL, &gp_LircConfig, NULL) != 0 )  retour = 2 ;  
  
  return retour ;
}
//====================================================================================
  
void INFRARED_CLOSE(STR_EXT_LIRC_CONFIG *gp_LircConfig) {
  
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
      
      DATAS_ACTION_BUF_TO_DAT( gp_Dat, gp_Cod->cod_out_act[i_indice_code] ) ;
    }

    if ( k > 0 && \
         i_indice_code < CODES_CODE_NB_CODES && \
         i_indice_code <= IR_CODE_REPETE_AUTORISE_MAX && \
         i_indice_code >= IR_CODE_REPETE_AUTORISE_MIN ) {

      DATAS_ACTION_BUF_TO_DAT( gp_Dat, gp_Cod->cod_out_act[i_indice_code] ) ;
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
