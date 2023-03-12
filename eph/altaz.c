/* Apply diurnal aberrations and calculate topocentric
 * altitude and azimuth, given the geocentric apparent
 * right ascension and declination.
 *
 * Ephemeris transit times can be obtained by modifying
 * aa.ini to declare TDT = UT.
 * 
 * This program assumes that deltaT, the difference
 * between Ephemeris Time (TDT) and Universal Time (UT),
 * has already been calculated.  The global variables
 * TDT and UT must already have been loaded with the
 * correct values of TDT and UT, respectively.  See periode.c.
 *
 * Inputs are polar coordinates:
 * dist is the true geocentric distance in au.
 * ra and dec are in radians.
 *
 * J is the Julian date in UT measure.
 *
 * AA page B60 and D3.
 */

#include "kep.h"
extern double tlong, tlat, glat;
double azimuth, elevation, refracted_elevation;

/*****************************************************************************************
* @fn     : altaz
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int altaz(infos,  pol, J )
 TOPOCENTRIC *infos ;
 double pol[3];
 double J;
{
  double dec, cosdec, sindec, lha, coslha, sinlha;
  double ra, dist, last, alt, az, coslat, sinlat;
  double N, D, x, y, z, TPI;
  
  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  /* TODO : FIXME : 2022 novembre */ 
  /* C'est ici que les coordonnees equatoriales sont recuperees */

  ra   = pol[0];
  dec  = pol[1];
  dist = pol[2];

  Trace("asc %.2f dec %.2f alt %.2f : ",\
    pol[0], \
    pol[1] * RTD, \
    pol[2] ) ;

  myhms("asc",pol[0]) ;
  Trace("%-10s : %.2f degres", "dec", pol[1] * RTD) ;

  TPI = 2.0*PI;
  
  /* local apparent sidereal time, seconds converted to radians
   */
  last = sidrlt( J, tlong ) * DTR/240.0;
  lha = last - ra; /* local hour angle, radians */
  if( prtflg )
  	{
  	// Trace1( "Local apparent sidereal time " );
  	hms( last );
  	// Trace1( "\n" );
  	}
  /* Display rate at which ra and dec are changing
   */
  /*
   *if( prtflg )
   *	{
   *	x = RTS/24.0;
   *	N = x*dradt;
   *	D = x*ddecdt;
   *	if( N != 0.0 )
   *		Trace1( "dRA/dt %.2f\"/h, dDec/dt %.2f\"/h\n", N, D );
   *	}
   */
  
  diurab( last, &ra, &dec );
  /* Do rise, set, and transit times
     trnsit.c takes diurnal parallax into account,
     but not diurnal aberration.  */
  lha = last - ra;
  trnsit( J, lha, dec );
  
  /* Diurnal parallax
   */
  diurpx( last, &ra, &dec, dist );
  
  /* Diurnal aberration
   */
  /*diurab( last, &ra, &dec );*/
  
  /* Convert ra and dec to altitude and azimuth
   */
  cosdec = cos(dec);
  sindec = sin(dec);
  lha = last - ra;
  coslha = cos(lha);
  sinlha = sin(lha);
  
  /* Use the geodetic latitude for altitude and azimuth */
  x = DTR * glat;
  coslat = cos(x);
  sinlat = sin(x);
  
  N = -cosdec * sinlha;
  D =  sindec * coslat  -  cosdec * coslha * sinlat;
  az = RTD * zatan2( D, N );
  alt = sindec * sinlat  +  cosdec * coslha * coslat;
  alt = RTD * asin(alt);
  
  /* Store results */
  azimuth = az;
  elevation = alt; /* Save unrefracted value. */
  
  /* Correction for atmospheric refraction
   * unit = degrees
   */
  D = refrac( alt );
  alt += D;
  refracted_elevation = alt;
  
  /* Convert back to R.A. and Dec.
   */
  y = sin(DTR*alt);
  x = cos(DTR*alt);
  z = cos(DTR*az);
  
  sinlha = -x * sin(DTR*az);
  coslha = y*coslat - x*z*sinlat;
  sindec = y*sinlat + x*z*coslat;
  lha = zatan2( coslha, sinlha );
  
  y = ra; /* save previous values, before refrac() */
  z = dec;
  dec = asin( sindec );
  ra = last - lha;
  y = ra - y; /* change in ra */
  while( y < -PI )
  	y += TPI;
  while( y > PI )
  	y -= TPI;
  y = RTS*y/15.0;
  z = RTS*(dec - z);

  if( prtflg )
  	{
  	//Trace1( "atmospheric refraction %.3f deg  dRA %.3fs dDec %.2f\"\n", D, y, z );
  	//Trace1( "Topocentric:  Altitude %.3f deg, ", alt );
  	//Trace1( "Azimuth %.3f deg\n", az );
    
    infos->alt = alt / RTD ;
    infos->azi = az / RTD ;
    infos->asc = ra ;
    infos->dec = dec ;
  
    Trace("alt %.2f azi %.2f",infos->alt * RTD , infos->azi * RTD );

  	hms( ra );
  	dms( dec );
  
  }
  return(0);
}
