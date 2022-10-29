/* #define _GNU_SOURCE */

#ifndef ASTRO_MAIN_H
#define ASTRO_MAIN_H

#include "astro_global.h"

void   ASTRO_TRAP_MAIN(int ) ;
void   ASTRO_TRAP_SUIVI_MENU(int )  ;
void   ASTRO_TRAP_SUIVI_VOUTE(int )  ;
void   ASTRO_TRAP_SUIVI_INFRAROUGE(int )  ;
void   ASTRO_TRAP_SUIVI_CAPTEURS(int )  ;
void   ASTRO_TRAP_SUIVI_CLAVIER(int )  ;

void * SUIVI_MENU(STRUCT_SUIVI * ) ;
void * SUIVI_VOUTE(STRUCT_SUIVI * ) ;
void * SUIVI_INFRAROUGE(STRUCT_SUIVI * ) ;
void * SUIVI_CLAVIER_getchar( STRUCT_SUIVI *  ) ;
void * SUIVI_CLAVIER_TERMIOS( STRUCT_SUIVI *  ) ;
void * SUIVI_CLAVIER_NCURSES(STRUCT_SUIVI*  ) ;
void * SUIVI_CAPTEURS(STRUCT_SUIVI * ) ;
void * SUIVI_CLAVIER_1(STRUCT_SUIVI * ) ;
void * SUIVI_CLAVIER_0(STRUCT_SUIVI * ) ;


extern void SOLAR_SYSTEM( \
 char   * nom, \
 double * ASC, \
 double * H, \
 double * a, \
 double * h, \
 double   lat, \
 double   lon, \
 double   alt, \
 int      annee, \
 int      mois, \
 int      jour, \
 int      heure, \
 int      minute, \
 int      seconde, \
 int      num ) ;

#endif
