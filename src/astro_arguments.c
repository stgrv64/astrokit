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
#               * renseignement gp_Con_Par->con_par_rep_home par -p <path>
# 11/03/2022  | * ajout fonction ARGUMENTS_MANAGE_REP_HOME
# -------------------------------------------------------------- 
*/

#include "astro_arguments.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

// extern ASTRO_TRAP_MAIN();

/*****************************************************************************************
* @fn     : ARGUMENTS_CREATE_VOUTE
* @author : s.gravois
* @brief  : 
* @param  : const char *fmt
* @param  :  ...
* @date   : 2022-01-19 creation entete de la fonction au format doxygen
* @todo   : (obsolete voir dangereux a utiliser en embarque)
*           => cf article sur le WEB (va_args / embedded)
* @todo   : (obsolete) la bonne pratique est de passer par getopt 
*****************************************************************************************/

void ARGUMENTS_CREATE_VOUTE( void) {
  
  STRUCT_ASTRE * lp_Ast = (STRUCT_ASTRE *)gp_Ast ;
  STRUCT_SUIVI * lp_Sui = (STRUCT_SUIVI *)gp_Sui ;
  STRUCT_VOUTE * lp_Vou = (STRUCT_VOUTE *)gp_Vou ;
  STRUCT_LIEU  * lp_Lie = (STRUCT_LIEU  *)gp_Lie ;

  char cmd[64]={0} ;
  int i_retour=0 ;
  FILE   *fout ;

  TraceArbo(__func__,0,"ceate a 3d voute") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  double a, h ;
  
  if ( ( fout=fopen( ARGUMENTS_FILE_VOUTE ,"w")) == NULL  ) {
    sprintf(cmd,"probleme ouverture %s", ARGUMENTS_FILE_VOUTE) ;
    SyslogErr(cmd) ;
    exit(2);
  } 

  /* -------------------------------------------
  *  On fait varier les coordonnees horaires / declinaison
  * avec un pas = lp_Vou->vou_pas
  * -------------------------------------------*/
  
  for(h=-(M_PI/2)+(lp_Lie->lie_lat)+0.001;h<M_PI/2;h+=lp_Vou->vou_pas)
    if (h>=0) 

    /* -------------------------------------------
    *  On fait varier les coordonnees en ascension droite
    * avec un pas = lp_Vou->vou_pas
    * -------------------------------------------*/

    for(a=-M_PI +0.001 ;a<M_PI;a+=lp_Vou->vou_pas){
     
     lp_Ast->ast_azi=a ;
     lp_Ast->ast_alt=h ;
     
     CALCULS_EQUATEUR(lp_Ast) ; // calcul coordonnees horaires en fait
     CALCULS_VITESSES(lp_Ast,lp_Lie,lp_Sui) ; // TODO : verifier lp_Cal->cal_mode avant
     
     /* Calcul de la norme de la vitesse */

     lp_Ast->ast_vit  = sqrt( sqr( lp_Ast->ast_azi_vit ) + sqr( lp_Ast->ast_alt_vit )) ;
     
     if ( lp_Ast->ast_azi_vit != 0) 
       lp_Ast->ast_ah = atan( lp_Ast->ast_alt_vit / lp_Ast->ast_azi_vit ) ;
     else
       lp_Ast->ast_ah = M_PI/2 ;
     
     CALCULS_COORD_R3(lp_Ast) ;
     
     Trace1("%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f", \
       lp_Ast->ast_azi * CALCULS_UN_RADIAN_EN_DEGRES, \
       lp_Ast->ast_alt * CALCULS_UN_RADIAN_EN_DEGRES, \
       lp_Ast->ast_asc * CALCULS_UN_RADIAN_EN_DEGRES, \
       lp_Ast->ast_dec * CALCULS_UN_RADIAN_EN_DEGRES, \
       lp_Ast->ast_r3_x , \
       lp_Ast->ast_r3_y, \
       lp_Ast->ast_r3_z, \
       lp_Ast->ast_azi_vit, \
       lp_Ast->ast_alt_vit, \
       lp_Ast->ast_vit, \
       lp_Ast->ast_ah ) ;
     
     Trace1("xx %.1f yy %.1f zz %.1f Va %.1f Vh %.1f V %.1f", \
       lp_Ast->ast_r3_xx , \
       lp_Ast->ast_r3_yy , \
       lp_Ast->ast_r3_zz , \
       lp_Ast->ast_azi_vit, \
       lp_Ast->ast_alt_vit, \
       lp_Ast->ast_vit ) ;
     
   }
  
  fclose(fout) ;
}

/*****************************************************************************************
* @fn     : ARGUMENTS_MANAGE_VA_ARGS
* @author : s.gravois
* @brief  : point entree su programme
* @param  : const char *fmt
* @param  :  ...
* @date   : 2022-01-19 creation entete de la fonction au format doxygen
* @todo   : (obsolete voir dangereux a utiliser en embarque)
*           => cf article sur le WEB (va_args / embedded)
* @todo   : (obsolete) la bonne pratique est de passer par getopt 
*****************************************************************************************/

void ARGUMENTS_MANAGE_VA_ARGS(const char *fmt, ... ) {
    int d;
    char c, *s; 
    va_list ap;
    va_start(ap, fmt);

    TraceArbo(__func__,0,"manage va_args") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
    
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

  TraceArbo(__func__,0,"help") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset(binaire, CALCULS_ZERO_CHAR,sizeof(binaire)) ;
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

      printf("%s <LAT> <DEC> <vou_begin voute> <fin voute> <pas voute> <acc voute>\n",binaire);

      exit(0) ;
    }
  }
}

/*****************************************************************************************
* @fn     : ARGUMENTS_MANAGE_REP_HOME
* @author : s.gravois
* @brief  : Gere le passage des arguments de facon classique
* @param  : int     argc
* @param  : char ** argv
* @date   : 2022-03-11 creation
* @todo   : fusioner avec ARGUMENTS_MANAGE_FACON_CLASSIQUE
*****************************************************************************************/

void ARGUMENTS_MANAGE_REP_HOME(int argc, char** argv) {

  TraceArbo(__func__,0,"manage rep home") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  /* ---------------------------------------------------------------
  * Gestion d un chemin externe (option -p <path>) si getcwd KO
  * ---------------------------------------------------------------*/

  if ( ( argc == 3 ) &&  ! strcmp( "-p" , argv[1]) ) {
  
    Trace1("option -p = gp_Con_Par->con_par_rep_home %s pris en compte\n",argv[2]);

    memset( gp_Con_Par->con_par_rep_home, 0, sizeof(gp_Con_Par->con_par_rep_home)) ;
    strcpy( gp_Con_Par->con_par_rep_home, argv[2] ) ;
  }
  else {
    Trace1("pas de option -p => lecture gp_Con_Par->con_par_rep_home avec getcwd");

    if ( getcwd(gp_Con_Par->con_par_rep_home, sizeof(gp_Con_Par->con_par_rep_home)) == NULL ) {
      SyslogEno("getcwd") ;
    }
    else {
      Trace1("gp_Con_Par->con_par_rep_home lu par getcwd = %s",gp_Con_Par->con_par_rep_home);
    }
  }
  Trace("gp_Con_Par->con_par_rep_home = %s",gp_Con_Par->con_par_rep_home);
}
/*****************************************************************************************
* @fn     : ARGUMENTS_MANAGE
* @author : s.gravois
* @brief  : Gere le passage des arguments de facon classique
* @param  : int     argc
* @param  : char ** argv
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void ARGUMENTS_MANAGE_FACON_CLASSIQUE(int argc, char** argv) {
  
  long c_ir_0 = 0 , c_ir_1 = 0 ;
  int  ir_old, ir ;
  double periode ;
  double nbpulse ;
  
  STRUCT_ASTRE * lp_Ast = (STRUCT_ASTRE *)gp_Ast ;
  STRUCT_SUIVI * lp_Sui = (STRUCT_SUIVI *)gp_Sui ;
  STRUCT_VOUTE * lp_Vou = (STRUCT_VOUTE *)gp_Vou ;
  STRUCT_LIEU  * lp_Lie = (STRUCT_LIEU  *)gp_Lie ;

  TraceArbo(__func__,0,"manage argc/argv") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  incrlog=30 ;

  /* ---------------------------------------------------------------
  * Gestion de l Alarme
  * ---------------------------------------------------------------*/

  if ( ( argc == 3 ) &&  ! strcmp("ala",argv[1]) ) {
    
    lp_Sui->sui_alarme=atoi(argv[2]) ;
    Trace("ala = %d\n",lp_Sui->sui_alarme) ;
  }
  /* ---------------------------------------------------------------
  * Gestion d un as en particulier : uniquement calcul et aff
  * ---------------------------------------------------------------*/

  if ( ( argc == 3 ) &&  ! strcmp("ast",argv[1]) ) {
  
    Trace("as %s pris en compte\n",argv[2]);
    memset( lp_Ast->ast_nom, 0, sizeof(lp_Ast->ast_nom)) ;
    strcpy( lp_Ast->ast_nom, argv[2] ) ;

    CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;
    
    /* Recherche de l'as dans les catalogues */
    
    if ( strstr( lp_Ast->ast_nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Ngc, lp_Ast) ; ;
    if ( strstr( lp_Ast->ast_nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Ngc, lp_Ast) ; ;
    if ( strstr( lp_Ast->ast_nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Eto, lp_Ast) ;

    CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;

    CALCULS_TOUT() ;
    /* CONFIG_DISPLAY_TOUT() ; */   

    exit(0) ;
  }
  /* ---------------------------------------------------------------
  * Gestion d un as en particulier : calcul, affichage et suivi
  * ---------------------------------------------------------------*/

  if ( ( argc == 3  ) &&  ! strcmp("AST",argv[1]) ) {
  
    Trace("as %s pris en compte\n",argv[2]);
    memset( lp_Ast->ast_nom, 0, sizeof(lp_Ast->ast_nom)) ;
    strcpy( lp_Ast->ast_nom, argv[2] ) ;

    CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;

    /* Recherche de l'as dans les catalogues */
    
    if ( strstr( lp_Ast->ast_nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Ngc, lp_Ast) ; ;
    if ( strstr( lp_Ast->ast_nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Ngc, lp_Ast) ; ;
    if ( strstr( lp_Ast->ast_nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Eto, lp_Ast) ;
    
    CALCULS_TOUT() ;
    /* CONFIG_DISPLAY_TOUT() ; */
  }
  /* ---------------------------------------------------------------
  * Gestion du calcul de l azimut en fonction 
  * des coordonnes equatoriales
  * ---------------------------------------------------------------*/

  if ( ( argc == 4 ) &&  ! strcmp("azi",argv[1]) ) {

    Trace("as nom mis a la valeur AZI0");
    memset( lp_Ast->ast_nom, 0, sizeof(lp_Ast->ast_nom)) ;
    strcpy( lp_Ast->ast_nom, "AZI0" ) ;

    memset( lp_Ast->ast_infos, 0, sizeof(lp_Ast->ast_nom)) ;
    strcpy( lp_Ast->ast_infos, "calcul volontaire : equatorial => azimutal" ) ;

    lp_Ast->ast_asc = atof(argv[2]) / CALCULS_UN_RADIAN_EN_DEGRES ;
    lp_Ast->ast_dec = atof(argv[3]) / CALCULS_UN_RADIAN_EN_DEGRES ;

    CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;
    CALCULS_TOUT() ;
    /* CONFIG_DISPLAY_TOUT() ;  */
	   
    exit(0) ;
  }
  /* ---------------------------------------------------------------
  * Gestion du calcul des coordonnees equatoriales en fonction 
  * des coordonnes azimutales et du gps (date heure lieu etc..)
  * ---------------------------------------------------------------*/

  if ( ( argc == 4 ) &&  ! strcmp("equ",argv[1]) ) {

    Trace("as nom mis a la valeur EQU0");
    memset( lp_Ast->ast_nom, 0, sizeof(lp_Ast->ast_nom)) ;
    strcpy( lp_Ast->ast_nom, "EQU0" ) ;

    memset( lp_Ast->ast_infos, 0, sizeof(lp_Ast->ast_nom)) ;
    strcpy( lp_Ast->ast_infos, "calcul volontaire : azimutal => equatorial" ) ;

    lp_Ast->ast_azi = atof(argv[2]) / CALCULS_UN_RADIAN_EN_DEGRES ;
    lp_Ast->ast_alt = atof(argv[3]) / CALCULS_UN_RADIAN_EN_DEGRES ;

    CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;
    CALCULS_TOUT() ;
    /* CONFIG_DISPLAY_TOUT() ;  */
	   
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
    memset( lp_Ast->ast_nom, 0, sizeof(lp_Ast->ast_nom)) ;
    strcpy( lp_Ast->ast_nom, argv[2] ) ;
    
    CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;

    if ( strstr( lp_Ast->ast_nom, CONFIG_MES ) != NULL ) CAT_FIND( gp_Ngc, lp_Ast) ; ;
    if ( strstr( lp_Ast->ast_nom, CONFIG_NGC ) != NULL ) CAT_FIND( gp_Ngc, lp_Ast) ; ;
    if ( strstr( lp_Ast->ast_nom, CONFIG_ETO ) != NULL ) CAT_FIND( gp_Eto, lp_Ast) ;
    
    CALCULS_TOUT() ;

    /* CONFIG_DISPLAY_TOUT() ; */   

    /* mode equateur */

    Trace("as nom mis a la valeur EQU0");
    memset( lp_Ast->ast_nom, 0, sizeof(lp_Ast->ast_nom)) ;
    strcpy( lp_Ast->ast_nom, "EQU0" ) ;

    CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;
    CALCULS_TOUT() ;
    /* CONFIG_DISPLAY_TOUT() ; */ 

    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 2  ) &&  ! strcmp("tou",argv[1]) ) {
  
    Trace("passage en mode azimutal : lp_Cal->cal_mode = CALCULS_MODE_AZIMUTAL") ;

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Cal->cal_mutex ) ;

    gp_Cal->cal_mode = CALCULS_MODE_AZIMUTAL ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Cal->cal_mutex ) ;

    Trace("as nom mis a la valeur TST0");
    memset( lp_Ast->ast_nom, 0, sizeof(lp_Ast->ast_nom)) ;
    strcpy( lp_Ast->ast_nom, "AZI0" ) ;

    CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;
    CALCULS_TOUT() ;
    /* CONFIG_DISPLAY_TOUT() ; */    

    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 2  ) &&  ! strcmp("sto",argv[1]) ) {
 
     ASTRO_TRAP_MAIN(1) ; 
     exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( argc == 7 ) {

    lp_Lie->lie_lat   = atof(argv[1]) / CALCULS_UN_RADIAN_EN_DEGRES ;
    lp_Ast->ast_dec   = atof(argv[2]) / CALCULS_UN_RADIAN_EN_DEGRES ;
    lp_Vou->vou_begin = atof(argv[3]) ;
    lp_Vou->vou_end   = atof(argv[4]) ;
    lp_Vou->vou_pas   = atof(argv[5]) ;
    lp_Vou->vou_acc   = atof(argv[6]) ;

    CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;
    CALCULS_TOUT() ;
    /* CONFIG_DISPLAY_TOUT() ; */   
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
    lp_Vou->vou_pas = atof(argv[2]) / CALCULS_UN_RADIAN_EN_DEGRES ;
    lp_Lie->lie_lat  = atof(argv[3]) / CALCULS_UN_RADIAN_EN_DEGRES ; 
    
    ARGUMENTS_CREATE_VOUTE( ) ;
    exit(0) ;
  }
  /* -----------------------------------------------------------------
    Teste les moteurs 
  */
  if ( ( argc == 2 ) &&  ! strcmp("mot",argv[1]) ) {
  
    GPIO_TEST_CONTROLER(  ) ;

    exit(0) ;
  }
  // -----------------------------------------------------------------

  // -----------------------------------------------------------------
  

  // -----------------------------------------------------------------
  

  // -----------------------------------------------------------------
  
  // -----------------------------------------------------------------
  if ( ( argc == 2 ) &&  ! strcmp("sta",argv[1]) ) {
    
    printf("ARGUMENTS_MANAGE : GPIO_STATUT demander\n") ;
    GPIO_STATUT() ;
    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 6 ) &&  ! strcmp("cal0",argv[1]) ) {
  
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;
    printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gi_gpio_in,gi_gpio_out)) ;
    
    if ( ! strcmp("alt", argv[5]) ) {

     printf("reduction totale altitude  = %f\n", gp_Cal_Par->cal_par_alt_red_tot) ;
     GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->cal_par_alt_red_2 * gp_Cal_Par->cal_par_alt_red_3 * gp_Cal_Par->cal_par_alt_red_4  ;
     periode = 60 / ( gp_Cal_Par->cal_par_alt_red_2 * gp_Cal_Par->cal_par_alt_red_3 * gp_Cal_Par->cal_par_alt_red_4 * atof(argv[3]) ) ;
     printf("deplacement moteur en altitude Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_1( atoi(argv[2]), periode, nbpulse , gp_Ctl_Par->par_alt_dir, gp_Ctl_Par->par_alt_clk) ;

     GPIO_SET_ALT( 0,0,0,0,0,0 ) ;
    }
    if ( ! strcmp("azi", argv[5]) ) {

     printf("reduction totale azimut    = %f\n", gp_Cal_Par->cal_par_azi_red_tot) ; 
     GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->cal_par_azi_red_2 * gp_Cal_Par->cal_par_azi_red_3 * gp_Cal_Par->cal_par_azi_red_4   ;
     periode = 60 / ( gp_Cal_Par->cal_par_azi_red_2 * gp_Cal_Par->cal_par_azi_red_3 * gp_Cal_Par->cal_par_azi_red_4 * atof(argv[3]) ) ;
     printf("deplacement moteur en azimut Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_1( atoi(argv[2]), periode, nbpulse , gp_Ctl_Par->par_azi_dir, gp_Ctl_Par->par_azi_clk) ;

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
     printf("reduction totale altitude  = %f\n", gp_Cal_Par->cal_par_alt_red_tot) ;
     GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->cal_par_alt_red_2 * gp_Cal_Par->cal_par_alt_red_3 * gp_Cal_Par->cal_par_alt_red_4  ;
     periode = 60 / ( gp_Cal_Par->cal_par_alt_red_2 * gp_Cal_Par->cal_par_alt_red_3 * gp_Cal_Par->cal_par_alt_red_4 * atof(argv[3]) ) ;
     printf("deplacement moteur en altitude Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_3( atoi(argv[2]), periode, nbpulse , gp_Ctl_Par->par_alt_dir, gp_Ctl_Par->par_alt_clk) ;

     GPIO_SET_ALT( 0,0,0,0,0,0 ) ;
    }
    if ( ! strcmp("azi", argv[5]) ) {
     printf("reduction totale azimut    = %f\n", gp_Cal_Par->cal_par_azi_red_tot) ; 
     GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->cal_par_azi_red_2 * gp_Cal_Par->cal_par_azi_red_3 * gp_Cal_Par->cal_par_azi_red_4   ;
     periode = 60 / ( gp_Cal_Par->cal_par_azi_red_2 * gp_Cal_Par->cal_par_azi_red_3 * gp_Cal_Par->cal_par_azi_red_4 * atof(argv[3]) ) ;
     printf("deplacement moteur en azimut Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;

     GPIO_MOVE_3( atoi(argv[2]), periode, nbpulse , gp_Ctl_Par->par_azi_dir, gp_Ctl_Par->par_azi_clk) ;

     GPIO_SET_AZI( 0,0,0,0,0,0 ) ;
    }
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;
    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 5 ) &&  ! strcmp("calibt",argv[1]) ) {
  
    printf("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;
    printf("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gi_gpio_in,gi_gpio_out)) ;
    
     printf("reduction totale altitude  = %f\n", gp_Cal_Par->cal_par_alt_red_tot) ;
     GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->cal_par_alt_red_2 * gp_Cal_Par->cal_par_alt_red_3 * gp_Cal_Par->cal_par_alt_red_4  ;
     periode = 60 / ( gp_Cal_Par->cal_par_alt_red_2 * gp_Cal_Par->cal_par_alt_red_3 * gp_Cal_Par->cal_par_alt_red_4 * atof(argv[3]) ) ;
     printf("deplacement moteur en altitude Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;
     
     gp_Fre->fre_th_mic = periode ;
    
     printf("reduction totale azimut    = %f\n", gp_Cal_Par->cal_par_azi_red_tot) ; 
     GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * gp_Cal_Par->cal_par_azi_red_2 * gp_Cal_Par->cal_par_azi_red_3 * gp_Cal_Par->cal_par_azi_red_4   ;
     periode = 60 / ( gp_Cal_Par->cal_par_azi_red_2 * gp_Cal_Par->cal_par_azi_red_3 * gp_Cal_Par->cal_par_azi_red_4 * atof(argv[3]) ) ;
     printf("deplacement moteur en azimut Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;
     
     gp_Fre->fre_ta_mic = periode ;
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
      //t_diff = (( t01.tv_sec - t00.tv_sec ) * TIME_MICRO_SEC) + t01.tv_usec - t00.tv_usec ;
      //if ( (double)t_diff >= TIME_MICRO_SEC / ARGUMENTS_FREQ_TRAME_INFRAROUGE ) {
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
  printf("fin %s\n","ARGUMENTS_MANAGE") ;
}

/*****************************************************************************************
* @fn     : ARGUMENTS_MANAGE
* @author : s.gravois
* @brief  : Gere le passage des arguments de facon classique
* @param  : int     argc
* @param  : char ** argv
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void ARGUMENTS_MANAGE_GETOPT(int argc, char** argv) {

  STRUCT_ASTRE * lp_Ast = (STRUCT_ASTRE *)gp_Ast ;
  STRUCT_SUIVI * lp_Sui = (STRUCT_SUIVI *)gp_Sui ;
  STRUCT_VOUTE * lp_Vou = (STRUCT_VOUTE *)gp_Vou ;
  STRUCT_LIEU  * lp_Lie = (STRUCT_LIEU  *)gp_Lie ;
  char c=0 ;

  TraceArbo(__func__,0,"manage via getopt") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  while ((c = getopt (argc, argv, "a:A:Lq:f:l:")) != -1) {
      switch (c) {
     
        case 'L':
          lp_Sui->sui_alarme=atoi(argv[2]) ;
          Trace("ARGUMENTS_MANAGE : ala = %d\n",lp_Sui->sui_alarme) ;
        break ;
        
        case 'a' :
          Trace("as %s pris en compte\n",optarg);
          memset( lp_Ast->ast_nom, 0, sizeof(lp_Ast->ast_nom)) ;
          strcpy( lp_Ast->ast_nom, optarg ) ;

          CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;
          
          CALCULS_TOUT() ;
          /* CONFIG_DISPLAY_TOUT() ; */   

          exit(0) ;
        break ;
        
        case 'A' :

          Trace("as %s pris en compte\n",optarg);
          memset( lp_Ast->ast_nom, 0, sizeof(lp_Ast->ast_nom)) ;
          strcpy( lp_Ast->ast_nom, optarg ) ;

          CALCULS_RECUP_MODE_ET_ASTRE_TYPE() ;
          CALCULS_TOUT() ;
          /* CONFIG_DISPLAY_TOUT() ; */   

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
