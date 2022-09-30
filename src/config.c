#define _GNU_SOURCE

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
#                * fonctions LCD_xxxx
#                * ajout mutex init sur mutex_datas_infrarouge
#                * => protection zone de code avec dat_inf
#                * ajout fonction LCD_DISPLAY_AGH_DEC / LCD_DISPLAY_EQU_DEC
#                * CONFIG_FORMATE_DONNEES_AFFICHAGE : ajout de 2 resolutions supplementaires
#                * protection zones de code impliquant LCD* (mutex_lcd)
# mai  2022      * reprise intégralité code pour utilisation pointeur fct (gp_Lcd->display_xxx)
# juin 2022      * deport du code concernant LCD dans lcd.c
#                * ajout prise en charge fichier CONFIG_FIC_LED (param config.txt)
#                  pour lecture numero led gpio IR , ce paramatre est defini 
#                  dans /boot/config.txt
#                * ajout PID_ECH PID_KP PID_KI PID_KD
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
* @brief  : Cette fonction met a jour les valeurs de gp_Sui->pas* en fonction
* @brief  : du contenu de gp_Sui->sui_dat.dat_inf
* @param  : SUIVI * gp_Sui
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
*****************************************************************************************/

void CONFIG_MAJ_SUIVI_PAS( SUIVI * gp_Sui) {

  char c_act = '0' ;

  // Arbo("0") ;

  if ( gp_Dev->DEVICE_USE_INFRAROUGE ) {
    
    pthread_mutex_lock(& gp_Pth->pth_mut.mut_dat );

    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "plus" ) )         { c_act='1'; gp_Sui->pas_acc_plus  = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "moins" ) )        { c_act='1'; gp_Sui->pas_acc_moins = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "forward" ) )      { c_act='1'; gp_Sui->pas_forward  = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "rewind" ) )       { c_act='1'; gp_Sui->pas_rewind = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "forwardfast" ) )  { c_act='1'; gp_Sui->pas_forward_fast  = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "rewindfast" ) )   { c_act='1'; gp_Sui->pas_rewind_fast = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "ne" ) )           { c_act='1'; gp_Sui->pas_nord=1 ; gp_Sui->pas_est=1   ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "no" ) )           { c_act='1'; gp_Sui->pas_nord=1 ; gp_Sui->pas_ouest=1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "se" ) )           { c_act='1'; gp_Sui->pas_sud=1  ; gp_Sui->pas_est=1   ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "so" ) )           { c_act='1'; gp_Sui->pas_sud=1  ; gp_Sui->pas_ouest=1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "n" ) )            { c_act='1'; gp_Sui->pas_nord  = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "o" ) )            { c_act='1'; gp_Sui->pas_ouest = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "e" ) )            { c_act='1'; gp_Sui->pas_est   = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "s" ) )            { c_act='1'; gp_Sui->pas_sud   = 1 ; }
    if ( ! strcmp( gp_Sui->sui_dat.dat_inf, "reset" ) )        { c_act='1'; gp_Sui->reset   = 1 ; }

    /* Si gp_Sui->sui_dat.dat_inf a ete utilise, il peut etre remis a zero */

    if ( c_act == '1' ) {
      Trace("raz de gp_Sui->sui_dat.dat_inf") ;
      memset( gp_Sui->sui_dat.dat_inf, 0, strlen( gp_Sui->sui_dat.dat_inf ) ) ;
      strcpy( gp_Sui->sui_dat.dat_inf, "") ;
    }
    pthread_mutex_unlock(& gp_Pth->pth_mut.mut_dat );

    TRACE2("%ld %ld %ld %ld %d %d\n", \
      gp_Sui->pas_ouest, \
      gp_Sui->pas_est, \
      gp_Sui->pas_nord, \
      gp_Sui->pas_sud, \
      gp_Sui->pas_acc_plus, \
      gp_Sui->pas_acc_moins );
  }

}
/*****************************************************************************************
* @fn     : CONFIG_INPUTS_GESTION_APPUIS
* @author : s.gravois
* @brief  : Cette fonction gere les appuis sur les "touches" (clavier, ir, ..)
* @brief  : et en deduit le contenu de la structure clavier (mot, action, etc..)
* @param  : SUIVI * gp_Sui
* @param  : CLAVIER *gp_Cla
* @date   : 2022-03-22 creation entete de la fonction au format doxygen
* @date   : 2022-03-22 renommage (ancien IR_xxx) et deplacment dans config.c /.h
* @date   : 2022-04-12 protection zone de code dat_inf
*****************************************************************************************/

void CONFIG_INPUTS_GESTION_APPUIS(SUIVI * gp_Sui, CLAVIER *gp_Cla) {

  int  i = 0 ;
  char val[255]={0} ;
  char s_buffer[CONFIG_TAILLE_BUFFER_32]={0} ;

  memset(val, 0, sizeof(val)) ;     

  pthread_mutex_lock(& gp_Pth->pth_mut.mut_dat );
  strcpy( val, gp_Sui->sui_dat.dat_inf ) ;
  pthread_mutex_unlock(& gp_Pth->pth_mut.mut_dat );

  Trace("val = %s\n", val ) ;
  
  if ( strcmp( val, "") ) {
    
    //printf("val = %s\n", val ) ;
    strcpy( gp_Cla->mot, val ) ; 
    gp_Cla->appui_en_cours = 1 ;
    gp_Cla->mot_en_cours = 1 ;    
  }
  else gp_Cla->appui_en_cours = 0 ; 
  
  // =======================================================================
  // Quand la touche est relacheee, on traite
  // ensuite appui en cours est remis a ZERO pour qu'on ne passe qu'une fois
  // dans cette partie de code
  // =======================================================================
  
  if ( gp_Cla->mot_en_cours && gp_Cla->appui_en_cours == 0 ) {
  
    Trace1("mot trouver = %s", gp_Cla->mot ) ;
		
		GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 5) ;

    //------------------------------------------------------------
    // On incremente la phrase avec le mot et
    // On incremente le nombre avec le mot si premier n'est pas vide
    //------------------------------------------------------------
    
    if ( strcmp( gp_Cla->mot, gp_Cla->valider) != 0 ) { 
      
      Trace1("Si mot != valider : on incremente la phrase avec le mot !!\n" ) ;

      if ( strlen(gp_Cla->phrase) + strlen(gp_Cla->mot) < CONFIG_TAILLE_BUFFER_32) {

        Trace1("on incremente la phrase %s avec le mot %s\n",gp_Cla->phrase,  gp_Cla->mot ) ;
        // correction bug 28/12/2017 : recopie de la chaine dans un buffer avant le sprintf

        memset( s_buffer, IR_ZERO_CHAR, sizeof( s_buffer )) ;
        strcpy( s_buffer, gp_Cla->phrase ) ;

        sprintf(gp_Cla->phrase,"%s%s",s_buffer, gp_Cla->mot);
      }
      Trace1("resultat => phrase = %s \n",gp_Cla->phrase ) ;
      
      Trace1("Si mot != valider et premier non vide => on met nombre + mot dans le nombre !!\n" ) ;

      if ( strcmp( gp_Cla->premier, "")) {
        
	      if ( strlen(gp_Cla->nombre) + strlen(gp_Cla->mot) < CONFIG_TAILLE_BUFFER_32) {

          Trace1("on incremente le nombre %s avec le mot %s\n",gp_Cla->nombre,  gp_Cla->mot ) ;
          // correction bug 28/12/2017 : recopie de la chaine dans un buffer avant le sprintf

          memset( s_buffer, IR_ZERO_CHAR, sizeof( s_buffer )) ;
          strcpy( s_buffer, gp_Cla->nombre ) ;

          sprintf(gp_Cla->nombre,"%s%s", s_buffer, gp_Cla->mot);
        }
      }
      Trace1("resultat => nombre = %s \n",gp_Cla->nombre ) ;
    }    
    //------------------------------------------------------------
    // On met le mot dans premier si il est vide 
    //------------------------------------------------------------
    
    if ( ! strcmp( gp_Cla->premier, "")){ 
      if ( strlen(gp_Cla->mot) < CONFIG_TAILLE_BUFFER_32)
      strcpy( gp_Cla->premier, gp_Cla->mot);
    }
    //------------------------------------------------------------
    // Si le mot en cours est une VALIDATION 
    // Exemple : valider
    // Alors on VALIDE la phrase en cours
    //  - en mettant gp_Cla->phrase_lu à 1
    // on efface tout sauf SYMBOLE et NOMBRE qui sont determiner plus tot
    //------------------------------------------------------------

    for( i=0 ; i < CONFIG_VALIDATIONS_SIZE ; i++ )
    if ( ! strcmp( gp_Cla->mot,    gp_Cla->valider )  ) {
      Trace1("Appui sur valider => on met premier dans symbole, phrase dans nombre, et NULL dans phrase et mot, phrase_lue a 1" ) ; 

      strcpy(gp_Cla->premier,"") ;
      strcpy(gp_Cla->phrase,"")  ;
      strcpy(gp_Cla->mot,"") ;
      gp_Cla->phrase_lue=1 ;
    }
    //------------------------------------------------------------
    // Si le mot est une ACTION, on efface la phrase en cours    
    // on met le mot dans PREMIER (premier mot de la phrase) 
    // on met le mot dans SYMBOLE (symbole utilise dans prog main) 
    //------------------------------------------------------------
    
    for( i=0 ; i < CONFIG_ACTIONS_SIZE ; i++ )
    if ( ! strcmp( gp_Cla->mot, gp_Cla->actions[i] )) {
        Trace1("Si le mot est une ACTION, alors on efface la phrase en cours et on met mot dans premier et symbole") ;
        strcpy(gp_Cla->premier,gp_Cla->mot) ;
        strcpy(gp_Cla->symbole,gp_Cla->mot)  ;

        strcpy(gp_Cla->nombre,"")  ;
        strcpy(gp_Cla->phrase,"")  ;
        strcpy(gp_Cla->mot,"") ;
        gp_Cla->phrase_lue=0 ;
    }
    
    gp_Cla->mot_en_cours = 0 ;
    gp_Cla->appui_en_cours = 0 ;
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
  
  Trace("") ;
  
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
* @fn     : CONFIG_INIT_ASTRE
* @author : s.gravois
* @brief  : Cette fonction initialise la structure ASTRE * gp_Ast
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-20 ajout membres de la structure dans la fonction
* @todo   : supprimer ASTRE *gp_Ast (var blog) et remplacer par void
*           passer par des pointeurs de fonctions dans un structure
*****************************************************************************************/

void CONFIG_INIT_ASTRE(ASTRE *gp_Ast) {

  int C ;
  
  for(C=0; C< ASTRE_NB_COLONNES;C++) {
    memset( gp_Ast->plus_proche[C], ZERO_CHAR, ASTRE_TAILLE_BUFFER);
    strcpy( gp_Ast->plus_proche[C], "") ;
  }
  memset( gp_Ast->nom,   ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  memset( gp_Ast->infos, ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  memset( gp_Ast->plus_proche, ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  
  gp_Ast->a   = 0  ;
  gp_Ast->h   = 0  ;
  gp_Ast->a0  = 0 ;
  gp_Ast->h0  = 0 ;
  gp_Ast->AGH   = 0  ; 
  gp_Ast->ASC   = 0  ;
  gp_Ast->DEC   = 0   ;
  gp_Ast->A0  = 0 ;
  gp_Ast->H0  = 0 ;
  gp_Ast->da  = 0 ;
  gp_Ast->dh  = 0 ;
  gp_Ast->dA  = 0 ;
  gp_Ast->dH  = 0 ;
  gp_Ast->Va  = 0 ;
  gp_Ast->Vh  = 0 ;
  gp_Ast->dVa = 0 ;
  gp_Ast->dVh = 0 ;
  gp_Ast->dVam= 0 ;
  gp_Ast->dVhm= 0 ;

  gp_Ast->x = 0 ;
  gp_Ast->xx = 0 ;
  gp_Ast->y  = 0 ;
  gp_Ast->yy =0;
  gp_Ast->z =0;
  gp_Ast->zz =0;
  
  gp_Ast->type = ASTRE_INDETERMINE ;
  gp_Ast->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  gp_Ast->numero = 0 ;
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_CLAVIER
* @author : s.gravois
* @brief  : Cette fonction initialise la structure CLAVIER * as
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-20 ajout membres de la structure dans la fonction
* @todo   : supprimer ASTRE *gp_Ast (var blog) et remplacer par void
*           passer par des pointeurs de fonctions dans un structure
*****************************************************************************************/

void CONFIG_INIT_CLAVIER(CLAVIER * gp_Cla) {

  int i ;
  
  Trace1("") ;

  memset( gp_Cla->phrase,  ZERO_CHAR, sizeof( gp_Cla->phrase ) );
  memset( gp_Cla->valider, ZERO_CHAR, sizeof( gp_Cla->valider ) );
  memset( gp_Cla->symbole, ZERO_CHAR, sizeof( gp_Cla->symbole ) );
  memset( gp_Cla->menu,    ZERO_CHAR, sizeof( gp_Cla->menu ) );
  memset( gp_Cla->premier, ZERO_CHAR, sizeof( gp_Cla->premier ) );
  memset( gp_Cla->nombre,  ZERO_CHAR, sizeof( gp_Cla->nombre ) );
  memset( gp_Cla->mot,     ZERO_CHAR, sizeof( gp_Cla->mot ) );
  
  strcpy( gp_Cla->valider, "valider" ) ;
  strcpy( gp_Cla->menu,    "MENU" ) ;
  
  strcpy( gp_Cla->symbole,  "" ) ;
  strcpy( gp_Cla->phrase,   "" ) ;
  strcpy( gp_Cla->premier,  "" ) ;
  strcpy( gp_Cla->nombre,   "" ) ;
  strcpy( gp_Cla->mot,      "" ) ;

  gp_Cla->mot_en_cours   = 0 ;
  gp_Cla->phrase_lue     = 0 ;
  gp_Cla->appui_en_cours = 0 ;
  gp_Cla->temporisation_clavier = TEMPO_CLAVIER ;
 
  // FIXME : definitions des actions : 
  // Les actions servent a 
  
  for( i=0 ; i < CONFIG_ACTIONS_SIZE ; i++ ) {
    memset( gp_Cla->actions[i], ZERO_CHAR, CONFIG_TAILLE_BUFFER_32);
  }
  // for( i=0 ; i < CONFIG_VALIDATIONS_SIZE ; i++ )  memset( gp_Cla->validations[i], ZERO_CHAR, CONFIG_TAILLE_BUFFER_32);

  strcpy( gp_Cla->actions[0], "MENU" ) ;
  strcpy( gp_Cla->actions[1], "SETUP" ) ;
  strcpy( gp_Cla->actions[2], "MES" ) ;
  strcpy( gp_Cla->actions[3], "NGC" ) ;
  strcpy( gp_Cla->actions[4], "ETO" ) ;
  strcpy( gp_Cla->actions[5], "PLA" ) ;
  strcpy( gp_Cla->actions[6], "TIME" ) ;
   
} 
/*****************************************************************************************
* @fn     : CONFIG_INIT_LIEU
* @author : s.gravois
* @brief  : Cette fonction initialise la structure LIEU *gp_Lie
* @param  : LIEU *gp_Lie
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : cf si JJ etc.. sont utiles dans la structure (parametres de temps)
*****************************************************************************************/

void CONFIG_INIT_LIEU(LIEU *gp_Lie) {
 
  gp_Lie->JJ  = 0 ; // jour julien
  gp_Lie->TS  = 0 ;  // temps sideral
  gp_Lie->TSR = 0 ;  // temps sideral en radians
  gp_Lie->JD  = 0 ;  // jour decimal
  gp_Lie->lat = LATITUDE / DEGRES ;
  gp_Lie->lon = LONGITUDE / DEGRES ;
  gp_Lie->alt = ALTITUDE ;
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
* @brief  : Cette fonction initialise la structure TEMPS * gp_Tim
* @param  : TEMPS * gp_Tim
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : configurer directement heure locale (?)
*****************************************************************************************/

void CONFIG_INIT_TEMPS( TEMPS * gp_Tim) {
  
  gp_Tim->c_si = '+' ; // signe 
  gp_Tim->mm = 0 ;  // month
  gp_Tim->yy = 0 ;  // year
  gp_Tim->dd = 0 ;  // day
  gp_Tim->HH = 0 ;  // hour
  gp_Tim->MM = 0 ;  // minutes
  gp_Tim->SS = 0 ;  // secondes
  gp_Tim->hd = 0.0 ;  // heure decimale (double)
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_VOUTE
* @author : s.gravois
* @brief  : Cette fonction initialise la structure VOUTE *gp_Vou
* @param  : VOUTE *gp_Vou
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           pourcentage_tempo et calibration_voute
*****************************************************************************************/

void CONFIG_INIT_VOUTE(VOUTE *gp_Vou) {
  
  gp_Vou->dt                = 0  ;
  gp_Vou->pourcentage_tempo = 0.96 ;   
  gp_Vou->calibration_voute = 0.97 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
	gp_Vou->num               = 0 ;
  gp_Vou->deb               = 0 ;
  gp_Vou->fin               = 0 ;
  gp_Vou->acc_old           = 1 ;
  gp_Vou->acc               = 1 ;
  gp_Vou->pas               = gp_Vou->dt * ROT_RAD_SEC ;
  /* dt en micro-sec */

  gp_Vou->DT = (unsigned long)( gp_Vou->dt * CONFIG_MICRO_SEC / gp_Vou->acc ) ;   
}

/*****************************************************************************************
* @fn     : CONFIG_INIT_CODE
* @author : s.gravois
* @brief  : Cette fonction initialise un code (appelle par CONFIG_INIT_CODES)
* @param  : VOUTE *gp_Vou
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           pourcentage_tempo et calibration_voute
*****************************************************************************************/

void CONFIG_INIT_CODE( \
 CODES * gp_Cod, \
 int          li_pos, \
 const char * gl_char_Codes[][CONFIG_COD_NB_IN_OUT] ) {

  Trace1("li_pos %d %-16s %-16s %-16s", \
    li_pos, \
    gl_char_Codes[li_pos][ CODES_POS_IN_TERM ], \
    gl_char_Codes[li_pos][ CODES_POS_IN_LIRC], \
    gl_char_Codes[li_pos][ CODES_POS_OUT_ACT] \
  );

  strcpy( gp_Cod->in_term[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_IN_TERM ] ) ;
  strcpy( gp_Cod->in_lirc[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_IN_LIRC] ) ;
  strcpy( gp_Cod->out_act[ li_pos ], gl_char_Codes[li_pos][ CODES_POS_OUT_ACT] ) ;
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_CODES
* @author : s.gravois
* @brief  : Cette fonction initialise la structure des codes *CODES
* @brief  : etablie la correspondance entre les KEY du fichier lircd.conf (partie LIRC)
* @brief  : et les codes utilises dans le programme principal (car pas de hachage en c)
* @brief  : etablie aussi la correspondance entre les KEY du clavier termios (partie TERMIOS)
* @brief  : et les codes utilises dans le programme principal (code_action)
* @param  : CODES *gp_Cod
* @date   : 2022-03-21 creation entete
* @todo   : modifier : completer avec une fonction de hachage (regarder si API sur net)
*****************************************************************************************/

void CONFIG_INIT_CODES(CODES *gp_Cod) {

  int i_pos ;
  
  for( i_pos=0 ; i_pos<CONFIG_CODE_NB_CODES ; i_pos++ ) memset( gp_Cod->out_act[i_pos], IR_ZERO_CHAR, sizeof(gp_Cod->out_act[i_pos]) ) ;
  for( i_pos=0 ; i_pos<CONFIG_CODE_NB_CODES ; i_pos++ ) memset( gp_Cod->in_lirc[i_pos],  IR_ZERO_CHAR, sizeof(gp_Cod->in_lirc[i_pos]) ) ;

  for( i_pos=0 ; i_pos<CONFIG_CODE_NB_CODES ; i_pos++ ) {
    memset( gp_Cod->in_term[i_pos],  IR_ZERO_CHAR, sizeof(gp_Cod->in_term[i_pos]) ) ;
    memset( gp_Cod->in_lirc[i_pos],  IR_ZERO_CHAR, sizeof(gp_Cod->in_lirc[i_pos]) ) ;
    memset( gp_Cod->out_act[i_pos],  IR_ZERO_CHAR, sizeof(gp_Cod->out_act[i_pos]) ) ;
  }
  for( i_pos=0 ; i_pos<CONFIG_CODE_NB_CODES ; i_pos++ ) {

    CONFIG_INIT_CODE( gp_Cod, i_pos, g_char_Codes  ) ; 
  }
  // FIXME :  ATTENTION !!! 
  // le nom des gp_Cod codes en MAJUSCULE servent a definir des actions dans config.c et le reste du programme

  /* ancienne partie :  
  
  strcpy( gp_Cod->in_lirc[0], "KEY_0") ;           strcpy( gp_Cod->out_act[0], "0") ;
  strcpy( gp_Cod->in_lirc[1], "KEY_1") ;           strcpy( gp_Cod->out_act[1], "1") ;
  strcpy( gp_Cod->in_lirc[2], "KEY_2") ;           strcpy( gp_Cod->out_act[2], "2") ;
  strcpy( gp_Cod->in_lirc[3], "KEY_3") ;           strcpy( gp_Cod->out_act[3], "3") ;
  strcpy( gp_Cod->in_lirc[4], "KEY_4") ;           strcpy( gp_Cod->out_act[4], "4") ;
  strcpy( gp_Cod->in_lirc[5], "KEY_5") ;           strcpy( gp_Cod->out_act[5], "5") ;
  strcpy( gp_Cod->in_lirc[6], "KEY_6") ;           strcpy( gp_Cod->out_act[6], "6") ;
  strcpy( gp_Cod->in_lirc[7], "KEY_7") ;           strcpy( gp_Cod->out_act[7], "7") ;
  strcpy( gp_Cod->in_lirc[8], "KEY_8") ;           strcpy( gp_Cod->out_act[8], "8") ;
  strcpy( gp_Cod->in_lirc[9], "KEY_9") ;           strcpy( gp_Cod->out_act[9], "9") ;

  strcpy( gp_Cod->in_lirc[10],"KEY_M") ;           strcpy( gp_Cod->out_act[10],"MES") ;
  strcpy( gp_Cod->in_lirc[11],"KEY_N") ;           strcpy( gp_Cod->out_act[11],"NGC") ;
  strcpy( gp_Cod->in_lirc[12],"KEY_E") ;           strcpy( gp_Cod->out_act[12],"ETO") ;
  strcpy( gp_Cod->in_lirc[13],"KEY_P") ;           strcpy( gp_Cod->out_act[13],"PLA") ;

  strcpy( gp_Cod->in_lirc[14],"KEY_PREVIOUS");     strcpy( gp_Cod->out_act[14],"previous") ;
  strcpy( gp_Cod->in_lirc[15],"KEY_PLAY") ;        strcpy( gp_Cod->out_act[15],"valider") ;
  strcpy( gp_Cod->in_lirc[16],"KEY_NEXT") ;        strcpy( gp_Cod->out_act[16],"plus") ;
  strcpy( gp_Cod->in_lirc[17],"KEY_STOP") ;        strcpy( gp_Cod->out_act[17],"stop") ;
  strcpy( gp_Cod->in_lirc[18],"KEY_MENU") ;        strcpy( gp_Cod->out_act[18],"MENU") ;
  strcpy( gp_Cod->in_lirc[19],"KEY_PAUSE") ;       strcpy( gp_Cod->out_act[19],"pause") ;
  strcpy( gp_Cod->in_lirc[20],"KEY_OK") ;          strcpy( gp_Cod->out_act[20],"reset") ;
  strcpy( gp_Cod->in_lirc[21],"KEY_UP") ;          strcpy( gp_Cod->out_act[21],"n") ;
  strcpy( gp_Cod->in_lirc[22],"KEY_RIGHT") ;       strcpy( gp_Cod->out_act[22],"e") ;
  strcpy( gp_Cod->in_lirc[23],"KEY_DOWN") ;        strcpy( gp_Cod->out_act[23],"s") ;
  strcpy( gp_Cod->in_lirc[24],"KEY_LEFT") ;        strcpy( gp_Cod->out_act[24],"o") ;
  strcpy( gp_Cod->in_lirc[25],"KEY_SETUP") ;       strcpy( gp_Cod->out_act[25],"SETUP") ;
  // strcpy( gp_Cod->in_lirc[26],"KEY_TIME") ;        strcpy( gp_Cod->out_act[26],"TIME") ;    // ajout du 10/10/2016
  strcpy( gp_Cod->in_lirc[26],"KEY_SOUND") ;        strcpy( gp_Cod->out_act[26],"TIME") ;    // ajout du 10/10/2016

  strcpy( gp_Cod->in_lirc[27],"KEY_FORWARD") ;     strcpy( gp_Cod->out_act[27],"forward") ;
  strcpy( gp_Cod->in_lirc[28],"KEY_REWIND") ;      strcpy( gp_Cod->out_act[28],"rewind") ;    // ajout du 26/11/2017

  strcpy( gp_Cod->in_lirc[29],"KEY_RED") ;         strcpy( gp_Cod->out_act[29],"red") ;
  strcpy( gp_Cod->in_lirc[30],"KEY_BLUE") ;        strcpy( gp_Cod->out_act[30],"blue") ;
  strcpy( gp_Cod->in_lirc[31],"KEY_YELLOW") ;      strcpy( gp_Cod->out_act[31],"yellow") ;
  strcpy( gp_Cod->in_lirc[32],"KEY_GREEN") ;       strcpy( gp_Cod->out_act[32],"green") ;   // ajout ulterieurs.. pour etre idem que reel 25 et superieur ...

  strcpy( gp_Cod->in_lirc[33],"KEY_POWER") ;       strcpy( gp_Cod->out_act[33],"key_power") ;

  strcpy( gp_Cod->in_lirc[34],"KEY_CHANNELUP") ;   strcpy( gp_Cod->out_act[34],"forwardfast") ;
  strcpy( gp_Cod->in_lirc[35],"KEY_CHANNELDOWN") ; strcpy( gp_Cod->out_act[35],"rewindfast") ;
  strcpy( gp_Cod->in_lirc[36],"KEY_VOLUMEUP") ;    strcpy( gp_Cod->out_act[36],"forward") ;
  strcpy( gp_Cod->in_lirc[37],"KEY_VOLUMEDOWN") ;  strcpy( gp_Cod->out_act[37],"rewind") ;

  strcpy( gp_Cod->in_lirc[38],"KEY_MUTE") ;        strcpy( gp_Cod->out_act[38],"TIME") ;

  strcpy( gp_Cod->in_lirc[39],"KEY_SCREEN") ;      strcpy( gp_Cod->out_act[39],"aff_azi_alt") ; 
  strcpy( gp_Cod->in_lirc[40],"KEY_TV") ;          strcpy( gp_Cod->out_act[40],"aff_tps_lie") ;
  strcpy( gp_Cod->in_lirc[41],"KEY_INFO") ;        strcpy( gp_Cod->out_act[41],"aff_info") ;
  strcpy( gp_Cod->in_lirc[42],"KEY_ZOOM") ;        strcpy( gp_Cod->out_act[42],"key_zoom") ;
  strcpy( gp_Cod->in_lirc[43],"KEY_LIST") ;        strcpy( gp_Cod->out_act[43],"key_azi") ;
  strcpy( gp_Cod->in_lirc[44],"KEY_MODE") ;        strcpy( gp_Cod->out_act[44],"key_equ") ; 
  strcpy( gp_Cod->in_lirc[45],"KEY_EXIT") ;        strcpy( gp_Cod->out_act[45],"key_exit") ;
  
  // codes VOUTE de la telecommande - joue sur la vitesse globale

  */
}
/*****************************************************************************************
* @fn     : CONFIG_VOUTE
* @author : s.gravois
* @brief  : Cette fonction configure la structure VOUTE *gp_Vou
* @param  : VOUTE *gp_Vou
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           pourcentage_tempo et calibration_voute
*****************************************************************************************/

void CONFIG_VOUTE( VOUTE *gp_Vou, double dt, double acc, double percent ) {
  
  gp_Vou->dt                 = dt  ;
  gp_Vou->pourcentage_tempo  = percent ; 
  gp_Vou->calibration_voute  = 0.99 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
  gp_Vou->deb                = 0 ;
  gp_Vou->num                = 0 ;
  gp_Vou->fin                = PIPI ;
  gp_Vou->acc_old            = gp_Vou->acc ;
  gp_Vou->acc                = acc ;
  gp_Vou->pas                = gp_Vou->dt * ROT_RAD_SEC ;
  /* dt en micro-sec */
  gp_Vou->DT = (unsigned long)( gp_Vou->dt * CONFIG_MICRO_SEC / gp_Vou->acc ) ;
}

/*****************************************************************************************
* @fn     : CONFIG_INIT_DEVICES
* @author : s.gravois
* @brief  : Cette fonction configure la structure DEVICES *gp_Dev
* @param  : DEVICES *gp_Dev
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : (obsolete) les devices sont lues depuis le fichier de configuration
*****************************************************************************************/

void CONFIG_INIT_DEVICES(DEVICES *gp_Dev) {

  gp_Dev->DEVICE_USE_CAPTEURS    = DEVICE_USE_CAPTEURS ;
  gp_Dev->DEVICE_USE_RAQUETTE    = DEVICE_USE_RAQUETTE ;
  gp_Dev->DEVICE_USE_BLUETOOTH   = DEVICE_USE_BLUETOOTH ;
  gp_Dev->DEVICE_USE_INFRAROUGE  = DEVICE_USE_INFRAROUGE ;
  gp_Dev->DEVICE_USE_CONTROLER   = DEVICE_USE_CONTROLER ;
  gp_Dev->DEVICE_USE_KEYBOARD    = DEVICE_USE_KEYBOARD ;
  gp_Dev->DEVICE_USE_LCD         = DEVICE_USE_LCD ; 
  gp_Dev->init_capteurs = 0 ; 
}
/*****************************************************************************************
* @fn     : CONFIG_INIT_SUIVI
* @author : s.gravois
* @brief  : Cette fonction configure la structure SUIVI * gp_Sui
* @param  : SUIVI * gp_Sui
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-01-20 passage d une partie de code dans CONFIG_INIT_DEVICES
* @date   : 2022-05-24 ajout mutex_thread suivi pour proteger section crotique threads
* @date   : 2022-06-17 ajout PID_ECH PID_KP PID_KI PID_KD 
* @todo   : voir si les mutex peuvent etre dispatches / eclates dans les autres struct
*****************************************************************************************/

void CONFIG_INIT_SUIVI(SUIVI * gp_Sui) {
  
  int i ;
  
  gp_Sui->SUIVI_MANUEL     = 0 ;
  gp_Sui->SUIVI_ALIGNEMENT = 0 ;
  gp_Sui->SUIVI_GOTO       = 0 ;
  gp_Sui->SUIVI_VOUTE      = 1 ;
  gp_Sui->SUIVI_EQUATORIAL = 0 ;

  // a modifier  : instancier ces variables a l aide du fichier de config

  gp_Sui->reset = 0 ;
  
  gp_Sui->pas_azi        = 1 ;
  gp_Sui->pas_alt        = 1 ;

  gp_Sui->pas_acc_plus   = 0 ;
  gp_Sui->pas_acc_moins  = 0 ;
  gp_Sui->pas_azi_old    = 0 ;
  gp_Sui->pas_alt_old    = 0 ;

  gp_Sui->pas_est        = 0 ;
  gp_Sui->pas_ouest      = 0 ; 
  gp_Sui->pas_nord       = 0 ;
  gp_Sui->pas_sud        = 0 ;

  gp_Sui->pas_forward       = 0 ;
  gp_Sui->pas_rewind        = 0 ;
  gp_Sui->pas_forward_fast  = 0 ;
  gp_Sui->pas_rewind_fast   = 0 ;

  /* Initialisation des accelerations */

  gp_Sui->acc_azi       = 1.0 ; // acceleration volontaire des vitesses brutes
  gp_Sui->acc_alt       = 1.0 ; // acceleration volontaire des vitesses brutes
  gp_Sui->acc_azi_pid   = 1.0 ; // acceleration PID par retour boucle des vitesses brutes
  gp_Sui->acc_alt_pid   = 1.0 ; // acceleration PID par retour boucle des vitesses brutes

  gp_Sui->sgn_azi   = 1 ;
  gp_Sui->sgn_alt   = 1 ;

  gp_Sui->t_diff = 0 ;
  gp_Sui->t_diff_sec = 0 ;
  gp_Sui->d_temps          = 0 ;   
  gp_Sui->d_appui_raq_azi  = 0 ;
  gp_Sui->d_appui_raq_alt  = 0 ;
  
  gp_Sui->Fa_mic        = 30 ;
  gp_Sui->Fh_mic        = 30 ;

  gp_Sui->Ta_mic        = 1 / gp_Sui->Fa_mic ;
  gp_Sui->Th_mic        = 1 / gp_Sui->Fh_mic ;
  
  gp_Sui->Tac        = 1.0 ;
  gp_Sui->Thc        = 1.0 ;
  
  gp_Sui->Tacc = 0.97 ;
  gp_Sui->Thcc = 0.97 ;
  
  gp_Sui->Ia = 0 ;
  gp_Sui->Ih = 0 ;
  
  gp_Sui->Ias = 0 ;
  gp_Sui->Ihs = 0 ;
  
  gp_Sui->Ia_prec = 0 ;
  gp_Sui->Ih_prec = 0 ;
  
  for(i=0;i<CONFIG_ASS;i++) gp_Sui->Iat[i] = 0 ;
  for(i=0;i<CONFIG_ASS;i++) gp_Sui->Iht[i] = 0 ;
  
  //gp_Sui->plus      =  1.02 ;
  //gp_Sui->moins     =  1.0 / gp_Sui->plus ;   
  
  gp_Sui->l_NANO_MOINS     = 0  ;     // a retirer sur les temporisations pour les tests
   
  gp_Sui->pas_asc = 0 ;
  gp_Sui->pas_dec = 0  ;
    
  gp_Sui->Sa = 0 ;       // signe de la vitesse (direction), tenir compte du flag FLAG_SENS_ALT
  gp_Sui->Sh = 0 ;       // signe de la vitesse (direction), tenir compte du flag FLAG_SENS_AZI
  gp_Sui->Sa_old = 0 ;   // flag de comparaison pour raffraichir ou non les ecritures
  gp_Sui->Sh_old = 0 ;   // flag de comparaison pour raffraichir ou non les ecritures
  
  gp_Sui->Da = 0  ;      // nombre a injecter dans les diviseurs de frequence
  gp_Sui->Dh = 0 ;       // nombre a injecter dans les diviseurs de frequence
  
  gp_Sui->menu = MENU_PAR_DEFAUT  ;    // menu par defaut
  gp_Sui->menu_old = MENU_PAR_DEFAUT  ;

  gp_Sui->alarme = 0 ;
  
  gp_Sui->pourcentage_tempo = 0.99 ; 
  
  gp_Sui->temporisation_menu     = TEMPO_MENU ;
  gp_Sui->temporisation_raq      = TEMPO_RAQ ;
  gp_Sui->temporisation_ir       = TEMPO_IR ;  
  gp_Sui->temporisation_termios  = TEMPO_TERMIOS ;
  gp_Sui->temporisation_clavier  = TEMPO_CLAVIER ; 
  gp_Sui->temporisation_capteurs = TEMPO_CAPTEURS ;
  gp_Sui->temporisation_lcd_loop = TEMPO_LCD_LOOP ;
  gp_Sui->temporisation_lcd_disp = TEMPO_LCD_DISP ;

  gp_Sui->temporisation_pid_loop = PID_ECH ;
  gp_Sui->temporisation_voute    = gp_Vou->DT ;

  gp_Sui->DTh = gp_Sui->Th_mic * CONFIG_MICRO_SEC ;
  gp_Sui->DTa = gp_Sui->Ta_mic * CONFIG_MICRO_SEC ;

  gettimeofday(&gp_Sui->tval,NULL) ;
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
* @date   : juin 2022 : ajout parametres PID
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

  TEMPO_MENU     = 50000;
  TEMPO_RAQ      = 51000; /* est utilisee uniquement dans SUIVI_MANUEL_ASSERVI */
  TEMPO_IR       = 52000;
  TEMPO_TERMIOS  = 53000;
  TEMPO_CLAVIER  = 25000;
  TEMPO_CAPTEURS = 50000;
  TEMPO_LCD_LOOP = 250000 ; 
  TEMPO_LCD_DISP = 100000 ; 

  PID_ECH        = 3 ;

   //----------------------------------------------------------------------
   // Lecture des variables dans la config lue dans le fichier de config
   //-----------ASTRE_PAR_DEFAUT-------------------------------------------

  memset( ASTRE_PAR_DEFAUT, ZERO_CHAR, sizeof( ASTRE_PAR_DEFAUT ) ) ;

  memset( CONFIG_REP_CAT, ZERO_CHAR, sizeof( CONFIG_REP_CAT ) ) ;
  memset( CONFIG_REP_CFG, ZERO_CHAR, sizeof( CONFIG_REP_CFG ) ) ;
  memset( CONFIG_REP_LOG, ZERO_CHAR, sizeof( CONFIG_REP_LOG ) ) ;
  memset( CONFIG_REP_IN, ZERO_CHAR, sizeof( CONFIG_REP_IN ) ) ;
  memset( CONFIG_REP_SCR, ZERO_CHAR, sizeof( CONFIG_REP_SCR ) ) ;
  memset( CONFIG_FIC_LED, ZERO_CHAR, sizeof( CONFIG_FIC_LED ) ) ;
  memset( CONFIG_FIC_LOG, ZERO_CHAR, sizeof( CONFIG_FIC_LOG ) ) ;
  memset( CONFIG_FIC_PID, ZERO_CHAR, sizeof( CONFIG_FIC_PID ) ) ;
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
     if(!strcmp("TEMPO_LCD_LOOP",g_Datas[l][0])) TEMPO_LCD_LOOP=atol(g_Datas[l][1]);
     if(!strcmp("TEMPO_LCD_DISP",g_Datas[l][0])) TEMPO_LCD_DISP=atol(g_Datas[l][1]);

     if(!strcmp("PID_ECH",g_Datas[l][0])) PID_ECH=atof(g_Datas[l][1]);
     if(!strcmp("PID_KP", g_Datas[l][0])) PID_KP =atof(g_Datas[l][1]);
     if(!strcmp("PID_KI", g_Datas[l][0])) PID_KP =atof(g_Datas[l][1]);
     if(!strcmp("PID_KD", g_Datas[l][0])) PID_KD =atof(g_Datas[l][1]);

     if(!strcmp("GPIO_LED_ETAT",g_Datas[l][0]))  GPIO_LED_ETAT=atoi(g_Datas[l][1]);

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
    if(!strcmp("CONFIG_FIC_PID",g_Datas[l][0]))  strcpy( CONFIG_FIC_PID, g_Datas[l][1]) ;
    if(!strcmp("CONFIG_FIC_LED",g_Datas[l][0]))  strcpy( CONFIG_FIC_LED, g_Datas[l][1]) ;
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
* @fn     : CONFIG_AFFICHER_ETAT_THREADS
* @author : s.gravois
* @brief  : Cette fonction affiche l etat des threads en cours 
* @param  : void
* @date   : 2022-05-24 creation
* @todo   : a finir
*****************************************************************************************/

void   CONFIG_AFFICHER_ETAT_THREADS(SUIVI * gp_Sui) {

  char c_thread_name [ 16 ] ;
  int i_num_thread=0 ;
  long id_thread ;

  for (i_num_thread=0; i_num_thread < MAX_THREADS; i_num_thread++) {

    id_thread = gp_Sui->p_pth->pth_att[i_num_thread].att_pid ;
    
    memset( c_thread_name, 0, sizeof(c_thread_name) ) ;
    
    if ( id_thread > 0) pthread_getname_np( id_thread , c_thread_name, 16 ) ;
    else break ;
    
    Trace1("Thread %d : id %ld nom %s ", i_num_thread, id_thread, c_thread_name )  ;
  }
  return ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_VARIABLES
* @author : s.gravois
* @brief  : Cette fonction affiche les parametres 
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-05-30 ajout TEMPO_LCD_xxx 
* @date   : 2022-06-17 ajout PID_ECH
* @todo   : 
*****************************************************************************************/

void   CONFIG_AFFICHER_VARIABLES(void) {

  Trace("TEMPO_RAQ = %ld",  TEMPO_RAQ);
  Trace("TEMPO_MENU = %ld",  TEMPO_MENU);
  Trace("TEMPO_IR = %ld",  TEMPO_IR);
  Trace("TEMPO_TERMIOS = %ld",  TEMPO_TERMIOS);
  Trace("TEMPO_CLAVIER = %ld",  TEMPO_CLAVIER);
  Trace("TEMPO_CAPTEURS = %ld",  TEMPO_CAPTEURS);
  Trace("TEMPO_LCD_LOOP = %ld",  TEMPO_LCD_LOOP);
  Trace("TEMPO_LCD_DISP = %ld",  TEMPO_LCD_DISP);

  Trace("PID_ECH = %f",  PID_ECH);
  Trace("PID_KI = %f",  PID_KI);
  Trace("PID_KP = %f",  PID_KP);
  Trace("PID_KD = %f",  PID_KD);

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
  Trace1("CONFIG_FIC_PID = %s", CONFIG_FIC_PID)  ; 
  Trace1("CONFIG_FIC_LED = %s", CONFIG_FIC_LED)  ; 
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

  FILE * fin ;
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
* @param  : LIEU *gp_Lie
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_AFFICHER_LIEU(LIEU *gp_Lie) {

  Trace1(" latitude   :  %.2f", gp_Lie->lat * DEGRES ) ; 
  Trace1(" longitude  :  %.2f", gp_Lie->lon * DEGRES ) ;
  Trace1("gp_Lie->JD    : %f",gp_Lie->JD) ;
  Trace1("gp_Lie->JJ    : %f",gp_Lie->JJ) ;
  Trace1("gp_Lie->TS    : %f",gp_Lie->TS) ;
  Trace1("gp_Lie->TSR   : %f",gp_Lie->TSR) ;

  Trace1("gp_Lie->alt = %f",gp_Lie->alt ) ;


  Trace1("gp_Lie->lat (deg) = %f",gp_Lie->lat * DEGRES ) ;
  Trace1("gp_Lie->lon (deg) = %f",gp_Lie->lon * DEGRES ) ;

  Trace1("----------------------------") ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_TEMPS
* @author : s.gravois
* @brief  : Cette fonction affiche les informations de temps
* @param  : LIEU *gp_Lie
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_AFFICHER_TEMPS(TEMPS * gp_Tim) {
  
  Trace(" DATE / HOUR : %d-%d-%d %-2d:%-2d:%d : %f", gp_Tim->yy, gp_Tim->mm, gp_Tim->dd, gp_Tim->HH, gp_Tim->MM, gp_Tim->SS, gp_Tim->hd ) ;

  Trace1("gp_Tim->yy = %d", gp_Tim->yy ) ;
  Trace1("gp_Tim->mm = %d", gp_Tim->mm ) ;
  Trace1("gp_Tim->dd = %d", gp_Tim->dd ) ;
  Trace1("gp_Tim->HH = %d", gp_Tim->HH ) ;
  Trace1("gp_Tim->MM = %d", gp_Tim->MM ) ;
  Trace1("gp_Tim->SS = %d", gp_Tim->SS ) ;
  Trace1("gp_Tim->hd = %f", gp_Tim->hd ) ;

  Trace("----------------------------") ;

}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_CLAVIER
* @author : s.gravois
* @brief  : Cette fonction affiche les informations d entrees input ("gp_Cla")
* @param  : CLAVIER *gp_Cla
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_AFFICHER_CLAVIER(CLAVIER *gp_Cla) {
  
  Trace1("phr %s mot %s sym %s nom %s pre %s val %s menu %s",\
    gp_Cla->phrase,\
    gp_Cla->mot,\
    gp_Cla->symbole,\
    gp_Cla->nombre,\
    gp_Cla->premier,\
    gp_Cla->valider,\
    gp_Cla->menu) ;

  Trace1("gp_Cla->mot         = %s",gp_Cla->mot) ;
  Trace1("gp_Cla->premier     = %s",gp_Cla->premier) ;
  Trace1("gp_Cla->phrase      = %s",gp_Cla->phrase) ;
  Trace1("gp_Cla->nombre      = %s",gp_Cla->nombre) ;
  Trace1("gp_Cla->symbole     = %s",gp_Cla->symbole) ;
  Trace1("gp_Cla->phrase_lue  = %d",gp_Cla->phrase_lue) ;
}
/*****************************************************************************************
* @fn     : CONFIG_FORMATE_DONNEES_AFFICHAGE
* @author : s.gravois
* @brief  : Cette fonction formate divers string en vue d un affichage pertinent
* @param  : ASTRE *gp_Ast
* @date   : 2022-04-12 creation
* @date   : 2022-04-21 remplacement ° par o (affichage LCD ° impossible)
* @date   : 2022-04-21 ajout de 2 resolutions plus simple (affichage contraint par LCD) :
* @date   : 2022-04-21 - c_hhmm_*
* @date   : 2022-04-21 - c_dd_*  
*****************************************************************************************/

void CONFIG_FORMATE_DONNEES_AFFICHAGE(ASTRE *gp_Ast) {

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

  sprintf( c_hhmmss_agh, " %3dh%2dm%2ds",  gp_Ast->AGHt.HH, gp_Ast->AGHt.MM, gp_Ast->AGHt.SS  ) ;
  sprintf( c_hhmmss_asc, " %3dh%2dm%2ds",  gp_Ast->ASCt.HH, gp_Ast->ASCt.MM, gp_Ast->ASCt.SS  ) ;
  sprintf( c_hhmmss_azi, " %3dh%2dm%2ds",  gp_Ast->AZIt.HH, gp_Ast->AZIt.MM, gp_Ast->AZIt.SS  ) ;
  sprintf( c_hhmmss_alt, " %3dh%2dm%2ds",  gp_Ast->ALTt.HH, gp_Ast->ALTt.MM, gp_Ast->ALTt.SS  ) ;
  sprintf( c_hhmmss_dec, " %3dh%2dm%2ds",  gp_Ast->DECt.HH, gp_Ast->DECt.MM, gp_Ast->DECt.SS  ) ;

  sprintf( c_hhmm_agh, " %3d h %2d m",  gp_Ast->AGHt.HH, gp_Ast->AGHt.MM ) ;
  sprintf( c_hhmm_asc, " %3d h %2d m",  gp_Ast->ASCt.HH, gp_Ast->ASCt.MM ) ;
  sprintf( c_hhmm_azi, " %3d h %2d m",  gp_Ast->AZIt.HH, gp_Ast->AZIt.MM ) ;
  sprintf( c_hhmm_alt, " %3d h %2d m",  gp_Ast->ALTt.HH, gp_Ast->ALTt.MM ) ;
  sprintf( c_hhmm_dec, " %3d h %2d m",  gp_Ast->DECt.HH, gp_Ast->DECt.MM ) ;

  /* traitement des donnees en degres / minutes / secondes */
  /* est inclus dans l affichage le signe */

  sprintf( c_ddmm_agh, "%c %-3d d %d'", gp_Ast->AGHa.c_si, gp_Ast->AGHa.DD, gp_Ast->AGHa.MM ) ;
  sprintf( c_ddmm_asc, "%c %-3d d %d'", gp_Ast->ASCa.c_si, gp_Ast->ASCa.DD, gp_Ast->ASCa.MM ) ;
  sprintf( c_ddmm_azi, "%c %-3d d %d'", gp_Ast->AZIa.c_si, gp_Ast->AZIa.DD, gp_Ast->AZIa.MM ) ;
  sprintf( c_ddmm_alt, "%c %-3d d %d'", gp_Ast->ALTa.c_si, gp_Ast->ALTa.DD, gp_Ast->ALTa.MM ) ;
  sprintf( c_ddmm_dec, "%c %-3d d %d'", gp_Ast->DECa.c_si, gp_Ast->DECa.DD, gp_Ast->DECa.MM ) ;

  sprintf( c_dd_agh, "%c %-3d deg", gp_Ast->AGHa.c_si, gp_Ast->AGHa.DD ) ;
  sprintf( c_dd_asc, "%c %-3d deg", gp_Ast->ASCa.c_si, gp_Ast->ASCa.DD ) ;
  sprintf( c_dd_azi, "%c %-3d deg", gp_Ast->AZIa.c_si, gp_Ast->AZIa.DD ) ;
  sprintf( c_dd_alt, "%c %-3d deg", gp_Ast->ALTa.c_si, gp_Ast->ALTa.DD ) ;
  sprintf( c_dd_dec, "%c %-3d deg", gp_Ast->DECa.c_si, gp_Ast->DECa.DD ) ;

  /* Sauvegarde des donnees formatees dans la structure astre */
  
  strcpy( gp_Ast->c_hhmmss_agh, c_hhmmss_agh)  ;
  strcpy( gp_Ast->c_hhmmss_asc, c_hhmmss_asc)  ;
  strcpy( gp_Ast->c_hhmmss_azi, c_hhmmss_azi)  ;
  strcpy( gp_Ast->c_hhmmss_alt, c_hhmmss_alt)  ;
  strcpy( gp_Ast->c_hhmmss_dec, c_hhmmss_dec)  ;

  strcpy( gp_Ast->c_hhmm_agh, c_hhmm_agh)  ;
  strcpy( gp_Ast->c_hhmm_asc, c_hhmm_asc)  ;
  strcpy( gp_Ast->c_hhmm_azi, c_hhmm_azi)  ;
  strcpy( gp_Ast->c_hhmm_alt, c_hhmm_alt)  ;
  strcpy( gp_Ast->c_hhmm_dec, c_hhmm_dec)  ;

  strcpy( gp_Ast->c_ddmm_agh, c_ddmm_agh)  ;
  strcpy( gp_Ast->c_ddmm_asc, c_ddmm_asc)  ;
  strcpy( gp_Ast->c_ddmm_azi, c_ddmm_azi)  ;
  strcpy( gp_Ast->c_ddmm_alt, c_ddmm_alt)  ;
  strcpy( gp_Ast->c_ddmm_dec, c_ddmm_dec)  ;

  strcpy( gp_Ast->c_dd_agh, c_dd_agh)  ;
  strcpy( gp_Ast->c_dd_asc, c_dd_asc)  ;
  strcpy( gp_Ast->c_dd_azi, c_dd_azi)  ;
  strcpy( gp_Ast->c_dd_alt, c_dd_alt)  ;
  strcpy( gp_Ast->c_dd_dec, c_dd_dec)  ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_MODE_STELLARIUM
* @author : s.gravois
* @brief  : Cette fonction affiche les informations a la sauce stellarium
* @param  : 
* @date   : 2022-03-18 creation
* @date   : 2022-03-28 simplification
*****************************************************************************************/

void CONFIG_AFFICHER_MODE_STELLARIUM(ASTRE *gp_Ast) {

  Trace("Va / Vh    : %3.2f / %3.2f" , gp_Ast->Va           , gp_Ast->Vh ) ;
  Trace("AD / Dec   : %s / %s"       , gp_Ast->c_hhmmss_asc , gp_Ast->c_ddmm_dec ) ;
  Trace("AH / Dec   : %s / %s"       , gp_Ast->c_hhmmss_agh , gp_Ast->c_ddmm_dec ) ;
  Trace("AZ./ Haut. : %s / %s"       , gp_Ast->c_ddmm_azi   , gp_Ast->c_ddmm_alt ) ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_MODE_LONG
* @author : s.gravois
* @brief  : Cette fonction affiche les informations relatives a l as observee
* @param  : ASTRE *gp_Ast
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
*****************************************************************************************/

void CONFIG_AFFICHER_MODE_LONG(void) {
  
  const char * c_nom  = gp_Ast->nom ;
  const char * c_type = c_Astre_Type [ gp_Ast->type ] ;
  const char * c_mode = c_Mode_Calcul[ gp_Ast->mode ] ;

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

  sprintf( c_hhmmss_agh, "%dh%dm%ds",   gp_Ast->AGHt.HH,  gp_Ast->AGHt.MM,  gp_Ast->AGHt.SS  ) ;
  sprintf( c_hhmmss_asc,  "%dh%dm%ds",  gp_Ast->ASCt.HH,   gp_Ast->ASCt.MM,   gp_Ast->ASCt.SS  ) ;

  sprintf( c_hhmmss_agh0, "%dh%dm%ds", gp_Ast->AGH0t.HH, gp_Ast->AGH0t.MM, gp_Ast->AGH0t.SS  ) ;
  sprintf( c_hhmmss_agh1, "%dh%dm%ds", gp_Ast->AGH1t.HH, gp_Ast->AGH1t.MM, gp_Ast->AGH1t.SS  ) ;
  sprintf( c_hhmmss_agh2, "%dh%dm%ds", gp_Ast->AGH2t.HH, gp_Ast->AGH2t.MM, gp_Ast->AGH2t.SS  ) ;

  sprintf( c_hhmmss_azi0, "%dh%dm%ds", gp_Ast->AZI0t.HH, gp_Ast->AZI0t.MM, gp_Ast->AZI0t.SS  ) ;
  sprintf( c_hhmmss_azi1, "%dh%dm%ds", gp_Ast->AZI1t.HH, gp_Ast->AZI1t.MM, gp_Ast->AZI1t.SS  ) ;
  sprintf( c_hhmmss_azi2, "%dh%dm%ds", gp_Ast->AZI2t.HH, gp_Ast->AZI2t.MM, gp_Ast->AZI2t.SS  ) ;

  Trace(" %s : infos         : %s", c_nom , gp_Ast->infos ) ;
  Trace(" %s : type          : %s", c_nom , c_type ) ;
  Trace(" %s : mode calcul   : %s", c_nom , c_mode ) ;
  Trace(" %s : latitude      : %.2f (deg) ", c_nom, gp_Lie->lat    * DEGRES ) ;
  Trace(" %s : longitude     : %.2f (deg) ", c_nom, gp_Lie->lon    * DEGRES ) ;
  Trace(" %s : vitesses      : %.2f (Va) %.2f (Vh)", c_nom, gp_Ast->Va,  gp_Ast->Vh ) ; 
  Trace(" %s : azimut        : %.2f (deg) ", c_nom, gp_Ast->a    * DEGRES ) ;
  Trace(" %s : altitude      : %.2f (deg) ", c_nom, gp_Ast->h    * DEGRES ) ;
  Trace(" %s : declinaison   : %.2f (deg) ", c_nom, gp_Ast->DEC  * DEGRES  ) ;
  Trace(" %s : ascension dro : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->ASC    * DEGRES, c_hhmmss_asc ) ;
  Trace(" %s : angle horaire : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH   * DEGRES, c_hhmmss_agh ) ;

  Trace1(" %s : Agh0          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH0  * DEGRES, c_hhmmss_agh0 ) ;
  Trace1(" %s : Agh1          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH1  * DEGRES, c_hhmmss_agh1 ) ;
  Trace1(" %s : Agh2          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH2  * DEGRES, c_hhmmss_agh2 ) ;

  Trace1(" %s : Azi0          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AZI0  * DEGRES, c_hhmmss_azi0 ) ;
  Trace1(" %s : Azi1          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AZI1  * DEGRES, c_hhmmss_azi1 ) ;
  Trace1(" %s : Azi2          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AZI2  * DEGRES, c_hhmmss_azi2 ) ;

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

void CONFIG_AFFICHER_VOUTE( VOUTE * gp_Vou) {
	
	Trace1("gp_Vou->num %lld", gp_Vou->num) ;
  Trace1("gp_Vou->pas %lld", gp_Vou->num) ;

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

  CONFIG_AFFICHER_TEMPS(   gp_Tim ) ;
  CONFIG_AFFICHER_LIEU(    gp_Lie );
  CONFIG_AFFICHER_MODE_LONG(  ) ;
  CONFIG_AFFICHER_VOUTE(   gp_Vou ) ;

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

  switch(gp_Sui->menu) {
     
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

  if ( gp_Sui->menu_old != gp_Sui->menu ) {

    Trace("appel : %d : %s" , gp_Sui->menu, s_menu) ;
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

  Trace("gp_Dev->DEVICE_USE_INFRAROUGE = %d",gp_Dev->DEVICE_USE_INFRAROUGE) ;
  Trace("gp_Dev->DEVICE_USE_CAPTEURS   = %d",gp_Dev->DEVICE_USE_CAPTEURS) ;
  Trace("gp_Dev->DEVICE_USE_RAQUETTE   = %d",gp_Dev->DEVICE_USE_RAQUETTE) ;
  Trace("gp_Dev->DEVICE_USE_BLUETOOTH  = %d",gp_Dev->DEVICE_USE_BLUETOOTH) ;
  Trace("gp_Dev->DEVICE_USE_KEYBOARD   = %d",gp_Dev->DEVICE_USE_KEYBOARD) ;
  Trace("gp_Dev->DEVICE_USE_CONTROLER  = %d",gp_Dev->DEVICE_USE_CONTROLER) ;
  Trace("gp_Dev->DEVICE_USE_LCD        = %d",gp_Dev->DEVICE_USE_LCD) ;

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
  Trace1("Chemin retenu : %s", g_paths) ;

  return i_retour ; 
}
//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================



