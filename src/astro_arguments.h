/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# Fichier     | astro_arguments.h
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 19/01/2022  | * ajout entete
#               * ajouts fonctions utilisant getopt
# 11/03/2022  | * ajout fonction ARGUMENTS_MANAGE_REP_HOME
# 07/10/2022  | * refonte complete du code : deplacement code GPIO_TEST_CONTROLER
#  dans gpio.c
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_ARGUMENTS_H
#define ASTRO_ARGUMENTS_H

#include "astro_global.h"

#define ARGUMENTS_VARG_STRING           115
#define ARGUMENTS_VARG_INT              100
#define ARGUMENTS_VARG_CHAR             99
#define ARGUMENTS_FREQ_TRAME_INFRAROUGE 10000000.0
#define ARGUMENTS_FILE_VOUTE            "voute.csv"


//extern void ASTRO_TRAP_MAIN(int sig) ;
/* janvier 2022 : 
  remplacements des arguments de type var glob par void 
  on considere que STRUCT_ASTRE * STRUCT_VOUTE* etc.. sont dorenavant var glob
*/
/* TODO : creer une structure d arguments */

void  ARGUMENTS_HELP                   (int , char** ) ;
void  ARGUMENTS_MANAGE_FACON_CLASSIQUE  (int , char** )  ;
void  ARGUMENTS_MANAGE_GETOPT           (int , char** )  ;
void  ARGUMENTS_MANAGE_REP_HOME         (int , char** ) ;

void  ARGUMENTS_CREATE_VOUTE (void) ;

#endif

//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================


