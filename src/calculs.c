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
#               * passage dernier arg de SOLAR_SYSTEM abec as->numero
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
// FIXME : * calcule les coordonnees (x,y,z) dans R3 a partir de azimut et altitude
// FIXME : * en supposant que la voute est de diametre = 1
//========================================================================================

void CALCUL_GEODE(ASTRE *as) {

  // on deduit de l'azimut et de l'altitude
  // les coordonnees x y et z dans la geode d'observation
  
  as->x = cos( as->h ) * cos( as->a )  ;
  as->y = cos( as->h ) * sin( as->a ) ;
  as->z = sin( as->h ) ;
  
  // On projette la sphere de rayon=1 sur une autre sphere de rayon la valeur qu'on veut
  // ce systeme permet de voir en 3D une valeur en fonction de 3 autres
  // Ici on veut representer la vitesse (norme du vecteur) 
  // en fonction de x y et z (x y z directions du vecteur sur la sphere unite)
  
  as->xx = as->x * as->V ;
  as->yy = as->y * as->V ;
  as->zz = as->z * as->V ;
  
}
//========================================================================================
// FIXME : CALCUL_AZIMUT : 
// FIXME : * transforme les coordonnees siderales en coordonnees azimutales
//========================================================================================

void CALCUL_AZIMUT(LIEU *lieu, ASTRE *as) {
  
  // transforme les coordonnees "sid�rales" en coordonnees azimutales
  // permet de trouver l'azimut et l'altitude en fonction de l'angle horaire
  // par rapport au m�ridien (SUD) et la declinaison
  // ATTENTION !! angle horaire = ascension droite sid�rale corrig�e
  // avec le temps sid�ral : une conversion doit etre faite pour trouver
  // as->AGH  = TS - (asc droite absolue)
  // Une fonction de calcul du temps sid�ral doit etre faite ult�rieurement 
  
  double lat,A,H,a0,h,a1,a2,af ;
  
  lat= lieu->lat ;
  A  = as->AGH ; // FIXME : angle horaire calcule prealablement dans astro.c en theorie
  H  = as->DEC ; // FIXME : declinaison - seule valeur qui ne change pas 
  
  TRACE1("agh = %2.3f\tH = %2.3f\t",(as->AGH)*DEGRES,(as->DEC)*DEGRES) ;

  h  = ALT(lat,A,H) ;
  as->h = h ;

  a0 = AZI0(A,H,h) ;
  a1 = AZI1(lat,A,H,h) ;
  a2 = AZI2(lat,h,H) ;

  as->a = SGN(a0)*a2 ;

  /* Si azimut negatif on ajoute 360 degres */  
  
  if ( as->a < 0 ) {
    as->a += 2*PI ;
  }

   // resultats de calculs : pour tests (a modifier : supprimer)
  
  as->AZI0 = a0 ;
  as->AZI1 = a1 ;
  as->AZI2 = a2 ;

  CALCUL_CONVERSIONS_ANGLES( as) ;

  Trace1("AZI0 = %d.%d.%d (hms) %.2f (deg)", \
    as->AZI0t.HH, \
    as->AZI0t.MM , \
    as->AZI0t.SS, \
    as->AZI0 * DEGRES );

  Trace1("AZI1 = %d.%d.%d (hms) %.2f (deg)", \
    as->AZI1t.HH, \
    as->AZI1t.MM , \
    as->AZI1t.SS, \
    as->AZI1 * DEGRES );

  Trace1("AZI2 = %d.%d.%d (hms) %.2f (deg)", \
    as->AZI2t.HH, \
    as->AZI2t.MM , \
    as->AZI2t.SS, \
    as->AZI2 * DEGRES );

  Trace1("azi = %d.%d.%d (hms) %.2f (deg)", \
    as->AGHt.HH, as->AGHt.MM , as->AGHt.SS, as->AGH * DEGRES );

  Trace1("alt = %f (deg)" , (as->h)*DEGRES) ;

}
//========================================================================================
// FIXME : CALCUL_EQUATEUR : 
// FIXME : * transforme les coordonnees azimutales en coordonnees siderales (angle horaire)
// TODO :  * probleme : les calculs ne sont pas bons (a verifier)
// TODO :  * cette fonction servira quand on devra avoir les coordonnees equatoriales
//          en fonction des coordonnnees azimutales , pour le calcul des VITESSES,
//          qui utilise les coordonnees equatoriales 
//========================================================================================

void CALCUL_EQUATEUR(LIEU *lieu, ASTRE *as) {

  // transforme les coordonnees azimutales en coordonnees siderales (angle horaire)
  // permet de trouver les coordonn�es siderales en fonction de l'azimut et de l'altitude
  // Pour avoir la correspondance avec les coordonnn�es �quatoriales brutes, il faut
  // utiliser la conversion AH = TS - ASC
  // AH  : angle horaire
  // TS  : temps sid�ral
  // ASC : ascension droite brute
  
  double A0,A2,A1,a,h,lat,Af,agh,H ;

  TRACE1("avant calcul => a = %2.3f\th = %2.3f\t=>agh = %2.3f\tH=%2.3f",\
    (as->a)   * DEGRES,\
    (as->h)   * DEGRES,\
    (as->AGH)* DEGRES,\
    (as->DEC) * DEGRES) ;

  lat      = lieu->lat ;
  a        = as->a ;
  h        = as->h ;
  
  // FIXME : obtention de declinaison et ascension droite (d'apres formules usuelles de Gauss)
  
  H  = DEC(lat,a,h) ;
  as->DEC  = H ;

  A0 = AGH0(a,h,H) ;       /* correct , ajouter 2*PI si negatif */
  A1 = AGH1(lat,a,h,H)+PI; /* correct , ajouter PI */ 
  A2 = AGH2(lat,H,h) ;     /* a anlyser = mauvais */ 
  
  if ( A0 < 0 ) {
    A0 += 2 * PI ;
  }
  // as->AGH = SGN(A0)*A1  ;

  as->AGH = A0 ;

  // resultats de calculs : pour tests (a modifier : supprimer)

  as->AGH0 = A0 ;
  as->AGH1 = A1 ;
  as->AGH2 = A2 ;
  
  CALCUL_CONVERSIONS_ANGLES( as) ;
  
  Trace1(" %s : ASC = %d.%d.%d (hms) %.2f (deg) %.2f (rad)", \
       as->nom , \
       as->ASCt.HH, \
       as->ASCt.MM, \
       as->ASCt.SS, \
       as->ASC * DEGRES, \
       as->ASC \
  ) ; 

  Trace1("AH0   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    as->AGH0t.HH, as->AGH0t.MM , as->AGH0t.SS, as->AGH0 * DEGRES );

  Trace1("AH1   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    as->AGH1t.HH, as->AGH1t.MM , as->AGH1t.SS, as->AGH1 * DEGRES );

  Trace1("AH2   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    as->AGH2t.HH, as->AGH2t.MM , as->AGH2t.SS, as->AGH2 * DEGRES );

  Trace1("AH    (deg) = %d.%d.%d (hms) %.2f (deg)", \
    as->AGHt.HH, as->AGHt.MM , as->AGHt.SS, as->AGH * DEGRES );

  Trace1("DEC   (deg) = %f" , (as->DEC)*DEGRES) ;

  Trace1("apres calcul =>a = %2.3f\th = %2.3f\t=>agh = %2.3f\tH=%2.3f",\
   (as->a)*DEGRES,\
   (as->h)*DEGRES,\
   (as->AGH)*DEGRES,\
   (as->DEC)*DEGRES) ;
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

void CALCUL_D(ASTRE *as, SUIVI *suivi) {
  
  suivi->Da = AZI_F * 2.0 * PI / ( AZI_R * pow(2.0, AZI_N) * ( as->Va / ( 60 * 60 * DEGRES ) )) ;
  suivi->Dh = ALT_F * 2.0 * PI / ( ALT_R * pow(2.0, ALT_N) * ( as->Vh / ( 60 * 60 * DEGRES ) )) ;
}
//========================================================================================
// FIXME : CALCUL_PERIODE : 
// FIXME : * calcule les "vrais" periodes et frequences des moteurs pas a pas
//           en tant que paramtres de la sinusoide de reference
//========================================================================================

void CALCUL_PERIODE(ASTRE *as,SUIVI* suivi, VOUTE *voute) {

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
  
  TRACE2("%f %f %f %f %f",suivi->acc_azi, voute->acc, AZI_R, as->Va, azi_rot);
  TRACE2("%f %f %f %f %f",suivi->acc_alt, voute->acc, AZI_R, as->Vh, alt_rot);

  /*------------------------*/
  /* Calculs des frequences */
  /*------------------------*/

  if ( devices->DEVICE_USE_CONTROLER ) {
    freq_azi = suivi->acc_azi * voute->acc * AZI_R * as->Va * azi_rot / DIV / PIPI ;
  }
  else {
    freq_azi = suivi->acc_azi * voute->acc * AZI_R * as->Va * azi_rot * AZI_R4 / DIV / PIPI / 4  ;
  }
  if ( devices->DEVICE_USE_CONTROLER ) {
    freq_alt = suivi->acc_alt * voute->acc * ALT_R * as->Vh * alt_rot / DIV / PIPI ;
  }
  else {
    freq_alt = suivi->acc_alt * voute->acc * ALT_R * as->Vh * alt_rot * ALT_R4 / DIV / PIPI / 4  ;
  }

  /*------------------------*/
  /* Calculs des periodes  */
  /*------------------------*/

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

void CALCUL_PERIODES_SUIVI_MANUEL(ASTRE *as, SUIVI* suivi, VOUTE *voute) {

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
  
        as->Va     = frequence * DIV * PIPI / ( voute->acc * AZI_R ) ;
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
    
        as->Vh     = frequence * DIV * PIPI / ( voute->acc * ALT_R ) ;
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
// FIXME : CALCUL_AFFICHER_ANGLE : 
// FIXME : * afficher angle sous la forme degres minute seconde
//========================================================================================

void CALCUL_AFFICHER_ANGLE( char * mesg, ANGLE *angle ) {

  Trace2("%s : %d°%d'%.2f\" %.4f (deg)", mesg, angle->DD, angle->MM, (double)angle->SS, angle->ad ) ;
}

//========================================================================================
// FIXME : CALCUL_HDEC : 
// FIXME : * Convertit heure minutes secondes en heure decimale 
//========================================================================================

void CALCUL_HDEC(TEMPS *temps) {
  
  temps->hd = temps->HH + (temps->MM / 60.0) + (temps->SS / 3600.0)  ;
}

/*****************************************************************************************
* @fn     : CALCUL_TEMPS_VERS_HMS
* @author : s.gravois
* @brief  : Convertit heure decimale en heure minutes secondes decimales
* @param  : TEMPS *temps
* @date   : 2022-03-18 creation 
* @todo   : ras
*****************************************************************************************/

void CALCUL_TEMPS_VERS_HMS(TEMPS *temps) {
  
  Trace1(""); 
  Trace1("TEMPS    decimal  = %.4f" , temps->hd) ;

  if ( temps->hd >=0 ) { 
    temps->si   =  1 ;
    temps->c_si = '+' ;
  }
  else {
    temps->si   = -1 ;
    temps->c_si = '-' ; 
  }
  
  temps->HH = (int)fabs(  temps->hd ) ;
  Trace1("heure    decimale = %d" , temps->HH) ;
  
  temps->MM = (int)fabs( (fabs(temps->hd) - temps->HH ) * 60.0 ) ;
  Trace1("minutes  decimale = %d" , temps->MM) ;

  temps->SS = (int)fabs(((fabs(temps->hd) - temps->HH ) * 60.0 - temps->MM ) * 60.0 ) ;
  Trace1("secondes decimale = %d" , temps->SS) ;
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

void CALCUL_CONVERSIONS_ANGLES(ASTRE *as) {
  
  /* ----------------------------*/
  /* azimut et altitude          */
  /* ----------------------------*/

  (as->at).hd = as->a * 24.0 / PIPI ;
  CALCUL_TEMPS_VERS_HMS(&as->at) ;

  (as->AZIa).AD = as->a ;
  (as->AZIa).ad = as->a * DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&as->AZIa) ;
  
  (as->ht).hd = as->h * 24.0 / PIPI ;
  CALCUL_TEMPS_VERS_HMS(&as->ht) ;
  
  (as->ALTa).AD = as->h ;
  (as->ALTa).ad = as->h * DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&as->ALTa) ;

  /* -----------------------------*/
  /* angle horaire et declinaison */
  /* ascension droite             */
  /* -----------------------------*/

  (as->AGHt).hd = as->AGH * 24.0 / PIPI ;
  CALCUL_TEMPS_VERS_HMS(&as->AGHt) ;
  
  (as->DECt).hd = as->DEC * 24.0 / PIPI ;
  CALCUL_TEMPS_VERS_HMS(&as->DECt) ;

  (as->ASCt).hd  = as->ASC * 24.0 / PIPI ;
  CALCUL_TEMPS_VERS_HMS(&as->ASCt) ;

  /* -------------------------------*/

  (as->AGHa).AD = as->AGH ;
  (as->AGHa).ad = as->AGH * DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&as->AGHa) ;

  (as->ASCa).AD = as->ASC ;
  (as->ASCa).ad = as->ASC * DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&as->ASCa) ;

  (as->DECa).AD = as->DEC ;
  (as->DECa).ad = as->DEC * DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&as->DECa) ;

  /* -------------------------------*/
  /* les calculs intermediaires     */
  /* -------------------------------*/

  (as->AGH0t).hd = as->AGH0 * 24.0 / PIPI ;
  CALCUL_TEMPS_VERS_HMS(&as->AGH0t) ;

  (as->AGH1t).hd = as->AGH1 * 24.0 / PIPI ;
  CALCUL_TEMPS_VERS_HMS(&as->AGH1t) ;

  (as->AGH2t).hd = as->AGH2 * 24.0 / PIPI ;
  CALCUL_TEMPS_VERS_HMS(&as->AGH2t) ;

  (as->AZI0t).hd = as->AZI0 * 24.0 / PIPI ;
  CALCUL_TEMPS_VERS_HMS(&as->AZI0t) ;

  (as->AZI1t).hd = as->AZI1 * 24.0 / PIPI ;
  CALCUL_TEMPS_VERS_HMS(&as->AZI1t) ;

  (as->AZI2t).hd = as->AZI2 * 24.0 / PIPI ;
  CALCUL_TEMPS_VERS_HMS(&as->AZI2t) ;
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
  
  CALCUL_TEMPS_VERS_HMS( & TSMG3 ) ;
  
  TSH   = temps->hd * 1.00273790935 * 15 ;
  
  TSMGH  = TSMG + TSH ;
  
  TSMGH2 = fmod(TSMGH, 360.0) ;
  TSMGH3.hd = TSMGH2 * 24 / 360 ;

  CALCUL_TEMPS_VERS_HMS( & TSMGH3) ;
  
  TSMH  = TSMGH - lieu->lon ;
  
  TSMH2 = fmod(TSMH, 360.0) ;
  TSMH3.hd = TSMH2 * 24.0 / 360.0 ;

  CALCUL_TEMPS_VERS_HMS( & TSMH3 ) ;
  
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
  
  CALCUL_TEMPS_VERS_HMS( & TSMG3) ;
  CALCUL_TEMPS_VERS_HMS( & TSMGH3 ) ;
  CALCUL_TEMPS_VERS_HMS( & TSMH3) ;
  
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
/*****************************************************************************************
* @fn     : CALCUL_ANGLE_HORAIRE
* @author : s.gravois
* @brief  : Convertit heure decimale en heure minutes secondes decimales
* @param  : TEMPS *temps
* @date   : 2022-03-19 correction valeur quand negatif 
* @todo   : ras
*****************************************************************************************/

void CALCUL_ANGLE_HORAIRE( LIEU *li, ASTRE *as ) {
  
  // en radians 

  /* correction ascension droite si negatif (on ajoute 360 degres) */

  if ( as->ASC < 0 ) {
    as->ASC += 2*PI ;
  }

  /* calcul */ 

  as->AGH = li->TSR - as->ASC ;
  
  /* correction angle horaire si negatif (on ajoute 360 degres) */

  if ( as->AGH < 0 ) {
    as->AGH += 2*PI ;
  }

  CALCUL_CONVERSIONS_ANGLES( as) ;
  
  Trace1("ascension droite (deg)   = %.2f", as->ASC * DEGRES) ;
  Trace1("temps sideral (rad)      = %.2f", li->TSR ) ;
  Trace1("angle horaire (deg)      = %.2f", as->AGH * DEGRES) ;

  /*
    CONFIG_AFFICHER_MODE_STELLARIUM(as) ;
  */
}

/*****************************************************************************************
* @fn     : CALCUL_ASCENSION_DROITE
* @author : s.gravois
* @brief  : Convertit heure decimale en heure minutes secondes decimales
* @param  : TEMPS *temps
* @date   : 2022-03-19 correction valeur quand negatif 
* @date   : 2022-03-20 verification que angle horaire est positif
* @todo   : ras
*****************************************************************************************/

void CALCUL_ASCENSION_DROITE( LIEU *li, ASTRE *as ) {
  
  // en radians 
  
  /* correction angle horaire si negatif (on ajoute 360 degres) */

  if ( as->AGH < 0 ) {
    as->AGH += 2*PI ;
  }

  /* calcul */ 

  as->ASC = li->TSR - as->AGH ;
  
  /* correction ascension droite si negatif (on ajoute 360 degres) */

  if ( as->ASC < 0 ) {
    as->ASC += 2*PI ;
  }

  CALCUL_CONVERSIONS_ANGLES( as) ;
  
  Trace1("ascension droite (deg)   = %.2f", as->ASC * DEGRES) ;
  Trace1("temps sideral (rad)      = %.2f", li->TSR ) ;
  Trace1("angle horaire (deg)      = %.2f", as->AGH * DEGRES) ;

  /*
    CONFIG_AFFICHER_MODE_STELLARIUM(as) ;
  */
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
  * prise en compte du fait que la as peut avoir plusieurs
  * prefixe dans son nom (PLA/NGC/MES/ETO mais aussi EQU/AZI)
  * Cela permet de determine le mode de calculs de l'as
  * et le type de l'as (CIEL_PROFOND etc...)
  -------------------------------------------------------*/

  if      ( strstr( as->nom, CONFIG_MES ) != NULL ) as->type = ASTRE_CIEL_PROFOND ;
  else if ( strstr( as->nom, CONFIG_NGC ) != NULL ) as->type = ASTRE_CIEL_PROFOND ;
  else if ( strstr( as->nom, CONFIG_ETO ) != NULL ) as->type = ASTRE_CIEL_PROFOND ;
  else if ( strstr( as->nom, CONFIG_PLA ) != NULL ) as->type = ASTRE_PLANETE ;
  else if ( strstr( as->nom, CONFIG_AZI ) != NULL ) as->type = ASTRE_INDETERMINE ;
  else if ( strstr( as->nom, CONFIG_EQU ) != NULL ) as->type = ASTRE_INDETERMINE ;
  else                                                 as->type = ASTRE_INDETERMINE ;
  /* TODO : completer avec les types manquants */ 

  if      ( strstr( as->nom, CONFIG_MES ) != NULL ) as->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  else if ( strstr( as->nom, CONFIG_NGC ) != NULL ) as->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  else if ( strstr( as->nom, CONFIG_ETO ) != NULL ) as->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  else if ( strstr( as->nom, CONFIG_PLA ) != NULL ) as->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  else if ( strstr( as->nom, CONFIG_AZI ) != NULL ) as->mode = MODE_CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
  else if ( strstr( as->nom, CONFIG_EQU ) != NULL ) as->mode = MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL ;
  else                                                 as->mode = MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL ; 
  /* TODO : completer avec les types manquants */ 

  /* recuperation du numero apres les 3 premiers caracteres */

	len=strlen(as->nom)-pos ;
  memset( c_sub, 0, sizeof(c_sub));
  memcpy( c_sub, &as->nom[pos], len );
  c_sub[len] = '\0';
  as->numero = atoi(c_sub) ;
  
  switch (as->type) {

    /* ----------------------------------------------------------------- */
      /* pour un as indetermine 
         on calcule les coordonnees equatoriales 
         necessaires au calcul des vitesses 
      */

    case ASTRE_INDETERMINE :
    
      pthread_mutex_lock( & suivi->mutex_cal );
      
      CALCUL_TEMPS_SIDERAL( lieu, temps ) ;
      /* CALCUL_AZIMUT       ( lieu, as) ; */

      if ( as->mode == MODE_CALCUL_AZIMUTAL_VERS_EQUATORIAL ) {
        
        CALCUL_EQUATEUR        ( lieu, as ) ;
        CALCUL_ASCENSION_DROITE( lieu, as ) ;
      }
      else {
        CALCUL_ANGLE_HORAIRE( lieu, as ) ;
        CALCUL_AZIMUT       ( lieu, as) ;
      }

      CALCUL_VITESSES     ( lieu, as, suivi) ;
      CALCUL_PERIODE      ( as, suivi, voute) ;
      
      pthread_mutex_unlock( & suivi->mutex_cal ) ;
      
      break ;

    /* ----------------------------------------------------------------- */

    case ASTRE_CIEL_PROFOND :
    
      pthread_mutex_lock( & suivi->mutex_cal );
      
      CALCUL_TEMPS_SIDERAL( lieu, temps ) ;

      CALCUL_ANGLE_HORAIRE( lieu, as ) ;
      CALCUL_AZIMUT       ( lieu, as) ;
      
      CALCUL_VITESSES     ( lieu, as, suivi) ;
      CALCUL_PERIODE      ( as, suivi, voute) ;
      
      pthread_mutex_unlock( & suivi->mutex_cal ) ;
      
      break ;
      
    /* ----------------------------------------------------------------- */
    
    case ASTRE_PLANETE :
      
      pthread_mutex_lock( & suivi->mutex_cal );
            
      CALCUL_TEMPS_SIDERAL( lieu, temps ) ;
      
      if ( as->numero > 9 ) {
        Trace("numero de planete interdit = %d", as->numero ) ;
        Trace("=> forçage a zero (soleil)"
         ) ;
        as->numero = 0 ; 
      }
      SOLAR_SYSTEM( as->nom,\
                      & as->ASC,\
                      & as->DEC,\
                      & as->a,\
                      & as->h ,\
                        lieu->lat, lieu->lon, lieu->alt,\
                        temps->yy, temps->mm, temps->dd,temps->HH, temps->MM, temps->SS,\
                        as->numero) ;
      
      CALCUL_ANGLE_HORAIRE( lieu, as ) ;
      
      // FIXME : pour les planetes , le calcul de l'azimut / altitude 
      // FIXME : ainsi que coordonnees equatoriales / declinaison
      // FIXME : est directement retourner par la fonction SOLAR_SYSTEM
      // TODO  : verifier que le CALCUL_AZIMUT en fonction des coordonnees siderales
      // TODO  : ne modifie pas azimut et altitude calcules par SOLAR_SYSTEM
      
      // CALCUL_AZIMUT  ( lieu, as) ;
      
      CALCUL_VITESSES ( lieu, as, suivi) ;
      CALCUL_PERIODE  ( as, suivi, voute) ;

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
        
        if ( devices->DEVICE_USE_CAPTEURS ) { 
          as->a = suivi->pitch ;         // FIXME : donne azimut
          as->h = suivi->heading ;       // FIXME : donne altitude 
          CALCUL_EQUATEUR ( lieu, as) ;  // FIXME : donnes ASC et DEC
        }
            
        CALCUL_ANGLE_HORAIRE( lieu, as ) ;
        CALCUL_AZIMUT  ( lieu, as) ;
        
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
  CALCUL_TEMPS_SIDERAL    ( lieu, temps ) ;
  CALCUL_EQUATEUR         ( lieu, as ) ;
  CALCUL_ASCENSION_DROITE ( lieu, as ) ;
  CALCUL_VITESSES         ( lieu, as, suivi) ;
  CALCUL_PERIODE          ( as, suivi, voute) ;
*/
  fout=fopen("voute.csv","w") ;
  
  CALCUL_TEMPS_SIDERAL( lieu, temps ) ;

  // On fait varier les coordonnees azimutales
  
  for(h=-(PI/2)+(lieu->lat)+0.001;h<PI/2;h+=voute->pas)
    if (h>=0) 
    for(a=-PI +0.001 ;a<PI;a+=voute->pas){
     
     as->a=a ;
     as->h=h ;
     
     CALCUL_EQUATEUR          ( lieu, as) ; 
     CALCUL_ASCENSION_DROITE  ( lieu, as ) ;
     CALCUL_VITESSES          ( lieu, as, suivi) ;
     CALCUL_PERIODE           ( as, suivi, voute) ;

     as->V  = sqrt( sqr( as->Va ) + sqr( as->Vh )) ;
     
     if ( as->Va != 0) 
       as->An = atan( as->Vh / as->Va ) ;
     else
       as->An = PI/2 ;
     
     CALCUL_GEODE( as ) ;
     
     Trace1("%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f\n", \
       as->a * DEGRES, \
       as->h * DEGRES, \
       as->AGH * DEGRES, \
       as->DEC * DEGRES, \
       as->x , \
       as->y, \
       as->z, \
       as->Va, \
       as->Vh, \
       as->V, \
       as->An ) ;
     
     /*  	
     printf("%.15f %.15f %.15f %.15f %.15f %.15f\n", \
       as->xx , \
       as->yy , \
       as->zz , \
       as->V ) ;
     
     printf("%.1f %.1f %.1f %.1f %.1f %.1f\n", \
       as->xx , \
       as->yy , \
       as->zz , \
       as->Va, \
       as->Vh, \
       as->V ) ;
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

        Trace1("%s : a %d h %d A %d H %d : Va %.2f Vh %.2f Ta %.2f Th %.2f Fa=%.2f Fh %.2f",\
          as->nom,\
          (int)((as->a)*DEGRES),\
          (int)((as->h)*DEGRES),\
          (int)((as->AGH)*DEGRES),\
          (int)((as->DEC)*DEGRES),\
          as->Va,\
          as->Vh,\
          suivi->Ta,\
          suivi->Th,\
          suivi->Fa,\
          suivi->Fh ) ;

        Trace1("Va=%2.4f Vh=%2.4f Ta=%2.4f Th=%2.4f Fa=%2.4f Fh=%2.4f Fam = %ld Fhm = %ld Tac = %f Thc = %f\n",\
              as->Va,\
              as->Vh,\
              suivi->Ta,\
              suivi->Th,\
              suivi->Fa,\
              suivi->Fh,\
              (suivi->Ia - suivi->Ia_prec),(suivi->Ih - suivi->Ih_prec ),suivi->Tac, suivi->Thc) ;
        
        suivi->SUIVI_ALIGNEMENT = 0 ;

      }
      */




