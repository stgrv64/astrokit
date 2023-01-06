
/* Sun - object - earth angles and distances.
 * q (object), e (earth), and p (q minus e) are input vectors.
 * The answers are posted in the following global locations:
 */

#include "kep.h"

double FAR SE;	/* earth-sun distance */
double FAR SO;	/* object-sun distance */
double FAR EO;	/* object-earth distance */

double FAR pq;	/* cosine of sun-object-earth angle */
double FAR ep;	/* -cosine of sun-earth-object angle */
double FAR qe;	/* cosine of earth-sun-object angle */

/*****************************************************************************************
* @fn     : angles
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int angles( p, q, e )
double p[], q[], e[];
{
double a, b, s;
int i;

TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

EO = 0.0;
SE = 0.0;
SO = 0.0;
pq = 0.0;
ep = 0.0;
qe = 0.0;
for( i=0; i<3; i++ )
	{
	a = e[i];
	b = q[i];
	s = p[i];
	EO += s * s;
	SE += a * a;
	SO += b * b;
	pq += s * b;
	ep += a * s;
	qe += b * a;
	}
EO = sqrt(EO); /* Distance between Earth and object */
SO = sqrt(SO); /* Sun - object */
SE = sqrt(SE); /* Sun - earth */
/* Avoid fatality: if object equals sun, SO is zero.  */
if( SO > 1.0e-12 )
	{
	pq /= EO*SO;	/* cosine of sun-object-earth */
	qe /= SO*SE;	/* cosine of earth-sun-object */
	}
ep /= SE*EO;	/* -cosine of sun-earth-object */
return(0);
}

