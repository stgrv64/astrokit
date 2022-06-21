#define _GNU_SOURCE

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | ajout entete
# 01/05/2021  | ajout fonction CONFIG_GETCWD
# 21/03/2022  | ajout fonctions initialisations des codes 
# 22/03/2022  | ajout fonction CONFIG_MAJ_SUIVI_PAS (anciennement 
#               dans ir.c / .h)
# 2022-04-28  | ajout de 2 resolutions plus simple (affichage contraint par LCD) :
# 2022-04-28  | - c_hhmm_*
# 2022-04-28  | - c_dd_*  
# 2022-05-02  | ajout fonction CONFIG_PATH_FIND
# 2022-05-24  | ajout fonction CONFIG_AFFICHER_ETAT_THREADS
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
#include <pthreads.h>
#include <lcd.h>
#include <pid.h>

// FIN INCLUDES =====================================

// CALCUL DE LA DIVISION DE LA FREQUENCE 
// f* = F / (2^n.D.R1.R2.R3.R4) = (1/(2.PI)).OMEGA
// OMEGA = ( 2.PI / 86164 ) rad.s-1

// FIXME : ajout 20191224 : fonction de log , a utiliser avec LOG*

void   CONFIG_INIT_LOG(void)  ;

void   CONFIG_INIT_CLAVIER ( CLAVIER * )  ;
void   CONFIG_INIT_LIEU    ( LIEU    * ) ;
void   CONFIG_INIT_ASTRE   ( ASTRE   *  ) ;
void   CONFIG_INIT_VOUTE   ( VOUTE   * ) ;
void   CONFIG_INIT_SUIVI   ( SUIVI   * ) ;
void   CONFIG_INIT_TEMPS   ( TEMPS   * ) ;
void   CONFIG_INIT_DEVICES ( DEVICES * ) ;

/* ajout mars 2022 */
/* fonctions anciennement dans ir.c /.h */

void  CONFIG_INIT_CODE     ( CODES *, int, const char * [][CONFIG_COD_NB_IN_OUT] ) ;
void  CONFIG_INIT_CODES    ( CODES *) ;
void  CONFIG_MAJ_SUIVI_PAS ( SUIVI *) ;
void  CONFIG_INPUTS_GESTION_APPUIS(SUIVI *, CLAVIER *) ;

/* fin ajout mars 2022 */

void   CONFIG_SET_YEAR_MONTH_AND_DAY(char * s_data) ; // FIXME ajout 20190822
void   CONFIG_SET_MONTH_AND_DAY(char * s_data) ;
void   CONFIG_SET_HOUR_AND_MINUTES(char * s_data) ;

void   CONFIG_VOUTE( VOUTE *gp_Vout, double dt, double acc, double percent ) ;

int    CONFIG_FORMAT_ADMIS(char c) ;
int    CONFIG_FIN_LIGNE(char c) ;
int    CONFIG_DEB_COM(char c);
int    CONFIG_FIN_MOT(char c) ;
int    CONFIG_FIN_FICHIER(char c) ;
void   CONFIG_INIT_VAR(char g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) ;
void   CONFIG_AFFICHER_VARIABLES(void) ;
void   CONFIG_AFFICHER_ETAT_THREADS(SUIVI* ) ;
void   CONFIG_AFFICHER_DATAS(char g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) ;
int    CONFIG_READ(char g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) ;
int    CONFIG_GETCWD(char * ) ;

void   CONFIG_AFFICHER_TEMPS(TEMPS * gp_Time) ; /* FIXME ajout 20191228 */
void   CONFIG_AFFICHER_MODE_LONG(ASTRE *gp_Astr) ;
void   CONFIG_AFFICHER_LIEU(LIEU *gp_Lieu) ;
void   CONFIG_AFFICHER_CLAVIER(CLAVIER *clavier)  ;

void   CONFIG_AFFICHER_TOUT(void) ;          /* FIXME ajout 20191228 */
void   CONFIG_AFFICHER_CHANGEMENTS (void) ;  /* FIXME ajout 20200102 */
void   CONFIG_AFFICHER_DEVICES_USE (void) ;  /* FIXME ajout 20220312 */

void   CONFIG_FORMATE_DONNEES_AFFICHAGE ( ASTRE * ) ;
void   CONFIG_AFFICHER_MODE_STELLARIUM  ( ASTRE * )  ;


int    CONFIG_PATH_FIND                    ( char * , char *) ;

#endif

//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================

