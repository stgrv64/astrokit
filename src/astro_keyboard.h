/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 21/03/2022  | * ajout entete
#               * ajout const TERMIOS_ESCAPE_CHAR
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_KEYBOARD_H
#define ASTRO_KEYBOARD_H

#include "astro_global.h"

#define TERMIOS_ESCAPE_CHAR            27
#define TERMIOS_CHARIAGE_RETURN_CHAR   10
#define TERMIOS_KBHIT_SIZE_BUFFER_READ 5

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

void KEYBOARD_TERMIOS_INIT     (void) ;
void KEYBOARD_TERMIOS_EXIT     (void) ;
int  KEYBOARD_TERMIOS_KBHIT    (void) ; 
int  KEYBOARD_TERMIOS_KBHIT_NEW(char *, int * ) ;
int  KEYBOARD_TERMIOS_READCH   (void) ;
int  KEYBOARD_TERMIOS_READ     (void) ;

void KEYBOARD_NCURSES_READ     (void) ;
void KEYBOARD_NCURSES_INIT     (void) ;

void KEYBOARD_READ             (void) ;
void KEYBOARD_INIT             (void) ;
void KEYBOARD_END              (void) ;
void KEYBOARD_LOG_LAST_LINE    (WINDOW *) ;

#endif