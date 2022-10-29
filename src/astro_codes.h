/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-06-01  | * creation
# 2022-06-01  | * rapatriement code depuis astro_config.h
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_CODES_H
#define ASTRO_CODES_H

#include "astro_global.h"

typedef enum {
    
  CODES_POS_IN_TERM=0,
  CODES_POS_IN_LIRC,
  CODES_POS_OUT_ACT  
}
t_en_Codes_Pos ;

struct STR_CODES {
  
  char in_term [ CODES_CODE_NB_CODES][CODES_CODE_BUFFER_SIZE] ;
  char in_lirc [ CODES_CODE_NB_CODES][CODES_CODE_BUFFER_SIZE] ;
  char out_act [ CODES_CODE_NB_CODES][CODES_CODE_BUFFER_SIZE] ;
} ;

typedef struct STR_CODES STRUCT_CODES ;

/*----------------------------------------------*/
/* DEFINITION DES CODES d INTERRACTION en INPUT */ 
/*----------------------------------------------*/

/* Les 3 champs du tableau suivant sont respectivement :

- le numero du code dans l interraction TERMIOS
- le nom    du code dans l'interraction LIRC
- le nom    du code dans le code C (action correspondante)
*/

/*------------------------------------------  */
/* TAILLE TABLEAU 70 = CODES_CODE_NB_CODES    */
/* TODO : redefinir si besoin (doubler a 100) */
/*------------------------------------------  */

void   CODES_INIT_CODE ( STRUCT_CODES *, int, const char * [][CODES_NB_IN_OUT] ) ;
void   CODES_INIT      ( STRUCT_CODES *) ;

#endif