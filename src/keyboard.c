/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 17/01/2022  | * creation
# -------------------------------------------------------------- 
*/

#include <keyboard.h>

/*
@fn 
*/

static struct termios config_initiale; 
static struct termios config_finale ; 

static int peek_caractere = -1 ; 
static int peek_char[ TERMIOS_KBHIT_SIZE_BUFFER_READ ] = { -1 } ; 

/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_INIT
* @author : s.gravois
* @brief  : sauvegarde la structure termios initiale (ouverture)
* @param  : 
* @date   : 2022-01-18 creation
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

void KEYBOARD_TERMIOS_INIT() {

 tcgetattr(0,&config_initiale); 
 
 config_finale = config_initiale ; 
 
 config_finale.c_lflag &= ~ICANON ;
 config_finale.c_lflag &= ~ECHO ;
 config_finale.c_lflag &= ~ISIG ;
 config_finale.c_cc[VMIN]  = 1 ;
 config_finale.c_cc[VTIME] = 0 ;
 
 tcsetattr(0,TCSANOW, &config_initiale) ;
}

/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_EXIT
* @author : s.gravois
* @brief  : retablit la structure termios initiale (fermeture)
* @param  : 
* @date   : 2022-01-18 creation
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

void KEYBOARD_TERMIOS_EXIT() {
 tcsetattr(0, TCSANOW, &config_initiale ) ;
}

/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_KBHIT
* @author : s.gravois
* @brief  : detecte la frappe d'une touche sur le clavier via termios
* @param  : 
* @date   : 2022-01-18 creation
* @date   : passage du buffer de lecture de char a char[]
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

int KEYBOARD_TERMIOS_KBHIT() {
  char chaine[ TERMIOS_KBHIT_SIZE_BUFFER_READ ] ;
  char ch ;
  int nread ; 

  if ( peek_caractere != -1 ) {
    Trace("peek_caractere != -1") ;
    return 1 ;
  }
  config_finale.c_cc[VMIN] = 0 ;
  tcsetattr( 0, TCSANOW, &config_finale ) ;

  /* Lecture dans le file descriptor 0 (stdin) */

  nread = read(0, &chaine, TERMIOS_KBHIT_SIZE_BUFFER_READ );

  ch = chaine[0] ;
  config_finale.c_cc[VMIN] = 1 ;
  tcsetattr( 0, TCSANOW, &config_finale )  ;

  if ( nread >0  ) {
    Trace1("nread = %d", nread) ;
  }
  if (nread == 1 ) {
    peek_caractere = ch ; 
    Trace1("peek_caractere = ch = %c", ch) ;
    return -1 ;
  }
  return 0 ;
}

/*****************************************************************************************
* @fn     : KEYBOARD_TERMIOS_KBHIT_NEW
* @author : s.gravois
* @brief  : detecte la frappe d'une touche sur le clavier via termios
* @param  : 
* @date   : 2022-01-18 creation
* @date   : passage du buffer de lecture de char a char[]
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

int KEYBOARD_TERMIOS_KBHIT_NEW(char * ch_chaine, int * i_sum_ascii) {
  char chaine[ TERMIOS_KBHIT_SIZE_BUFFER_READ ] ;
  char ch ;
  int nread ; 

  *i_sum_ascii=0 ;
  memset(chaine, 0, sizeof(chaine)) ;

  if ( peek_char[0] != -1 ) {
    Trace("peek_char[0] != -1") ;
    return 1 ;
  }
  config_finale.c_cc[VMIN] = 0 ;
  tcsetattr( 0, TCSANOW, &config_finale ) ;

  /* Lecture dans le file descriptor 0 (stdin) */

  nread = read(0, &chaine, TERMIOS_KBHIT_SIZE_BUFFER_READ );

  config_finale.c_cc[VMIN] = 1 ;
  tcsetattr( 0, TCSANOW, &config_finale )  ;

  if ( nread >0  ) {

    Trace("nread positif = %d", nread) ;
    
    for(int i=0;i<TERMIOS_KBHIT_SIZE_BUFFER_READ;i++) {
      peek_char[i] = (int)chaine[i] ;
      Trace1("peek_chars[%d] = %c %d i_sum_ascii = %d", i, peek_char[i], (int) peek_char[i], *i_sum_ascii) ;
      *i_sum_ascii+=(int)peek_char[i] ;
    }
    strcpy( ch_chaine , chaine) ;  

    memset(peek_char, 0, sizeof(peek_char)) ;
    for(int i=0;i<TERMIOS_KBHIT_SIZE_BUFFER_READ;i++) {
      peek_char[i]=-1;
    }
    memset(chaine, 0, sizeof(chaine)) ;

    return -1 ;
  }
  else {
    Trace1("nread = %d", nread) ;
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

int KEYBOARD_TERMIOS_READCH() {
  char ch ;
  int nread =0 ; 

  if(peek_caractere != -1 ) {
    ch = peek_caractere ;
    peek_caractere = -1 ;
    Trace("ch = %c (peek_caractere != -1)", ch) ;
    return ch ;
  }

  // nread = read(0, &chaine, TERMIOS_KBHIT_SIZE_BUFFER_READ );
  nread = read( 0, &ch, 1)  ;

  Trace("ch = %c nread = %d", ch, nread) ;
  return ch ;
}

/*****************************************************************************************
* @fn     : main
* @author : s.gravois
* @brief  : point de depart du programme 
* @param  : 
* @date   : 2022-01-18 creation
* @todo   : verifier fonctionnement dans astrokit au milieu d'un thread
*****************************************************************************************/

int KEYBOARD_TERMIOS_READ(void) {

  int ch =0 ;

  KEYBOARD_TERMIOS_INIT() ;

  while(ch!='q') {
    // printf("boucle en cours\n") ;
    usleep(50000) ;
    if ( KEYBOARD_TERMIOS_KBHIT()) {
      ch= KEYBOARD_TERMIOS_READCH() ;
      printf("keycode %c => %d\n", ch, ch) ;
    }
  }
  KEYBOARD_TERMIOS_EXIT() ;
  exit(0) ;
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
    temp[n] = '\0';
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
  TRACE("start") ;
  /* unlink(MY_LOGFILE); */

  if (newterm(0, stdout, stdin) == 0) {
    fprintf(stderr, "Cannot initialize terminal\n");
    TRACE("Cannot initialize terminal") ;
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
