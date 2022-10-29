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

#include "astro_devices.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_OTHER ;

// les variables suivantes servent si un expandeur de port est utilise. Exemple = MCP23017 ou MCP23008

unsigned char MCP_DIR_AZI   ; // numero de port GPIO pour l'horloge de l'azimut
unsigned char MCP_CLK_AZI   ; // numero de port GPIO pour l'horloge de l'azimut
unsigned char MCP_SLP_AZI   ; // numero de port GPIO pour le sleep du controleur du moteur
unsigned char MCP_RST_AZI   ; // numero de port GPIO pour le sleep du controleur du moteur
unsigned char MCP_M2_AZI    ; // numero de port GPIO pour le choix du micro pas
unsigned char MCP_M1_AZI    ; // numero de port GPIO pour le choix du micro pas 
unsigned char MCP_M0_AZI   ;  // numero de port GPIO pour le choix du micro pas

/*****************************************************************************************
* @fn     : DEVICES_INIT
* @author : s.gravois
* @brief  : Cette fonction configure la structure STRUCT_DEVICES *gp_Dev
* @param  : STRUCT_DEVICES *gp_Dev
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : (obsolete) les devices sont lues depuis le fichier de configuration
*****************************************************************************************/

void DEVICES_INIT(STRUCT_DEVICES *gp_Dev) {

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  gp_Dev->use_capteurs    = gp_Dev_Par->par_use_Capteurs ;
  gp_Dev->use_raquette    = gp_Dev_Par->par_use_Raquette ;
  gp_Dev->use_bluetooth   = gp_Dev_Par->par_use_Bluetooth ;
  gp_Dev->use_infrarouge  = gp_Dev_Par->par_use_Infrared ;
  gp_Dev->use_controler   = gp_Dev_Par->par_use_Controler ;
  gp_Dev->use_keyboard    = gp_Dev_Par->par_use_Keyboard ;
  gp_Dev->use_lcd         = gp_Dev_Par->par_use_Lcd ; 
  gp_Dev->init_capteurs = 0 ; 
}

/*****************************************************************************************
* @fn     : DEVICES_AFFICHER_UTILISATION
* @author : s.gravois
* @brief  : Cette fonction affiche le parametrage utilisation des devices 
* @param  : void
* @date   : 2022-03-12 creation
* @date   : 2022-10-08 rapatriement et changement de nom depuis config.c
*****************************************************************************************/

void DEVICES_AFFICHER_UTILISATION (void) {

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("gp_Dev->use_infrarouge = %d",gp_Dev->use_infrarouge) ;
  Trace("gp_Dev->use_capteurs   = %d",gp_Dev->use_capteurs) ;
  Trace("gp_Dev->use_raquette   = %d",gp_Dev->use_raquette) ;
  Trace("gp_Dev->use_bluetooth  = %d",gp_Dev->use_bluetooth) ;
  Trace("gp_Dev->use_keyboard   = %d",gp_Dev->use_keyboard) ;
  Trace("gp_Dev->use_controler  = %d",gp_Dev->use_controler) ;
  Trace("gp_Dev->use_lcd        = %d",gp_Dev->use_lcd) ;

  return ;
}
