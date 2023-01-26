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

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#ifndef CONFIG_TAILLE_BUFFER_256
#define CONFIG_TAILLE_BUFFER_256 256
#endif

typedef struct STR_TIME STRUCT_TIME ;

struct STR_TIME {
  pthread_mutex_t tim_mutex ;
  struct timeval  tim_tval ; 
  FILE           *tim_file ;
  void          (*tim_log)     ( STRUCT_TIME * ) ;
  void          (*tim_display) ( STRUCT_TIME * ) ;
  void          (*tim_lock)    ( STRUCT_TIME * ) ;
  void          (*tim_unlock)  ( STRUCT_TIME * ) ;
  int             tim_loglevel ;
  char            tim_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char            tim_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;

  double          tim_hd ;   // heure decimale
  char            tim_sig ;  /* signe */ 
  int             tim_si ;   // signe
  int             tim_mm ;   // month
  int             tim_yy ;   // year
  int             tim_dd ;   // day
  int             tim_HH ;   // hour
  int             tim_MM ;   // minutes
  int             tim_SS ;   // secondes

} ;

#include "astro_global.h"

#define TIME_DEFAULT_TEMPO_VOUTE    1000000
#define TIME_DEFAULT_TEMPO_NEMU     50000
#define TIME_DEFAULT_TEMPO_RAQ      51000
#define TIME_DEFAULT_TEMPO_IR       52000
#define TIME_DEFAULT_TEMPO_TERMIOS  53000
#define TIME_DEFAULT_TEMPO_CAPTEURS 54000
#define TIME_DEFAULT_TEMPO_LCD_LOOP 250000
#define TIME_DEFAULT_TEMPO_LCD_DISP 100000
#define TIME_DEFAULT_TEMPO_PID_LOOP 100000

#define TIME_DEFAULT_TEMPO_COEFF_MULT_RAQ      1.0
#define TIME_DEFAULT_TEMPO_COEFF_MULT_NEMU     1.1
#define TIME_DEFAULT_TEMPO_COEFF_MULT_IR       1.2
#define TIME_DEFAULT_TEMPO_COEFF_MULT_TERMIOS  1.3
#define TIME_DEFAULT_TEMPO_COEFF_MULT_CAPTEURS 1.4

#define TIME_DEFAULT_TEMPO_COEFF_MULT_LCD_LOOP 1.5
#define TIME_DEFAULT_TEMPO_COEFF_MULT_LCD_DISP 2.5
#define TIME_DEFAULT_TEMPO_COEFF_MULT_PID_LOOP 10.0

/*
#define TIME_DEFAULT_TEMPO_COEFF_MULT_RAQ      0.75
#define TIME_DEFAULT_TEMPO_COEFF_MULT_IR       0.75
#define TIME_DEFAULT_TEMPO_COEFF_MULT_TERMIOS  0.75
#define TIME_DEFAULT_TEMPO_COEFF_MULT_CAPTEURS 0.75
#define TIME_DEFAULT_TEMPO_COEFF_MULT_LCD_LOOP 10.0
#define TIME_DEFAULT_TEMPO_COEFF_MULT_LCD_DISP 2.5
#define TIME_DEFAULT_TEMPO_COEFF_MULT_PID_LOOP 10.0
*/
/* Definition de la structure associeee au fichier avant inclusion astro_global.h */
/* Ceci est permis par le pre processeur */

struct STR_TIME_TEMPOS {
  pthread_mutex_t tpo_mutex ;
  unsigned long   tpo_ir        ; 
  unsigned long   tpo_menu      ;
  unsigned long   tpo_voute     ; /* non utilise pour l instant (vou_tempo a la place )*/
  unsigned long   tpo_raq       ;
  unsigned long   tpo_termios   ; 
  unsigned long   tpo_capteurs  ; 
  unsigned long   tpo_lcd_loop  ;
  unsigned long   tpo_lcd_disp  ;
  unsigned long   tpo_pid_loop  ;
} ;
typedef struct STR_TIME_TEMPOS STRUCT_TIME_TEMPOS ;

/* Parametre hd = L' heure decimale sert pour la representation 
  * d'un angle sous la forme  : 12h25mn05s ( 360 degres = 24h )
  */



struct STR_TIME_PARAMS {

  pthread_mutex_t tim_par_mutex ;
  unsigned long   tim_par_tpo ;
  unsigned long   tim_par_tpo_ir ;
  unsigned long   tim_par_tpo_menu ;
  unsigned long   tim_par_tpo_raq ;
  unsigned long   tim_par_tpo_termios ;
  unsigned long   tim_par_tpo_capteurs ;
  unsigned long   tim_par_tpo_lcd_loop ;
  unsigned long   tim_par_tpo_lcd_disp ;
  unsigned long   tim_par_tpo_pid_loop ;
} ;
typedef struct STR_TIME_PARAMS STRUCT_TIME_PARAMS ;

// ------ TEMPORISATIONS DES BOUCLES ------------

       void TIME_INIT                      ( STRUCT_TIME * ) ;
       void TIME_RELEASE                   ( STRUCT_TIME * ) ;
static void TIME_DISPLAY_2                 ( STRUCT_TIME * ) ;

static void TIME_DISPLAY_PREPARE ( STRUCT_TIME * ) ;
static void TIME_DISPLAY         ( STRUCT_TIME * ) ;
static void TIME_UNLOCK          ( STRUCT_TIME * ) ;
static void TIME_LOCK            ( STRUCT_TIME * ) ;
static void TIME_LOG             ( STRUCT_TIME * ) ;

void   TIME_INIT_TEMPOS               ( STRUCT_TIME_TEMPOS * ) ;
void   TIME_PARAMS_INIT               ( STRUCT_TIME_PARAMS * ) ;
void   TIME_PARAMS_DISPLAY            ( STRUCT_TIME_PARAMS * ) ;

void   TIME_TEMPOS_CONFIG             ( STRUCT_TIME_TEMPOS * ) ;
void   TIME_TEMPOS_DISPLAY            ( STRUCT_TIME_TEMPOS * ) ;

int    TIME_CALCULS_SIDERAL_TIME      ( STRUCT_TIME * , STRUCT_LIEU * ) ;
int    TIME_CALCULS_JULIAN_DAY        ( STRUCT_TIME * , STRUCT_LIEU * ) ;
int    TIME_CALCULS_LOCAL_DATE        ( STRUCT_TIME *  ) ;
void   TIME_CALCULS_HMS_VERS_DEC      ( STRUCT_TIME *  ) ;
void   TIME_CALCULS_DEC_VERS_HMS      ( STRUCT_TIME *  ) ;
void   TIME_DISPLAY_MSG_HHMMSS        ( STRUCT_TIME * , char * ) ;
void   TIME_CALCULS_HMS_VERS_DEC_DIR  ( double *, double, double, double) ;

double TIME_CALCULS_DUREE_SECONDES    ( struct timeval * )  ;
double TIME_CALCULS_DUREE_MICROSEC    ( struct timeval * ) ;
double TIME_CALCULS_DUREE_NANOSEC     ( struct timeval * ) ;
long   TIME_TEMPORISATION_MICROSEC    ( struct timeval , double , double ) ;


void   TIME_SET_YEAR_MONTH_AND_DAY    ( char *  ) ; // FIXME ajout 20190822
void   TIME_SET_MONTH_AND_DAY         ( char *  ) ;
void   TIME_SET_HOUR_AND_MINUTES      ( char *  ) ;

#endif

/* =====================================================================================
 *
 * FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - 
 * 
 * ===================================================================================== */

/* ---------------------------------------------------------------------------------------
 man date sur noyau compile 
 Display time (using +FMT), or set time

	[-s,--set] TIME	Set time to TIME
	-u,--utc	Work in UTC (don't convert to local time)
	-R,--rfc-2822	Output RFC-2822 compliant date string
	-I[SPEC]	Output ISO-8601 compliant date string
			SPEC='date' (default) for date only,
			'hours', 'minutes', or 'seconds' for date and
			time to the indicated precision
	-r,--reference FILE	Display last modification time of FILE
	-d,--date TIME	Display TIME, not 'now'
	-D FMT		Use FMT for -d TIME conversion

Recognized TIME formats:
	hh:mm[:ss]
	[YYYY.]MM.DD-hh:mm[:ss]
	YYYY-MM-DD hh:mm[:ss]
	[[[[[YY]YY]MM]DD]hh]mm[.ss]
	'date TIME' form accepts MMDDhhmm[[YY]YY][.ss] instead
--------------------------------------------------------------------------------------- */