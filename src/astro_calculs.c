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
#               * passage dernier arg de SOLAR_SYSTEM abec gp_Ast->ast_num
# 12/03/2022  | * correction calcul DEC (declinaison) - => + dans formule
# 
# mars  2022  | * reprise des calculs et ajout de fonctions de conversions
#               * correction deuxieme argument appel SOLAR_SYSTEM
# -------------------------------------------------------------- 
*/

#include "astro_calculs.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

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
double RAD  (int degres, int minutes )                  { return ((double)degres + ( SGN(degres)*(double)minutes) / 60.0 ) / CALCULS_UN_RADIAN_EN_DEGRES ; }
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

double RAD  (int degres, int minutes )                  { return ((double)degres + ( SGN(degres)*(double)minutes) / 60.0 ) / CALCULS_UN_RADIAN_EN_DEGRES ; }
double DEG  (int degres, int minutes )                  { return (double)degres  + ( SGN(degres)*(double)minutes) / 60.0 ; }

/*****************************************************************************************
* @fn     : CALCULS_REDUCTION_TOTALE
* @author : s.gravois
* @brief  : permet de calculer unela reduction totale de la vitesse 
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
*****************************************************************************************/

double CALCULS_REDUCTION_TOTALE(void) {

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  return 0.0 ;
}

/*****************************************************************************************
* @fn     : CALCULS_INIT
* @author : s.gravois
* @brief  : Initialise la structure calculs STRUCT_CALCULS
* @brief  :  en supposant la voute de rayon 1
* @param  : STRUCT_CALCULS * gp_Cal
* @date   : 2022-11-17 creation entete doxygen
* @date   : 2022-12-17 ajout structures necessaires pour init pointeurs
*****************************************************************************************/

void   CALCULS_INIT ( \

  STRUCT_CALCULS          * gp_Cal , \
  STRUCT_SUIVI_FREQUENCES * gp_Fre , \
  STRUCT_SUIVI_STATS      * gp_Sta , \
  STRUCT_SUIVI_PAS        * gp_Pas , \
  STRUCT_ASTRE            * gp_Ast , \
  STRUCT_VOUTE            * gp_Vou , \
  STRUCT_LIEU             * gp_Lie , \
  STRUCT_DEVICES          * gp_Dev , \
  STRUCT_MUTEXS           * gp_Mut , \
  STRUCT_TIME             * gp_Tim , \
  STRUCT_SUIVI            * gp_Sui ) {

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & gp_Cal->cal_mutex ) ;
 
  gp_Cal->cal_mode = CALCULS_EQUATORIAL_VERS_AZIMUTAL ;

  gp_Cal->cal_p_Fre = gp_Fre ;
  gp_Cal->cal_p_Sta = gp_Sta ;
  gp_Cal->cal_p_Pas = gp_Pas ;
  gp_Cal->cal_p_Ast = gp_Ast ;
  gp_Cal->cal_p_Cal = gp_Cal ;
  gp_Cal->cal_p_Vou = gp_Vou ;
  gp_Cal->cal_p_Lie = gp_Lie ;
  gp_Cal->cal_p_Dev = gp_Dev ;
  gp_Cal->cal_p_Mut = gp_Mut ;
  gp_Cal->cal_p_Tim = gp_Tim ;
  gp_Cal->cal_p_Sui = gp_Sui ;

  return ;
}

/*****************************************************************************************
* @fn     : CALCULS_COORD_R3
* @author : s.gravois
* @brief  : Calcule les coordonnees (x,y,z) dans R3 a partir de azimut et altitude
* @brief  :  en supposant la voute de rayon 1
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-10-11 creation entete doxygen 
*****************************************************************************************/

void CALCULS_COORD_R3(void) {

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // on deduit de l'azimut et de l'altitude
  // les coordonnees x y et z dans la geode d'observation
  
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Ast->ast_mutex ) ;

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

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Ast->ast_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CALCULS_AZIMUT
* @author : s.gravois
* @brief  : Transforme les coordonnees siderales en coordonnees azimutales
* @param  : STRUCT_LIEU  * gp_Lie
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-10-11 creation entete doxygen 
*****************************************************************************************/

void CALCULS_AZIMUT(void) {
  
  // transforme les coordonnees "sid�rales" en coordonnees azimutales
  // permet de trouver l'azimut et l'altitude en fonction de l'angle horaire
  // par rapport au m�ridien (SUD) et la declinaison
  // ATTENTION !! angle horaire = ascension droite sid�rale corrig�e
  // avec le temps sid�ral : une conversion doit etre faite pour trouver
  // gp_Ast->AGH  = TS - (asc droite absolue)
  // Une fonction de calcul du temps sid�ral doit etre faite ult�rieurement 
  
  double lat,A,H,a0,h,a1,a2,af ;
  
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  lat= gp_Lie->lie_lat ;
  A  = gp_Ast->AGH ; // FIXME : angle horaire calcule prealablement dans astro.c en theorie
  H  = gp_Ast->DEC ; // FIXME : declinaison - seule valeur qui ne change pas 
  
  Trace2("agh = %2.3f\tH = %2.3f\t",(gp_Ast->AGH)*CALCULS_UN_RADIAN_EN_DEGRES,(gp_Ast->DEC)*CALCULS_UN_RADIAN_EN_DEGRES) ;

  h  = ALT(lat,A,H) ;
  a0 = AZI0(A,H,h) ;
  a1 = AZI1(lat,A,H,h) ;
  a2 = AZI2(lat,h,H) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Ast->ast_mutex ) ;

  gp_Ast->h = h ;
  gp_Ast->a = SGN(a0)*a2 ;

  /* Si azimut negatif on ajoute 360 degres */  
  
  if ( gp_Ast->a < 0 ) {
    gp_Ast->a += 2*M_PI ;
  }

   // resultats de calculs : pour tests (a modifier : supprimer)
  
  gp_Ast->AZI0 = a0 ;
  gp_Ast->AZI1 = a1 ;
  gp_Ast->AZI2 = a2 ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Ast->ast_mutex ) ;

  CALCULS_CONVERSIONS_ANGLES() ;

  Trace2("AZI0 = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_azi0_t.tim_HH, \
    gp_Ast->ast_azi0_t.tim_MM , \
    gp_Ast->ast_azi0_t.tim_SS, \
    gp_Ast->AZI0 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace2("AZI1 = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_azi1_t.tim_HH, \
    gp_Ast->ast_azi1_t.tim_MM , \
    gp_Ast->ast_azi1_t.tim_SS, \
    gp_Ast->AZI1 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace2("AZI2 = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_azi2_t.tim_HH, \
    gp_Ast->ast_azi2_t.tim_MM , \
    gp_Ast->ast_azi2_t.tim_SS, \
    gp_Ast->AZI2 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("azi = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_agh_t.tim_HH, \
    gp_Ast->ast_agh_t.tim_MM, \
    gp_Ast->ast_agh_t.tim_SS, \
    gp_Ast->AGH * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("alt = %f (deg)" , (gp_Ast->h)*CALCULS_UN_RADIAN_EN_DEGRES) ;

  return ;
}

/*****************************************************************************************
* @fn     : CALCULS_EQUATEUR
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

void CALCULS_EQUATEUR(void) {

  // transforme les coordonnees azimutales en coordonnees siderales (angle horaire)
  // permet de trouver les coordonn�es siderales en fonction de l'azimut et de l'altitude
  // Pour avoir la correspondance avec les coordonnn�es �quatoriales brutes, il faut
  // utiliser la conversion AH = TS - ASC
  // AH  : angle horaire
  // TS  : temps sid�ral
  // ASC : ascension droite brute
  
  double A0,A2,A1,a,h,lat,Af,agh,H ;

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1("avant calcul => a = %2.3f\th = %2.3f\t=>agh = %2.3f\tH=%2.3f",\
    (gp_Ast->a)   * CALCULS_UN_RADIAN_EN_DEGRES,\
    (gp_Ast->h)   * CALCULS_UN_RADIAN_EN_DEGRES,\
    (gp_Ast->AGH) * CALCULS_UN_RADIAN_EN_DEGRES,\
    (gp_Ast->DEC) * CALCULS_UN_RADIAN_EN_DEGRES) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Lie->lie_mutex ) ;

  lat = gp_Lie->lie_lat ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Lie->lie_mutex ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Ast->ast_mutex ) ;

  a = gp_Ast->a ;
  h = gp_Ast->h ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Ast->ast_mutex ) ;

  // FIXME : obtention de declinaison et ascension droite (d'apres formules usuelles de Gauss)
  
  H  = DEC(lat,a,h) ;
  A0 = AGH0(a,h,H) ;       /* correct , ajouter 2*M_PI si negatif */
  A1 = AGH1(lat,a,h,H)+M_PI; /* correct , ajouter M_PI */ 
  A2 = AGH2(lat,H,h) ;     /* a anlyser = mauvais */ 
  
  if ( A0 < 0 ) {
    A0 += 2 * M_PI ;
  }
  // gp_Ast->AGH = SGN(A0)*A1  ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Ast->ast_mutex ) ;

  gp_Ast->DEC  = H ;
  gp_Ast->AGH = A0 ;
  gp_Ast->AGH0 = A0 ;
  gp_Ast->AGH1 = A1 ;
  gp_Ast->AGH2 = A2 ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Ast->ast_mutex ) ;

  CALCULS_CONVERSIONS_ANGLES() ;
  
  Trace1(" %s : ASC = %d.%d.%d (hms) %.2f (deg) %.2f (rad)", \
       gp_Ast->nom , \
       gp_Ast->ast_asc_t.tim_HH, \
       gp_Ast->ast_asc_t.tim_MM, \
       gp_Ast->ast_asc_t.tim_SS, \
       gp_Ast->ASC * CALCULS_UN_RADIAN_EN_DEGRES, \
       gp_Ast->ASC \
  ) ; 

  Trace1("AH0   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_agh0_t.tim_HH, gp_Ast->ast_agh0_t.tim_MM , gp_Ast->ast_agh0_t.tim_SS, gp_Ast->AGH0 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("AH1   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_agh1_t.tim_HH, gp_Ast->ast_agh1_t.tim_MM , gp_Ast->ast_agh1_t.tim_SS, gp_Ast->AGH1 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("AH2   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_agh2_t.tim_HH, gp_Ast->ast_agh2_t.tim_MM , gp_Ast->ast_agh2_t.tim_SS, gp_Ast->AGH2 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("AH    (deg) = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_agh_t.tim_HH, gp_Ast->ast_agh_t.tim_MM , gp_Ast->ast_agh_t.tim_SS, gp_Ast->AGH * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("DEC   (deg) = %f" , (gp_Ast->DEC)*CALCULS_UN_RADIAN_EN_DEGRES) ;

  Trace1("apres calcul =>a = %2.3f\th = %2.3f\t=>agh = %2.3f\tH=%2.3f",\
   (gp_Ast->a)*CALCULS_UN_RADIAN_EN_DEGRES,\
   (gp_Ast->h)*CALCULS_UN_RADIAN_EN_DEGRES,\
   (gp_Ast->AGH)*CALCULS_UN_RADIAN_EN_DEGRES,\
   (gp_Ast->DEC)*CALCULS_UN_RADIAN_EN_DEGRES) ;

   return ;
} 

/*****************************************************************************************
* @fn     : CALCULS_VITESSES_EQUATORIAL
* @author : s.gravois
* @brief  : MAJ de Va et Vh (vitesses sur les moteurs azimut et altitude) en vit equatoriales
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : 
*****************************************************************************************/

void CALCULS_VITESSES_EQUATORIAL(void) {

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Ast->ast_mutex ) ;

  gp_Ast->Va = -15.0 ;
  gp_Ast->Vh = -15.0 ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Ast->ast_mutex ) ;

  return ;
}

/*****************************************************************************************
* @fn     : CALCULS_VITESSES
* @author : s.gravois
* @brief  : MAJ de Va et Vh (vitesses sur les moteurs azimut et altitude) en vit equatoriales
* @param  : STRUCT_LIEU  * gp_Lie
* @param  : STRUCT_ASTRE * gp_Ast
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : 
*****************************************************************************************/

void CALCULS_VITESSES(void) {

  // le calcul des vitesses est fait avec
  // l'angle horaire et la declinaison (azimut et altitude inutiles)
 
  double G, Va, Vh, angle ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Sui->sui_mode_equatorial == 1 ) {
    gp_Ast->Va = -15.0 ;
    gp_Ast->Vh = -15.0 ;
  }
  else {
    angle=CALCULS_NB_SECARC_PAR_S_EQU ;
    G  = sqr(sin(gp_Ast->AGH))*sqr(cos(gp_Ast->DEC))+sqr(sin(gp_Lie->lie_lat)*cos(gp_Ast->DEC)*cos(gp_Ast->AGH)-cos(gp_Lie->lie_lat)*sin(gp_Ast->DEC)) ;
    Va = angle*(cos(gp_Ast->DEC)*(cos(gp_Lie->lie_lat)*cos(gp_Ast->AGH)*sin(gp_Ast->DEC)-sin(gp_Lie->lie_lat)*cos(gp_Ast->DEC)))/G ;
    Vh = angle*sin(gp_Ast->AGH)*cos(gp_Ast->DEC)*cos(gp_Lie->lie_lat)/sqrt(G) ;
    gp_Ast->Va=Va ;
    gp_Ast->Vh=Vh ;
  }
  Trace2("%2.0f\t%2.8f\t%2.4f\t=>\t%2.8f\t%2.8f\n",(gp_Lie->lie_lat)*CALCULS_UN_RADIAN_EN_DEGRES,(gp_Ast->AGH)*CALCULS_UN_RADIAN_EN_DEGRES,(gp_Ast->DEC)*CALCULS_UN_RADIAN_EN_DEGRES,Va,Vh) ;
}

/*****************************************************************************************
* @fn     : CALCULS_DIVISEUR_FREQUENCE
* @author : s.gravois
* @brief  : calcule les nombres a injecter dans des diviseurs de frequence 
* @param  : STRUCT_SUIVI * gp_Sui
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : TODO :  * a supprimer ? inutilise (a servi au debut du projet) 
*****************************************************************************************/

void CALCULS_DIVISEUR_FREQUENCE(void) {
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  gp_Sui->sui_Da = gp_Cal_Par->cal_par_azi_f * 2.0 * M_PI / ( gp_Cal_Par->cal_par_azi_red_tot * pow(2.0, gp_Cal_Par->cal_par_azi_n) * ( gp_Ast->Va / ( 60 * 60 * CALCULS_UN_RADIAN_EN_DEGRES ) )) ;
  gp_Sui->sui_Dh = gp_Cal_Par->cal_par_alt_f * 2.0 * M_PI / ( gp_Cal_Par->cal_par_alt_red_tot * pow(2.0, gp_Cal_Par->cal_par_alt_n) * ( gp_Ast->Vh / ( 60 * 60 * CALCULS_UN_RADIAN_EN_DEGRES ) )) ;
}

/*****************************************************************************************
* @fn     : CALCULS_PERIODE
* @author : s.gravois
* @brief  : calcule les "vraies" periodes et frequences des moteurs pas a pas
* @brief  : en tant que parametres de la sinusoide de reference 
* @param  : STRUCT_ASTRE * gp_Ast
* @param  : STRUCT_SUIVI * gp_Sui
* @param  : STRUCT_VOUTE * gp_Vou
* @date   : 2022-05 ajout non prise en compte des micro-pas pour la frequence moteur
* @date   : 2022-06 correction petite erreur sur fre_fa_mot / fre_fh_mot
* @date   : 2022-06 ajout champs xxx_bru freq /periode avant multiplication par acceleration
*****************************************************************************************/

void CALCULS_PERIODE(void) {

  double freq_alt_mic, freq_azi_mic ;
  double freq_alt_mot, freq_azi_mot ;
  double freq_alt_bru, freq_azi_bru ;
  double azi_rot,   alt_rot ;
  
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // Calcul de la periode necessaire si pas de terme D ni 2 puissance N
  // ceci dans le cas ou on genere directement la frequence de reference
  // depuis la RASPI, au lieu d'utiliser 2 diviseurs de frequence en serie
  // qui correspondent au diviseur 2^N et diviseur programmable
  // Ou bien ces periodes / frequences servent pour la generation des signaux PWM
  
  // Les flags de reversibilite *_ROT permettent de ne pas etre dependant du sens de branchement des moteurs !

  if ( gp_Cal_Par->cal_par_azi_rev == 0 )       azi_rot = -1.0 ; else azi_rot = 1.0 ;
  if ( gp_Cal_Par->cal_par_alt_rev == 0 )       alt_rot = -1.0 ; else alt_rot = 1.0 ;

  // La periode de base en mode controleur est directement geree par ce controleur
  // La periode de base en mode PWM est le quart d'une sinusoide
  
  Trace2("%f %f %f %f %f",gp_Pas->pas_acc_azi, gp_Vou->vou_acc, gp_Cal_Par->cal_par_azi_red_tot, gp_Ast->Va, azi_rot);
  Trace2("%f %f %f %f %f",gp_Pas->pas_acc_alt, gp_Vou->vou_acc, gp_Cal_Par->cal_par_alt_red_tot, gp_Ast->Vh, alt_rot);

  /*------------------------*/
  /* Calculs des frequences */
  /*------------------------*/

  /* calcul des frequences brutes non corrigees = brutes */
  /* ces frequences ne sont pas corrigees par les accelerations diverses */

  freq_azi_bru = gp_Cal_Par->cal_par_azi_red_tot * gp_Ast->Va * azi_rot / CALCULS_DIVISEUR_SEPCIFIQUE / CALCULS_PI_FOIS_DEUX / 4  ;
  freq_alt_bru = gp_Cal_Par->cal_par_alt_red_tot * gp_Ast->Vh * alt_rot / CALCULS_DIVISEUR_SEPCIFIQUE / CALCULS_PI_FOIS_DEUX / 4  ;

  /* calcul des frequences corrigees avant prise en compte micro pas */
  /* ces frequences sont corrigees par les accelerations diverses */

  freq_azi_mot = gp_Pid->pid_acc_azi * gp_Pas->pas_acc_azi * gp_Vou->vou_acc * freq_azi_bru  ;
  freq_alt_mot = gp_Pid->pid_acc_alt * gp_Pas->pas_acc_alt * gp_Vou->vou_acc * freq_alt_bru  ;

  /* calcul des frequences finales UTILES */
  /* La frequence retenue est la frequence moteur multipliee par le nb de micro pas */

  freq_azi_mic     = freq_azi_mot * gp_Cal_Par->cal_par_azi_red_4 ;
  freq_alt_mic     = freq_alt_mot * gp_Cal_Par->cal_par_alt_red_4 ;

  /*-----------------------------------------------------*/
  /* Le calcul est different si on utilise un controleur                    */
  /* la frequence est la frequence moteur (sans tenir compte des micro pas) */
  /*-----------------------------------------------------*/

  if ( gp_Dev->dev_use_controler ) {
    freq_azi_mic   = freq_azi_mot ;
    freq_alt_mic   = freq_alt_mot ;
  }

  /*------------------------*/
  /* Calculs des periodes  */
  /*------------------------*/

  pthread_mutex_lock(& gp_Mut->mut_glo_azi );
      
    gp_Fre->fre_sa_old = gp_Fre->fre_sa ; 
    gp_Fre->fre_sa     = (int)SGN(freq_azi_mic)  ;

    gp_Fre->fre_fa_mic = fabs(freq_azi_mic )  ;
    gp_Fre->fre_fa_bru = fabs(freq_azi_bru )  ;    
    gp_Fre->fre_fa_mot = fabs(freq_azi_mot) ;

    gp_Fre->fre_ta_mic = 1 / gp_Fre->fre_fa_mic ;
    gp_Fre->fre_ta_bru = 1 / gp_Fre->fre_fa_bru ;
    gp_Fre->fre_ta_mot = 1 / gp_Fre->fre_fa_mot ;

  pthread_mutex_unlock(& gp_Mut->mut_glo_azi );
  
  TraceArbo(__func__,2,"2eme lock") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  pthread_mutex_lock(& gp_Mut->mut_glo_alt );

    gp_Fre->fre_sh_old = gp_Fre->fre_sh ; 
    gp_Fre->fre_sh     = (int)SGN(freq_alt_mic) ;

    gp_Fre->fre_fh_mic = fabs(freq_alt_mic)  ;
    gp_Fre->fre_fh_bru = fabs(freq_alt_bru) ;
    gp_Fre->fre_fh_mot = fabs(freq_alt_mot) ;
    
    gp_Fre->fre_th_mic = 1 / gp_Fre->fre_fh_mic ;
    gp_Fre->fre_th_bru = 1 / gp_Fre->fre_fh_bru ;
    gp_Fre->fre_th_mot = 1 / gp_Fre->fre_fh_mot ;

  pthread_mutex_unlock(& gp_Mut->mut_glo_alt );

  TraceArbo(__func__,2,"fin") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

}

/*****************************************************************************************
* @fn     : CALCULS_PERIODES_SUIVI_MANUEL
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

void CALCULS_PERIODES_SUIVI_MANUEL(void) {

  double frequence ;
  double azi_rot, alt_rot ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // calcul de la periode en fonction des informations de suivi
  // renvoyer par les boutons de la raquette
  // on deduit la frequence du nombre de pas comptes et de la periode ecoulee
  
  if ( gp_Pas->pas_azi !=  gp_Pas->pas_azi_old ) {
    if ( gp_Pas->pas_azi !=0 ) {
      if ( gp_Cal_Par->cal_par_azi_rev == 0 ) azi_rot = -1 ; else azi_rot = 1 ;
    
      //frequence = azi_rot * (double)gp_Pas->pas_azi / t_appui_raq_azi ;
      frequence = azi_rot * (double)gp_Pas->pas_azi ;
    
      pthread_mutex_lock(& gp_Mut->mut_glo_azi );
  
        gp_Ast->Va = frequence * CALCULS_DIVISEUR_SEPCIFIQUE * CALCULS_PI_FOIS_DEUX / ( gp_Vou->vou_acc * gp_Cal_Par->cal_par_azi_red_tot ) ;
        gp_Fre->fre_sa_old = gp_Fre->fre_sa ; 
        gp_Fre->fre_sa     = (int)SGN(frequence)  ;
        gp_Fre->fre_fa_mic = fabs(frequence) ;
        gp_Fre->fre_ta_mic = 1 / gp_Fre->fre_fa_mic ;
    
      pthread_mutex_unlock(& gp_Mut->mut_glo_azi );
    
      Trace1("fre_ta_mic = %2.4f fre_th_mic = %2.4f fre_fa_mic = %2.4f fre_fh_mic = %2.4f\n", \
        gp_Fre->fre_ta_mic, \
        gp_Fre->fre_th_mic, \
        gp_Fre->fre_fa_mic, \
        gp_Fre->fre_fh_mic) ;
    }
  }
  if ( gp_Pas->pas_alt !=  gp_Pas->pas_alt_old ) {
    if ( gp_Pas->pas_alt !=0 ) {
      if ( gp_Cal_Par->cal_par_alt_rev == 0 ) alt_rot = -1 ; else alt_rot = 1 ;
    
      //frequence = alt_rot * (double)gp_Pas->pas_alt / t_appui_raq_alt ;
      frequence = alt_rot * (double)gp_Pas->pas_alt ;
    
      pthread_mutex_lock( & gp_Mut->mut_glo_alt );
    
        gp_Ast->Vh     = frequence * CALCULS_DIVISEUR_SEPCIFIQUE * CALCULS_PI_FOIS_DEUX / ( gp_Vou->vou_acc * gp_Cal_Par->cal_par_alt_red_tot ) ;
        gp_Fre->fre_sh_old = gp_Fre->fre_sh ; 
        gp_Fre->fre_sh         = (int)SGN(frequence)  ;
        gp_Fre->fre_fh_mic = fabs(frequence) ;
        gp_Fre->fre_th_mic = 1 / gp_Fre->fre_fh_mic ;
  
      pthread_mutex_unlock( & gp_Mut->mut_glo_alt );
    
      Trace1("fre_ta_mic = %2.4f fre_th_mic = %2.4f fre_fa_mic = %2.4f fre_fh_mic = %2.4f\n", \
        gp_Fre->fre_ta_mic, \
        gp_Fre->fre_th_mic, \
        gp_Fre->fre_fa_mic, \
        gp_Fre->fre_fh_mic) ;
    }
  }
}

/*****************************************************************************************
* @fn     : CALCULS_ANGLE_VERS_DMS
* @author : s.gravois
* @brief  : Convertit angle en heure minutes secondes decimales
* @param  : STRUCT_ANGLE *angle
* @date   : 2022-03-18 creation 
* @todo   : ras
*****************************************************************************************/

void CALCULS_ANGLE_VERS_DMS(STRUCT_ANGLE *angle) {
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace2("angle decimal (radian)  = %.4f" , angle->ang_dec_rad) ;
  Trace2("angle decimal (degres)  = %.4f" , angle->ang_dec_deg) ;

  if ( angle->ang_dec_rad >=0 ) { 
    angle->ang_si =  1 ;
    angle->ang_sig = '+' ;
  }
  else {
    angle->ang_si = -1 ;
    angle->ang_sig = '-' ;
  }

  angle->ang_dd = (int)fabs(       angle->ang_dec_deg ) ;
  angle->ang_mm = (int)fabs( (fabs(angle->ang_dec_deg) - angle->ang_dd ) * 60.0 ) ;
  angle->ang_ss = (int)fabs(((fabs(angle->ang_dec_deg) - angle->ang_dd ) * 60.0 - angle->ang_mm ) * 60.0 ) ;

  Trace2("minutes  decimale = %d" , angle->ang_mm) ;
  Trace2("heure    decimale = %d" , angle->ang_dd) ;
  Trace2("secondes decimale = %d" , angle->ang_ss) ;
}

/*****************************************************************************************
* @fn     : CALCULS_CONVERSIONS_ANGLES
* @author : s.gravois
* @brief  : Convertit angles en heure  minutes secondes (exemple 23h58m12s)
* @brief  : Convertit angles en degres minutes secondes (exemple 11°58'12")
* @param  : STRUCT_ANGLE *angle
* @date   : 2022-03-18 creation 
* @todo   : ras
*****************************************************************************************/

void CALCULS_CONVERSIONS_ANGLES(void) {
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /* ----------------------------*/
  /* azimut et altitude          */
  /* ----------------------------*/

  (gp_Ast->ast_at).tim_hd = gp_Ast->a * 24.0 / CALCULS_PI_FOIS_DEUX ;
  TIME_CALCULS_DEC_VERS_HMS(&gp_Ast->ast_at) ;

  (gp_Ast->ast_azi_a).ang_dec_rad = gp_Ast->a ;
  (gp_Ast->ast_azi_a).ang_dec_deg = gp_Ast->a * CALCULS_UN_RADIAN_EN_DEGRES ;
  CALCULS_ANGLE_VERS_DMS(&gp_Ast->ast_azi_a) ;
  
  (gp_Ast->ast_ht).tim_hd = gp_Ast->h * 24.0 / CALCULS_PI_FOIS_DEUX ;
  TIME_CALCULS_DEC_VERS_HMS(&gp_Ast->ast_ht) ;
  
  (gp_Ast->ast_alt_a).ang_dec_rad = gp_Ast->h ;
  (gp_Ast->ast_alt_a).ang_dec_deg = gp_Ast->h * CALCULS_UN_RADIAN_EN_DEGRES ;
  CALCULS_ANGLE_VERS_DMS(&gp_Ast->ast_alt_a) ;

  /* -----------------------------*/
  /* angle horaire et declinaison */
  /* ascension droite             */
  /* -----------------------------*/

  (gp_Ast->ast_agh_t).tim_hd = gp_Ast->AGH * 24.0 / CALCULS_PI_FOIS_DEUX ;
  TIME_CALCULS_DEC_VERS_HMS(&gp_Ast->ast_agh_t) ;
  
  (gp_Ast->ast_dec_t).tim_hd = gp_Ast->DEC * 24.0 / CALCULS_PI_FOIS_DEUX ;
  TIME_CALCULS_DEC_VERS_HMS(&gp_Ast->ast_dec_t) ;

  (gp_Ast->ast_asc_t).tim_hd  = gp_Ast->ASC * 24.0 / CALCULS_PI_FOIS_DEUX ;
  TIME_CALCULS_DEC_VERS_HMS(&gp_Ast->ast_asc_t) ;

  /* -------------------------------*/

  (gp_Ast->ast_agh_a).ang_dec_rad = gp_Ast->AGH ;
  (gp_Ast->ast_agh_a).ang_dec_deg = gp_Ast->AGH * CALCULS_UN_RADIAN_EN_DEGRES ;
  CALCULS_ANGLE_VERS_DMS(&gp_Ast->ast_agh_a) ;

  (gp_Ast->ast_asc_a).ang_dec_rad = gp_Ast->ASC ;
  (gp_Ast->ast_asc_a).ang_dec_deg = gp_Ast->ASC * CALCULS_UN_RADIAN_EN_DEGRES ;
  CALCULS_ANGLE_VERS_DMS(&gp_Ast->ast_asc_a) ;

  (gp_Ast->ast_dec_a).ang_dec_rad = gp_Ast->DEC ;
  (gp_Ast->ast_dec_a).ang_dec_deg = gp_Ast->DEC * CALCULS_UN_RADIAN_EN_DEGRES ;
  CALCULS_ANGLE_VERS_DMS(&gp_Ast->ast_dec_a) ;

  /* -------------------------------*/
  /* les calculs intermediaires     */
  /* -------------------------------*/

  (gp_Ast->ast_agh0_t).tim_hd = gp_Ast->AGH0 * 24.0 / CALCULS_PI_FOIS_DEUX ;
  TIME_CALCULS_DEC_VERS_HMS(&gp_Ast->ast_agh0_t) ;

  (gp_Ast->ast_agh1_t).tim_hd = gp_Ast->AGH1 * 24.0 / CALCULS_PI_FOIS_DEUX ;
  TIME_CALCULS_DEC_VERS_HMS(&gp_Ast->ast_agh1_t) ;

  (gp_Ast->ast_agh2_t).tim_hd = gp_Ast->AGH2 * 24.0 / CALCULS_PI_FOIS_DEUX ;
  TIME_CALCULS_DEC_VERS_HMS(&gp_Ast->ast_agh2_t) ;

  (gp_Ast->ast_azi0_t).tim_hd = gp_Ast->AZI0 * 24.0 / CALCULS_PI_FOIS_DEUX ;
  TIME_CALCULS_DEC_VERS_HMS(&gp_Ast->ast_azi0_t) ;

  (gp_Ast->ast_azi1_t).tim_hd = gp_Ast->AZI1 * 24.0 / CALCULS_PI_FOIS_DEUX ;
  TIME_CALCULS_DEC_VERS_HMS(&gp_Ast->ast_azi1_t) ;

  (gp_Ast->ast_azi2_t).tim_hd = gp_Ast->AZI2 * 24.0 / CALCULS_PI_FOIS_DEUX ;
  TIME_CALCULS_DEC_VERS_HMS(&gp_Ast->ast_azi2_t) ;
}

/*****************************************************************************************
* @fn     : CALCULS_ANGLE_HORAIRE
* @author : s.gravois
* @brief  : Convertit heure decimale en heure minutes secondes decimales
* @param  : STRUCT_LIEU  * gp_Lie
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-03-19 correction valeur quand negatif 
* @todo   : ras
*****************************************************************************************/

void CALCULS_ANGLE_HORAIRE(void) {
  
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // en radians 

  /* correction ascension droite si negatif (on ajoute 360 degres) */

  if ( gp_Ast->ASC < 0 ) {
    gp_Ast->ASC += 2*M_PI ;
  }

  /* calcul */ 

  gp_Ast->AGH = gp_Lie->lie_tsr - gp_Ast->ASC ;
  
  /* correction angle horaire si negatif (on ajoute 360 degres) */

  if ( gp_Ast->AGH < 0 ) {
    gp_Ast->AGH += 2*M_PI ;
  }

  CALCULS_CONVERSIONS_ANGLES() ;
  
  Trace1("ascension droite (deg)   = %.2f", gp_Ast->ASC * CALCULS_UN_RADIAN_EN_DEGRES) ;
  Trace1("temps sideral (rad)      = %.2f", gp_Lie->lie_tsr ) ;
  Trace1("angle horaire (deg)      = %.2f", gp_Ast->AGH * CALCULS_UN_RADIAN_EN_DEGRES) ;

  /*
    ASTRE_AFFICHER_MODE_STELLARIUM() ;
  */
}

/*****************************************************************************************
* @fn     : CALCULS_ASCENSION_DROITE
* @author : s.gravois
* @brief  : Convertit heure decimale en heure minutes secondes decimales
* @param  : STRUCT_TIME * gp_Tim
* @date   : 2022-03-19 correction valeur quand negatif 
* @date   : 2022-03-20 verification que angle horaire est positif
* @todo   : ras
*****************************************************************************************/

void CALCULS_ASCENSION_DROITE(void) {
  
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // en radians 
  
  /* correction angle horaire si negatif (on ajoute 360 degres) */

  if ( gp_Ast->AGH < 0 ) {
    gp_Ast->AGH += 2*M_PI ;
  }

  /* calcul */ 

  gp_Ast->ASC = gp_Lie->lie_tsr - gp_Ast->AGH ;
  
  /* correction ascension droite si negatif (on ajoute 360 degres) */

  if ( gp_Ast->ASC < 0 ) {
    gp_Ast->ASC += 2*M_PI ;
  }

  CALCULS_CONVERSIONS_ANGLES() ;
  
  Trace1("ascension droite (deg)   = %.2f", gp_Ast->ASC * CALCULS_UN_RADIAN_EN_DEGRES) ;
  Trace1("temps sideral (rad)      = %.2f", gp_Lie->lie_tsr ) ;
  Trace1("angle horaire (deg)      = %.2f", gp_Ast->AGH * CALCULS_UN_RADIAN_EN_DEGRES) ;

  /*
    ASTRE_AFFICHER_MODE_STELLARIUM() ;
  */
}
/*****************************************************************************************
* @fn     : CALCULS_RECUP_MODE_ET_ASTRE_TYPE()
* @author : s.gravois
* @brief  : Aggregation partie de code dans CALCULS_TOUT ici (recuperation type et nom astre)
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-09-29 creation
*****************************************************************************************/

void CALCULS_RECUP_MODE_ET_ASTRE_TYPE() {

  int i_len_prefixe=3; /* longueur du prefixe exemple NGC1254 */
  int len=0 ;
  char c_sub[32];

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /*---------------- evolution 19/01/2022 ----------------
  * prise en compte du fait que la as peut avoir plusieurs
  * prefixe dans son nom (PLA/NGC/MES/ETO mais aussi EQU/AZI)
  * Cela permet de determine le mode de calculs de l'as
  * et le type de l'as (CIEL_PROFOND etc...)
  -------------------------------------------------------*/

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK(&gp_Ast->ast_mutex) ;
  
  gp_Ast->ast_typ = ASTRE_INDETERMINE ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK(&gp_Ast->ast_mutex) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK(&gp_Cal->cal_mutex) ;

  gp_Cal->cal_mode = CALCULS_AZIMUTAL_VERS_EQUATORIAL ;   

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK(&gp_Cal->cal_mutex) ;

  len = strlen(gp_Ast->nom)-i_len_prefixe ;

  Trace1("nom %s i_len_prefixe %d len %d", gp_Ast->nom, i_len_prefixe, len ) ;
  Trace1("gp_Ast->nom %s CONFIG_MES %s", gp_Ast->nom, CONFIG_NGC ) ;
  Trace1("strstr( gp_Ast->nom, CONFIG_MES ) = %s", strstr( gp_Ast->nom, CONFIG_NGC )) ;

  /* La chaine doit etre superieure en longueur a i_len_prefixe */

  if ( strlen(gp_Ast->nom) > i_len_prefixe ) {

    Trace1("len>i_len_prefixe") ;

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK(&gp_Ast->ast_mutex) ;

    if      ( strstr( gp_Ast->nom, CONFIG_MES ) != NULL ) gp_Ast->ast_typ = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->nom, CONFIG_NGC ) != NULL ) gp_Ast->ast_typ = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->nom, CONFIG_ETO ) != NULL ) gp_Ast->ast_typ = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->nom, CONFIG_PLA ) != NULL ) gp_Ast->ast_typ = ASTRE_PLANETE ;
    else if ( strstr( gp_Ast->nom, CONFIG_AZI ) != NULL ) gp_Ast->ast_typ = ASTRE_INDETERMINE ;
    else if ( strstr( gp_Ast->nom, CONFIG_EQU ) != NULL ) gp_Ast->ast_typ = ASTRE_INDETERMINE ;
    else                                                  gp_Ast->ast_typ = ASTRE_INDETERMINE ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK(&gp_Ast->ast_mutex) ;

    /* TODO : completer avec les types manquants */ 

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK(&gp_Cal->cal_mutex) ;

    if      ( strstr( gp_Ast->nom, CONFIG_MES ) != NULL ) gp_Cal->cal_mode = CALCULS_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_NGC ) != NULL ) gp_Cal->cal_mode = CALCULS_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_ETO ) != NULL ) gp_Cal->cal_mode = CALCULS_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_PLA ) != NULL ) gp_Cal->cal_mode = CALCULS_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_AZI ) != NULL ) gp_Cal->cal_mode = CALCULS_EQUATORIAL_VERS_AZIMUTAL ;
    else if ( strstr( gp_Ast->nom, CONFIG_EQU ) != NULL ) gp_Cal->cal_mode = CALCULS_AZIMUTAL_VERS_EQUATORIAL ;
    else                                                  gp_Cal->cal_mode = CALCULS_AZIMUTAL_VERS_EQUATORIAL ; 

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK(&gp_Cal->cal_mutex) ;

    /* TODO : completer avec les types manquants */ 
  }
  else {
    Trace1("ASTRE_INDETERMINE") ;

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK(&gp_Ast->ast_mutex) ;

    gp_Ast->ast_typ = ASTRE_INDETERMINE ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK(&gp_Ast->ast_mutex) ;
  }
  /* recuperation du numero apres les 3 premiers caracteres */

  memset( c_sub, 0, sizeof(c_sub));
  
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK(&gp_Ast->ast_mutex) ;

  if ( gp_Ast->ast_typ != ASTRE_INDETERMINE ) {

    if( strlen(gp_Ast->nom) > i_len_prefixe ) {

      if ( len > sizeof(c_sub) ) { Trace("erreur : %d >  %d", len,(int)sizeof(c_sub) ) ;}
      if ( len <= 0 )            { Trace("erreur : %d <= 0",   len) ;}

      memcpy( c_sub, &gp_Ast->nom[i_len_prefixe], len );

      c_sub[len] = CONFIG_ZERO_CHAR ;
      gp_Ast->ast_num = atoi(c_sub) ;
    }
    else {
      gp_Ast->ast_num=0 ;
    }
  }
  else {
    gp_Ast->ast_num=0 ;
  }

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK(&gp_Ast->ast_mutex) ;

  Trace("mode %-30s type %-30s", gc_hach_calcul_mode[ gp_Cal->cal_mode ] , gc_hach_astre_types[ gp_Ast->ast_typ ] ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CALCULS_TOUT
* @author : s.gravois
* @brief  : Rafraichit l integralite des calculs
* @param  : STRUCT_ASTRE * gp_Ast
* @param  : STRUCT_LIEU  * gp_Lie
* @param  : STRUCT_SUIVI * gp_Suivi
* @date   : 2022-06-13 correction argument 1 de SOLAR_SYSTEM astre->nom => astre->infos
* @date   : 2022-09-29 deplacement code recup type et nom dans une fct dediee
* @todo   : ras
*****************************************************************************************/

void CALCULS_TOUT(void) {

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  switch (gp_Ast->ast_typ) {

    /* ----------------------------------------------------------------- */
      /* pour un as indetermine 
         on calcule les coordonnees equatoriales 
         necessaires au calcul des vitesses 
      */

    case ASTRE_INDETERMINE :
    
      TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;

      if ( gp_Cal->cal_mode == CALCULS_AZIMUTAL_VERS_EQUATORIAL ) {
        
        CALCULS_EQUATEUR() ;
        CALCULS_ASCENSION_DROITE() ;
      }
      else {
        CALCULS_ANGLE_HORAIRE() ;
        CALCULS_AZIMUT() ;
      }
      CALCULS_VITESSES() ;
      CALCULS_PERIODE() ;
            
      break ;
    /* ----------------------------------------------------------------- */

    case ASTRE_CIEL_PROFOND :
    
      TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;
      CALCULS_ANGLE_HORAIRE() ;
      CALCULS_AZIMUT() ;
      CALCULS_VITESSES() ;
      CALCULS_PERIODE() ;
      
      break ;
    /* ----------------------------------------------------------------- */
    
    case ASTRE_PLANETE :
      
      pthread_mutex_lock( & gp_Mut->mut_cal );
            
      TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;
      
      if ( gp_Ast->ast_num > 9 ) {
        Trace("numero de planete interdit = %d", gp_Ast->ast_num ) ;
        Trace("=> forçage a zero (soleil)"
         ) ;
        gp_Ast->ast_num = 0 ; 
      }

      SOLAR_SYSTEM(     gp_Ast->infos, \
                      & gp_Ast->ASC,  \
                      & gp_Ast->DEC, \
                      & gp_Ast->a, \
                      & gp_Ast->h , \
                        gp_Lie->lie_lat, \
                        gp_Lie->lie_lon, \
                        gp_Lie->lie_alt, \
                        gp_Tim->tim_yy, \
                        gp_Tim->tim_mm, \
                        gp_Tim->tim_dd, \
                        gp_Tim->tim_HH, \
                        gp_Tim->tim_MM, \
                        gp_Tim->tim_SS,\
                        gp_Ast->ast_num ) ;
      
      CALCULS_ANGLE_HORAIRE() ;
      
      // FIXME : pour les planetes , le calcul de l'azimut / altitude 
      // FIXME : ainsi que coordonnees equatoriales / declinaison
      // FIXME : est directement retourner par la fonction SOLAR_SYSTEM
      // TODO  : verifier que le CALCULS_AZIMUT en fonction des coordonnees siderales
      // TODO  : ne modifie pas azimut et altitude calcules par SOLAR_SYSTEM
      
      // CALCULS_AZIMUT  ( gp_Lie, gp_Ast) ;
      
      CALCULS_VITESSES() ;
      CALCULS_PERIODE() ;

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
         (nouveau code 2022) CALCULS_AZIMUTAL_VERS_EQUATORIAL
         ou bien CALCULS_AZIMUTAL_VERS_EQUATORIAL */

      /* 
      case CAPTEURS :

        TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;
        
        // TODO : modifier / completer / corriger ..
        
        if ( gp_Dev->dev_use_capteurs ) { 
          gp_Ast->a = gp_Sui->pitch ;         // FIXME : donne azimut
          gp_Ast->h = gp_Sui->acc_heading ;       // FIXME : donne altitude 
          CALCULS_EQUATEUR ( gp_Lie, gp_Ast) ;  // FIXME : donnes ASC et DEC
        }
            
        CALCULS_ANGLE_HORAIRE( gp_Lie, gp_Ast ) ;
        CALCULS_AZIMUT  ( gp_Lie, gp_Ast) ;
        
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
void CALCULS_VOUTE(void) {
  
  FILE   *fout ;
  // double A, H ;
  double a, h ;

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
/*
  TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;
  CALCULS_EQUATEUR         ( gp_Lie, gp_Ast ) ;
  CALCULS_ASCENSION_DROITE ( gp_Lie, gp_Ast ) ;
  CALCULS_VITESSES         ( gp_Lie, gp_Ast, gp_Sui) ;
  CALCULS_PERIODE          ( gp_Ast, gp_Sui, gp_Vou) ;
*/
  fout=fopen("voute.csv","w") ;
  
  TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;

  // On fait varier les coordonnees azimutales
  
  for(h=-(M_PI/2)+(gp_Lie->lie_lat)+0.001;h<M_PI/2;h+=gp_Vou->vou_pas)
    if (h>=0) 
    for(a=-M_PI +0.001 ;a<M_PI;a+=gp_Vou->vou_pas){
     
     gp_Ast->a=a ;
     gp_Ast->h=h ;
     
     CALCULS_EQUATEUR() ; 
     CALCULS_ASCENSION_DROITE() ;
     CALCULS_VITESSES() ;
     CALCULS_PERIODE() ;

     gp_Ast->V  = sqrt( sqr( gp_Ast->Va ) + sqr( gp_Ast->Vh )) ;
     
     if ( gp_Ast->Va != 0) 
       gp_Ast->An = atan( gp_Ast->Vh / gp_Ast->Va ) ;
     else
       gp_Ast->An = M_PI/2 ;
     
     CALCULS_COORD_R3() ;
     
     Trace1("%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f\n", \
       gp_Ast->a * CALCULS_UN_RADIAN_EN_DEGRES, \
       gp_Ast->h * CALCULS_UN_RADIAN_EN_DEGRES, \
       gp_Ast->AGH * CALCULS_UN_RADIAN_EN_DEGRES, \
       gp_Ast->DEC * CALCULS_UN_RADIAN_EN_DEGRES, \
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
      if ( gp_Ast->ast_new  ) {

        Trace1("%s : a %d h %d A %d H %d : Va %.2f Vh %.2f fre_ta_mic %.2f fre_th_mic %.2f fre_fa_mic=%.2f fre_fh_mic %.2f",\
          gp_Ast->nom,\
          (int)((gp_Ast->a)*CALCULS_UN_RADIAN_EN_DEGRES),\
          (int)((gp_Ast->h)*CALCULS_UN_RADIAN_EN_DEGRES),\
          (int)((gp_Ast->AGH)*CALCULS_UN_RADIAN_EN_DEGRES),\
          (int)((gp_Ast->DEC)*CALCULS_UN_RADIAN_EN_DEGRES),\
          gp_Ast->Va,\
          gp_Ast->Vh,\
          gp_Fre->fre_ta_mic,\
          gp_Fre->fre_th_mic,\
          gp_Fre->fre_fa_mic,\
          gp_Fre->fre_fh_mic ) ;

        Trace1("Va=%2.4f Vh=%2.4f fre_ta_mic=%2.4f fre_th_mic=%2.4f fre_fa_mic=%2.4f fre_fh_mic=%2.4f Fam = %ld Fhm = %ld sta_Tac = %f sta_Thc = %f\n",\
              gp_Ast->Va,\
              gp_Ast->Vh,\
              gp_Fre->fre_ta_mic,\
              gp_Fre->fre_th_mic,\
              gp_Fre->fre_fa_mic,\
              gp_Fre->fre_fh_mic,\
              (gp_Sta->sta_Ia - gp_Sta->sta_Ia_prec),(gp_Sta->sta_Ih - gp_Sta->sta_Ih_prec ),gp_Sta->sta_Tac, gp_Sta->sta_Thc) ;
        
        gp_Ast->ast_new = 0 ;

      }
      */




