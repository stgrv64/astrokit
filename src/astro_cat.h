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
#define   CAT_NB_LIGNES      10000
#define   CAT_NB_COLONNES    6
#define   CAT_TAILLE_BUFFER  100

/* TODO : finir de modifier le code pour passer partout par la structure STRUCT_CAT */

struct STR_CAT {
 char c_cat         [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
 char c_cat_dec     [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
 char c_cat_eto     [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
 char c_cat_eto_dec [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
} ;

char g_c_cat         [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
char g_c_cat_dec     [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
char g_c_cat_eto     [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
char g_c_cat_eto_dec [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;

/* TODO : creer une structure de catalogues STRUCT_CAT */ 

void      CAT_INIT          (STRUCT_CAT * ) ;

int       CAT_FIN_MOT       ( char c) ;
void      CAT_AFFICHER      ( char [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;
void      CAT_READ          ( char * , char [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;
void      CAT_FIND          ( STRUCT_ASTRE *, char [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;
void      CAT_FIND_OLD      ( char * ,STRUCT_ASTRE *,char *) ;
void      CAT_FORMAT_DECIMAL_NGC( char * , char [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER], \
                           char [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;
void      CAT_FORMAT_DECIMAL_ETO( char * , char     [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER], \
                           char [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;
void      CAT_ZONE(STRUCT_ASTRE *gp_Ast, double deg, char [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;

#endif
