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

static void KEYBOARD_log_last_line(WINDOW *win) {

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

void KEYBOARD_INIT(void) {
  const char * name ;
  bool escaped = FALSE;
  int n = 0 ;
  int ch = 0;    
  struct timeval previous;


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
  move(0, 0);

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

  gettimeofday(&previous, 0);
}

void KEYBOARD_READ(void) {
  const char * name ;
  bool escaped = FALSE;
  int n = 0 ;
  int ch = 0;    
  struct timeval previous;


  /*---------------------------------------
    etant donne qu'on est dans un thread le while est inutile
    
  */
  /* while ((ch = getch()) != ERR) { */
  
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

    KEYBOARD_log_last_line(stdscr); 
        
    clrtoeol();
    
  endwin();
  exit(0) ;

}

/* Fin fichier - ----------------------------------------------
----------------------------------------------
----------------------------------------------
---------------------------------------------- */
