#include <arguments.h>

//================================================================================================
// exemple d'appel : 
// ARGUMENTS_VARIABLES("%s %d %c", "string" , 235, 6, 't') ;
//================================================================================================

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
//================================================================================================
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
      printf("%s sys (numero planete)   : afficher les calculs concernant la planete\n",binaire) ;
      printf("%s ala (alarm)            : arrete le main aubout de argv[1] secondes\n",binaire) ;
      printf("%s car (caracteristiques) : afiche les caracteristiques du lieu et de la date : JOUR JULIEN et TEMPS SIDERAL\n", binaire) ;
      printf("%s azi (azimut) <A> <H>   : <angle horaire(deg) <declinaison(deg)> : calcul l'azimut en fonction de AH et DEC\n",binaire) ;
      printf("%s equ (equateur) <a> <h> : <azimut(deg) <hauteur(deg)> : calcul l'angle horaire et la declinaison en fonction de azimut et altitude\n",binaire) ;
      printf("%s ast (astre) <nom>      : exemple %s NGC1234 : calcule et affiche en fonction de l'astre\n",binaire,binaire) ;
      printf("%s mot (test moteurs)     : teste les moteurs\n",binaire) ;
      printf("%s vou (voute) <r> <l>    : genere un fichier de la voute entiere : voute.csv (resolution,altitude))\n",binaire) ;
      printf("\n") ;
      printf("%s cal (calibration) <sens> <nombre de tours par minute> <nombre de tours> <atl/azi>: deplace les moteurs en fonction des 3 args suivants en altitude ou bien en azimut en fonction de la configuration\n",binaire) ;
      printf("%s cat <sens> <nombre de tours par minute> <nombre de tours> <atl/azi>: idem calib mais gerer par les threads (et donc un suivi meilleur de la periode SIC)\n",binaire) ;
      printf("%s statut : affiche le statut des broches GPIO\n",binaire) ;
      printf("%s <LAT> <DEC> <deb voute> <fin voute> <pas voute> <acc voute>\n",binaire);
      exit(0) ;
    }
  }
}

//================================================================================================
void  ARGUMENTS_TEST_MOTEURS(void ) {

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
  
    nb_puls = GPIO_ACCELERATION_2( GPIO_CLK_ALT, GPIO_CLK_AZI, f_deb, f_fin, delai, suivi->l_NANO_MOINS) ;
    
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
//================================================================================================
void ARGUMENTS_VOUTE(void ) {
  
  FILE   *fout ;
  // double A, H ;
  double a, h ;
  
  fout=fopen("voute.csv","w") ;
  
  // On fait varier les coordonnees HORAIRES
  
  for(h=-(PI/2)+(lieu->lat)+0.001;h<PI/2;h+=voute->pas)
    if (h>=0) 
    for(a=-PI +0.001 ;a<PI;a+=voute->pas){
     
     astre->a=a ;
     astre->h=h ;
     
     CALCUL_EQUATEUR  ( lieu, astre) ; // calcul coordonnees horaires en fait
     CALCUL_VITESSES  ( lieu, astre, suivi) ; // TODO : verifier suivi->SUIVI_EQUATORIAL avant
     
     astre->V  = sqrt( sqr( astre->Va ) + sqr( astre->Vh )) ;
     
     if ( astre->Va != 0) 
       astre->An = atan( astre->Vh / astre->Va ) ;
     else
       astre->An = PI/2 ;
     
     CALCUL_GEODE( astre ) ;
     
     printf("%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f\n", \
       astre->a * DEGRES, \
       astre->h * DEGRES, \
       astre->A * DEGRES, \
       astre->H * DEGRES, \
       astre->x , \
       astre->y, \
       astre->z, \
       astre->Va, \
       astre->Vh, \
       astre->V, \
       astre->An ) ;
     
     /*  	
     printf("%.15f %.15f %.15f %.15f %.15f %.15f\n", \
       astre->xx , \
       astre->yy , \
       astre->zz , \
       astre->V ) ;
     
     printf("%.1f %.1f %.1f %.1f %.1f %.1f\n", \
       astre->xx , \
       astre->yy , \
       astre->zz , \
       astre->Va, \
       astre->Vh, \
       astre->V ) ;
     */
   }
  
  fclose(fout) ;
}

//================================================================================================
void ARGUMENTS_GERER(int argc, char** argv) {
  
  long c_ir_0 = 0 , c_ir_1 = 0 ;
  int  ir_old, ir ;
  double periode ;
  double nbpulse ;
  
  incrlog=30 ;
  
  // -----------------------------------------------------------------
  if ( ( argc == 2  ) &&  ! strcmp("car",argv[1]) ) {
  
    CALCUL_TOUT(lieu, temps, astre, suivi, clavier ) ;
    CONFIG_AFFICHER_ASTRE(astre) ;
    CONFIG_AFFICHER_LIEU( lieu) ;
    CALCUL_AFFICHER_HEURE( "Il est : ", temps ) ;
    CALCUL_TEMPS_SIDERAL( lieu, temps ) ;

    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 2  ) &&  ! strcmp("stop",argv[1]) ) {
 
     //TRAP_MAIN(1) ; 
     exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( argc == 7 ) {
    lieu->lat   = atof(argv[1]) / DEGRES ;
    astre->H    = atof(argv[2]) / DEGRES ;
    voute->deb  = atof(argv[3]) ;
    voute->fin  = atof(argv[4]) ;
    voute->pas  = atof(argv[5]) ;
    voute->acc  = atof(argv[6]) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 4  ) &&  ! strcmp("voute",argv[1]) ) {
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
    voute->pas = atof(argv[2]) / DEGRES ;
    lieu->lat  = atof(argv[3]) / DEGRES ; 
    
    ARGUMENTS_VOUTE( ) ;
    exit(0) ;
  }
  /* -----------------------------------------------------------------
    Teste les moteurs 
  */
  if ( ( argc == 3 ) &&  ! strcmp("mot",argv[1]) ) {
  
    suivi->l_NANO_MOINS = atol( argv[2]) ;
    
    ARGUMENTS_TEST_MOTEURS(  ) ;
    exit(0) ;
  }
  // -----------------------------------------------------------------
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
  // -----------------------------------------------------------------
  
  if ( ( argc == 3 ) &&  ! strcmp("ast",argv[1]) ) {
  
    printf("astre %s pris en compte\n",argv[2]);
    memset( astre->nom, 0, sizeof(astre->nom)) ;
    strcpy( astre->nom, argv[2] ) ;
  }
  // -----------------------------------------------------------------
  
  if ( ( argc == 4 ) &&  ! strcmp("azi",argv[1]) ) {
  
    astre->A = atof(argv[2]) / DEGRES ;
    astre->H = atof(argv[3]) / DEGRES ;
    strcpy( astre->nom, "test" ) ;

    CALCUL_AZIMUT( lieu, astre ) ;
    CALCUL_VITESSES( lieu, astre, suivi) ; // TODO : verifier suivi->SUIVI_EQUATORIAL avant
    CALCUL_PERIODE ( astre, suivi, voute) ;
    
    CONFIG_AFFICHER_ASTRE(astre) ;
    CONFIG_AFFICHER_LIEU( lieu) ;
	   
    exit(0) ;
  }
  // -----------------------------------------------------------------
  
  if ( ( argc == 4 ) &&  ! strcmp("equ",argv[1]) ) {
  
    astre->a = atof(argv[2]) / DEGRES ;
    astre->h = atof(argv[3]) / DEGRES ;
    
    CALCUL_EQUATEUR( lieu, astre ) ;
    CALCUL_VITESSES( lieu, astre, suivi) ; // TODO : verifier suivi->SUIVI_EQUATORIAL avant
    CALCUL_PERIODE ( astre, suivi, voute) ;
    
    CONFIG_AFFICHER_ASTRE(astre) ;
    CONFIG_AFFICHER_LIEU( lieu) ;
	   
    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 2 ) &&  ! strcmp("sta",argv[1]) ) {
    
    printf("ARGUMENTS_GERER : GPIO_STATUT demander\n") ;
    GPIO_STATUT() ;
    exit(0) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 3 ) &&  ! strcmp("ala",argv[1]) ) {
    
    suivi->alarme=atoi(argv[2]) ;
    printf("ARGUMENTS_GERER : ala = %d\n",suivi->alarme) ;
  }
  // -----------------------------------------------------------------
  if ( ( argc == 6 ) &&  ! strcmp("car",argv[1]) ) {
  
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
     
     suivi->Th = periode ;
    
     printf("reduction totale azimut    = %f\n", AZI_R) ; 
     GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
     nbpulse = atof(argv[4]) * AZI_R2 * AZI_R3 * AZI_R4   ;
     periode = 60 / ( AZI_R2 * AZI_R3 * AZI_R4 * atof(argv[3]) ) ;
     printf("deplacement moteur en azimut Nb tours par minute = %f =  Nombre de tours = %f\n", atof(argv[3]), atof(argv[4]) ) ;
     
     suivi->Ta = periode ;
  }    
  // -----------------------------------------------------------------
  TRACE("astre %s pris en compte",argv[1]);
  printf("fin %s\n","ARGUMENTS_GERER") ;
}
