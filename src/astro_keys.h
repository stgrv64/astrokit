/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-10-07  | * creation
# 2022-10-07  | * rapatriement code depuis autres fichiers
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_KEYS_H
#define ASTRO_KEYS_H

#include "astro_global.h"

#define KEYS_ACTIONS_SIZE     7
#define KEYS_VALIDATIONS_SIZE 10

struct STR_KEYS {

  char symbole  [CONFIG_TAILLE_BUFFER_32] ;
  char nombre   [CONFIG_TAILLE_BUFFER_32] ;
  char mot      [CONFIG_TAILLE_BUFFER_32] ;
  char phrase   [CONFIG_TAILLE_BUFFER_32] ;
  char premier  [CONFIG_TAILLE_BUFFER_32] ;
  char valider  [CONFIG_TAILLE_BUFFER_32] ;
  char menu     [CONFIG_TAILLE_BUFFER_32] ;
  
  char actions[     KEYS_ACTIONS_SIZE     ][ CONFIG_TAILLE_BUFFER_32 ]  ;
  char validations[ KEYS_VALIDATIONS_SIZE ][ CONFIG_TAILLE_BUFFER_32 ]  ;
  
  int  appui_en_cours ;
  int  mot_en_cours ;
  int  phrase_lue ;
  int  premier_en_cours ;
} ;

void   KEYS_INIT                  ( STRUCT_KEYS * )  ;
void   KEYS_INPUTS_GESTION_APPUIS ( STRUCT_SUIVI * , STRUCT_KEYS *) ;
void   KEYS_AFFICHER              ( STRUCT_KEYS *) ;

#endif

