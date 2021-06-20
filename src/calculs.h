#ifndef CALCULS_H
#define CALCULS_H

// INCLUDES =====================================
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <types.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <linux/i2c-dev.h>
#include <lirc_client.h>

#include <arguments.h>
#include <astro.h>
//#include <calculs.h>
#include <cat.h>
#include <config.h>
#include <gpio.h>
#include <i2c.h>
#include <ir.h>
#include <stat.h>
#include <types.h>

// FIN INCLUDES =====================================

#define  SIDERAL      86164.0             // duree du jour sideral en seconde
#define  PI           3.141592653589793238462
#define  OMEGA        0.000072921235169903748
#define  SIN45        0.70710678118654
#define  COS45        0.70710678118654
#define  DEGRES       57.29577951308232
#define  ANGLE_S      15.04108444361914   // angle en secondes parcouru pendant une seconde a l'equateur
#define  ROT_RAD_SEC  0.000072921235170   // vitesse de rotation de la Terre (radians par seconde)
#define  ROT_DEG_SEC  0.004178079012116   // vitesse de rotation de la Terre (degres par seconde)

// ------------------------------------------------------------------------
// macros de calcul 
// ------------------------------------------------------------------------

double sqr(double x)                                    ;
double max(double x1,double x2)                         ;
double SGN(double x)                                    ;
double DEC  (double LAT, double a, double h)            ;
double ASC  (double H,   double a, double h)            ;
double ASC1 (double LAT, double H, double a, double h)  ;
double ASC2 (double LAT, double H, double h)            ;
double ALT  (double LAT, double A, double H)            ;
double AZI  (double A, double H, double h)              ;
double AZI1 (double LAT, double A, double H, double h)  ;
double RAD  (int degres, int minutes )                  ;
double DEG  (int degres, int minutes )                  ;

// Ces deux fonctions permettent une temporisation precises (microsedondes) 
// en profitant d'un sleep parametrable (eviter une consommation CPU
// avant de rentrer en boucle active d'attente

long   CALCUL_TEMPORISATION_VOUTE     ( VOUTE *voute, struct timeval t00) ;
long   CALCUL_TEMPORISATION_MICROSEC  ( double microsecondes, double percent, struct timeval t00) ;
void   CALCUL_TEMPORISATION_ALTITUDE  ( SUIVI *suivi, struct timeval * pt00)  ;
void   CALCUL_TEMPORISATION_AZIMUT    ( SUIVI *suivi, struct timeval * pt00) ;

void   CALCUL_GEODE                   ( ASTRE *astre) ;
void   CALCUL_AZIMUT                  ( LIEU *lieu, ASTRE *astre) ;
void   CALCUL_EQUATEUR                ( LIEU *lieu, ASTRE *astre) ;

void   CALCUL_VITESSES_EQUATORIAL     (ASTRE *as) ;
void   CALCUL_VITESSES                ( LIEU *lieu, ASTRE *astre, SUIVI *suivi) ;

void   CALCUL_ANGLE_HORAIRE           ( LIEU *lieu, ASTRE *astre) ;

void   CALCUL_TOUT                    (LIEU* lieu, TEMPS *temps, ASTRE *astre, SUIVI *suivi, CLAVIER *clavier ) ;

void   CALCUL_D                       ( ASTRE *astre, SUIVI *suivi) ;   
void   CALCUL_PERIODE                 ( ASTRE *astre, SUIVI* suivi,VOUTE *voute)  ;
void   CALCUL_PERIODES_SUIVI_MANUEL   ( ASTRE *astre, SUIVI* suivi, VOUTE *voute) ;

int    CALCUL_TEMPS_SIDERAL           ( LIEU* lieu, TEMPS *temps ) ;
int    CALCUL_JOUR_JULIEN             ( LIEU* lieu, TEMPS *temps) ;
int    CALCUL_DATE                    ( TEMPS *temps ) ;
void   CALCUL_HDEC                    ( TEMPS *temps ) ;
void   CALCUL_HMS                     ( TEMPS *temps ) ;

void   CALCUL_CONVERSION_ANGLE_EN_TEMPS ( ASTRE *astre) ;
void   CALCUL_AFFICHER_HEURE            ( char * mesg, TEMPS *temps ) ;
 
void   SET_ASTRE( ASTRE *as,char *parametre, double valeur) ;
void   SET_LIEU(  LIEU *lieu,char *parametre, double valeur) ;
void   SET_VOUTE( VOUTE *voute,char *parametre, double valeur) ;

double CALCUL_DUREE_MICROSEC           (struct timeval *) ;
double CALCUL_DUREE_NANOSEC            (struct timeval *) ;

#endif
