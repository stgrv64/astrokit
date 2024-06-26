#ifndef ASTRO_H
#define ASTRO_H

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
//#include <astro.h>
#include <calculs.h>
#include <cat.h>
#include <config.h>
#include <gpio.h>
#include <i2c.h>
#include <ir.h>
#include <stat.h>
#include <types.h>

// FIN INCLUDES =====================================

void SUIVI_MENU_PREALABLE (SUIVI *suivi) ;
void SUIVI_TRAITEMENT_MOT( SUIVI *suivi, CLAVIER *clavier ) ;

void SUIVI_MANUEL_0(SUIVI * suivi, CLAVIER *clavier) ;
void SUIVI_MANUEL_1(SUIVI * suivi, CLAVIER *clavier)  ;

void * SUIVI_MENU(SUIVI * suivi) ;
void * SUIVI_VOUTE(SUIVI * suivi) ;
void * SUIVI_INFRAROUGE(SUIVI * suivi) ;
void * SUIVI_CAPTEURS(SUIVI * suivi) ;

// extern void SOLAR_SYSTEM(char * nom,double * a, double * h,double lat, double lon, double alt, double jd, int num) ;
void SOLAR_SYSTEM_NEW( char * nom, double * ASC, double * H, double * a, double * h, double lat, double lon, double alt, \
   int annee, int mois, int jour, int heure, int minute, int seconde, int num ) ;



#endif
