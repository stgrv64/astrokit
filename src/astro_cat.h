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

#define   CAT_NB_LIGNES      10000
#define   CAT_NB_COLONNES    6
#define   CAT_TAILLE_BUFFER  100

/* TODO : finir de modifier le code pour passer partout par la structure STRUCT_CAT */

struct STR_CAT {
 pthread_mutex_t cat_mutex ;
 char            cat_dat [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER] ;
 char            cat_dec [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER] ;
} ;
typedef struct STR_CAT STRUCT_CAT ;

char g_c_cat         [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
char g_c_cat_dec     [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
char g_c_cat_eto     [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
char g_c_cat_eto_dec [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;

/* TODO : creer une structure de catalogues STRUCT_CAT */ 

void      CAT_INIT          ( STRUCT_CAT * ) ;
void      CAT_READ          ( STRUCT_CAT * , char * ) ;
void      CAT_FIND          ( STRUCT_CAT * , STRUCT_ASTRE *) ;
void      CAT_ZONE          ( STRUCT_CAT * , STRUCT_ASTRE *, double ) ;
void      CAT_DISPLAY_DAT  ( STRUCT_CAT * ) ;
void      CAT_DISPLAY_DEC  ( STRUCT_CAT * ) ;

void      CAT_FORMAT_DECIMAL_NGC ( STRUCT_CAT *, char * ) ;
void      CAT_FORMAT_DECIMAL_ETO ( STRUCT_CAT *, char * ) ;

int       CAT_FIN_MOT      ( char ) ;

#endif
