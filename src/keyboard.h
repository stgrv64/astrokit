#ifndef KEYBOARD_H
#define KEYBOARD_H

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
#include <ctype.h>
#include <ncurses.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <linux/i2c-dev.h>
#include <lirc_client.h>
/*
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
*/
/* #include <keyboard.h> */

#define NEED_TIME_H

#if defined(NCURSES_VERSION) && NCURSES_EXT_FUNCS

#define MY_LOGFILE  "demo_altkeys.log"
#define MY_KEYS     (KEY_MAX + 1)
#define UChar(c)    ((unsigned char)(c))

#ifdef __cplusplus
#define NCURSES_VOID        /* nothing */
#else
#define NCURSES_VOID (void)
#endif
  
#if USE_STRING_HACKS && HAVE_SNPRINTF
#define _nc_SPRINTF             NCURSES_VOID (snprintf)
#define _nc_SLIMIT(n)           NCURSES_CAST(size_t,n),
#else
#define _nc_SPRINTF             NCURSES_VOID (sprintf)
#define _nc_SLIMIT(n)       /* nothing */
#endif

#if defined(NCURSES_VERSION) && HAVE_NC_ALLOC_H
#include <nc_alloc.h>
#if HAVE_EXIT_TERMINFO && (defined(USE_TERMINFO) || defined(USE_TINFO))
#undef ExitProgram
#define ExitProgram(code) exit_terminfo(code)
#elif HAVE_EXIT_CURSES
#undef ExitProgram
#define ExitProgram(code) exit_curses(code)
#endif
#else /* not ncurses-tree */
  #define typeMalloc(type,n) (type *) malloc((size_t)(n) * sizeof(type))
  #define typeCalloc(type,elts) (type *) calloc((size_t)(elts), sizeof(type))
  #define typeRealloc(type,n,p) (type *) realloc(p, (size_t)(n) * sizeof(type))
#endif
#endif


void KEYBOARD_READ(void) ;


#endif
