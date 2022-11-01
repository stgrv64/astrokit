/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-10-07  | * creation
# 2022-10-07  | * rapatriement code depuis autres fichiers
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_TIME_H
#define ASTRO_TIME_H

/* Definition de la structure associeee au fichier avant inclusion astro_global.h */
/* Ceci est permis par le pre processeur */

struct STR_TIME {
  
  /* L heure decimale sert pour la representation 
   * d'un angle sous la forme  : 12h25mn05s 
   * 360 degres = 24h
   * -------------------------------------------- */
   
  double hd ;      // heure decimale
  char   c_si ;    /* signe */ 
  int    si ;      // signe
  int    mm ;      // month
  int    yy ;      // year
  int    dd ;      // day
  int    HH ;      // hour
  int    MM ;      // minutes
  int    SS ;      // secondes

} ;
typedef struct STR_TIME STRUCT_TIME ;

#include "astro_global.h"

//---------------------------------------------------------------------------------------
// man date sur noyau compile 
// Display time (using +FMT), or set time
//
//	[-s,--set] TIME	Set time to TIME
//	-u,--utc	Work in UTC (don't convert to local time)
//	-R,--rfc-2822	Output RFC-2822 compliant date string
//	-I[SPEC]	Output ISO-8601 compliant date string
//			SPEC='date' (default) for date only,
//			'hours', 'minutes', or 'seconds' for date and
//			time to the indicated precision
//	-r,--reference FILE	Display last modification time of FILE
//	-d,--date TIME	Display TIME, not 'now'
//	-D FMT		Use FMT for -d TIME conversion
//
//Recognized TIME formats:
//	hh:mm[:ss]
//	[YYYY.]MM.DD-hh:mm[:ss]
//	YYYY-MM-DD hh:mm[:ss]
//	[[[[[YY]YY]MM]DD]hh]mm[.ss]
//	'date TIME' form accepts MMDDhhmm[[YY]YY][.ss] instead
//---------------------------------------------------------------------------------------

#define  TEMPS_MILLI_SEC    1000                // frequence d'une microseconde (pour les usleep et calculs)
#define  TEMPS_MICRO_SEC    1000000             // frequence d'une microseconde (pour les usleep et calculs)
#define  TEMPS_NANO_SEC     1000000000          // frequence d'une nanoseconde (pour les nanosleep et calculs) 



struct STR_TIME_PARAMS {
  
  unsigned long par_tempo_Menu ;
  unsigned long par_tempo_Raq ;
  unsigned long par_tempo_Ir ;
  unsigned long par_tempo_Termios ;
  unsigned long par_tempo_Clavier ;
  unsigned long par_tempo_Capteurs ;
  unsigned long par_tempo_Lcd_Loop ;
  unsigned long par_tempo_Lcd_Disp ;
} ;
typedef struct STR_TIME_PARAMS STRUCT_TIME_PARAMS ;

// ------ TEMPORISATIONS DES BOUCLES ------------

void   TEMPS_INIT   ( STRUCT_TIME * ) ;

void   TEMPS_AFFICHER               ( STRUCT_TIME *  ) ; /* FIXME ajout 20191228 */
void   TEMPS_SET_YEAR_MONTH_AND_DAY ( char *  ) ; // FIXME ajout 20190822
void   TEMPS_SET_MONTH_AND_DAY      ( char *  ) ;
void   TEMPS_SET_HOUR_AND_MINUTES   ( char *  ) ;

double TEMPS_CALCUL_DUREE_SECONDES        (struct timeval * )  ;
double TEMPS_CALCUL_DUREE_MICROSEC        (struct timeval * ) ;
double TEMPS_CALCUL_DUREE_NANOSEC         (struct timeval * ) ;
long   TEMPS_TEMPORISATION_MICROSEC       (double , double , struct timeval ) ;

void  TEMPS_AFFICHER_MSG_HHMMSS          ( char * , STRUCT_TIME *  ) ;

int    TEMPS_CALCUL_TEMPS_SIDERAL           ( STRUCT_LIEU* gp_Lie, STRUCT_TIME * gp_Tim ) ;
int    TEMPS_CALCUL_JOUR_JULIEN             ( STRUCT_LIEU* gp_Lie, STRUCT_TIME * gp_Tim) ;
int    TEMPS_CALCUL_DATE                    ( STRUCT_TIME * gp_Tim ) ;
void   TEMPS_CALCUL_TEMPS_HMS_VERS_DEC      ( STRUCT_TIME * gp_Tim ) ;
void   TEMPS_CALCUL_TEMPS_DEC_VERS_HMS      ( STRUCT_TIME * gp_Tim ) ;
void   TEMPS_CALCUL_TEMPS_HMS_VERS_DEC_DIRECT  ( double *, double, double, double) ;

#endif