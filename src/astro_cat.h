/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 0/045/2021  | * ajout entete
#               * suppression CAT_REP
# -------------------------------------------------------------- 
*/

#ifndef  ASTRO_CAT_H
#define  ASTRO_CAT_H

#include "astro_global.h"

// FIN INCLUDES =====================================
// FIXME : 20210501 : creation entete de la fonction au format doxygen ligne suivante 
// FIXME : inutile avec passage par /etc/config.txt pour lecture parametres
// FIXME : de repertoires et de fichiers 

// #define   CAT_REP            "/root/catalogues"

#define   CAT_NGC_TXT        "ngc.csv"
#define   CAT_ETO_TXT        "eto.csv"

#define   CAT_NGC_DEC_TXT    "ngc.csv.dec"
#define   CAT_ETO_DEC_TXT    "eto.csv.dec"

#define   CAT_NB_LIGNES          10000
#define   CAT_NB_COLONNES        6
#define   CAT_TAILLE_BUFFER      100
#define   CAT_TAILLE_BUFFER_256  256

/* TODO : finir de modifier le code pour passer partout par la structure STRUCT_CAT */

typedef struct STR_CAT STRUCT_CAT ;

struct STR_CAT {
  void          (*cat_lock)       ( STRUCT_CAT *) ;
  void          (*cat_unlock)     ( STRUCT_CAT *) ;  
  void          (*cat_log)        ( STRUCT_CAT *) ;
  void          (*cat_display)    ( STRUCT_CAT *) ;
  pthread_mutex_t cat_mutex ;
  STR_EXT_TIMEVAL cat_tval ; 
  FILE           *cat_file ; 
  int             cat_loglevel ;
  char            cat_dis_for [ CONFIG_TAILLE_BUFFER_256 ] ;
  char            cat_dis_cmd [ CONFIG_TAILLE_BUFFER_256 ] ;

  char            cat_dat [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER] ;
  char            cat_dec [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER] ;
  char            cat_path [CAT_TAILLE_BUFFER_256] ;
} ;

void      CAT_INIT          ( STRUCT_CAT * ) ;
void      CAT_READ          ( STRUCT_CAT * , char * ) ;
void      CAT_FIND          ( STRUCT_CAT * , STRUCT_ASTRE *, int ) ;
void      CAT_ZONE          ( STRUCT_CAT * , STRUCT_ASTRE *, double ) ;
void      CAT_DISPLAY_DAT   ( STRUCT_CAT * ) ;
void      CAT_DISPLAY_DEC   ( STRUCT_CAT * ) ;

void      CAT_FORMAT_DECIMAL_NGC ( STRUCT_CAT *, char * ) ;
void      CAT_FORMAT_DECIMAL_ETO ( STRUCT_CAT *, char * ) ;

int       CAT_FIN_MOT      ( char ) ;

#endif
