#define _GNU_SOURCE

#include <stat.h>

void STAT_MESURE_PULSATIONS_INJECTEES(SUIVI* suivi) {

  int i,incr=0 ;
  // calcul de la valeur moyenne du tableau des valeurs mesurees
	
	suivi->Ias=0 ;
	for(i=0;i<CONFIG_ASS;i++) suivi->Ias += suivi->Iat[i] ;
	suivi->Ias = suivi->Ias / CONFIG_ASS ;
	
	// if ( incr > CONFIG_ASS )   suivi->Tac = suivi->Tacc * suivi->Ias / suivi->Fa   ;
	if ( incr > CONFIG_ASS )   suivi->Tac = suivi->Tacc ;
	//suivi->Tac = pow(suivi->Tac, 1.02) ;
	
	// roulement du tableau des valeurs mesurees
	
	for(i=0;i<(CONFIG_ASS-1);i++) suivi->Iat[i] = suivi->Iat[i+1] ;  
	suivi->Iat[CONFIG_ASS-1] = suivi->Ia - suivi->Ia_prec   ;
	
  suivi->Ia_prec = suivi->Ia ;
	
	suivi->Ihs=0 ;
	for(i=0;i<CONFIG_ASS;i++) suivi->Ihs += suivi->Iht[i] ;
	suivi->Ihs = suivi->Ihs / CONFIG_ASS ;
	
	// if ( incr > CONFIG_ASS )   suivi->Thc = suivi->Thcc * suivi->Ihs / suivi->Fh ;
	if ( incr > CONFIG_ASS )   suivi->Thc = suivi->Thcc ;
	//suivi->Thc = pow(suivi->Thc, 1.02) ;
	
	// roulement du tableau des valeurs mesurees
	
	for(i=0;i<(CONFIG_ASS-1);i++) suivi->Iht[i] = suivi->Iht[i+1] ;  
	suivi->Iht[CONFIG_ASS-1] = suivi->Ih - suivi->Ih_prec ;
	
  suivi->Ih_prec = suivi->Ih ;

}
