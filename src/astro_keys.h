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

#define MACRO_IF_KEY_MOT_IS(s)  if(!strcmp(gp_Key->key_mot,s))

#define ASTRO_KEYS_LOG(loglevel,nb,fmt, args...) \
do { \
  if (loglevel>=nb) { \
    fprintf(stdout, "\n%-36s -> " fmt, __func__, ##args) ; \
  } \
} \
while (0)

#define KEYS_ACTIONS_SIZE     8
#define KEYS_VALIDATIONS_SIZE 10

typedef struct STR_KEYS STRUCT_KEYS ;

struct STR_KEYS {
  pthread_mutex_t key_mutex ;
  STR_EXT_TIMEVAL key_tval ; 
  FILE           *key_file ; 
  char            key_loglevel ;
  void          (*key_lock)       ( STRUCT_KEYS*) ;
  void          (*key_unlock)     ( STRUCT_KEYS*) ;  
  void          (*key_log)        ( STRUCT_KEYS*) ;
  void          (*key_display)    ( STRUCT_KEYS*) ;
  char            key_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char            key_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;

  char            key_symbole     [ CONFIG_TAILLE_BUFFER_32 ] ;
  char            key_nombre      [ CONFIG_TAILLE_BUFFER_32 ] ;
  char            key_mot         [ CONFIG_TAILLE_BUFFER_32 ] ;
  char            key_phrase      [ CONFIG_TAILLE_BUFFER_32 ] ;
  char            key_premier     [ CONFIG_TAILLE_BUFFER_32 ] ;
  char            key_valider     [ CONFIG_TAILLE_BUFFER_32 ] ;
  char            key_menu        [ CONFIG_TAILLE_BUFFER_32 ] ;
  char            key_actions     [ KEYS_ACTIONS_SIZE       ][ CONFIG_TAILLE_BUFFER_32 ]  ;
  char            key_validations [ KEYS_VALIDATIONS_SIZE   ][ CONFIG_TAILLE_BUFFER_32 ]  ;
  int             key_appui_en_cours ;
  int             key_mot_en_cours ;
  int             key_phrase_lue ;
} ;

void KEYS_INIT                  ( STRUCT_KEYS * ) ;
void KEYS_INPUTS_GESTION_APPUIS ( STRUCT_KEYS * ) ;
void KEYS_RESET_MOT             ( STRUCT_KEYS * ) ;

#endif

