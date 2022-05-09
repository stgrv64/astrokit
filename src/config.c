/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | * ajout entete
#  * ajout entree chemins de config.txt
# --------------------------------------------------------------
# 01/05/2021  | 
#       * modification chemin config.txt :
#       * le chemin est directement le repertoire courant
#         lue grace a getcwd, les autres chemins dependent
#         de ce chemin getcwd
#         ajoutee des valeurs lues dans config.txt
#       * mise a jour de la fonction CONFIG_AFFICHER_VARIABLES 
#         avec nouvelles variables
# --------------------------------------------------------------
# 15/11/2021  | * (types.h) modification des types enum et contenu enum
#               * (types.h) modification ordre des menus (MENU_AZIMUTAL=0)
#  => modification ordre MENUS dans switch
# 17/01/2022  | * ajout DEVICE_USE_KEYBOARD pour utilisation du clavier
# 18/01/2022  | * ajout CONFIG_SCR_KERNEL pour execution script avec droits
#                 root via systemd / execve / execl
#               * remplacement fonctions Trace par Trace 
#                 (evite utilisation fichier log alors que celui ci n'est pas encore ouvert)
# 20/01/2022  |  * creation des entetes doxygen des fonctions
#                * ajout mode et type de ASTRE dans CONFIG_INIT_ASTRE
# 23/01/2022  |  * suppression MODE_EQUATORIAL
#                * changement type MENU_PAR_DEFAUT
# 21/03/2022  |  * creation fonction CONFIG_INTI_CODES (deport depuis ir.c/h)
#                * creation fonction CONFIG_INIT_CODE
# avril 2002  |  * debut codage fonctionnalites LCD1602 :
#                * fonctions CONFIG_LCD_xxxx
#                * ajout mutex init sur mutex_datas_infrarouge
#                * => protection zone de code avec datas_infrarouge
#                * ajout fonction CONFIG_LCD_AFFICHER_AGH_DEC / CONFIG_LCD_AFFICHER_EQU_DEC
#                * CONFIG_FORMATE_DONNEES_AFFICHAGE : ajout de 2 resolutions supplementaires
#                * protection zones de code impliquant LCD* (mutex_lcd)
# -------------------------------------------------------------- 
*/

#include <config.h>

// ------------------------------------------------------------------------
// FIN definition des variables dependant du fichier de conf --------------
// ------------------------------------------------------------------------

int NOR_EXCLUSIF(int i,int j) { return !i^j ;};

// R = R1.R2.R3.R4
// F/(2^N.D.R) = (1/2Pi)vitesse
// cas particulier equateur omega = 2Pi / 86164 rad.s-1
// ==> 86164.F = 2^N.D.R


/*****************************************************************************************
* @fn     : CONFIG_MAJ_SUIVI_PAS
* @author : s.gravois
* @brief  : Cette fonction met a jour les valeurs de suivi->pas* en fonction
* @brief  : du contenu de suivi->datas_infrarouge
* @param  : SUIVI *suivi
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
*****************************************************************************************/

void CONFIG_MAJ_SUIVI_PAS( SUIVI *suivi) {

  char c_act = '0' ;

  if ( devices->DEVICE_USE_INFRAROUGE ) {
    
    pthread_mutex_lock(& suivi->mutex_infrarouge );

    if ( ! strcmp( suivi->datas_infrarouge, "plus" ) )         { c_act='1'; suivi->pas_acc_plus  = 1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "moins" ) )        { c_act='1'; suivi->pas_acc_moins = 1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "forward" ) )      { c_act='1'; suivi->pas_forward  = 1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "rewind" ) )       { c_act='1'; suivi->pas_rewind = 1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "forwardfast" ) )  { c_act='1'; suivi->pas_forward_fast  = 1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "rewindfast" ) )   { c_act='1'; suivi->pas_rewind_fast = 1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "ne" ) )           { c_act='1'; suivi->pas_nord=1 ; suivi->pas_est=1   ; }
    if ( ! strcmp( suivi->datas_infrarouge, "no" ) )           { c_act='1'; suivi->pas_nord=1 ; suivi->pas_ouest=1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "se" ) )           { c_act='1'; suivi->pas_sud=1  ; suivi->pas_est=1   ; }
    if ( ! strcmp( suivi->datas_infrarouge, "so" ) )           { c_act='1'; suivi->pas_sud=1  ; suivi->pas_ouest=1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "n" ) )            { c_act='1'; suivi->pas_nord  = 1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "o" ) )            { c_act='1'; suivi->pas_ouest = 1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "e" ) )            { c_act='1'; suivi->pas_est   = 1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "s" ) )            { c_act='1'; suivi->pas_sud   = 1 ; }
    if ( ! strcmp( suivi->datas_infrarouge, "reset" ) )        { c_act='1'; suivi->reset   = 1 ; }

    /* Si suivi->datas_infrarouge a ete utilise, il peut etre remis a zero */

    if ( c_act == '1' ) {
      Trace("raz de suivi->datas_infrarouge") ;
      memset( suivi->datas_infrarouge, 0, strlen( suivi->datas_infrarouge ) ) ;
      strcpy( suivi->datas_infrarouge, "") ;
    }
    pthread_mutex_unlock(& suivi->mutex_infrarouge );

    TRACE2("%ld %ld %ld %ld %d %d\n", \
      suivi->pas_ouest, \
      suivi->pas_est, \
      suivi->pas_nord, \
      suivi->pas_sud, \
      suivi->pas_acc_plus, \
      suivi->pas_acc_moins );
  }

}
/*****************************************************************************************
* @fn     : CONFIG_INPUTS_GESTION_APPUIS
* @author : s.gravois
* @brief  : Cette fonction gere les appuis sur les "touches" (clavier, ir, ..)
* @brief  : et en deduit le contenu de la structure clavier (mot, action, etc..)
* @param  : SUIVI *suivi
* @param  : CLAVIER *clavier
* @date   : 2022-03-22 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
* @date   : 2022-04-12 protection zone de code datas_infrarouge
*****************************************************************************************/

void CONFIG_INPUTS_GESTION_APPUIS(SUIVI *suivi, CLAVIER *clavier) {

  int  i = 0 ;
  char val[255] ;
  char s_buffer[CONFIG_TAILLE_BUFFER_32] ;

  memset(val, 0, strlen(val)) ;     

  pthread_mutex_lock(& suivi->mutex_infrarouge );
  strcpy( val, suivi->datas_infrarouge ) ;
  pthread_mutex_unlock(& suivi->mutex_infrarouge );

  Trace1("val = %s\n", val ) ;
  
  if ( strcmp( val, "") ) {
    
    //printf("val = %s\n", val ) ;
    strcpy( clavier->mot, val ) ; 
    clavier->appui_en_cours = 1 ;
    clavier->mot_en_cours = 1 ;    
  }
  else clavier->appui_en_cours = 0 ; 
  
  // =======================================================================
  // Quand la touche est relacheee, on traite
  // ensuite appui en cours est remis a ZERO pour qu'on ne passe qu'une fois
  // dans cette partie de code
  // =======================================================================
  
  if ( clavier->mot_en_cours && clavier->appui_en_cours == 0 ) {
  
    Trace1("mot trouver = %s", clavier->mot ) ;
		
		GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 5) ;

    //------------------------------------------------------------
    // On incremente la phrase avec le mot et
    // On incremente le nombre avec le mot si premier n'est pas vide
    //------------------------------------------------------------
    
    if ( strcmp( clavier->mot, clavier->valider) != 0 ) { 
      
      Trace1("Si mot != valider : on incremente la phrase avec le mot !!\n" ) ;

      if ( strlen(clavier->phrase) + strlen(clavier->mot) < CONFIG_TAILLE_BUFFER_32) {

        Trace1("on incremente la phrase %s avec le mot %s\n",clavier->phrase,  clavier->mot ) ;
        // correction bug 28/12/2017 : recopie de la chaine dans un buffer avant le sprintf

        memset( s_buffer, IR_ZERO_CHAR, strlen( s_buffer )) ;
        strcpy( s_buffer, clavier->phrase ) ;

        sprintf(clavier->phrase,"%s%s",s_buffer, clavier->mot);
      }
      Trace1("resultat => phrase = %s \n",clavier->phrase ) ;
      
      Trace1("Si mot != valider et premier non vide => on met nombre + mot dans le nombre !!\n" ) ;

      if ( strcmp( clavier->premier, "")) {
        
	      if ( strlen(clavier->nombre) + strlen(clavier->mot) < CONFIG_TAILLE_BUFFER_32) {

          Trace1("on incremente le nombre %s avec le mot %s\n",clavier->nombre,  clavier->mot ) ;
          // correction bug 28/12/2017 : recopie de la chaine dans un buffer avant le sprintf

          memset( s_buffer, IR_ZERO_CHAR, strlen( s_buffer )) ;
          strcpy( s_buffer, clavier->nombre ) ;

          sprintf(clavier->nombre,"%s%s", s_buffer, clavier->mot);
        }
      }
      Trace1("resultat => nombre = %s \n",clavier->nombre ) ;
    }    
    //------------------------------------------------------------
    // On met le mot dans premier si il est vide 
    //------------------------------------------------------------
    
    if ( ! strcmp( clavier->premier, "")){ 
      if ( strlen(clavier->mot) < CONFIG_TAILLE_BUFFER_32)
      strcpy( clavier->premier, clavier->mot);
    }
    //------------------------------------------------------------
    // Si le mot en cours est une VALIDATION 
    // Exemple : valider
    // Alors on VALIDE la phrase en cours
    //  - en mettant clavier->phrase_lu à 1
    // on efface tout sauf SYMBOLE et NOMBRE qui sont determiner plus tot
    //------------------------------------------------------------

    for( i=0 ; i < CONFIG_VALIDATIONS_SIZE ; i++ )
    if ( ! strcmp( clavier->mot,    clavier->valider )  ) {
      Trace1("Appui sur valider => on met premier dans symbole, phrase dans nombre, et NULL dans phrase et mot, phrase_lue a 1" ) ; 

      strcpy(clavier->premier,"") ;
      strcpy(clavier->phrase,"")  ;
      strcpy(clavier->mot,"") ;
      clavier->phrase_lue=1 ;
    }
    //------------------------------------------------------------
    // Si le mot est une ACTION, on efface la phrase en cours    
    // on met le mot dans PREMIER (premier mot de la phrase) 
    // on met le mot dans SYMBOLE (symbole utilise dans prog main) 
    //------------------------------------------------------------
    
    for( i=0 ; i < CONFIG_ACTIONS_SIZE ; i++ )
    if ( ! strcmp( clavier->mot, clavier->actions[i] )) {
        Trace1("Si le mot est une ACTION, alors on efface la phrase en cours et on met mot dans premier et symbole") ;
        strcpy(clavier->premier,clavier->mot) ;
        strcpy(clavier->symbole,clavier->mot)  ;

        strcpy(clavier->nombre,"")  ;
        strcpy(clavier->phrase,"")  ;
        strcpy(clavier->mot,"") ;
        clavier->phrase_lue=0 ;
    }
    
    clavier->mot_en_cours = 0 ;
    clavier->appui_en_cours = 0 ;
  }
}
/*****************************************************************************************
* @fn     : CONFIG_SYSTEM_LOG_0
* @author : s.gravois
* @brief  : Cette fonction ecrit dans la log via un appel systeme avec system
* @param  : int *incrlog
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : (obsolete)
*****************************************************************************************/

void CONFIG_SYSTEM_LOG_0(int *incrlog) {
  int ret ;
  (*incrlog)++ ;
  char cmd[255] ;
  if ( DEBUG_LOG ) {
    // FIXME 01 mai 2021 : modification chemin relatif
    sprintf(cmd,"echo %d >> %s/%s/%s",*incrlog, CONFIG_REP_HOME, CONFIG_REP_LOG, CONFIG_FIC_LOG) ;
    ret = system(cmd) ;
    if ( ret < 0 ) Trace("Probleme avec %s : retourner avec error negative",cmd) ;
    //if ( ret == 0 ) Trace("Probleme avec %s : shell non disponible",cmd) ;
  }
}
/*****************************************************************************************
* @fn     : CONFIG_SYSTEM_LOG_1
* @author : s.gravois
* @brief  : Cette fonction ecrit dans la log via un strtime et system
* @param  : int *incrlog
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : (obsolete)
*****************************************************************************************/
/* obsolete */

void CONFIG_SYSTEM_LOG_1(char *txt) {

  int ret ;
  char cmd[255] ;
  char c_out[255] ;
  time_t now = time (NULL);
  struct tm *tmp;
  tmp = localtime(&now);
  char s_date[16];

  memset( s_date, 0, sizeof( s_date)) ;
  memset( c_out, 0, sizeof( c_out)) ;
  
  if (tmp == NULL) {
    perror("localtime");
    exit(EXIT_FAILURE);
  }

  if ( DEBUG_LOG ) {
    if ( strftime(s_date, sizeof(s_date), "%Y%m%d.%H%M", tmp) == 0 ) {
        fprintf(stderr, "strftime a renvoyé 0");
        exit(EXIT_FAILURE);
    }
    sprintf( c_out, "%s : %s", s_date, txt ) ;
    sprintf( cmd,"echo %s >> %s/%s/%s",c_out, CONFIG_REP_HOME, CONFIG_REP_LOG, CONFIG_FIC_LOG) ;
    ret =  system(cmd) ;
    if ( ret < 0 ) Trace("Probleme avec %s : retourner avec error negative",cmd) ;
    //if ( ret == 0 ) Trace("Probleme avec %s : shell non disponible",cmd) ;
  }
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_LOG
* @author : s.gravois
* @brief  : Cette fonction ouvre le fichier de log defini par 
*           CONFIG_REP_HOME / CONFIG_REP_LOG / CONFIG_FIC_LOG
*           definis dans le fichier de configuration
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : rappatrier syslog init ICI ?
*****************************************************************************************/

void CONFIG_INIT_LOG(void) {

  char buf[255] ;
  
  Trace("start") ;
  
  if ( DEBUG_LOG ) {
    
    memset(buf, ZERO_CHAR, sizeof(buf));
    sprintf(buf,"%s/%s/%s", CONFIG_REP_HOME, CONFIG_REP_LOG, CONFIG_FIC_LOG) ;
    
    if ( (flog=fopen(buf,"a")) == NULL) {
      // completer et modifier
      Trace("probleme ouverture 3 %s",buf) ;
      SyslogErrFmt("probleme ouverture %s", buf) ;
      exit(2) ;
    }
    else Trace("open %s ok", buf) ;
  }
  return ;
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_LCD
* @author : s.gravois
* @brief  : Cette fonction initialise la structure lcd *l
* @param  : LIEU *lieu
* @date   : 2022-04-08 creation
* @date   : 2022-04-27 mise en commentaire de CALCUL_TEMPS_SIDERAL
*****************************************************************************************/

void CONFIG_INIT_LCD(LCD *lcd) {

  Trace("") ;

  /* Initialisation mutex */

  pthread_mutex_init( & lcd->mutex_lcd, NULL ) ;
  
  /* Initialisation pointeurs de fonctions */

  lcd->display = CONFIG_LCD_PT_DISPLAY ;
  lcd->depiler = CONFIG_LCD_DEPILER ;
  lcd->empiler = CONFIG_LCD_EMPILER ;
  lcd->continu = CONFIG_LCD_CONTINU ;

  /* Initialisation autres champs */

  memset( lcd->c_line_0, 0 , sizeof( lcd->c_line_0 )) ;
  memset( lcd->c_line_1, 0 , sizeof( lcd->c_line_1 )) ;
  memset( lcd->c_line_0_old, 0 , sizeof( lcd->c_line_0_old )) ;
  memset( lcd->c_line_1_old, 0 , sizeof( lcd->c_line_1_old )) ;

  strcpy( lcd->c_line_0, "") ; 
  strcpy( lcd->c_line_1, "") ; 
  strcpy( lcd->c_line_0_old, "") ; 
  strcpy( lcd->c_line_1_old, "") ; 

  lcd->i_board = 0 ;
  lcd->i_i2c_num = CONFIG_LCD_I2C_DEFAULT_DEV_PORT ; 
  
  /*--------------------------*/
  /* Initialisation ecran LCD */ 
  /*--------------------------*/

  if ((lcd->i_fd = LCD1602Init(lcd->i_i2c_num)) == -1) {
     SyslogErr("Fail to init LCD1602Init\n");
     Trace("Fail to init LCD1602Init") ;
     return ;
  }
  else {
    Trace("LCD1602Init(ok)") ;
  }

  /*--------------------------*/
  /* Affichage phrases init   */ 
  /*--------------------------*/

  sprintf(lcd->c_line_0, "%d %s %d %d:%d", \
    temps->yy , \
    c_Lcd_Display_Months[ temps->mm -1  ] , \
    temps->dd, \
    temps->HH, \
    temps->MM ) ;
  
  sprintf(lcd->c_line_1, "%.2f %.2f", \
    lieu->lat * DEGRES, \
    lieu->lon * DEGRES ) ;

  if ( LCD1602Clear(lcd->i_fd) == -1) {
    SyslogErr("Fail to LCD1602Clear\n");
    Trace("Fail to LCD1602Clear") ;
    return ;
  }
  else {
    Trace("LCD1602Clear(ok)") ;
  }

  usleep(CONFIG_LCD_USLEEP_AFTER_CLEARING) ;

  if (LCD1602DispLines(lcd->i_fd, lcd->c_line_0, lcd->c_line_1 ) == -1) {
    SyslogErr("Fail to LCD1602DispLines\n");
    Trace("Fail to LCD1602DispLines");
  }
  else {
    Trace("LCD1602DispLines(ok)") ;
  }

  return ;
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_ASTRE
* @author : s.gravois
* @brief  : Cette fonction initialise la structure ASTRE * as
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-20 ajout membres de la structure dans la fonction
* @todo   : supprimer ASTRE *as (var blog) et remplacer par void
*           passer par des pointeurs de fonctions dans un structure
*****************************************************************************************/

void CONFIG_INIT_ASTRE(ASTRE *as) {

  int C ;
  
  for(C=0; C< ASTRE_NB_COLONNES;C++) {
    memset( as->plus_proche[C], ZERO_CHAR, ASTRE_TAILLE_BUFFER);
    strcpy( as->plus_proche[C], "") ;
  }
  memset( as->nom,   ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  memset( as->infos, ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  memset( as->plus_proche, ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  
  as->a   = 0  ;
  as->h   = 0  ;
  as->a0  = 0 ;
  as->h0  = 0 ;
  as->AGH   = 0  ; 
  as->ASC   = 0  ;
  as->DEC   = 0   ;
  as->A0  = 0 ;
  as->H0  = 0 ;
  as->da  = 0 ;
  as->dh  = 0 ;
  as->dA  = 0 ;
  as->dH  = 0 ;
  as->Va  = 0 ;
  as->Vh  = 0 ;
  as->dVa = 0 ;
  as->dVh = 0 ;
  as->dVam= 0 ;
  as->dVhm= 0 ;

  as->x = 0 ;
  as->xx = 0 ;
  as->y  = 0 ;
  as->yy =0;
  as->z =0;
  as->zz =0;
  
  as->type = ASTRE_INDETERMINE ;
  as->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  as->numero = 0 ;
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_CLAVIER
* @author : s.gravois
* @brief  : Cette fonction initialise la structure CLAVIER * as
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-20 ajout membres de la structure dans la fonction
* @todo   : supprimer ASTRE *as (var blog) et remplacer par void
*           passer par des pointeurs de fonctions dans un structure
*****************************************************************************************/

void CONFIG_INIT_CLAVIER(CLAVIER * clavier) {

 int i ;
  
  Trace("") ;

  memset( clavier->phrase,  ZERO_CHAR, strlen( clavier->phrase ) );
  memset( clavier->valider, ZERO_CHAR, strlen( clavier->valider ) );
  memset( clavier->symbole, ZERO_CHAR, strlen( clavier->symbole ) );
  memset( clavier->menu,    ZERO_CHAR, strlen( clavier->menu ) );
  memset( clavier->premier, ZERO_CHAR, strlen( clavier->premier ) );
  memset( clavier->nombre,  ZERO_CHAR, strlen( clavier->nombre ) );
  memset( clavier->mot,     ZERO_CHAR, strlen( clavier->mot ) );
  
  strcpy( clavier->valider, "valider" ) ;
  strcpy( clavier->menu,    "MENU" ) ;
  
  strcpy( clavier->symbole,  "" ) ;
  strcpy( clavier->phrase,   "" ) ;
  strcpy( clavier->premier,  "" ) ;
  strcpy( clavier->nombre,   "" ) ;
  strcpy( clavier->mot,      "" ) ;

  clavier->mot_en_cours   = 0 ;
  clavier->phrase_lue     = 0 ;
  clavier->appui_en_cours = 0 ;
  clavier->temporisation_clavier = TEMPO_CLAVIER ;
 
  // FIXME : definitions des actions : 
  // Les actions servent a 
  
  for( i=0 ; i < CONFIG_ACTIONS_SIZE ; i++ ) {
    memset( clavier->actions[i], ZERO_CHAR, CONFIG_TAILLE_BUFFER_32);
  }
  // for( i=0 ; i < CONFIG_VALIDATIONS_SIZE ; i++ )  memset( clavier->validations[i], ZERO_CHAR, CONFIG_TAILLE_BUFFER_32);

  strcpy( clavier->actions[0], "MENU" ) ;
  strcpy( clavier->actions[1], "SETUP" ) ;
  strcpy( clavier->actions[2], "MES" ) ;
  strcpy( clavier->actions[3], "NGC" ) ;
  strcpy( clavier->actions[4], "ETO" ) ;
  strcpy( clavier->actions[5], "PLA" ) ;
  strcpy( clavier->actions[6], "TIME" ) ;
   
} 
/*****************************************************************************************
* @fn     : CONFIG_INIT_LIEU
* @author : s.gravois
* @brief  : Cette fonction initialise la structure LIEU *lieu
* @param  : LIEU *lieu
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : cf si JJ etc.. sont utiles dans la structure (parametres de temps)
*****************************************************************************************/

void CONFIG_INIT_LIEU(LIEU *lieu) {
 
  lieu->JJ  = 0 ; // jour julien
  lieu->TS  = 0 ;  // temps sideral
  lieu->TSR = 0 ;  // temps sideral en radians
  lieu->JD  = 0 ;  // jour decimal
  lieu->lat = LATITUDE / DEGRES ;
  lieu->lon = LONGITUDE / DEGRES ;
  lieu->alt = ALTITUDE ;
}
//---------------------------------------------------------------------------------------
// man date sur noyau compile 
// Display time (using +FMT), or set time
//
//	[-s,--set] TIME	Set time to TIME
//	-u,--utc	Work in UTC (don't convert to local time)
//	-R,--rfc-2822	Output RFC-2822 compliant date string
//	-I[SPEC]	Output ISO-8601 compliant date string
//			SPEC='date' (default) for date only,
//			'hours', 'minutes', or 'seconds' for date and
//			time to the indicated precision
//	-r,--reference FILE	Display last modification time of FILE
//	-d,--date TIME	Display TIME, not 'now'
//	-D FMT		Use FMT for -d TIME conversion
//
//Recognized TIME formats:
//	hh:mm[:ss]
//	[YYYY.]MM.DD-hh:mm[:ss]
//	YYYY-MM-DD hh:mm[:ss]
//	[[[[[YY]YY]MM]DD]hh]mm[.ss]
//	'date TIME' form accepts MMDDhhmm[[YY]YY][.ss] instead
//---------------------------------------------------------------------------------------

/*****************************************************************************************
* @fn     : CONFIG_SET_YEAR_MONTH_AND_DAY
* @author : s.gravois
* @brief  : Cette fonction configure la date (annee mois jour)
* @param  : char * s_data
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : remplacer fonction system par une fonction C (eviter passage par /bin/bash date)
*****************************************************************************************/

void CONFIG_SET_YEAR_MONTH_AND_DAY(char * s_data) { // taille des g_Datas = 5 (unite de l'annee en premier)
 
  char buf [   CONFIG_TAILLE_BUFFER_64 ] ;
  char year [  CONFIG_TAILLE_BUFFER_8] ;
  char month [ CONFIG_TAILLE_BUFFER_4] ;
  char day [   CONFIG_TAILLE_BUFFER_4] ;
  int i = 0 ;

  if ( strlen(s_data) != 8 ){

    Trace("mauvais format : %s", s_data) ;
    Trace("la chaine de caractere doit etre composee de <annee><mois><jour>") ;
    Trace("Exemple : 20190822 = 22 aout 2019") ;
    return ;
  }
  memset( buf,   ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  memset( year,  ZERO_CHAR, CONFIG_TAILLE_BUFFER_8 ) ;
  memset( month, ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  memset( day,   ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  
  for(i=0;i<4;i++) year[i]  = s_data[i] ;
  for(i=0;i<2;i++) month[i] = s_data[i+4] ;
  for(i=0;i<2;i++) day[i]   = s_data[i+6] ;

  sprintf(buf, "/bin/date -s %s-%s-%s", year, month, day ) ;
  if ( system( buf ) < 0 ) perror( buf) ;
	Trace("buf = %s", buf) ;

  memset( buf, ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  sprintf(buf, "/bin/echo %s-%s-%s > %s/%s/%s ", year, month, day, CONFIG_REP_HOME, CONFIG_REP_CFG, CONFIG_FIC_DATE ) ;
  Trace("buf = %s", buf) ;

  if ( system( buf ) < 0 ) perror( buf) ;

  Trace("Nouvelle date : %s", buf) ;

}
/*****************************************************************************************
* @fn     : CONFIG_SET_HOUR_AND_MINUTES
* @author : s.gravois
* @brief  : Cette fonction configure l heure (heure et minutes)
* @param  : char * s_data
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : remplacer fonction system par une fonction C (eviter passage par /bin/bash date)
*****************************************************************************************/

void CONFIG_SET_HOUR_AND_MINUTES(char * s_data) {

  char buf [ CONFIG_TAILLE_BUFFER_64 ]  ;
  char hou [ CONFIG_TAILLE_BUFFER_4] ;
  char min [ CONFIG_TAILLE_BUFFER_4] ;
  int i = 0 ;
  
  // FIXME : 20190822 : ajout Traces

  if ( strlen(s_data) != 4 ){

    Trace("mauvais format : %s", s_data) ;
    Trace("la chaine de caractere doit etre composee de <heure><minutes>") ;
    Trace("Exemple : 0804 = 8 heures et 4 minutes") ;
    return ;
  }

  memset( buf, ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  memset( hou, ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  memset( min, ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  
  for(i=0;i<2;i++) hou[i]=s_data[i] ;
  for(i=0;i<2;i++) min[i]=s_data[i+2] ;
  
  sprintf(buf, "/bin/date -s %s:%s",hou, min ) ;
  Trace("buf = %s", buf) ;
  if ( system( buf ) < 0 ) perror( buf) ;

  memset( buf, ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  sprintf(buf, "/bin/echo %s:%s > %s/%s/%s ", hou, min, CONFIG_REP_HOME, CONFIG_REP_CFG, CONFIG_FIC_HHMM ) ;
  Trace("buf = %s", buf) ;
  if ( system( buf ) < 0 ) perror( buf) ;
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_TEMPS
* @author : s.gravois
* @brief  : Cette fonction initialise la structure TEMPS * temps
* @param  : TEMPS *temps
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : configurer directement heure locale (?)
*****************************************************************************************/

void CONFIG_INIT_TEMPS( TEMPS *temps) {
  
  temps->mm = 0 ;  // month
  temps->yy = 0 ;  // year
  temps->dd = 0 ;  // day
  temps->HH = 0 ;  // hour
  temps->MM = 0 ;  // minutes
  temps->SS = 0 ;  // secondes
  temps->hd = 0.0 ;  // heure decimale (double)
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_VOUTE
* @author : s.gravois
* @brief  : Cette fonction initialise la structure VOUTE *voute
* @param  : VOUTE *voute
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           pourcentage_tempo et calibration_voute
*****************************************************************************************/

void CONFIG_INIT_VOUTE(VOUTE *voute) {
  
  voute->dt                = 0  ;
  voute->pourcentage_tempo = 0.96 ;   
  voute->calibration_voute = 0.97 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
	voute->num               = 0 ;
  voute->deb               = 0 ;
  voute->fin               = 0 ;
  voute->acc_old           = 1 ;
  voute->acc               = 1 ;
  voute->pas               = voute->dt * ROT_RAD_SEC ;
  /* dt en micro-sec */

  voute->DT = (unsigned long)( voute->dt * CONFIG_MICRO_SEC / voute->acc ) ;   
}

/*****************************************************************************************
* @fn     : CONFIG_INIT_CODE
* @author : s.gravois
* @brief  : Cette fonction initialise un code (appelle par CONFIG_INIT_CODES)
* @param  : VOUTE *voute
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           pourcentage_tempo et calibration_voute
*****************************************************************************************/

void CONFIG_INIT_CODE( \
 CODES * gp_Codes, \
 int          li_pos, \
 const char * gl_char_Codes[][CONFIG_CODES_NB_IN_OUT] ) {

  Trace1("li_pos %d %-16s %-16s %-16s", \
    li_pos, \
    gl_char_Codes[li_pos][ CODES_POS_IN_TERM ], \
    gl_char_Codes[li_pos][ CODES_POS_IN_LIRC], \
    gl_char_Codes[li_pos][ CODES_POS_OUT_ACT] \
  );

  strcpy( gp_Codes->in_term[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_IN_TERM ] ) ;
  strcpy( gp_Codes->in_lirc[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_IN_LIRC] ) ;
  strcpy( gp_Codes->out_act[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_OUT_ACT] ) ;
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_CODES
* @author : s.gravois
* @brief  : Cette fonction initialise la structure des codes *CODES
* @brief  : etablie la correspondance entre les KEY du fichier lircd.conf (partie LIRC)
* @brief  : et les codes utilises dans le programme principal (car pas de hachage en c)
* @brief  : etablie aussi la correspondance entre les KEY du clavier termios (partie TERMIOS)
* @brief  : et les codes utilises dans le programme principal (code_action)
* @param  : CODES *gp_Codes
* @date   : 2022-03-21 creation entete
* @todo   : modifier : completer avec une fonction de hachage (regarder si API sur net)
*****************************************************************************************/

void CONFIG_INIT_CODES(CODES *gp_Codes) {

  int i_pos ;
  
  for( i_pos=0 ; i_pos<CONFIG_CODE_NB_CODES ; i_pos++ ) memset( gp_Codes->out_act[i_pos], IR_ZERO_CHAR, strlen(gp_Codes->out_act[i_pos]) ) ;
  for( i_pos=0 ; i_pos<CONFIG_CODE_NB_CODES ; i_pos++ ) memset( gp_Codes->in_lirc[i_pos],  IR_ZERO_CHAR, strlen(gp_Codes->in_lirc[i_pos]) ) ;

  for( i_pos=0 ; i_pos<CONFIG_CODE_NB_CODES ; i_pos++ ) {
    memset( gp_Codes->in_term[i_pos],  IR_ZERO_CHAR, strlen(gp_Codes->in_term[i_pos]) ) ;
    memset( gp_Codes->in_lirc[i_pos],  IR_ZERO_CHAR, strlen(gp_Codes->in_lirc[i_pos]) ) ;
    memset( gp_Codes->out_act[i_pos],  IR_ZERO_CHAR, strlen(gp_Codes->out_act[i_pos]) ) ;
  }
  for( i_pos=0 ; i_pos<CONFIG_CODE_NB_CODES ; i_pos++ ) {

    CONFIG_INIT_CODE( gp_Codes, i_pos, g_char_Codes  ) ; 
  }
  // FIXME :  ATTENTION !!! 
  // le nom des gp_Codes codes en MAJUSCULE servent a definir des actions dans config.c et le reste du programme

  /* ancienne partie :  
  
  strcpy( gp_Codes->in_lirc[0], "KEY_0") ;           strcpy( gp_Codes->out_act[0], "0") ;
  strcpy( gp_Codes->in_lirc[1], "KEY_1") ;           strcpy( gp_Codes->out_act[1], "1") ;
  strcpy( gp_Codes->in_lirc[2], "KEY_2") ;           strcpy( gp_Codes->out_act[2], "2") ;
  strcpy( gp_Codes->in_lirc[3], "KEY_3") ;           strcpy( gp_Codes->out_act[3], "3") ;
  strcpy( gp_Codes->in_lirc[4], "KEY_4") ;           strcpy( gp_Codes->out_act[4], "4") ;
  strcpy( gp_Codes->in_lirc[5], "KEY_5") ;           strcpy( gp_Codes->out_act[5], "5") ;
  strcpy( gp_Codes->in_lirc[6], "KEY_6") ;           strcpy( gp_Codes->out_act[6], "6") ;
  strcpy( gp_Codes->in_lirc[7], "KEY_7") ;           strcpy( gp_Codes->out_act[7], "7") ;
  strcpy( gp_Codes->in_lirc[8], "KEY_8") ;           strcpy( gp_Codes->out_act[8], "8") ;
  strcpy( gp_Codes->in_lirc[9], "KEY_9") ;           strcpy( gp_Codes->out_act[9], "9") ;

  strcpy( gp_Codes->in_lirc[10],"KEY_M") ;           strcpy( gp_Codes->out_act[10],"MES") ;
  strcpy( gp_Codes->in_lirc[11],"KEY_N") ;           strcpy( gp_Codes->out_act[11],"NGC") ;
  strcpy( gp_Codes->in_lirc[12],"KEY_E") ;           strcpy( gp_Codes->out_act[12],"ETO") ;
  strcpy( gp_Codes->in_lirc[13],"KEY_P") ;           strcpy( gp_Codes->out_act[13],"PLA") ;

  strcpy( gp_Codes->in_lirc[14],"KEY_PREVIOUS");     strcpy( gp_Codes->out_act[14],"previous") ;
  strcpy( gp_Codes->in_lirc[15],"KEY_PLAY") ;        strcpy( gp_Codes->out_act[15],"valider") ;
  strcpy( gp_Codes->in_lirc[16],"KEY_NEXT") ;        strcpy( gp_Codes->out_act[16],"plus") ;
  strcpy( gp_Codes->in_lirc[17],"KEY_STOP") ;        strcpy( gp_Codes->out_act[17],"stop") ;
  strcpy( gp_Codes->in_lirc[18],"KEY_MENU") ;        strcpy( gp_Codes->out_act[18],"MENU") ;
  strcpy( gp_Codes->in_lirc[19],"KEY_PAUSE") ;       strcpy( gp_Codes->out_act[19],"pause") ;
  strcpy( gp_Codes->in_lirc[20],"KEY_OK") ;          strcpy( gp_Codes->out_act[20],"reset") ;
  strcpy( gp_Codes->in_lirc[21],"KEY_UP") ;          strcpy( gp_Codes->out_act[21],"n") ;
  strcpy( gp_Codes->in_lirc[22],"KEY_RIGHT") ;       strcpy( gp_Codes->out_act[22],"e") ;
  strcpy( gp_Codes->in_lirc[23],"KEY_DOWN") ;        strcpy( gp_Codes->out_act[23],"s") ;
  strcpy( gp_Codes->in_lirc[24],"KEY_LEFT") ;        strcpy( gp_Codes->out_act[24],"o") ;
  strcpy( gp_Codes->in_lirc[25],"KEY_SETUP") ;       strcpy( gp_Codes->out_act[25],"SETUP") ;
  // strcpy( gp_Codes->in_lirc[26],"KEY_TIME") ;        strcpy( gp_Codes->out_act[26],"TIME") ;    // ajout du 10/10/2016
  strcpy( gp_Codes->in_lirc[26],"KEY_SOUND") ;        strcpy( gp_Codes->out_act[26],"TIME") ;    // ajout du 10/10/2016

  strcpy( gp_Codes->in_lirc[27],"KEY_FORWARD") ;     strcpy( gp_Codes->out_act[27],"forward") ;
  strcpy( gp_Codes->in_lirc[28],"KEY_REWIND") ;      strcpy( gp_Codes->out_act[28],"rewind") ;    // ajout du 26/11/2017

  strcpy( gp_Codes->in_lirc[29],"KEY_RED") ;         strcpy( gp_Codes->out_act[29],"red") ;
  strcpy( gp_Codes->in_lirc[30],"KEY_BLUE") ;        strcpy( gp_Codes->out_act[30],"blue") ;
  strcpy( gp_Codes->in_lirc[31],"KEY_YELLOW") ;      strcpy( gp_Codes->out_act[31],"yellow") ;
  strcpy( gp_Codes->in_lirc[32],"KEY_GREEN") ;       strcpy( gp_Codes->out_act[32],"green") ;   // ajout ulterieurs.. pour etre idem que reel 25 et superieur ...

  strcpy( gp_Codes->in_lirc[33],"KEY_POWER") ;       strcpy( gp_Codes->out_act[33],"key_power") ;

  strcpy( gp_Codes->in_lirc[34],"KEY_CHANNELUP") ;   strcpy( gp_Codes->out_act[34],"forwardfast") ;
  strcpy( gp_Codes->in_lirc[35],"KEY_CHANNELDOWN") ; strcpy( gp_Codes->out_act[35],"rewindfast") ;
  strcpy( gp_Codes->in_lirc[36],"KEY_VOLUMEUP") ;    strcpy( gp_Codes->out_act[36],"forward") ;
  strcpy( gp_Codes->in_lirc[37],"KEY_VOLUMEDOWN") ;  strcpy( gp_Codes->out_act[37],"rewind") ;

  strcpy( gp_Codes->in_lirc[38],"KEY_MUTE") ;        strcpy( gp_Codes->out_act[38],"TIME") ;

  strcpy( gp_Codes->in_lirc[39],"KEY_SCREEN") ;      strcpy( gp_Codes->out_act[39],"aff_azi_alt") ; 
  strcpy( gp_Codes->in_lirc[40],"KEY_TV") ;          strcpy( gp_Codes->out_act[40],"aff_tps_lie") ;
  strcpy( gp_Codes->in_lirc[41],"KEY_INFO") ;        strcpy( gp_Codes->out_act[41],"aff_info") ;
  strcpy( gp_Codes->in_lirc[42],"KEY_ZOOM") ;        strcpy( gp_Codes->out_act[42],"key_zoom") ;
  strcpy( gp_Codes->in_lirc[43],"KEY_LIST") ;        strcpy( gp_Codes->out_act[43],"key_azi") ;
  strcpy( gp_Codes->in_lirc[44],"KEY_MODE") ;        strcpy( gp_Codes->out_act[44],"key_equ") ; 
  strcpy( gp_Codes->in_lirc[45],"KEY_EXIT") ;        strcpy( gp_Codes->out_act[45],"key_exit") ;
  
  // codes VOUTE de la telecommande - joue sur la vitesse globale

  */
}
/*****************************************************************************************
* @fn     : CONFIG_VOUTE
* @author : s.gravois
* @brief  : Cette fonction configure la structure VOUTE *voute
* @param  : VOUTE *voute
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           pourcentage_tempo et calibration_voute
*****************************************************************************************/

void CONFIG_VOUTE( VOUTE *voute, double dt, double acc, double percent ) {
  
  voute->dt                 = dt  ;
  voute->pourcentage_tempo  = percent ; 
  voute->calibration_voute  = 0.99 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
  voute->deb                = 0 ;
  voute->num                = 0 ;
  voute->fin                = PIPI ;
  voute->acc_old            = voute->acc ;
  voute->acc                = acc ;
  voute->pas                = voute->dt * ROT_RAD_SEC ;
  /* dt en micro-sec */
  voute->DT = (unsigned long)( voute->dt * CONFIG_MICRO_SEC / voute->acc ) ;
}

/*****************************************************************************************
* @fn     : CONFIG_INIT_DEVICES
* @author : s.gravois
* @brief  : Cette fonction configure la structure DEVICES *devices
* @param  : DEVICES *devices
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : (obsolete) les devices sont lues depuis le fichier de configuration
*****************************************************************************************/

void CONFIG_INIT_DEVICES(DEVICES *devices) {

  devices->DEVICE_USE_CAPTEURS    = DEVICE_USE_CAPTEURS ;
  devices->DEVICE_USE_RAQUETTE    = DEVICE_USE_RAQUETTE ;
  devices->DEVICE_USE_BLUETOOTH   = DEVICE_USE_BLUETOOTH ;
  devices->DEVICE_USE_INFRAROUGE  = DEVICE_USE_INFRAROUGE ;
  devices->DEVICE_USE_CONTROLER   = DEVICE_USE_CONTROLER ;
  devices->DEVICE_USE_KEYBOARD    = DEVICE_USE_KEYBOARD ;
  devices->DEVICE_USE_LCD         = DEVICE_USE_LCD ; 
  devices->init_capteurs = 0 ; 
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_SUIVI
* @author : s.gravois
* @brief  : Cette fonction configure la structure SUIVI *suivi
* @param  : SUIVI *suivi
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-20 passage d une partie de code dans CONFIG_INIT_DEVICES
* @todo   : 
*****************************************************************************************/

void CONFIG_INIT_SUIVI(SUIVI *suivi) {
  
  int i ;
  
  suivi->SUIVI_MANUEL     = 0 ;
  suivi->SUIVI_ALIGNEMENT = 0 ;
  suivi->SUIVI_GOTO       = 0 ;
  suivi->SUIVI_VOUTE      = 1 ;
  suivi->SUIVI_EQUATORIAL = 0 ;

  // a modifier  : instancier ces variables a l aide du fichier de config

  suivi->reset = 0 ;
  
  suivi->pas_azi        = 1 ;
  suivi->pas_alt        = 1 ;

  suivi->pas_acc_plus   = 0 ;
  suivi->pas_acc_moins  = 0 ;
  suivi->pas_azi_old    = 0 ;
  suivi->pas_alt_old    = 0 ;

  suivi->pas_est        = 0 ;
  suivi->pas_ouest      = 0 ; 
  suivi->pas_nord       = 0 ;
  suivi->pas_sud        = 0 ;

  suivi->pas_forward       = 0 ;
  suivi->pas_rewind        = 0 ;
  suivi->pas_forward_fast  = 0 ;
  suivi->pas_rewind_fast   = 0 ;

  suivi->acc_azi   = 1.0 ;     // cette variable est utilisee dans le calcul des periodes
  suivi->acc_alt   = 1.0 ;     // cette variable est utilisee dans le calcul des periodes
  suivi->sgn_azi   = 1 ;
  suivi->sgn_alt   = 1 ;

  suivi->t_diff = 0 ;
  suivi->t_diff_sec = 0 ;
  suivi->d_temps          = 0 ;   
  suivi->d_appui_raq_azi  = 0 ;
  suivi->d_appui_raq_alt  = 0 ;
  
  suivi->Fa        = 30 ;
  suivi->Fh        = 30 ;

  suivi->Ta        = 1 / suivi->Fa ;
  suivi->Th        = 1 / suivi->Fh ;
  
  suivi->Tac        = 1.0 ;
  suivi->Thc        = 1.0 ;
  
  suivi->Tacc = 0.97 ;
  suivi->Thcc = 0.97 ;
  
  suivi->Ia = 0 ;
  suivi->Ih = 0 ;
  
  suivi->Ias = 0 ;
  suivi->Ihs = 0 ;
  
  suivi->Ia_prec = 0 ;
  suivi->Ih_prec = 0 ;
  
  for(i=0;i<CONFIG_ASS;i++) suivi->Iat[i] = 0 ;
  for(i=0;i<CONFIG_ASS;i++) suivi->Iht[i] = 0 ;
  
  //suivi->plus      =  1.02 ;
  //suivi->moins     =  1.0 / suivi->plus ;   
  
  suivi->l_NANO_MOINS     = 0  ;     // a retirer sur les temporisations pour les tests
   
  suivi->pas_asc = 0 ;
  suivi->pas_dec = 0  ;
    
  suivi->Sa = 0 ;       // signe de la vitesse (direction), tenir compte du flag FLAG_SENS_ALT
  suivi->Sh = 0 ;       // signe de la vitesse (direction), tenir compte du flag FLAG_SENS_AZI
  suivi->Sa_old = 0 ;   // flag de comparaison pour raffraichir ou non les ecritures
  suivi->Sh_old = 0 ;   // flag de comparaison pour raffraichir ou non les ecritures
  
  suivi->Da = 0  ;      // nombre a injecter dans les diviseurs de frequence
  suivi->Dh = 0 ;       // nombre a injecter dans les diviseurs de frequence
  
  suivi->menu = MENU_PAR_DEFAUT  ;    // menu par defaut
  suivi->menu_old = MENU_PAR_DEFAUT  ;

  suivi->alarme = 0 ;
  
  suivi->pourcentage_tempo = 0.99 ; 
  
  suivi->temporisation_menu     = TEMPO_MENU ;
  suivi->temporisation_raq      = TEMPO_RAQ ;
  suivi->temporisation_ir       = TEMPO_IR ;  
  suivi->temporisation_termios  = TEMPO_TERMIOS ;
  suivi->temporisation_clavier  = TEMPO_CLAVIER ; 
  suivi->temporisation_capteurs = TEMPO_CAPTEURS ;
  suivi->temporisation_lcd      = TEMPO_LCD ;

  suivi->temporisation_voute    = voute->DT ;

  suivi->DTh = suivi->Th * CONFIG_MICRO_SEC ;
  suivi->DTa = suivi->Ta * CONFIG_MICRO_SEC ;

  pthread_mutex_init( & suivi->mutex_alt, NULL ) ;
  pthread_mutex_init( & suivi->mutex_azi, NULL ) ;
  pthread_mutex_init( & suivi->mutex_cal, NULL ) ;
  pthread_mutex_init( & suivi->mutex_infrarouge , NULL ) ;

  suivi->temps_a = 0 ; 
  suivi->temps_h = 0 ; 
  
  gettimeofday(&suivi->tval,NULL) ;

  suivi->lcd = gp_Lcd ;
}
/*****************************************************************************************
* @fn     : CONFIG_FORMAT_ADMIS
* @author : s.gravois
* @brief  : Cette fonction verifie si caractere lu est correct (CONFIG_READ)
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_FORMAT_ADMIS(char c) {
  
  int i=(int)c ;
  int j ;
  
  j=0;
  
  if ( i>45 && i<58 )j=1;  // chiffres et le point
  if ( i>64 && i<91 )j=1;  // MAJUSCULES  
  if ( i>96 && i<123)j=1;  // minuscules
  if ( i==95)j=1;          // underscore
  if ( i==44)j=1;          // virgule
  if ( i==45)j=1;          // signe -

  return j ; 
}
/*****************************************************************************************
* @fn     : CONFIG_FIN_LIGNE
* @author : s.gravois
* @brief  : Cette fonction verifie si caractere est un caracter fin de ligne (\n=10)
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_FIN_LIGNE(char c) {
  int i=(int)c ;
  int j ;
  j=0;
  if ( i==10)j=1;  // \n
  return j ; 
}
/*****************************************************************************************
* @fn     : CONFIG_DEB_COM
* @author : s.gravois
* @brief  : Cette fonction verifie si caractere un debut de commentaire '#'
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_DEB_COM(char c) {
  int i=(int)c ;
  int j ;
  j=0;
  if ( i==35)j=1 ; 
  return j ; 
}
/*****************************************************************************************
* @fn     : CONFIG_FIN_MOT
* @author : s.gravois
* @brief  : Cette fonction verifie si caractere est apres un mot (espace ou \t)
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_FIN_MOT(char c) {
  int i=(int)c ;
  int j ;
  j=0;
  if ( i==9 )j=1;  // \t
  if ( i==32)j=1;  // space
  return j ; 
}
/*****************************************************************************************
* @fn     : CONFIG_FIN_MOT
* @author : s.gravois
* @brief  : Cette fonction verifie si caractere est une fin de fichier (EOF) ou non admis
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_FIN_FICHIER(char c) {
  int i=(int)c ;
  int j ;
  
  j=0;
  if ( i>122 )j=1;      // caracteres non admis ascii > 122
  if ( i==-1) j=1;      // EOF
  if ( i<32 && i!=9 && i!=10 ) j=1; // caracteres non admis ascii < 32
  return j ; 
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_VAR
* @author : s.gravois
* @brief  : Cette fonction initialise les parametres a l'aide d'un tableau precedemment lu
            par CONFIG_READ / GPIO_READ
* @param  : g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_INIT_VAR(char g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) {

  int l ;

   //----------------------------------------------------------------------
   // Variables susceptibles de ne pas etre lues (doivent etre mise a zero)
   //----------------------------------------------------------------------
   
   ALT_R = 0 ;
   AZI_R = 0 ;
   
   /* Les coefficients de reduction sont egaux a 1 par defaut */ 
   /* car leur presence n est pas obligatoires dans le fichier config.txt */

   ALT_R1 = 1.0 ;  // reduction liee a la monture ()
   ALT_R2 = 1.0;   // reducteur du moteur (nombre de pas)
   ALT_R3 = 1.0;   // reducteur du moteur (gearbox)
   ALT_R4 = 1.0;   // mode micro pas utilisee (1/R4)
   ALT_R5 = 1.0;   // reduction liee a la poulie
   ALT_R6 = 1.0;   // reduction liee au cpu
   ALT_R7 = 1.0;   // reduction non decrite plus haut

   AZI_R1 = 1.0 ;  // reduction liee a la monture
   AZI_R2 = 1.0;   // reducteur du moteur
   AZI_R3 = 1.0;   // nombre de pas du moteur en azimut
   AZI_R4 = 1.0;   // mode micro pas utilisee (1/R4)
   AZI_R5 = 1.0;   // reduction liee a la poulie
   AZI_R6 = 1.0;   // reduction liee au cpu
   AZI_R7 = 1.0;   // reduction non decrite plus haut

   DEVICE_USE_CAPTEURS = 0    ;
   DEVICE_USE_RAQUETTE = 0    ;
   DEVICE_USE_BLUETOOTH = 0   ;
   DEVICE_USE_INFRAROUGE = 0  ;
   DEVICE_USE_CONTROLER = 0 ;
   DEVICE_USE_KEYBOARD = 0 ;
   DEVICE_USE_LCD = 0 ;

   GPIO_LED_ETAT=0;

   /* Definition de valeurs par defauts pour les TEMPO */ 

  TEMPO_MENU     =  50000;
  TEMPO_RAQ      =  51000; /* est utilisee uniquement dans SUIVI_MANUEL_ASSERVI */
  TEMPO_IR       =  52000;
  TEMPO_TERMIOS  =  53000;
  TEMPO_CLAVIER  =  25000;
  TEMPO_CAPTEURS =  50000;
  TEMPO_LCD      =  2000000 ; 
  
   //----------------------------------------------------------------------
   // Lecture des variables dans la config lue dans le fichier de config
   //-----------ASTRE_PAR_DEFAUT-------------------------------------------

  memset( ASTRE_PAR_DEFAUT, ZERO_CHAR, sizeof( ASTRE_PAR_DEFAUT ) ) ;

  memset( CONFIG_REP_CAT, ZERO_CHAR, sizeof( CONFIG_REP_CAT ) ) ;
  memset( CONFIG_REP_CFG, ZERO_CHAR, sizeof( CONFIG_REP_CFG ) ) ;
  memset( CONFIG_REP_LOG, ZERO_CHAR, sizeof( CONFIG_REP_LOG ) ) ;
  memset( CONFIG_REP_IN, ZERO_CHAR, sizeof( CONFIG_REP_IN ) ) ;
  memset( CONFIG_REP_SCR, ZERO_CHAR, sizeof( CONFIG_REP_SCR ) ) ;

  memset( CONFIG_FIC_LOG, ZERO_CHAR, sizeof( CONFIG_FIC_LOG ) ) ;
  memset( CONFIG_FIC_DATE, ZERO_CHAR, sizeof( CONFIG_FIC_DATE ) ) ;
  memset( CONFIG_FIC_HHMM, ZERO_CHAR, sizeof( CONFIG_FIC_HHMM ) ) ;
  memset( CONFIG_SCR_KERNEL, ZERO_CHAR, sizeof( CONFIG_SCR_KERNEL ) ) ;

  for(l=0;l<DATAS_NB_LIGNES;l++) {
     
      // FIXME : note 2021 : les variables GPIO_xxx sont gérées dans le ficheir gpio.c

     if(!strcmp("ASTRE_PAR_DEFAUT",g_Datas[l][0])) strcpy( ASTRE_PAR_DEFAUT, g_Datas[l][1]) ;

     if(!strcmp("MENU_PAR_DEFAUT",g_Datas[l][0])) {
      if(!strcmp(g_Datas[l][1],c_Menus[ MENU_AZIMUTAL ]))           MENU_PAR_DEFAUT = MENU_AZIMUTAL ;
      if(!strcmp(g_Datas[l][1],c_Menus[ MENU_EQUATORIAL ]))         MENU_PAR_DEFAUT = MENU_EQUATORIAL ;
      if(!strcmp(g_Datas[l][1],c_Menus[ MENU_MANUEL_BRUT ]))        MENU_PAR_DEFAUT = MENU_MANUEL_BRUT ;
      if(!strcmp(g_Datas[l][1],c_Menus[ MENU_MANUEL_NON_ASSERVI ])) MENU_PAR_DEFAUT = MENU_MANUEL_NON_ASSERVI ;
      if(!strcmp(g_Datas[l][1],c_Menus[ MENU_MANUEL_ASSERVI ]))     MENU_PAR_DEFAUT = MENU_MANUEL_ASSERVI ;
      if(!strcmp(g_Datas[l][1],c_Menus[ MENU_GOTO ]))               MENU_PAR_DEFAUT = MENU_GOTO ;
      if(!strcmp(g_Datas[l][1],c_Menus[ MENU_INFO ]))               MENU_PAR_DEFAUT = MENU_INFO ;
      if(!strcmp(g_Datas[l][1],c_Menus[ MENU_RESEAU_UP ]))          MENU_PAR_DEFAUT = MENU_RESEAU_UP ;
      if(!strcmp(g_Datas[l][1],c_Menus[ MENU_RESEAU_DOWN ]))        MENU_PAR_DEFAUT = MENU_RESEAU_DOWN ;
      if(!strcmp(g_Datas[l][1],c_Menus[ MENU_PROGRAMME_DOWN ]))     MENU_PAR_DEFAUT = MENU_PROGRAMME_DOWN ;
      if(!strcmp(g_Datas[l][1],c_Menus[ MENU_DOWN ]))               MENU_PAR_DEFAUT = MENU_DOWN ;
     } 
      /* Definition de valeurs par defauts pour les TEMPO */ 
     if(!strcmp("TEMPO_RAQ",g_Datas[l][0]))      TEMPO_RAQ=atol(g_Datas[l][1]);
     if(!strcmp("TEMPO_MENU",g_Datas[l][0]))     TEMPO_MENU=atol(g_Datas[l][1]);
     if(!strcmp("TEMPO_IR",g_Datas[l][0]))       TEMPO_IR=atol(g_Datas[l][1]);
     if(!strcmp("TEMPO_TERMIOS",g_Datas[l][0]))  TEMPO_TERMIOS=atol(g_Datas[l][1]);
     if(!strcmp("TEMPO_CLAVIER",g_Datas[l][0]))  TEMPO_CLAVIER=atol(g_Datas[l][1]);
     if(!strcmp("TEMPO_CAPTEURS",g_Datas[l][0])) TEMPO_CAPTEURS=atol(g_Datas[l][1]);
     if(!strcmp("TEMPO_LCD",g_Datas[l][0]))      TEMPO_LCD=atol(g_Datas[l][1]);
     
     if(!strcmp("GPIO_LED_ETAT",g_Datas[l][0]))      GPIO_LED_ETAT=atoi(g_Datas[l][1]);

     if(!strcmp("DEVICE_USE_CONTROLER",g_Datas[l][0]))  DEVICE_USE_CONTROLER=atoi(g_Datas[l][1]);
     if(!strcmp("DEVICE_USE_CAPTEURS",g_Datas[l][0]))   DEVICE_USE_CAPTEURS=atoi(g_Datas[l][1]);
     if(!strcmp("DEVICE_USE_RAQUETTE",g_Datas[l][0]))   DEVICE_USE_RAQUETTE=atoi(g_Datas[l][1]);
     if(!strcmp("DEVICE_USE_BLUETOOTH",g_Datas[l][0]))  DEVICE_USE_BLUETOOTH=atoi(g_Datas[l][1]);
     if(!strcmp("DEVICE_USE_INFRAROUGE",g_Datas[l][0])) DEVICE_USE_INFRAROUGE=atoi(g_Datas[l][1]);
     if(!strcmp("DEVICE_USE_KEYBOARD",g_Datas[l][0]))   DEVICE_USE_KEYBOARD=atoi(g_Datas[l][1]);
     if(!strcmp("DEVICE_USE_LCD",g_Datas[l][0]))        DEVICE_USE_LCD=atoi(g_Datas[l][1]);

     if(!strcmp("ALT_ROT",g_Datas[l][0]))      ALT_ROT=atoi(g_Datas[l][1]);
     if(!strcmp("AZI_ROT",g_Datas[l][0]))      AZI_ROT=atoi(g_Datas[l][1]);
     
     if(!strcmp("ALT_F",g_Datas[l][0]))        ALT_F=atol(g_Datas[l][1]);
     if(!strcmp("AZI_F",g_Datas[l][0]))        AZI_F=atol(g_Datas[l][1]);
     
     if(!strcmp("ALT_N",g_Datas[l][0]))        ALT_N=atoi(g_Datas[l][1]);
     if(!strcmp("AZI_N",g_Datas[l][0]))        AZI_N=atoi(g_Datas[l][1]);
     
     if(!strcmp("ALT_R",g_Datas[l][0]))        ALT_R=atof(g_Datas[l][1]);
     if(!strcmp("AZI_R",g_Datas[l][0]))        AZI_R=atof(g_Datas[l][1]);
     
     if(!strcmp("GPIO_RAQ_O",g_Datas[l][0]))   GPIO_RAQ_O=atoi(g_Datas[l][1]);
     if(!strcmp("GPIO_RAQ_E",g_Datas[l][0]))   GPIO_RAQ_E=atoi(g_Datas[l][1]);
     if(!strcmp("GPIO_RAQ_S",g_Datas[l][0]))   GPIO_RAQ_S=atoi(g_Datas[l][1]);
     if(!strcmp("GPIO_RAQ_N",g_Datas[l][0]))   GPIO_RAQ_N=atoi(g_Datas[l][1]);
     if(!strcmp("GPIO_RAQ_V",g_Datas[l][0]))   GPIO_RAQ_V=atoi(g_Datas[l][1]);
     
     if(!strcmp("GPIO_KEY_L1",g_Datas[l][0]))   GPIO_KEY_L1=atoi(g_Datas[l][1]);
     if(!strcmp("GPIO_KEY_L2",g_Datas[l][0]))   GPIO_KEY_L2=atoi(g_Datas[l][1]);
     if(!strcmp("GPIO_KEY_L3",g_Datas[l][0]))   GPIO_KEY_L3=atoi(g_Datas[l][1]);
     if(!strcmp("GPIO_KEY_L4",g_Datas[l][0]))   GPIO_KEY_L4=atoi(g_Datas[l][1]);
     
     if(!strcmp("GPIO_KEY_C1",g_Datas[l][0]))   GPIO_KEY_C1=atoi(g_Datas[l][1]);
     if(!strcmp("GPIO_KEY_C2",g_Datas[l][0]))   GPIO_KEY_C2=atoi(g_Datas[l][1]);
     if(!strcmp("GPIO_KEY_C3",g_Datas[l][0]))   GPIO_KEY_C3=atoi(g_Datas[l][1]);
     if(!strcmp("GPIO_KEY_C4",g_Datas[l][0]))   GPIO_KEY_C4=atoi(g_Datas[l][1]);

     if(!strcmp("LONGITUDE",g_Datas[l][0]))     LONGITUDE=atof(g_Datas[l][1]);
     if(!strcmp("LATITUDE",g_Datas[l][0]))      LATITUDE=atof(g_Datas[l][1]);
     if(!strcmp("ALTITUDE",g_Datas[l][0]))      ALTITUDE=atof(g_Datas[l][1]);

     // devices de altitude

     if(!strcmp("ALT_R1",g_Datas[l][0]))       ALT_R1 = atof(g_Datas[l][1]);         
     if(!strcmp("ALT_R2",g_Datas[l][0]))       ALT_R2 = atof(g_Datas[l][1]);
     if(!strcmp("ALT_R3",g_Datas[l][0]))       ALT_R3 = atof(g_Datas[l][1]);         
     if(!strcmp("ALT_R4",g_Datas[l][0]))       ALT_R4 = atof(g_Datas[l][1]);  
     if(!strcmp("ALT_R5",g_Datas[l][0]))       ALT_R5 = atof(g_Datas[l][1]);
     if(!strcmp("ALT_R6",g_Datas[l][0]))       ALT_R6 = atof(g_Datas[l][1]);       
     if(!strcmp("ALT_R7",g_Datas[l][0]))       ALT_R7 = atof(g_Datas[l][1]);

     if(!strcmp("ALT_ACC",g_Datas[l][0]))      ALT_ACC= atof(g_Datas[l][1])      ;

     if(!strcmp("GPIO_DIR_ALT",g_Datas[l][0])) GPIO_DIR_ALT=atoi(g_Datas[l][1]);  
     if(!strcmp("GPIO_CLK_ALT",g_Datas[l][0])) GPIO_CLK_ALT=atoi(g_Datas[l][1]);  
     if(!strcmp("GPIO_SLP_ALT",g_Datas[l][0])) GPIO_SLP_ALT=atoi(g_Datas[l][1]);  
     if(!strcmp("GPIO_RST_ALT",g_Datas[l][0])) GPIO_RST_ALT=atoi(g_Datas[l][1]);  
     if(!strcmp("GPIO_MMM_ALT",g_Datas[l][0])) GPIO_MMM_ALT=atoi(g_Datas[l][1]);  
     if(!strcmp("GPIO_ENA_ALT",g_Datas[l][0])) GPIO_ENA_ALT=atoi(g_Datas[l][1]);  
     if(!strcmp("GPIO_M2_ALT",g_Datas[l][0]))  GPIO_M2_ALT=atoi(g_Datas[l][1]);  
     if(!strcmp("GPIO_M1_ALT",g_Datas[l][0]))  GPIO_M1_ALT=atoi(g_Datas[l][1]);  
     if(!strcmp("GPIO_M0_ALT",g_Datas[l][0]))  GPIO_M0_ALT=atoi(g_Datas[l][1]);  

     if(!strcmp("MCP_DIR_ALT",g_Datas[l][0]))  MCP_DIR_ALT=atoi(g_Datas[l][1])  ;  
     if(!strcmp("MCP_CLK_ALT",g_Datas[l][0]))  MCP_CLK_ALT=atoi(g_Datas[l][1])  ;  
     if(!strcmp("MCP_SLP_ALT",g_Datas[l][0]))  MCP_SLP_ALT=atoi(g_Datas[l][1])  ;  
     if(!strcmp("MCP_RST_ALT",g_Datas[l][0]))  MCP_RST_ALT=atoi(g_Datas[l][1])  ;  
     if(!strcmp("MCP_M2_ALT",g_Datas[l][0]))   MCP_M2_ALT=atoi(g_Datas[l][1])   ;  
     if(!strcmp("MCP_M1_ALT",g_Datas[l][0]))   MCP_M1_ALT=atoi(g_Datas[l][1])   ;  
     if(!strcmp("MCP_M0_ALT",g_Datas[l][0]))   MCP_M0_ALT=atoi(g_Datas[l][1])   ;  
     
     // devices de azimut

     if(!strcmp("AZI_R1",g_Datas[l][0]))       AZI_R1 = atof(g_Datas[l][1])      ; 
     if(!strcmp("AZI_R2",g_Datas[l][0]))       AZI_R2 = atof(g_Datas[l][1])      ; 
     if(!strcmp("AZI_R3",g_Datas[l][0]))       AZI_R3 = atof(g_Datas[l][1])      ; 
     if(!strcmp("AZI_R4",g_Datas[l][0]))       AZI_R4 = atof(g_Datas[l][1])      ; 
     if(!strcmp("AZI_R5",g_Datas[l][0]))       AZI_R5 = atof(g_Datas[l][1])      ; 
     if(!strcmp("AZI_R6",g_Datas[l][0]))       AZI_R6 = atof(g_Datas[l][1])      ; 
     if(!strcmp("AZI_R7",g_Datas[l][0]))       AZI_R7 = atof(g_Datas[l][1])      ; 

     if(!strcmp("AZI_ACC",g_Datas[l][0]))      AZI_ACC= atof(g_Datas[l][1])      ;

     if(!strcmp("GPIO_DIR_AZI",g_Datas[l][0])) GPIO_DIR_AZI=atoi(g_Datas[l][1]) ; 
     if(!strcmp("GPIO_CLK_AZI",g_Datas[l][0])) GPIO_CLK_AZI=atoi(g_Datas[l][1]) ; 
     if(!strcmp("GPIO_SLP_AZI",g_Datas[l][0])) GPIO_SLP_AZI=atoi(g_Datas[l][1]) ;
     if(!strcmp("GPIO_RST_AZI",g_Datas[l][0])) GPIO_RST_AZI=atoi(g_Datas[l][1]);  
     if(!strcmp("GPIO_MMM_AZI",g_Datas[l][0])) GPIO_MMM_AZI=atoi(g_Datas[l][1]);  
     if(!strcmp("GPIO_ENA_AZI",g_Datas[l][0])) GPIO_ENA_AZI=atoi(g_Datas[l][1]);
     if(!strcmp("GPIO_M2_AZI",g_Datas[l][0]))  GPIO_M2_AZI=atoi(g_Datas[l][1])  ;
     if(!strcmp("GPIO_M1_AZI",g_Datas[l][0]))  GPIO_M1_AZI=atoi(g_Datas[l][1])  ; 
     if(!strcmp("GPIO_M0_AZI",g_Datas[l][0]))  GPIO_M0_AZI=atoi(g_Datas[l][1])  ; 

     if(!strcmp("MCP_DIR_AZI",g_Datas[l][0]))  MCP_DIR_AZI=atoi(g_Datas[l][1])   ;
     if(!strcmp("MCP_CLK_AZI",g_Datas[l][0]))  MCP_CLK_AZI=atoi(g_Datas[l][1])   ;
     if(!strcmp("MCP_SLP_AZI",g_Datas[l][0]))  MCP_SLP_AZI=atoi(g_Datas[l][1])   ;
     if(!strcmp("MCP_RST_AZI",g_Datas[l][0]))  MCP_RST_AZI=atoi(g_Datas[l][1])   ; 
     if(!strcmp("MCP_M2_AZI",g_Datas[l][0]))   MCP_M2_AZI=atoi(g_Datas[l][1])    ;
     if(!strcmp("MCP_M1_AZI",g_Datas[l][0]))   MCP_M1_AZI=atoi(g_Datas[l][1])    ;
     if(!strcmp("MCP_M0_AZI",g_Datas[l][0]))   MCP_M0_AZI=atoi(g_Datas[l][1])   ;

     // devices de azimut et altitude (qui concernent les 2 en mm temps)
     
     if(!strcmp("ALTAZ_FORWARD",g_Datas[l][0])) ALTAZ_FORWARD= atof(g_Datas[l][1])      ;
     if(!strcmp("ALTAZ_REWIND",g_Datas[l][0]))  ALTAZ_REWIND= atof(g_Datas[l][1])      ;

     if(!strcmp("ALTAZ_FORWARD_FAST",g_Datas[l][0])) ALTAZ_FORWARD_FAST= atof(g_Datas[l][1])      ;
     if(!strcmp("ALTAZ_REWIND_FAST",g_Datas[l][0]))  ALTAZ_REWIND_FAST= atof(g_Datas[l][1])      ;

     // chemins des repertoires et fichiers (2021)

    if(!strcmp("CONFIG_REP_CAT",g_Datas[l][0]))  strcpy( CONFIG_REP_CAT, g_Datas[l][1]) ;
    if(!strcmp("CONFIG_REP_CFG",g_Datas[l][0]))  strcpy( CONFIG_REP_CFG, g_Datas[l][1]) ;
    if(!strcmp("CONFIG_REP_LOG",g_Datas[l][0]))  strcpy( CONFIG_REP_LOG, g_Datas[l][1]) ;
    if(!strcmp("CONFIG_REP_IN",g_Datas[l][0]))   strcpy( CONFIG_REP_IN, g_Datas[l][1]) ;
    if(!strcmp("CONFIG_FIC_LOG",g_Datas[l][0]))  strcpy( CONFIG_FIC_LOG, g_Datas[l][1]) ;
    if(!strcmp("CONFIG_FIC_DATE",g_Datas[l][0])) strcpy( CONFIG_FIC_DATE, g_Datas[l][1]) ;
    if(!strcmp("CONFIG_FIC_HHMM",g_Datas[l][0])) strcpy( CONFIG_FIC_HHMM, g_Datas[l][1]) ;  

    if(!strcmp("CONFIG_REP_SCR",g_Datas[l][0]))    strcpy( CONFIG_REP_SCR, g_Datas[l][1]) ;  
    if(!strcmp("CONFIG_SCR_KERNEL",g_Datas[l][0])) strcpy( CONFIG_SCR_KERNEL, g_Datas[l][1]) ;  
  }

  //if ( ALT_R == 0 ) ALT_R = ALT_R1 * ALT_R2 * ALT_R3 * ALT_R4 ;
  //if ( AZI_R == 0 ) AZI_R = AZI_R1 * AZI_R2 * AZI_R3 * AZI_R4 ;
  
  /* Attention ALT_R4 (micro pas) est traite independamment */

  if ( ALT_R == 0 ) {
    ALT_R = ALT_R1 * ALT_R2 * ALT_R3 * ALT_R5 * ALT_R6 * ALT_R7 ;
  }
  /* Attention ALT_R4 (micro pas) est traite independamment */

  if ( AZI_R == 0 ) {
    AZI_R = AZI_R1 * AZI_R2 * AZI_R3 * AZI_R5 * AZI_R6 * AZI_R7 ;
  }
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_VARIABLES
* @author : s.gravois
* @brief  : Cette fonction affiche les parametres 
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void   CONFIG_AFFICHER_VARIABLES(void) {

  Trace("TEMPO_RAQ = %ld",  TEMPO_RAQ);
  Trace("TEMPO_MENU = %ld",  TEMPO_MENU);
  Trace("TEMPO_IR = %ld",  TEMPO_IR);
  Trace("TEMPO_TERMIOS = %ld",  TEMPO_TERMIOS);
  Trace("TEMPO_CLAVIER = %ld",  TEMPO_CLAVIER);
  Trace("TEMPO_CAPTEURS = %ld",  TEMPO_CAPTEURS);
  Trace("TEMPO_LCD = %ld",  TEMPO_LCD);

  Trace1("DEVICE_USE_CONTROLER = %d",  DEVICE_USE_CONTROLER);
  Trace1("DEVICE_USE_CAPTEURS = %d",  DEVICE_USE_CAPTEURS);
  Trace1("DEVICE_USE_BLUETOOTH = %d",  DEVICE_USE_BLUETOOTH);
  Trace1("DEVICE_USE_INFRAROUGE = %d",  DEVICE_USE_INFRAROUGE);
  Trace1("DEVICE_USE_RAQUETTE = %d",  DEVICE_USE_RAQUETTE);
  Trace1("DEVICE_USE_KEYBOARD = %d",  DEVICE_USE_KEYBOARD);
  Trace1("DEVICE_USE_LCD = %d",  DEVICE_USE_LCD);

  Trace1("ASTRE_PAR_DEFAUT = %s",  ASTRE_PAR_DEFAUT );
  Trace1("MENU_PAR_DEFAUT = %s",  c_Menus[MENU_PAR_DEFAUT]);

  Trace1("LATITUDE  = %f",          LATITUDE );
  Trace1("LONGITUDE = %f",          LONGITUDE );
  Trace1("ALTITUDE  = %f",          ALTITUDE );

  Trace1("GPIO_LED_ETAT = %d", GPIO_LED_ETAT );

  Trace1("ALT_R1 = %f",       ALT_R1);         
  Trace1("ALT_R2 = %f",       ALT_R2);
  Trace1("ALT_R3 = %f",       ALT_R3);         
  Trace1("ALT_R4 = %f",       ALT_R4);         
  Trace1("ALT_ROT = %d",      ALT_ROT);
  Trace1("ALT_ACC = %f",      ALT_ACC);

  Trace1("AZI_R1 = %f",       AZI_R1)      ; 
  Trace1("AZI_R2 = %f",       AZI_R2)      ; 
  Trace1("AZI_R3 = %f",       AZI_R3)      ; 
  Trace1("AZI_R4 = %f",       AZI_R4)      ; 
  Trace1("AZI_ROT = %d",      AZI_ROT)     ;
  Trace1("AZI_ACC = %f",      AZI_ACC)     ; 

  Trace1("CONFIG_REP_CAT = %s", CONFIG_REP_CAT)  ;
  Trace1("CONFIG_REP_CFG = %s", CONFIG_REP_CFG)  ; 
  Trace1("CONFIG_REP_LOG = %s", CONFIG_REP_LOG)  ; 
  Trace1("CONFIG_REP_IN = %s", CONFIG_REP_IN)  ; 
  Trace1("CONFIG_FIC_LOG = %s", CONFIG_FIC_LOG)  ; 
  Trace1("CONFIG_FIC_DATE = %s", CONFIG_FIC_DATE)  ; 
  Trace1("CONFIG_FIC_HHMM = %s", CONFIG_FIC_HHMM)  ;  

  Trace1("CONFIG_REP_SCR = %s", CONFIG_REP_SCR)  ; 
  Trace1("CONFIG_SCR_KERNEL = %s", CONFIG_SCR_KERNEL)  ; 

  Trace1("GPIO_ALT = %s", GPIO_ALT)  ;  
  Trace1("GPIO_AZI = %s", GPIO_AZI)  ;  
  Trace1("GPIO_MAS = %s", GPIO_MAS)  ;  
  Trace1("GPIO_FREQUENCE_PWM = %s", GPIO_FREQUENCE_PWM)  ;  

  Trace2("anciennes variables\n");
  Trace2("GPIO_RAQ_O   = %d",  GPIO_RAQ_O);
  Trace2("GPIO_RAQ_E   = %d",  GPIO_RAQ_E);
  Trace2("GPIO_RAQ_S   = %d",  GPIO_RAQ_S);
  Trace2("GPIO_RAQ_N   = %d",  GPIO_RAQ_N);
  Trace2("GPIO_RAQ_V   = %d",  GPIO_RAQ_V);
  Trace2("GPIO_KEY_L1  = %d",  GPIO_KEY_L1);
  Trace2("GPIO_KEY_L2  = %d",  GPIO_KEY_L2);
  Trace2("GPIO_KEY_L3  = %d",  GPIO_KEY_L3);
  Trace2("GPIO_KEY_L4  = %d",  GPIO_KEY_L4);
  Trace2("GPIO_KEY_C1  = %d",  GPIO_KEY_C1);
  Trace2("GPIO_KEY_C2  = %d",  GPIO_KEY_C2);
  Trace2("GPIO_KEY_C3  = %d",  GPIO_KEY_C3);
  Trace2("GPIO_KEY_C4  = %d",  GPIO_KEY_C4);
  Trace2("GPIO_DIR_ALT = %d", GPIO_DIR_ALT);  
  Trace2("GPIO_CLK_ALT = %d", GPIO_CLK_ALT);  
  Trace2("GPIO_SLP_ALT = %d", GPIO_SLP_ALT);  
  Trace2("GPIO_RST_ALT = %d", GPIO_RST_ALT);  
  Trace2("GPIO_MMM_ALT = %d", GPIO_MMM_ALT);  
  Trace2("GPIO_ENA_ALT = %d", GPIO_ENA_ALT);  
  Trace2("GPIO_M2_ALT = %d",  GPIO_M2_ALT)  ;  
  Trace2("GPIO_M1_ALT = %d",  GPIO_M1_ALT)  ;  
  Trace2("GPIO_M0_ALT = %d",  GPIO_M0_ALT)  ;  
  Trace2("GPIO_DIR_AZI = %d", GPIO_DIR_AZI) ; 
  Trace2("GPIO_CLK_AZI = %d", GPIO_CLK_AZI) ; 
  Trace2("GPIO_SLP_AZI = %d", GPIO_SLP_AZI) ;
  Trace2("GPIO_RST_AZI = %d", GPIO_RST_AZI);  
  Trace2("GPIO_MMM_AZI = %d", GPIO_MMM_AZI);  
  Trace2("GPIO_ENA_AZI = %d", GPIO_ENA_AZI);  
  Trace2("GPIO_M2_AZI = %d",  GPIO_M2_AZI)  ;
  Trace2("GPIO_M1_AZI = %d",  GPIO_M1_AZI)  ; 
  Trace2("GPIO_M0_AZI = %d",  GPIO_M0_AZI)  ; 
  Trace2("=====================================================\n");
  Trace2("MCP_DIR_AZI = %d",  MCP_DIR_AZI)   ;
  Trace2("MCP_CLK_AZI = %d",  MCP_CLK_AZI)   ;
  Trace2("MCP_SLP_AZI = %d",  MCP_SLP_AZI)   ;
  Trace2("MCP_RST_AZI = %d",  MCP_RST_AZI)   ; 
  Trace2("MCP_M2_AZI = %d",   MCP_M2_AZI)    ;
  Trace2("MCP_M1_AZI = %d",   MCP_M1_AZI)    ;
  Trace2("MCP_M0_AZI = %d",   MCP_M0_AZI)   ;
  Trace2("MCP_DIR_ALT = %d",  MCP_DIR_ALT)  ;  
  Trace2("MCP_CLK_ALT = %d",  MCP_CLK_ALT)  ;  
  Trace2("MCP_SLP_ALT = %d",  MCP_SLP_ALT)  ;  
  Trace2("MCP_RST_ALT = %d",  MCP_RST_ALT)  ;  
  Trace2("MCP_M2_ALT = %d",   MCP_M2_ALT)   ;  
  Trace2("MCP_M1_ALT = %d",   MCP_M1_ALT)   ;  
  Trace2("MCP_M0_ALT = %d",   MCP_M0_ALT)   ;  
}
/*****************************************************************************************
* @fn     : CONFIG_GETCWD
* @author : s.gravois
* @brief  : Cette fonction obtient le nom du repertoire courant d execution
* @param  : char * c_getcwd
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : (obsolete) re-verifier mais a priori BUG lors de l'appel 
*           => appel direct a getcwd dans astro.c
*****************************************************************************************/

int CONFIG_GETCWD(char * c_getcwd) {

  if (getcwd(c_getcwd, sizeof(c_getcwd)) != NULL) {
     Trace("Current working dir: %s\n", c_getcwd);
  } else {
     perror("getcwd() error");
     return 1;
  }
  return 0 ;
} 
/*****************************************************************************************
* @fn     : CONFIG_READ
* @author : s.gravois
* @brief  : Cette fonction lit les parametres  dans le fichier de configuration
* @param  : g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : voir si un passage par librairie JSON plus pratique (comme pour mesDep)
*****************************************************************************************/

int CONFIG_READ(char g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) {

  FILE *fin ;
  char buf   [CONFIG_TAILLE_BUFFER] ;
  char buffer[CONFIG_TAILLE_BUFFER] ;
  char c ;
  int i,C,L ;

  // FIXME : initialisation du tableau de valeurs lues dans le fichier = g_Datas

  Trace1(" DATAS_NB_LIGNES   = %d",   DATAS_NB_LIGNES) ;
  Trace1(" DATAS_NB_COLONNES = %d", DATAS_NB_COLONNES) ;

  for(L=0;L<DATAS_NB_LIGNES;L++) {
    for(C=0;C<DATAS_NB_COLONNES;C++) { 
      memset(g_Datas[L][C],ZERO_CHAR,CONFIG_TAILLE_BUFFER-1);
    }
  }
  
  // FIXME : construction du chemin du fichier de configuration
  // FIXME : la variable CONFIG_REP_HOME doit etre lue auparavant (getcwd) (2021)

  memset(buf,ZERO_CHAR,CONFIG_TAILLE_BUFFER-1);
  sprintf(buf,"%s/%s",CONFIG_REP_HOME, CONFIG_FIC_CFG) ;
  
  // FIXME : ouverture du fichier de configuration


  if ( (fin=fopen(buf,"r")) == NULL) {
   memset(buffer,ZERO_CHAR,CONFIG_TAILLE_BUFFER);
   sprintf(buffer,"Pbme ouverture %s",buf) ;
   //LOG(buffer) ;
   Trace("probleme ouverture 4 %s",buf) ; 
   exit(2) ;
  }
  else Trace("open %s ok", buf) ;

  memset(buf,ZERO_CHAR,CONFIG_TAILLE_BUFFER-1);
  i=0;
  L=0;
  C=0;
  
  // FIXME : lecture du fichier de configuration 

  while(1) {
   
   c=fgetc(fin) ;
   if (CONFIG_FIN_FICHIER(c)) break ;
   if (CONFIG_FIN_LIGNE(c) && i==0 ) continue ;
   if (CONFIG_FIN_MOT(c) && i==0 ) continue ;
   if (CONFIG_DEB_COM(c)) {
     while(!CONFIG_FIN_LIGNE(fgetc(fin)));
     L++;
     C=0;
   }
   if (CONFIG_FIN_MOT(c)||(CONFIG_FIN_LIGNE(c)&& i!=0)||i==CONFIG_TAILLE_BUFFER) {
    
    if (CONFIG_FIN_MOT(c)) {
      memset(g_Datas[L][C],ZERO_CHAR,CONFIG_TAILLE_BUFFER);
      strcpy(g_Datas[L][C],buf);

      Trace1("g_Datas fin mot[%d][%d]=(%s)",L,C,g_Datas[L][C] );
      
      C++;
    }
    if ((CONFIG_FIN_LIGNE(c)&& i!=0)||i==CONFIG_TAILLE_BUFFER) {
      memset(g_Datas[L][C],ZERO_CHAR,CONFIG_TAILLE_BUFFER);
      strcpy(g_Datas[L][C],buf);
      
      Trace1("g_Datas fin lig[%d][%d]=(%s)",L,C,g_Datas[L][C] );
      
      L++;
      C=0;
    }
    memset(buf,ZERO_CHAR,CONFIG_TAILLE_BUFFER);
    i=0;
    
   }
   if (CONFIG_FORMAT_ADMIS(c)){buf[i]=(char)c;i++;}
  }
  fclose(fin);

  return 0 ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_DATAS
* @author : s.gravois
* @brief  : Cette fonction affiche le tabeau de parametres lu a partir du fichier de config
* @param  : char g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_AFFICHER_DATAS(char g_Datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) {
  int L, C ;  

  for(L=0;L<DATAS_NB_LIGNES;L++) {
    for(C=0;C<DATAS_NB_COLONNES;C++) { 
      if ( C>0 ) {
         Trace2("") ;
      }
      if (strlen(g_Datas[L][C])) {
        Trace2(" %s " , g_Datas[L][C]) ;
      }
    }
    Trace2("") ;
  }
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_LIEU
* @author : s.gravois
* @brief  : Cette fonction affiche les informations du lieu d observation
* @param  : LIEU *lieu
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_AFFICHER_LIEU(LIEU *lieu) {

  Trace1(" latitude   :  %.2f", lieu->lat * DEGRES ) ; 
  Trace1(" longitude  :  %.2f", lieu->lon * DEGRES ) ;
  Trace1("lieu->JD    : %f",lieu->JD) ;
  Trace1("lieu->JJ    : %f",lieu->JJ) ;
  Trace1("lieu->TS    : %f",lieu->TS) ;
  Trace1("lieu->TSR   : %f",lieu->TSR) ;

  Trace1("lieu->alt = %f",lieu->alt ) ;


  Trace1("lieu->lat (deg) = %f",lieu->lat * DEGRES ) ;
  Trace1("lieu->lon (deg) = %f",lieu->lon * DEGRES ) ;

  Trace1("----------------------------") ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_TEMPS
* @author : s.gravois
* @brief  : Cette fonction affiche les informations de temps
* @param  : LIEU *lieu
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_AFFICHER_TEMPS(TEMPS *temps) {
  
  Trace(" DATE / HOUR : %d-%d-%d %d:%d:%d : %f", temps->yy, temps->mm, temps->dd, temps->HH, temps->MM, temps->SS, temps->hd ) ;

  Trace1("temps->yy = %d", temps->yy ) ;
  Trace1("temps->mm = %d", temps->mm ) ;
  Trace1("temps->dd = %d", temps->dd ) ;
  Trace1("temps->HH = %d", temps->HH ) ;
  Trace1("temps->MM = %d", temps->MM ) ;
  Trace1("temps->SS = %d", temps->SS ) ;
  Trace1("temps->hd = %f", temps->hd ) ;

  Trace("----------------------------") ;

}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_CLAVIER
* @author : s.gravois
* @brief  : Cette fonction affiche les informations d entrees input ("clavier")
* @param  : CLAVIER *clavier
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_AFFICHER_CLAVIER(CLAVIER *clavier) {
  
  Trace1("phr %s mot %s sym %s nom %s pre %s val %s menu %s",\
    clavier->phrase,\
    clavier->mot,\
    clavier->symbole,\
    clavier->nombre,\
    clavier->premier,\
    clavier->valider,\
    clavier->menu) ;

  Trace1("clavier->mot         = %s",clavier->mot) ;
  Trace1("clavier->premier     = %s",clavier->premier) ;
  Trace1("clavier->phrase      = %s",clavier->phrase) ;
  Trace1("clavier->nombre      = %s",clavier->nombre) ;
  Trace1("clavier->symbole     = %s",clavier->symbole) ;
  Trace1("clavier->phrase_lue  = %d",clavier->phrase_lue) ;
}
/*****************************************************************************************
* @fn     : CONFIG_FORMATE_DONNEES_AFFICHAGE
* @author : s.gravois
* @brief  : Cette fonction formate divers string en vue d un affichage pertinent
* @param  : ASTRE *as
* @date   : 2022-04-12 creation
* @date   : 2022-04-21 remplacement ° par o (affichage LCD ° impossible)
* @date   : 2022-04-21 ajout de 2 resolutions plus simple (affichage contraint par LCD) :
* @date   : 2022-04-21 - c_hhmm_*
* @date   : 2022-04-21 - c_dd_*  
*****************************************************************************************/

void CONFIG_FORMATE_DONNEES_AFFICHAGE(ASTRE *as) {

  char  c_hhmmss_agh[ 16] ;
  char  c_hhmmss_asc[ 16] ;
  char  c_hhmmss_azi[ 16] ;
  char  c_hhmmss_alt[ 16] ;
  char  c_hhmmss_dec[ 16] ;

  char  c_hhmm_agh[ 16] ;
  char  c_hhmm_asc[ 16] ;
  char  c_hhmm_azi[ 16] ;
  char  c_hhmm_alt[ 16] ;
  char  c_hhmm_dec[ 16] ;

  char  c_ddmm_agh[ 16] ;
  char  c_ddmm_asc[ 16] ;
  char  c_ddmm_azi[ 16] ;
  char  c_ddmm_alt[ 16] ;
  char  c_ddmm_dec[ 16] ;

  char  c_dd_agh[ 16] ;
  char  c_dd_asc[ 16] ;
  char  c_dd_azi[ 16] ;
  char  c_dd_alt[ 16] ;
  char  c_dd_dec[ 16] ;

  memset( c_hhmmss_agh, 0, sizeof(c_hhmmss_agh) ) ;
  memset( c_hhmmss_asc, 0, sizeof(c_hhmmss_asc) ) ;
  memset( c_hhmmss_azi, 0, sizeof(c_hhmmss_azi) ) ;
  memset( c_hhmmss_alt, 0, sizeof(c_hhmmss_alt) ) ;
  memset( c_hhmmss_dec, 0, sizeof(c_hhmmss_dec) ) ;

  memset( c_hhmm_agh, 0, sizeof(c_hhmm_agh) ) ;
  memset( c_hhmm_asc, 0, sizeof(c_hhmm_asc) ) ;
  memset( c_hhmm_azi, 0, sizeof(c_hhmm_azi) ) ;
  memset( c_hhmm_alt, 0, sizeof(c_hhmm_alt) ) ;
  memset( c_hhmm_dec, 0, sizeof(c_hhmm_dec) ) ;

  memset( c_ddmm_agh, 0, sizeof(c_ddmm_agh) ) ;
  memset( c_ddmm_asc, 0, sizeof(c_ddmm_asc) ) ;
  memset( c_ddmm_azi, 0, sizeof(c_ddmm_azi) ) ;
  memset( c_ddmm_alt, 0, sizeof(c_ddmm_alt) ) ;
  memset( c_ddmm_dec, 0, sizeof(c_ddmm_dec) ) ;

  memset( c_dd_agh, 0, sizeof(c_dd_agh) ) ;
  memset( c_dd_asc, 0, sizeof(c_dd_asc) ) ;
  memset( c_dd_azi, 0, sizeof(c_dd_azi) ) ;
  memset( c_dd_alt, 0, sizeof(c_dd_alt) ) ;
  memset( c_dd_dec, 0, sizeof(c_dd_dec) ) ;

  /* traitement des donnees en heures / minutes / secondes */

  sprintf( c_hhmmss_agh, " %3dh%2dm%2ds",  as->AGHt.HH, as->AGHt.MM, as->AGHt.SS  ) ;
  sprintf( c_hhmmss_asc, " %3dh%2dm%2ds",  as->ASCt.HH, as->ASCt.MM, as->ASCt.SS  ) ;
  sprintf( c_hhmmss_azi, " %3dh%2dm%2ds",  as->AZIt.HH, as->AZIt.MM, as->AZIt.SS  ) ;
  sprintf( c_hhmmss_alt, " %3dh%2dm%2ds",  as->ALTt.HH, as->ALTt.MM, as->ALTt.SS  ) ;
  sprintf( c_hhmmss_dec, " %3dh%2dm%2ds",  as->DECt.HH, as->DECt.MM, as->DECt.SS  ) ;

  sprintf( c_hhmm_agh, " %3d h %2d m",  as->AGHt.HH, as->AGHt.MM ) ;
  sprintf( c_hhmm_asc, " %3d h %2d m",  as->ASCt.HH, as->ASCt.MM ) ;
  sprintf( c_hhmm_azi, " %3d h %2d m",  as->AZIt.HH, as->AZIt.MM ) ;
  sprintf( c_hhmm_alt, " %3d h %2d m",  as->ALTt.HH, as->ALTt.MM ) ;
  sprintf( c_hhmm_dec, " %3d h %2d m",  as->DECt.HH, as->DECt.MM ) ;

  /* traitement des donnees en degres / minutes / secondes */
  /* est inclus dans l affichage le signe */

  sprintf( c_ddmm_agh, "%c %-3d d %d'", as->AGHa.c_si, as->AGHa.DD, as->AGHa.MM ) ;
  sprintf( c_ddmm_asc, "%c %-3d d %d'", as->ASCa.c_si, as->ASCa.DD, as->ASCa.MM ) ;
  sprintf( c_ddmm_azi, "%c %-3d d %d'", as->AZIa.c_si, as->AZIa.DD, as->AZIa.MM ) ;
  sprintf( c_ddmm_alt, "%c %-3d d %d'", as->ALTa.c_si, as->ALTa.DD, as->ALTa.MM ) ;
  sprintf( c_ddmm_dec, "%c %-3d d %d'", as->DECa.c_si, as->DECa.DD, as->DECa.MM ) ;

  sprintf( c_dd_agh, "%c %-3d deg", as->AGHa.c_si, as->AGHa.DD ) ;
  sprintf( c_dd_asc, "%c %-3d deg", as->ASCa.c_si, as->ASCa.DD ) ;
  sprintf( c_dd_azi, "%c %-3d deg", as->AZIa.c_si, as->AZIa.DD ) ;
  sprintf( c_dd_alt, "%c %-3d deg", as->ALTa.c_si, as->ALTa.DD ) ;
  sprintf( c_dd_dec, "%c %-3d deg", as->DECa.c_si, as->DECa.DD ) ;

  /* Sauvegarde des donnees formatees dans la structure astre */
  
  strcpy( as->c_hhmmss_agh, c_hhmmss_agh)  ;
  strcpy( as->c_hhmmss_asc, c_hhmmss_asc)  ;
  strcpy( as->c_hhmmss_azi, c_hhmmss_azi)  ;
  strcpy( as->c_hhmmss_alt, c_hhmmss_alt)  ;
  strcpy( as->c_hhmmss_dec, c_hhmmss_dec)  ;

  strcpy( as->c_hhmm_agh, c_hhmm_agh)  ;
  strcpy( as->c_hhmm_asc, c_hhmm_asc)  ;
  strcpy( as->c_hhmm_azi, c_hhmm_azi)  ;
  strcpy( as->c_hhmm_alt, c_hhmm_alt)  ;
  strcpy( as->c_hhmm_dec, c_hhmm_dec)  ;

  strcpy( as->c_ddmm_agh, c_ddmm_agh)  ;
  strcpy( as->c_ddmm_asc, c_ddmm_asc)  ;
  strcpy( as->c_ddmm_azi, c_ddmm_azi)  ;
  strcpy( as->c_ddmm_alt, c_ddmm_alt)  ;
  strcpy( as->c_ddmm_dec, c_ddmm_dec)  ;

  strcpy( as->c_dd_agh, c_dd_agh)  ;
  strcpy( as->c_dd_asc, c_dd_asc)  ;
  strcpy( as->c_dd_azi, c_dd_azi)  ;
  strcpy( as->c_dd_alt, c_dd_alt)  ;
  strcpy( as->c_dd_dec, c_dd_dec)  ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_MODE_STELLARIUM
* @author : s.gravois
* @brief  : Cette fonction affiche les informations a la sauce stellarium
* @param  : 
* @date   : 2022-03-18 creation
* @date   : 2022-03-28 simplification
*****************************************************************************************/

void CONFIG_AFFICHER_MODE_STELLARIUM(ASTRE *as) {

  Trace("Va / Vh    : %3.2f / %3.2f" , as->Va           , as->Vh ) ;
  Trace("AD / Dec   : %s / %s"       , as->c_hhmmss_asc , as->c_ddmm_dec ) ;
  Trace("AH / Dec   : %s / %s"       , as->c_hhmmss_agh , as->c_ddmm_dec ) ;
  Trace("AZ./ Haut. : %s / %s"       , as->c_ddmm_azi   , as->c_ddmm_alt ) ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_MODE_LONG
* @author : s.gravois
* @brief  : Cette fonction affiche les informations relatives a l as observee
* @param  : ASTRE *as
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
*****************************************************************************************/

void CONFIG_AFFICHER_MODE_LONG(ASTRE *as) {
  
  const char * c_nom  = as->nom ;
  const char * c_type = c_Astre_Type [ as->type ] ;
  const char * c_mode = c_Mode_Calcul[ as->mode ] ;

  char  c_hhmmss_agh0[ 16] ;
  char  c_hhmmss_agh1[ 16] ;
  char  c_hhmmss_agh2[ 16] ;

  char  c_hhmmss_azi0[ 16] ;
  char  c_hhmmss_azi1[ 16] ;
  char  c_hhmmss_azi2[ 16] ;

  char  c_hhmmss_agh[ 16] ;
  char  c_hhmmss_asc[ 16] ;

  memset( c_hhmmss_agh0, 0, sizeof(c_hhmmss_agh0) ) ;
  memset( c_hhmmss_agh1, 0, sizeof(c_hhmmss_agh1) ) ;
  memset( c_hhmmss_agh2, 0, sizeof(c_hhmmss_agh2) ) ;
  memset( c_hhmmss_agh, 0, sizeof(c_hhmmss_agh) ) ;
  memset( c_hhmmss_asc, 0, sizeof(c_hhmmss_asc) ) ;

  sprintf( c_hhmmss_agh, "%dh%dm%ds",   as->AGHt.HH,  as->AGHt.MM,  as->AGHt.SS  ) ;
  sprintf( c_hhmmss_asc,  "%dh%dm%ds",  as->ASCt.HH,   as->ASCt.MM,   as->ASCt.SS  ) ;

  sprintf( c_hhmmss_agh0, "%dh%dm%ds", as->AGH0t.HH, as->AGH0t.MM, as->AGH0t.SS  ) ;
  sprintf( c_hhmmss_agh1, "%dh%dm%ds", as->AGH1t.HH, as->AGH1t.MM, as->AGH1t.SS  ) ;
  sprintf( c_hhmmss_agh2, "%dh%dm%ds", as->AGH2t.HH, as->AGH2t.MM, as->AGH2t.SS  ) ;

  sprintf( c_hhmmss_azi0, "%dh%dm%ds", as->AZI0t.HH, as->AZI0t.MM, as->AZI0t.SS  ) ;
  sprintf( c_hhmmss_azi1, "%dh%dm%ds", as->AZI1t.HH, as->AZI1t.MM, as->AZI1t.SS  ) ;
  sprintf( c_hhmmss_azi2, "%dh%dm%ds", as->AZI2t.HH, as->AZI2t.MM, as->AZI2t.SS  ) ;

  Trace(" %s : infos         : %s", c_nom , as->infos ) ;
  Trace(" %s : type          : %s", c_nom , c_type ) ;
  Trace(" %s : mode calcul   : %s", c_nom , c_mode ) ;
  Trace(" %s : vitesses      : %.2f (Va) %.2f (Vh)", c_nom, as->Va,  as->Vh ) ; 
  Trace(" %s : azimut        : %.2f (deg) ", c_nom, as->a    * DEGRES ) ;
  Trace(" %s : altitude      : %.2f (deg) ", c_nom, as->h    * DEGRES ) ;
  Trace(" %s : declinaison   : %.2f (deg) ", c_nom, as->DEC  * DEGRES  ) ;
  Trace(" %s : ascension dro : %.2f (deg) %s (HH.MM.SS)", c_nom, as->ASC    * DEGRES, c_hhmmss_asc ) ;
  Trace(" %s : angle horaire : %.2f (deg) %s (HH.MM.SS)", c_nom, as->AGH   * DEGRES, c_hhmmss_agh ) ;

  Trace1(" %s : Agh0          : %.2f (deg) %s (HH.MM.SS)", c_nom, as->AGH0  * DEGRES, c_hhmmss_agh0 ) ;
  Trace1(" %s : Agh1          : %.2f (deg) %s (HH.MM.SS)", c_nom, as->AGH1  * DEGRES, c_hhmmss_agh1 ) ;
  Trace1(" %s : Agh2          : %.2f (deg) %s (HH.MM.SS)", c_nom, as->AGH2  * DEGRES, c_hhmmss_agh2 ) ;

  Trace1(" %s : Azi0          : %.2f (deg) %s (HH.MM.SS)", c_nom, as->AZI0  * DEGRES, c_hhmmss_azi0 ) ;
  Trace1(" %s : Azi1          : %.2f (deg) %s (HH.MM.SS)", c_nom, as->AZI1  * DEGRES, c_hhmmss_azi1 ) ;
  Trace1(" %s : Azi2          : %.2f (deg) %s (HH.MM.SS)", c_nom, as->AZI2  * DEGRES, c_hhmmss_azi2 ) ;

  Trace("----------------------------") ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_VOUTE
* @author : s.gravois
* @brief  : Cette fonction affiche les infos liees a la voute VOUTE *
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : a completer eventuellement
*****************************************************************************************/

void CONFIG_AFFICHER_VOUTE( VOUTE * voute) {
	
	Trace1("voute->num %lld", voute->num) ;
  Trace1("voute->pas %lld", voute->num) ;

  Trace1("----------------------------") ;

}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_TOUT
* @author : s.gravois
* @brief  : Cette fonction appelle toutes les autres fonctions d'affichage
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_AFFICHER_TOUT(void) {


  CONFIG_AFFICHER_TEMPS(   temps ) ;
  CONFIG_AFFICHER_LIEU(    lieu );
  CONFIG_AFFICHER_MODE_LONG(   as ) ;
  CONFIG_AFFICHER_VOUTE(   voute ) ;

  Trace("\n") ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_CHANGEMENTS
* @author : s.gravois
* @brief  : Cette fonction detecte et affiche les changements 
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : a consolider et voir utilite
*****************************************************************************************/

void CONFIG_AFFICHER_CHANGEMENTS (void)  {

  char s_menu[256] ;

  memset(s_menu, 0, sizeof(s_menu)) ;

  switch(suivi->menu) {
     
    case MENU_AZIMUTAL             :strcpy(s_menu,"MENU_AZIMUTAL") ; break ; 
    case MENU_EQUATORIAL           :strcpy(s_menu,"MENU_EQUATORIAL") ; break ; 
    case MENU_MANUEL_BRUT          :strcpy(s_menu,"MENU_MANUEL_BRUT") ; break ; 
    case MENU_MANUEL_NON_ASSERVI   :strcpy(s_menu,"MENU_MANUEL_NON_ASSERVI") ; break ; 
    case MENU_MANUEL_ASSERVI       :strcpy(s_menu,"MENU_MANUEL_ASSERVI") ; break ; 
    case MENU_GOTO                 :strcpy(s_menu,"MENU_GOTO") ; break ; 
    case MENU_INFO                 :strcpy(s_menu,"MENU_INFO") ; break ; 
    case MENU_RESEAU_UP            :strcpy(s_menu,"MENU_RESEAU_UP") ; break ; 
    case MENU_RESEAU_DOWN          :strcpy(s_menu,"MENU_RESEAU_DOWN") ; break ; 
    case MENU_PROGRAMME_DOWN       :strcpy(s_menu,"MENU_PROGRAMME_DOWN") ; break ; 
    case MENU_DOWN                 :strcpy(s_menu,"MENU_DOWN") ; break ; 
  }

  if ( suivi->menu_old != suivi->menu ) {

    Trace("appel : %d : %s" , suivi->menu, s_menu) ;
    GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 100) ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_DEVICES_USE
* @author : s.gravois
* @brief  : Cette fonction affiche le parametrage utilisation des devices 
* @param  : void
* @date   : 2022-03-12 creation 
*****************************************************************************************/

void CONFIG_AFFICHER_DEVICES_USE (void) {

  Trace("devices->DEVICE_USE_INFRAROUGE = %d",devices->DEVICE_USE_INFRAROUGE) ;
  Trace("devices->DEVICE_USE_CAPTEURS   = %d",devices->DEVICE_USE_CAPTEURS) ;
  Trace("devices->DEVICE_USE_RAQUETTE   = %d",devices->DEVICE_USE_RAQUETTE) ;
  Trace("devices->DEVICE_USE_BLUETOOTH  = %d",devices->DEVICE_USE_BLUETOOTH) ;
  Trace("devices->DEVICE_USE_KEYBOARD   = %d",devices->DEVICE_USE_KEYBOARD) ;
  Trace("devices->DEVICE_USE_CONTROLER  = %d",devices->DEVICE_USE_CONTROLER) ;
  Trace("devices->DEVICE_USE_LCD        = %d",devices->DEVICE_USE_LCD) ;

  return ;
}
/*****************************************************************************************
* @fn     : CONFIG_LCD_EMPILER
* @author : s.gravois
* @brief  : Cette fonction decale les buffers d'affichage telle une PILE vers le bas
* @brief  : les valeurs courantes a afficher sont lcd->c_line*
* @param  : void
* @date   : 2022-04-11 creation 
*****************************************************************************************/

void CONFIG_LCD_EMPILER(int i_duree_us,  char* c_line_0, char * c_line_1) {

  if ( devices->DEVICE_USE_LCD ) {

    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    memset( gp_Lcd->c_line_0_old, 0, sizeof( gp_Lcd->c_line_0_old )) ;
    memset( gp_Lcd->c_line_1_old, 0, sizeof( gp_Lcd->c_line_1_old )) ;

    strcpy( gp_Lcd->c_line_0_old, gp_Lcd->c_line_0 ) ;
    strcpy( gp_Lcd->c_line_1_old, gp_Lcd->c_line_1 ) ;

    memset( gp_Lcd->c_line_0, 0, sizeof( gp_Lcd->c_line_0 )) ;
    memset( gp_Lcd->c_line_1, 0, sizeof( gp_Lcd->c_line_1 )) ;

    strcpy( gp_Lcd->c_line_0, c_line_0 ) ;
    strcpy( gp_Lcd->c_line_1, c_line_1 ) ;

    gp_Lcd->i_change = TRUE ; 
    gp_Lcd->i_duree_us = i_duree_us ;

    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : CONFIG_LCD_DEPILER
* @author : s.gravois
* @brief  : Cette fonction decale les buffers d'affichage telle une PILE vers le haut
* @brief  : les valeurs courantes a afficher sont lcd->c_line*
* @param  : void
* @date   : 2022-04-11 creation 
* @date   : 2022-05-02 modification en utilisation pour pointeur de fonction
*****************************************************************************************/

void CONFIG_LCD_DEPILER(void) {

  if ( devices->DEVICE_USE_LCD ) {

    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    memset( gp_Lcd->c_line_0, 0, sizeof( gp_Lcd->c_line_0 )) ;
    memset( gp_Lcd->c_line_1, 0, sizeof( gp_Lcd->c_line_1 )) ;

    /* OLD -> NEW ; au final OLD OLD */
    strcpy( gp_Lcd->c_line_0, gp_Lcd->c_line_0_old ) ;
    strcpy( gp_Lcd->c_line_1, gp_Lcd->c_line_1_old ) ;

    gp_Lcd->i_change = FALSE ;

    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
  }
  return ;
}
/*****************************************************************************************
* @fn     : CONFIG_LCD_EMPILER
* @author : s.gravois
* @brief  : Cette fonction definit l 'affichage en continu (reattribut les lignes old et new)
* @brief  : les valeurs courantes a afficher sont lcd->c_line*
* @param  : void
* @date   : 2022-05-02 creation 
*****************************************************************************************/

void CONFIG_LCD_CONTINU(char* c_line_0, char * c_line_1) {

  if ( devices->DEVICE_USE_LCD ) {

    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    memset( gp_Lcd->c_line_0_old, 0, sizeof( gp_Lcd->c_line_0_old )) ;
    memset( gp_Lcd->c_line_1_old, 0, sizeof( gp_Lcd->c_line_1_old )) ;
    memset( gp_Lcd->c_line_0, 0,     sizeof( gp_Lcd->c_line_0 )) ;
    memset( gp_Lcd->c_line_1, 0,     sizeof( gp_Lcd->c_line_1 )) ;

    strcpy( gp_Lcd->c_line_0_old, c_line_0 ) ;
    strcpy( gp_Lcd->c_line_1_old, c_line_1 ) ;
    strcpy( gp_Lcd->c_line_0,     c_line_0 ) ;
    strcpy( gp_Lcd->c_line_1,     c_line_1 ) ;

    gp_Lcd->i_change = TRUE ; 
    gp_Lcd->i_duree_us = 1000000 ;

    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
  }
  return ;
}
/*****************************************************************************************
* @fn     : CONFIG_LCD_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les deux chaines de la struct LCD* sur ecran LCD  
* @brief  : fonction de BASE pour les autres fonctions
* @param  : LCD * lcd
* @param  : char* c_line_0
* @param  : char* c_line_1
* @date   : 2022-04-09 creation 
* @date   : 2022-04-28 suppression fonction LCD1602Init (fait dans CONFIG_LCD_INIT)
* @date   : 2022-04-28 protection par un mutex des donnees de lcd*
* @todo   : verifier la valeur de usleep (parametrer ?)
*****************************************************************************************/

void CONFIG_LCD_DISPLAY(LCD * lcd) {

  if ( devices->DEVICE_USE_LCD ) {

    pthread_mutex_lock( & lcd->mutex_lcd ) ;

    /*--------------------------------*/
    /* Clearing the LCD               */ 
    /*--------------------------------*/

    if ( LCD1602Clear(lcd->i_fd) == -1) {
      SyslogErr("Failed to LCD1602Clear\n");
      Trace("Failed to LCD1602Clear");
      pthread_mutex_unlock( & lcd->mutex_lcd ) ;
      return ;
    }
    else {
      Trace("LCD1602Clear(ok)");
    }
    pthread_mutex_unlock( & lcd->mutex_lcd ) ;
    /* entre 2500 et 5000 semble une bonne valeur de usleep */
    /* si on ne fait pas de usleep , l ecran ne se clear pas completement (teste) */

    usleep( CONFIG_LCD_USLEEP_AFTER_CLEARING ) ;
    
    pthread_mutex_lock( & lcd->mutex_lcd ) ;

    /*--------------------------------*/
    /* Displaying the lines on LCD    */ 
    /*--------------------------------*/

    if ( LCD1602DispLines(\
        lcd->i_fd, \
        lcd->c_line_0, \
        lcd->c_line_1 ) \
    == -1) { 

      SyslogErr("Failed to Display String\n");
      Trace("Failed to Display String");
      pthread_mutex_unlock( & lcd->mutex_lcd ) ;
      return ;
    }
    else {
      Trace("LCD1602DispLines(ok)");
    }

    pthread_mutex_unlock( & lcd->mutex_lcd ) ;

  }
  return ;
}

/*****************************************************************************************
* @fn     : CONFIG_LCD_PT_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les deux chaines de la struct LCD* sur ecran LCD  
* @brief  : fonction de BASE pour les autres fonctions
* @param  : LCD * lcd
* @param  : char* c_line_0
* @param  : char* c_line_1
* @date   : 2022-04-09 creation 
* @date   : 2022-04-28 suppression fonction LCD1602Init (fait dans CONFIG_LCD_INIT)
* @date   : 2022-04-28 protection par un mutex des donnees de lcd*
* @todo   : verifier la valeur de usleep (parametrer ?)
*****************************************************************************************/

void CONFIG_LCD_PT_DISPLAY(void) {

  if ( devices->DEVICE_USE_LCD ) {

    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    if ( LCD1602Clear(gp_Lcd->i_fd) == -1) {
      SyslogErr("Failed to LCD1602Clear\n");
      Trace("Failed to LCD1602Clear");

      pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;

      return ;
    }
    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
    /* entre 2500 et 5000 semble une bonne valeur de usleep */
    /* si on ne fait pas de usleep , l ecran ne se clear pas completement (teste) */
    usleep( CONFIG_LCD_USLEEP_AFTER_CLEARING ) ;
    
    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    if ( LCD1602DispLines(\
        gp_Lcd->i_fd, \
        gp_Lcd->c_line_0, \
        gp_Lcd->c_line_1 ) \
    == -1) { 

      SyslogErr("Failed to Display String\n");
      Trace("Failed to Display String");
      
      pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;

      return ;
    }

    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : CONFIG_LCD_AFFICHER
* @author : s.gravois
* @brief  : Cette fonction affiche deux chaines sur ecran LCD  
* @param  : LCD * lcd
* @param  : int i_duree_us  => si 0  affichage definitif
*                        => si >0 affichage de la duree correspodante
* @param  : char* c_line_0
* @param  : char* c_line_1
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void CONFIG_LCD_AFFICHER(LCD * lcd, int i_duree_us, char * c_line_0, char * c_line_1) {

  lcd->empiler(i_duree_us,c_line_0,c_line_1);
  lcd->display() ;
/*
  if ( i_duree_us > 0 ) {
    usleep(i_duree_us  ) ;
    CONFIG_LCD_DEPILER(lcd) ;
    CONFIG_LCD_DISPLAY( lcd ) ;
  }
*/
  return ;
}
/*****************************************************************************************
* @fn     : CONFIG_LCD_AFFICHER_STRINGS
* @author : s.gravois
* @brief  : Cette fonction affiche deux chaines sur ecran LCD  
* @param  : LCD * lcd
* @param  : char* c_line_0
* @param  : char* c_line_1
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void CONFIG_LCD_AFFICHER_STRINGS(LCD * lcd, int i_duree_us , char* c_line_0, char * c_line_1) {

  CONFIG_LCD_AFFICHER(lcd, i_duree_us, c_line_0, c_line_1) ;

  return ;
}
/*****************************************************************************************
* @fn     : CONFIG_LCD_AFFICHER_STRINGS
* @author : s.gravois
* @brief  : Cette fonction affiche une chaine et un entier sur ecran LCD  
* @param  : LCD * lcd
* @param  : char* c_line_0
* @param  : char* c_line_1
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void   CONFIG_LCD_AFFICHER_STRING_INT      ( LCD *lcd, int i_duree_us, char* c_line_0, int i) {
  char c_line_1[16] = {0} ;
  memset(c_line_1,0,sizeof(c_line_1));
  sprintf( c_line_1, "%d", i) ;

  CONFIG_LCD_AFFICHER(lcd, i_duree_us, c_line_0, c_line_1) ;
}
/*****************************************************************************************
* @fn     : CONFIG_LCD_AFFICHER_TEMPS_LIEU
* @author : s.gravois
* @brief  : Cette fonction calcule le temps sideral en cours puis affiche sur LCD
* @brief  : la date et l heure ainsi que la latitude et la longitude
* @param  : LCD * lcd
* @param  : char* c_line_0
* @param  : char* c_line_1
* @date   : 2022-04-09 creation 
* @date   : 2022-04-27 corrrection longueur de c_line_0
*****************************************************************************************/

void CONFIG_LCD_AFFICHER_TEMPS_LIEU( LCD *lcd, int i_duree_us, LIEU* lieu, TEMPS *temps) {

  char c_line_0[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;
  char c_line_1[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;

  memset( c_line_0, 0, sizeof(c_line_0) ) ; 
  memset( c_line_1, 0, sizeof(c_line_1) ) ; 

  CALCUL_TEMPS_SIDERAL(lieu, temps) ;

  /* Remplissage de line 0 et line 1 */

  sprintf( c_line_0, "%d%s%d %d:%d", \
    temps->yy ,\
    c_Lcd_Display_Months[ temps->mm -1  ] , \
    temps->dd, \
    temps->HH, \
    temps->MM ) ;
  
  sprintf( c_line_1, "%.2f %.2f", \
    lieu->lat * DEGRES, \
    lieu->lon * DEGRES ) ;

  CONFIG_LCD_AFFICHER( lcd, i_duree_us, c_line_0, c_line_1 ) ;

  return ;
}

/*****************************************************************************************
* @fn     : CONFIG_LCD_AFFICHER_ASTRE_VITESSES
* @author : s.gravois
* @brief  : Cette fonction affiche le nom de l 'astre et les vitesses
* @param  : LCD * lcd
* @param  : char* c_line_0
* @param  : char* c_line_1
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void CONFIG_LCD_AFFICHER_ASTRE_VITESSES(LCD * lcd, int i_duree_us, ASTRE* as ) {

  char c_line_0[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;
  char c_line_1[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;

  memset( c_line_0, 0, sizeof(c_line_0)) ; 
  memset( c_line_1, 0, sizeof(c_line_1)) ;

  strcpy(  c_line_0, as->nom ) ;
  sprintf( c_line_1,"%.2f %.2f", as->Va, as->Vh);

  CONFIG_LCD_AFFICHER(lcd, i_duree_us, c_line_0, c_line_1) ;

  return ;
}

/*****************************************************************************************
* @fn     : CONFIG_LCD_AFFICHER_AZI_ALT
* @author : s.gravois
* @brief  : Cette fonction affiche les coordonnees azimutales en cours
* @param  : LCD * lcd
* @param  : int i_duree_us
* @param  : ASTRE* as
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void CONFIG_LCD_AFFICHER_AZI_ALT(LCD * lcd, int i_duree_us, ASTRE* as ) {

  char c_line_0[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;
  char c_line_1[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;

  memset( c_line_0, 0, sizeof(c_line_0)) ; 
  memset( c_line_1, 0, sizeof(c_line_1)) ;

  sprintf( c_line_0, "AZI %s", as->c_ddmm_azi ) ;
  sprintf( c_line_1, "ALT %s", as->c_ddmm_alt );

  CONFIG_LCD_AFFICHER(lcd, i_duree_us, c_line_0, c_line_1) ;

  return ;
}

/*****************************************************************************************
* @fn     : CONFIG_LCD_AFFICHER_AGH_DEC
* @author : s.gravois
* @brief  : Cette fonction affiche les coordonnees horaires en cours
* @param  : LCD * lcd
* @param  : int i_duree_us
* @param  : ASTRE* as
* @date   : 2022-04-28 creation 
*****************************************************************************************/

void CONFIG_LCD_AFFICHER_AGH_DEC(LCD * lcd, int i_duree_us, ASTRE* as ) {

  char c_line_0[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;
  char c_line_1[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;

  memset( c_line_0, 0, sizeof(c_line_0)) ; 
  memset( c_line_1, 0, sizeof(c_line_1)) ;

  sprintf( c_line_0, "AGH %s", as->c_hhmm_agh ) ;
  sprintf( c_line_1, "DEC %s", as->c_ddmm_dec ) ;

  CONFIG_LCD_AFFICHER(lcd, i_duree_us, c_line_0, c_line_1) ;

  return ;
}

/*****************************************************************************************
* @fn     : CONFIG_LCD_AFFICHER_ASC_DEC
* @author : s.gravois
* @brief  : Cette fonction affiche les coordonnees horaires en cours
* @param  : LCD * lcd
* @param  : int i_duree_us
* @param  : ASTRE* as
* @date   : 2022-04-28 creation 
*****************************************************************************************/

void CONFIG_LCD_AFFICHER_ASC_DEC(LCD * lcd, int i_duree_us, ASTRE* as ) {

  char c_line_0[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;
  char c_line_1[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;

  memset( c_line_0, 0, sizeof(c_line_0)) ; 
  memset( c_line_1, 0, sizeof(c_line_1)) ;

  sprintf( c_line_0, "ASC %s", as->c_hhmm_asc ) ;
  sprintf( c_line_1, "DEC %s", as->c_ddmm_dec ) ;

  CONFIG_LCD_AFFICHER(lcd, i_duree_us, c_line_0, c_line_1) ;

  return ;
}

/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_LCD_MODE_STELLARIUM
* @author : s.gravois
* @brief  : Cette fonction s inspire de CONFIG_AFFICHER_MODE_STELLARIUM pour le LCD
* @param  : 
* @date   : 2022-03-18 creation
* @date   : 2022-03-28 simplification
*****************************************************************************************/

void CONFIG_AFFICHER_LCD_MODE_STELLARIUM(LCD * lcd, int i_duree_us, ASTRE *as) {

  char c_line_0[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;
  char c_line_1[ CONFIG_LCD_LINES_CHAR_NUMBERS + CONFIG_LCD_LINES_CHAR_NUMBERS_secure ]  ;

  memset( c_line_0, 0, sizeof(c_line_0)) ; 
  memset( c_line_1, 0, sizeof(c_line_1)) ;

  sprintf( c_line_0, "(AZI) %s", as->c_ddmm_azi ) ;
  sprintf( c_line_1, "(ALT) %s", as->c_ddmm_alt );

  CONFIG_LCD_AFFICHER(lcd, i_duree_us, c_line_0, c_line_1) ;
  /*
  Trace("Va / Vh    : %3.2f / %3.2f" , as->Va           , as->Vh ) ;
  Trace("AD / Dec   : %s / %s"       , as->c_hhmmss_asc , as->c_ddmm_dec ) ;
  Trace("AH / Dec   : %s / %s"       , as->c_hhmmss_agh , as->c_ddmm_dec ) ;
  Trace("AZ./ Haut. : %s / %s"       , as->c_ddmm_azi   , as->c_ddmm_alt ) ;
  */
}

/*****************************************************************************************
* @fn     : CONFIG_LCD_AFFICHER_INFORMATIONS
* @author : s.gravois
* @brief  : Cette fonction affiche les informations qu on souhaite
* @param  : LCD * lcd
* @param  : int i_duree_us
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void   CONFIG_LCD_AFFICHER_INFORMATIONS    ( LCD * lcd, int i_duree_us) {

  return ;
}

/*****************************************************************************************
* @fn     : CONFIG_PATH_FIND
* @author : s.gravois
* @brief  : Cette fonction affiche les informations qu on souhaite
* @param  : char * g_paths
* @param  : char * cmd
* @date   : 2022-04-09 creation 
*****************************************************************************************/

int   CONFIG_PATH_FIND (char * g_paths, char *cmd) {

  int i=-1;
  int i_retour = FALSE ;
  char c_path[ CONFIG_TAILLE_BUFFER_32 ] ;

  while( ++i < CONFIG_C_BIN_POSSIBLE_PATHS_LENGTH && i_retour == FALSE ) {
    sprintf( c_path, "%s/%s", c_Bin_Possible_Paths[i], cmd) ;
    Trace("Test chemin %s", c_path) ;
    if( access( c_path, F_OK ) == 0 ) { 
      i_retour = TRUE ; 
      strcpy( g_paths, c_path ) ;
    }
  }
  return i_retour ; 
}
//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================



