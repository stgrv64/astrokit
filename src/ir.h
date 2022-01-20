#ifndef IR_H
#define IR_H

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
#include <config.h>
#include <gpio.h>
#include <i2c.h>
//#include <ir.h>
#include <stat.h>
#include <types.h>

// FIN INCLUDES =====================================

#define IR_GAP          120000
#define IR_SEPARATEUR   '='
#define IR_BUFFER_SIZE  255 
#define IR_NB_CODES     50 
#define IR_ZERO_CHAR    '\0'

// c est le code min et max des touches autorisees a etre repetees 
// pour que ces touches soient prises en compte rapidement et de 
// maniere repetitive  = touches de deplacement N S E O 

#define IR_CODE_REPETE_AUTORISE_MIN 21 
#define IR_CODE_REPETE_AUTORISE_MAX 24

//------------------------------------------------------------------------------
typedef struct  {
  
  char code[ IR_NB_CODES][IR_BUFFER_SIZE] ;
  char key [ IR_NB_CODES][IR_BUFFER_SIZE] ;
  
}
IR_LIRC_CODES ;
//------------------------------------------------------------------------------

struct lirc_config *lircconfig;
IR_LIRC_CODES ir_codes, *irc ;

//------------------------------------------------------------------------------
// Prototype des fonctions
//------------------------------------------------------------------------------

int   LIRC_OPEN(struct lirc_config *lircconfig) ;
void  LIRC_CLOSE(struct lirc_config *lircconfig) ;
void  LIRC_READ(SUIVI *suivi) ;

void  LIRC_CONFIG_CODES(IR_LIRC_CODES *irc) ;

void  IR_KEYBOARD_READ(SUIVI *suivi, CLAVIER *clavier) ;
void  IR_KEYBOARD_MAJ_SUIVI( SUIVI *suivi)  ;
void  IR_ACTIONS_PARTICULIERES( SUIVI *suivi) ;

#endif
