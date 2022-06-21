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

#ifndef LCD_H
#define LCD_H

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

void   LCD_INIT                      (LCD *) ;

void   LCD_DISPLAY_DEFAULT           ( void ) ;
void   LCD_DISPLAY_CURRENT           ( void ) ;

void   LCD_CHANGE_CURRENT            ( const int , const char* , const char * ) ;
void   LCD_CHANGE_DEFAULT            ( const int , const char* , const char * ) ;

void   LCD_DISPLAY_STRING_STRING     ( const int , const char* , const char * ) ;
void   LCD_DISPLAY_STRING_INT        ( const int , const char* , const int ) ;
void   LCD_DISPLAY_INT_INT           ( const int , const int, const int ) ;

void   LCD_DISPLAY_TEMPS_LIEU        ( const int ) ;
void   LCD_DISPLAY_ASTRE_VITESSES    ( const int ) ;

void   LCD_DISPLAY_AZI_ALT           ( const int ) ;
void   LCD_DISPLAY_AGH_DEC           ( const int ) ;
void   LCD_DISPLAY_ASC_DEC           ( const int ) ;

void   LCD_DISPLAY_MODE_STELLARIUM   ( const int ) ;

void   LCD_DISPLAY_CFG_GPIOS_ALT_AZI ( const int ) ;
void   LCD_DISPLAY_CFG_GPIOS_MAS_FRE ( const int ) ;
void   LCD_DISPLAY_CFG_GPIOS_LEDS    ( const int ) ;
void   LCD_DISPLAY_CFG_REDUCTION     ( const int ) ;

void   LCD_DISPLAY_AST_FREQUENCES    ( const int ) ;
void   LCD_DISPLAY_AST_PERIODES      ( const int ) ;

void   LCD_DISPLAY_INFORMATIONS      ( const int ) ;
void   LCD_DISPLAY_ACC_ALT_AZI       ( const int ) ;

#endif