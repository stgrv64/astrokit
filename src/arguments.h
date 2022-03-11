/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# Fichier     | arguments.h
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 19/01/2022  | * ajout entete
#               * ajouts fonctions utilisant getopt
# 11/03/2022  | * ajout fonction ARGUMENTS_GERER_REP_HOME
# -------------------------------------------------------------- 
*/

#ifndef ARGUMENTS_H
#define ARGUMENTS_H

// INCLUDES =====================================
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
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

//#include <arguments.h>
#include <astro.h>
#include <calculs.h>
#include <cat.h>
#include <config.h>
#include <gpio.h>
#include <i2c.h>
#include <ir.h>
#include <stat.h>
#include <types.h>

// FIN INCLUDES =====================================

#define ARGUMENTS_VARG_STRING  115
#define ARGUMENTS_VARG_INT         100
#define ARGUMENTS_VARG_CHAR     99
#define ARGUMENTS_FREQ_TRAME_INFRAROUGE 10000000.0

int incrlog ;

//extern void TRAP_MAIN(int sig) ;
/* janvier 2022 : 
  remplacements des arguments de type var glob par void 
  on considere que ASTRE* VOUTE* etc.. sont dorenavant var glob
*/
void  ARGUMENTS_HELP                   (int , char** ) ;
void  ARGUMENTS_GERER_FACON_CLASSIQUE  (int , char** )  ;
void  ARGUMENTS_GERER_GETOPT           (int , char** )  ;
void  ARGUMENTS_GERER_REP_HOME         (int , char** ) ;

void  GPIO_TEST_MOTEURS      (void ) ;
void  ARGUMENTS_VOUTE        (void) ;

#endif

//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================


