#ifndef  CAT_H
#define  CAT_H


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
#include <calculs.h>
//#include <cat.h>
#include <config.h>
#include <gpio.h>
#include <i2c.h>
#include <ir.h>
#include <stat.h>
#include <types.h>

// FIN INCLUDES =====================================

#define   CAT_REP            "/root/catalogues"
#define   CAT_NGC_TXT        "ngc.csv"
#define   CAT_MES_TXT        "mes.csv"
#define   CAT_ETO_TXT        "eto.csv"
#define   CAT_NB_LIGNES      10000
#define   CAT_NB_COLONNES    6
#define   CAT_TAILLE_BUFFER  100
// ===========================================================================
char      cat           [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
char      cat_dec       [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
char      etoiles       [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
char      etoiles_dec   [CAT_NB_LIGNES]   [CAT_NB_COLONNES]   [CAT_TAILLE_BUFFER] ;
// ===========================================================================
int       CAT_FIN_MOT(char c) ;
void      CAT_AFFICHER(char catalogue[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;
void      CAT_READ(char * catalogue_txt, char datas[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;

void      CAT_FIND(ASTRE *astre, char cat[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;
void      CAT_FIND_OLD(char * catalogue_txt,ASTRE *objet,char *type) ;

void      CAT_FORMAT_DECIMAL( char * catalogue_txt, char datas[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER], \
                           char datas_dec[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;

void      CAT_FORMAT_DECIMAL_2( char * catalogue_txt, char datas    [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER], \
                           char datas_dec[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;
                           
void      CAT_ZONE(ASTRE *astre, double deg, char cat[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) ;
// ===========================================================================
#endif
