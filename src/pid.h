#define _GNU_SOURCE

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-06-01  | * creation
# 2022-06-01  | * rapatriement code depuis config.h
# -------------------------------------------------------------- 
*/

#ifndef PID_H
#define PID_H

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
#include <semaphore.h>
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
#include <config.h>
#include <gpio.h>
#include <i2c.h>
#include <ir.h>
#include <stat.h>
#include <types.h>
#include <pthreads.h>
/* #include <lcd.h>
#include <pid.h> */

#define PID_ERR_PRE_INIT 56.245856

void   PID_INIT             (PID *, double, double, double, double) ;
void   PID_RESET         ( void)  ;
void   PID_TEST         ( void)  ;
void   PID_ACTIVATE         ( void) ;
int    PID_IS_IN_CONDITION  ( void) ;
void   PID_CALCULATE        ( double, double ) ;

#endif