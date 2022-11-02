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
#               * renseignement gp_Con_Par->par_rep_home par -p <path>
# 11/03/2022  | * ajout fonction ARGUMENTS_GERER_REP_HOME
# -------------------------------------------------------------- 
*/

#include "astro_arguments.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

// extern ASTRO_TRAP_MAIN();

/*****************************************************************************************
* @fn     : ARGUMENTS_VOUTE
* @author : s.gravois
* @brief  : 
* @param  : const char *fmt
* @param  :  ...
* @date   : 2022-01-19 creation entete de la fonction au format doxygen
* @todo   : (obsolete voir dangereux a utiliser en embarque)
*           => cf article sur le WEB (va_args / embedded)
* @todo   : (obsolete) la bonne pratique est de passer par getopt 
*****************************************************************************************/

void ARGUMENTS_VOUTE( void) {
  
  char cmd[64]={0} ;
  int i_retour=0 ;
  FILE   *fout ;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  double a, h ;
  
  if ( ( fout=fopen( ARGUMENTS_FILE_VOUTE ,"w")) == NULL  ) {
    sprintf(cmd,"probleme ouverture %s", ARGUMENTS_FILE_VOUTE) ;
    SyslogErr(cmd) ;
    exit(2);
  } 

  /* -------------------------------------------
  *  On fait varier les coordonnees horaires / declinaison
  * avec un pas = gp_Vou->pas
  * -------------------------------------------*/
  
  for(h=-(M_PI/2)+(gp_Lie->lat)+0.001;h<M_PI/2;h+=gp_Vou->pas)
    if (h>=0) 

    /* -------------------------------------------
    *  On fait varier les coordonnees en ascension droite
    * avec un pas = gp_Vou->pas
    * -------------------------------------------*/

    for(a=-M_PI +0.001 ;a<M_PI;a+=gp_Vou->pas){
     
     gp_Ast->a=a ;
     gp_Ast->h=h ;
     
     CALCUL_EQUATEUR  ( gp_Lie, gp_Ast) ;        // calcul coordonnees horaires en fait
     CALCUL_VITESSES  ( gp_Lie, gp_Ast, gp_Sui) ; // TODO : verifier gp_Sui->SUIVI_EQUATORIAL avant
     
     /* Calcul de la norme de la vitesse */

     gp_Ast->V  = sqrt( sqr( gp_Ast->Va ) + sqr( gp_Ast->Vh )) ;
     
     if ( gp_Ast->Va != 0) 
       gp_Ast->An = atan( gp_Ast->Vh / gp_Ast->Va ) ;
     else
       gp_Ast->An = M_PI/2 ;
     
     CALCUL_GEODE( gp_Ast ) ;
     
     Trace1("%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f", \
       gp_Ast->a * CALCUL_UN_RADIAN_EN_DEGRES, \
       gp_Ast->h * CALCUL_UN_RADIAN_EN_DEGRES, \
       gp_Ast->ASC * CALCUL_UN_RADIAN_EN_DEGRES, \
       gp_Ast->DEC * CALCUL_UN_RADIAN_EN_DEGRES, \
       gp_Ast->x , \
       gp_Ast->y, \
       gp_Ast->z, \
       gp_Ast->Va, \
       gp_Ast->Vh, \
       gp_Ast->V, \
       gp_Ast->An ) ;
     
     Trace1("xx %.1f yy %.1f zz %.1f Va %.1f Vh %.1f V %.1f", \
       gp_Ast->xx , \
       gp_Ast->yy , \
       gp_Ast->zz , \
       gp_Ast->Va, \
       gp_Ast->Vh, \
       gp_Ast->V ) ;
     
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

    TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
    
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

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset(binaire, CALCUL_ZERO_CHAR,sizeof(binaire)) ;
  strcpy(binaire,argv[0]) ;
  
  /* janvier 2022 : ajout argument pour afficher les paramatres 
       de la planete passee avec son numero en argument
  */

  if ( argc == 2 ) {

    if (!strcmp("-h",argv[1]) || !strcmp("--help",argv[1])) {

      printf("%s : USAGE : \n",binaire) ;
      printf("%s -a / ast (astre) <nom>      : calcule tout et affiche tout en fonction de l'astre\n",binaire) ;
      printf("%s -A / AST (astre) <nom>      : idem -a mais continue et effectue le suivi\n",binaire) ;
      printf("%s -L / ala (aLarm)            : arrete le main aubout de argv[1] secondes\n",binaire) ;
      printf("%s -z / azi (aZimut) <A> <H>   : <angle horaire(deg) <declinaison(deg)> : calcul l'azimut en fonction de AH et DEC\n",binaire) ;
      printf("%s -q / equ (eQuateur) <a> <h> : <azimut(deg) <hauteur(deg)>            : calcul l'angle horaire et la declinaison en fonction de azimut et altitude\n",binaire) ;
      printf("%s ast+equ                     : combinaison ast + equ , l idee est de retouver le calcul des vitesses en sens inverse", binaire) ;
      printf("%s -p / pla (planete)          : afficher les calculs concernant la planete (SOLAR_SYSTEM)\n",binaire) ;
      printf("%s -t / tou (tout)             : calcule et affiche toutes les caracteristiques\n", binaire) ;      
      printf("%s -m / mot (moteurs)          : effectue un test simple sur les moteurs\n",binaire) ;
      printf("%s -v / g_Voute (voute) <r(deg)> <l>  : genere un fichier de la voute entiere : voute.csv (resolution,latitude))\n",binaire) ;
      
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

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  /* ---------------------------------------------------------------
  * Gestion d un chemin externe (option -p <path>) si getcwd KO
  * ---------------------------------------------------------------*/

  if ( ( argc == 3 ) &&  ! strcmp( "-p" , argv[1]) ) {
  
    Trace1("option -p = gp_Con_Par->par_rep_home %s pris en compte\n",argv[2]);

    memset( gp_Con_Par->par_rep_home, 0, sizeof(gp_Con_Par->par_rep_home)) ;
    strcpy( gp_Con_Par->par_rep_home, argv[2] ) ;
  }
  else {
    Trace1("pas de option -p => lecture gp_Con_Par->par_rep_home avec getcwd");

    if ( getcwd(gp_Con_Par->par_rep_home, sizeof(gp_Con_Par->par_rep_home)) == NULL ) {
      SyslogEno("getcwd") ;
    }
    else {
      Trace1("gp_Con_Par->par_rep_home lu par getcwd = %s",gp_Con_Par->par_rep_home);
    }
  }
  Trace("gp_Con_Par->par_rep_home = %s",gp_Con_Par->par_rep_home);
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
  
  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
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
    memset( gp_Ast->nom, 0, sizeof(gp_Ast->nom)) ;
    strcpy( gp_Ast->nom, argv[2] ) ;

    /* Recherche de l'as dans les catalogues */
    
    if ( strstr( gp_Ast->nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_dec) ;
    if ( strstr( gp_Ast->nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_dec) ;
    if ( strstr( gp_Ast->nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_eto_dec) ;

    CALCUL_TOUT() ;
    /* CONFIG_AFFICHER_TOUT() ; */   

    exit(0) ;
  }
  /* ---------------------------------------------------------------
  * Gestion d un as en particulier : calcul, affichage et suivi
  * ---------------------------------------------------------------*/

  if ( ( argc == 3  ) &&  ! strcmp("AST",argv[1]) ) {
  
    Trace("as %s pris en compte\n",argv[2]);
    memset( gp_Ast->nom, 0, sizeof(gp_Ast->nom)) ;
    strcpy( gp_Ast->nom, argv[2] ) ;

    /* Recherche de l'as dans les catalogues */
    
    if ( strstr( gp_Ast->nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_dec) ;
    if ( strstr( gp_Ast->nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_dec) ;
    if ( strstr( gp_Ast->nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_eto_dec) ;

    CALCUL_TOUT() ;
    /* CONFIG_AFFICHER_TOUT() ; */
  }
  /* ---------------------------------------------------------------
  * Gestion du calcul de l azimut en fonction 
  * des coordonnes equatoriales
  * ---------------------------------------------------------------*/

  if ( ( argc == 4 ) &&  ! strcmp("azi",argv[1]) ) {

    Trace("as nom mis a la valeur AZI0");
    memset( gp_Ast->nom, 0, sizeof(gp_Ast->nom)) ;
    strcpy( gp_Ast->nom, "AZI0" ) ;

    memset( gp_Ast->infos, 0, sizeof(gp_Ast->nom)) ;
    strcpy( gp_Ast->infos, "calcul volontaire : equatorial => azimutal" ) ;

    gp_Ast->ASC = atof(argv[2]) / CALCUL_UN_RADIAN_EN_DEGRES ;
    gp_Ast->DEC = atof(argv[3]) / CALCUL_UN_RADIAN_EN_DEGRES ;
    
    CALCUL_TOUT() ;
    /* CONFIG_AFFICHER_TOUT() ;  */
	   
    exit(0) ;
  }
  /* ---------------------------------------------------------------
  * Gestion du calcul des coordonnees equatoriales en fonction 
  * des coordonnes azimutales et du gps (date heure lieu etc..)
  * ---------------------------------------------------------------*/

  if ( ( argc == 4 ) &&  ! strcmp("equ",argv[1]) ) {

    Trace("as nom mis a la valeur EQU0");
    memset( gp_Ast->nom, 0, sizeof(gp_Ast->nom)) ;
    strcpy( gp_Ast->nom, "EQU0" ) ;

    memset( gp_Ast->infos, 0, sizeof(gp_Ast->nom)) ;
    strcpy( gp_Ast->infos, "calcul volontaire : azimutal => equatorial" ) ;

    gp_Ast->a = atof(argv[2]) / CALCUL_UN_RADIAN_EN_DEGRES ;
    gp_Ast->h = atof(argv[3]) / CALCUL_UN_RADIAN_EN_DEGRES ;
    
    CALCUL_TOUT() ;
    /* CONFIG_AFFICHER_TOUT() ;  */
	   
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
    memset( gp_Ast->nom, 0, sizeof(gp_Ast->nom)) ;
    strcpy( gp_Ast->nom, argv[2] ) ;
    
    if ( strstr( gp_Ast->nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_dec) ;
    if ( strstr( gp_Ast->nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_dec) ;
    if ( strstr( gp_Ast->nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Ast, g_c_cat_eto_dec) ;

    CALCUL_TOUT() ;
    /* CONFIG_AFFICHER_TOUT() ; */   

    /* mode equateur */

    Trace("as nom mis a la valeur EQU0");
    memset( gp_Ast->nom, 0, sizeof(gp_Ast->nom)) ;
    strcpy( gp_Ast->nom, "EQU0" ) ;
    
    CALCUL_TOUT() ;
    /* CONFIG_AFFICHER_TOUT() ; */ 

    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 2  ) &&  ! strcmp("tou",argv[1]) ) {
  
    Trace("passage en mode azimutal : gp_Sui->SUIVI_EQUATORIAL=0") ;
    gp_Sui->SUIVI_EQUATORIAL=0 ;

    Trace("as nom mis a la valeur TST0");
    memset( gp_Ast->nom, 0, sizeof(gp_Ast->nom)) ;
    strcpy( gp_Ast->nom, "AZI0" ) ;

    CALCUL_TOUT() ;
    /* CONFIG_AFFICHER_TOUT() ; */    

    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 2  ) &&  ! strcmp("sto",argv[1]) ) {
 
     ASTRO_TRAP_MAIN(1) ; 
     exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( argc == 7 ) {

    gp_Lie->lat   = atof(argv[1]) / CALCUL_UN_RADIAN_EN_DEGRES ;
    gp_Ast->DEC    = atof(argv[2]) / CALCUL_UN_RADIAN_EN_DEGRES ;
    gp_Vou->deb  = atof(argv[3]) ;
    gp_Vou->fin  = atof(argv[4]) ;
    gp_Vou->pas  = atof(argv[5]) ;
    gp_Vou->acc  = atof(argv[6]) ;

    CALCUL_TOUT() ;
    /* CONFIG_AFFICHER_TOUT() ; */   
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
    gp_Vou->pas = atof(argv[2]) / CALCUL_UN_RADIAN_EN_DEGRES ;
    gp_Lie->lat  = atof(argv[3]) / CALCUL_UN_RADIAN_EN_DEGRES ; 
    
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
  
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;
    printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gi_gpio_in,gi_gpio_out)) ;
    
    if ( ! strcmp("alt", argv[5]) ) {

     printf("reduction totale altitude  = %f\n", gp_Cal_Par->par_alt_red_tot) ;
     GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->par_alt_red_2 * gp_Cal_Par->par_alt_red_3 * gp_Cal_Par->par_alt_red_4  ;
     periode = 60 / ( gp_Cal_Par->par_alt_red_2 * gp_Cal_Par->par_alt_red_3 * gp_Cal_Par->par_alt_red_4 * atof(argv[3]) ) ;
     printf("deplacement moteur en altitude Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_1( atoi(argv[2]), periode, nbpulse , gp_Gpi_Par_Con->par_alt_dir, gp_Gpi_Par_Con->par_alt_clk) ;

     GPIO_SET_ALT( 0,0,0,0,0,0 ) ;
    }
    if ( ! strcmp("azi", argv[5]) ) {

     printf("reduction totale azimut    = %f\n", gp_Cal_Par->par_azi_red_tot) ; 
     GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->par_azi_red2 * gp_Cal_Par->par_azi_red3 * gp_Cal_Par->par_azi_red4   ;
     periode = 60 / ( gp_Cal_Par->par_azi_red2 * gp_Cal_Par->par_azi_red3 * gp_Cal_Par->par_azi_red4 * atof(argv[3]) ) ;
     printf("deplacement moteur en azimut Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_1( atoi(argv[2]), periode, nbpulse , gp_Gpi_Par_Con->par_azi_dir, gp_Gpi_Par_Con->par_azi_clk) ;

     GPIO_SET_AZI( 0,0,0,0,0,0 ) ;
    }
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;
    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 6 ) &&  ! strcmp("calib2",argv[1]) ) {
  
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;
    printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gi_gpio_in,gi_gpio_out)) ;
    
    if ( ! strcmp("alt", argv[5]) ) {
     printf("reduction totale altitude  = %f\n", gp_Cal_Par->par_alt_red_tot) ;
     GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->par_alt_red_2 * gp_Cal_Par->par_alt_red_3 * gp_Cal_Par->par_alt_red_4  ;
     periode = 60 / ( gp_Cal_Par->par_alt_red_2 * gp_Cal_Par->par_alt_red_3 * gp_Cal_Par->par_alt_red_4 * atof(argv[3]) ) ;
     printf("deplacement moteur en altitude Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_3( atoi(argv[2]), periode, nbpulse , gp_Gpi_Par_Con->par_alt_dir, gp_Gpi_Par_Con->par_alt_clk) ;

     GPIO_SET_ALT( 0,0,0,0,0,0 ) ;
    }
    if ( ! strcmp("azi", argv[5]) ) {
     printf("reduction totale azimut    = %f\n", gp_Cal_Par->par_azi_red_tot) ; 
     GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->par_azi_red2 * gp_Cal_Par->par_azi_red3 * gp_Cal_Par->par_azi_red4   ;
     periode = 60 / ( gp_Cal_Par->par_azi_red2 * gp_Cal_Par->par_azi_red3 * gp_Cal_Par->par_azi_red4 * atof(argv[3]) ) ;
     printf("deplacement moteur en azimut Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_3( atoi(argv[2]), periode, nbpulse , gp_Gpi_Par_Con->par_azi_dir, gp_Gpi_Par_Con->par_azi_clk) ;

     GPIO_SET_AZI( 0,0,0,0,0,0 ) ;
    }
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;
    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 5 ) &&  ! strcmp("calibt",argv[1]) ) {
  
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;
    printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gi_gpio_in,gi_gpio_out)) ;
    
     printf("reduction totale altitude  = %f\n", gp_Cal_Par->par_alt_red_tot) ;
     GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->par_alt_red_2 * gp_Cal_Par->par_alt_red_3 * gp_Cal_Par->par_alt_red_4  ;
     periode = 60 / ( gp_Cal_Par->par_alt_red_2 * gp_Cal_Par->par_alt_red_3 * gp_Cal_Par->par_alt_red_4 * atof(argv[3]) ) ;
     printf("deplacement moteur en altitude Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;
     
     gp_Sui->Th_mic = periode ;
    
     printf("reduction totale azimut    = %f\n", gp_Cal_Par->par_azi_red_tot) ; 
     GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->par_azi_red2 * gp_Cal_Par->par_azi_red3 * gp_Cal_Par->par_azi_red4   ;
     periode = 60 / ( gp_Cal_Par->par_azi_red2 * gp_Cal_Par->par_azi_red3 * gp_Cal_Par->par_azi_red4 * atof(argv[3]) ) ;
     printf("deplacement moteur en azimut Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;
     
     gp_Sui->Ta_mic = periode ;
  }    
  /* (obsolete)
  if ( ( argc == 2 ) &&  ! strcmp("ir",argv[1]) ) {
  
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;
    printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gi_gpio_in,gi_gpio_out)) ;
    
    //gettimeofday(&t00,NULL) ;
    ir_old=1 ;
    ir=1;
    while(1) {
      //usleep( (long)(1000000/38000.0)) ;
      //gettimeofday(&t01,NULL) ;
      //t_diff = (( t01.tv_sec - t00.tv_sec ) * TEMPS_MICRO_SEC) + t01.tv_usec - t00.tv_usec ;
      //if ( (double)t_diff >= TEMPS_MICRO_SEC / ARGUMENTS_FREQ_TRAME_INFRAROUGE ) {
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
  Trace1("as %s pris en compte",argv[1]);
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

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  while ((c = getopt (argc, argv, "a:A:Lq:f:l:")) != -1) {
      switch (c) {
     
        case 'L':
          gp_Sui->alarme=atoi(argv[2]) ;
          Trace("ARGUMENTS_GERER : ala = %d\n",gp_Sui->alarme) ;
        break ;
        
        case 'a' :
          Trace("as %s pris en compte\n",optarg);
          memset( gp_Ast->nom, 0, sizeof(gp_Ast->nom)) ;
          strcpy( gp_Ast->nom, optarg ) ;

          CALCUL_TOUT() ;
          /* CONFIG_AFFICHER_TOUT() ; */   

          exit(0) ;
        break ;
        
        case 'A' :

          Trace("as %s pris en compte\n",optarg);
          memset( gp_Ast->nom, 0, sizeof(gp_Ast->nom)) ;
          strcpy( gp_Ast->nom, optarg ) ;

          CALCUL_TOUT() ;
          /* CONFIG_AFFICHER_TOUT() ; */   

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
