
/* Calculate the velocity vector of the earth
 * as it moves in its elliptical orbit.
 * The difference in velocity between this and assuming
 * a circular orbit is only about 1 part in 10**4.
 *
 * Note that this gives heliocentric, not barycentric, velocity.
 *
 * Input is Julian date.  Output left in global array vearth[].
 */

/* Calcule le vecteur vitesse de la terre
  * lors de son déplacement sur son orbite elliptique.
  * La différence de vitesse entre cela et en supposant
  * une orbite circulaire ne représente qu'environ 1 partie sur 10**4.
  *
  * Notez que cela donne une vitesse héliocentrique et non barycentrique.
  *
  * L'entrée est la date julienne. Sortie laissée dans le tableau global vearth[].
  */
 
double jvearth = -1.0;
double vearth[3] = {0.0};

#include "kep.h"

extern struct orbit earth;

/*****************************************************************************************
* @fn     : velearth
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int velearth( J )
double J;
{
	double e[3], p[3], t;
	int i;

	
	#if DEBUG
	double x[3]={0}, A=0, q=0;
	#endif

	TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
	
	if( J == jvearth )
		return(0);

	jvearth = J;

	/* calculate heliocentric position of the earth
	* as of a short time ago.
	*/
	t = 0.005;

	kepler( TDT-t, &earth, e, p );

	for( i=0; i<3; i++ )
		vearth[i] = (rearth[i] - e[i])/t;

	#if DEBUG
	/* Generate display for comparison with Almanac values. */
	for( i=0; i<3; i++ )
		{
		q = vearth[i];
		A += q*q;
		x[i] = q;
		}
	A = sqrt(A);
	precess( x, TDT, 1 );
	Trace1( "Vearth %.6e, X %.6f, Y %.6f, Z %.6f\n", A, x[0], x[1], x[2] );
	#endif
	return(0);
}
