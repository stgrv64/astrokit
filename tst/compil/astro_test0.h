
/* #define _GNU_SOURCE */

#ifndef ASTRO_TEST0_H
#define ASTRO_TEST0_H

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
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <linux/i2c-dev.h>

#include "astro_test1.h"

/* la declaration suivante permet de compiler avec la re*inclusion multiple */

/* typedef struct Str_Test1 STRUCT_TEST1 ; */

/* erreur sans le typedef : 
  f=astro_test1
  gcc -c $f.c -o $f.o
  In file included from astro_test1.h:29,
                   from astro_test1.c:11:
  astro_test0.h:44:42: error: unknown type name ‘STRUCT_TEST1’; did you mean ‘STRUCT_TEST0’?
     44 | void FctTest0_0(STRUCT_TEST0 * lp_Test0 , STRUCT_TEST1 * lp_Test1 ) ;
        |                                          ^~~~~~~~~~~~
        |                                          STRUCT_TEST0

*/


struct Str_Test2 {
 int    a ;
 int    b ;
 double c ; 
}  ;
typedef struct Str_Test2 STRUCT_TEST2 ;

struct Str_Test0 {
 struct Str_Test2  st_t2 ;
 struct Str_Test1 *pst_t1 ;
 int a ;
 int b ;
}  ;
typedef struct Str_Test0 STRUCT_TEST0 ;
typedef struct Str_Test1 STRUCT_TEST1 ;
/* typedef struct Str_Test0 STRUCT_TEST0 ; */

void FctTest0_0(STRUCT_TEST0 * , STRUCT_TEST1 * ) ;
void FctTest0_1(STRUCT_TEST0 * , STRUCT_TEST1 * ) ;

#endif






