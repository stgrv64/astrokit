/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 21/03/2022  | * ajout entete
#               * ajout const TERMIOS_ESCAPE_CHAR
# mars 2023   | * ajout ter_nread
# mars 2023   | * ajout mode enter et quit du mode TEMRIOS 
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

typedef struct STR_TERMIOS STRUCT_TERMIOS ;

struct STR_TERMIOS {
  pthread_mutex_t  ter_mutex ;
  STR_EXT_TIMEVAL  ter_tval ; 
  FILE            *ter_file ; 
  int              ter_loglevel ;
  void           (*ter_lock)       ( STRUCT_TERMIOS * ) ;
  void           (*ter_unlock)     ( STRUCT_TERMIOS * ) ;  
  void           (*ter_log)        ( STRUCT_TERMIOS * ) ;
  void           (*ter_display)    ( STRUCT_TERMIOS * ) ;
  void           (*ter_enter)      ( STRUCT_TERMIOS * ) ;
  void           (*ter_quit)       ( STRUCT_TERMIOS * ) ;    
  char             ter_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char             ter_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;

  STR_EXT_TERMIOS  ter_config_initiale; 
  STR_EXT_TERMIOS  ter_config_current ;   
  char             ter_buffer    [ TERMIOS_KBHIT_SIZE_BUFFER_READ ] ; 
  int              ter_sum_ascii ;
  int              ter_peek_char  ; 
  int              ter_nread ;
};

void KEYBOARD_INIT             (STRUCT_TERMIOS *) ;
void KEYBOARD_READ             (STRUCT_TERMIOS *) ;
void KEYBOARD_TERMIOS_INIT     (STRUCT_TERMIOS *) ;
void KEYBOARD_TERMIOS_EXIT     (STRUCT_TERMIOS *) ;


int  KEYBOARD_TERMIOS_READCH   (STRUCT_TERMIOS *) ;
int  KEYBOARD_TERMIOS_READ     (STRUCT_TERMIOS *) ;

/* Nouvelles fonctions 2022/2023 pour lire la somme des nread 
   pour certain caracteers */

int  KEYBOARD_TERMIOS_KBHIT_READ_CHAR  ( STRUCT_TERMIOS * ) ; 
int  KEYBOARD_TERMIOS_KBHIT_READ_CHARS ( STRUCT_TERMIOS * ) ;

/* Anciennes fonctions ncurses a revoir lors du placement dans un thread poxix :
   en attendant on utilise TERMIOS */

void KEYBOARD_NCURSES_READ     (void) ;
void KEYBOARD_NCURSES_INIT     (void) ;

void KEYBOARD_END              (void) ;
void KEYBOARD_LOG_LAST_LINE    (WINDOW *) ;

#endif
