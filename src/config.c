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

//==========================================================
// cette fonction ecrit dans la log avec la commande system

void CONFIG_SYSTEM_LOG(int *incrlog) {
  int ret ;
  (*incrlog)++ ;
  char cmd[255] ;
  if ( CONFIG_ASTROLOG ) {
    // FIXME 01 mai 2021 : modification chemin relatif
    sprintf(cmd,"echo %d >> %s/%s/%s",*incrlog, CONFIG_REP_HOME, CONFIG_REP_LOG, CONFIG_FIC_LOG) ;
    ret = system(cmd) ;
    if ( ret < 0 ) TRACE("Probleme avec %s : retourner avec error negative",cmd) ;
    //if ( ret == 0 ) TRACE("Probleme avec %s : shell non disponible",cmd) ;
  }
}
//==========================================================
// cette fonction ecrit dans la log avec un fwrite

void CONFIG_LOG(char *txt) {
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

  if ( CONFIG_ASTROLOG ) {
    if ( strftime(s_date, sizeof(s_date), "%Y%m%d.%H%M", tmp) == 0 ) {
        fprintf(stderr, "strftime a renvoyé 0");
        exit(EXIT_FAILURE);
    }
    sprintf( c_out, "%s : %s", s_date, txt ) ;
    sprintf( cmd,"echo %s >> %s/%s/%s",c_out, CONFIG_REP_HOME, CONFIG_REP_LOG, CONFIG_FIC_LOG) ;
    ret =  system(cmd) ;
    if ( ret < 0 ) TRACE("Probleme avec %s : retourner avec error negative",cmd) ;
    //if ( ret == 0 ) TRACE("Probleme avec %s : shell non disponible",cmd) ;
  }
}
//==========================================================
void CONFIG_INIT_LOG(void) {

  char buf[255] ;
  
  if ( CONFIG_ASTROLOG ) {
    memset(buf, ZERO_CHAR, sizeof(buf));
    sprintf(buf,"%s/%s/%s", CONFIG_REP_HOME, CONFIG_REP_LOG, CONFIG_FIC_LOG) ;
    
    if ( (flog=fopen(buf,"a")) == NULL) {
      // completer et modifier
      TRACE("probleme ouverture %s\n",buf) ;
      exit(2) ;
    }
    else printf("open %s ok\n", buf) ;
  }
  return ;
}
//---------------------------------------------------------------------------------------
void CONFIG_INIT_ASTRE(ASTRE *as) {

  int C ;
  
  for(C=0; C< ASTRE_NB_COLONNES;C++) {
    memset( as->plus_proche[C], ZERO_CHAR, ASTRE_TAILLE_BUFFER);
    strcpy( as->plus_proche[C], "") ;
  }
  memset( as->nom,   ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  memset( as->infos, ZERO_CHAR, ASTRE_TAILLE_BUFFER);
  
  as->a   = 0.0  ;
  as->h   = 0.0  ;
  as->a0  = 0.0 ;
  as->h0  = 0.0 ;
  as->A   = 0.0  ;
  as->H   = 0.0   ;
  as->A0  = 0.0 ;
  as->H0  = 0.0 ;
  as->da  = 0.0 ;
  as->dh  = 0.0 ;
  as->dA  = 0.0 ;
  as->dH  = 0.0 ;
  as->Va  = 0.0 ;
  as->Vh  = 0.0 ;
  as->dVa = 0.0 ;
  as->dVh = 0.0 ;
  as->dVam= 0.0 ;
  as->dVhm= 0.0 ;
}
//---------------------------------------------------------------------------------------
void CONFIG_INIT_CLAVIER(CLAVIER * clavier) {

 int i ;
  
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
  
  for( i=0 ; i < CONFIG_ACTIONS_SIZE ; i++ )      memset( clavier->actions[i],     ZERO_CHAR, CONFIG_TAILLE_BUFFER_32);
  // for( i=0 ; i < CONFIG_VALIDATIONS_SIZE ; i++ )  memset( clavier->validations[i], ZERO_CHAR, CONFIG_TAILLE_BUFFER_32);

  strcpy( clavier->actions[0], "MENU" ) ;
  strcpy( clavier->actions[1], "SETUP" ) ;
  strcpy( clavier->actions[2], "MES" ) ;
  strcpy( clavier->actions[3], "NGC" ) ;
  strcpy( clavier->actions[4], "ETO" ) ;
  strcpy( clavier->actions[5], "PLA" ) ;
  strcpy( clavier->actions[6], "TIME" ) ;
   
} 
//---------------------------------------------------------------------------------------
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
//Display time (using +FMT), or set time
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

void CONFIG_SET_YEAR_MONTH_AND_DAY(char * s_data) { // taille des datas = 5 (unite de l'annee en premier)
 
  char buf [   CONFIG_TAILLE_BUFFER_64 ]  ;
  char year [ CONFIG_TAILLE_BUFFER_8] ;
  char month [ CONFIG_TAILLE_BUFFER_4] ;
  char day [   CONFIG_TAILLE_BUFFER_4] ;
  int i = 0 ;

  if ( strlen(s_data) != 8 ){

    TRACE("mauvais format : %s", s_data) ;
    TRACE("la chaine de caractere doit etre composee de <annee><mois><jour>") ;
    TRACE("Exemple : 20190822 = 22 aout 2019") ;
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
	TRACE("buf = %s", buf) ;

  memset( buf, ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  sprintf(buf, "/bin/echo %s-%s-%s > %s/%s/%s ", year, month, day, CONFIG_REP_HOME, CONFIG_REP_CFG, CONFIG_FIC_DATE ) ;
  TRACE("buf = %s", buf) ;

  if ( system( buf ) < 0 ) perror( buf) ;

  TRACE("Nouvelle date : %s", buf) ;

}
//---------------------------------------------------------------------------------------
void CONFIG_SET_HOUR_AND_MINUTES(char * s_data) {

  char buf [ CONFIG_TAILLE_BUFFER_64 ]  ;
  char hou [ CONFIG_TAILLE_BUFFER_4] ;
  char min [ CONFIG_TAILLE_BUFFER_4] ;
  int i = 0 ;
  
  // FIXME : 20190822 : ajout traces

  if ( strlen(s_data) != 4 ){

    TRACE("mauvais format : %s", s_data) ;
    TRACE("la chaine de caractere doit etre composee de <heure><minutes>") ;
    TRACE("Exemple : 0804 = 8 heures et 4 minutes") ;
    return ;
  }

  memset( buf, ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  memset( hou, ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  memset( min, ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  
  for(i=0;i<2;i++) hou[i]=s_data[i] ;
  for(i=0;i<2;i++) min[i]=s_data[i+2] ;
  
  sprintf(buf, "/bin/date -s %s:%s",hou, min ) ;
  TRACE("buf = %s", buf) ;
  if ( system( buf ) < 0 ) perror( buf) ;

  memset( buf, ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  sprintf(buf, "/bin/echo %s:%s > %s/%s/%s ", hou, min, CONFIG_REP_HOME, CONFIG_REP_CFG, CONFIG_FIC_HHMM ) ;
  TRACE("buf = %s", buf) ;
  if ( system( buf ) < 0 ) perror( buf) ;
}
//---------------------------------------------------------------------------------------
void CONFIG_INIT_TEMPS( TEMPS *temps) {
  
  temps->mm = 0 ;  // month
  temps->yy = 0 ;  // year
  temps->dd = 0 ;  // day
  temps->HH = 0 ;  // hour
  temps->MM = 0 ;  // minutes
  temps->SS = 0 ;  // secondes
  temps->hd = 0 ;  // heure decimale
}
//---------------------------------------------------------------------------------------
void CONFIG_INIT_VOUTE(VOUTE *voute) {
  
  voute->pourcentage_tempo = 0.96 ;   
  voute->calibration_voute = 0.97 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
  
	voute->num   = 0 ;
  voute->deb   = 0 ;
  voute->fin   = 0 ;
  voute->dt    = 1  ;
  voute->pas   = voute->dt * ROT_RAD_SEC ;
  voute->acc_old = 1 ;
  voute->acc   = 1 ;
  voute->DT    = (unsigned long)( voute->dt * CONFIG_MICRO_SEC / voute->acc ) ;   // dt en micro-sec
}
//---------------------------------------------------------------------------------------
void CONFIG_VOUTE( VOUTE *voute, double dt, double acc, double percent ) {
  
  voute->pourcentage_tempo  = percent ; 
  voute->calibration_voute  = 0.99 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
  voute->deb                = 0 ;
  voute->num                = 0 ;
  voute->fin                = PIPI ;
  voute->dt                 = dt  ;
  voute->pas                = voute->dt * ROT_RAD_SEC ;
  voute->acc_old            = voute->acc ;
  voute->acc                = acc ;
  voute->DT                 = (unsigned long)( voute->dt * CONFIG_MICRO_SEC / voute->acc ) ;   // dt en micro-sec
}
//---------------------------------------------------------------------------------------
void CONFIG_INIT_SUIVI(SUIVI *suivi) {
  
  int i ;
  
  suivi->SUIVI_MANUEL     = 0 ;
  suivi->SUIVI_ALIGNEMENT = 0 ;
  suivi->SUIVI_GOTO       = 0 ;
  suivi->SUIVI_VOUTE      = 1 ;
  suivi->SUIVI_EQUATORIAL = 0 ;

  // a modifier  : instancier ces variables a l aide du fichier de config

  suivi->DONNEES_CAPTEURS    = DONNEES_CAPTEURS ;
  suivi->DONNEES_RAQUETTE    = DONNEES_RAQUETTE ;
  suivi->DONNEES_BLUETOOTH   = DONNEES_BLUETOOTH ;
  suivi->DONNEES_INFRAROUGE  = DONNEES_INFRAROUGE ;
  suivi->DONNEES_CONTROLEUR  = DONNEES_CONTROLEUR ;

  suivi->init_capteurs = 0 ; 
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
  suivi->pas_forward    = 0 ;
  suivi->pas_rewind     = 0 ;
  suivi->pas_forward_fast   = 0 ;
  suivi->pas_rewind_fast  = 0 ;

  suivi->acc_azi   = 1.0 ;     // cette variable est utilisee dans le calcul des periodes
  suivi->acc_alt   = 1.0 ;     // cette variable est utilisee dans le calcul des periodes
  suivi->sgn_azi   = 1 ;
  suivi->sgn_alt   = 1 ;

  suivi->t_diff = 0 ;
  suivi->t_diff_sec = 0.0 ;
  suivi->d_temps          = 0.0 ;   
  suivi->d_appui_raq_azi  = 0.0 ;
  suivi->d_appui_raq_alt  = 0.0 ;
  
  suivi->Fa        = 30 ;
  suivi->Fh        = 30 ;

  suivi->Ta        = 1/suivi->Fa ;
  suivi->Th        = 1/suivi->Fh ;
  
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
  
  suivi->l_NANO_MOINS     = 0.0  ;     // a retirer sur les temporisations pour les tests
   
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
  suivi->temporisation_clavier  = TEMPO_CLAVIER ; 
  suivi->temporisation_capteurs = TEMPO_CAPTEURS ;
  
  suivi->DTh = suivi->Th * CONFIG_MICRO_SEC ;
  suivi->DTa = suivi->Ta * CONFIG_MICRO_SEC ;

  pthread_mutex_init( & suivi->mutex_alt, NULL ) ;
  pthread_mutex_init( & suivi->mutex_azi, NULL ) ;
  pthread_mutex_init( & suivi->mutex_cal, NULL ) ;

  suivi->temps_a = 0 ; 
  suivi->temps_h = 0 ; 
  
   gettimeofday(&suivi->tval,NULL) ;
}
//============================================================================
int CONFIG_FORMAT_ADMIS(char c) {
  
  int i=(int)c ;
  int j ;
  
  j=0;
  
  if ( i>45 && i<58 )j=1;  // chiffres et le point
  if ( i>64 && i<91 )j=1;  // MAJUSCULES  
  if ( i>96 && i<123)j=1; // minuscules
  if ( i==95)j=1;         // underscore
  if ( i==44)j=1;         // virgule
  if ( i==45)j=1;         // signe -
  return j ; 
}
//============================================================================
int CONFIG_FIN_LIGNE(char c) {
  int i=(int)c ;
  int j ;
  j=0;
  if ( i==10)j=1;  // \n
  return j ; 
}
//============================================================================
int CONFIG_DEB_COM(char c) {
  int i=(int)c ;
  int j ;
  j=0;
  if ( i==35)j=1 ; // //
  return j ; 
}
//============================================================================
int CONFIG_FIN_MOT(char c) {
  int i=(int)c ;
  int j ;
  j=0;
  if ( i==9 )j=1;  // \t
  if ( i==32)j=1;  // space
  return j ; 
}
//============================================================================
int CONFIG_FIN_FICHIER(char c) {
  int i=(int)c ;
  int j ;
  
  j=0;
  if ( i>122 )j=1;      // caracteres non admis ascii > 122
  if ( i==-1) j=1;      // EOF
  if ( i<32 && i!=9 && i!=10 ) j=1; // caracteres non admis ascii < 32
  return j ; 
}
//============================================================================
void CONFIG_INIT_VAR(char datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) {

  int l ;

   //----------------------------------------------------------------------
   // Variables susceptibles de ne pas etre lues (doivent etre mise a zero)
   //----------------------------------------------------------------------
   
   ALT_R = 0 ;
   AZI_R = 0 ;
   DONNEES_CAPTEURS = 0    ;
   DONNEES_RAQUETTE = 0    ;
   DONNEES_BLUETOOTH = 0   ;
   DONNEES_INFRAROUGE = 0  ;
   DONNEES_CONTROLEUR = 0 ;
   GPIO_LED_ETAT=0;

   //----------------------------------------------------------------------
   // Lecture des variables dans la config lue dans le fichier de config
   //-----------ASTRE_PAR_DEFAUT-------------------------------------------

  memset( ASTRE_PAR_DEFAUT, ZERO_CHAR, sizeof( ASTRE_PAR_DEFAUT ) ) ;
  memset( CONFIG_REP_CAT, ZERO_CHAR, sizeof( CONFIG_REP_CAT ) ) ;
  memset( CONFIG_REP_CFG, ZERO_CHAR, sizeof( CONFIG_REP_CFG ) ) ;
  memset( CONFIG_REP_LOG, ZERO_CHAR, sizeof( CONFIG_REP_LOG ) ) ;
  memset( CONFIG_REP_IN, ZERO_CHAR, sizeof( CONFIG_REP_IN ) ) ;
  memset( CONFIG_FIC_LOG, ZERO_CHAR, sizeof( CONFIG_FIC_LOG ) ) ;
  memset( CONFIG_FIC_DATE, ZERO_CHAR, sizeof( CONFIG_FIC_DATE ) ) ;
  memset( CONFIG_FIC_HHMM, ZERO_CHAR, sizeof( CONFIG_FIC_HHMM ) ) ;
   
  for(l=0;l<DATAS_NB_LIGNES;l++) {
     
      // FIXME : note 2021 : les variables GPIO_xxx sont gérées dans le ficheir gpio.c

     if(!strcmp("ASTRE_PAR_DEFAUT",datas[l][0])) strcpy( ASTRE_PAR_DEFAUT, datas[l][1]) ;
     
     if(!strcmp("TEMPO_RAQ",datas[l][0]))      TEMPO_RAQ=atol(datas[l][1]);
     if(!strcmp("TEMPO_MENU",datas[l][0]))     TEMPO_MENU=atol(datas[l][1]);
     if(!strcmp("TEMPO_IR",datas[l][0]))       TEMPO_IR=atol(datas[l][1]);
     if(!strcmp("TEMPO_CLAVIER",datas[l][0]))  TEMPO_CLAVIER=atol(datas[l][1]);
     if(!strcmp("TEMPO_CAPTEURS",datas[l][0])) TEMPO_CAPTEURS=atol(datas[l][1]);
     
     if(!strcmp("MODE_EQUATORIAL",datas[l][0]))    MODE_EQUATORIAL=atoi(datas[l][1]);
     if(!strcmp("MENU_PAR_DEFAUT",datas[l][0]))    MENU_PAR_DEFAUT=atoi(datas[l][1]);

     if(!strcmp("GPIO_LED_ETAT",datas[l][0]))      GPIO_LED_ETAT=atoi(datas[l][1]);

     if(!strcmp("DONNEES_CONTROLEUR",datas[l][0]))  DONNEES_CONTROLEUR=atoi(datas[l][1]);
     if(!strcmp("DONNEES_CAPTEURS",datas[l][0]))   DONNEES_CAPTEURS=atoi(datas[l][1]);
     if(!strcmp("DONNEES_RAQUETTE",datas[l][0]))   DONNEES_RAQUETTE=atoi(datas[l][1]);
     if(!strcmp("DONNEES_BLUETOOTH",datas[l][0]))  DONNEES_BLUETOOTH=atoi(datas[l][1]);
     if(!strcmp("DONNEES_INFRAROUGE",datas[l][0])) DONNEES_INFRAROUGE=atoi(datas[l][1]);

     if(!strcmp("ALT_ROT",datas[l][0]))      ALT_ROT=atoi(datas[l][1]);
     if(!strcmp("AZI_ROT",datas[l][0]))      AZI_ROT=atoi(datas[l][1]);
     
     if(!strcmp("ALT_F",datas[l][0]))        ALT_F=atol(datas[l][1]);
     if(!strcmp("AZI_F",datas[l][0]))        AZI_F=atol(datas[l][1]);
     
     if(!strcmp("ALT_N",datas[l][0]))        ALT_N=atoi(datas[l][1]);
     if(!strcmp("AZI_N",datas[l][0]))        AZI_N=atoi(datas[l][1]);
     
     if(!strcmp("ALT_R",datas[l][0]))        ALT_R=atof(datas[l][1]);
     if(!strcmp("AZI_R",datas[l][0]))        AZI_R=atof(datas[l][1]);
     
     if(!strcmp("GPIO_RAQ_O",datas[l][0]))   GPIO_RAQ_O=atoi(datas[l][1]);
     if(!strcmp("GPIO_RAQ_E",datas[l][0]))   GPIO_RAQ_E=atoi(datas[l][1]);
     if(!strcmp("GPIO_RAQ_S",datas[l][0]))   GPIO_RAQ_S=atoi(datas[l][1]);
     if(!strcmp("GPIO_RAQ_N",datas[l][0]))   GPIO_RAQ_N=atoi(datas[l][1]);
     if(!strcmp("GPIO_RAQ_V",datas[l][0]))   GPIO_RAQ_V=atoi(datas[l][1]);
     
     if(!strcmp("GPIO_KEY_L1",datas[l][0]))   GPIO_KEY_L1=atoi(datas[l][1]);
     if(!strcmp("GPIO_KEY_L2",datas[l][0]))   GPIO_KEY_L2=atoi(datas[l][1]);
     if(!strcmp("GPIO_KEY_L3",datas[l][0]))   GPIO_KEY_L3=atoi(datas[l][1]);
     if(!strcmp("GPIO_KEY_L4",datas[l][0]))   GPIO_KEY_L4=atoi(datas[l][1]);
     
     if(!strcmp("GPIO_KEY_C1",datas[l][0]))   GPIO_KEY_C1=atoi(datas[l][1]);
     if(!strcmp("GPIO_KEY_C2",datas[l][0]))   GPIO_KEY_C2=atoi(datas[l][1]);
     if(!strcmp("GPIO_KEY_C3",datas[l][0]))   GPIO_KEY_C3=atoi(datas[l][1]);
     if(!strcmp("GPIO_KEY_C4",datas[l][0]))   GPIO_KEY_C4=atoi(datas[l][1]);

     if(!strcmp("LONGITUDE",datas[l][0]))     LONGITUDE=atof(datas[l][1]);
     if(!strcmp("LATITUDE",datas[l][0]))      LATITUDE=atof(datas[l][1]);
     if(!strcmp("ALTITUDE",datas[l][0]))      ALTITUDE=atof(datas[l][1]);

     // donnees de altitude

     if(!strcmp("ALT_R1",datas[l][0]))       ALT_R1 = atof(datas[l][1]);         
     if(!strcmp("ALT_R2",datas[l][0]))       ALT_R2 = atof(datas[l][1]);
     if(!strcmp("ALT_R3",datas[l][0]))       ALT_R3 = atof(datas[l][1]);         
     if(!strcmp("ALT_R4",datas[l][0]))       ALT_R4 = atof(datas[l][1]);         
     if(!strcmp("ALT_ACC",datas[l][0]))      ALT_ACC= atof(datas[l][1])      ;

     if(!strcmp("GPIO_DIR_ALT",datas[l][0])) GPIO_DIR_ALT=atoi(datas[l][1]);  
     if(!strcmp("GPIO_CLK_ALT",datas[l][0])) GPIO_CLK_ALT=atoi(datas[l][1]);  
     if(!strcmp("GPIO_SLP_ALT",datas[l][0])) GPIO_SLP_ALT=atoi(datas[l][1]);  
     if(!strcmp("GPIO_RST_ALT",datas[l][0])) GPIO_RST_ALT=atoi(datas[l][1]);  
     if(!strcmp("GPIO_MMM_ALT",datas[l][0])) GPIO_MMM_ALT=atoi(datas[l][1]);  
     if(!strcmp("GPIO_ENA_ALT",datas[l][0])) GPIO_ENA_ALT=atoi(datas[l][1]);  
     if(!strcmp("GPIO_M2_ALT",datas[l][0]))  GPIO_M2_ALT=atoi(datas[l][1]);  
     if(!strcmp("GPIO_M1_ALT",datas[l][0]))  GPIO_M1_ALT=atoi(datas[l][1]);  
     if(!strcmp("GPIO_M0_ALT",datas[l][0]))  GPIO_M0_ALT=atoi(datas[l][1]);  

     if(!strcmp("MCP_DIR_ALT",datas[l][0]))  MCP_DIR_ALT=atoi(datas[l][1])  ;  
     if(!strcmp("MCP_CLK_ALT",datas[l][0]))  MCP_CLK_ALT=atoi(datas[l][1])  ;  
     if(!strcmp("MCP_SLP_ALT",datas[l][0]))  MCP_SLP_ALT=atoi(datas[l][1])  ;  
     if(!strcmp("MCP_RST_ALT",datas[l][0]))  MCP_RST_ALT=atoi(datas[l][1])  ;  
     if(!strcmp("MCP_M2_ALT",datas[l][0]))   MCP_M2_ALT=atoi(datas[l][1])   ;  
     if(!strcmp("MCP_M1_ALT",datas[l][0]))   MCP_M1_ALT=atoi(datas[l][1])   ;  
     if(!strcmp("MCP_M0_ALT",datas[l][0]))   MCP_M0_ALT=atoi(datas[l][1])   ;  
     
     // donnees de azimut

     if(!strcmp("AZI_R1",datas[l][0]))       AZI_R1 = atof(datas[l][1])      ; 
     if(!strcmp("AZI_R2",datas[l][0]))       AZI_R2 = atof(datas[l][1])      ; 
     if(!strcmp("AZI_R3",datas[l][0]))       AZI_R3 = atof(datas[l][1])      ; 
     if(!strcmp("AZI_R4",datas[l][0]))       AZI_R4 = atof(datas[l][1])      ; 
     if(!strcmp("AZI_ACC",datas[l][0]))      AZI_ACC= atof(datas[l][1])      ;

     if(!strcmp("GPIO_DIR_AZI",datas[l][0])) GPIO_DIR_AZI=atoi(datas[l][1]) ; 
     if(!strcmp("GPIO_CLK_AZI",datas[l][0])) GPIO_CLK_AZI=atoi(datas[l][1]) ; 
     if(!strcmp("GPIO_SLP_AZI",datas[l][0])) GPIO_SLP_AZI=atoi(datas[l][1]) ;
     if(!strcmp("GPIO_RST_AZI",datas[l][0])) GPIO_RST_AZI=atoi(datas[l][1]);  
     if(!strcmp("GPIO_MMM_AZI",datas[l][0])) GPIO_MMM_AZI=atoi(datas[l][1]);  
     if(!strcmp("GPIO_ENA_AZI",datas[l][0])) GPIO_ENA_AZI=atoi(datas[l][1]);
     if(!strcmp("GPIO_M2_AZI",datas[l][0]))  GPIO_M2_AZI=atoi(datas[l][1])  ;
     if(!strcmp("GPIO_M1_AZI",datas[l][0]))  GPIO_M1_AZI=atoi(datas[l][1])  ; 
     if(!strcmp("GPIO_M0_AZI",datas[l][0]))  GPIO_M0_AZI=atoi(datas[l][1])  ; 

     if(!strcmp("MCP_DIR_AZI",datas[l][0]))  MCP_DIR_AZI=atoi(datas[l][1])   ;
     if(!strcmp("MCP_CLK_AZI",datas[l][0]))  MCP_CLK_AZI=atoi(datas[l][1])   ;
     if(!strcmp("MCP_SLP_AZI",datas[l][0]))  MCP_SLP_AZI=atoi(datas[l][1])   ;
     if(!strcmp("MCP_RST_AZI",datas[l][0]))  MCP_RST_AZI=atoi(datas[l][1])   ; 
     if(!strcmp("MCP_M2_AZI",datas[l][0]))   MCP_M2_AZI=atoi(datas[l][1])    ;
     if(!strcmp("MCP_M1_AZI",datas[l][0]))   MCP_M1_AZI=atoi(datas[l][1])    ;
     if(!strcmp("MCP_M0_AZI",datas[l][0]))   MCP_M0_AZI=atoi(datas[l][1])   ;

     // donnees de azimut et altitude (qui concernent les 2 en mm temps)
     
     if(!strcmp("ALTAZ_FORWARD",datas[l][0])) ALTAZ_FORWARD= atof(datas[l][1])      ;
     if(!strcmp("ALTAZ_REWIND",datas[l][0]))  ALTAZ_REWIND= atof(datas[l][1])      ;

     if(!strcmp("ALTAZ_FORWARD_FAST",datas[l][0])) ALTAZ_FORWARD_FAST= atof(datas[l][1])      ;
     if(!strcmp("ALTAZ_REWIND_FAST",datas[l][0]))  ALTAZ_REWIND_FAST= atof(datas[l][1])      ;

     // chemins des repertoires et fichiers (2021)

    if(!strcmp("CONFIG_REP_CAT",datas[l][0])) strcpy( CONFIG_REP_CAT, datas[l][1]) ;
    if(!strcmp("CONFIG_REP_CFG",datas[l][0])) strcpy( CONFIG_REP_CFG, datas[l][1]) ;
    if(!strcmp("CONFIG_REP_LOG",datas[l][0])) strcpy( CONFIG_REP_LOG, datas[l][1]) ;
    if(!strcmp("CONFIG_REP_IN",datas[l][0])) strcpy( CONFIG_REP_IN, datas[l][1]) ;
    if(!strcmp("CONFIG_FIC_LOG",datas[l][0])) strcpy( CONFIG_FIC_LOG, datas[l][1]) ;
    if(!strcmp("CONFIG_FIC_DATE",datas[l][0])) strcpy( CONFIG_FIC_DATE, datas[l][1]) ;
    if(!strcmp("CONFIG_FIC_HHMM",datas[l][0])) strcpy( CONFIG_FIC_HHMM, datas[l][1]) ;  

  }

  //if ( ALT_R == 0 ) ALT_R = ALT_R1 * ALT_R2 * ALT_R3 * ALT_R4 ;
  //if ( AZI_R == 0 ) AZI_R = AZI_R1 * AZI_R2 * AZI_R3 * AZI_R4 ;
  
  if ( ALT_R == 0 ) ALT_R = ALT_R1 * ALT_R2 * ALT_R3  ;
  if ( AZI_R == 0 ) AZI_R = AZI_R1 * AZI_R2 * AZI_R3  ;
}
//============================================================================
void   CONFIG_AFFICHER_VARIABLES(void) {

  TRACE("TEMPO_RAQ = %ld",  TEMPO_RAQ);
  TRACE("TEMPO_MENU = %ld",  TEMPO_MENU);
  TRACE("TEMPO_IR = %ld",  TEMPO_IR);
  TRACE("TEMPO_CLAVIER = %ld",  TEMPO_CLAVIER);
  TRACE("TEMPO_CAPTEURS = %ld",  TEMPO_CAPTEURS);

  TRACE("DONNEES_CONTROLEUR = %d",  DONNEES_CONTROLEUR);
  TRACE("DONNEES_CAPTEURS = %d",  DONNEES_CAPTEURS);
  TRACE("DONNEES_BLUETOOTH = %d",  DONNEES_BLUETOOTH);
  TRACE("DONNEES_INFRAROUGE = %d",  DONNEES_INFRAROUGE);
  TRACE("DONNEES_RAQUETTE = %d",  DONNEES_RAQUETTE);

  TRACE("ASTRE_PAR_DEFAUT = %s",  ASTRE_PAR_DEFAUT );
  TRACE("MODE_EQUATORIAL = %d",  MODE_EQUATORIAL);
  TRACE("MENU_PAR_DEFAUT = %d",  MENU_PAR_DEFAUT);

  TRACE("LATITUDE  = %f",          LATITUDE );
  TRACE("LONGITUDE = %f",          LONGITUDE );
  TRACE("ALTITUDE  = %f",          ALTITUDE );

  TRACE("GPIO_LED_ETAT = %d", GPIO_LED_ETAT );

  TRACE("ALT_R1 = %f",       ALT_R1);         
  TRACE("ALT_R2 = %f",       ALT_R2);
  TRACE("ALT_R3 = %f",       ALT_R3);         
  TRACE("ALT_R4 = %f",       ALT_R4);         
  TRACE("ALT_ROT = %d",      ALT_ROT);
  TRACE("ALT_ACC = %f",      ALT_ACC);

  TRACE("AZI_R1 = %f",       AZI_R1)      ; 
  TRACE("AZI_R2 = %f",       AZI_R2)      ; 
  TRACE("AZI_R3 = %f",       AZI_R3)      ; 
  TRACE("AZI_R4 = %f",       AZI_R4)      ; 
  TRACE("AZI_ROT = %d",      AZI_ROT)     ;
  TRACE("AZI_ACC = %f",      AZI_ACC)     ; 

  TRACE("CONFIG_REP_CAT = %s", CONFIG_REP_CAT)  ;
  TRACE("CONFIG_REP_CFG = %s", CONFIG_REP_CFG)  ; 
  TRACE("CONFIG_REP_LOG = %s", CONFIG_REP_LOG)  ; 
  TRACE("CONFIG_REP_IN = %s", CONFIG_REP_IN)  ; 
  TRACE("CONFIG_FIC_LOG = %s", CONFIG_FIC_LOG)  ; 
  TRACE("CONFIG_FIC_DATE = %s", CONFIG_FIC_DATE)  ; 
  TRACE("CONFIG_FIC_HHMM = %s", CONFIG_FIC_HHMM)  ;  

  TRACE("GPIO_ALT = %s", GPIO_ALT)  ;  
  TRACE("GPIO_AZI = %s", GPIO_AZI)  ;  
  TRACE("GPIO_MAS = %s", GPIO_MAS)  ;  
  TRACE("GPIO_FREQUENCE_PWM = %s", GPIO_FREQUENCE_PWM)  ;  

  TRACE1("anciennes variables\n");
  TRACE1("GPIO_RAQ_O   = %d",  GPIO_RAQ_O);
  TRACE1("GPIO_RAQ_E   = %d",  GPIO_RAQ_E);
  TRACE1("GPIO_RAQ_S   = %d",  GPIO_RAQ_S);
  TRACE1("GPIO_RAQ_N   = %d",  GPIO_RAQ_N);
  TRACE1("GPIO_RAQ_V   = %d",  GPIO_RAQ_V);
  TRACE1("GPIO_KEY_L1  = %d",  GPIO_KEY_L1);
  TRACE1("GPIO_KEY_L2  = %d",  GPIO_KEY_L2);
  TRACE1("GPIO_KEY_L3  = %d",  GPIO_KEY_L3);
  TRACE1("GPIO_KEY_L4  = %d",  GPIO_KEY_L4);
  TRACE1("GPIO_KEY_C1  = %d",  GPIO_KEY_C1);
  TRACE1("GPIO_KEY_C2  = %d",  GPIO_KEY_C2);
  TRACE1("GPIO_KEY_C3  = %d",  GPIO_KEY_C3);
  TRACE1("GPIO_KEY_C4  = %d",  GPIO_KEY_C4);
  TRACE1("GPIO_DIR_ALT = %d", GPIO_DIR_ALT);  
  TRACE1("GPIO_CLK_ALT = %d", GPIO_CLK_ALT);  
  TRACE1("GPIO_SLP_ALT = %d", GPIO_SLP_ALT);  
  TRACE1("GPIO_RST_ALT = %d", GPIO_RST_ALT);  
  TRACE1("GPIO_MMM_ALT = %d", GPIO_MMM_ALT);  
  TRACE1("GPIO_ENA_ALT = %d", GPIO_ENA_ALT);  
  TRACE1("GPIO_M2_ALT = %d",  GPIO_M2_ALT)  ;  
  TRACE1("GPIO_M1_ALT = %d",  GPIO_M1_ALT)  ;  
  TRACE1("GPIO_M0_ALT = %d",  GPIO_M0_ALT)  ;  
  TRACE1("GPIO_DIR_AZI = %d", GPIO_DIR_AZI) ; 
  TRACE1("GPIO_CLK_AZI = %d", GPIO_CLK_AZI) ; 
  TRACE1("GPIO_SLP_AZI = %d", GPIO_SLP_AZI) ;
  TRACE1("GPIO_RST_AZI = %d", GPIO_RST_AZI);  
  TRACE1("GPIO_MMM_AZI = %d", GPIO_MMM_AZI);  
  TRACE1("GPIO_ENA_AZI = %d", GPIO_ENA_AZI);  
  TRACE1("GPIO_M2_AZI = %d",  GPIO_M2_AZI)  ;
  TRACE1("GPIO_M1_AZI = %d",  GPIO_M1_AZI)  ; 
  TRACE1("GPIO_M0_AZI = %d",  GPIO_M0_AZI)  ; 
  TRACE1("=====================================================\n");
  TRACE1("MCP_DIR_AZI = %d",  MCP_DIR_AZI)   ;
  TRACE1("MCP_CLK_AZI = %d",  MCP_CLK_AZI)   ;
  TRACE1("MCP_SLP_AZI = %d",  MCP_SLP_AZI)   ;
  TRACE1("MCP_RST_AZI = %d",  MCP_RST_AZI)   ; 
  TRACE1("MCP_M2_AZI = %d",   MCP_M2_AZI)    ;
  TRACE1("MCP_M1_AZI = %d",   MCP_M1_AZI)    ;
  TRACE1("MCP_M0_AZI = %d",   MCP_M0_AZI)   ;
  TRACE1("MCP_DIR_ALT = %d",  MCP_DIR_ALT)  ;  
  TRACE1("MCP_CLK_ALT = %d",  MCP_CLK_ALT)  ;  
  TRACE1("MCP_SLP_ALT = %d",  MCP_SLP_ALT)  ;  
  TRACE1("MCP_RST_ALT = %d",  MCP_RST_ALT)  ;  
  TRACE1("MCP_M2_ALT = %d",   MCP_M2_ALT)   ;  
  TRACE1("MCP_M1_ALT = %d",   MCP_M1_ALT)   ;  
  TRACE1("MCP_M0_ALT = %d",   MCP_M0_ALT)   ;  
}
//============================================================================
int CONFIG_GETCWD(char * c_getcwd) {

  if (getcwd(c_getcwd, sizeof(c_getcwd)) != NULL) {
     TRACE("Current working dir: %s\n", c_getcwd);
  } else {
     perror("getcwd() error");
     return 1;
  }
  return 0 ;
} 
//============================================================================

int CONFIG_READ(char datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) {

  FILE *fin ;
  char buf   [CONFIG_TAILLE_BUFFER] ;
  char buffer[CONFIG_TAILLE_BUFFER] ;
  char c ;
  int i,C,L ;

  // FIXME : initialisation du tableau de valeurs lues dans le fichier = datas

  printf(" DATAS_NB_LIGNES=%d\n",   DATAS_NB_LIGNES) ;
  printf(" DATAS_NB_COLONNES=%d\n", DATAS_NB_COLONNES) ;

  for(L=0;L<DATAS_NB_LIGNES;L++) {
    for(C=0;C<DATAS_NB_COLONNES;C++) { 
      memset(datas[L][C],ZERO_CHAR,CONFIG_TAILLE_BUFFER-1);
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
   printf("probleme ouverture %s\n",buf) ; 
   exit(2) ;
  }
  else printf("open %s ok\n", buf) ;

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
      memset(datas[L][C],ZERO_CHAR,CONFIG_TAILLE_BUFFER);
      strcpy(datas[L][C],buf);
      printf("datas[%d][%d]=%s\n",L,C,datas[L][C] );
      C++;
    }
    if ((CONFIG_FIN_LIGNE(c)&& i!=0)||i==CONFIG_TAILLE_BUFFER) {
      memset(datas[L][C],ZERO_CHAR,CONFIG_TAILLE_BUFFER);
      strcpy(datas[L][C],buf);
      printf("datas[%d][%d]=%s\n",L,C,datas[L][C] );
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
//============================================================================
void CONFIG_AFFICHER_DATAS(char datas[DATAS_NB_LIGNES][DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER]) {
  int L, C ;  

  for(L=0;L<DATAS_NB_LIGNES;L++) {
    for(C=0;C<DATAS_NB_COLONNES;C++) { 
      if ( C>0 )printf(" ") ;
      if (strlen(datas[L][C])) printf("%s",datas[L][C]) ;
    }
    printf("\n") ;
  }
/*
  for(l=0;l<DATAS_NB_LIGNES;l++) {

     memset( buffer,ZERO_CHAR, CONFIG_TAILLE_BUFFER * DATAS_NB_COLONNES) ;

     for(c=0;c<DATAS_NB_COLONNES;c++) {
      sprintf( buffer, "%-10s %-10s", buffer, datas[l][c] ) ;
     }
     if (strlen(buffer)>1) {
      TRACE("%s",buffer) ;
     }
  }
*/
}
//---------------------------------------------------------------------------------------
void CONFIG_AFFICHER_LIEU(LIEU *lieu) {
  
  TRACE("lat %.2f lon %.2f alt %.2f JJ %.2f TS %.2f TSR %.2f JD %.2f",\
          lieu->lat * DEGRES,\
          lieu->lon * DEGRES,\
          lieu->alt , lieu->JJ,
          lieu->TS,\
          lieu->TSR,\
          lieu->JD ) ;

  TRACE1("lieu->lat (degres) = %f",lieu->lat * DEGRES ) ;
  TRACE1("lieu->lon (degres) = %f",lieu->lon * DEGRES ) ;
  TRACE1("lieu->alt = %f",lieu->alt ) ;
  TRACE1("lieu->JJ  = %f",lieu->JJ) ;
  TRACE1("lieu->TS  = %f",lieu->TS) ;
  TRACE1("lieu->TSR = %f",lieu->TSR) ;
  TRACE1("lieu->JD  = %f",lieu->JD) ;
}
//---------------------------------------------------------------------------------------
void CONFIG_AFFICHER_TEMPS(TEMPS *temps) {
  
  TRACE("%d %d %d %d %d %d : %f", temps->yy, temps->mm, temps->dd, temps->HH, temps->MM, temps->SS, temps->hd ) ;

  TRACE1("temps->yy = %d", temps->yy ) ;
  TRACE1("temps->mm = %d", temps->mm ) ;
  TRACE1("temps->dd = %d", temps->dd ) ;
  TRACE1("temps->HH = %d", temps->HH ) ;
  TRACE1("temps->MM = %d", temps->MM ) ;
  TRACE1("temps->SS = %d", temps->SS ) ;
  TRACE1("temps->hd = %f", temps->hd ) ;
}
//---------------------------------------------------------------------------------------
void CONFIG_AFFICHER_CLAVIER(CLAVIER *clavier) {
  
  TRACE("phrase %s mot %s symbole %s nombre %s premier %s valider %s menu %s",\
    clavier->phrase,\
    clavier->mot,\
    clavier->symbole,\
    clavier->nombre,\
    clavier->premier,\
    clavier->valider,\
    clavier->menu) ;

  TRACE1("clavier->mot         = %s",clavier->mot) ;
  TRACE1("clavier->premier     = %s",clavier->premier) ;
  TRACE1("clavier->phrase      = %s",clavier->phrase) ;
  TRACE1("clavier->nombre      = %s",clavier->nombre) ;
  TRACE1("clavier->symbole     = %s",clavier->symbole) ;
  TRACE1("clavier->phrase_lue  = %d",clavier->phrase_lue) ;
}
//---------------------------------------------------------------------------------------
void CONFIG_AFFICHER_ASTRE(ASTRE *as) {
    
  TRACE(" ASTRE       : %10s Va = %.2f Vh = %.2f", as->nom, as->Va,  as->Vh ) ; 
  
  TRACE(" azimut      : %.2f (degres) : %d.%d (hh.mm)", astre->a * DEGRES, (astre->at).HH, (astre->at).MM ) ;
  TRACE(" altitude    : %.2f (degres) : %d.%d (hh.mm)", astre->h * DEGRES, (astre->ht).HH, (astre->ht).MM ) ;
  TRACE(" ang-horaire : %.2f (degres) : %d.%d (hh.mm)", astre->A * DEGRES, (astre->At).HH, (astre->At).MM ) ;
  TRACE(" declinaison : %.2f (degres) : %d.%d (hh.mm)", astre->H * DEGRES, (astre->Ht).HH, (astre->Ht).MM ) ;
  
  TRACE2("astre->AZI  = %f",astre->AZI) ;
  TRACE2("astre->AZI1 = %f",astre->AZI1) ;
  TRACE2("astre->ASC  = %f",astre->ASC) ;
  TRACE2("astre->ASC1 = %f",astre->ASC1) ;
  TRACE2("astre->ASC2 = %f",astre->ASC2) ;
  TRACE2("astre->ASC = %f - %f (degres)",astre->ASC , astre->ASC * DEGRES) ;
}
//============================================================================
void CONFIG_AFFICHER_VOUTE( VOUTE * voute) {
	
	TRACE("voute->num %lld", voute->num) ;
}
//============================================================================
void CONFIG_AFFICHER_TOUT(CLAVIER *clavier, TEMPS *temps, LIEU *lieu, ASTRE *astre, VOUTE *voute) {

  CONFIG_AFFICHER_CLAVIER( clavier ) ;   
  CONFIG_AFFICHER_TEMPS( temps ) ;
  CONFIG_AFFICHER_LIEU( lieu );
  CONFIG_AFFICHER_ASTRE( astre ) ;
  CONFIG_AFFICHER_VOUTE( voute ) ;
}
//============================================================================
// 15/11/2021 : modification ordre
void CONFIG_AFFICHER_CHANGEMENTS (SUIVI *suivi) {

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

    TRACE("appel : %d : %s" , suivi->menu,s_menu) ;

   GPIO_CLIGNOTE(GPIO_LED_ETAT, 1, 100) ;

//    GPIO_CLIGNOTE(GPIO_LED_ETAT, suivi->menu, 2) ;
  }
  return ;
}
//============================================================================



