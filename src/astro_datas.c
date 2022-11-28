/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 07/10/2022  | * creation
# -------------------------------------------------------------- 
*/

#include "astro_datas.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/*****************************************************************************************
* @fn     : DATAS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise les champs de la structure STRUCT_DATAS
* @param  : STRUCT_DEVICES *gp_Dev
* @date   : 2022-10-08 creation 
* @todo   : ras
*****************************************************************************************/

void DATAS_INIT ( STRUCT_DATAS * lp_Dat ) {

  int i =0 ;
  
  pthread_mutex_init( & lp_Dat->dat_mutex, NULL ) ;

  memset( lp_Dat, 0, sizeof (STRUCT_DATAS ) ) ;
  strcpy( lp_Dat->dat_acc, ""  ) ;
  strcpy( lp_Dat->dat_bou, ""  ) ;
  strcpy( lp_Dat->dat_inf, ""  ) ;

  return ;
}