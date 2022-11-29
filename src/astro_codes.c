/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
--------------------------------------------------------------
date           | commentaires 
--------------------------------------------------------------
octobre 2022 :
  * creation
  * recuperation du contenu des fonctions depuis config.c               
-------------------------------------------------------------- */

#include "astro_codes.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/*****************************************************************************************
* @fn     : CODES_INIT_CODE
* @author : s.gravois
* @brief  : Cette fonction initialise un code (appelle par CODES_INIT)
* @param  : STRUCT_VOUTE *gp_Vou
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : TODO : passer par une lecture de parametre dans config.txt pour \
*           pourcentage_tempo et vou_calibration_delta_t
*****************************************************************************************/

void CODES_INIT_CODE( \

  STRUCT_CODES * lp_Cod, \
  int          li_pos, \
  const char * gl_char_Codes[][CODES_NB_IN_OUT] ) {

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1("li_pos %d %-16s %-16s %-16s", \
    li_pos, \
    gl_char_Codes[li_pos][ CODES_POS_IN_TERM ], \
    gl_char_Codes[li_pos][ CODES_POS_IN_LIRC], \
    gl_char_Codes[li_pos][ CODES_POS_OUT_ACT] \
  );

  strcpy( lp_Cod->cod_in_term[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_IN_TERM ] ) ;
  strcpy( lp_Cod->cod_in_lirc[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_IN_LIRC] ) ;
  strcpy( lp_Cod->cod_out_act[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_OUT_ACT] ) ;
}
/*****************************************************************************************
* @fn     : CODES_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure des codes *STRUCT_CODES
* @brief  : etablie la correspondance entre les KEY du fichier lircd.conf (partie LIRC)
* @brief  : et les codes utilises dans le programme principal (car pas de hachage en c)
* @brief  : etablie aussi la correspondance entre les KEY du clavier termios (partie TERMIOS)
* @brief  : et les codes utilises dans le programme principal (code_action)
* @param  : STRUCT_CODES *lp_Cod
* @date   : 2022-03-21 creation entete
* @todo   : modifier : completer avec une fonction de hachage (regarder si API sur net)
*****************************************************************************************/

void CODES_INIT(STRUCT_CODES *lp_Cod) {

  int i_pos ;
  
  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Cod->cod_mutex ) ;

  for( i_pos=0 ; i_pos<CODES_CODE_NB_CODES ; i_pos++ ) memset( lp_Cod->cod_out_act[i_pos], CONFIG_ZERO_CHAR, sizeof(lp_Cod->cod_out_act[i_pos]) ) ;
  for( i_pos=0 ; i_pos<CODES_CODE_NB_CODES ; i_pos++ ) memset( lp_Cod->cod_in_lirc[i_pos],  CONFIG_ZERO_CHAR, sizeof(lp_Cod->cod_in_lirc[i_pos]) ) ;

  for( i_pos=0 ; i_pos<CODES_CODE_NB_CODES ; i_pos++ ) {
    memset( lp_Cod->cod_in_term[i_pos],  CONFIG_ZERO_CHAR, sizeof(lp_Cod->cod_in_term[i_pos]) ) ;
    memset( lp_Cod->cod_in_lirc[i_pos],  CONFIG_ZERO_CHAR, sizeof(lp_Cod->cod_in_lirc[i_pos]) ) ;
    memset( lp_Cod->cod_out_act[i_pos],  CONFIG_ZERO_CHAR, sizeof(lp_Cod->cod_out_act[i_pos]) ) ;
  }
  for( i_pos=0 ; i_pos<CODES_CODE_NB_CODES ; i_pos++ ) {

    CODES_INIT_CODE( lp_Cod, i_pos, gc_hach_codes  ) ; 
  }
  // FIXME :  ATTENTION !!! 
  // le nom des lp_Cod codes en MAJUSCULE servent a definir des actions dans config.c et le reste du programme

}