/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 15/11/2021  | * (astro_global.h) modification des types enum et contenu enum
#               * (astro_global.h) modification ordre des menus (MENU_AZIMUTAL=0)
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

#include "astro_calculs.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_OTHER ;

static const char * c_Reduction_Type[] = {
  "REDUCTION_INDETERMINE",
  "REDUCTION_MONTURE_NB_DENTS",
  "REDUCTION_POULIE_MONTURE_NB_DENTS",
  "REDUCTION_POULIE_MOTEUR_NB_DENTS",
  "REDUCTION_REDUCTEUR_PLANETAIRE",
  "REDUCTION_MOTEUR_NB_PAS",
  "REDUCTION_MOTEUR_NB_MICROPAS",
  "REDUCTION_CPU_CORRECTION"
} ;

static const char * c_Calculs_Mode[] = {
  "CALCUL_INDETERMINE",  
  "CALCUL_AZIMUTAL_VERS_EQUATORIAL",
  "CALCUL_EQUATORIAL_VERS_AZIMUTAL"
} ;

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
double RAD  (int degres, int minutes )                  { return ((double)degres + ( SGN(degres)*(double)minutes) / 60.0 ) / CALCUL_UN_RADIAN_EN_DEGRES ; }
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

double RAD  (int degres, int minutes )                  { return ((double)degres + ( SGN(degres)*(double)minutes) / 60.0 ) / CALCUL_UN_RADIAN_EN_DEGRES ; }
double DEG  (int degres, int minutes )                  { return (double)degres  + ( SGN(degres)*(double)minutes) / 60.0 ; }

/*****************************************************************************************
* @fn     : CALCUL_REDUCTION_TOTALE
* @author : s.gravois
* @brief  : permet de calculer unela reduction totale de la vitesse 
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
*****************************************************************************************/

double CALCUL_REDUCTION_TOTALE() {

  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  return 0.0 ;
}

/*****************************************************************************************
* @fn     : CALCUL_GEODE
* @author : s.gravois
* @brief  : Calcule les coordonnees (x,y,z) dans R3 a partir de azimut et altitude
* @brief  :  en supposant la voute de rayon 1
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-10-11 creation entete doxygen 
*****************************************************************************************/

void CALCUL_GEODE(STRUCT_ASTRE *gp_Ast) {

  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

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

/*****************************************************************************************
* @fn     : CALCUL_AZIMUT
* @author : s.gravois
* @brief  : Transforme les coordonnees siderales en coordonnees azimutales
* @param  : STRUCT_LIEU  * gp_Lie
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-10-11 creation entete doxygen 
*****************************************************************************************/

void CALCUL_AZIMUT(STRUCT_LIEU *gp_Lie, STRUCT_ASTRE *gp_Ast) {
  
  // transforme les coordonnees "sid�rales" en coordonnees azimutales
  // permet de trouver l'azimut et l'altitude en fonction de l'angle horaire
  // par rapport au m�ridien (SUD) et la declinaison
  // ATTENTION !! angle horaire = ascension droite sid�rale corrig�e
  // avec le temps sid�ral : une conversion doit etre faite pour trouver
  // gp_Ast->AGH  = TS - (asc droite absolue)
  // Une fonction de calcul du temps sid�ral doit etre faite ult�rieurement 
  
  double lat,A,H,a0,h,a1,a2,af ;
  
  ARBO(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  lat= gp_Lie->lat ;
  A  = gp_Ast->AGH ; // FIXME : angle horaire calcule prealablement dans astro.c en theorie
  H  = gp_Ast->DEC ; // FIXME : declinaison - seule valeur qui ne change pas 
  
  TRACE1("agh = %2.3f\tH = %2.3f\t",(gp_Ast->AGH)*CALCUL_UN_RADIAN_EN_DEGRES,(gp_Ast->DEC)*CALCUL_UN_RADIAN_EN_DEGRES) ;

  h  = ALT(lat,A,H) ;
  gp_Ast->h = h ;

  a0 = AZI0(A,H,h) ;
  a1 = AZI1(lat,A,H,h) ;
  a2 = AZI2(lat,h,H) ;

  gp_Ast->a = SGN(a0)*a2 ;

  /* Si azimut negatif on ajoute 360 degres */  
  
  if ( gp_Ast->a < 0 ) {
    gp_Ast->a += 2*M_PI ;
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
    gp_Ast->AZI0 * CALCUL_UN_RADIAN_EN_DEGRES );

  Trace1("AZI1 = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AZI1t.HH, \
    gp_Ast->AZI1t.MM , \
    gp_Ast->AZI1t.SS, \
    gp_Ast->AZI1 * CALCUL_UN_RADIAN_EN_DEGRES );

  Trace1("AZI2 = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AZI2t.HH, \
    gp_Ast->AZI2t.MM , \
    gp_Ast->AZI2t.SS, \
    gp_Ast->AZI2 * CALCUL_UN_RADIAN_EN_DEGRES );

  Trace1("azi = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AGHt.HH, gp_Ast->AGHt.MM , gp_Ast->AGHt.SS, gp_Ast->AGH * CALCUL_UN_RADIAN_EN_DEGRES );

  Trace1("alt = %f (deg)" , (gp_Ast->h)*CALCUL_UN_RADIAN_EN_DEGRES) ;

}

/*****************************************************************************************
* @fn     : CALCUL_EQUATEUR
* @author : s.gravois
* @brief  : Transforme les coordonnees azimutales en coordonnees siderales (angle horaire)
* @param  : STRUCT_LIEU  * gp_Lie
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : TODO : verifier a nouveau que les calculs sont corrects (2022)
*****************************************************************************************/

/* FIXME : cette fonction servira quand on devra avoir les coordonnees equatoriales
  en fonction des coordonnnees azimutales , pour le calcul des VITESSES, qui utilise 
  les coordonnees equatoriales 
*/

void CALCUL_EQUATEUR(STRUCT_LIEU *gp_Lie, STRUCT_ASTRE *gp_Ast) {

  // transforme les coordonnees azimutales en coordonnees siderales (angle horaire)
  // permet de trouver les coordonn�es siderales en fonction de l'azimut et de l'altitude
  // Pour avoir la correspondance avec les coordonnn�es �quatoriales brutes, il faut
  // utiliser la conversion AH = TS - ASC
  // AH  : angle horaire
  // TS  : temps sid�ral
  // ASC : ascension droite brute
  
  double A0,A2,A1,a,h,lat,Af,agh,H ;

  ARBO(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  TRACE1("avant calcul => a = %2.3f\th = %2.3f\t=>agh = %2.3f\tH=%2.3f",\
    (gp_Ast->a)   * CALCUL_UN_RADIAN_EN_DEGRES,\
    (gp_Ast->h)   * CALCUL_UN_RADIAN_EN_DEGRES,\
    (gp_Ast->AGH)* CALCUL_UN_RADIAN_EN_DEGRES,\
    (gp_Ast->DEC) * CALCUL_UN_RADIAN_EN_DEGRES) ;

  lat      = gp_Lie->lat ;
  a        = gp_Ast->a ;
  h        = gp_Ast->h ;
  
  // FIXME : obtention de declinaison et ascension droite (d'apres formules usuelles de Gauss)
  
  H  = DEC(lat,a,h) ;
  gp_Ast->DEC  = H ;

  A0 = AGH0(a,h,H) ;       /* correct , ajouter 2*M_PI si negatif */
  A1 = AGH1(lat,a,h,H)+M_PI; /* correct , ajouter M_PI */ 
  A2 = AGH2(lat,H,h) ;     /* a anlyser = mauvais */ 
  
  if ( A0 < 0 ) {
    A0 += 2 * M_PI ;
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
       gp_Ast->ASC * CALCUL_UN_RADIAN_EN_DEGRES, \
       gp_Ast->ASC \
  ) ; 

  Trace1("AH0   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AGH0t.HH, gp_Ast->AGH0t.MM , gp_Ast->AGH0t.SS, gp_Ast->AGH0 * CALCUL_UN_RADIAN_EN_DEGRES );

  Trace1("AH1   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AGH1t.HH, gp_Ast->AGH1t.MM , gp_Ast->AGH1t.SS, gp_Ast->AGH1 * CALCUL_UN_RADIAN_EN_DEGRES );

  Trace1("AH2   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AGH2t.HH, gp_Ast->AGH2t.MM , gp_Ast->AGH2t.SS, gp_Ast->AGH2 * CALCUL_UN_RADIAN_EN_DEGRES );

  Trace1("AH    (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->AGHt.HH, gp_Ast->AGHt.MM , gp_Ast->AGHt.SS, gp_Ast->AGH * CALCUL_UN_RADIAN_EN_DEGRES );

  Trace1("DEC   (deg) = %f" , (gp_Ast->DEC)*CALCUL_UN_RADIAN_EN_DEGRES) ;

  Trace1("apres calcul =>a = %2.3f\th = %2.3f\t=>agh = %2.3f\tH=%2.3f",\
   (gp_Ast->a)*CALCUL_UN_RADIAN_EN_DEGRES,\
   (gp_Ast->h)*CALCUL_UN_RADIAN_EN_DEGRES,\
   (gp_Ast->AGH)*CALCUL_UN_RADIAN_EN_DEGRES,\
   (gp_Ast->DEC)*CALCUL_UN_RADIAN_EN_DEGRES) ;
} 

/*****************************************************************************************
* @fn     : CALCUL_VITESSES_EQUATORIAL
* @author : s.gravois
* @brief  : MAJ de Va et Vh (vitesses sur les moteurs azimut et altitude) en vit equatoriales
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : 
*****************************************************************************************/

void CALCUL_VITESSES_EQUATORIAL(STRUCT_ASTRE *gp_Ast) {

  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  gp_Ast->Va = -15.0 ;
  gp_Ast->Vh = -15.0 ;
}

/*****************************************************************************************
* @fn     : CALCUL_VITESSES
* @author : s.gravois
* @brief  : MAJ de Va et Vh (vitesses sur les moteurs azimut et altitude) en vit equatoriales
* @param  : STRUCT_LIEU  * l_li
* @param  : STRUCT_ASTRE * l_as
* @param  : STRUCT_SUIVI * l_sui
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : 
*****************************************************************************************/

void CALCUL_VITESSES(STRUCT_LIEU *l_li, STRUCT_ASTRE *l_as, STRUCT_SUIVI * l_sui) {

  // le calcul des vitesses est fait avec
  // l'angle horaire et la declinaison (azimut et altitude inutiles)
 
  double G, Va, Vh, angle ;
  
  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( l_sui->SUIVI_EQUATORIAL == 1 ) {
    l_as->Va = -15.0 ;
    l_as->Vh = -15.0 ;
  }
  else {
    angle=CALCUL_NB_SECARC_PAR_S_EQU ;
    G  = sqr(sin(l_as->AGH))*sqr(cos(l_as->DEC))+sqr(sin(l_li->lat)*cos(l_as->DEC)*cos(l_as->AGH)-cos(l_li->lat)*sin(l_as->DEC)) ;
    Va = angle*(cos(l_as->DEC)*(cos(l_li->lat)*cos(l_as->AGH)*sin(l_as->DEC)-sin(l_li->lat)*cos(l_as->DEC)))/G ;
    Vh = angle*sin(l_as->AGH)*cos(l_as->DEC)*cos(l_li->lat)/sqrt(G) ;
    l_as->Va=Va ;
    l_as->Vh=Vh ;
  }
  TRACE1("%2.0f\t%2.8f\t%2.4f\t=>\t%2.8f\t%2.8f\n",(l_li->lat)*CALCUL_UN_RADIAN_EN_DEGRES,(l_as->AGH)*CALCUL_UN_RADIAN_EN_DEGRES,(l_as->DEC)*CALCUL_UN_RADIAN_EN_DEGRES,Va,Vh) ;
}

/*****************************************************************************************
* @fn     : CALCUL_DIVISEUR_FREQUENCE
* @author : s.gravois
* @brief  : calcule les nombres a injecter dans des diviseurs de frequence 
* @param  : STRUCT_SUIVI * l_sui
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : TODO :  * a supprimer ? inutilise (a servi au debut du projet) 
*****************************************************************************************/

void CALCUL_DIVISEUR_FREQUENCE(STRUCT_ASTRE *gp_Ast, STRUCT_SUIVI *gp_Sui) {
  
  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  gp_Sui->Da = gp_Cal_Par->par_azi_f * 2.0 * M_PI / ( gp_Cal_Par->par_azi_red_tot * pow(2.0, gp_Cal_Par->par_azi_n) * ( gp_Ast->Va / ( 60 * 60 * CALCUL_UN_RADIAN_EN_DEGRES ) )) ;
  gp_Sui->Dh = gp_Cal_Par->par_alt_f * 2.0 * M_PI / ( gp_Cal_Par->par_alt_red_tot * pow(2.0, gp_Cal_Par->par_alt_n) * ( gp_Ast->Vh / ( 60 * 60 * CALCUL_UN_RADIAN_EN_DEGRES ) )) ;
}

/*****************************************************************************************
* @fn     : CALCUL_PERIODE
* @author : s.gravois
* @brief  : calcule les "vraies" periodes et frequences des moteurs pas a pas
* @brief  : en tant que parametres de la sinusoide de reference 
* @param  : STRUCT_ASTRE * gp_Ast
* @param  : STRUCT_SUIVI * gp_Sui
* @param  : STRUCT_VOUTE * gp_Vou
* @date   : 2022-05 ajout non prise en compte des micro-pas pour la frequence moteur
* @date   : 2022-06 correction petite erreur sur Fa_mot / Fh_mot
* @date   : 2022-06 ajout champs xxx_bru freq /periode avant multiplication par acceleration
*****************************************************************************************/

void CALCUL_PERIODE(STRUCT_ASTRE *gp_Ast, STRUCT_SUIVI * gp_Sui, STRUCT_VOUTE *gp_Vou) {

  double freq_alt_mic, freq_azi_mic ;
  double freq_alt_mot, freq_azi_mot ;
  double freq_alt_bru, freq_azi_bru ;
  double azi_rot,   alt_rot ;
  
  ARBO(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // Calcul de la periode necessaire si pas de terme D ni 2 puissance N
  // ceci dans le cas ou on genere directement la frequence de reference
  // depuis la RASPI, au lieu d'utiliser 2 diviseurs de frequence en serie
  // qui correspondent au diviseur 2^N et diviseur programmable
  // Ou bien ces periodes / frequences servent pour la generation des signaux PWM
  
  // Les flags de reversibilite *_ROT permettent de ne pas etre dependant du sens de branchement des moteurs !

  if ( gp_Cal_Par->par_azi_rev == 0 )       azi_rot = -1.0 ; else azi_rot = 1.0 ;
  if ( gp_Cal_Par->par_alt_rev == 0 )       alt_rot = -1.0 ; else alt_rot = 1.0 ;

  // La periode de base en mode controleur est directement geree par ce controleur
  // La periode de base en mode PWM est le quart d'une sinusoide
  
  TRACE2("%f %f %f %f %f",gp_Sui->acc_azi, gp_Vou->acc, gp_Cal_Par->par_azi_red_tot, gp_Ast->Va, azi_rot);
  TRACE2("%f %f %f %f %f",gp_Sui->acc_alt, gp_Vou->acc, gp_Cal_Par->par_alt_red_tot, gp_Ast->Vh, alt_rot);

  /*------------------------*/
  /* Calculs des frequences */
  /*------------------------*/

  /* calcul des frequences brutes non corrigees = brutes */
  /* ces frequences ne sont pas corrigees par les accelerations diverses */

  freq_azi_bru = gp_Cal_Par->par_azi_red_tot * gp_Ast->Va * azi_rot / CALCUL_DIVISEUR_SEPCIFIQUE / CALCUL_PI_FOIS_DEUX / 4  ;
  freq_alt_bru = gp_Cal_Par->par_alt_red_tot * gp_Ast->Vh * alt_rot / CALCUL_DIVISEUR_SEPCIFIQUE / CALCUL_PI_FOIS_DEUX / 4  ;

  /* calcul des frequences corrigees avant prise en compte micro pas */
  /* ces frequences sont corrigees par les accelerations diverses */

  freq_azi_mot = gp_Sui->acc_azi_pid * gp_Sui->acc_azi * gp_Vou->acc * freq_azi_bru  ;
  freq_alt_mot = gp_Sui->acc_alt_pid * gp_Sui->acc_alt * gp_Vou->acc * freq_alt_bru  ;

  /* calcul des frequences finales UTILES */
  /* La frequence retenue est la frequence moteur multipliee par le nb de micro pas */

  freq_azi_mic     = freq_azi_mot * gp_Cal_Par->par_azi_red4 ;
  freq_alt_mic     = freq_alt_mot * gp_Cal_Par->par_alt_red_4 ;

  /*-----------------------------------------------------*/
  /* Le calcul est different si on utilise un controleur                    */
  /* la frequence est la frequence moteur (sans tenir compte des micro pas) */
  /*-----------------------------------------------------*/

  if ( gp_Dev->use_controler ) {
    freq_azi_mic   = freq_azi_mot ;
    freq_alt_mic   = freq_alt_mot ;
  }

  /*------------------------*/
  /* Calculs des periodes  */
  /*------------------------*/

  ARBO(__func__,2,"1er lock") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  pthread_mutex_lock(& gp_Mut->mut_azi );
      
    gp_Sui->Sa_old = gp_Sui->Sa ; 
    gp_Sui->Sa     = (int)SGN(freq_azi_mic)  ;

    gp_Sui->Fa_mic = fabs(freq_azi_mic )  ;
    gp_Sui->Fa_bru = fabs(freq_azi_bru )  ;    
    gp_Sui->Fa_mot = fabs(freq_azi_mot) ;

    gp_Sui->Ta_mic = 1 / gp_Sui->Fa_mic ;
    gp_Sui->Ta_bru = 1 / gp_Sui->Fa_bru ;
    gp_Sui->Ta_mot = 1 / gp_Sui->Fa_mot ;

  Trace(""); pthread_mutex_unlock(& gp_Mut->mut_azi );
  
  ARBO(__func__,2,"2eme lock") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  pthread_mutex_lock(& gp_Mut->mut_alt );

    gp_Sui->Sh_old = gp_Sui->Sh ; 
    gp_Sui->Sh     = (int)SGN(freq_alt_mic) ;

    gp_Sui->Fh_mic = fabs(freq_alt_mic)  ;
    gp_Sui->Fh_bru = fabs(freq_alt_bru) ;
    gp_Sui->Fh_mot = fabs(freq_alt_mot) ;
    
    gp_Sui->Th_mic     = 1 / gp_Sui->Fh_mic ;
    gp_Sui->Th_bru = 1 / gp_Sui->Fh_bru ;
    gp_Sui->Th_mot = 1 / gp_Sui->Fh_mot ;

  Trace(""); pthread_mutex_unlock(& gp_Mut->mut_alt );

  ARBO(__func__,2,"fin") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

}

/*****************************************************************************************
* @fn     : CALCUL_PERIODES_SUIVI_MANUEL
* @author : s.gravois
* @brief  : est cense calculer les "vrais" periodes et frequences des moteurs pas a pas
* @brief  :  - en tant que paramtres de la sinusoide de reference
* @brief  :  - en fonction du suivi manuel effectuee avec les boutons E/N/S/O et reset
* @param  : STRUCT_SUIVI * gp_Sui
* @param  : STRUCT_ASTRE * gp_Ast
* @param  : STRUCT_VOUTE * gp_Vou
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : a modifier car cela marche pas tres bien (cf astro.c : MENU_MANUEL_1)
* @todo   : le but de ce suivi est de deduire des actions N-S-O-E de l'utilisateur 
* @todo   : les periodes / frequences en azimut et altitude
*****************************************************************************************/

void CALCUL_PERIODES_SUIVI_MANUEL(STRUCT_ASTRE *gp_Ast, STRUCT_SUIVI * gp_Sui, STRUCT_VOUTE *gp_Vou) {

  double frequence ;
  double azi_rot, alt_rot ;
  
  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // calcul de la periode en fonction des informations de suivi
  // renvoyer par les boutons de la raquette
  // on deduit la frequence du nombre de pas comptes et de la periode ecoulee
  
  if ( gp_Sui->pas_azi !=  gp_Sui->pas_azi_old ) {
    if ( gp_Sui->pas_azi !=0 ) {
      if ( gp_Cal_Par->par_azi_rev == 0 ) azi_rot = -1 ; else azi_rot = 1 ;
    
      //frequence = azi_rot * (double)gp_Sui->pas_azi / gp_Sui->d_appui_raq_azi ;
      frequence = azi_rot * (double)gp_Sui->pas_azi ;
    
      pthread_mutex_lock(& gp_Mut->mut_azi );
  
        gp_Ast->Va    = frequence * CALCUL_DIVISEUR_SEPCIFIQUE * CALCUL_PI_FOIS_DEUX / ( gp_Vou->acc * gp_Cal_Par->par_azi_red_tot ) ;
        gp_Sui->Sa_old = gp_Sui->Sa ; 
        gp_Sui->Sa     = (int)SGN(frequence)  ;
        gp_Sui->Fa_mic = fabs(frequence) ;
        gp_Sui->Ta_mic = 1 / gp_Sui->Fa_mic ;
    
      Trace(""); pthread_mutex_unlock(& gp_Mut->mut_azi );
    
      TRACE1("Ta_mic = %2.4f Th_mic = %2.4f Fa_mic = %2.4f Fh_mic = %2.4f\n",gp_Sui->Ta_mic, gp_Sui->Th_mic, gp_Sui->Fa_mic, gp_Sui->Fh_mic) ;
    }
  }
  if ( gp_Sui->pas_alt !=  gp_Sui->pas_alt_old ) {
    if ( gp_Sui->pas_alt !=0 ) {
      if ( gp_Cal_Par->par_alt_rev == 0 ) alt_rot = -1 ; else alt_rot = 1 ;
    
      //frequence = alt_rot * (double)gp_Sui->pas_alt / gp_Sui->d_appui_raq_alt ;
      frequence = alt_rot * (double)gp_Sui->pas_alt ;
    
      // fprintf(stderr, "CALCUL_PERIODES_SUIVI_MANUEL demande le mutex\n");
      pthread_mutex_lock( & gp_Mut->mut_alt );
    
        gp_Ast->Vh     = frequence * CALCUL_DIVISEUR_SEPCIFIQUE * CALCUL_PI_FOIS_DEUX / ( gp_Vou->acc * gp_Cal_Par->par_alt_red_tot ) ;
        gp_Sui->Sh_old = gp_Sui->Sh ; 
        gp_Sui->Sh     = (int)SGN(frequence)  ;
        gp_Sui->Fh_mic     = fabs(frequence) ;
        gp_Sui->Th_mic     = 1 / gp_Sui->Fh_mic ;
  
        // fprintf(stderr, "CALCUL_PERIODES_SUIVI_MANUEL relache le mutex\n");
      Trace(""); pthread_mutex_unlock( & gp_Mut->mut_alt );
    
      TRACE1("Ta_mic = %2.4f Th_mic = %2.4f Fa_mic = %2.4f Fh_mic = %2.4f\n",gp_Sui->Ta_mic, gp_Sui->Th_mic, gp_Sui->Fa_mic, gp_Sui->Fh_mic) ;
    }
  }
}

/*****************************************************************************************
* @fn     : CALCUL_AFFICHER_MSG_ANGLE
* @author : s.gravois
* @brief  : Afficher message puis angle sous la forme degres minute seconde
* @param  : STRUCT_ANGLE *angle
* @param  : char * mesg
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : ras
*****************************************************************************************/

void CALCUL_AFFICHER_MSG_ANGLE( char * mesg, STRUCT_ANGLE *angle ) {

  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace2("%s : %d°%d'%.2f\" %.4f (deg)", mesg, angle->DD, angle->MM, (double)angle->SS, angle->ad ) ;
}

/*****************************************************************************************
* @fn     : CALCUL_ANGLE_VERS_DMS
* @author : s.gravois
* @brief  : Convertit angle en heure minutes secondes decimales
* @param  : STRUCT_ANGLE *angle
* @date   : 2022-03-18 creation 
* @todo   : ras
*****************************************************************************************/

void CALCUL_ANGLE_VERS_DMS(STRUCT_ANGLE *angle) {
  
  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

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
* @param  : STRUCT_ANGLE *angle
* @date   : 2022-03-18 creation 
* @todo   : ras
*****************************************************************************************/

void CALCUL_CONVERSIONS_ANGLES(STRUCT_ASTRE *gp_Ast) {
  
  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /* ----------------------------*/
  /* azimut et altitude          */
  /* ----------------------------*/

  (gp_Ast->at).hd = gp_Ast->a * 24.0 / CALCUL_PI_FOIS_DEUX ;
  TEMPS_CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->at) ;

  (gp_Ast->AZIa).AD = gp_Ast->a ;
  (gp_Ast->AZIa).ad = gp_Ast->a * CALCUL_UN_RADIAN_EN_DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&gp_Ast->AZIa) ;
  
  (gp_Ast->ht).hd = gp_Ast->h * 24.0 / CALCUL_PI_FOIS_DEUX ;
  TEMPS_CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->ht) ;
  
  (gp_Ast->ALTa).AD = gp_Ast->h ;
  (gp_Ast->ALTa).ad = gp_Ast->h * CALCUL_UN_RADIAN_EN_DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&gp_Ast->ALTa) ;

  /* -----------------------------*/
  /* angle horaire et declinaison */
  /* ascension droite             */
  /* -----------------------------*/

  (gp_Ast->AGHt).hd = gp_Ast->AGH * 24.0 / CALCUL_PI_FOIS_DEUX ;
  TEMPS_CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AGHt) ;
  
  (gp_Ast->DECt).hd = gp_Ast->DEC * 24.0 / CALCUL_PI_FOIS_DEUX ;
  TEMPS_CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->DECt) ;

  (gp_Ast->ASCt).hd  = gp_Ast->ASC * 24.0 / CALCUL_PI_FOIS_DEUX ;
  TEMPS_CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->ASCt) ;

  /* -------------------------------*/

  (gp_Ast->AGHa).AD = gp_Ast->AGH ;
  (gp_Ast->AGHa).ad = gp_Ast->AGH * CALCUL_UN_RADIAN_EN_DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&gp_Ast->AGHa) ;

  (gp_Ast->ASCa).AD = gp_Ast->ASC ;
  (gp_Ast->ASCa).ad = gp_Ast->ASC * CALCUL_UN_RADIAN_EN_DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&gp_Ast->ASCa) ;

  (gp_Ast->DECa).AD = gp_Ast->DEC ;
  (gp_Ast->DECa).ad = gp_Ast->DEC * CALCUL_UN_RADIAN_EN_DEGRES ;
  CALCUL_ANGLE_VERS_DMS(&gp_Ast->DECa) ;

  /* -------------------------------*/
  /* les calculs intermediaires     */
  /* -------------------------------*/

  (gp_Ast->AGH0t).hd = gp_Ast->AGH0 * 24.0 / CALCUL_PI_FOIS_DEUX ;
  TEMPS_CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AGH0t) ;

  (gp_Ast->AGH1t).hd = gp_Ast->AGH1 * 24.0 / CALCUL_PI_FOIS_DEUX ;
  TEMPS_CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AGH1t) ;

  (gp_Ast->AGH2t).hd = gp_Ast->AGH2 * 24.0 / CALCUL_PI_FOIS_DEUX ;
  TEMPS_CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AGH2t) ;

  (gp_Ast->AZI0t).hd = gp_Ast->AZI0 * 24.0 / CALCUL_PI_FOIS_DEUX ;
  TEMPS_CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AZI0t) ;

  (gp_Ast->AZI1t).hd = gp_Ast->AZI1 * 24.0 / CALCUL_PI_FOIS_DEUX ;
  TEMPS_CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AZI1t) ;

  (gp_Ast->AZI2t).hd = gp_Ast->AZI2 * 24.0 / CALCUL_PI_FOIS_DEUX ;
  TEMPS_CALCUL_TEMPS_DEC_VERS_HMS(&gp_Ast->AZI2t) ;
}

/*****************************************************************************************
* @fn     : CALCUL_ANGLE_HORAIRE
* @author : s.gravois
* @brief  : Convertit heure decimale en heure minutes secondes decimales
* @param  : STRUCT_TIME * gp_Tim
* @date   : 2022-03-19 correction valeur quand negatif 
* @todo   : ras
*****************************************************************************************/

void CALCUL_ANGLE_HORAIRE( STRUCT_LIEU *l_li, STRUCT_ASTRE *gp_Ast ) {
  
  ARBO(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // en radians 

  /* correction ascension droite si negatif (on ajoute 360 degres) */

  if ( gp_Ast->ASC < 0 ) {
    gp_Ast->ASC += 2*M_PI ;
  }

  /* calcul */ 

  gp_Ast->AGH = l_li->TSR - gp_Ast->ASC ;
  
  /* correction angle horaire si negatif (on ajoute 360 degres) */

  if ( gp_Ast->AGH < 0 ) {
    gp_Ast->AGH += 2*M_PI ;
  }

  CALCUL_CONVERSIONS_ANGLES( gp_Ast) ;
  
  Trace1("ascension droite (deg)   = %.2f", gp_Ast->ASC * CALCUL_UN_RADIAN_EN_DEGRES) ;
  Trace1("temps sideral (rad)      = %.2f", l_li->TSR ) ;
  Trace1("angle horaire (deg)      = %.2f", gp_Ast->AGH * CALCUL_UN_RADIAN_EN_DEGRES) ;

  /*
    ASTRE_AFFICHER_MODE_STELLARIUM(gp_Ast) ;
  */
}

/*****************************************************************************************
* @fn     : CALCUL_ASCENSION_DROITE
* @author : s.gravois
* @brief  : Convertit heure decimale en heure minutes secondes decimales
* @param  : STRUCT_TIME * gp_Tim
* @date   : 2022-03-19 correction valeur quand negatif 
* @date   : 2022-03-20 verification que angle horaire est positif
* @todo   : ras
*****************************************************************************************/

void CALCUL_ASCENSION_DROITE( STRUCT_LIEU *l_li, STRUCT_ASTRE *gp_Ast ) {
  
  ARBO(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // en radians 
  
  /* correction angle horaire si negatif (on ajoute 360 degres) */

  if ( gp_Ast->AGH < 0 ) {
    gp_Ast->AGH += 2*M_PI ;
  }

  /* calcul */ 

  gp_Ast->ASC = l_li->TSR - gp_Ast->AGH ;
  
  /* correction ascension droite si negatif (on ajoute 360 degres) */

  if ( gp_Ast->ASC < 0 ) {
    gp_Ast->ASC += 2*M_PI ;
  }

  CALCUL_CONVERSIONS_ANGLES( gp_Ast) ;
  
  Trace1("ascension droite (deg)   = %.2f", gp_Ast->ASC * CALCUL_UN_RADIAN_EN_DEGRES) ;
  Trace1("temps sideral (rad)      = %.2f", l_li->TSR ) ;
  Trace1("angle horaire (deg)      = %.2f", gp_Ast->AGH * CALCUL_UN_RADIAN_EN_DEGRES) ;

  /*
    ASTRE_AFFICHER_MODE_STELLARIUM(gp_Ast) ;
  */
}
/*****************************************************************************************
* @fn     : CALCUL_ASTRE_RECUP_TYPE_ET_NOM
* @author : s.gravois
* @brief  : Aggregation partie de code dans CALCUL_TOUT ici (recuperation type et nom astre)
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-09-29 creation
*****************************************************************************************/

void CALCUL_ASTRE_RECUP_TYPE_ET_NOM(void) {

  int pos=3;
  int len=0 ;
  char c_sub[32];

  ARBO(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /*---------------- evolution 19/01/2022 ----------------
  * prise en compte du fait que la as peut avoir plusieurs
  * prefixe dans son nom (PLA/NGC/MES/ETO mais aussi EQU/AZI)
  * Cela permet de determine le mode de calculs de l'as
  * et le type de l'as (CIEL_PROFOND etc...)
  -------------------------------------------------------*/

  gp_Ast->en_type = ASTRE_INDETERMINE ;
  gp_Ast->en_mode = CALCUL_AZIMUTAL_VERS_EQUATORIAL ;   

  len = strlen(gp_Ast->nom)-pos ;

  if ( len > pos ) {

    if      ( strstr( gp_Ast->nom, CONFIG_MES ) != NULL ) gp_Ast->en_type = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->nom, CONFIG_NGC ) != NULL ) gp_Ast->en_type = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->nom, CONFIG_ETO ) != NULL ) gp_Ast->en_type = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->nom, CONFIG_PLA ) != NULL ) gp_Ast->en_type = ASTRE_PLANETE ;
    else if ( strstr( gp_Ast->nom, CONFIG_AZI ) != NULL ) gp_Ast->en_type = ASTRE_INDETERMINE ;
    else if ( strstr( gp_Ast->nom, CONFIG_EQU ) != NULL ) gp_Ast->en_type = ASTRE_INDETERMINE ;
    else                                                   gp_Ast->en_type = ASTRE_INDETERMINE ;
    /* TODO : completer avec les types manquants */ 

    if      ( strstr( gp_Ast->nom, CONFIG_MES ) != NULL ) gp_Ast->en_mode = CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_NGC ) != NULL ) gp_Ast->en_mode = CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_ETO ) != NULL ) gp_Ast->en_mode = CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_PLA ) != NULL ) gp_Ast->en_mode = CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_AZI ) != NULL ) gp_Ast->en_mode = CALCUL_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_EQU ) != NULL ) gp_Ast->en_mode = CALCUL_AZIMUTAL_VERS_EQUATORIAL ;
    else                                                   gp_Ast->en_mode = CALCUL_AZIMUTAL_VERS_EQUATORIAL ; 
    /* TODO : completer avec les types manquants */ 
  }
  else {
    gp_Ast->en_type = ASTRE_INDETERMINE ;
  }
  /* recuperation du numero apres les 3 premiers caracteres */

  memset( c_sub, 0, sizeof(c_sub));
  
  if ( gp_Ast->en_type != ASTRE_INDETERMINE ) {

    if( len > pos ) {

      if ( len > sizeof(c_sub) ) { Trace("erreur : %d >  %d", len,(int)sizeof(c_sub) ) ;}
      if ( len <= 0 )            { Trace("erreur : %d <= 0",   len) ;}

      memcpy( c_sub, &gp_Ast->nom[pos], len );

      c_sub[len] = CONFIG_ZERO_CHAR ;
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
* @param  : STRUCT_ASTRE * gp_Ast
* @param  : STRUCT_LIEU  * gp_Lie
* @param  : STRUCT_SUIVI * gp_Suivi
* @date   : 2022-06-13 correction argument 1 de SOLAR_SYSTEM astre->nom => astre->infos
* @date   : 2022-09-29 deplacement code recup type et nom dans une fct dediee
* @todo   : ras
*****************************************************************************************/

void CALCUL_TOUT(void) {

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  CALCUL_ASTRE_RECUP_TYPE_ET_NOM() ;

  switch (gp_Ast->en_type) {

    /* ----------------------------------------------------------------- */
      /* pour un as indetermine 
         on calcule les coordonnees equatoriales 
         necessaires au calcul des vitesses 
      */

    case ASTRE_INDETERMINE :
    
      pthread_mutex_lock( & gp_Mut->mut_cal );
      
      TEMPS_CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;
      /* CALCUL_AZIMUT       ( gp_Lie, gp_Ast) ; */

      if ( gp_Ast->en_mode == CALCUL_AZIMUTAL_VERS_EQUATORIAL ) {
        
        CALCUL_EQUATEUR        ( gp_Lie, gp_Ast ) ;
        CALCUL_ASCENSION_DROITE( gp_Lie, gp_Ast ) ;
      }
      else {
        CALCUL_ANGLE_HORAIRE( gp_Lie, gp_Ast ) ;
        CALCUL_AZIMUT       ( gp_Lie, gp_Ast) ;
      }

      CALCUL_VITESSES     ( gp_Lie, gp_Ast, gp_Sui) ;
      CALCUL_PERIODE      ( gp_Ast, gp_Sui, gp_Vou) ;
      
      Trace(""); pthread_mutex_unlock( & gp_Mut->mut_cal ) ;
      
      break ;

    /* ----------------------------------------------------------------- */

    case ASTRE_CIEL_PROFOND :
    
      pthread_mutex_lock( & gp_Mut->mut_cal );
      
      TEMPS_CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;

      CALCUL_ANGLE_HORAIRE( gp_Lie, gp_Ast ) ;
      CALCUL_AZIMUT       ( gp_Lie, gp_Ast) ;
      
      CALCUL_VITESSES     ( gp_Lie, gp_Ast, gp_Sui) ;
      CALCUL_PERIODE      ( gp_Ast, gp_Sui, gp_Vou) ;
      
      Trace(""); pthread_mutex_unlock( & gp_Mut->mut_cal ) ;
      
      break ;
      
    /* ----------------------------------------------------------------- */
    
    case ASTRE_PLANETE :
      
      pthread_mutex_lock( & gp_Mut->mut_cal );
            
      TEMPS_CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;
      
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

      Trace(""); pthread_mutex_unlock( & gp_Mut->mut_cal ) ;
      
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
         (nouveau code 2022) CALCUL_AZIMUTAL_VERS_EQUATORIAL
         ou bien CALCUL_AZIMUTAL_VERS_EQUATORIAL */

      /* 
      case CAPTEURS :

        TEMPS_CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;
        
        // TODO : modifier / completer / corriger ..
        
        if ( gp_Dev->use_capteurs ) { 
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

  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
/*
  TEMPS_CALCUL_TEMPS_SIDERAL    ( gp_Lie, gp_Tim ) ;
  CALCUL_EQUATEUR         ( gp_Lie, gp_Ast ) ;
  CALCUL_ASCENSION_DROITE ( gp_Lie, gp_Ast ) ;
  CALCUL_VITESSES         ( gp_Lie, gp_Ast, gp_Sui) ;
  CALCUL_PERIODE          ( gp_Ast, gp_Sui, gp_Vou) ;
*/
  fout=fopen("voute.csv","w") ;
  
  TEMPS_CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;

  // On fait varier les coordonnees azimutales
  
  for(h=-(M_PI/2)+(gp_Lie->lat)+0.001;h<M_PI/2;h+=gp_Vou->pas)
    if (h>=0) 
    for(a=-M_PI +0.001 ;a<M_PI;a+=gp_Vou->pas){
     
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
       gp_Ast->An = M_PI/2 ;
     
     CALCUL_GEODE( gp_Ast ) ;
     
     Trace1("%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f\n", \
       gp_Ast->a * CALCUL_UN_RADIAN_EN_DEGRES, \
       gp_Ast->h * CALCUL_UN_RADIAN_EN_DEGRES, \
       gp_Ast->AGH * CALCUL_UN_RADIAN_EN_DEGRES, \
       gp_Ast->DEC * CALCUL_UN_RADIAN_EN_DEGRES, \
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
          (int)((gp_Ast->a)*CALCUL_UN_RADIAN_EN_DEGRES),\
          (int)((gp_Ast->h)*CALCUL_UN_RADIAN_EN_DEGRES),\
          (int)((gp_Ast->AGH)*CALCUL_UN_RADIAN_EN_DEGRES),\
          (int)((gp_Ast->DEC)*CALCUL_UN_RADIAN_EN_DEGRES),\
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




