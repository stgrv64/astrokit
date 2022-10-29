
/* #define _GNU_SOURCE */

#ifndef ASTRO_TEST1_H
#define ASTRO_TEST1_H

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

#include "astro_test0.h"

struct Str_Test1 {
 struct Str_Test1 *pst_t1 ;
 int a ;
 int b ;
}  ;

typedef struct Str_Test1 STRUCT_TEST1 ;

void FctTest1_0(STRUCT_TEST1 * lp_Test1 ) ;
void FctTest1_1(STRUCT_TEST1 * lp_Test1 ) ;

#endif










