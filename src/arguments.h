#ifndef ARGUMENTS_H
#define ARGUMENTS_H

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

void  ARGUMENTS_HELP(int argc, char** argv) ;
void  ARGUMENTS_GERER(int argc, char** argv,LIEU *lieu,ASTRE *astre, VOUTE *voute, SUIVI *suivi)  ;
void  ARGUMENTS_TEST_MOTEURS(SUIVI * suivi) ;
void  ARGUMENTS_VOUTE(LIEU *lieu,ASTRE *astre, VOUTE *voute, SUIVI *suivi ) ;

#endif
