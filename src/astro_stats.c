/* #define _GNU_SOURCE */

#include "astro_stats.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/*****************************************************************************************
* @fn     : STATS_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_STATS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void STATS_LOCK ( STRUCT_STATS * lp_Sta) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Sta->sta_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : STATS_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_STATS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void STATS_UNLOCK ( STRUCT_STATS * lp_Sta) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Sta->sta_mutex ) ;

  return ;
}

/*****************************************************************************************
* @fn     : STATS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_STATS * 
* @param  : STRUCT_STATS * lp_Sta
* @date   : 2023-01-02 creation 
* @todo   : 
*****************************************************************************************/

void STATS_INIT(STRUCT_STATS * lp_Sta) {

  int i = 0 ;
  
  TraceArbo(__func__,0,"init stats") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Sta->sta_mutex) ;

  lp_Sta->sta_Tac        = 1.0 ;
  lp_Sta->sta_Thc        = 1.0 ;
  lp_Sta->sta_Tacc       = 0.97 ;
  lp_Sta->sta_Thcc       = 0.97 ;
  lp_Sta->sta_Ia         = 0 ;
  lp_Sta->sta_Ih         = 0 ;
  lp_Sta->sta_Ias        = 0 ;
  lp_Sta->sta_Ihs        = 0 ;
  lp_Sta->sta_Ia_prec    = 0 ;
  lp_Sta->sta_Ih_prec    = 0 ;
  
  for(i=0;i<STATS_ASS;i++) {
	  lp_Sta->sta_Iat[i] = 0 ;
  }
  for(i=0;i<STATS_ASS;i++) {
	  lp_Sta->sta_Iht[i] = 0 ;
  }
  return ;
}
/*****************************************************************************************
* @fn     : STAT_MESURE_PULSATIONS_INJECTEES
* @author : s.gravois
* @brief  : Effectue les calculs statistiques sur les mesures
* @param  : STRUCT_STATS *
* @date   : 2022-12-20 creation entete
*****************************************************************************************/

void STAT_MESURE_PULSATIONS_INJECTEES(STRUCT_STATS * lp_Sta) {

	int i,incr=0 ;
	// calcul de la valeur moyenne du tableau des valeurs mesurees
	
	lp_Sta->sta_Ias=0 ;
	for(i=0;i<STATS_ASS;i++) lp_Sta->sta_Ias += lp_Sta->sta_Iat[i] ;
	lp_Sta->sta_Ias = lp_Sta->sta_Ias / STATS_ASS ;

	// if ( incr > STATS_ASS )   lp_Sta->sta_Tac = lp_Sta->sta_Tacc * lp_Sta->sta_Ias / gp_Fre->fre_fa_mic   ;
	if ( incr > STATS_ASS )   lp_Sta->sta_Tac = lp_Sta->sta_Tacc ;
	//lp_Sta->sta_Tac = pow(lp_Sta->sta_Tac, 1.02) ;

	// roulement du tableau des valeurs mesurees

	for(i=0;i<(STATS_ASS-1);i++) lp_Sta->sta_Iat[i] = lp_Sta->sta_Iat[i+1] ;  
	lp_Sta->sta_Iat[STATS_ASS-1] = lp_Sta->sta_Ia - lp_Sta->sta_Ia_prec   ;

	lp_Sta->sta_Ia_prec = lp_Sta->sta_Ia ;

	lp_Sta->sta_Ihs=0 ;
	for(i=0;i<STATS_ASS;i++) lp_Sta->sta_Ihs += lp_Sta->sta_Iht[i] ;
	lp_Sta->sta_Ihs = lp_Sta->sta_Ihs / STATS_ASS ;

	// if ( incr > STATS_ASS )   lp_Sta->sta_Thc = lp_Sta->sta_Thcc * lp_Sta->sta_Ihs / gp_Fre->fre_fh_mic ;
	if ( incr > STATS_ASS )   lp_Sta->sta_Thc = lp_Sta->sta_Thcc ;
	//lp_Sta->sta_Thc = pow(lp_Sta->sta_Thc, 1.02) ;

	// roulement du tableau des valeurs mesurees

	for(i=0;i<(STATS_ASS-1);i++) lp_Sta->sta_Iht[i] = lp_Sta->sta_Iht[i+1] ;  
	lp_Sta->sta_Iht[STATS_ASS-1] = lp_Sta->sta_Ih - lp_Sta->sta_Ih_prec ;

	lp_Sta->sta_Ih_prec = lp_Sta->sta_Ih ;

	return ;
}
