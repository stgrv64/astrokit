/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 15/11/2021  | * (types.h) modification des types enum et contenu enum
#               * (types.h) modification ordre des menus (MENU_AZIMUTAL=0)
# 19/01/2022  | * gestion du cas ou l astre n a pas de nom
#                 c'est a dire q'on effectue les calculs en fonction des coordevices
#                 sans l'information de nom (recherche catagues inutiles)
# 21/01/2022  | * recuperation numero objet (PLA5 => 5, MES10 -> 10, ..)
#               * passage dernier arg de SOLAR_SYSTEM abec astre->numero
# 12/03/2022  | * correction calcul DEC (declinaison) - => + dans formule
# -------------------------------------------------------------- 
*/

#include <calculs.h>

double sqr(double x)                                    { return x*x ; }
double max(double x1,double x2)                         { if(fabs(x1)>fabs(x2)) return x1; else return x2 ; }
double SGN(double x)                                    { if(x>=0.0) return 1.0; else return -1.0 ; }

/*
double ASC  (double H,   double a, double h)            { return asin(sin(a)*cos(h)/cos(H));}
double ASC1 (double LAT, double H, double a, double h)  { return acos((cos(a)*cos(h)+cos(LAT)*sin(H))/(sin(LAT)*cos(H))) ;}
double ASC2 (double LAT, double H, double h)            { return acos((sin(h)-sin(H)*sin(LAT))/(cos(H)*cos(LAT))) ;}
double ALT  (double LAT, double A, double H)            { return asin(sin(LAT)*sin(H)+cos(LAT)*cos(H)*cos(A));}
double AZI  (double A, double H, double h)              { return asin( sin(A) * cos(H) / cos(h));}
double AZI1 (double LAT, double A, double H, double h)  { return acos((sin(LAT)*cos(H)*cos(A)-cos(LAT)*sin(H))/cos(h)) ;}
double RAD  (int degres, int minutes )                  { return ((double)degres + ( SGN(degres)*(double)minutes) / 60.0 ) / DEGRES ; }
double DEG  (int degres, int minutes )                  { return (double)degres  + ( SGN(degres)*(double)minutes) / 60.0 ; }

*/
/* 
   => latitude lat 
   => azimut            azi et altitude  alt
   => ascension droite  asc et déclinaison dec 
   => H = angle horaire
equatorial => azimutal

agh = t - asc
sin (alt) = sin (dec) sin (lat) + cos (dec) cos (lat) cos (agh)
sin (azi) = - sin (agh) cos ( dec ) / cos (alt)
cos (azi) = {sin (dec) - sin (lat) sin (alt)} / {cos (lat) cos (alt)}

azimutal => equatorial

sin (dec) = sin (alt) sin (lat) + cos (alt) cos (lat) cos (azi)
sin (agh) = - sin (azi) cos (alt) / cos ( dec )
cos (agh) = {sin (alt) - sin (dec) sin (lat)} / cos (dec) cos (lat)
asc = t - H

*/
double DEC  (double LAT, double azi, double alt)            { return asin(sin(LAT)*sin(alt) + cos(LAT)*cos(alt)*cos(azi));}
/*
ouble  ALT  (double LAT, double A, double H)            { return asin(sin(LAT)*sin(H)+cos(LAT)*cos(H)*cos(A));}
*/
double ALT  (double LAT, double A, double H)            { return asin(sin(LAT)*sin(H)+cos(LAT)*cos(H)*cos(A));}
/*
double AGH0 (double H,   double azi, double alt)           { return asin( sin(azi) * cos(alt) / cos(H));}
*/
double AGH0 (double azi,  double alt, double H)            { return asin( sin(azi) * cos(alt) / cos(H));}
double AZI   (double A,  double H, double alt)            { return asin( sin(A) * cos(H) / cos(alt));}
/*
double AGH1 (double LAT, double H, double azi, double alt) { return acos((cos(azi)  * cos(alt)+cos(LAT)*sin(H))/(sin(LAT)*cos(H))) ;}
*/
double AGH1  (double LAT, double azi, double alt, double H)  { return acos((sin(LAT)* cos(alt)*cos(azi)-cos(LAT)*sin(alt))/cos(H)) ;}
double AZI1  (double LAT, double A, double H, double alt)  { return acos((sin(LAT)* cos(H)*cos(A)-cos(LAT)*sin(H))/cos(alt)) ;}

double AGH2 (double LAT, double H, double alt)           { return acos((sin(alt) - sin(H)*sin(LAT))/(cos(H)*cos(LAT))) ;}

double RAD  (int degres, int minutes )                  { return ((double)degres + ( SGN(degres)*(double)minutes) / 60.0 ) / DEGRES ; }
double DEG  (int degres, int minutes )                  { return (double)degres  + ( SGN(degres)*(double)minutes) / 60.0 ; }

//========================================================================================
// FIXME : CALCUL_DUREE_MICROSEC : 
// FIXME : * permet de calculer une duree depuis un dernier appel a cette meme focntion
// FIXME : * resultat en microsecondes
// FIXME : * met a jour la structure appelee (pointeur)
// FIXME : * utlisation gettimeofday
//========================================================================================

double CALCUL_DUREE_MICROSEC(struct timeval *t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  gettimeofday(&t01,NULL) ;

  t_diff = GPIO_MICRO_SEC * ( t01.tv_sec  - t00->tv_sec ) + ( t01.tv_usec - t00->tv_usec ) ;

  t00->tv_sec  = t01.tv_sec ; 
  t00->tv_usec = t01.tv_usec ;

  return t_diff ;
}
//========================================================================================
// FIXME : CALCUL_DUREE_NANOSEC : 
// FIXME : * permet de calculer une duree depuis un dernier appel a cette meme focntion
// FIXME : * resultat en nanosecondes
// FIXME : * met a jour la structure appelee (pointeur)
// FIXME : * utlisation FIXME : modifier et mettre a jour
//========================================================================================
// TODO : a finir : modifier pour passer en nanosecondes

double CALCUL_DUREE_NANOSEC(struct timeval *t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  gettimeofday(&t01,NULL) ;

  t_diff = GPIO_MICRO_SEC * ( t01.tv_sec  - t00->tv_sec ) + ( t01.tv_usec - t00->tv_usec ) ;

  t00->tv_sec  = t01.tv_sec ; 
  t00->tv_usec = t01.tv_usec ;

  return t_diff ;
}
//========================================================================================
// FIXME : CALCUL_TEMPORISATION_VOUTE : 
// FIXME : * permet de faire une temporisation (1s) dans la boucle infinie de SUIVI_VOUTE
// FIXME : * utilise un usleep() sur 99% du temps de la temporisation (cf calibration_voute)
// TODO  : * verifier si on peut augmenter la valeur de calibration_voute en augmentant 
//           la priorite du thread egalement
//========================================================================================

long CALCUL_TEMPORISATION_VOUTE(VOUTE *voute, struct timeval t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  usleep ( (long)(voute->DT * voute->pourcentage_tempo) ) ;
  
  t_diff=0;
  
  // FIXME : rappel : voute->calibration_voute = 0.99 normalement
  // FIXME : pour eviter de faire appel a la boucle while 99% du temps
  
  while( t_diff < ( voute->DT * voute->calibration_voute ) ) {
     
   gettimeofday(&t01,NULL) ;
   t_diff = (double)(( t01.tv_sec - t00.tv_sec)  * GPIO_MICRO_SEC ) + t01.tv_usec - t00.tv_usec;
  }
  return t_diff ;
}
//========================================================================================
// FIXME : CALCUL_TEMPORISATION_MICROSEC : 
// FIXME : * permet de faire une temporisation 
// TODO  : * supprimer ? non utilise actuellement
//========================================================================================

long CALCUL_TEMPORISATION_MICROSEC(double microsecondes, double percent, struct timeval t00) {
  
  struct timeval t01 ;
  unsigned long  t_diff ;
  
  // temporisation pour ne pas consommer de CPU
  // on sommeille = pourcentage_tempo du usleep 
  
  usleep ( (long)(microsecondes * percent) ) ;
  
  t_diff=0;
  while( t_diff < microsecondes ) {
   gettimeofday(&t01,NULL) ;
   t_diff = (( t01.tv_sec - t00.tv_sec)  * GPIO_MICRO_SEC ) + t01.tv_usec - t00.tv_usec;
  }
  return t_diff ;
}
//========================================================================================
// FIXME : CALCUL_TEMPORISATION_ALTITUDE : 
// FIXME : * permet de faire une temporisation dans le thread SUIVI_ALTITUDE de astro.c
// TODO  : * supprimer ? SUIVI_ALTITUDE non utilise actuellement
//========================================================================================

void CALCUL_TEMPORISATION_ALTITUDE(SUIVI *suivi, struct timeval * pt00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  // temporisation pour ne pas consommer de CPU
  // on sommeille = pourcentage_tempo du usleep 
  
  usleep ( (long)(suivi->DTh * suivi->pourcentage_tempo) ) ;
  
  t_diff=0;
  while( t_diff < suivi->DTh ) {
     
   gettimeofday(&t01,NULL) ;
   t_diff = (( t01.tv_sec - pt00->tv_sec)  * GPIO_MICRO_SEC ) + t01.tv_usec - pt00->tv_usec;
  }
  gettimeofday(pt00,NULL) ;
}
//========================================================================================
// FIXME : CALCUL_TEMPORISATION_AZIMUT : 
// FIXME : * permet de faire une temporisation dans le thread SUIVI_AZIMUT de astro.c
// TODO  : * supprimer ? SUIVI_AZIMUT non utilise actuellement
//========================================================================================

void CALCUL_TEMPORISATION_AZIMUT(SUIVI *suivi, struct timeval * pt00) {
  
  struct timeval t01 ;
  double t_diff ;
  
  // temporisation pour ne pas consommer de CPU
  // on sommeille = pourcentage_tempo du usleep 
  
  usleep ( (long)(suivi->DTa * suivi->pourcentage_tempo) ) ;
  
  t_diff=0;
  while( t_diff < suivi->DTa ) {
     
   gettimeofday(&t01,NULL) ;
   t_diff = (( t01.tv_sec - pt00->tv_sec)  * GPIO_MICRO_SEC ) + t01.tv_usec - pt00->tv_usec;
  }
  gettimeofday(pt00,NULL) ;
}
//========================================================================================
// FIXME : CALCUL_GEODE : 
// FIXME : * calcule les coordevices (x,y,z) dans R3 a partir de azimut et altitude
// FIXME : * en supposant que la voute est de diametre = 1
//========================================================================================

void CALCUL_GEODE(ASTRE *astre) {

  // on deduit de l'azimut et de l'altitude
  // les coordevices x y et z dans la geode d'observation
  
  astre->x = cos( astre->h ) * cos( astre->a )  ;
  astre->y = cos( astre->h ) * sin( astre->a ) ;
  astre->z = sin( astre->h ) ;
  
  // On projette la sphere de rayon=1 sur une autre sphere de rayon la valeur qu'on veut
  // ce systeme permet de voir en 3D une valeur en fonction de 3 autres
  // Ici on veut representer la vitesse (norme du vecteur) 
  // en fonction de x y et z (x y z directions du vecteur sur la sphere unite)
  
  astre->xx = astre->x * astre->V ;
  astre->yy = astre->y * astre->V ;
  astre->zz = astre->z * astre->V ;
  
}
//========================================================================================
// FIXME : CALCUL_AZIMUT : 
// FIXME : * transforme les coordevices siderales en coordevices azimutales
//========================================================================================

void CALCUL_AZIMUT(LIEU *lieu, ASTRE *astre) {
  
  // transforme les coordevices "sid�rales" en coordevices azimutales
  // permet de trouver l'azimut et l'altitude en fonction de l'angle horaire
  // par rapport au m�ridien (SUD) et la declinaison
  // ATTENTION !! angle horaire = ascension droite sid�rale corrig�e
  // avec le temps sid�ral : une conversion doit etre faite pour trouver
  // astre->AGH  = TS - (asc droite absolue)
  // Une fonction de calcul du temps sid�ral doit etre faite ult�rieurement 
  
  double LAT,A,H,a,h,a1,af ;
  
  LAT= lieu->lat ;
  A  = astre->AGH ; // FIXME : angle horaire calcule prealablement dans astro.c en theorie
  H  = astre->DEC ; // FIXME : declinaison - seule valeur qui ne change pas 
  
  TRACE1("A = %2.3f\tH = %2.3f\t",(astre->AGH)*DEGRES,(astre->DEC)*DEGRES) ;

  h  = ALT(LAT,A,H) ;
  a  = AZI(A,H,h) ;
  a1 = AZI1(LAT,A,H,h) ;
  af = SGN(a)*a1 ;
  
  astre->a = af ;
  astre->h = h ;
  
   // resultats de calculs : pour tests (a modifier : supprimer)
  
  astre->AZI = a ;
  astre->AZI1 = a1 ;
  
  CALCUL_CONVERSION_ANGLE_EN_TEMPS( astre) ;
  
  TRACE1("a = %2.3f\th = %2.3f\t",(astre->a)*DEGRES,(astre->h)*DEGRES) ;
}
//========================================================================================
// FIXME : CALCUL_EQUATEUR : 
// FIXME : * transforme les coordevices azimutales en coordevices siderales (angle horaire)
// TODO :  * probleme : les calculs ne sont pas bons (a verifier)
// TODO :  * cette fonction servira quand on devra avoir les coordevices equatoriales
//          en fonction des coordonnnees azimutales , pour le calcul des VITESSES,
//          qui utilise les coordevices equatoriales 
//========================================================================================

void CALCUL_EQUATEUR(LIEU *lieu, ASTRE *astre) {

  // transforme les coordevices azimutales en coordevices siderales (angle horaire)
  // permet de trouver les coordonn�es siderales en fonction de l'azimut et de l'altitude
  // Pour avoir la correspondance avec les coordonnn�es �quatoriales brutes, il faut
  // utiliser la conversion AH = TS - ASC
  // AH  : angle horaire
  // TS  : temps sid�ral
  // ASC : ascension droite brute
  
  double A0,A2,A1,a,h,LAT,Af,A,H ;

  TRACE1("avant calcul => a = %2.3f\th = %2.3f\t=>A = %2.3f\tH=%2.3f",\
    (astre->a)   * DEGRES,\
    (astre->h)   * DEGRES,\
    (astre->AGH)* DEGRES,\
    (astre->DEC) * DEGRES) ;

  LAT      = lieu->lat ;
  a        = astre->a ;
  h        = astre->h ;
  
  // FIXME : obtention de declinaison et ascension droite (d'apres formules usuelles de Gauss)
  
  H  = DEC(LAT,a,h) ;
  astre->DEC  = H ;

  /* double AGH0 (double a, double h, double H)  { return asin( sin(a) * cos(h) / cos(H));} */
  A0 = AGH0(a,h,H) ;
  /* 
  double AGH1  (double LAT, double a, double h, double H)  { return acos((sin(LAT)* cos(h)*cos(a)-cos(LAT)*sin(h))/cos(H)) ;}
  */
  A1 = AGH1(LAT,a,h,H); 
  A2 = AGH2(LAT,H,h) ;
  
  astre->AGH = SGN(A0)*A1 + PI ;
  
  // resultats de calculs : pour tests (a modifier : supprimer)

  astre->AGH0 = A0 ;
  astre->AGH1 = A1 ;
  astre->AGH2 = A2 ;
  
  CALCUL_CONVERSION_ANGLE_EN_TEMPS( astre) ;
  
  TRACE(" %s : ASC = %d.%d.%d (hms) %.2f (deg) %.2f (rad)", \
       astre->nom , \
       astre->ASCt.HH, \
       astre->ASCt.MM, \
       astre->ASCt.SS, \
       astre->ASC * DEGRES, \
       astre->ASC \
  ) ; 

  TRACE("AH0   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    astre->AGH0t.HH, astre->AGH0t.MM , astre->AGH0t.SS, astre->AGH0 * DEGRES );

  TRACE("AH1   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    astre->AGH1t.HH, astre->AGH1t.MM , astre->AGH1t.SS, astre->AGH1 * DEGRES );

  TRACE("AH2   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    astre->AGH2t.HH, astre->AGH2t.MM , astre->AGH2t.SS, astre->AGH2 * DEGRES );

  TRACE("AH    (deg) = %d.%d.%d (hms) %.2f (deg)", \
    astre->AGHt.HH, astre->AGHt.MM , astre->AGHt.SS, astre->AGH * DEGRES );

  TRACE("DEC   (deg) = %f" , (astre->DEC)*DEGRES) ;

  TRACE2("apres calcul =>a = %2.3f\th = %2.3f\t=>A = %2.3f\tH=%2.3f",\
   (astre->a)*DEGRES,\
   (astre->h)*DEGRES,\
   (astre->AGH)*DEGRES,\
   (astre->DEC)*DEGRES) ;
} 

//========================================================================================
// FIXME : CALCUL_VITESSES_EQUATORIAL : 
// FIXME : * remet a jour Va et Vh (vitesses sur les moteurs azimut et altitude) 
//           comme vitesses equatoriales theoriques (15" / s ) 
//========================================================================================

void CALCUL_VITESSES_EQUATORIAL(ASTRE *as) {

  as->Va = -15.0 ;
  as->Vh = -15.0 ;
}
//========================================================================================
// FIXME : CALCUL_VITESSES : 
// FIXME : * calcule les vitesses azimutales Va et Vh (vitesses sur les moteurs azimut et altitude) 
//========================================================================================

void CALCUL_VITESSES(LIEU *li,ASTRE *as, SUIVI *suivi) {

  // le calcul des vitesses est fait avec
  // l'angle horaire et la declinaison (azimut et altitude inutiles)
 
  double G, Va, Vh, angle ;
  
  if ( suivi->SUIVI_EQUATORIAL == 1 ) {
    as->Va = -15.0 ;
    as->Vh = -15.0 ;
  }
  else {
    angle=ANGLE_S ;
    G  = sqr(sin(as->AGH))*sqr(cos(as->DEC))+sqr(sin(li->lat)*cos(as->DEC)*cos(as->AGH)-cos(li->lat)*sin(as->DEC)) ;
    Va = angle*(cos(as->DEC)*(cos(li->lat)*cos(as->AGH)*sin(as->DEC)-sin(li->lat)*cos(as->DEC)))/G ;
    Vh = angle*sin(as->AGH)*cos(as->DEC)*cos(li->lat)/sqrt(G) ;
    as->Va=Va ;
    as->Vh=Vh ;
  }
  TRACE1("%2.0f\t%2.8f\t%2.4f\t=>\t%2.8f\t%2.8f\n",(li->lat)*DEGRES,(as->AGH)*DEGRES,(as->DEC)*DEGRES,Va,Vh) ;
}
//========================================================================================
// FIXME : CALCUL_D : 
// FIXME : * calcule les nombres a injecter dans des diviseurs de frequence 
// TODO :  * inutilise (a servi au debut du projet) 
// TODO :  * a supprimer ?
//========================================================================================

void CALCUL_D(ASTRE *astre, SUIVI *suivi) {
  
  suivi->Da = AZI_F * 2.0 * PI / ( AZI_R * pow(2.0, AZI_N) * ( astre->Va / ( 60 * 60 * DEGRES ) )) ;
  suivi->Dh = ALT_F * 2.0 * PI / ( ALT_R * pow(2.0, ALT_N) * ( astre->Vh / ( 60 * 60 * DEGRES ) )) ;
}
//========================================================================================
// FIXME : CALCUL_PERIODE : 
// FIXME : * calcule les "vrais" periodes et frequences des moteurs pas a pas
//           en tant que paramtres de la sinusoide de reference
//========================================================================================

void CALCUL_PERIODE(ASTRE *astre,SUIVI* suivi, VOUTE *voute) {

  double freq_alt, freq_azi ;
  double azi_rot,   alt_rot ;
  
  // Calcul de la periode necessaire si pas de terme D ni 2 puissance N
  // ceci dans le cas ou on genere directement la frequence de reference
  // depuis la RASPI, au lieu d'utiliser 2 diviseurs de frequence en serie
  // qui correspondent au diviseur 2^N et diviseur programmable
  // Ou bien ces periodes / frequences servent pour la generation des signaux PWM
  
  // Les flags de reversibilite *_ROT permettent de ne pas etre dependant du sens de branchement des moteurs !

  if ( AZI_ROT == 0 )       azi_rot = -1.0 ; else azi_rot = 1.0 ;
  if ( ALT_ROT == 0 )       alt_rot = -1.0 ; else alt_rot = 1.0 ;

  // La periode de base en mode controleur est directement geree par ce controleur
  // La periode de base en mode PWM est le quart d'une sinusoide
  
  TRACE2("%f %f %f %f %f",suivi->acc_azi, voute->acc, AZI_R, astre->Va, azi_rot);
  TRACE2("%f %f %f %f %f",suivi->acc_alt, voute->acc, AZI_R, astre->Vh, alt_rot);

  if ( devices->DEVICE_CONTROLEUR_MOTEUR_USE )  freq_azi     = suivi->acc_azi * voute->acc * AZI_R * astre->Va * azi_rot / DIV / PIPI ;
  else                              freq_azi     = suivi->acc_azi * voute->acc * AZI_R * astre->Va * azi_rot * AZI_R4 / DIV / PIPI / 4  ;

  if ( devices->DEVICE_CONTROLEUR_MOTEUR_USE )  freq_alt     = suivi->acc_alt * voute->acc * ALT_R * astre->Vh * alt_rot / DIV / PIPI ;
  else                              freq_alt     = suivi->acc_alt * voute->acc * ALT_R * astre->Vh * alt_rot * ALT_R4 / DIV / PIPI / 4  ;

  pthread_mutex_lock(& suivi->mutex_azi );
      
    suivi->Sa_old = suivi->Sa ; 
    suivi->Sa     = (int)SGN(freq_azi)  ;
    suivi->Fa     = fabs(freq_azi )  ;
    suivi->Ta     = 1 / suivi->Fa ;
  
  pthread_mutex_unlock(& suivi->mutex_azi );
  
  pthread_mutex_lock(& suivi->mutex_alt );

    suivi->Sh_old = suivi->Sh ; 
    suivi->Sh     = (int)SGN(freq_alt) ;
    suivi->Fh     = fabs(freq_alt)  ;
    suivi->Th     = 1 / suivi->Fh ;

  pthread_mutex_unlock(& suivi->mutex_alt );
}
//========================================================================================
// FIXME : CALCUL_PERIODES_SUIVI_MANUEL : 
// FIXME : * est cense calculer les "vrais" periodes et frequences des moteurs pas a pas
//           - en tant que paramtres de la sinusoide de reference
//           - en fonction du suivi manuel effectuee avec les boutons E/N/S/O et reset
// TODO : a modifier car cela marche pas tres bien (cf astro.c : MENU_MANUEL_1)
// TODO : le but de ce suivi est de deduire des actions N-S-O-E de l'utilisateur 
// TODO : les periodes / frequences en azimut et altitude
//========================================================================================

void CALCUL_PERIODES_SUIVI_MANUEL(ASTRE *astre, SUIVI* suivi, VOUTE *voute) {

  double frequence ;
  double azi_rot, alt_rot ;
  
  // calcul de la periode en fonction des informations de suivi
  // renvoyer par les boutons de la raquette
  // on deduit la frequence du nombre de pas comptes et de la periode ecoulee
  
  if ( suivi->pas_azi !=  suivi->pas_azi_old ) {
    if ( suivi->pas_azi !=0 ) {
      if ( AZI_ROT == 0 ) azi_rot = -1 ; else azi_rot = 1 ;
    
      //frequence = azi_rot * (double)suivi->pas_azi / suivi->d_appui_raq_azi ;
      frequence = azi_rot * (double)suivi->pas_azi ;
    
      pthread_mutex_lock(& suivi->mutex_azi );
  
        astre->Va     = frequence * DIV * PIPI / ( voute->acc * AZI_R ) ;
        suivi->Sa_old = suivi->Sa ; 
        suivi->Sa     = (int)SGN(frequence)  ;
        suivi->Fa     = fabs(frequence) ;
        suivi->Ta     = 1 / suivi->Fa ;
    
      pthread_mutex_unlock(& suivi->mutex_azi );
    
      TRACE1("Ta = %2.4f Th = %2.4f Fa = %2.4f Fh = %2.4f\n",suivi->Ta, suivi->Th, suivi->Fa, suivi->Fh) ;
    }
  }
  if ( suivi->pas_alt !=  suivi->pas_alt_old ) {
    if ( suivi->pas_alt !=0 ) {
      if ( ALT_ROT == 0 ) alt_rot = -1 ; else alt_rot = 1 ;
    
      //frequence = alt_rot * (double)suivi->pas_alt / suivi->d_appui_raq_alt ;
      frequence = alt_rot * (double)suivi->pas_alt ;
    
      // fprintf(stderr, "CALCUL_PERIODES_SUIVI_MANUEL demande le mutex\n");
      pthread_mutex_lock( & suivi->mutex_alt );
    
        astre->Vh     = frequence * DIV * PIPI / ( voute->acc * ALT_R ) ;
        suivi->Sh_old = suivi->Sh ; 
        suivi->Sh     = (int)SGN(frequence)  ;
        suivi->Fh     = fabs(frequence) ;
        suivi->Th     = 1 / suivi->Fh ;
  
        // fprintf(stderr, "CALCUL_PERIODES_SUIVI_MANUEL relache le mutex\n");
      pthread_mutex_unlock( & suivi->mutex_alt );
    
      TRACE1("Ta = %2.4f Th = %2.4f Fa = %2.4f Fh = %2.4f\n",suivi->Ta, suivi->Th, suivi->Fa, suivi->Fh) ;
    }
  }
}
//========================================================================================
// FIXME : CALCUL_AFFICHER_HEURE : 
// FIXME : * afficher heure minute seconde et heure decimal
//========================================================================================

void CALCUL_AFFICHER_HEURE( char * mesg, TEMPS *temps ) {

  Trace2("%s : %dh%dmn%ds : %f", mesg, temps->HH, temps->MM, temps->SS, temps->hd ) ;
}
//========================================================================================
// FIXME : CALCUL_HDEC : 
// FIXME : * Convertit heure minutes secondes en heure decimale 
//========================================================================================

void CALCUL_HDEC(TEMPS *temps) {
  
  temps->hd = temps->HH + (temps->MM / 60.0) + (temps->SS / 3600.0)  ;
}
//========================================================================================
// FIXME : CALCUL_HMS : 
// FIXME : * Convertit heure decimale en heure minutes secondes
//========================================================================================

void CALCUL_HMS(TEMPS *temps) {
  
  temps->HH = (int)fabs(  temps->hd ) ;
  temps->MM = (int)fabs( (temps->hd - temps->HH ) * 60.0 ) ;
  temps->SS = (int)fabs(((temps->hd - temps->HH ) * 60.0 - temps->MM ) * 60.0 ) ;
}
//========================================================================================
// FIXME : CALCUL_CONVERSION_ANGLE_EN_TEMPS : 
// FIXME : * convertit les angles (radians) en temps pour representation angles en temps
//========================================================================================

void CALCUL_CONVERSION_ANGLE_EN_TEMPS(ASTRE *astre) {
  
  /* ----------------------------*/
  /* azimut et altitude          */
  /* ----------------------------*/

  (astre->at).hd = astre->a * 24.0 / PIPI ;
  CALCUL_HMS(&astre->at) ;
  
  (astre->ht).hd = astre->h * 24.0 / PIPI ;
  CALCUL_HMS(&astre->ht) ;
  
  /* -----------------------------*/
  /* angle horaire et declinaison */
  /* -----------------------------*/

  (astre->AGHt).hd = astre->AGH * 24.0 / PIPI ;
  CALCUL_HMS(&astre->AGHt) ;
  
  (astre->DECt).hd = astre->DEC * 24.0 / PIPI ;
  CALCUL_HMS(&astre->DECt) ;

  /* -------------------------------*/
  /* les calculs d asscension droite */
  /* -------------------------------*/

  (astre->ASCt).hd  = astre->ASC * 24.0 / PIPI ;
  CALCUL_HMS(&astre->ASCt) ;

  (astre->AGH0t).hd = astre->AGH0 * 24.0 / PIPI ;
  CALCUL_HMS(&astre->AGH0t) ;

  (astre->AGH1t).hd = astre->AGH1 * 24.0 / PIPI ;
  CALCUL_HMS(&astre->AGH1t) ;

  (astre->AGH2t).hd = astre->AGH2 * 24.0 / PIPI ;
  CALCUL_HMS(&astre->AGH2t) ;
}
//========================================================================================
// FIXME : CALCUL_DATE : 
// FIXME : * calcule la date locale servant a tous les calculs ulterieurs
//========================================================================================

int CALCUL_DATE(TEMPS *temps) {
  
  time_t t ;
  
  struct tm *tm ;
  
  temps->HH = 0 ;
  temps->MM = 0 ;
  temps->SS = 0 ;
  
  temps->yy = 0 ;
  temps->dd = 0 ;
  temps->mm = 0 ;
  
  time(&t) ;
  // tzset() ;
  // tm=localtime(&t) ; // pour le temps local (UTC+1 en hiver et UTC+2 en ete)
  tm=gmtime(&t);   // pour UTC
  
  temps->HH = tm->tm_hour ;
  temps->MM = tm->tm_min ;
  temps->SS = tm->tm_sec ;
  
  temps->yy = 1900 + tm->tm_year ;
  temps->dd = tm->tm_mday ;
  temps->mm = tm->tm_mon + 1 ;
  
  CALCUL_HDEC (temps ) ;
  
  //printf("%d %d %d %d %d %d\n",tm->tm_sec,tm->tm_min,tm->tm_hour,tm->tm_mday,tm->tm_mon,tm->tm_year ) ;
  Trace2("%d %d %d %d %d %d %f",temps->yy,temps->mm,temps->dd,temps->HH,temps->MM,temps->SS, temps->hd ) ;
  
  return 0 ;
}
//========================================================================================
// FIXME : CALCUL_JOUR_JULIEN
// FIXME : * calcul la date julienne 
// TODO : * valider / comparer avec valeurs de logiciels astronomiques (stellarium etc..)
//========================================================================================

int CALCUL_JOUR_JULIEN(LIEU* lieu, TEMPS * temps) {
  
  int    yy,mm,A,B,C,Y,M,D ;
  double T ;          // fraction de jour a ajouter au jour Julien	
  double JJ ;         // jour julien
  
  Y  = temps->yy ;
  M  = temps->mm ;
  D  = temps->dd ;
  
  yy = temps->yy ;
  mm = temps->mm ;
  
  lieu->JJ = 0 ;
  JJ=0 ;
  
  if ( temps->mm < 3) {
    yy = temps->yy  - 1 ;
    mm = temps->mm + 12 ;
  }
  A  = yy ;
  C  = floor(yy / 100  ) ;  
  B  = 2-C+floor(C/4) ;
  
  Trace2("%d %d %d %d %d %d",temps->yy,temps->mm,temps->dd,temps->HH,temps->MM,temps->SS ) ;
  
  T  = (double)temps->HH / 24.0 + (double)temps->MM / 1440.0 + (double)temps->SS / 86400.0 ;
  
  Trace2("A B C T = %d %d %d %8.8f",A,B,C,T) ;
  
  JJ = floor(365.25*(A+4716.0))+floor(30.6001*(mm+1))+(temps->dd)+T+B-1524.5 ;
  
  Trace2("Date Julienne (Jour Julien corriger avec le temps local) = %7.9f\n",JJ) ;
  
  JJ = 367*Y - 7*(Y + ( M + 9) / 12 ) / 4 + 275 * M / 9 + D + 1721014 - 0.5 + T ;
  
  Trace2("Date Julienne = %7.9f",JJ) ;
  
  lieu->JJ = JJ ;
  
  return 0 ;
}
//========================================================================================
// FIXME : CALCUL_TEMPS_SIDERAL
// FIXME :  * calcul le temps sideral local en fonction du jour julien, de la longitude
// FIXME :  * a besoin de CALCUL_DATE  et de CALCUL_JOUR_JULIEN et du LIEU
// TODO  : valider 1ere methode / 2eme methode et tester sur le ciel ou comparer
// TODO  : avec valeurs de logiciels astronomiques (stellarium etc..)
//========================================================================================

int CALCUL_TEMPS_SIDERAL(LIEU* lieu, TEMPS *temps ) {
  
  double d, T, TSMG, TSMG2, TSH, TSMGH, TSMGH2, TSMH, TSMH2, GMSTO, GMST, LMST ;
  const double P = 2451545.5 ; 
  
  CALCUL_DATE( temps ) ;   
  CALCUL_JOUR_JULIEN  ( lieu, temps ) ;
  
  TEMPS TSMG3 ;
  TEMPS TSMGH3 ;
  TEMPS TSMH3 ;

  if ( lieu->JJ == 0 ) return 1 ;
  
  d = lieu->JJ - P ;
  T = d / 36525.0 ;
  T = ( lieu->JJ - P) / 36525.0 ; 
  
  Trace2("T = %f",T);
  
  // =======================  Premiere methode ==========================
  
  TSMG  = 100.46061837 + 36000.770053608 * T + 0.000387933*T*T - ( T*T*T ) / 38710000.0 ;   
  //TSMG  = 100.46061837 + 36000.770053608 * T ;
  TSMG2 = fmod(TSMG,360.0) ;
  TSMG3.hd = TSMG2 * 24 / 360 ;
  
  CALCUL_HMS( & TSMG3 ) ;
  
  TSH   = temps->hd * 1.00273790935 * 15 ;
  
  TSMGH  = TSMG + TSH ;
  
  TSMGH2 = fmod(TSMGH, 360.0) ;
  TSMGH3.hd = TSMGH2 * 24 / 360 ;
  CALCUL_HMS( & TSMGH3) ;
  
  TSMH  = TSMGH - lieu->lon ;
  
  TSMH2 = fmod(TSMH, 360.0) ;
  TSMH3.hd = TSMH2 * 24.0 / 360.0 ;
  CALCUL_HMS( & TSMH3 ) ;
  
  CALCUL_AFFICHER_HEURE("1ere methode - Temps sideral Greenwich 0hTU",& TSMG3) ;
  CALCUL_AFFICHER_HEURE("1ere methode - Temps sideral Greenwich tps local (TU)",& TSMGH3) ;
  CALCUL_AFFICHER_HEURE("1ere methode - Temps sideral Lieu Local + Heure Locale (TU)",& TSMH3) ;
  
  // =======================  Deuxieme methode ==========================
  
  GMSTO = 100.460618375 + 0.985647366286 * d ;
  GMST  = GMSTO + 1.00273790935 * 15 * temps->hd  ;
  GMST  = GMSTO + 1.002737 * 15 * temps->hd  ;
  
  LMST = GMST + lieu->lon ; // FIXME : attention !! ici la longitude du lien intervient !!
  
  TSMG2  = fmod(GMSTO, 360.0) ;
  TSMGH2 = fmod(GMST,  360.0) ;
  TSMH2  = fmod(LMST,  360.0) ;
  
  TSMG3.hd  = TSMG2  * 24.0 / 360.0 ;
  TSMGH3.hd = TSMGH2 * 24.0 / 360.0 ;
  TSMH3.hd  = TSMH2  * 24.0 / 360.0 ;
  
  CALCUL_HMS( & TSMG3) ;
  CALCUL_HMS( & TSMGH3 ) ;
  CALCUL_HMS( & TSMH3) ;
  
  CALCUL_AFFICHER_HEURE("2eme methode - Temps sideral Greenwich 0hTU",& TSMG3) ;
  CALCUL_AFFICHER_HEURE("2eme methode - Temps sideral Greenwich tps local (TU)",& TSMGH3) ;
  CALCUL_AFFICHER_HEURE("2eme methode - Temps sideral Lieu Local + Heure Locale (TU)",& TSMH3) ;
  
  lieu->TS  = TSMH3.hd ;
  lieu->TSR = (TSMH3.hd / 24) * PIPI ;
  
  Trace2("Methode 1 : lieu->JJ = %f lieu->TSR = %f",lieu->JJ, lieu->TSR ) ;
  Trace2("Methode 1 : lieu->JJ = %f lieu->TSR = %f",lieu->JJ, lieu->TSR ) ;
  Trace2("Methode 1 : lieu->JJ = %f lieu->TSR = %f",lieu->JJ, lieu->TSR ) ;

  return 0 ;
}
//========================================================================================
// FIXME : CALCUL_ANGLE_HORAIRE
// FIXME :  * calcul de l angle horaire de l astre 
// FIXME :  * a besoin de TSR et ASC , donc de CALCUL_TEMPS_SIDERAL
//========================================================================================

void CALCUL_ANGLE_HORAIRE( LIEU *lieu, ASTRE *astre ) {
  
  // en radians 
  
  astre->AGH = lieu->TSR - astre->ASC ;
  
  CALCUL_CONVERSION_ANGLE_EN_TEMPS( astre) ;

  Trace2("lieu->JJ    = %f\tlieu->TSR     = %f", lieu->JJ,    lieu->TSR ) ;
  Trace2("astre->AGH = %f\tastre->ASC = %f", astre->AGH, astre->ASC ) ;
}
//========================================================================================
// FIXME : CALCUL_ASCENSION_DROITE
// FIXME :  * calcul de l ascension droite de l astre en fonction de l'angle horaire
// FIXME :  * a besoin de TSR et ASC , donc de CALCUL_TEMPS_SIDERAL
//========================================================================================

void CALCUL_ASCENSION_DROITE( LIEU *lieu, ASTRE *astre ) {
  
  // en radians 
  
  astre->ASC = lieu->TSR - astre->AGH ;
  
  CALCUL_CONVERSION_ANGLE_EN_TEMPS( astre) ;
  
  Trace2("lieu->JJ    = %f\tlieu->TSR     = %f", lieu->JJ,    lieu->TSR ) ;
  Trace2("astre->AGH = %f\tastre->ASC = %f", astre->AGH, astre->ASC ) ;
}
//========================================================================================
// FIXME : CALCUL_TOUT
// FIXME :  * rafraichit l integralite des calculs 
// FIXME :  * a besoin de tous les calculs , ainsi que du mode 
//========================================================================================

void CALCUL_TOUT(void) {
  
  int pos=3;
  int len=0 ;
  char c_sub[32];

  Trace2("") ;

  /*---------------- evolution 19/01/2022 ----------------
  * prise en compte du fait que la astre peut avoir plusieurs
  * prefixe dans son nom (PLA/NGC/MES/ETO mais aussi EQU/AZI)
  * Cela permet de determine le mode de calculs de l'astre
  * et le type de l'astre (CIEL_PROFOND etc...)
  -------------------------------------------------------*/

  if      ( strstr( astre->nom, CONFIG_MES ) != NULL ) astre->type = ASTRE_CIEL_PROFOND ;
  else if ( strstr( astre->nom, CONFIG_NGC ) != NULL ) astre->type = ASTRE_CIEL_PROFOND ;
  else if ( strstr( astre->nom, CONFIG_ETO ) != NULL ) astre->type = ASTRE_CIEL_PROFOND ;
  else if ( strstr( astre->nom, CONFIG_PLA ) != NULL ) astre->type = ASTRE_PLANETE ;
  else if ( strstr( astre->nom, CONFIG_AZI ) != NULL ) astre->type = ASTRE_INDETERMINE ;
  else if ( strstr( astre->nom, CONFIG_EQU ) != NULL ) astre->type = ASTRE_INDETERMINE ;
  else                                                 astre->type = ASTRE_INDETERMINE ;
  /* TODO : completer avec les types manquants */ 

  if      ( strstr( astre->nom, CONFIG_MES ) != NULL ) astre->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  else if ( strstr( astre->nom, CONFIG_NGC ) != NULL ) astre->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  else if ( strstr( astre->nom, CONFIG_ETO ) != NULL ) astre->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  else if ( strstr( astre->nom, CONFIG_PLA ) != NULL ) astre->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  else if ( strstr( astre->nom, CONFIG_AZI ) != NULL ) astre->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  else if ( strstr( astre->nom, CONFIG_EQU ) != NULL ) astre->mode = MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL ;
  else                                                 astre->mode = MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL ; 
  /* TODO : completer avec les types manquants */ 

  /* recuperation du numero apres les 3 premiers caracteres */

	len=strlen(astre->nom)-pos ;
  memset( c_sub, 0, sizeof(c_sub));
  memcpy( c_sub, &astre->nom[pos], len );
  c_sub[len] = '\0';
  astre->numero = atoi(c_sub) ;
  
  switch (astre->type) {

    /* ----------------------------------------------------------------- */
      /* pour un astre indetermine 
         on calcule les coordevices equatoriales 
         necessaires au calcul des vitesses 
      */

    case ASTRE_INDETERMINE :
    
      pthread_mutex_lock( & suivi->mutex_cal );
      
      CALCUL_TEMPS_SIDERAL( lieu, temps ) ;
      /* CALCUL_AZIMUT       ( lieu, astre) ; */

      if ( astre->mode == MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL ) {
        
        CALCUL_EQUATEUR        ( lieu, astre ) ;
        CALCUL_ASCENSION_DROITE( lieu, astre ) ;
      }
      else {
        CALCUL_ANGLE_HORAIRE( lieu, astre ) ;
        CALCUL_AZIMUT       ( lieu, astre) ;
      }

      CALCUL_VITESSES     ( lieu, astre, suivi) ;
      CALCUL_PERIODE      ( astre, suivi, voute) ;
      
      pthread_mutex_unlock( & suivi->mutex_cal ) ;
      
      break ;

    /* ----------------------------------------------------------------- */

    case ASTRE_CIEL_PROFOND :
    
      pthread_mutex_lock( & suivi->mutex_cal );
      
      CALCUL_TEMPS_SIDERAL( lieu, temps ) ;

      CALCUL_ANGLE_HORAIRE( lieu, astre ) ;
      CALCUL_AZIMUT       ( lieu, astre) ;
      
      CALCUL_VITESSES     ( lieu, astre, suivi) ;
      CALCUL_PERIODE      ( astre, suivi, voute) ;
      
      pthread_mutex_unlock( & suivi->mutex_cal ) ;
      
      break ;
      
    /* ----------------------------------------------------------------- */
    
    case ASTRE_PLANETE :
      
      pthread_mutex_lock( & suivi->mutex_cal );
            
      CALCUL_TEMPS_SIDERAL( lieu, temps ) ;
      
      SOLAR_SYSTEM( astre->nom,\
                      & astre->AGH0,\
                      & astre->DEC,\
                      & astre->a,\
                      & astre->h ,\
                        lieu->lat, lieu->lon, lieu->alt,\
                        temps->yy, temps->mm, temps->dd,temps->HH, temps->MM, temps->SS,\
                        astre->numero) ;
                        
      CALCUL_ANGLE_HORAIRE( lieu, astre ) ;
      
      // FIXME : pour les planetes , le calcul de l'azimut / altitude 
      // FIXME : ainsi que coordevices equatoriales / declinaison
      // FIXME : est directement retourner par la fonction SOLAR_SYSTEM
      // TODO  : verifier que le CALCUL_AZIMUT en fonction des coordevices siderales
      // TODO  : ne modifie pas azimut et altitude calcules par SOLAR_SYSTEM
      
      // CALCUL_AZIMUT  ( lieu, astre) ;
      
      CALCUL_VITESSES ( lieu, astre, suivi) ;
      CALCUL_PERIODE  ( astre, suivi, voute) ;

      pthread_mutex_unlock( & suivi->mutex_cal ) ;
      
      break ;

      //---------------------------------------------------------------------------------------
      
      case ASTRE_SATELLITE :
        /* TODO completer */
        break ;

      //---------------------------------------------------------------------------------------

      case ASTRE_COMETE :
        /* TODO completer */
        break ;

      //---------------------------------------------------------------------------------------

      default :
        break ;

      //---------------------------------------------------------------------------------------

      /* Le cas suivant est ancien : considerer le type de calcul
         (nouveau code 2022) MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL
         ou bien MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL */

      /* 
      case CAPTEURS :

        CALCUL_TEMPS_SIDERAL( lieu, temps ) ;
        
        // TODO : modifier / completer / corriger ..
        
        if ( devices->DEVICE_CAPTEURS_USE ) { 
          astre->a = suivi->pitch ;         // FIXME : donne azimut
          astre->h = suivi->heading ;       // FIXME : donne altitude 
          CALCUL_EQUATEUR ( lieu, astre) ;  // FIXME : donnes ASC et DEC
        }
            
        CALCUL_ANGLE_HORAIRE( lieu, astre ) ;
        CALCUL_AZIMUT  ( lieu, astre) ;
        
        break ;
      */
  }  
}
//================================================================================================
void CALCUL_VOUTE(void ) {
  
  FILE   *fout ;
  // double A, H ;
  double a, h ;
  
  fout=fopen("voute.csv","w") ;
  
  // On fait varier les coordevices HORAIRES
  
  for(h=-(PI/2)+(lieu->lat)+0.001;h<PI/2;h+=voute->pas)
    if (h>=0) 
    for(a=-PI +0.001 ;a<PI;a+=voute->pas){
     
     astre->a=a ;
     astre->h=h ;
     
     CALCUL_EQUATEUR  ( lieu, astre) ; // calcul coordevices horaires en fait
     CALCUL_VITESSES  ( lieu, astre, suivi) ; // TODO : verifier suivi->SUIVI_EQUATORIAL avant
     
     astre->V  = sqrt( sqr( astre->Va ) + sqr( astre->Vh )) ;
     
     if ( astre->Va != 0) 
       astre->An = atan( astre->Vh / astre->Va ) ;
     else
       astre->An = PI/2 ;
     
     CALCUL_GEODE( astre ) ;
     
     Trace("%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f\n", \
       astre->a * DEGRES, \
       astre->h * DEGRES, \
       astre->AGH * DEGRES, \
       astre->DEC * DEGRES, \
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
//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================

      // si un alignement vient d etre effectue on affiche les vitesses calculees
      // TODO : verifier / completer / modifier ..
      /*
      if ( suivi->SUIVI_ALIGNEMENT  ) {

        TRACE("%s : a %d h %d A %d H %d : Va %.2f Vh %.2f Ta %.2f Th %.2f Fa=%.2f Fh %.2f",\
          astre->nom,\
          (int)((astre->a)*DEGRES),\
          (int)((astre->h)*DEGRES),\
          (int)((astre->AGH)*DEGRES),\
          (int)((astre->DEC)*DEGRES),\
          astre->Va,\
          astre->Vh,\
          suivi->Ta,\
          suivi->Th,\
          suivi->Fa,\
          suivi->Fh ) ;

        TRACE("Va=%2.4f Vh=%2.4f Ta=%2.4f Th=%2.4f Fa=%2.4f Fh=%2.4f Fam = %ld Fhm = %ld Tac = %f Thc = %f\n",\
              astre->Va,\
              astre->Vh,\
              suivi->Ta,\
              suivi->Th,\
              suivi->Fa,\
              suivi->Fh,\
              (suivi->Ia - suivi->Ia_prec),(suivi->Ih - suivi->Ih_prec ),suivi->Tac, suivi->Thc) ;
        
        suivi->SUIVI_ALIGNEMENT = 0 ;

      }
      */




