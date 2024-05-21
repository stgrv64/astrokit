/*  Utility programs for unit conversions and calendar
 */

/*double PI = 3.14159265358979323846;*/
#include "kep.h"

#if __STDC__
double caltoj (long, int, double);
#else
double caltoj();
#endif

char *intfmt = "%d";
char *lngfmt = "%ld";
char *dblfmt = "%lf";
char *strfmt = "%s";

// ================================================================================
/* Display Right Ascension and Declination
 * from input equatorial rectangular unit vector.
 * Output vector pol[] contains R.A., Dec., and radius. */
// ================================================================================

/*****************************************************************************************
* @fn     : showrd
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int showrd( msg, p, pol ) char *msg; double p[], pol[]; {

  double x, y, r;
  int i;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  r = 0.0;
  
  for( i=0; i<3; i++ ) {
    x = p[i];
    r += x * x;
  }
  r = sqrt(r);
  x = zatan2( p[0], p[1] );
  y = asin( p[2]/r );

  pol[0] = x;
  pol[1] = y; 
  pol[2] = r;

  if (prtflg != 0) {
    
    Trace1( "%s  R.A. ", msg );
    
    hms( x );
    
    Trace1( "Dec. " );
    
    dms( y );
  }
  return(0);
}

// ================================================================================
/* Display magnitude of correction vector
 * in arc seconds
 */
// ================================================================================

/*****************************************************************************************
* @fn     : showcor
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int showcor( strng, p, dp ) char *strng; double p[], dp[]; {
  double p1[3], dr, dd;
  int i;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if( prtflg == 0 ) return(0);
  for( i=0; i<3; i++ ) p1[i] = p[i] + dp[i];

  deltap( p, p1, &dr, &dd );
  Trace1( "%s dRA %.3fs dDec %.2f\"", strng, RTS*dr/15.0, RTS*dd );
  
return(0);
}
// ================================================================================
/* Radians to degrees, minutes, seconds
 */
// ================================================================================

/*****************************************************************************************
* @fn     : dms
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int dms( x ) double x; {
  double s;
  int d, m;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  s = x * RTD;

  if( s < 0.0 ) {
    s = -s;
  }

  d = (int) s;
  s -= d;
  s *= 60;
  m = (int) s;
  s -= m;
  s *= 60;

  Trace1( "%d %d %.2f", d, m, s );

  return(0);
}
// ================================================================================
/* Radians to hours, minutes, seconds
 */
// ================================================================================

#define RTOH (12.0/PI)

/*****************************************************************************************
* @fn     : myhms
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int myhms( char * s_message, double x ) {
 int h, m;
 long sint, sfrac;
 double s;

 s = x * RTOH;
 if( s < 0.0 ) s += 24.0;
 h = (int) s;
 s -= h;
 s *= 60;
 m = (int) s;
 s -= m;
 s *= 60;

 /* Handle shillings and pence roundoff. */

 sfrac = (long) (1000.0 * s + 0.5);

 if( sfrac >= 60000L ) {
  sfrac -= 60000L;
  m += 1;
  
  if( m >= 60 ) {
   m -= 60;
   h += 1;
  }
 }

 sint = sfrac / 1000;
 sfrac -= sint * 1000;
 
 Trace1( "%-10s : %.02d h %.02d m %.02ds  ", s_message, h, m, (int)s );
 
return(0);
}
/*****************************************************************************************
* @fn     : hms
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int hms( x ) double x; {
 int h, m;
 long sint, sfrac;
 double s;

TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

 s = x * RTOH;
 if( s < 0.0 ) s += 24.0;
 h = (int) s;
 s -= h;
 s *= 60;
 m = (int) s;
 s -= m;
 s *= 60;

 /* Handle shillings and pence roundoff. */

 sfrac = (long) (1000.0 * s + 0.5);

 if( sfrac >= 60000L ) {
  sfrac -= 60000L;
  m += 1;
  
  if( m >= 60 ) {
   m -= 60;
   h += 1;
  }
 }

 sint = sfrac / 1000;
 sfrac -= sint * 1000;
 
 Trace1( "%.02d h %.02d m %.02ds  ", h, m, (int)s );
 
return(0);
}
// ================================================================================
/*		julian.c
 *
 * This program calculates Julian day number from calendar
 * date, and the date and day of the week from Julian day.
 * The Julian date is double precision floating point
 * with the origin used by astronomers.  The calendar output
 * converts fractions of a day into hours, minutes, and seconds.
 * There is no year 0.  Enter B.C. years as negative; i.e.,
 * 2 B.C. = -2.
 *
 * The approximate range of dates handled is 4713 B.C. to
 * 54,078 A.D.  This should be adequate for most applications.
 *
 * B.C. dates are calculated by extending the Gregorian sequence
 * of leap years and century years into the past.  This seems
 * the only sensible definition, but I don't know if it is
 * the official one.
 *
 * Note that the astronomical Julian day starts at noon on
 * the previous calendar day.  Thus at midnight in the morning
 * of the present calendar day the Julian date ends in .5;
 * it rolls over to tomorrow at noon today.
 *
 * The month finding algorithm is attributed to Meeus.
 *
 * - Steve Moshier
 */
// ================================================================================

char *months[12] = {
"January",
"February",
"March",
"April",
"May",
"June",
"July",
"August",
"September",
"October",
"November",
"December"
};

char *days[7] = {
"Sunday",
"Monday",
"Tuesday",
"Wednesday",
"Thursday",
"Friday",
"Saturday"
};

long cyear = 1986L;
extern long cyear;
static int month = 1;
static double day = 1.0;
short yerend = 0;
extern short yerend;

/*****************************************************************************************
* @fn     : zgetdate_moi
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

double zgetdate_moi(int yearr, int monthh, int dayy) {

  double J;
  char cmd[16] ;
  
  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset    ( cmd, 0,sizeof(cmd)) ;
  sprintf   ( cmd, "%d",yearr);
  getnum_moi( cmd, &cyear, lngfmt );
  
  memset    ( cmd, 0,sizeof(cmd)) ;
  sprintf   ( cmd, "%d",monthh);
  getnum_moi( cmd, &month, intfmt);
  
  memset    ( cmd, 0,sizeof(cmd)) ;
  sprintf   ( cmd, "%d",dayy);
  getnum_moi( cmd, &day, dblfmt );
  
  J = caltoj(cyear,month,day);

  return(J);
}


/*****************************************************************************************
* @fn     : zgetdate
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

double zgetdate() {

  double J;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /* Get operator to type in a date. */

  getnum( "Calendar date: Year", &cyear, lngfmt );

  if( (cyear > 53994L) || (cyear < -4713L) ) { Trace1( "Year out of range.\n" ); goto err; }
  if( cyear == 0 )                           { Trace1( "There is no year 0.\n" ); err:	J = 0.0; goto pdate; }

  getnum( "Month (1-12)", &month, intfmt);
  getnum( "Day.fraction", &day, dblfmt );

  /* Find the Julian day. */
  J = caltoj(cyear,month,day);
  /*Trace( "Julian day %.1f\n", J );*/

  pdate:
  /* Convert back to calendar date. */
  /* jtocal( J ); */
  return(J);
}

/*     Calculate Julian day from Gregorian calendar date
 */

/*****************************************************************************************
* @fn     : caltoj
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

double caltoj( year, month, day ) long year; int month; double day; {
 long y, a, b, c, e, m;
 double J;
 
 /* The origin should be chosen to be a century year
 * that is also a leap year.  We pick 4801 B.C. */

 y = year + 4800;
 if( year < 0 ) {
  y += 1;
 }

 /* The following magic arithmetic calculates a sequence
  * whose successive terms differ by the correct number of
  * days per calendar month.  It starts at 122 = March; January
  * and February come after December.
  */
 
 m = month;
 if( m <= 2 ) {
  m += 12;
  y -= 1;
 }
 e = (306 * (m+1))/10;
 a = y/100;	/* number of centuries */
 
 if( year <= 1582L ) {
  if( year == 1582L ) { if( month < 10 ) goto julius; if( month > 10) goto gregor;if( day >= 15 ) goto gregor; }

  julius:
   /* (mise en commentaire 2023) */
   /* Trace( " Julian Calendar assumed.\n" ); */
   b = -38;
 }
 else { /* -number of century years that are not leap years */
  gregor:
  b = (a/4) - a;
 }
 c = (36525L * y)/100; /* Julian calendar years and leap years */

 /* Add up these terms, plus offset from J 0 to 1 Jan 4801 B.C.
 * Also fudge for the 122 days from the month algorithm. */
 J = b + c + e + day - 32167.5;
 return( J );
}
/* Calculate month, day, and year from Julian date */

int jtocal( J ) double J; {
 int month, day;
 long year, a, c, d, x, y, jd;
 int BC;
 double dd;

 if( J < 1721423.5 ) /* January 1.0, 1 A.D. */ BC = 1;
 else                BC = 0;

 jd = (long) (J + 0.5); /* round Julian date up to integer */

 /* Find the number of Gregorian centuries
 * since March 1, 4801 B.C. */

 a = (100*jd + 3204500L)/3652425L;

 /* Transform to Julian calendar by adding in Gregorian century years
  * that are not leap years.
  * Subtract 97 days to shift origin of JD to March 1.
  * Add 122 days for magic arithmetic algorithm.
  * Add four years to ensure the first leap year is detected.
  */
 c = jd + 1486;
 if( jd >= 2299160.5 )
	c += a - a/4;
 else
	c += 38;
 /* Offset 122 days, which is where the magic arithmetic
  * month formula sequence starts (March 1 = 4 * 30.6 = 122.4).
  */
 d = (100*c - 12210L)/36525L;
 /* Days in that many whole Julian years */
 x = (36525L * d)/100L;

 /* Find month and day. */
 y = ((c-x)*100L)/3061L;
 day = (int) (c - x - ((306L*y)/10L));
 month = (int) (y - 1);
 if( y > 13 )
	month -= 12;

 /* Get the year right. */
 year = d - 4715;
 if( month > 2 )
	year -= 1;

 /* Day of the week. */
 a = (jd + 1) % 7;

 /* Fractional part of day. */
 dd = day + J - jd + 0.5;

 /* post the year. */
 cyear = year;

 if( BC )
	{
	year = -year + 1;
	cyear = -year;
	if( prtflg ) {
    /* (mise en commentaire 2023) */
		/* Trace( "%ld B.C. ", year ); */
  }
	}
 else {
	if( prtflg ) {
    /* (mise en commentaire 2023) */
		// Trace( "%ld ", year );
  }
 }
 day = (int) dd;

 if( prtflg ) {
 /* (mise en commentaire 2023) */
	/* Trace( "%s %d %s", months[month-1], day, days[(int) a] ); */
 }
 /* Flag last or first day of year */
 if( ((month == 1) && (day == 1))
	|| ((month == 12) && (day == 31)) )
	yerend = 1;
 else
	yerend = 0;

 /* Display fraction of calendar day
  * as clock time.
  */
 a = (long) dd;
 dd = dd - a;
 if( prtflg ) {
    hms( 2.0*PI*dd );

    /* (mise en commentaire 2023) */
    /*
    if( J == TDT ) {
      Trace( "TDT\n" ); // Indicate Terrestrial Dynamical Time
    }
    else if( J == UT ) {
      Trace( "UT\n" ); // Universal Time
    }
    else {
      Trace( "\n" );
    }
    */
 }
 return(0);
}


/* Reduce x modulo 360 degrees
 */


/*****************************************************************************************
* @fn     : mod360
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

double mod360(x)
double x;
{
  long k;
  double y;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  k = (long) (x/360.0);
  y = x  -  k * 360.0;
  while( y < 0.0 )
    y += 360.0;
  while( y > 360.0 )
    y -= 360.0;
  return(y);
}


/* Reduce x modulo 2 pi
 */
#define TPI (2.0*PI)

/*****************************************************************************************
* @fn     : modtp
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

double modtp(x)
double x;
{
  double y;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  y = floor( x/TPI );
  y = x - y * TPI;
  while( y < 0.0 )
    y += TPI;
  while( y >= TPI )
    y -= TPI;
  return(y);
}


/* Get operator to type in hours, minutes, and seconds
 */
static int hours = 0;
static int minutes = 0;
static double seconds = 0.0;

/*****************************************************************************************
* @fn     : gethms_moi
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

double gethms_moi(int hourr,int minn, int secc) {
 double t;
 char cmd[64] ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset(cmd,0,sizeof(cmd)) ;
  sprintf(cmd,"%d",hourr);
  getnum_moi( cmd, &hours, intfmt );
  
  memset(cmd,0,sizeof(cmd)) ;
  sprintf(cmd,"%d",minn); 
  getnum_moi( cmd, &minutes, intfmt );
  
  memset(cmd,0,sizeof(cmd)) ;
  sprintf(cmd,"%d",secc);
  getnum_moi( cmd, &seconds, dblfmt );

  t = (3600.0*hours + 60.0*minutes + seconds)/86400.0 ;

  return(t);
}
/*****************************************************************************************
* @fn     : gethms
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

double gethms() {
 double t;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  getnum( "Time: Hours", &hours, intfmt );
  getnum( "Minutes", &minutes, intfmt );
  getnum( "Seconds", &seconds, dblfmt );
  t = (3600.0*hours + 60.0*minutes + seconds)/86400.0;

  return(t);
}
/*****************************************************************************************
* @fn     : getnum_moi
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int getnum_moi( char s[40], void *num, char *format) {

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /*
  if     ( format == strfmt ) Trace( format,  (char *)  num );
  else if( format == dblfmt ) Trace( format, *(double *)num );
  else if( format == intfmt ) Trace( format, *(int *)   num );
  else if( format == lngfmt ) Trace( format, *(long *)  num );
  else                        Trace( "Illegal input format\n"  );
  */
  if( s[0] != '\0' ) sscanf( s, format, num );

  return(0);
}
/*****************************************************************************************
* @fn     : getnum
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int getnum( msg, num, format ) char *msg; void *num; char *format; {
  char s[40];

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1( "%s (", msg );
  /*
  if     ( format == strfmt ) Trace( format, (char *) num );
  else if( format == dblfmt ) Trace( format, *(double *)num );
  else if( format == intfmt ) Trace( format, *(int *)num );
  else if( format == lngfmt ) Trace( format, *(long *)num );
  else                        Trace( "Illegal input format\n"  );

  Trace( ") ? ");
  */
  // gets(s);
  memset(s,0, sizeof(s));
  fgets(s,40,stdin ) ;

  if( s[0] != '\0' ) sscanf( s, format, num );
  return(0);
}

/*
 * Convert change in rectangular coordinatates to change
 * in right ascension and declination.
 * For changes greater than about 0.1 degree, the
 * coordinates are converted directly to R.A. and Dec.
 * and the results subtracted.  For small changes,
 * the change is calculated to first order by differentiating
 *   tan(R.A.) = y/x
 * to obtain
 *    dR.A./cos**2(R.A.) = dy/x  -  y dx/x**2
 * where
 *    cos**2(R.A.)  =  1/(1 + (y/x)**2).
 *
 * The change in declination arcsin(z/R) is
 *   d asin(u) = du/sqrt(1-u**2)
 *   where u = z/R.
 *
 * p0 is the initial object - earth vector and
 * p1 is the vector after motion or aberration.
 *
 */
/*****************************************************************************************
* @fn     : deltap
* @author : s.gravois / nasa
* @brief  : ras
* @param  : ras
* @date   : 2023-01-04 creation entete doxygen
* @todo   : ras
*****************************************************************************************/

int deltap( p0, p1, dr, dd )
double p0[], p1[];
double *dr, *dd;
{
  double dp[3], A, B, P, Q, x, y, z;
  int i;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  P = 0.0;
  Q = 0.0;
  z = 0.0;
  for( i=0; i<3; i++ )
    {
    x = p0[i];
    y = p1[i];
    P += x * x;
    Q += y * y;
    y = y - x;
    dp[i] = y;
    z += y*y;
    }

  A = sqrt(P);
  B = sqrt(Q);

  if( (A < 1.e-7) || (B < 1.e-7) || (z/(P+Q)) > 5.e-7 )
    {
    P = zatan2( p0[0], p0[1] );
    Q = zatan2( p1[0], p1[1] );
    Q = Q - P;
    while( Q < -PI )
      Q += 2.0*PI;
    while( Q > PI )
      Q -= 2.0*PI;
    *dr = Q;
    P = asin( p0[2]/A );
    Q = asin( p1[2]/B );
    *dd = Q - P;
    return(0);
    }


  x = p0[0];
  y = p0[1];

  if( x == 0.0 )
    {
    *dr = 1.0e38;
    }
  else
    {
    Q = y/x;
    Q = (dp[1]  -  dp[0]*y/x)/(x * (1.0 + Q*Q));
    *dr = Q;
    }

  x = p0[2]/A;
  P = sqrt( 1.0 - x*x );
  *dd = (p1[2]/B - x)/P;
  
  return(0);
}
