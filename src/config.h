/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | ajout entete
# 01/05/2021  | ajout fonction CONFIG_GETCWD
# -------------------------------------------------------------- 
*/

#ifndef CONFIG_H
#define CONFIG_H

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
#include <cat.h>
//#include <config.h>
#include <gpio.h>
#include <i2c.h>
#include <ir.h>
#include <stat.h>
#include <types.h>

// FIN INCLUDES =====================================

// CALCUL DE LA DIVISION DE LA FREQUENCE 
// f* = F / (2^n.D.R1.R2.R3.R4) = (1/(2.PI)).OMEGA
// OMEGA = ( 2.PI / 86164 ) rad.s-1

// FIXME : ajout 20191224 : fonction de log , a utiliser avec LOG*

void   CONFIG_INIT_LOG(void)  ;

void   CONFIG_INIT_CLAVIER(CLAVIER * clavier)  ;
void   CONFIG_INIT_LIEU(LIEU *lieu) ;
void   CONFIG_INIT_ASTRE(ASTRE *as ) ;
void   CONFIG_INIT_VOUTE(VOUTE *voute) ;
void   CONFIG_INIT_SUIVI(SUIVI *suivi) ;
void   CONFIG_INIT_TEMPS(TEMPS *temps) ;
void   CONFIG_INIT_DEVICES(DEVICES *devices) ;

void   CONFIG_SET_YEAR_MONTH_AND_DAY(char * s_data) ; // FIXME ajout 20190822
void   CONFIG_SET_MONTH_AND_DAY(char * s_data) ;
void   CONFIG_SET_HOUR_AND_MINUTES(char * s_data) ;

void   CONFIG_VOUTE( VOUTE *voute, double dt, double acc, double percent ) ;

int    CONFIG_FORMAT_ADMIS(char c) ;
int    CONFIG_FIN_LIGNE(char c) ;
int    CONFIG_DEB_COM(char c);
int    CONFIG_FIN_MOT(char c) ;
int    CONFIG_FIN_FICHIER(char c) ;
void   CONFIG_INIT_VAR(char datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) ;
void   CONFIG_AFFICHER_VARIABLES(void) ;
void   CONFIG_AFFICHER_DATAS(char datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) ;
int    CONFIG_READ(char datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) ;
int    CONFIG_GETCWD(char * ) ;

void   CONFIG_AFFICHER_TEMPS(TEMPS *temps) ; /* FIXME ajout 20191228 */
void   CONFIG_AFFICHER_ASTRE(ASTRE *as) ;
void   CONFIG_AFFICHER_LIEU(LIEU *lieu) ;
void   CONFIG_AFFICHER_CLAVIER(CLAVIER *clavier)  ;

void   CONFIG_AFFICHER_TOUT(void) ;          /* FIXME ajout 20191228 */
void   CONFIG_AFFICHER_CHANGEMENTS (void) ;  /* FIXME ajout 20200102 */
void   CONFIG_AFFICHER_DEVICES_USE (void) ;  /* FIXME ajout 20220312 */

void   CONFIG_AFFICHER_ASTRE_MODE_STELLARIUM(ASTRE *)  ;

#endif

//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================

