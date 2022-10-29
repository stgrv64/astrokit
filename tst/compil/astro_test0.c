/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
--------------------------------------------------------------
date           | commentaires 
--------------------------------------------------------------
octobre 2022 : creation
-------------------------------------------------------------- */

#include "astro_test0.h"

/* Programme test inclusion de structure dans une autre comme 
  membre avec pointeur */

/* Montrant que l'allocation statique n'est pas appropriee */
  
STRUCT_TEST1 * gp_t1 , g_t1 ;
STRUCT_TEST0 * gp_t0 , g_t0 ; 

/*****************************************************************************************
* @fn     : FctTest0_0
* @author : s.gravois
* @brief  : Cette fonction initialise les structures de test STRUCT_TEST0 * et STRUCT_TEST1 *
* @param  : STRUCT_TEST0 * lp_t0 
* @param  : STRUCT_TEST1 * lp_t1 
* @date   : 2022-10-29 creation 
* @todo   : 
*****************************************************************************************/

void FctTest0_0 (STRUCT_TEST0 * lp_t0 , STRUCT_TEST1 * lp_t1 ) {

  STRUCT_TEST1 * lp_t1_1 , l_t1_1 ;
  lp_t1_1 = & l_t1_1 ; 
  
  lp_t0->pst_t1 = lp_t1 ;
  
  lp_t0->pst_t1->a = 1 ;
  lp_t0->pst_t1->b = 1 ;

  lp_t0->a = 1 ;
  lp_t0->b = 1 ;
  
  printf("t0 : %d %d  t1 : %d %d \n", gp_t0->a , gp_t0->b, gp_t1->a , gp_t1->b ) ;
}

/*****************************************************************************************
* @fn     : FctTest0_1
* @author : s.gravois
* @brief  : Cette fonction initialise les structures de test STRUCT_TEST0 * et STRUCT_TEST1 *
* @param  : STRUCT_TEST0 * lp_t0 
* @param  : STRUCT_TEST1 * lp_t1 
* @date   : 2022-10-29 creation 
* @todo   : 
*****************************************************************************************/

void FctTest0_1 (STRUCT_TEST0 * lp_t0 , STRUCT_TEST1 * lp_t1) {

  STRUCT_TEST1 * lp_t1_1 , l_t1_1 ;
  lp_t1_1 = & l_t1_1 ; 
  
  lp_t0->pst_t1 = lp_t1_1 ;
  
  lp_t0->pst_t1->a = 2 ;
  lp_t0->pst_t1->b = 2 ;

  lp_t0->a = 2 ;
  lp_t0->b = 2 ;
  
  printf("t0 : %d %d  t1 : %d %d \n", gp_t0->a , gp_t0->b, gp_t1->a , gp_t1->b ) ;
}

/*****************************************************************************************
* @fn     : main
* @author : s.gravois
* @brief  : Entree du programme principal
* @param  : int     argc
* @param  : char ** argv
* @date   : 2022-10-29 creation 
* @todo   : 
*****************************************************************************************/

void main(int argc , char ** argv) {

  gp_t0 = & g_t0 ;
  gp_t1 = & g_t1 ;
  
  FctTest0_0( gp_t0, gp_t1 ) ;
  
  printf("t0 : %d %d  t1 : %d %d \n", gp_t0->a , gp_t0->b, gp_t1->a , gp_t1->b ) ;
  
  FctTest0_1( gp_t0, gp_t1 ) ;
  
  printf("t0 : %d %d  t1 : %d %d \n", gp_t0->a , gp_t0->b, gp_t1->a , gp_t1->b ) ;
}


/*****************************************************************************************
* @fn     : NULL
* @author : s.gravois
* @brief  : FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICH
* @param  : 
* @date   : 2022-10-29 creation 
* @todo   : 
*****************************************************************************************/


