#define _GNU_SOURCE

#include <stat.h>

void STAT_MESURE_PULSATIONS_INJECTEES(SUIVI * gp_Sui) {

  int i,incr=0 ;
  // calcul de la valeur moyenne du tableau des valeurs mesurees
	
	gp_Sui->Ias=0 ;
	for(i=0;i<CONFIG_ASS;i++) gp_Sui->Ias += gp_Sui->Iat[i] ;
	gp_Sui->Ias = gp_Sui->Ias / CONFIG_ASS ;
	
	// if ( incr > CONFIG_ASS )   gp_Sui->Tac = gp_Sui->Tacc * gp_Sui->Ias / gp_Sui->Fa_mic   ;
	if ( incr > CONFIG_ASS )   gp_Sui->Tac = gp_Sui->Tacc ;
	//gp_Sui->Tac = pow(gp_Sui->Tac, 1.02) ;
	
	// roulement du tableau des valeurs mesurees
	
	for(i=0;i<(CONFIG_ASS-1);i++) gp_Sui->Iat[i] = gp_Sui->Iat[i+1] ;  
	gp_Sui->Iat[CONFIG_ASS-1] = gp_Sui->Ia - gp_Sui->Ia_prec   ;
	
  gp_Sui->Ia_prec = gp_Sui->Ia ;
	
	gp_Sui->Ihs=0 ;
	for(i=0;i<CONFIG_ASS;i++) gp_Sui->Ihs += gp_Sui->Iht[i] ;
	gp_Sui->Ihs = gp_Sui->Ihs / CONFIG_ASS ;
	
	// if ( incr > CONFIG_ASS )   gp_Sui->Thc = gp_Sui->Thcc * gp_Sui->Ihs / gp_Sui->Fh_mic ;
	if ( incr > CONFIG_ASS )   gp_Sui->Thc = gp_Sui->Thcc ;
	//gp_Sui->Thc = pow(gp_Sui->Thc, 1.02) ;
	
	// roulement du tableau des valeurs mesurees
	
	for(i=0;i<(CONFIG_ASS-1);i++) gp_Sui->Iht[i] = gp_Sui->Iht[i+1] ;  
	gp_Sui->Iht[CONFIG_ASS-1] = gp_Sui->Ih - gp_Sui->Ih_prec ;
	
  gp_Sui->Ih_prec = gp_Sui->Ih ;

}
