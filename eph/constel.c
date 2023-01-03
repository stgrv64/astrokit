/* Constellation names
 */

#include <stdio.h>
#if __STDC__
static int islow (char *);
static int isup (char *);
static int isnumber (char *);
static int skipwh(char *);
double sqrt (double);
double sin (double);
double cos (double);
double atan2 (double, double);
double asin (double);
int precess (double *, double, int);
#else
static int islow(), isup(), isnumber(), skipwh();
double sqrt(), sin(), cos(), atan2(), asin();
int precess();
#endif

#define  RTD 57.295779513082320877

#define NCON 89
char *constel[NCON] = {
"And Andromedae",
"Ant Antliae",
"Aps Apodis",
"Aql Aquilae",
"Aqr Aquarii",
"Ari Arietis",
"Ara Arae",
"Aur Aurigae",
"Boo Bootis",
"Cae Caeli",
"Cam Camelopardalis",
"Can Cancri",		/* also abbreviated Cnc */
"Cap Capricorni",
"Car Carinae",
"Cas Cassiopeiae",
"Cen Centauri",
"Cep Cephei",
"Cet Ceti",
"Cha Chamaeleontis",
"Cir Circini",
"CMa Canis Majoris",
"CMi Canis Minoris",
"Cnc Cancri",
"Col Columbae",
"Com Comae Berenices",
"CrA Coronae Austrinae",
"CrB Coronae Borealis",
"Crt Crateris",
"Cru Crucis",
"Crv Corvi",
"CVn Canum Venaticorum",
"Cyg Cygni",
"Del Delphini",
"Dor Doradus",
"Dra Draconis",
"Equ Equulei",
"Eri Eridani",
"For Fornacis",
"Gem Geminorum",
"Gru Gruis",
"Her Herculis",
"Hor Horologii",
"Hya Hydrae",
"Hyi Hydri",
"Ind Indi",
"Lac Lacertae",
"Leo Leonis",
"Lep Leporis",
"Lib Librae",
"LMi Leonis Minoris",
"Lup Lupi",
"Lyn Lyncis",
"Lyr Lyrae",
"Men Mensae",
"Mic Microscopii",
"Mon Monocerotis",
"Mus Muscae",
"Nor Normae",
"Oct Octantis",
"Oph Ophiuchi",
"Ori Orionis",
"Pav Pavonis",
"Peg Pegasi",
"Per Persei",
"Phe Phoenicis",
"Pic Pictoris",
"PsA Piscis Austrini",
"Psc Piscium",
"Pup Puppis",
"Pyx Pyxidis",
"Ret Reticuli",
"Scl Sculptoris",
"Sco Scorpii",
"Sct Scuti",
"Ser Serpentis",
"Sex Sextantis",
"Sge Sagittae",
"Sgr Sagittarii",
"Tau Tauri",
"Tel Telescopii",
"TrA Trianguli Australis",
"Tri Trianguli",
"Tuc Tucanae",
"UMa Ursae Majoris",
"UMi Ursae Minoris",
"Vel Velorum",
"Vir Virginis",
"Vol Volantis",
"Vul Vulpeculae",
};



/* Greek letters
 */

#define NGREEK 24
char *greek[NGREEK] = {
"alpha",
"beta",
"gamma",
"delta",
"epsilon",
"zeta",
"eta",
"theta",
"iota",
"kappa",
"lambda",
"mu",
"nu",
"xi",
"omicron",
"pi",
"rho",
"sigma",
"tau",
"upsilon",
"phi",
"chi",
"psi",
"omega",
};

int showcname( in )
char *in;
{
char *g, *p, *q;
char ans[80];
int i;


p = in;
q = ans;


skipwh(p);
if( isnumber(p) )
	{
	while( isnumber(p) )
		*q++ = *p++;
	}
skipwh(p);
*q++ = ' ';

if( islow(p) )
	{
	for( i=0; i<NGREEK; i++ )
		{
		g = greek[i];
		if( (*p == *g) && (*(p+1) == *(g+1)) )
			break;
		}
	if( i < NGREEK )
		{
		while( *g != '\0' )
			*q++ = *g++;
		}
	while( islow(p) )
		++p;
	}
skipwh(p);
/* copy things like "-a" until uppercase letter found */
while( (*p != '\0') && !isup(p) )
	*q++ = *p++;

*q++ = ' ';

if( isup(p) )
	{
/* Check the list of constellation names */
	for( i=0; i<NCON; i++ )
		{
		g = constel[i];
		if( (*p == *g) && ( *(p+1) == *(g+1) )
			&& ( *(p+2) == *(g+2) ) )
			break;
		}
/* Get the name found */
	if( i < NCON )
		{
		g += 4;
		while( *g != '\0' )
			*q++ = *g++;
		p += 3;
		}
	}
skipwh(p);
*q++ = ' ';
while( *p )
	*q++ = *p++;
*q++ = '\0';
/* convert all '_' characters to spaces */
q = ans;
while( *q != '\0' ) {
	if( *q == '_' )
		*q = ' ';
	++q;
}
/* (mise en commentaire 2023) */
// printf( "\n              %s\n", ans );
return(0);
}




static int islow(p)
char *p;
{
if( (*p >= 'a') && (*p <= 'z') )
	return(1);
else
	return(0);
}



static int isup(p)
char *p;
{
if( (*p >= 'A') && (*p <= 'Z') )
	return(1);
else
	return(0);
}




static int isnumber(p)
char *p;
{
if( (*p >= '0') && (*p <= '9') )
	return(1);
else
	return(0);
}



static int skipwh(p)
char *p;
{
while( ((*p == ' ') || (*p == '\t') || (*p == '_'))
	&& (*p != '\0') && (*p != '\n') && (*p != '\r') )
		++p;
return(0);
}


/* Table of constellation boundaries.

    Roman, Nancy Grace, "Identification of a Constellation from a Position"
    Pub. Astron. Soc. Pac. 99, 695, (1987)

   Array items are
   Lower Right Ascension, Upper Right Ascension,
     both in units of hours times 3600;
   Lower Declination, in units of degrees times 3600;
   and array index of constellation name.  */

#define NBNDRIES 357
long bndries[4*NBNDRIES] = {
     0,  86400, 316800,  84,
 28800,  52200, 311400,  84,
 75600,  82800, 310200,  84,
 64800,  75600, 309600,  84,
     0,  28800, 306000,  16,
 33000,  38400, 295200,  10,
     0,  18000, 288000,  16,
 38400,  52200, 288000,  10,
 63000,  64800, 288000,  84,
 72600,  75600, 288000,  34,
     0,  12630, 277200,  16,
 41400,  48900, 277200,  10,
 59520,  63000, 270000,  84,
 72600,  74400, 270000,  16,
 28680,  33000, 264600,  10,
 33000,  40800, 264600,  34,
 46800,  59520, 252000,  84,
 11160,  12300, 244800,  14,
 73500,  74400, 241200,  34,
 40800,  43200, 239400,  34,
     0,   1200, 237600,  16,
 50400,  56400, 237600,  84,
 84900,  86400, 237600,  16,
 43200,  48600, 230400,  34,
 48600,  51900, 226800,  34,
 83400,  84900, 226800,  16,
 21960,  25200, 223200,  10,
 72000,  73500, 221400,  34,
 73932,  74160, 219300,  16,
 25200,  28680, 216000,  10,
 28680,  30300, 216000,  83,
 71160,  72000, 214200,  34,
 72000,  73932, 214200,  16,
 82320,  83400, 212700,  16,
     0,   8760, 210600,  14,
 69900,  71160, 208800,  34,
  6120,   6870, 207000,  14,
  8760,  11160, 205200,  14,
 11160,  11400, 205200,  10,
 80340,  82320, 202500,  16,
 18000,  21960, 201600,  10,
 50520,  51900, 199800,  83,
 51900,  69900, 199800,  34,
 11400,  12000, 198000,  10,
 79680,  80340, 198000,  16,
 74160,  79080, 197400,  16,
     0,   6120, 194400,  14,
 21960,  23400, 194400,  51,
 43500,  48600, 190800,  83,
 54900,  56700, 190800,  34,
 79080,  79680, 189900,  16,
 12000,  18000, 189000,  10,
 82320,  84000, 189000,  14,
 56700,  61200, 185400,  34,
  7350,   9060, 181800,  63,
 61200,  65640, 181800,  34,
     0,   4920, 180000,  14,
  4920,   6000, 180000,  63,
 23400,  24480, 180000,  51,
 84000,  86400, 180000,  14,
 48600,  50520, 174600,  83,
     0,   4020, 172800,  14,
 84900,  86400, 172800,  14,
 65430,  65640, 171000,  40,
 65640,  68700, 171000,  34,
 68700,  69000, 171000,  31,
  6000,   7350, 169200,  63,
 30300,  33000, 169200,  83,
   600,   3120, 165600,  14,
 43200,  43500, 162000,  83,
 24480,  26520, 160200,  51,
 78870,  79080, 158400,  31,
 78750,  78870, 157500,  31,
 69000,  69840, 156600,  31,
 33000,  36600, 151200,  83,
 36600,  38820, 144000,  83,
 55560,  56700, 144000,   8,
 56700,  58800, 144000,  40,
 33300,  34500, 143100,  51,
     0,   9060, 132300,   0,
  9060,   9240, 132300,  63,
 69690,  69840, 131400,  52,
 16200,  16890, 129600,  63,
 78240,  78750, 129600,  31,
 78750,  79200, 129600,  45,
 23520,  26520, 127800,   7,
 26520,  27900, 127800,  51,
     0,   7200, 126000,   0,
 79200,  82140, 126000,  45,
 82140,  82320, 124200,  45,
 82320,  84600, 124200,   0,
  9240,   9780, 122400,  63,
 38820,  39600, 122400,  83,
 43200,  44400, 122400,  30,
 27900,  33300, 120600,  51,
 33300,  35580, 120600,  49,
  2580,   5070, 118800,   0,
 54660,  55560, 118800,   8,
 84600,  85500, 115500,   0,
 44400,  47700, 115200,  30,
 85500,  86400, 112800,   0,
 50250,  50520, 110700,  30,
  8700,   9780, 110400,  81,
  9780,  16200, 110400,  63,
 16200,  17100, 108000,   7,
 65430,  69690, 108000,  52,
 39600,  43200, 104400,  83,
 70800,  75300, 104400,  31,
 17100,  21180, 102600,   7,
 35580,  37800, 102600,  49,
 47700,  50250, 102600,  30,
     0,    240, 100800,   0,
  5070,   6000, 100800,  81,
 21180,  23520, 100800,   7,
 28380,  28800, 100800,  38,
 75300,  78240, 100800,  31,
 69330,  70800,  99000,  31,
  6900,   8700,  98100,  81,
 58200,  58800,  97200,  26,
 54300,  54660,  93600,   8,
 54660,  58200,  93600,  26,
 66120,  67920,  93600,  52,
 38700,  39600,  91800,  49,
 67920,  69330,  91800,  52,
  6000,   6900,  90000,  81,
  2580,   3060,  85500,  67,
 37800,  38700,  84600,  49,
 76500,  77100,  84600,  88,
 20520,  21180,  82200,  78,
   240,    510,  79200,   0,
 57300,  57720,  79200,  74,
 21180,  22380,  77400,  38,
 71400,  72900,  76500,  88,
 67920,  69300,  75900,  88,
   510,   3060,  75600,   0,
 72900,  74040,  73800,  88,
 28110,  28380,  72000,  38,
 74040,  76500,  70200,  88,
 69300,  71400,  69000,  88,
 11820,  12120,  68400,   5,
 67920,  68400,  66600,  76,
 20520,  20760,  64800,  60,
 22380,  22710,  63000,  38,
 68400,  71400,  58200,  76,
 17880,  19200,  57600,  78,
 57300,  57900,  57600,  40,
 71400,  72900,  56700,  76,
 16620,  17880,  55800,  78,
 19200,  20160,  55800,  78,
 46200,  48600,  54000,  24,
 62100,  65700,  51600,  40,
 42720,  46200,  50400,  24,
 27000,  28110,  48600,  38,
 60300,  62100,  46200,  40,
     0,    510,  45000,  62,
 20160,  20760,  45000,  78,
 25200,  27000,  45000,  38,
 76020,  76800,  45000,  62,
 22710,  24960,  43200,  38,
 65700,  67920,  43200,  40,
 75150,  75780,  42600,  32,
 75780,  76020,  42600,  62,
 41460,  42720,  39600,  46,
 22470,  22710,  36000,  60,
 24960,  25200,  36000,  38,
 28110,  28530,  36000,  22,
 85800,  86400,  36000,  62,
  6000,  11820,  35700,   5,
 72510,  73080,  30600,  32,
 48600,  54300,  28800,   8,
 81900,  85800,  27000,  62,
 28530,  33300,  25200,  22,
 33300,  38700,  25200,  46,
 65700,  67184,  22500,  59,
 67184,  67920,  22500,   3,
 75000,  75150,  21600,  32,
 25200,  25260,  19800,  21,
 65700,  66330,  16200,  74,
 57900,  60300,  14400,  40,
 65700,  66330,  10800,  59,
 77280,  78000,   9900,  62,
     0,   7200,   7200,  67,
 66900,  67920,   7200,  74,
 73080,  75000,   7200,  32,
 75000,  76800,   7200,  35,
 76800,  77280,   7200,  62,
 79200,  81900,   7200,  62,
 78000,  79200,   6300,  62,
 25260,  25920,   5400,  21,
 12900,  16620,      0,  78,
 16620,  16800,      0,  60,
 25920,  29100,      0,  21,
 52800,  54300,      0,  86,
 64200,  65700,      0,  59,
  9540,  11820,  -6300,  17,
 11820,  12900,  -6300,  78,
 54300,  58560, -11700,  74,
 16800,  18300, -14400,  60,
 21000,  22470, -14400,  60,
 64200,  64680, -14400,  74,
 65700,  66900, -14400,  74,
 66900,  67920, -14400,   3,
 81900,  85800, -14400,  67,
 38700,  41460, -21600,  46,
 41460,  42600, -21600,  86,
     0,   1200, -25200,  67,
 85800,  86400, -25200,  67,
 51300,  52800, -28800,  86,
 57300,  58560, -28800,  59,
 72000,  73920, -32400,   3,
 76800,  78720, -32400,   4,
 61800,  64680, -36000,  59,
 21000,  29100, -39600,  55,
 17700,  18300, -39600,  36,
 18300,  21000, -39600,  60,
 29100,  30120, -39600,  42,
 34500,  38700, -39600,  75,
 42600,  46200, -39600,  86,
 63300,  63600, -42000,  59,
 67920,  72000, -43320,   3,
 17400,  17700, -52200,  36,
 73920,  76800, -54000,   4,
 61800,  65700, -57600,  74,
 65700,  67920, -57600,  73,
 30120,  30900, -61200,  42,
 58560,  58950, -65700,  59,
 30900,  32700, -68400,  42,
 38700,  39000, -68400,  27,
 58560,  58950, -69300,  59,
 56400,  57300, -72000,  48,
 45300,  46200, -79200,  29,
 46200,  51300, -79200,  86,
 32700,  35100, -86400,  42,
  6000,   9540, -87780,  17,
  9540,  13500, -87780,  36,
 39000,  42600, -88200,  27,
 42600,  45300, -88200,  29,
 51300,  53700, -88200,  48,
 58560,  60300, -88500,  59,
     0,   6000, -91800,  17,
 76800,  78720, -91800,  12,
 78720,  85800, -91800,   4,
 85800,  86400, -91800,  17,
 35100,  36900, -95400,  42,
 16920,  17400, -98100,  36,
 17400,  22020, -98100,  47,
 72000,  76800, -100800,  12,
 36900,  38100, -105000,  42,
 45300,  53700, -106200,  42,
 53700,  56400, -106200,  48,
 56400,  57600, -106200,  72,
 16500,  16920, -108000,  36,
 60300,  63360, -108000,  59,
 63360,  64200, -108000,  77,
 38100,  39000, -112200,  42,
 22020,  26520, -118800,  20,
 44100,  45300, -118800,  42,
 39000,  44100, -126000,  42,
 12600,  13500, -129600,  37,
 30120,  33720, -132300,  69,
 15360,  16500, -133200,  36,
 64200,  69000, -133200,  77,
 76800,  82800, -133200,  66,
 82800,  84000, -133200,  71,
 10800,  12600, -142500,  37,
 33720,  39600, -143100,   1,
     0,   6000, -144000,  71,
  6000,  10800, -144000,  37,
 13920,  15360, -144000,  36,
 84000,  86400, -144000,  71,
 51000,  53700, -151200,  15,
 56400,  57600, -151200,  50,
 57600,  59115, -151200,  72,
 17400,  18000, -154800,   9,
 18000,  23700, -154800,  23,
 28800,  30120, -154800,  68,
 12300,  13920, -158400,  36,
 59115,  64200, -163800,  72,
 64200,  69000, -163800,  25,
 69000,  73200, -163800,  77,
 73200,  76800, -163800,  54,
 10800,  12300, -165600,  36,
 16200,  17400, -167400,   9,
 55200,  56400, -172800,  50,
     0,   8400, -173400,  64,
  9600,  10800, -176400,  36,
 14700,  15360, -176400,  41,
 15360,  16200, -176400,   9,
 76800,  79200, -180000,  39,
 21600,  28800, -182700,  68,
 28800,  29400, -182700,  85,
  8700,   9600, -183600,  36,
 13800,  14700, -183600,  41,
     0,   6600, -185400,  64,
 21600,  22200, -189000,  13,
 29400,  30420, -190800,  85,
 12600,  13800, -191400,  41,
 13800,  14400, -191400,  33,
     0,   5700, -192600,  64,
  7800,   8700, -194400,  36,
 16200,  18000, -194400,  65,
 54180,  55200, -194400,  50,
 30420,  31800, -196200,  85,
 22200,  23400, -198000,  13,
 42600,  46200, -198000,  15,
 51000,  54180, -198000,  50,
 54180,  55200, -198000,  57,
 14400,  15600, -203400,  33,
 31800,  39600, -203400,  85,
 39600,  40500, -203400,  15,
 63000,  64800, -205200,   6,
 64800,  73200, -205200,  79,
 79200,  84000, -205200,  39,
 11520,  12600, -207000,  41,
 18000,  19800, -207000,  65,
 23400,  24600, -208800,  13,
     0,   4800, -210600,  64,
  4800,   7800, -210600,  36,
 84000,  86400, -210600,  64,
 15600,  16500, -212400,  33,
 55200,  59115, -216000,  57,
 73200,  76800, -216000,  44,
 19800,  21600, -219600,  65,
 54600,  55200, -219600,  19,
 59115,  59700, -219600,   6,
 53700,  54600, -228900,  19,
 59700,  60300, -228900,   6,
 21600,  24600, -230400,  65,
 24600,  32520, -230400,  13,
 40500,  42600, -230400,  15,
 42600,  46200, -230400,  28,
 46200,  52320, -230400,  15,
 48600,  49200, -234000,  19,
 60300,  60600, -234000,   6,
  7800,  11520, -243000,  41,
 11520,  16500, -243000,  70,
 53100,  53700, -243000,  19,
 60600,  63000, -243000,   6,
 63000,  64800, -243000,  61,
 79200,  84000, -243000,  82,
 16500,  23700, -252000,  33,
 49200,  53100, -252000,  19,
 53100,  61200, -252000,  80,
     0,   4800, -270000,  82,
 12600,  16500, -270000,  43,
 23700,  32520, -270000,  87,
 32520,  40500, -270000,  13,
 40500,  49200, -270000,  56,
 64800,  76800, -270000,  61,
 76800,  84000, -270000,  44,
 84000,  86400, -270000,  82,
  2700,   4800, -273600,  82,
     0,  12600, -297000,  43,
 27600,  49200, -297000,  18,
 49200,  64800, -297000,   2,
 12600,  27600, -306000,  53,
     0,  86400, -324000,  58,
};


/* Return the constellation name corresponding to a given mean equatorial
   position P.  EPOCH is the precessional equinox and ecliptic date
   of P.  */

char *
whatconstel (pp, epoch)
double pp[];
double epoch;
{
  int i, k;
  double ra, dec, d;
  double p[3];

  for (i = 0; i < 3; i++)
    p[i] = pp[i];

  /* Precess from given epoch to J2000.  */
  precess (p, epoch, 1);
  /* Precess from J2000 to Besselian epoch 1875.0.  */
  precess (p, 2405889.25855, -1);
  d = p[0] * p[0] + p[1] * p[1] + p[2] * p[2];
  d = sqrt (d);
  ra = atan2 (p[1], p[0]) * (RTD * 3600. / 15.);
  if (ra < 0.0)
    ra += 86400.0;
  dec = asin (p[2] / d) * (RTD * 3600.);

    /* FIND CONSTELLATION SUCH THAT THE DECLINATION ENTERED IS HIGHER THAN
       THE LOWER BOUNDARY OF THE CONSTELLATION WHEN THE UPPER AND LOWER
       RIGHT ASCENSIONS FOR THE CONSTELLATION BOUND THE ENTERED RIGHT
       ASCENSION
    */
  for (i = 0; i < NBNDRIES; i++)
    {
      k = i << 2;
      if (ra >= bndries[k] && ra < bndries[k+1] && dec > bndries[k+2])
	{
	  k = bndries[k+3];
	  return (constel[k]);
	}
    }
  return ("?? constellation not found");
}

#if 0
/* Test program  */
double J2000 = 2451545.0;
double STR = 4.8481368110953599359e-6;

test (r,d)
double r, d;
{
  double c, p[3], jd;

  d /= RTD;
  r *= 15.0/RTD;
  c = cos(d);
  p[2] = sin(d);
  p[0] = c * cos(r);
  p[1] = c * sin(r);
  jd = 2433282.423; /* 1950.0 Besselian epoch */
  printf ("%8.4f %9.4f %s\n", r, d, whatconstel (p, jd));
}


int
main()
{
  /*    The following is an example of the output of the program:
      RA =  9.0000 DEC =  65.0000  IS IN CONSTELLATION UMa
      RA = 23.5000 DEC = -20.0000  IS IN CONSTELLATION Aqr
      RA =  5.1200 DEC =   9.1200  IS IN CONSTELLATION Ori
      RA =  9.4555 DEC = -19.9000  IS IN CONSTELLATION Hya
      RA = 12.8888 DEC =  22.0000  IS IN CONSTELLATION Com
      RA = 15.6687 DEC = -12.1234  IS IN CONSTELLATION Lib
      RA = 19.0000 DEC = -40.0000  IS IN CONSTELLATION CrA
      RA =  6.2222 DEC = -81.1234  IS IN CONSTELLATION Men  */
  test (9.0, 65.0);
  test (23.5, -20.0);
  test (5.12, 9.12);
  test (9.4555, -19.9);
  test (12.8888, 22.0);
  test (15.6687, -12.1234);
  test (19.0, -40.0);
  test (6.2222, -81.1234);
  exit(0);
}
#endif
