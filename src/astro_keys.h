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

  pthread_mutex_t key_mutex ;

  char key_symbole     [ CONFIG_TAILLE_BUFFER_32 ] ;
  char key_nombre      [ CONFIG_TAILLE_BUFFER_32 ] ;
  char key_mot         [ CONFIG_TAILLE_BUFFER_32 ] ;
  char key_phrase      [ CONFIG_TAILLE_BUFFER_32 ] ;
  char key_premier     [ CONFIG_TAILLE_BUFFER_32 ] ;
  char key_valider     [ CONFIG_TAILLE_BUFFER_32 ] ;
  char key_menu        [ CONFIG_TAILLE_BUFFER_32 ] ;
  char key_actions     [ KEYS_ACTIONS_SIZE       ][ CONFIG_TAILLE_BUFFER_32 ]  ;
  char key_validations [ KEYS_VALIDATIONS_SIZE   ][ CONFIG_TAILLE_BUFFER_32 ]  ;
  int  key_appui_en_cours ;
  int  key_mot_en_cours ;
  int  key_phrase_lue ;
} ;

void   KEYS_INIT                  ( STRUCT_KEYS * )  ;
void   KEYS_INPUTS_GESTION_APPUIS ( STRUCT_SUIVI * , STRUCT_KEYS *) ;
void   KEYS_AFFICHER              ( STRUCT_KEYS *) ;

#endif

