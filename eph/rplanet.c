
/* The following program reduces the heliocentric equatorial
 * rectangular coordinates of the earth and object that
 * were computed by kepler() and produces apparent geocentric
 * right ascension and declination.
 */

/* Le programme suivant réduit l'équatoriale héliocentrique
  * coordonnées rectangulaires de la terre et de l'objet qui
  * ont été calculés par kepler() et produit des géocentriques apparents
  * ascension droite et déclinaison.
  */

#include "kep.h"

/*****************************************************************************************
* @fn     : reduce
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int reduce( infos, elemnt, q, e )

TOPOCENTRIC *infos ;
struct orbit *elemnt;	/* orbital elements of q */
double q[], e[];	/* heliocentric coordinates */
{
double p[3], temp[3], polar[3];
double a, b, s;
int i;
double sqrt(), asin(), log();

TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

/* Save the geometric coordinates at TDT
 */
for( i=0; i<3; i++ )
	temp[i] = q[i];

/* Display ecliptic longitude and latitude, precessed to equinox
    of date.  */
if( prtflg )
	lonlat( q, TDT, polar, 1 );

/* Adjust for light time (planetary aberration)
*/
lightt( elemnt, q, e );

/* Find Euclidean vectors between earth, object, and the sun
 */
for( i=0; i<3; i++ )
	p[i] = q[i] - e[i];

angles( p, q, e );

if( prtflg )
	{
	a = 0.0;
	for( i=0; i<3; i++ )
		{
		b = temp[i] - e[i];
		a += b * b;
		}
	a = sqrt(a);
	Trace1( "true geocentric distance %.7f au    ", a ); /* was EO */
	Trace1( "equatorial diameter %.2f\"\n", 2.0*elemnt->sdiam/EO );


/* Calculate visual magnitude.
 * "Visual" refers to the spectrum of visible light.
 * Phase = 0.5(1+pq) = geometric fraction of disc illuminated.
 * where pq = cos( sun-object-earth angle )
 * The magnitude is
 *    V(1,0) + 2.5 log10( SE^2 SO^2 / Phase)
 * where V(1,0) = elemnt->mag is the magnitude at 1au from
 * both earth and sun and 100% illumination.
 */
	a = 0.5 * (1.0 + pq);
/* Fudge the phase for light leakage in magnitude estimation.
 * Note this phase term estimate does not reflect reality well.
 * Calculated magnitudes of Mercury and Venus are inaccurate.
 */
	b = 0.5 * (1.01 + 0.99*pq);
	s = elemnt->mag + 2.1715 * log( EO*SO ) - 1.085*log(b);
	Trace1( "approx. visual magnitude %.1f, phase %.3f\n", s, a );
	}

/* Find unit vector from earth in direction of object
 */
for( i=0; i<3; i++ )
	{
	p[i] /= EO;
	temp[i] = p[i];
	}

if( prtflg )
	{
/* Report astrometric position
 */
	showrd( "Astrometric J2000.0:", p, polar );

/* Also in 1950 coordinates
 */
	precess( temp, B1950, -1 );
	showrd( "Astrometric B1950.0:", temp, polar );
	}

/* Correct position for light deflection
 */
relativity( p, q, e );

/* Correct for annual aberration
 */
annuab( p );

/* Precession of the equinox and ecliptic
 * from J2000.0 to ephemeris date
 */
precess( p, TDT, -1 );

/* Ajust for nutation
 * at current ecliptic.
 */
epsiln( TDT );
nutate( TDT, p );

/* Display the final apparent R.A. and Dec.
 * for equinox of date.
 */
/* (mise en commentaire 2023) */
if( prtflg ) { 
  Trace1 ("%s.", whatconstel (p, TDT));
}

showrd( "  Apparent:", p, polar );

/* Geocentric ecliptic longitude and latitude.  */
if( prtflg ) 
{
	/* (mise en commentaire 2023) */
    Trace1 ("Apparent geocentric ");
    for( i=0; i<3; i++ )
	p[i] *= EO;
    lonlat( p, TDT, temp, 0 );
  }
/* Go do topocentric reductions.
 */
polar[2] = EO;

altaz( infos, polar, UT );

return(0);
}

extern struct orbit *elobject;
extern double robject[];

/*****************************************************************************************
* @fn     : doplanet
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int doplanet(TOPOCENTRIC *infos)
{

	TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
	/* calculate heliocentric position of the object */

	kepler( TDT, elobject, robject, obpolar );

	/* apply correction factors and print apparent place */

	reduce( infos, elobject, robject, rearth );

	return 0;
}
