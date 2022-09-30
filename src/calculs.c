#define _GNU_SOURCE

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 15/11/2021  | * (types.h) modification des types enum et contenu enum
#               * (types.h) modification ordre des menus (MENU_AZIMUTAL=0)
# 19/01/2022  | * gestion du cas ou l as n a pas de nom
#                 c'est a dire q'on effectue les calculs en fonction des coordonnees
#                 sans l'information de nom (recherche catagues inutiles)
# 21/01/2022  | * recuperation numero objet (PLA5 => 5, MES10 -> 10, ..)
#               * passage dernier arg de SOLAR_SYSTEM abec gp_Ast->numero
# 12/03/2022  | * correction calcul DEC (declinaison) - => + dans formule
# 
# mars  2022  | * reprise des calculs et ajout de fonctions de conversions
#               * correction deuxieme argument appel SOLAR_SYSTEM
# -------------------------------------------------------------- 
*/

#include <calculs.h>

double sqr(double x)                                    { return x*x ; }
double max(double x1,double x2)                         { if(fabs(x1)>fabs(x2)) return x1; else return x2 ; }
double SGN(double x)                                    { if(x>=0.0) return 1.0; else return -1.0 ; }

/*
double ASC  (double H,   double a, double h)            { return asin(sin(a)*cos(h)/cos(H));}
double ASC1 (double lat, double H, double a, double h)  { return acos((cos(a)*cos(h)+cos(lat)*sin(H))/(sin(lat)*cos(H))) ;}
double ASC2 (double lat, double H, double h)            { return acos((sin(h)-sin(H)*sin(lat))/(cos(H)*cos(lat))) ;}
double ALT  (double lat, double A, double H)            { return asin(sin(lat)*sin(H)+cos(lat)*cos(H)*cos(A));}
double AZI  (double A, double H, double h)              { return asin( sin(A) * cos(H) / cos(h));}
double AZI1 (double lat, double A, double H, double h)  { return acos((sin(lat)*cos(H)*cos(A)-cos(lat)*sin(H))/cos(h)) ;}
double RAD  (int degres, int minutes )                  { return ((double)degres + ( SGN(degres)*(double)minutes) / 60.0 ) / DEGRES ; }
double DEG  (int degres, int minutes )                  { return (double)degres  + ( SGN(degres)*(double)minutes) / 60.0 ; }

*/
/* 
   => latitude lat 
   => azimut            azi et altitude  alt
   => ascension droite  asc et déclinaison dec 
   => agh = angle horaire

azimutal => equatorial

sin (dec) = sin (alt) sin (lat) + cos (alt) cos (lat) cos (azi)
sin (agh) = - sin (azi) cos (alt) / cos ( dec )
cos (agh) = {sin (alt) - sin (dec) sin (lat)} / cos (dec) cos (lat)

asc = t - H

equatorial => azimutal

agh = t - asc

sin (alt) =   sin (dec) sin (lat) + cos (dec) cos (lat) cos (agh)
sin (azi) = - sin (agh) cos ( dec ) / cos (alt)
cos (azi) = {sin (dec) - sin (alt) sin (lat)} / { cos (alt) cos (lat)}

*/ 
/*    A : angle horaie
      H : declinaison
      a : azimut
      h : altitude
  h  = ALT(lat,A,H) ;
  a0 = AZI(A,H,h) ;
  a1 = AZI1(lat,A,H,h) ;
  af = SGN(a0)*a1 ;
*/

/* CONVENTIONS :
- angle horaie midi au nord , 00h au sud 
- azimut 0 / 360 au nord , 
*/

double DEC  (double lat, double azi, double alt)     { return asin( sin(lat)*sin(alt) + cos(lat)*cos(alt)*cos(azi));}
double ALT  (double lat, double agh, double dec)     { return asin( sin(lat)*sin(dec) + cos(lat)*cos(dec)*cos(agh));}

double AGH0 (double azi,  double alt, double dec)     { return asin( -1.0 * sin(azi) * cos(alt) / cos(dec));}
double AZI0 (double agh,  double dec, double alt)     { return asin( -1.0 * sin(agh) * cos(dec) / cos(alt));}

double AGH1  (double lat, double azi, double alt, double dec)  { return acos((sin(lat)*cos(alt)*cos(azi)-cos(lat)*sin(alt))/cos(dec)) ;}
double AZI1  (double lat, double agh, double dec, double alt)  { return acos((sin(lat)*cos(dec)*cos(agh)-cos(lat)*sin(dec))/cos(alt)) ;}

double AGH2 (double lat, double dec, double alt)   { return acos(((sin(alt) - sin(dec)*sin(lat)) / cos(dec)) * cos(lat) ) ;}
double AZI2 (double lat, double alt, double dec)   { return acos(( sin(dec) - sin(alt)*sin(lat)) / ( cos(alt) * cos(lat))) ;}

double RAD  (int degres, int minutes )                  { return ((double)degres + ( SGN(degres)*(double)minutes) / 60.0 ) / DEGRES ; }
double DEG  (int degres, int minutes )                  { return (double)degres  + ( SGN(degres)*(double)minutes) / 60.0 ; }

/*****************************************************************************************
* @fn     : CALCUL_REDUCTION_TOTALE
* @author : s.gravois
* @brief  : permet de calculer unela reduction totale de la vitesse 
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
*****************************************************************************************/

double CALCUL_REDUCTION_TOTALE() {


   return 0.0 ;
}
/*****************************************************************************************
* @fn     : CALCUL_DUREE_SECONDES
* @author : s.gravois
* @brief  : permet de calculer une duree depuis un dernier appel a cette meme fonction
* @brief  : resultat en secondes
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
*****************************************************************************************/

double CALCUL_DUREE_SECONDES(struct timeval *t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  gettimeofday(&t01,NULL) ;

  t_diff = GPIO_MICRO_SEC * ( t01.tv_sec  - t00->tv_sec ) + ( t01.tv_usec - t00->tv_usec ) ;
  t_diff = t_diff / GPIO_MICRO_SEC ;

  t00->tv_sec  = t01.tv_sec ; 
  t00->tv_usec = t01.tv_usec ;

  return t_diff  ;
}

/*****************************************************************************************
* @fn     : CALCUL_DUREE_MICROSEC
* @author : s.gravois
* @brief  : permet de calculer une duree depuis un dernier appel a cette meme fonction
* @brief  : resultat en microsecondes
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
*****************************************************************************************/

double CALCUL_DUREE_MICROSEC(struct timeval *t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  gettimeofday(&t01,NULL) ;

  t_diff = GPIO_MICRO_SEC * ( t01.tv_sec  - t00->tv_sec ) + ( t01.tv_usec - t00->tv_usec ) ;

  t00->tv_sec  = t01.tv_sec ; 
  t00->tv_usec = t01.tv_usec ;

  return t_diff ;
}

/*****************************************************************************************
* @fn     : CALCUL_DUREE_NANOSEC
* @author : s.gravois
* @brief  : permet de calculer une duree depuis un dernier appel a cette meme fonction
* @brief  : resultat en nanosecondes
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
* @todo   : finir ( modifier pour passer en nanosecondes)
*****************************************************************************************/

double CALCUL_DUREE_NANOSEC(struct timeval *t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  gettimeofday(&t01,NULL) ;

  t_diff = GPIO_MICRO_SEC * ( t01.tv_sec  - t00->tv_sec ) + ( t01.tv_usec - t00->tv_usec ) ;

  t00->tv_sec  = t01.tv_sec ; 
  t00->tv_usec = t01.tv_usec ;

  return t_diff ;
}

/*****************************************************************************************
* @fn     : CALCUL_TEMPORISATION_VOUTE
* @author : s.gravois
* @brief  : permet de faire une temporisation (1s) dans la boucle infinie de SUIVI_VOUTE
* @brief  : utilise un usleep() sur 99% du temps de la temporisation (cf calibration_voute)
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
* @todo   : verifier si on peut augmenter la valeur de calibration_voute en augmentant
*           la priorite du thread egalement
*****************************************************************************************/

long CALCUL_TEMPORISATION_VOUTE(VOUTE *gp_Vou, struct timeval t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  usleep ( (long)(gp_Vou->DT * gp_Vou->pourcentage_tempo) ) ;
  
  t_diff=0;
  
  // FIXME : rappel : gp_Vou->calibration_voute = 0.99 normalement
  // FIXME : pour eviter de faire appel a la boucle while 99% du temps
  
  while( t_diff < ( gp_Vou->DT * gp_Vou->calibration_voute ) ) {
     
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

void CALCUL_TEMPORISATION_ALTITUDE(SUIVI * gp_Sui, struct timeval * pt00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  // temporisation pour ne pas consommer de CPU
  // on sommeille = pourcentage_tempo du usleep 
  
  usleep ( (long)(gp_Sui->DTh * gp_Sui->pourcentage_tempo) ) ;
  
  t_diff=0;
  while( t_diff < gp_Sui->DTh ) {
     
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

void CALCUL_TEMPORISATION_AZIMUT(SUIVI * gp_Sui, struct timeval * pt00) {
  
  struct timeval t01 ;
  double t_diff ;
  
  // temporisation pour ne pas consommer de CPU
  // on sommeille = pourcentage_tempo du usleep 
  
  usleep ( (long)(gp_Sui->DTa * gp_Sui->pourcentage_tempo) ) ;
  
  t_diff=0;
  while( t_diff < gp_Sui->DTa ) {
     
   gettimeofday(&t01,NULL) ;
   t_diff = (( t01.tv_sec - pt00->tv_sec)  * GPIO_MICRO_SEC ) + t01.tv_usec - pt00->tv_usec;
  }
  gettimeofday(pt00,NULL) ;
}
//========================================================================================
// FIXME : CALCUL_GEODE : 
// FIXME : * calcule les coordonnees (x,y,z) dans R3 a partir de azimut et altitude
// FIXME : * en supposant que la voute est de diametre = 1
//========================================================================================

void CALCUL_GEODE(ASTRE *gp_Ast) {

  // on deduit de l'azimut et de l'altitude
  // les coordonnees x y et z dans la geode d'observation
  
  gp_Ast->x = cos( gp_Ast->h ) * cos( gp_Ast->a )  ;
  gp_Ast->y = cos( gp_Ast->h ) * sin( gp_Ast->a ) ;
  gp_Ast->z = sin( gp_Ast->h ) ;
  
  // On projette la sphere de rayon=1 sur une autre sphere de rayon la valeur qu'on veut
  // ce systeme permet de voir en 3D une valeur en fonction de 3 autres
  // Ici on veut representer la vitesse (norme du vecteur) 
  // en fonction de x y et z (x y z directions du vecteur sur la sphere unite)
  
  gp_Ast->xx = gp_Ast->x * gp_Ast->V ;
  gp_Ast->yy = gp_Ast->y * gp_Ast->V ;
  gp_Ast->zz = gp_Ast->z * gp_Ast->V ;
  
}
//========================================================================================
// FIXME : CALCUL_AZIMUT : 
// FIXME : * transforme les coordonnees siderales en coordonnees azimutales
//========================================================================================

void CALCUL_AZIMUT(LIEU *gp_Lie, ASTRE *gp_Ast) {
  
  // transforme les coordonnees "sid�rales" en coordonnees azimutales
  // permet de trouver l'azimut et l'altitude en fonction de l'angle horaire
  // par rapport au m�ridien (SUD) et la declinaison
  // ATTENTION !! angle horaire = ascension droite sid�rale corrig�e
  // avec le temps sid�ral : une conversion doit etre faite pour trouver
  // gp_Ast->AGH  = TS - (asc droite absolue)
  // Une fonction de calcul du temps sid�ral doit etre faite ult�rieurement 
  
  double lat,A,H,a0,h,a1,a2,af ;
  
  lat= gp_Lie->lat ;
  A  = gp_Ast->AGH ; // FIXME : angle horaire calcule prealablement dans astro.c en theorie
  H  = gp_Ast->DEC ; // FIXME : declinaison - seule valeur qui ne change pas 
  
  TRACE1("agh = %2.3f\tH = %2.3f\t",(gp_Ast->AGH)*DEGRES,(gp_Ast->DEC)*DEGRES) ;

  h  = ALT(lat,A,H) ;
  gp_Ast->h = h ;

  a0 = AZI0(A,H,h) ;
  a1 = AZI1(lat,A,H,h) ;
  a2 = AZI2(lat,h,H) ;

  gp_Ast->a = SGN(a0)*a2 ;

  /* Si azimut negatif on ajoute 360 degres */  
  
  if ( gp_Ast->a < 0 ) {
    gp_Ast->a += 2*PI ;
  }

   // resultats de calculs : pour tests (a modifier : supprimer)
  
  gp_Ast->AZI0 = a0 ;
  gp_Ast->AZI1 = a1 ;
  gp_Ast->AZI2 = a2 ;

  CALCUL_CONVERSIONS_ANGLES( gp_Ast) ;

  Trace1("AZI0 = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AZI0t.HH, \
    gp_Ast->AZI0t.MM , \
    gp_Ast->AZI0t.SS, \
    gp_Ast->AZI0 * DEGRES );

  Trace1("AZI1 = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AZI1t.HH, \
    gp_Ast->AZI1t.MM , \
    gp_Ast->AZI1t.SS, \
    gp_Ast->AZI1 * DEGRES );

  Trace1("AZI2 = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AZI2t.HH, \
    gp_Ast->AZI2t.MM , \
    gp_Ast->AZI2t.SS, \
    gp_Ast->AZI2 * DEGRES );

  Trace1("azi = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AGHt.HH, gp_Ast->AGHt.MM , gp_Ast->AGHt.SS, gp_Ast->AGH * DEGRES );

  Trace1("alt = %f (deg)" , (gp_Ast->h)*DEGRES) ;

}
//========================================================================================
// FIXME : CALCUL_EQUATEUR : 
// FIXME : * transforme les coordonnees azimutales en coordonnees siderales (angle horaire)
// TODO :  * probleme : les calculs ne sont pas bons (a verifier)
// TODO :  * cette fonction servira quand on devra avoir les coordonnees equatoriales
//          en fonction des coordonnnees azimutales , pour le calcul des VITESSES,
//          qui utilise les coordonnees equatoriales 
//========================================================================================

void CALCUL_EQUATEUR(LIEU *gp_Lie, ASTRE *gp_Ast) {

  // transforme les coordonnees azimutales en coordonnees siderales (angle horaire)
  // permet de trouver les coordonn�es siderales en fonction de l'azimut et de l'altitude
  // Pour avoir la correspondance avec les coordonnn�es �quatoriales brutes, il faut
  // utiliser la conversion AH = TS - ASC
  // AH  : angle horaire
  // TS  : temps sid�ral
  // ASC : ascension droite brute
  
  double A0,A2,A1,a,h,lat,Af,agh,H ;

  TRACE1("avant calcul => a = %2.3f\th = %2.3f\t=>agh = %2.3f\tH=%2.3f",\
    (gp_Ast->a)   * DEGRES,\
    (gp_Ast->h)   * DEGRES,\
    (gp_Ast->AGH)* DEGRES,\
    (gp_Ast->DEC) * DEGRES) ;

  lat      = gp_Lie->lat ;
  a        = gp_Ast->a ;
  h        = gp_Ast->h ;
  
  // FIXME : obtention de declinaison et ascension droite (d'apres formules usuelles de Gauss)
  
  H  = DEC(lat,a,h) ;
  gp_Ast->DEC  = H ;

  A0 = AGH0(a,h,H) ;       /* correct , ajouter 2*PI si negatif */
  A1 = AGH1(lat,a,h,H)+PI; /* correct , ajouter PI */ 
  A2 = AGH2(lat,H,h) ;     /* a anlyser = mauvais */ 
  
  if ( A0 < 0 ) {
    A0 += 2 * PI ;
  }
  // gp_Ast->AGH = SGN(A0)*A1  ;

  gp_Ast->AGH = A0 ;

  // resultats de calculs : pour tests (a modifier : supprimer)

  gp_Ast->AGH0 = A0 ;
  gp_Ast->AGH1 = A1 ;
  gp_Ast->AGH2 = A2 ;
  
  CALCUL_CONVERSIONS_ANGLES( gp_Ast) ;
  
  Trace1(" %s : ASC = %d.%d.%d (hms) %.2f (deg) %.2f (rad)", \
       gp_Ast->nom , \
       gp_Ast->ASCt.HH, \
       gp_Ast->ASCt.MM, \
       gp_Ast->ASCt.SS, \
       gp_Ast->ASC * DEGRES, \
       gp_Ast->ASC \
  ) ; 

  Trace1("AH0   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AGH0t.HH, gp_Ast->AGH0t.MM , gp_Ast->AGH0t.SS, gp_Ast->AGH0 * DEGRES );

  Trace1("AH1   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AGH1t.HH, gp_Ast->AGH1t.MM , gp_Ast->AGH1t.SS, gp_Ast->AGH1 * DEGRES );

  Trace1("AH2   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AGH2t.HH, gp_Ast->AGH2t.MM , gp_Ast->AGH2t.SS, gp_Ast->AGH2 * DEGRES );

  Trace1("AH    (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AGHt.HH, gp_Ast->AGHt.MM , gp_Ast->AGHt.SS, gp_Ast->AGH * DEGRES );

  Trace1("DEC   (deg) = %f" , (gp_Ast->DEC)*DEGRES) ;

  Trace1("apres calcul =>a = %2.3f\th = %2.3f\t=>agh = %2.3f\tH=%2.3f",\
   (gp_Ast->a)*DEGRES,\
   (gp_Ast->h)*DEGRES,\
   (gp_Ast->AGH)*DEGRES,\
   (gp_Ast->DEC)*DEGRES) ;
} 

//========================================================================================
// FIXME : CALCUL_VITESSES_EQUATORIAL : 
// FIXME : * remet a jour Va et Vh (vitesses sur les moteurs azimut et altitude) 
//           comme vitesses equatoriales theoriques (15" / s ) 
//========================================================================================

void CALCUL_VITESSES_EQUATORIAL(ASTRE *gp_Ast) {

  gp_Ast->Va = -15.0 ;
  gp_Ast->Vh = -15.0 ;
}
//========================================================================================
// FIXME : CALCUL_VITESSES : 
// FIXME : * calcule les vitesses azimutales Va et Vh (vitesses sur les moteurs azimut et altitude) 
//========================================================================================

void CALCUL_VITESSES(LIEU *li,ASTRE *gp_Ast, SUIVI * gp_Sui) {

  // le calcul des vitesses est fait avec
  // l'angle horaire et la declinaison (azimut et altitude inutiles)
 
  double G, Va, Vh, angle ;
  
  if ( gp_Sui->SUIVI_EQUATORIAL == 1 ) {
    gp_Ast->Va = -15.0 ;
    gp_Ast->Vh = -15.0 ;
  }
  else {
    angle=ANGLE_S ;
    G  = sqr(sin(gp_Ast->AGH))*sqr(cos(gp_Ast->DEC))+sqr(sin(li->lat)*cos(gp_Ast->DEC)*cos(gp_Ast->AGH)-cos(li->lat)*sin(gp_Ast->DEC)) ;
    Va = angle*(cos(gp_Ast->DEC)*(cos(li->lat)*cos(gp_Ast->AGH)*sin(gp_Ast->DEC)-sin(li->lat)*cos(gp_Ast->DEC)))/G ;
    Vh = angle*sin(gp_Ast->AGH)*cos(gp_Ast->DEC)*cos(li->lat)/sqrt(G) ;
    gp_Ast->Va=Va ;
    gp_Ast->Vh=Vh ;
  }
  TRACE1("%2.0f\t%2.8f\t%2.4f\t=>\t%2.8f\t%2.8f\n",(li->lat)*DEGRES,(gp_Ast->AGH)*DEGRES,(gp_Ast->DEC)*DEGRES,Va,Vh) ;
}
//========================================================================================
// FIXME : CALCUL_D : 
// FIXME : * calcule les nombres a injecter dans des diviseurs de frequence 
// TODO :  * inutilise (a servi au debut du projet) 
// TODO :  * a supprimer ?
//========================================================================================

void CALCUL_D(ASTRE *gp_Ast, SUIVI * gp_Sui) {
  
  gp_Sui->Da = AZI_F * 2.0 * PI / ( AZI_R * pow(2.0, AZI_N) * ( gp_Ast->Va / ( 60 * 60 * DEGRES ) )) ;
  gp_Sui->Dh = ALT_F * 2.0 * PI / ( ALT_R * pow(2.0, ALT_N) * ( gp_Ast->Vh / ( 60 * 60 * DEGRES ) )) ;
}

/*****************************************************************************************
* @fn     : CALCUL_PERIODE
* @author : s.gravois
* @brief  : calcule les "vraies" periodes et frequences des moteurs pas a pas
* @brief  : en tant que parametres de la sinusoide de reference 
* @param  : ASTRE * gp_Ast
* @param  : SUIVI * gp_Sui
* @param  : VOUTE * gp_Vou
* @date   : 2022-05 ajout non prise en compte des micro-pas pour la frequence moteur
* @date   : 2022-06 correction petite erreur sur Fa_mot / Fh_mot
* @date   : 2022-06 ajout champs xxx_bru freq /periode avant multiplication par acceleration
*****************************************************************************************/

void CALCUL_PERIODE(ASTRE *gp_Ast,SUIVI * gp_Sui, VOUTE *gp_Vou) {

  double freq_alt_mic, freq_azi_mic ;
  double freq_alt_mot, freq_azi_mot ;
  double freq_alt_bru, freq_azi_bru ;
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
  
  TRACE2("%f %f %f %f %f",gp_Sui->acc_azi, gp_Vou->acc, AZI_R, gp_Ast->Va, azi_rot);
  TRACE2("%f %f %f %f %f",gp_Sui->acc_alt, gp_Vou->acc, AZI_R, gp_Ast->Vh, alt_rot);

  /*------------------------*/
  /* Calculs des frequences */
  /*------------------------*/

  /* calcul des frequences brutes non corrigees = brutes */
  /* ces frequences ne sont pas corrigees par les accelerations diverses */

  freq_azi_bru = AZI_R * gp_Ast->Va * azi_rot / DIV / PIPI / 4  ;
  freq_alt_bru = ALT_R * gp_Ast->Vh * alt_rot / DIV / PIPI / 4  ;

  /* calcul des frequences corrigees avant prise en compte micro pas */
  /* ces frequences sont corrigees par les accelerations diverses */

  freq_azi_mot = gp_Sui->acc_azi_pid * gp_Sui->acc_azi * gp_Vou->acc * freq_azi_bru  ;
  freq_alt_mot = gp_Sui->acc_alt_pid * gp_Sui->acc_alt * gp_Vou->acc * freq_alt_bru  ;

  /* calcul des frequences finales UTILES */
  /* La frequence retenue est la frequence moteur multipliee par le nb de micro pas */

  freq_azi_mic     = freq_azi_mot * AZI_R4 ;
  freq_alt_mic     = freq_alt_mot * ALT_R4 ;

  /*-----------------------------------------------------*/
  /* Le calcul est different si on utilise un controleur                    */
  /* la frequence est la frequence moteur (sans tenir compte des micro pas) */
  /*-----------------------------------------------------*/

  if ( gp_Dev->DEVICE_USE_CONTROLER ) {
    freq_azi_mic   = freq_azi_mot ;
    freq_alt_mic   = freq_alt_mot ;
  }

  /*------------------------*/
  /* Calculs des periodes  */
  /*------------------------*/

  pthread_mutex_lock(& gp_Pth->pth_mut.mut_azi );
      
    gp_Sui->Sa_old = gp_Sui->Sa ; 
    gp_Sui->Sa     = (int)SGN(freq_azi_mic)  ;

    gp_Sui->Fa_mic = fabs(freq_azi_mic )  ;
    gp_Sui->Fa_bru = fabs(freq_azi_bru )  ;    
    gp_Sui->Fa_mot = fabs(freq_azi_mot) ;

    gp_Sui->Ta_mic = 1 / gp_Sui->Fa_mic ;
    gp_Sui->Ta_bru = 1 / gp_Sui->Fa_bru ;
    gp_Sui->Ta_mot = 1 / gp_Sui->Fa_mot ;

  pthread_mutex_unlock(& gp_Pth->pth_mut.mut_azi );
  
  pthread_mutex_lock(& gp_Pth->pth_mut.mut_alt );

    gp_Sui->Sh_old = gp_Sui->Sh ; 
    gp_Sui->Sh     = (int)SGN(freq_alt_mic) ;

    gp_Sui->Fh_mic = fabs(freq_alt_mic)  ;
    gp_Sui->Fh_bru = fabs(freq_alt_bru) ;
    gp_Sui->Fh_mot = fabs(freq_alt_mot) ;
    
    gp_Sui->Th_mic     = 1 / gp_Sui->Fh_mic ;
    gp_Sui->Th_bru = 1 / gp_Sui->Fh_bru ;
    gp_Sui->Th_mot = 1 / gp_Sui->Fh_mot ;

  pthread_mutex_unlock(& gp_Pth->pth_mut.mut_alt );
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

void CALCUL_PERIODES_SUIVI_MANUEL(ASTRE *gp_Ast, SUIVI * gp_Sui, VOUTE *gp_Vou) {

  double frequence ;
  double azi_rot, alt_rot ;
  
  // calcul de la periode en fonction des informations de suivi
  // renvoyer par les boutons de la raquette
  // on deduit la frequence du nombre de pas comptes et de la periode ecoulee
  
  if ( gp_Sui->pas_azi !=  gp_Sui->pas_azi_old ) {
    if ( gp_Sui->pas_azi !=0 ) {
      if ( AZI_ROT == 0 ) azi_rot = -1 ; else azi_rot = 1 ;
    
      //frequence = azi_rot * (double)gp_Sui->pas_azi / gp_Sui->d_appui_raq_azi ;
      frequence = azi_rot * (double)gp_Sui->pas_azi ;
    
      pthread_mutex_lock(& gp_Pth->pth_mut.mut_azi );
  
        gp_Ast->Va    = frequence * DIV * PIPI / ( gp_Vou->acc * AZI_R ) ;
        gp_Sui->Sa_old = gp_Sui->Sa ; 
        gp_Sui->Sa     = (int)SGN(frequence)  ;
        gp_Sui->Fa_mic = fabs(frequence) ;
        gp_Sui->Ta_mic = 1 / gp_Sui->Fa_mic ;
    
      pthread_mutex_unlock(& gp_Pth->pth_mut.mut_azi );
    
      TRACE1("Ta_mic = %2.4f Th_mic = %2.4f Fa_mic = %2.4f Fh_mic = %2.4f\n",gp_Sui->Ta_mic, gp_Sui->Th_mic, gp_Sui->Fa_mic, gp_Sui->Fh_mic) ;
    }
  }
  if ( gp_Sui->pas_alt !=  gp_Sui->pas_alt_old ) {
    if ( gp_Sui->pas_alt !=0 ) {
      if ( ALT_ROT == 0 ) alt_rot = -1 ; else alt_rot = 1 ;
    
      //frequence = alt_rot * (double)gp_Sui->pas_alt / gp_Sui->d_appui_raq_alt ;
      frequence = alt_rot * (double)gp_Sui->pas_alt ;
    
      // fprintf(stderr, "CALCUL_PERIODES_SUIVI_MANUEL demande le mutex\n");
      pthread_mutex_lock( & gp_Pth->pth_mut.mut_alt );
    
        gp_Ast->Vh     = frequence * DIV * PIPI / ( gp_Vou->acc * ALT_R ) ;
        gp_Sui->Sh_old = gp_Sui->Sh ; 
        gp_Sui->Sh     = (int)SGN(frequence)  ;
        gp_Sui->Fh_mic     = fabs(frequence) ;
        gp_Sui->Th_mic     = 1 / gp_Sui->Fh_mic ;
  
        // fprintf(stderr, "CALCUL_PERIODES_SUIVI_MANUEL relache le mutex\n");
      pthread_mutex_unlock( & gp_Pth->pth_mut.mut_alt );
    
      TRACE1("Ta_mic = %2.4f Th_mic = %2.4f Fa_mic = %2.4f Fh_mic = %2.4f\n",gp_Sui->Ta_mic, gp_Sui->Th_mic, gp_Sui->Fa_mic, gp_Sui->Fh_mic) ;
    }
  }
}
//========================================================================================
// FIXME : CALCUL_AFFICHER_HEURE : 
// FIXME : * afficher heure minute seconde et heure decimal
//========================================================================================

void CALCUL_AFFICHER_HEURE( char * mesg, TEMPS * gp_Tim ) {

  Trace1("%s : %dh%dmn%ds : %f", mesg, gp_Tim->HH, gp_Tim->MM, gp_Tim->SS, gp_Tim->hd ) ;
}

//========================================================================================
// FIXME : CALCUL_AFFICHER_ANGLE : 
// FIXME : * afficher angle sous la forme degres minute seconde
//========================================================================================

void CALCUL_AFFICHER_ANGLE( char * mesg, ANGLE *angle ) {

  Trace2("%s : %d°%d'%.2f\" %.4f (deg)", mesg, angle->DD, angle->MM, (double)angle->SS, angle->ad ) ;
}

//========================================================================================
// FIXME : CALCUL_TEMPS_HMS_VERS_DEC : 
// FIXME : * Convertit heure minutes secondes en heure decimale 
/* 2022-09-26 : remplacement du nom HDEC -> TEMPS_VERS_HDEC */
//========================================================================================

void CALCUL_TEMPS_HMS_VERS_DEC(TEMPS * gp_Tim) {
  
  gp_Tim->hd = gp_Tim->HH + (gp_Tim->MM / 60.0) + (gp_Tim->SS / 3600.0)  ;
}

//========================================================================================
// FIXME : CALCUL_TEMPS_HMS_VERS_DEC : 
// FIXME : * Convertit heure minutes secondes en heure decimale 
/* 2022-09-26 : remplacement du nom HDEC -> TEMPS_VERS_HDEC */
//========================================================================================

void CALCUL_TEMPS_HMS_VERS_DEC_DIRECT(double * hdec, double hou, double min, double sec) {
  
  *hdec = hou + ( min / 60.0) + ( sec / 3600.0)  ;
}

/*****************************************************************************************
* @fn     : CALCUL_TEMPS_DEC_VERS_HMS
* @author : s.gravois
* @brief  : Convertit heure decimale en heure minutes secondes decimales
* @param  : TEMPS * gp_Tim
* @date   : 2022-03-18 creation 
* @todo   : ras
*****************************************************************************************/

void CALCUL_TEMPS_DEC_VERS_HMS(TEMPS * gp_Tim) {
  
  Trace1(""); 
  Trace1("TEMPS    decimal  = %.4f" , gp_Tim->hd) ;

  if ( gp_Tim->hd >=0 ) { 
    gp_Tim->si   =  1 ;
    gp_Tim->c_si = '+' ;
  }
  else {
    gp_Tim->si   = -1 ;
    gp_Tim->c_si = '-' ; 
  }
  
  gp_Tim->HH = (int)fabs(  gp_Tim->hd ) ;
  Trace1("heure    decimale = %d" , gp_Tim->HH) ;
  
  gp_Tim->MM = (int)fabs( (fabs(gp_Tim->hd) - gp_Tim->HH ) * 60.0 ) ;
  Trace1("minutes  decimale = %d" , gp_Tim->MM) ;

  gp_Tim->SS = (int)fabs(((fabs(gp_Tim->hd) - gp_Tim->HH ) * 60.0 - gp_Tim->MM ) * 60.0 ) ;
  Trace1("secondes decimale = %d" , gp_Tim->SS) ;
}

/*****************************************************************************************
* @fn     : CALCUL_ANGLE_VERS_DMS
* @author : s.gravois
* @brief  : Convertit angle en heure minutes secondes decimales
* @param  : ANGLE *angle
* @date   : 2022-03-18 creation 
* @todo   : ras
*****************************************************************************************/

void CALCUL_ANGLE_VERS_DMS(ANGLE *angle) {
  
  Trace1(""); 
  Trace1("angle decimal (radian)  = %.4f" , angle->AD) ;
  Trace1("angle decimal (degres)  = %.4f" , angle->ad) ;

  if ( angle->AD >=0 ) { 
    angle->si =  1 ;
    angle->c_si = '+' ;
  }
  else {
    angle->si = -1 ;
    angle->c_si = '-' ;
  }

  angle->DD = (int)fabs(  angle->ad ) ;
  Trace1("heure    decimale = %d" , angle->DD) ;
  
  angle->MM = (int)fabs( (fabs(angle->ad) - angle->DD ) * 60.0 ) ;
  Trace1("minutes  decimale = %d" , angle->MM) ;

  angle->SS = (int)fabs(((fabs(angle->ad) - angle->DD ) * 60.0 - angle->MM ) * 60.0 ) ;
  Trace1("secondes decimale = %d" , angle->SS) ;
}

/*****************************************************************************************
* @fn     : CALCUL_CONVERSIONS_ANGLES
* @author : s.gravois
* @brief  : Convertit angles en heure  minutes secondes (exemple 23h58m12s)
* @brief  : Convertit angles en degres minutes secondes (exemple 11°58'12")
* @param  : ANGLE *angle
* @date   : 2022-03-18 creation 
* @todo   : ras
*****************************************************************************************/

void CALCUL_CONVERSIONS_ANGLES(ASTRE *gp_Ast) {
  
  /* ----------------------------*/
  /* azimut et altitude          */
  /* ----------------------------*/

  (gp_Ast->at).hd = gp_Ast->a * 24.0 / PIPI ;
  CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->at) ;

  (gp_Ast->AZIa).AD = gp_Ast->a ;
  (gp_Ast->AZIa).ad = gp_Ast->a * DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&gp_Ast->AZIa) ;
  
  (gp_Ast->ht).hd = gp_Ast->h * 24.0 / PIPI ;
  CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->ht) ;
  
  (gp_Ast->ALTa).AD = gp_Ast->h ;
  (gp_Ast->ALTa).ad = gp_Ast->h * DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&gp_Ast->ALTa) ;

  /* -----------------------------*/
  /* angle horaire et declinaison */
  /* ascension droite             */
  /* -----------------------------*/

  (gp_Ast->AGHt).hd = gp_Ast->AGH * 24.0 / PIPI ;
  CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AGHt) ;
  
  (gp_Ast->DECt).hd = gp_Ast->DEC * 24.0 / PIPI ;
  CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->DECt) ;

  (gp_Ast->ASCt).hd  = gp_Ast->ASC * 24.0 / PIPI ;
  CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->ASCt) ;

  /* -------------------------------*/

  (gp_Ast->AGHa).AD = gp_Ast->AGH ;
  (gp_Ast->AGHa).ad = gp_Ast->AGH * DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&gp_Ast->AGHa) ;

  (gp_Ast->ASCa).AD = gp_Ast->ASC ;
  (gp_Ast->ASCa).ad = gp_Ast->ASC * DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&gp_Ast->ASCa) ;

  (gp_Ast->DECa).AD = gp_Ast->DEC ;
  (gp_Ast->DECa).ad = gp_Ast->DEC * DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&gp_Ast->DECa) ;

  /* -------------------------------*/
  /* les calculs intermediaires     */
  /* -------------------------------*/

  (gp_Ast->AGH0t).hd = gp_Ast->AGH0 * 24.0 / PIPI ;
  CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AGH0t) ;

  (gp_Ast->AGH1t).hd = gp_Ast->AGH1 * 24.0 / PIPI ;
  CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AGH1t) ;

  (gp_Ast->AGH2t).hd = gp_Ast->AGH2 * 24.0 / PIPI ;
  CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AGH2t) ;

  (gp_Ast->AZI0t).hd = gp_Ast->AZI0 * 24.0 / PIPI ;
  CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AZI0t) ;

  (gp_Ast->AZI1t).hd = gp_Ast->AZI1 * 24.0 / PIPI ;
  CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AZI1t) ;

  (gp_Ast->AZI2t).hd = gp_Ast->AZI2 * 24.0 / PIPI ;
  CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AZI2t) ;
}
//========================================================================================
// FIXME : CALCUL_DATE : 
// FIXME : * calcule la date locale servant a tous les calculs ulterieurs
//========================================================================================

int CALCUL_DATE(TEMPS * gp_Tim) {
  
  time_t t ;
  
  struct tm *tm ;
  
  gp_Tim->HH = 0 ;
  gp_Tim->MM = 0 ;
  gp_Tim->SS = 0 ;
  
  gp_Tim->yy = 0 ;
  gp_Tim->dd = 0 ;
  gp_Tim->mm = 0 ;
  
  time(&t) ;
  // tzset() ;
  // tm=localtime(&t) ; // pour le temps local (UTC+1 en hiver et UTC+2 en ete)
  tm=gmtime(&t);   // pour UTC
  
  gp_Tim->HH = tm->tm_hour ;
  gp_Tim->MM = tm->tm_min ;
  gp_Tim->SS = tm->tm_sec ;
  
  gp_Tim->yy = 1900 + tm->tm_year ;
  gp_Tim->dd = tm->tm_mday ;
  gp_Tim->mm = tm->tm_mon + 1 ;
  
  CALCUL_TEMPS_HMS_VERS_DEC (gp_Tim ) ;
  
  //printf("%d %d %d %d %d %d\n",tm->tm_sec,tm->tm_min,tm->tm_hour,tm->tm_mday,tm->tm_mon,tm->tm_year ) ;
  Trace2("%d %d %d %d %d %d %f",gp_Tim->yy,gp_Tim->mm,gp_Tim->dd,gp_Tim->HH,gp_Tim->MM,gp_Tim->SS, gp_Tim->hd ) ;
  
  return 0 ;
}
//========================================================================================
// FIXME : CALCUL_JOUR_JULIEN
// FIXME : * calcul la date julienne 
// TODO : * valider / comparer avec valeurs de logiciels astronomiques (stellarium etc..)
//========================================================================================

int CALCUL_JOUR_JULIEN(LIEU* gp_Lie, TEMPS * gp_Tim) {
  
  int    yy,mm,A,B,C,Y,M,D ;
  double T ;          // fraction de jour a ajouter au jour Julien	
  double JJ ;         // jour julien
  
  Y  = gp_Tim->yy ;
  M  = gp_Tim->mm ;
  D  = gp_Tim->dd ;
  
  yy = gp_Tim->yy ;
  mm = gp_Tim->mm ;
  
  gp_Lie->JJ = 0 ;
  JJ=0 ;
  
  if ( gp_Tim->mm < 3) {
    yy = gp_Tim->yy  - 1 ;
    mm = gp_Tim->mm + 12 ;
  }
  A  = yy ;
  C  = floor(yy / 100  ) ;  
  B  = 2-C+floor(C/4) ;
  
  Trace2("%d %d %d %d %d %d",gp_Tim->yy,gp_Tim->mm,gp_Tim->dd,gp_Tim->HH,gp_Tim->MM,gp_Tim->SS ) ;
  
  T  = (double)gp_Tim->HH / 24.0 + (double)gp_Tim->MM / 1440.0 + (double)gp_Tim->SS / 86400.0 ;
  
  Trace2("A B C T = %d %d %d %8.8f",A,B,C,T) ;
  
  JJ = floor(365.25*(A+4716.0))+floor(30.6001*(mm+1))+(gp_Tim->dd)+T+B-1524.5 ;
  
  Trace2("Date Julienne (Jour Julien corriger avec le temps local) = %7.9f\n",JJ) ;
  
  JJ = 367*Y - 7*(Y + ( M + 9) / 12 ) / 4 + 275 * M / 9 + D + 1721014 - 0.5 + T ;
  
  Trace1("Date Julienne = %7.9f",JJ) ;
  
  gp_Lie->JJ = JJ ;
  
  return 0 ;
}
//========================================================================================
// FIXME : CALCUL_TEMPS_SIDERAL
// FIXME :  * calcul le temps sideral local en fonction du jour julien, de la longitude
// FIXME :  * a besoin de CALCUL_DATE  et de CALCUL_JOUR_JULIEN et du LIEU

/* La fonction comprend l appel aux deux fonctions suivantes dans le corps de la fonction : 
  CALCUL_DATE( gp_Tim ) ;   
  CALCUL_JOUR_JULIEN  ( gp_Lie, gp_Tim ) ;
*/

// TODO  : valider 1ere methode / 2eme methode et tester sur le ciel ou comparer
// TODO  : avec valeurs de logiciels astronomiques (stellarium etc..)

/* septembre 2022 : correction de deux BUGS passes inapercus car la longitude
   utilisee etait proche de ce celle de GREENWICH ( -0.36 pour escout) => 
   La correction local / greenwich n 'etait pas correct
   bug 1 : 
     dans methode 1 et 2 , le terme gp_Lie->lon est a multiplier par DEGRES
     bug constate quand on passait a un endroit de la Terre eloigne de Greenwich
     (verification sur stellarium)
   bug 2 :
     remplacement de 24h -> 23h56m04s dans les calculs de conversion angles vers H dec
    on doit utiliser le temps sideral et non pas le temps solaire
*/
//========================================================================================

int CALCUL_TEMPS_SIDERAL(LIEU* gp_Lie, TEMPS * gp_Tim ) {
  
  double d, T, TSMG, TSMG2, TSH, TSMGH, TSMGH2, TSMH ;
  double TSMGS, TSMH2,TSMH3, GMSTO, GMST, LMST ;
  double jour_sideral_decimal ;
  const double P_2000 = 2451545.5 ; 
  const double P_1900 = 2415020.0 ;

  CALCUL_TEMPS_HMS_VERS_DEC_DIRECT(&jour_sideral_decimal , 23 , 56 , 4.09054) ;

  Trace1("jour sideral : %f", jour_sideral_decimal) ;
  
  CALCUL_DATE( gp_Tim ) ;   
  CALCUL_JOUR_JULIEN  ( gp_Lie, gp_Tim ) ;
  
  TEMPS Temps0 ;
  TEMPS TSMG3 ;
  TEMPS TSMGH3 ;

  TEMPS TSR2 ;
  TEMPS TSR3 ;

  if ( gp_Lie->JJ == 0 ) return 1 ;
  
  d = gp_Lie->JJ - P_2000 ;
  T = d / 36525.0 ;
  T = ( gp_Lie->JJ - P_2000) / 36525.0 ; 
  
  Trace2("T = %f",T);
  
  // =======================  Premiere methode ==========================
  /* cette methode ne tient pas compte du terme 36525 */
  /* en degres */
  /* calcul du temps sideral moyen de greenwich a 0h TU */
  TSMG  = 100.46061837 + 36000.770053608 * T + ( 0.000387933*T*T) - (( T*T*T ) / 38710000.0) ;   
  //TSMG  = 100.46061837 + 36000.770053608 * T ;
  /* rappel : fmod donne le reste de l operation modulo */
  /* These functions compute the floating-point remainder of dividing x by y */ 

  TSMG2 = fmod(TSMG,360.0) ;
  TSMG3.hd = TSMG2 * jour_sideral_decimal / 360 ; /* pour ramener en heures */
  
  CALCUL_TEMPS_DEC_VERS_HMS( & TSMG3 ) ;
  /* *15 = pour ramener en degres */
  TSH       = gp_Tim->hd * 1.00273790935 * 15 ;
  TSMGH     = TSMG + TSH ;
  TSMGH2    = fmod(TSMGH, 360.0) ;
  TSMGH3.hd = TSMGH2 * jour_sideral_decimal / 360 ;

  CALCUL_TEMPS_DEC_VERS_HMS( & TSMGH3) ;

  TSMH  = TSMGH + ( gp_Lie->lon * DEGRES )  ;
  /*
  Trace("TSMGH  TSMGH2  TSMGH3.hd    : %0.5f   %0.5f   %0.5f", TSMGH, TSMGH2, TSMGH3.hd) ;
  Trace("TSMH = TSMGH - gp_Lie->lon : %0.5f = %0.5f - %0.5f ", TSMH, TSMGH, gp_Lie->lon) ; 
  */
  TSMH2 = fmod(TSMH, 360.0) ;
  TSR2.hd = TSMH2 * jour_sideral_decimal / 360 ;
 
  CALCUL_TEMPS_DEC_VERS_HMS( & TSR2 ) ;
  
  CALCUL_AFFICHER_HEURE("1ere methode - Temps sideral Greenwich 0hTU",& TSMG3) ;
  CALCUL_AFFICHER_HEURE("1ere methode - Temps sideral Greenwich tps local (TU)",& TSMGH3) ;
  CALCUL_AFFICHER_HEURE("1ere methode - Temps sideral Lieu Local + Heure Locale (TU)",& TSR2) ;
  
  // =======================  Deuxieme methode ==========================
  /* cette methode ne tient pas compte du terme 36525 */
  /* en degres */
  /* calcul du temps sideral moyen de greenwich a 0h TU */
  GMSTO = 100.460618375 + 0.985647366286 * d ;
  GMST  = GMSTO + 1.00273790935 * 15 * gp_Tim->hd  ;
  
  LMST = GMST - ( gp_Lie->lon * DEGRES )  ; // FIXME : attention !! ici la longitude du lien intervient !!
  /*
  Trace("GMST  GMSTO  gp_Tim->hd    : %0.5f   %0.5f   %0.5f", GMST, GMSTO, gp_Tim->hd) ;
  Trace("LMST = GMST  - gp_Lie->lon : %0.5f = %0.5f - %0.5f ", LMST, GMST, gp_Lie->lon) ; 
  */
  TSMG2  = fmod(GMSTO, 360.0) ;
  TSMGH2 = fmod(GMST,  360.0) ;
  TSMH2  = fmod(LMST,  360.0) ;
  
  TSMG3.hd  = TSMG2  * jour_sideral_decimal / 360.0 ;
  TSMGH3.hd = TSMGH2 * jour_sideral_decimal / 360.0 ;
  TSR3.hd  = TSMH2  * jour_sideral_decimal / 360.0 ;
  
  CALCUL_TEMPS_DEC_VERS_HMS( & TSMG3) ;
  CALCUL_TEMPS_DEC_VERS_HMS( & TSMGH3 ) ;
  CALCUL_TEMPS_DEC_VERS_HMS( & TSR3) ;
  
  CALCUL_AFFICHER_HEURE("2eme methode - Temps sideral Greenwich 0hTU",& TSMG3) ;
  CALCUL_AFFICHER_HEURE("2eme methode - Temps sideral Greenwich tps local (TU)",& TSMGH3) ;
  CALCUL_AFFICHER_HEURE("2eme methode - Temps sideral Lieu Local + Heure Locale (TU)",& TSR3) ;
  
  // =======================  Troisieme methode ==========================
  /* unite = la seconde */ 
  /* reference : astronomie pratique et informatique */
  /* TG0 = 6(h)41(m)50.54841(s) + \
           8640184.812866(s) * T + \
           0.093103(s)       * T*T - \
           0.000000621(s)    * T*T*T 
  */     
         
  TSMGS = (6*60*60) + (41*60) + 50.54841 + 8640184.812866 * T + 0.093103 * T*T - 0.000000621 * T*T*T ;

  // =======================   ==========================

  gp_Lie->TS  = TSR2.hd ;

  // gp_Lie->TSR = (TSR3.hd / jour_sideral_decimal) * PIPI ;
  gp_Lie->TSR = (TSR2.hd / jour_sideral_decimal) * PIPI ;

  Trace1("JJ %f gp_Lie->TSR = %f",gp_Lie->JJ, gp_Lie->TSR ) ;

  return 0 ;
}
/*****************************************************************************************
* @fn     : CALCUL_ANGLE_HORAIRE
* @author : s.gravois
* @brief  : Convertit heure decimale en heure minutes secondes decimales
* @param  : TEMPS * gp_Tim
* @date   : 2022-03-19 correction valeur quand negatif 
* @todo   : ras
*****************************************************************************************/

void CALCUL_ANGLE_HORAIRE( LIEU *li, ASTRE *gp_Ast ) {
  
  // en radians 

  /* correction ascension droite si negatif (on ajoute 360 degres) */

  if ( gp_Ast->ASC < 0 ) {
    gp_Ast->ASC += 2*PI ;
  }

  /* calcul */ 

  gp_Ast->AGH = li->TSR - gp_Ast->ASC ;
  
  /* correction angle horaire si negatif (on ajoute 360 degres) */

  if ( gp_Ast->AGH < 0 ) {
    gp_Ast->AGH += 2*PI ;
  }

  CALCUL_CONVERSIONS_ANGLES( gp_Ast) ;
  
  Trace1("ascension droite (deg)   = %.2f", gp_Ast->ASC * DEGRES) ;
  Trace1("temps sideral (rad)      = %.2f", li->TSR ) ;
  Trace1("angle horaire (deg)      = %.2f", gp_Ast->AGH * DEGRES) ;

  /*
    CONFIG_AFFICHER_MODE_STELLARIUM(gp_Ast) ;
  */
}

/*****************************************************************************************
* @fn     : CALCUL_ASCENSION_DROITE
* @author : s.gravois
* @brief  : Convertit heure decimale en heure minutes secondes decimales
* @param  : TEMPS * gp_Tim
* @date   : 2022-03-19 correction valeur quand negatif 
* @date   : 2022-03-20 verification que angle horaire est positif
* @todo   : ras
*****************************************************************************************/

void CALCUL_ASCENSION_DROITE( LIEU *li, ASTRE *gp_Ast ) {
  
  // en radians 
  
  /* correction angle horaire si negatif (on ajoute 360 degres) */

  if ( gp_Ast->AGH < 0 ) {
    gp_Ast->AGH += 2*PI ;
  }

  /* calcul */ 

  gp_Ast->ASC = li->TSR - gp_Ast->AGH ;
  
  /* correction ascension droite si negatif (on ajoute 360 degres) */

  if ( gp_Ast->ASC < 0 ) {
    gp_Ast->ASC += 2*PI ;
  }

  CALCUL_CONVERSIONS_ANGLES( gp_Ast) ;
  
  Trace1("ascension droite (deg)   = %.2f", gp_Ast->ASC * DEGRES) ;
  Trace1("temps sideral (rad)      = %.2f", li->TSR ) ;
  Trace1("angle horaire (deg)      = %.2f", gp_Ast->AGH * DEGRES) ;

  /*
    CONFIG_AFFICHER_MODE_STELLARIUM(gp_Ast) ;
  */
}
/*****************************************************************************************
* @fn     : CALCUL_ASTRE_RECUP_TYPE_ET_NOM
* @author : s.gravois
* @brief  : Aggregation partie de code dans CALCUL_TOUT ici (recuperation type et nom astre)
* @param  : ASTRE * gp_Ast
* @date   : 2022-09-29 creation
*****************************************************************************************/

void CALCUL_ASTRE_RECUP_TYPE_ET_NOM(void) {

  int pos=3;
  int len=0 ;
  char c_sub[32];

  /*---------------- evolution 19/01/2022 ----------------
  * prise en compte du fait que la as peut avoir plusieurs
  * prefixe dans son nom (PLA/NGC/MES/ETO mais aussi EQU/AZI)
  * Cela permet de determine le mode de calculs de l'as
  * et le type de l'as (CIEL_PROFOND etc...)
  -------------------------------------------------------*/

  gp_Ast->type = ASTRE_INDETERMINE ;
  gp_Ast->mode = MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL ;   

  len = strlen(gp_Ast->nom)-pos ;

  if ( len > pos ) {

    if      ( strstr( gp_Ast->nom, CONFIG_MES ) != NULL ) gp_Ast->type = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->nom, CONFIG_NGC ) != NULL ) gp_Ast->type = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->nom, CONFIG_ETO ) != NULL ) gp_Ast->type = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->nom, CONFIG_PLA ) != NULL ) gp_Ast->type = ASTRE_PLANETE ;
    else if ( strstr( gp_Ast->nom, CONFIG_AZI ) != NULL ) gp_Ast->type = ASTRE_INDETERMINE ;
    else if ( strstr( gp_Ast->nom, CONFIG_EQU ) != NULL ) gp_Ast->type = ASTRE_INDETERMINE ;
    else                                                   gp_Ast->type = ASTRE_INDETERMINE ;
    /* TODO : completer avec les types manquants */ 

    if      ( strstr( gp_Ast->nom, CONFIG_MES ) != NULL ) gp_Ast->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_NGC ) != NULL ) gp_Ast->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_ETO ) != NULL ) gp_Ast->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_PLA ) != NULL ) gp_Ast->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_AZI ) != NULL ) gp_Ast->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_EQU ) != NULL ) gp_Ast->mode = MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL ;
    else                                                   gp_Ast->mode = MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL ; 
    /* TODO : completer avec les types manquants */ 
  }
  else {
    gp_Ast->type = ASTRE_INDETERMINE ;
  }
  /* recuperation du numero apres les 3 premiers caracteres */

  memset( c_sub, 0, sizeof(c_sub));
  
  if ( gp_Ast->type != ASTRE_INDETERMINE ) {

    if( len > pos ) {

      if ( len > sizeof(c_sub) ) { Trace("erreur : %d >  %d", len,(int)sizeof(c_sub) ) ;}
      if ( len <= 0 )            { Trace("erreur : %d <= 0",   len) ;}

      memcpy( c_sub, &gp_Ast->nom[pos], len );

      c_sub[len] = '\0';
      gp_Ast->numero = atoi(c_sub) ;
    }
    else {
      gp_Ast->numero=0 ;
    }
  }
  else {
    gp_Ast->numero=0 ;
  }
  return ;
}
/*****************************************************************************************
* @fn     : CALCUL_TOUT
* @author : s.gravois
* @brief  : Rafraichit l integralite des calculs
* @param  : ASTRE * gp_Ast
* @param  : LIEU  * gp_Lie
* @param  : SUIVI * gp_Suivi
* @date   : 2022-06-13 correction argument 1 de SOLAR_SYSTEM astre->nom => astre->infos
* @date   : 2022-09-29 deplacement code recup type et nom dans une fct dediee
* @todo   : ras
*****************************************************************************************/

void CALCUL_TOUT(void) {

  CALCUL_ASTRE_RECUP_TYPE_ET_NOM() ;

  switch (gp_Ast->type) {

    /* ----------------------------------------------------------------- */
      /* pour un as indetermine 
         on calcule les coordonnees equatoriales 
         necessaires au calcul des vitesses 
      */

    case ASTRE_INDETERMINE :
    
      pthread_mutex_lock( & gp_Pth->pth_mut.mut_cal );
      
      CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;
      /* CALCUL_AZIMUT       ( gp_Lie, gp_Ast) ; */

      if ( gp_Ast->mode == MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL ) {
        
        CALCUL_EQUATEUR        ( gp_Lie, gp_Ast ) ;
        CALCUL_ASCENSION_DROITE( gp_Lie, gp_Ast ) ;
      }
      else {
        CALCUL_ANGLE_HORAIRE( gp_Lie, gp_Ast ) ;
        CALCUL_AZIMUT       ( gp_Lie, gp_Ast) ;
      }

      CALCUL_VITESSES     ( gp_Lie, gp_Ast, gp_Sui) ;
      CALCUL_PERIODE      ( gp_Ast, gp_Sui, gp_Vou) ;
      
      pthread_mutex_unlock( & gp_Pth->pth_mut.mut_cal ) ;
      
      break ;

    /* ----------------------------------------------------------------- */

    case ASTRE_CIEL_PROFOND :
    
      pthread_mutex_lock( & gp_Pth->pth_mut.mut_cal );
      
      CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;

      CALCUL_ANGLE_HORAIRE( gp_Lie, gp_Ast ) ;
      CALCUL_AZIMUT       ( gp_Lie, gp_Ast) ;
      
      CALCUL_VITESSES     ( gp_Lie, gp_Ast, gp_Sui) ;
      CALCUL_PERIODE      ( gp_Ast, gp_Sui, gp_Vou) ;
      
      pthread_mutex_unlock( & gp_Pth->pth_mut.mut_cal ) ;
      
      break ;
      
    /* ----------------------------------------------------------------- */
    
    case ASTRE_PLANETE :
      
      pthread_mutex_lock( & gp_Pth->pth_mut.mut_cal );
            
      CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;
      
      if ( gp_Ast->numero > 9 ) {
        Trace("numero de planete interdit = %d", gp_Ast->numero ) ;
        Trace("=> forçage a zero (soleil)"
         ) ;
        gp_Ast->numero = 0 ; 
      }

      SOLAR_SYSTEM(     gp_Ast->infos,\
                      & gp_Ast->ASC,\
                      & gp_Ast->DEC,\
                      & gp_Ast->a,\
                      & gp_Ast->h ,\
                        gp_Lie->lat, gp_Lie->lon, gp_Lie->alt,\
                        gp_Tim->yy, gp_Tim->mm, gp_Tim->dd,gp_Tim->HH, gp_Tim->MM, gp_Tim->SS,\
                        gp_Ast->numero) ;
      
      CALCUL_ANGLE_HORAIRE( gp_Lie, gp_Ast ) ;
      
      // FIXME : pour les planetes , le calcul de l'azimut / altitude 
      // FIXME : ainsi que coordonnees equatoriales / declinaison
      // FIXME : est directement retourner par la fonction SOLAR_SYSTEM
      // TODO  : verifier que le CALCUL_AZIMUT en fonction des coordonnees siderales
      // TODO  : ne modifie pas azimut et altitude calcules par SOLAR_SYSTEM
      
      // CALCUL_AZIMUT  ( gp_Lie, gp_Ast) ;
      
      CALCUL_VITESSES ( gp_Lie, gp_Ast, gp_Sui) ;
      CALCUL_PERIODE  ( gp_Ast, gp_Sui, gp_Vou) ;

      pthread_mutex_unlock( & gp_Pth->pth_mut.mut_cal ) ;
      
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

        CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;
        
        // TODO : modifier / completer / corriger ..
        
        if ( gp_Dev->DEVICE_USE_CAPTEURS ) { 
          gp_Ast->a = gp_Sui->pitch ;         // FIXME : donne azimut
          gp_Ast->h = gp_Sui->heading ;       // FIXME : donne altitude 
          CALCUL_EQUATEUR ( gp_Lie, gp_Ast) ;  // FIXME : donnes ASC et DEC
        }
            
        CALCUL_ANGLE_HORAIRE( gp_Lie, gp_Ast ) ;
        CALCUL_AZIMUT  ( gp_Lie, gp_Ast) ;
        
        break ;
      */
  }  
}
//================================================================================================
/* On cherche a balayer la voute avec un pas de 1 degres par exemple
   afin de verifier homogeneite des claculs entre 
   azimutal <-> equatorial  et 
  vice versa
  Le fichier en sortie doit pouvoir se representer avec gnuplot et montrer 
  des points de facon homogene sur la voute 3D
*/
void CALCUL_VOUTE(void ) {
  
  FILE   *fout ;
  // double A, H ;
  double a, h ;

/*
  CALCUL_TEMPS_SIDERAL    ( gp_Lie, gp_Tim ) ;
  CALCUL_EQUATEUR         ( gp_Lie, gp_Ast ) ;
  CALCUL_ASCENSION_DROITE ( gp_Lie, gp_Ast ) ;
  CALCUL_VITESSES         ( gp_Lie, gp_Ast, gp_Sui) ;
  CALCUL_PERIODE          ( gp_Ast, gp_Sui, gp_Vou) ;
*/
  fout=fopen("voute.csv","w") ;
  
  CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;

  // On fait varier les coordonnees azimutales
  
  for(h=-(PI/2)+(gp_Lie->lat)+0.001;h<PI/2;h+=gp_Vou->pas)
    if (h>=0) 
    for(a=-PI +0.001 ;a<PI;a+=gp_Vou->pas){
     
     gp_Ast->a=a ;
     gp_Ast->h=h ;
     
     CALCUL_EQUATEUR          ( gp_Lie, gp_Ast) ; 
     CALCUL_ASCENSION_DROITE  ( gp_Lie, gp_Ast ) ;
     CALCUL_VITESSES          ( gp_Lie, gp_Ast, gp_Sui) ;
     CALCUL_PERIODE           ( gp_Ast, gp_Sui, gp_Vou) ;

     gp_Ast->V  = sqrt( sqr( gp_Ast->Va ) + sqr( gp_Ast->Vh )) ;
     
     if ( gp_Ast->Va != 0) 
       gp_Ast->An = atan( gp_Ast->Vh / gp_Ast->Va ) ;
     else
       gp_Ast->An = PI/2 ;
     
     CALCUL_GEODE( gp_Ast ) ;
     
     Trace1("%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f\n", \
       gp_Ast->a * DEGRES, \
       gp_Ast->h * DEGRES, \
       gp_Ast->AGH * DEGRES, \
       gp_Ast->DEC * DEGRES, \
       gp_Ast->x , \
       gp_Ast->y, \
       gp_Ast->z, \
       gp_Ast->Va, \
       gp_Ast->Vh, \
       gp_Ast->V, \
       gp_Ast->An ) ;
     
     /*  	
     printf("%.15f %.15f %.15f %.15f %.15f %.15f\n", \
       gp_Ast->xx , \
       gp_Ast->yy , \
       gp_Ast->zz , \
       gp_Ast->V ) ;
     
     printf("%.1f %.1f %.1f %.1f %.1f %.1f\n", \
       gp_Ast->xx , \
       gp_Ast->yy , \
       gp_Ast->zz , \
       gp_Ast->Va, \
       gp_Ast->Vh, \
       gp_Ast->V ) ;
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
      if ( gp_Sui->SUIVI_ALIGNEMENT  ) {

        Trace1("%s : a %d h %d A %d H %d : Va %.2f Vh %.2f Ta_mic %.2f Th_mic %.2f Fa_mic=%.2f Fh_mic %.2f",\
          gp_Ast->nom,\
          (int)((gp_Ast->a)*DEGRES),\
          (int)((gp_Ast->h)*DEGRES),\
          (int)((gp_Ast->AGH)*DEGRES),\
          (int)((gp_Ast->DEC)*DEGRES),\
          gp_Ast->Va,\
          gp_Ast->Vh,\
          gp_Sui->Ta_mic,\
          gp_Sui->Th_mic,\
          gp_Sui->Fa_mic,\
          gp_Sui->Fh_mic ) ;

        Trace1("Va=%2.4f Vh=%2.4f Ta_mic=%2.4f Th_mic=%2.4f Fa_mic=%2.4f Fh_mic=%2.4f Fam = %ld Fhm = %ld Tac = %f Thc = %f\n",\
              gp_Ast->Va,\
              gp_Ast->Vh,\
              gp_Sui->Ta_mic,\
              gp_Sui->Th_mic,\
              gp_Sui->Fa_mic,\
              gp_Sui->Fh_mic,\
              (gp_Sui->Ia - gp_Sui->Ia_prec),(gp_Sui->Ih - gp_Sui->Ih_prec ),gp_Sui->Tac, gp_Sui->Thc) ;
        
        gp_Sui->SUIVI_ALIGNEMENT = 0 ;

      }
      */




