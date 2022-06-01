/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# Fichier     | arguments.c
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 19/01/2022  | * ajout entete
#               * ajouts fonctions utilisant getopt
# 11/03/2022  | * prise en compte option -p (facon classique)
#               * renseignement CONFIG_REP_HOME par -p <path>
# 11/03/2022  | * ajout fonction ARGUMENTS_GERER_REP_HOME
# -------------------------------------------------------------- 
*/

#include <arguments.h>

void ARGUMENTS_VOUTE(void) {
  
  int i_retour=0 ;
  FILE   *fout ;
  
  double a, h ;
  
  if ( ( fout=fopen("voute.csv","w")) == NULL  ) {
    SyslogErr("probleme ouverture voute.csv") ;
    exit(2);
  } 
  
  /* -------------------------------------------
  *  On fait varier les coordonnees horaires / declinaison
  * avec un pas = gp_Vout->pas
  * -------------------------------------------*/
  
  for(h=-(PI/2)+(gp_Lieu->lat)+0.001;h<PI/2;h+=gp_Vout->pas)
    if (h>=0) 

    /* -------------------------------------------
    *  On fait varier les coordonnees en ascension droite
    * avec un pas = gp_Vout->pas
    * -------------------------------------------*/

    for(a=-PI +0.001 ;a<PI;a+=gp_Vout->pas){
     
     gp_Astr->a=a ;
     gp_Astr->h=h ;
     
     CALCUL_EQUATEUR  ( gp_Lieu, gp_Astr) ;        // calcul coordonnees horaires en fait
     CALCUL_VITESSES  ( gp_Lieu, gp_Astr, gp_Sui) ; // TODO : verifier gp_Sui->SUIVI_EQUATORIAL avant
     
     /* Calcul de la norme de la vitesse */

     gp_Astr->V  = sqrt( sqr( gp_Astr->Va ) + sqr( gp_Astr->Vh )) ;
     
     if ( gp_Astr->Va != 0) 
       gp_Astr->An = atan( gp_Astr->Vh / gp_Astr->Va ) ;
     else
       gp_Astr->An = PI/2 ;
     
     CALCUL_GEODE( gp_Astr ) ;
     
     Trace1("%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f", \
       gp_Astr->a * DEGRES, \
       gp_Astr->h * DEGRES, \
       gp_Astr->ASC * DEGRES, \
       gp_Astr->DEC * DEGRES, \
       gp_Astr->x , \
       gp_Astr->y, \
       gp_Astr->z, \
       gp_Astr->Va, \
       gp_Astr->Vh, \
       gp_Astr->V, \
       gp_Astr->An ) ;
     
     Trace1("xx %.1f yy %.1f zz %.1f Va %.1f Vh %.1f V %.1f", \
       gp_Astr->xx , \
       gp_Astr->yy , \
       gp_Astr->zz , \
       gp_Astr->Va, \
       gp_Astr->Vh, \
       gp_Astr->V ) ;
     
   }
  
  fclose(fout) ;
}

/*****************************************************************************************
* @fn     : ARGUMENTS_VARIABLES
* @author : s.gravois
* @brief  : point entree su programme
* @param  : const char *fmt
* @param  :  ...
* @date   : 2022-01-19 creation entete de la fonction au format doxygen
* @todo   : (obsolete voir dangereux a utiliser en embarque)
*           => cf article sur le WEB (va_args / embedded)
* @todo   : (obsolete) la bonne pratique est de passer par getopt 
*****************************************************************************************/

void ARGUMENTS_VARIABLES(const char *fmt, ... ) {
    int d;
    char c, *s; 
    
    va_list ap;
    va_start(ap, fmt);
    
    while(*fmt) { 
       switch ((int)*fmt ++) {
        case ARGUMENTS_VARG_STRING :            
            s = va_arg (ap, char *);
            printf("arg chaine = %s\n", s);
            break;
        case ARGUMENTS_VARG_INT :             
            d = va_arg (ap, int);
            printf("arg int = %d\n", d);
            break;
	      case ARGUMENTS_VARG_CHAR :             
            c = (char)va_arg (ap, int);
            printf("arg char = %c\n", c);
            break;
        }
    }
    va_end(ap) ;
}

/*****************************************************************************************
* @fn     : GPIO_TEST_MOTEURS
* @author : s.gravois
* @brief  : fonction de test simple des moteurs
* @param  : 
* @date   : 2022-01-19 creation entete de la fonction au format doxygen
* @todo   : completer et deplacer dans gpio.c / gpio.h
*****************************************************************************************/

void  GPIO_TEST_MOTEURS(void ) {

  int i ;
  struct timeval t00,t01 ;
  long   t_diff , nb_puls;
  double t_diff_sec,  t_diff_moy ;
  double f_fin, f_deb, delai ;
  double FREQ[5][3] = { \
    {10,  100,   3}, \
    {100, 1000,  3}, \
    {1000,5000,  3}, \
    {5000,10000, 3}, \
    {10000,20000, 3}, \
  } ;  
  // signal(SIGTERM,TRAP_SUIVI_TEST) ; 
  
  printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gpio_in,gpio_out)) ;
    
  GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
  GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
  
  gettimeofday(&t00,NULL) ;
  
  i=0;
  delai=0.0 ;
  t_diff_moy = 0.0 ;
  
  GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
  GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
    
  printf("%-10s%-10s%-10s%-10s%-10s%-10s\n","freq_deb","freq_fin","delai","constate","diff","nbpuls") ;
  while (i<5) {
    
    f_deb = FREQ[i][0] ;
    f_fin = FREQ[i][1] ;
    delai = FREQ[i][2] ;
  
    nb_puls = GPIO_ACCELERATION_2( GPIO_CLK_ALT, GPIO_CLK_AZI, f_deb, f_fin, delai, gp_Sui->l_NANO_MOINS) ;
    
    gettimeofday(&t01,NULL) ;

    t_diff = (( t01.tv_sec - t00.tv_sec ) * GPIO_MICRO_SEC) + t01.tv_usec - t00.tv_usec ;
    t_diff_sec = (double)t_diff / (double)GPIO_MICRO_SEC ;
    gettimeofday(&t00,NULL) ;
    printf("%4.4f%2s%4.4f%2s%4.4f%2s%4.4f%2s%4.4f%2s%10ld\n",f_deb,"", f_fin,"", delai,"", t_diff_sec,"", t_diff_sec-delai,"",nb_puls ) ;
    t_diff_moy += t_diff_sec -  delai ;
    i++ ;
  }
  printf("Moyenne des differences = %2.4f\n",  t_diff_moy/i ) ;
  
  GPIO_SET_ALT( 0,0,0,0,1,1 ) ;
  GPIO_SET_AZI( 0,0,0,0,1,1 ) ;
  
  printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;
  
  GPIO_STATUT() ;
}
/*****************************************************************************************
* @fn     : ARGUMENTS_HELP
* @author : s.gravois
* @brief  : affiche l aide du programme facon "classique"
* @param  : int argc
* @param  : char ** argv
* @date   : 2022-01-19 creation entete de la fonction au format doxygen
* @todo   : a completer par passage par getopt
*****************************************************************************************/

void ARGUMENTS_HELP(int argc, char** argv) {

  char binaire[255] ;
  
  memset(binaire, ZERO_CHAR,sizeof(binaire)) ;
  strcpy(binaire,argv[0]) ;
  
  /* janvier 2022 : ajout argument pour afficher les paramatres 
       de la planete passee avec son numero en argument
  */

  if ( argc == 2 ) {

    if (!strcmp("-h",argv[1]) || !strcmp("--help",argv[1])) {

      printf("%s : USAGE : \n",binaire) ;
      printf("%s -a / ast (gp_Astr) <nom>      : calcule tout et affiche tout en fonction de l'gp_Astr\n",binaire) ;
      printf("%s -A / AST (gp_Astr) <nom>      : idem -a mais continue et effectue le suivi\n",binaire) ;
      printf("%s -L / ala (aLarm)            : arrete le main aubout de argv[1] secondes\n",binaire) ;
      printf("%s -z / azi (aZimut) <A> <H>   : <angle horaire(deg) <declinaison(deg)> : calcul l'azimut en fonction de AH et DEC\n",binaire) ;
      printf("%s -q / equ (eQuateur) <a> <h> : <azimut(deg) <hauteur(deg)>            : calcul l'angle horaire et la declinaison en fonction de azimut et altitude\n",binaire) ;
      printf("%s ast+equ                     : combinaison ast + equ , l idee est de retouver le calcul des vitesses en sens inverse", binaire) ;
      printf("%s -p / pla (planete)          : afficher les calculs concernant la planete (SOLAR_SYSTEM)\n",binaire) ;
      printf("%s -t / tou (tout)             : calcule et affiche toutes les caracteristiques\n", binaire) ;      
      printf("%s -m / mot (moteurs)          : effectue un test simple sur les moteurs\n",binaire) ;
      printf("%s -v / vou (voute) <r(deg)> <l>  : genere un fichier de la voute entiere : voute.csv (resolution,latitude))\n",binaire) ;
      
      printf("\n") ;
      
      printf("%s -c / cal0 (calibration)      : \n \
       <sens> <nombre de tours par minute> <nombre de tours> <atl/azi> : \n \
       deplace les moteurs en fonction des 3 args suivants en altitude ou \n \
       bien en azimut en fonction de la configuration \n",binaire) ;

      printf("%s -C / cal1 (calibration)      : \n \
       <sens> <nombre de tours par minute> <nombre de tours> <atl/azi> \n \
       idem calib mais gerer par les threads (et donc un suivi meilleur de la periode SIC)\n",binaire) ;

      printf("%s -g / gpios (gpios) : affiche le statut des broches GPIO\n",binaire) ;

      printf("%s <LAT> <DEC> <deb voute> <fin voute> <pas voute> <acc voute>\n",binaire);

      exit(0) ;
    }
  }
}

/*****************************************************************************************
* @fn     : ARGUMENTS_GERER_REP_HOME
* @author : s.gravois
* @brief  : Gere le passage des arguments de facon classique
* @param  : int     argc
* @param  : char ** argv
* @date   : 2022-03-11 creation
* @todo   : fusioner avec ARGUMENTS_GERER_FACON_CLASSIQUE
*****************************************************************************************/

void ARGUMENTS_GERER_REP_HOME(int argc, char** argv) {
  /* ---------------------------------------------------------------
  * Gestion d un chemin externe (option -p <path>) si getcwd KO
  * ---------------------------------------------------------------*/

  if ( ( argc == 3 ) &&  ! strcmp( "-p" , argv[1]) ) {
  
    Trace1("option -p = CONFIG_REP_HOME %s pris en compte\n",argv[2]);

    memset( CONFIG_REP_HOME, 0, sizeof(CONFIG_REP_HOME)) ;
    strcpy( CONFIG_REP_HOME, argv[2] ) ;
  }
  else {
    Trace1("pas de option -p => lecture CONFIG_REP_HOME avec getcwd");

    if ( getcwd(CONFIG_REP_HOME, sizeof(CONFIG_REP_HOME)) == NULL ) {
      SyslogEno("getcwd") ;
    }
    else {
      Trace1("CONFIG_REP_HOME lu par getcwd = %s",CONFIG_REP_HOME);
    }
  }
  Trace("CONFIG_REP_HOME = %s",CONFIG_REP_HOME);
}
/*****************************************************************************************
* @fn     : ARGUMENTS_GERER
* @author : s.gravois
* @brief  : Gere le passage des arguments de facon classique
* @param  : int     argc
* @param  : char ** argv
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void ARGUMENTS_GERER_FACON_CLASSIQUE(int argc, char** argv) {
  
  long c_ir_0 = 0 , c_ir_1 = 0 ;
  int  ir_old, ir ;
  double periode ;
  double nbpulse ;
  
  incrlog=30 ;

  /* ---------------------------------------------------------------
  * Gestion de l Alarme
  * ---------------------------------------------------------------*/

  if ( ( argc == 3 ) &&  ! strcmp("ala",argv[1]) ) {
    
    gp_Sui->alarme=atoi(argv[2]) ;
    Trace("ala = %d\n",gp_Sui->alarme) ;
  }
  /* ---------------------------------------------------------------
  * Gestion d un as en particulier : uniquement calcul et aff
  * ---------------------------------------------------------------*/

  if ( ( argc == 3 ) &&  ! strcmp("ast",argv[1]) ) {
  
    Trace("as %s pris en compte\n",argv[2]);
    memset( gp_Astr->nom, 0, sizeof(gp_Astr->nom)) ;
    strcpy( gp_Astr->nom, argv[2] ) ;

    /* Recherche de l'as dans les catalogues */
    
    if ( strstr( gp_Astr->nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Astr, cat_dec) ;
    if ( strstr( gp_Astr->nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Astr, cat_dec) ;
    if ( strstr( gp_Astr->nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Astr, etoiles_dec) ;

    CALCUL_TOUT() ;
    CONFIG_AFFICHER_TOUT() ;   

    exit(0) ;
  }
  /* ---------------------------------------------------------------
  * Gestion d un as en particulier : calcul, affichage et suivi
  * ---------------------------------------------------------------*/

  if ( ( argc == 3  ) &&  ! strcmp("AST",argv[1]) ) {
  
    Trace("as %s pris en compte\n",argv[2]);
    memset( gp_Astr->nom, 0, sizeof(gp_Astr->nom)) ;
    strcpy( gp_Astr->nom, argv[2] ) ;

    /* Recherche de l'as dans les catalogues */
    
    if ( strstr( gp_Astr->nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Astr, cat_dec) ;
    if ( strstr( gp_Astr->nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Astr, cat_dec) ;
    if ( strstr( gp_Astr->nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Astr, etoiles_dec) ;

    CALCUL_TOUT() ;
    CONFIG_AFFICHER_TOUT() ;   
  }
  /* ---------------------------------------------------------------
  * Gestion du calcul de l azimut en fonction 
  * des coordonnes equatoriales
  * ---------------------------------------------------------------*/

  if ( ( argc == 4 ) &&  ! strcmp("azi",argv[1]) ) {

    Trace("as nom mis a la valeur AZI0");
    memset( gp_Astr->nom, 0, sizeof(gp_Astr->nom)) ;
    strcpy( gp_Astr->nom, "AZI0" ) ;

    memset( gp_Astr->infos, 0, sizeof(gp_Astr->nom)) ;
    strcpy( gp_Astr->infos, "calcul volontaire : equatorial => azimutal" ) ;

    gp_Astr->ASC = atof(argv[2]) / DEGRES ;
    gp_Astr->DEC = atof(argv[3]) / DEGRES ;
    
    CALCUL_TOUT() ;
    CONFIG_AFFICHER_TOUT() ; 
	   
    exit(0) ;
  }
  /* ---------------------------------------------------------------
  * Gestion du calcul des coordonnees equatoriales en fonction 
  * des coordonnes azimutales et du gps (date heure lieu etc..)
  * ---------------------------------------------------------------*/

  if ( ( argc == 4 ) &&  ! strcmp("equ",argv[1]) ) {

    Trace("as nom mis a la valeur EQU0");
    memset( gp_Astr->nom, 0, sizeof(gp_Astr->nom)) ;
    strcpy( gp_Astr->nom, "EQU0" ) ;

    memset( gp_Astr->infos, 0, sizeof(gp_Astr->nom)) ;
    strcpy( gp_Astr->infos, "calcul volontaire : azimutal => equatorial" ) ;

    gp_Astr->a = atof(argv[2]) / DEGRES ;
    gp_Astr->h = atof(argv[3]) / DEGRES ;
    
    CALCUL_TOUT() ;
    CONFIG_AFFICHER_TOUT() ; 
	   
    exit(0) ;
  }
    /* ---------------------------------------------------------------
  * Gestion du calcul successif suivants : 
  * 1 - mode "ast" => recuperation des coordonnes / calcul altaz / vitesses
  * 2 - mode "equ" => recalcul coordonnes horaires / 
  * ---------------------------------------------------------------*/

  if ( ( argc == 3 ) &&  ! strcmp("ast+equ",argv[1]) ) {

    /* mode ast */

    Trace("as %s pris en compte\n",argv[2]);
    memset( gp_Astr->nom, 0, sizeof(gp_Astr->nom)) ;
    strcpy( gp_Astr->nom, argv[2] ) ;
    
    if ( strstr( gp_Astr->nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Astr, cat_dec) ;
    if ( strstr( gp_Astr->nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Astr, cat_dec) ;
    if ( strstr( gp_Astr->nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Astr, etoiles_dec) ;

    CALCUL_TOUT() ;
    CONFIG_AFFICHER_TOUT() ;   

    /* mode equateur */

    Trace("as nom mis a la valeur EQU0");
    memset( gp_Astr->nom, 0, sizeof(gp_Astr->nom)) ;
    strcpy( gp_Astr->nom, "EQU0" ) ;
    
    CALCUL_TOUT() ;
    CONFIG_AFFICHER_TOUT() ; 

    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 2  ) &&  ! strcmp("tou",argv[1]) ) {
  
    Trace("passage en mode azimutal : gp_Sui->SUIVI_EQUATORIAL=0") ;
    gp_Sui->SUIVI_EQUATORIAL=0 ;

    Trace("as nom mis a la valeur TST0");
    memset( gp_Astr->nom, 0, sizeof(gp_Astr->nom)) ;
    strcpy( gp_Astr->nom, "AZI0" ) ;

    CALCUL_TOUT() ;
    CONFIG_AFFICHER_TOUT() ;    

    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 2  ) &&  ! strcmp("sto",argv[1]) ) {
 
     TRAP_MAIN(1) ; 
     exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( argc == 7 ) {

    gp_Lieu->lat   = atof(argv[1]) / DEGRES ;
    gp_Astr->DEC    = atof(argv[2]) / DEGRES ;
    gp_Vout->deb  = atof(argv[3]) ;
    gp_Vout->fin  = atof(argv[4]) ;
    gp_Vout->pas  = atof(argv[5]) ;
    gp_Vout->acc  = atof(argv[6]) ;

    CALCUL_TOUT() ;
    CONFIG_AFFICHER_TOUT() ;   
  }
  // -----------------------------------------------------------------
  if ( ( argc == 4  ) &&  ! strcmp("vou",argv[1]) ) {
    /*
    printf("Generation d'un fichier de quadrillage de la voute avec une resolution de %f degres pour la latitude %d",\
     atof(argv[2]),\
     atof(argv[3])) ;
    
    printf("Champs du fichier produit =\n \
     <altitude>\n\
     <azimut>\n\
     <angle horaire>\n\
     <declinaison>\n\
     <vitess en azimut>\n\
     <vitesse en altitude>\n\
     <norme vitesse>\n\
     <angle du vecteur vitesse>\n") ;
    */
    gp_Vout->pas = atof(argv[2]) / DEGRES ;
    gp_Lieu->lat  = atof(argv[3]) / DEGRES ; 
    
    ARGUMENTS_VOUTE( ) ;
    exit(0) ;
  }
  /* -----------------------------------------------------------------
    Teste les moteurs 
  */
  if ( ( argc == 3 ) &&  ! strcmp("mot",argv[1]) ) {
  
    gp_Sui->l_NANO_MOINS = atol( argv[2]) ;
    
    GPIO_TEST_MOTEURS(  ) ;
    exit(0) ;
  }
  // -----------------------------------------------------------------

  // -----------------------------------------------------------------
  

  // -----------------------------------------------------------------
  

  // -----------------------------------------------------------------
  
  // -----------------------------------------------------------------
  if ( ( argc == 2 ) &&  ! strcmp("sta",argv[1]) ) {
    
    printf("ARGUMENTS_GERER : GPIO_STATUT demander\n") ;
    GPIO_STATUT() ;
    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 6 ) &&  ! strcmp("cal0",argv[1]) ) {
  
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;
    printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gpio_in,gpio_out)) ;
    
    if ( ! strcmp("alt", argv[5]) ) {

     printf("reduction totale altitude  = %f\n", ALT_R) ;
     GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * ALT_R2 * ALT_R3 * ALT_R4  ;
     periode = 60 / ( ALT_R2 * ALT_R3 * ALT_R4 * atof(argv[3]) ) ;
     printf("deplacement moteur en altitude Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_1( atoi(argv[2]), periode, nbpulse , GPIO_DIR_ALT, GPIO_CLK_ALT) ;

     GPIO_SET_ALT( 0,0,0,0,0,0 ) ;
    }
    if ( ! strcmp("azi", argv[5]) ) {

     printf("reduction totale azimut    = %f\n", AZI_R) ; 
     GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * AZI_R2 * AZI_R3 * AZI_R4   ;
     periode = 60 / ( AZI_R2 * AZI_R3 * AZI_R4 * atof(argv[3]) ) ;
     printf("deplacement moteur en azimut Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_1( atoi(argv[2]), periode, nbpulse , GPIO_DIR_AZI, GPIO_CLK_AZI) ;

     GPIO_SET_AZI( 0,0,0,0,0,0 ) ;
    }
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;
    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 6 ) &&  ! strcmp("calib2",argv[1]) ) {
  
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;
    printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gpio_in,gpio_out)) ;
    
    if ( ! strcmp("alt", argv[5]) ) {
     printf("reduction totale altitude  = %f\n", ALT_R) ;
     GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * ALT_R2 * ALT_R3 * ALT_R4  ;
     periode = 60 / ( ALT_R2 * ALT_R3 * ALT_R4 * atof(argv[3]) ) ;
     printf("deplacement moteur en altitude Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_3( atoi(argv[2]), periode, nbpulse , GPIO_DIR_ALT, GPIO_CLK_ALT) ;

     GPIO_SET_ALT( 0,0,0,0,0,0 ) ;
    }
    if ( ! strcmp("azi", argv[5]) ) {
     printf("reduction totale azimut    = %f\n", AZI_R) ; 
     GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * AZI_R2 * AZI_R3 * AZI_R4   ;
     periode = 60 / ( AZI_R2 * AZI_R3 * AZI_R4 * atof(argv[3]) ) ;
     printf("deplacement moteur en azimut Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_3( atoi(argv[2]), periode, nbpulse , GPIO_DIR_AZI, GPIO_CLK_AZI) ;

     GPIO_SET_AZI( 0,0,0,0,0,0 ) ;
    }
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;
    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 5 ) &&  ! strcmp("calibt",argv[1]) ) {
  
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;
    printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gpio_in,gpio_out)) ;
    
     printf("reduction totale altitude  = %f\n", ALT_R) ;
     GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * ALT_R2 * ALT_R3 * ALT_R4  ;
     periode = 60 / ( ALT_R2 * ALT_R3 * ALT_R4 * atof(argv[3]) ) ;
     printf("deplacement moteur en altitude Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;
     
     gp_Sui->Th = periode ;
    
     printf("reduction totale azimut    = %f\n", AZI_R) ; 
     GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * AZI_R2 * AZI_R3 * AZI_R4   ;
     periode = 60 / ( AZI_R2 * AZI_R3 * AZI_R4 * atof(argv[3]) ) ;
     printf("deplacement moteur en azimut Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;
     
     gp_Sui->Ta = periode ;
  }    
  /* (obsolete)
  if ( ( argc == 2 ) &&  ! strcmp("ir",argv[1]) ) {
  
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gpio_in,gpio_out)) ;
    printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gpio_in,gpio_out)) ;
    
    //gettimeofday(&t00,NULL) ;
    ir_old=1 ;
    ir=1;
    while(1) {
      //usleep( (long)(1000000/38000.0)) ;
      //gettimeofday(&t01,NULL) ;
      //t_diff = (( t01.tv_sec - t00.tv_sec ) * GPIO_MICRO_SEC) + t01.tv_usec - t00.tv_usec ;
      //if ( (double)t_diff >= GPIO_MICRO_SEC / ARGUMENTS_FREQ_TRAME_INFRAROUGE ) {
      //	gettimeofday(&t00,NULL) ;
      ir = GPIO_GET(21) ;
      if ( ir == 0 && ir_old == 0 ) { c_ir_0++ ; }
      if ( ir == 1 && ir_old == 1 ) { c_ir_1++ ; }
      if ( ir == 0 && ir_old == 1 ) { printf("%ld ",c_ir_1) ; if(c_ir_1>1000) printf("\n"); c_ir_0 = 1 ; }
      if ( ir == 1 && ir_old == 0 ) { printf("%ld ",c_ir_0) ; if(c_ir_0>1000) printf("\n"); c_ir_1 = 1 ; }
      ir_old = ir ;
      // t_diff=0 ;
      //}
    }
    exit(0) ;
  }
  */
  // -----------------------------------------------------------------
  TRACE("as %s pris en compte",argv[1]);
  printf("fin %s\n","ARGUMENTS_GERER") ;
}

/*****************************************************************************************
* @fn     : ARGUMENTS_GERER
* @author : s.gravois
* @brief  : Gere le passage des arguments de facon classique
* @param  : int     argc
* @param  : char ** argv
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void ARGUMENTS_GERER_GETOPT(int argc, char** argv) {

  char c=0 ;

  while ((c = getopt (argc, argv, "a:A:Lq:f:l:")) != -1) {
      switch (c) {
     
        case 'L':
          gp_Sui->alarme=atoi(argv[2]) ;
          Trace("ARGUMENTS_GERER : ala = %d\n",gp_Sui->alarme) ;
        break ;
        
        case 'a' :
          Trace("as %s pris en compte\n",optarg);
          memset( gp_Astr->nom, 0, sizeof(gp_Astr->nom)) ;
          strcpy( gp_Astr->nom, optarg ) ;

          CALCUL_TOUT() ;
          CONFIG_AFFICHER_TOUT() ;   

          exit(0) ;
        break ;
        
        case 'A' :

          Trace("as %s pris en compte\n",optarg);
          memset( gp_Astr->nom, 0, sizeof(gp_Astr->nom)) ;
          strcpy( gp_Astr->nom, optarg ) ;

          CALCUL_TOUT() ;
          CONFIG_AFFICHER_TOUT() ;   

          exit(0) ;

        break ;
/*
        case '' : 
        break ;

        case '' : 
        break ;

        case '' : 
        break ;

        case '' : 
        break ;
*/
        default :
        break ;
      }
  }
}

//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================
