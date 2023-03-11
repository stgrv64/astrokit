/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 17/01/2022  | * creation
# -------------------------------------------------------------- 
*/

#include "astro_keyboard.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_TERMIOS *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void KEYBOARD_TERMIOS_DISPLAY_PREPARE ( STRUCT_TERMIOS * lp_Ter) {

  TraceArbo(__func__,2,"astre format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ter->ter_mutex ) ;

  sprintf( lp_Ter->ter_dis_cmd , STR_TER_FORMAT_0,\
    lp_Ter->ter_buffer, \
    lp_Ter->ter_sum_ascii ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ter->ter_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static KEYBOARD_TERMIOS_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_TERMIOS *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void KEYBOARD_TERMIOS_DISPLAY( STRUCT_TERMIOS *lp_Ter) {

  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,2,"display informations on Astre") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  KEYBOARD_TERMIOS_DISPLAY_PREPARE( lp_Ter ) ;

  ASTRO_GLOBAL_LOG_ON ( &  lp_Ter->ter_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Ter->ter_loglevel , 1 , "%s", lp_Ter->ter_dis_cmd ) ;
  ASTRO_GLOBAL_LOG_OFF( &  lp_Ter->ter_loglevel ) ;

  return ;
}
/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_TERMIOS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void KEYBOARD_TERMIOS_LOCK ( STRUCT_TERMIOS * lp_Ter) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ter->ter_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_LOG
* @author : s.gravois
* @brief  : Log la structure en parametre 
* @param  : STRUCT_TERMIOS *
* @date   : 2023-01-24 creation
* @todo   : (a completer )
*****************************************************************************************/

static void KEYBOARD_TERMIOS_LOG ( STRUCT_TERMIOS * lp_Ter) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ter->ter_mutex ) ;

  /* TODO : complete */ 

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ter->ter_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_TERMIOS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void KEYBOARD_TERMIOS_UNLOCK ( STRUCT_TERMIOS * lp_Ter) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ter->ter_mutex ) ;

  return ;
}

/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_MODE_ENTER
* @author : s.gravois
* @brief  : entre dans le mode TERMIOS pour le terminal
* @param  : STRUCT_TERMIOS * 
* @date   : 2023-03-11 creation
* @todo   : 
*****************************************************************************************/

static void KEYBOARD_TERMIOS_MODE_ENTER(STRUCT_TERMIOS * lp_Ter) {

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  lp_Ter->ter_lock( lp_Ter ) ; 

  lp_Ter->ter_config_current.c_cc[VMIN]   = 0 ;
  tcsetattr(0, TCSANOW, &lp_Ter->ter_config_current ) ; 

  lp_Ter->ter_nread = 0 ;
  lp_Ter->ter_peek_char = '0' ;
  lp_Ter->ter_sum_ascii = 0 ;
  memset( lp_Ter->ter_buffer , 0, sizeof( lp_Ter->ter_buffer )) ;

  lp_Ter->ter_unlock( lp_Ter ) ;

 return ;
}

/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_MODE_QUIT
* @author : s.gravois
* @brief  : quitte dans le mode TERMIOS pour le terminal
* @param  : STRUCT_TERMIOS * 
* @date   : 2023-03-11 creation
* @todo   : 
*****************************************************************************************/

static void KEYBOARD_TERMIOS_MODE_QUIT(STRUCT_TERMIOS * lp_Ter) {

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  lp_Ter->ter_lock( lp_Ter ) ; 

  /* lp_Ter->ter_config_current.c_cc[VMIN]   = 1 ; */
  tcsetattr(0, TCSANOW, &lp_Ter->ter_config_initiale ) ; 

  lp_Ter->ter_unlock( lp_Ter ) ;

 return ;
}

/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_INIT
* @author : s.gravois
* @brief  : sauvegarde la structure termios initiale (ouverture)
* @param  : STRUCT_TERMIOS *
* @date   : 2022-01-18 creation
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

void KEYBOARD_TERMIOS_INIT(STRUCT_TERMIOS * lp_Ter) {

  TraceArbo(__func__,0,"init termios") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Ter->ter_mutex ) ;
                                     lp_Ter->ter_log      = KEYBOARD_TERMIOS_LOG ;
                                     lp_Ter->ter_lock     = KEYBOARD_TERMIOS_LOCK ;
                                     lp_Ter->ter_unlock   = KEYBOARD_TERMIOS_UNLOCK ;
                                     lp_Ter->ter_display  = KEYBOARD_TERMIOS_DISPLAY ;
                                     lp_Ter->ter_enter    = KEYBOARD_TERMIOS_MODE_ENTER ;
                                     lp_Ter->ter_quit     = KEYBOARD_TERMIOS_MODE_QUIT ;
                                     lp_Ter->ter_loglevel = 0 ; 
                                     lp_Ter->ter_file     = NULL ;
  gettimeofday (                   & lp_Ter->ter_tval, NULL ) ;

  tcgetattr(0,&lp_Ter->ter_config_initiale); 

  /* Recuperation de la config TERMIOS  inititiale & sauvegarde */

  lp_Ter->ter_config_current = lp_Ter->ter_config_initiale ; 

  /* Modification de la config TERMIOS qui va servir ensuite comme "courante" */

  lp_Ter->ter_config_current.c_lflag     &= ~ICANON ;
  lp_Ter->ter_config_current.c_lflag     &= ~ECHO ;
  lp_Ter->ter_config_current.c_lflag     &= ~ISIG ;
  lp_Ter->ter_config_current.c_cc[VMIN]   = 1 ;
  lp_Ter->ter_config_current.c_cc[VTIME]  = 0 ;

  tcsetattr(0, TCSANOW, &lp_Ter->ter_config_initiale ) ; 

  lp_Ter->ter_nread = 0 ;
  lp_Ter->ter_peek_char = '0' ;
  lp_Ter->ter_sum_ascii = 0 ;
  memset( lp_Ter->ter_buffer , 0, sizeof( lp_Ter->ter_buffer )) ;

  return ;
}

/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_EXIT
* @author : s.gravois
* @brief  : retablit la structure termios initiale (fermeture)
* @param  : STRUCT_TERMIOS * 
* @date   : 2022-01-18 creation
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

void KEYBOARD_TERMIOS_EXIT(STRUCT_TERMIOS * lp_Ter) {

 tcsetattr(0, TCSANOW, &lp_Ter->ter_config_initiale ) ;

 return ;
}

/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_KBHIT_READ_CHARS
* @author : s.gravois
* @brief  : detecte la frappe d'une touche sur le clavier via termios
* @param  : STRUCT_TERMIOS * lp_Ter
* @date   : 2022-01-18 creation
* @date   : passage du buffer de lecture de char a char[]
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
* @todo   : eviter les traces a l interieur de cette fonction
*****************************************************************************************/

int KEYBOARD_TERMIOS_KBHIT_READ_CHARS(STRUCT_TERMIOS * lp_Ter) {

  // char lc_buf[ TERMIOS_KBHIT_SIZE_BUFFER_READ ] ;
  char ch ;
  int nread = 0 ; 

  lp_Ter->ter_enter(lp_Ter) ;

  lp_Ter->ter_nread = read(0,&(lp_Ter->ter_buffer),TERMIOS_KBHIT_SIZE_BUFFER_READ);
  
  lp_Ter->ter_quit(lp_Ter) ;

  /* UNLOCK ---------------------------------- */
  
  if ( lp_Ter->ter_nread > 0  ) {

    // Trace("nread positif = %d", nread) ;
    
    lp_Ter->ter_lock(lp_Ter) ;

    for(int i=0;i<TERMIOS_KBHIT_SIZE_BUFFER_READ;i++) {
      lp_Ter->ter_sum_ascii+=(int)lp_Ter->ter_buffer[i] ;
    }
    
    lp_Ter->ter_unlock(lp_Ter) ;

    return nread ;
  }
  else {
    return -1 ;
  }
  return 0 ;
}
/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_READCH
* @author : s.gravois
* @brief  : lit le prochain caractere dans le terminal 
* @param  : 
* @date   : 2022-01-18 creation
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

int KEYBOARD_TERMIOS_READCH(STRUCT_TERMIOS * lp_Ter) {
  char ch ;
  int nread =0 ; 

  if(lp_Ter->ter_peek_char != -1 ) {
    ch = lp_Ter->ter_peek_char ;
    lp_Ter->ter_peek_char = -1 ;
    // Trace("ch = %c (lp_Ter->ter_peek_char != -1)", ch) ;
    return ch ;
  }

  // nread = read(0, &lc_buf, TERMIOS_KBHIT_SIZE_BUFFER_READ );
  nread = read( 0, &ch, 1)  ;

  // Trace("ch = %c nread = %d", ch, nread) ;
  return ch ;
}


/*****************************************************************************************
* @fn     : KEYBOARD_LOG_LAST_LINE
* @author : s.gravois
* @brief  : log les traces de ncurses dans un fichier de log
* @param  : 
* @date   : 2022-01-18 creation
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

void KEYBOARD_LOG_LAST_LINE(WINDOW *win) {

  FILE *fp;

  if ((fp = fopen(MY_LOGFILE, "a")) != 0) {
  
    char temp[256];
    int y, x, n;
    int need = sizeof(temp) - 1;

    if (need > COLS)
      need = COLS;
    getyx(win, y, x);
    wmove(win, y - 1, 0);
    n = winnstr(win, temp, need);
    while (n-- > 0) {
      if (isspace(UChar(temp[n])))
    temp[n] = CONFIG_ZERO_CHAR;
      else
    break;
    }
    wmove(win, y, x);
    fprintf(fp, "%s\n", temp);
    fclose(fp);
  }
}

/*****************************************************************************************
* @fn     : KEYBOARD_NCURSES_INIT
* @author : s.gravois
* @brief  : initialisation generique ncurses
* @param  : 
* @date   : 2022-01-18 creation
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

void KEYBOARD_NCURSES_INIT(void) {
  int n = 0 ;
  // Trace1("start") ;
  /* unlink(MY_LOGFILE); */

  if (newterm(0, stdout, stdin) == 0) {
    fprintf(stderr, "Cannot initialize terminal\n");
    // Trace1("Cannot initialize terminal") ;
    exit(2);
  }
  cbreak();		     /* take input chars one at a time, no wait for \n */
  noecho();		     /* don't echo input */
  scrollok(stdscr, FALSE);
  keypad(stdscr, FALSE );
  move(50, 50);

  /* we do the define_key() calls after keypad(), since the first call to
   * keypad() initializes the corresponding data.
   */
  for (n = 0; n < 255; ++n) {
    char temp[10];
    _nc_SPRINTF(temp, _nc_SLIMIT(sizeof(temp)) "\033%c", n);
    define_key(temp, n + MY_KEYS);
  }
  for (n = KEY_MIN; n < KEY_MAX; ++n) {

    char *value;
    
    if ((value = keybound(n, 0)) != 0) {
        size_t need = strlen(value) + 2;
        char *temp = typeMalloc(char, need);
        _nc_SPRINTF(temp, _nc_SLIMIT(need) "\033%s", value);
        define_key(temp, n + MY_KEYS);
        free(temp);
        free(value);
    }
  }
}

/*****************************************************************************************
* @fn     : KEYBOARD_NCURSES_READ
* @author : s.gravois
* @brief  : lecture generique ncurses
* @param  : 
* @date   : 2022-01-18 creation
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

void KEYBOARD_NCURSES_READ(void) {

    const char * name ;
    bool escaped = FALSE;
    
    int ch = 0;    
    struct timeval previous;

    gettimeofday(&previous, 0);

    ch = getch() ;
    if ( ch == ERR ) return ;

    escaped = (ch >= MY_KEYS);
    name = keyname(escaped ? (ch - MY_KEYS) : ch);
    int          secs, msecs;
    struct timeval current;

    gettimeofday(&current, 0);

    secs = (int) (current.tv_sec - previous.tv_sec);
    msecs = (int) ((current.tv_usec - previous.tv_usec) / 1000);
    if (msecs < 0) {
        msecs += 1000;
        --secs;
    }
    if (msecs >= 1000) {
        secs += msecs / 1000;
        msecs %= 1000;
    }
    printw("%6d.%03d ", secs, msecs);
    previous = current;

    printw("Keycode %d, name %s%s\n",
        ch, \
        escaped ? "ESC-" : "", \
        name != 0 ? name : "<null>" \
    );

    KEYBOARD_LOG_LAST_LINE(stdscr); 
        
    clrtoeol();
}
/*****************************************************************************************
* @fn     : KEYBOARD_END
* @author : s.gravois
* @brief  : fermeture generique ncurses
* @param  : 
* @date   : 2022-01-18 creation
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

void KEYBOARD_END(void) {

  endwin();
}
/* Fin fichier - ----------------------------------------------
----------------------------------------------
----------------------------------------------
---------------------------------------------- */


/*****************************************************************************************
* @fn     : main
* @author : s.gravois
* @brief  : point de depart du programme 
* @param  : STRUCT_TERMIOS *
* @date   : 2022-01-18 creation
* @date   : 2023-03-11 ajout commentaire (non utilise)
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
* @todo   : (non utilise)
*****************************************************************************************/
/*
int KEYBOARD_TERMIOS_READ(STRUCT_TERMIOS * lp_Ter) {

  int ch =0 ;

  KEYBOARD_TERMIOS_INIT(lp_Ter) ;

  while(ch!='q') {
    // printf("boucle en cours\n") ;
    usleep(50000) ;

    if ( KEYBOARD_TERMIOS_KBHIT_READ_CHAR(lp_Ter)) {

      ch= KEYBOARD_TERMIOS_READCH(lp_Ter) ;
      
      Trace("keycode %c => %d\n", ch, ch) ;
    }
  }
  KEYBOARD_TERMIOS_EXIT(lp_Ter) ;
  exit(0) ;
}
*/