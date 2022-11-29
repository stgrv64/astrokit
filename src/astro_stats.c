/* #define _GNU_SOURCE */

#include "astro_stats.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

void STAT_MESURE_PULSATIONS_INJECTEES(STRUCT_SUIVI * gp_Sui) {

  int i,incr=0 ;
  // calcul de la valeur moyenne du tableau des valeurs mesurees
	
	gp_Sta->sta_Ias=0 ;
	for(i=0;i<STATS_ASS;i++) gp_Sta->sta_Ias += gp_Sta->sta_Iat[i] ;
	gp_Sta->sta_Ias = gp_Sta->sta_Ias / STATS_ASS ;
	
	// if ( incr > STATS_ASS )   gp_Sta->sta_Tac = gp_Sta->sta_Tacc * gp_Sta->sta_Ias / gp_Fre->fre_fa_mic   ;
	if ( incr > STATS_ASS )   gp_Sta->sta_Tac = gp_Sta->sta_Tacc ;
	//gp_Sta->sta_Tac = pow(gp_Sta->sta_Tac, 1.02) ;
	
	// roulement du tableau des valeurs mesurees
	
	for(i=0;i<(STATS_ASS-1);i++) gp_Sta->sta_Iat[i] = gp_Sta->sta_Iat[i+1] ;  
	gp_Sta->sta_Iat[STATS_ASS-1] = gp_Sta->sta_Ia - gp_Sta->sta_Ia_prec   ;
	
  gp_Sta->sta_Ia_prec = gp_Sta->sta_Ia ;
	
	gp_Sta->sta_Ihs=0 ;
	for(i=0;i<STATS_ASS;i++) gp_Sta->sta_Ihs += gp_Sta->sta_Iht[i] ;
	gp_Sta->sta_Ihs = gp_Sta->sta_Ihs / STATS_ASS ;
	
	// if ( incr > STATS_ASS )   gp_Sta->sta_Thc = gp_Sta->sta_Thcc * gp_Sta->sta_Ihs / gp_Fre->fre_fh_mic ;
	if ( incr > STATS_ASS )   gp_Sta->sta_Thc = gp_Sta->sta_Thcc ;
	//gp_Sta->sta_Thc = pow(gp_Sta->sta_Thc, 1.02) ;
	
	// roulement du tableau des valeurs mesurees
	
	for(i=0;i<(STATS_ASS-1);i++) gp_Sta->sta_Iht[i] = gp_Sta->sta_Iht[i+1] ;  
	gp_Sta->sta_Iht[STATS_ASS-1] = gp_Sta->sta_Ih - gp_Sta->sta_Ih_prec ;
	
  gp_Sta->sta_Ih_prec = gp_Sta->sta_Ih ;

}
