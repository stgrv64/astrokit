/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-10-08  | * creation
# 2022-10-08  | * rapatriement code depuis autres fichiers
# -------------------------------------------------------------- 
*/

#include "astro_time.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

//========================================================================================
// FIXME : TIME_CALCULS_DATE : 
// FIXME : * calcule la date locale servant a tous les calculs ulterieurs
//========================================================================================

int TIME_CALCULS_DATE(STRUCT_TIME * lp_Tim) {
  
  time_t t ;
  
  struct tm *tm ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  lp_Tim->tim_HH = 0 ;
  lp_Tim->tim_MM = 0 ;
  lp_Tim->tim_SS = 0 ;
  
  lp_Tim->tim_yy = 0 ;
  lp_Tim->tim_dd = 0 ;
  lp_Tim->tim_mm = 0 ;
  
  time(&t) ;
  // tzset() ;
  // tm=localtime(&t) ; // pour le temps local (UTC+1 en hiver et UTC+2 en ete)
  tm=gmtime(&t);   // pour UTC
  
  lp_Tim->tim_HH = tm->tm_hour ;
  lp_Tim->tim_MM = tm->tm_min ;
  lp_Tim->tim_SS = tm->tm_sec ;
  
  lp_Tim->tim_yy = 1900 + tm->tm_year ;
  lp_Tim->tim_dd = tm->tm_mday ;
  lp_Tim->tim_mm = tm->tm_mon + 1 ;
  
  TIME_CALCULS_TEMPS_HMS_VERS_DEC (lp_Tim ) ;
  
  //printf("%d %d %d %d %d %d\n",tm->tm_sec,tm->tm_min,tm->tm_hour,tm->tm_mday,tm->tm_mon,tm->tm_year ) ;
  Trace2("%d %d %d %d %d %d %f", \
    lp_Tim->tim_yy, \
    lp_Tim->tim_mm, \
    lp_Tim->tim_dd, \
    lp_Tim->tim_HH, \
    lp_Tim->tim_MM, \
    lp_Tim->tim_SS, \
    lp_Tim->tim_hd ) ;
  
  return 0 ;
}
//========================================================================================
// FIXME : TIME_CALCULS_JOUR_JULIEN
// FIXME : * calcul la date julienne 
// TODO : * valider / comparer avec valeurs de logiciels astronomiques (stellarium etc..)
//========================================================================================

int TIME_CALCULS_JOUR_JULIEN(STRUCT_LIEU* gp_Lie, STRUCT_TIME * lp_Tim) {
  
  int    yy,mm,A,B,C,Y,M,D ;
  double T ;          // fraction de jour a ajouter au jour Julien	
  double JJ ;         // jour julien
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Y  = lp_Tim->tim_yy ;
  M  = lp_Tim->tim_mm ;
  D  = lp_Tim->tim_dd ;
  
  yy = lp_Tim->tim_yy ;
  mm = lp_Tim->tim_mm ;
  
  gp_Lie->lie_jj = 0 ;
  JJ=0 ;
  
  if ( lp_Tim->tim_mm < 3) {
    yy = lp_Tim->tim_yy  - 1 ;
    mm = lp_Tim->tim_mm + 12 ;
  }
  A  = yy ;
  C  = floor(yy / 100  ) ;  
  B  = 2-C+floor(C/4) ;
  
  Trace2("%d %d %d %d %d %d", \
    lp_Tim->tim_yy, \
    lp_Tim->tim_mm, \
    lp_Tim->tim_dd, \
    lp_Tim->tim_HH, \
    lp_Tim->tim_MM, \
    lp_Tim->tim_SS ) ;
  
  T  = (double)lp_Tim->tim_HH / 24.0 + (double)lp_Tim->tim_MM / 1440.0 + (double)lp_Tim->tim_SS / 86400.0 ;
  
  Trace2("A B C T = %d %d %d %8.8f",A,B,C,T) ;
  
  JJ = floor(365.25*(A+4716.0))+floor(30.6001*(mm+1))+(lp_Tim->tim_dd)+T+B-1524.5 ;
  
  Trace2("Date Julienne (Jour Julien corriger avec le temps local) = %7.9f\n",JJ) ;
  
  JJ = 367*Y - 7*(Y + ( M + 9) / 12 ) / 4 + 275 * M / 9 + D + 1721014 - 0.5 + T ;
  
  Trace1("Date Julienne = %7.9f",JJ) ;
  
  gp_Lie->lie_jj = JJ ;
  
  return 0 ;
}
//========================================================================================
// FIXME : TIME_CALCULS_TEMPS_SIDERAL
// FIXME :  * calcul le temps sideral local en fonction du jour julien, de la longitude
// FIXME :  * a besoin de TIME_CALCULS_DATE  et de TIME_CALCULS_JOUR_JULIEN et du STRUCT_LIEU

/* La fonction comprend l appel aux deux fonctions suivantes dans le corps de la fonction : 
  TIME_CALCULS_DATE( lp_Tim ) ;   
  TIME_CALCULS_JOUR_JULIEN  ( gp_Lie, lp_Tim ) ;
*/

// TODO  : valider 1ere methode / 2eme methode et tester sur le ciel ou comparer
// TODO  : avec valeurs de logiciels astronomiques (stellarium etc..)

/* septembre 2022 : correction de deux BUGS passes inapercus car la longitude
   utilisee etait proche de ce celle de GREENWICH ( -0.36 pour escout) => 
   La correction local / greenwich n 'etait pas correct
   bug 1 : 
     dans methode 1 et 2 , le terme gp_Lie->lie_lon est a multiplier par CALCULS_UN_RADIAN_EN_DEGRES
     bug constate quand on passait a un endroit de la Terre eloigne de Greenwich
     (verification sur stellarium)
   bug 2 :
     remplacement de 24h -> 23h56m04s dans les calculs de conversion angles vers H dec
    on doit utiliser le temps sideral et non pas le temps solaire
*/
//========================================================================================

int TIME_CALCULS_TEMPS_SIDERAL(STRUCT_LIEU* gp_Lie, STRUCT_TIME * lp_Tim ) {
  
  double d, T, TSMG, TSMG2, TSH, TSMGH, TSMGH2, TSMH ;
  double TSMGS, TSMH2,TSMH3, GMSTO, GMST, LMST ;
  double jour_sideral_decimal ;
  const double P_2000 = 2451545.5 ; 
  const double P_1900 = 2415020.0 ;

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  TIME_CALCULS_TEMPS_HMS_VERS_DEC_DIRECT(&jour_sideral_decimal , 23 , 56 , 4.09054) ;

  Trace1("jour sideral : %f", jour_sideral_decimal) ;
  
  TIME_CALCULS_DATE( lp_Tim ) ;   
  TIME_CALCULS_JOUR_JULIEN  ( gp_Lie, lp_Tim ) ;
  
  STRUCT_TIME Temps0 ;
  STRUCT_TIME TSMG3 ;
  STRUCT_TIME TSMGH3 ;

  STRUCT_TIME TSR2 ;
  STRUCT_TIME TSR3 ;

  if ( gp_Lie->lie_jj == 0 ) return 1 ;
  
  d = gp_Lie->lie_jj - P_2000 ;
  T = d / 36525.0 ;
  T = ( gp_Lie->lie_jj - P_2000) / 36525.0 ; 
  
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
  TSMG3.tim_hd = TSMG2 * jour_sideral_decimal / 360 ; /* pour ramener en heures */
  
  TIME_CALCULS_TEMPS_DEC_VERS_HMS( & TSMG3 ) ;
  /* *15 = pour ramener en degres */
  TSH       = lp_Tim->tim_hd * 1.00273790935 * 15 ;
  TSMGH     = TSMG + TSH ;
  TSMGH2    = fmod(TSMGH, 360.0) ;
  TSMGH3.tim_hd = TSMGH2 * jour_sideral_decimal / 360 ;

  TIME_CALCULS_TEMPS_DEC_VERS_HMS( & TSMGH3) ;

  TSMH  = TSMGH + ( gp_Lie->lie_lon * CALCULS_UN_RADIAN_EN_DEGRES )  ;
  /*
  Trace("TSMGH  TSMGH2  TSMGH3.tim_hd    : %0.5f   %0.5f   %0.5f", TSMGH, TSMGH2, TSMGH3.tim_hd) ;
  Trace("TSMH = TSMGH - gp_Lie->lie_lon : %0.5f = %0.5f - %0.5f ", TSMH, TSMGH, gp_Lie->lie_lon) ; 
  */
  TSMH2 = fmod(TSMH, 360.0) ;
  TSR2.tim_hd = TSMH2 * jour_sideral_decimal / 360 ;
 
  TIME_CALCULS_TEMPS_DEC_VERS_HMS( & TSR2 ) ;
  
  TIME_AFFICHER_MSG_HHMMSS("1ere methode - Temps sideral Greenwich 0hTU",& TSMG3) ;
  TIME_AFFICHER_MSG_HHMMSS("1ere methode - Temps sideral Greenwich tps local (TU)",& TSMGH3) ;
  TIME_AFFICHER_MSG_HHMMSS("1ere methode - Temps sideral Lieu Local + Heure Locale (TU)",& TSR2) ;
  
  // =======================  Deuxieme methode ==========================
  /* cette methode ne tient pas compte du terme 36525 */
  /* en degres */
  /* calcul du temps sideral moyen de greenwich a 0h TU */
  GMSTO = 100.460618375 + 0.985647366286 * d ;
  GMST  = GMSTO + 1.00273790935 * 15 * lp_Tim->tim_hd  ;
  
  LMST = GMST - ( gp_Lie->lie_lon * CALCULS_UN_RADIAN_EN_DEGRES )  ; // FIXME : attention !! ici la longitude du lien intervient !!
  /*
  Trace("GMST  GMSTO  lp_Tim->tim_hd    : %0.5f   %0.5f   %0.5f", GMST, GMSTO, lp_Tim->tim_hd) ;
  Trace("LMST = GMST  - gp_Lie->lie_lon : %0.5f = %0.5f - %0.5f ", LMST, GMST, gp_Lie->lie_lon) ; 
  */
  TSMG2  = fmod(GMSTO, 360.0) ;
  TSMGH2 = fmod(GMST,  360.0) ;
  TSMH2  = fmod(LMST,  360.0) ;
  
  TSMG3.tim_hd  = TSMG2  * jour_sideral_decimal / 360.0 ;
  TSMGH3.tim_hd = TSMGH2 * jour_sideral_decimal / 360.0 ;
  TSR3.tim_hd  = TSMH2  * jour_sideral_decimal / 360.0 ;
  
  TIME_CALCULS_TEMPS_DEC_VERS_HMS( & TSMG3) ;
  TIME_CALCULS_TEMPS_DEC_VERS_HMS( & TSMGH3 ) ;
  TIME_CALCULS_TEMPS_DEC_VERS_HMS( & TSR3) ;
  
  TIME_AFFICHER_MSG_HHMMSS("2eme methode - Temps sideral Greenwich 0hTU",& TSMG3) ;
  TIME_AFFICHER_MSG_HHMMSS("2eme methode - Temps sideral Greenwich tps local (TU)",& TSMGH3) ;
  TIME_AFFICHER_MSG_HHMMSS("2eme methode - Temps sideral Lieu Local + Heure Locale (TU)",& TSR3) ;
  
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

  gp_Lie->lie_ts  = TSR2.tim_hd ;

  // gp_Lie->lie_tsr = (TSR3.tim_hd / jour_sideral_decimal) * CALCULS_PI_FOIS_DEUX ;
  gp_Lie->lie_tsr = (TSR2.tim_hd / jour_sideral_decimal) * CALCULS_PI_FOIS_DEUX ;

  Trace1("JJ %f gp_Lie->lie_tsr = %f",gp_Lie->lie_jj, gp_Lie->lie_tsr ) ;

  return 0 ;
}

//========================================================================================
// FIXME : TIME_CALCULS_TEMPS_HMS_VERS_DEC_DIRECT : 
// FIXME : * Convertit heure minutes secondes en heure decimale 
/* 2022-09-26 : remplacement du nom HDEC -> TIME_VERS_HDEC */
//========================================================================================

void TIME_CALCULS_TEMPS_HMS_VERS_DEC_DIRECT(double * hdec, double hou, double min, double sec) {
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  *hdec = hou + ( min / 60.0) + ( sec / 3600.0)  ;
}

/*****************************************************************************************
* @fn     : TIME_CALCULS_TEMPS_DEC_VERS_HMS
* @author : s.gravois
* @brief  : Convertit heure decimale en heure minutes secondes decimales
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-03-18 creation 
* @todo   : ras
*****************************************************************************************/

void TIME_CALCULS_TEMPS_DEC_VERS_HMS(STRUCT_TIME * lp_Tim) {
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace2("TEMPS    decimal  = %.4f" , lp_Tim->tim_hd) ;

  if ( lp_Tim->tim_hd >=0 ) { 
    lp_Tim->tim_si   =  1 ;
    lp_Tim->tim_sig = '+' ;
  }
  else {
    lp_Tim->tim_si   = -1 ;
    lp_Tim->tim_sig = '-' ; 
  }
  
  lp_Tim->tim_HH = (int)fabs(  lp_Tim->tim_hd ) ;
  Trace2("heure    decimale = %d" , lp_Tim->tim_HH) ;
  
  lp_Tim->tim_MM = (int)fabs( (fabs(lp_Tim->tim_hd) - lp_Tim->tim_HH ) * 60.0 ) ;
  Trace2("minutes  decimale = %d" , lp_Tim->tim_MM) ;

  lp_Tim->tim_SS = (int)fabs(((fabs(lp_Tim->tim_hd) - lp_Tim->tim_HH ) * 60.0 - lp_Tim->tim_MM ) * 60.0 ) ;
  Trace2("secondes decimale = %d" , lp_Tim->tim_SS) ;


}

//========================================================================================
// FIXME : TIME_TEMPORISATION_MICROSEC : 
// FIXME : * permet de faire une temporisation 
// TODO  : * supprimer ? non utilise actuellement
//========================================================================================

long TIME_TEMPORISATION_MICROSEC(double microsecondes, double percent, struct timeval t00) {
  
  struct timeval t01 ;
  unsigned long  t_diff ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  usleep ( (long)(microsecondes * percent) ) ;
  
  t_diff=0;
  while( t_diff < microsecondes ) {
   gettimeofday(&t01,NULL) ;
   t_diff = (( t01.tv_sec - t00.tv_sec)  * TIME_MICRO_SEC ) + t01.tv_usec - t00.tv_usec;
  }
  return t_diff ;
}

/*****************************************************************************************
* @fn     : TIME_CALCULS_DUREE_SECONDES
* @author : s.gravois
* @brief  : permet de calculer une duree depuis un dernier appel a cette meme fonction
* @brief  : resultat en secondes
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
*****************************************************************************************/

double TIME_CALCULS_DUREE_SECONDES(struct timeval *t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  TraceArbo(__func__,4,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  gettimeofday(&t01,NULL) ;

  t_diff = TIME_MICRO_SEC * ( t01.tv_sec  - t00->tv_sec ) + ( t01.tv_usec - t00->tv_usec ) ;
  t_diff = t_diff / TIME_MICRO_SEC ;

  t00->tv_sec  = t01.tv_sec ; 
  t00->tv_usec = t01.tv_usec ;

  return t_diff  ;
}

/*****************************************************************************************
* @fn     : TIME_CALCULS_DUREE_MICROSEC
* @author : s.gravois
* @brief  : permet de calculer une duree depuis un dernier appel a cette meme fonction
* @brief  : resultat en microsecondes
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
*****************************************************************************************/

double TIME_CALCULS_DUREE_MICROSEC(struct timeval *t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  gettimeofday(&t01,NULL) ;

  t_diff = TIME_MICRO_SEC * ( t01.tv_sec  - t00->tv_sec ) + ( t01.tv_usec - t00->tv_usec ) ;

  t00->tv_sec  = t01.tv_sec ; 
  t00->tv_usec = t01.tv_usec ;

  return t_diff ;
}

/*****************************************************************************************
* @fn     : TIME_CALCULS_DUREE_NANOSEC
* @author : s.gravois
* @brief  : permet de calculer une duree depuis un dernier appel a cette meme fonction
* @brief  : resultat en nanosecondes
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
* @todo   : finir ( modifier pour passer en nanosecondes)
*****************************************************************************************/

double TIME_CALCULS_DUREE_NANOSEC(struct timeval *t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  gettimeofday(&t01,NULL) ;

  t_diff = TIME_MICRO_SEC * ( t01.tv_sec  - t00->tv_sec ) + ( t01.tv_usec - t00->tv_usec ) ;

  t00->tv_sec  = t01.tv_sec ; 
  t00->tv_usec = t01.tv_usec ;

  return t_diff ;
}
/*****************************************************************************************
* @fn     : TIME_AFFICHER
* @author : s.gravois
* @brief  : Cette fonction affiche les informations de temps
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-10-08 rapatriement depuis config.c
* @todo   : ras
*****************************************************************************************/

void TIME_AFFICHER(STRUCT_TIME * lp_Tim) {
  
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace(" DATE / HOUR : %d-%d-%d %-2d:%-2d:%d : %f", \
    lp_Tim->tim_yy, \
    lp_Tim->tim_mm, \
    lp_Tim->tim_dd, \
    lp_Tim->tim_HH, \
    lp_Tim->tim_MM, \
    lp_Tim->tim_SS, \
    lp_Tim->tim_hd ) ;

  Trace1("lp_Tim->tim_yy = %d", lp_Tim->tim_yy ) ;
  Trace1("lp_Tim->tim_mm = %d", lp_Tim->tim_mm ) ;
  Trace1("lp_Tim->tim_dd = %d", lp_Tim->tim_dd ) ;
  Trace1("lp_Tim->tim_HH = %d", lp_Tim->tim_HH ) ;
  Trace1("lp_Tim->tim_MM = %d", lp_Tim->tim_MM ) ;
  Trace1("lp_Tim->tim_SS = %d", lp_Tim->tim_SS ) ;
  Trace1("lp_Tim->tim_hd = %f", lp_Tim->tim_hd ) ;

  Trace("----------------------------") ;

}
/*****************************************************************************************
* @fn     : TIME_CALCULS_TEMPS_HMS_VERS_DEC
* @author : s.gravois
* @brief  : Convertit heure minutes secondes en heure decimale 
* @param  : STRUCT_ANGLE *angle
* @param  : char * mesg
* @date   : 2022-10-11 creation entete doxygen 
* @date   : 2022-09-26 : remplacement du nom HDEC -> TIME_VERS_HDEC 
* @todo   : ras
*****************************************************************************************/

void TIME_CALCULS_TEMPS_HMS_VERS_DEC(STRUCT_TIME * lp_Tim) {
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  lp_Tim->tim_hd = lp_Tim->tim_HH + (lp_Tim->tim_MM / 60.0) + (lp_Tim->tim_SS / 3600.0)  ;
}

/*****************************************************************************************
* @fn     : TIME_AFFICHER_MSG_HHMMSS
* @author : s.gravois
* @brief  : Affiche un message suivi du temps
* @param  : STRUCT_TIME * lp_Tim
* @param  : char * mesg
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : ras
*****************************************************************************************/

void TIME_AFFICHER_MSG_HHMMSS( char * mesg, STRUCT_TIME * lp_Tim ) {

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace2("%s : %dh%dmn%ds : %f", mesg, lp_Tim->tim_HH, lp_Tim->tim_MM, lp_Tim->tim_SS, lp_Tim->tim_hd ) ;
}

/*****************************************************************************************
* @fn     : TIME_SET_YEAR_MONTH_AND_DAY
* @author : s.gravois
* @brief  : Cette fonction configure la date (annee mois jour)
* @param  : char * s_data
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : remplacer fonction system par une fonction C (eviter passage par /bin/bash date)
*****************************************************************************************/

void TIME_SET_YEAR_MONTH_AND_DAY(char * s_data) { // taille des lc_Params = 5 (unite de l'annee en premier)
 
  char buf [   CONFIG_TAILLE_BUFFER_64 ] ;
  char year [  CONFIG_TAILLE_BUFFER_8] ;
  char month [ CONFIG_TAILLE_BUFFER_4] ;
  char day [   CONFIG_TAILLE_BUFFER_4] ;
  int i = 0 ;

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( strlen(s_data) != 8 ){

    Trace("mauvais format : %s", s_data) ;
    Trace("la chaine de caractere doit etre composee de <annee><mois><jour>") ;
    Trace("Exemple : 20190822 = 22 aout 2019") ;
    return ;
  }
  memset( buf,   CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  memset( year,  CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_8 ) ;
  memset( month, CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  memset( day,   CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  
  for(i=0;i<4;i++) year[i]  = s_data[i] ;
  for(i=0;i<2;i++) month[i] = s_data[i+4] ;
  for(i=0;i<2;i++) day[i]   = s_data[i+6] ;

  sprintf(buf, "/bin/date -s %s-%s-%s", year, month, day ) ;
  if ( system( buf ) < 0 ) perror( buf) ;
	Trace("buf = %s", buf) ;

  memset( buf, CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  sprintf(buf, "/bin/echo %s-%s-%s > %s/%s/%s ", year, month, day, gp_Con_Par->par_rep_home, gp_Con_Par->par_rep_cfg, gp_Con_Par->par_fic_dat ) ;
  Trace("buf = %s", buf) ;

  if ( system( buf ) < 0 ) perror( buf) ;

  Trace("Nouvelle date : %s", buf) ;

}
/*****************************************************************************************
* @fn     : TIME_SET_HOUR_AND_MINUTES
* @author : s.gravois
* @brief  : Cette fonction configure l heure (heure et minutes)
* @param  : char * s_data
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : remplacer fonction system par une fonction C (eviter passage par /bin/bash date)
*****************************************************************************************/

void TIME_SET_HOUR_AND_MINUTES(char * s_data) {

  char buf [ CONFIG_TAILLE_BUFFER_64 ]  ;
  char hou [ CONFIG_TAILLE_BUFFER_4] ;
  char min [ CONFIG_TAILLE_BUFFER_4] ;
  int i = 0 ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // FIXME : 20190822 : ajout Traces

  if ( strlen(s_data) != 4 ){

    Trace("mauvais format : %s", s_data) ;
    Trace("la chaine de caractere doit etre composee de <heure><minutes>") ;
    Trace("Exemple : 0804 = 8 heures et 4 minutes") ;
    return ;
  }

  memset( buf, CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  memset( hou, CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  memset( min, CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  
  for(i=0;i<2;i++) hou[i]=s_data[i] ;
  for(i=0;i<2;i++) min[i]=s_data[i+2] ;
  
  sprintf(buf, "/bin/date -s %s:%s",hou, min ) ;
  Trace("buf = %s", buf) ;
  if ( system( buf ) < 0 ) perror( buf) ;

  memset( buf, CALCULS_ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  sprintf(buf, "/bin/echo %s:%s > %s/%s/%s ", hou, min, gp_Con_Par->par_rep_home, gp_Con_Par->par_rep_cfg, gp_Con_Par->par_fic_hhm ) ;
  Trace("buf = %s", buf) ;
  if ( system( buf ) < 0 ) perror( buf) ;
}
/*****************************************************************************************
* @fn     : TIME_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_TIME * lp_Tim
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : configurer directement heure locale (?)
*****************************************************************************************/

void TIME_INIT( STRUCT_TIME * lp_Tim, STRUCT_TIME_TEMPOS * lp_Tpo) {
  
  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  pthread_mutex_init( & lp_Tpo->tpo_mutex, NULL ) ;
  pthread_mutex_init( & lp_Tim->tim_mutex, NULL ) ;

  lp_Tim->tim_sig = '+' ; // signe 
  lp_Tim->tim_mm = 0 ;  // month
  lp_Tim->tim_yy = 0 ;  // year
  lp_Tim->tim_dd = 0 ;  // day
  lp_Tim->tim_HH = 0 ;  // hour
  lp_Tim->tim_MM = 0 ;  // minutes
  lp_Tim->tim_SS = 0 ;  // secondes
  lp_Tim->tim_hd = 0.0 ;  // heure decimale (double)

  lp_Tpo->tpo_menu     = gp_Tim_Par->tim_par_tpo_menu ;
  lp_Tpo->tpo_raq      = gp_Tim_Par->tim_par_tpo_raq ;
  lp_Tpo->tpo_ir       = gp_Tim_Par->tim_par_tpo_ir ;  
  lp_Tpo->tpo_termios  = gp_Tim_Par->tim_par_tpo_termios ;
  lp_Tpo->tpo_capteurs = gp_Tim_Par->tim_par_tpo_capteurs ;
  lp_Tpo->tpo_lcd_loop = gp_Tim_Par->tim_par_tpo_lcd_loop ;
  lp_Tpo->tpo_lcd_disp = gp_Tim_Par->tim_par_tpo_lcd_disp ;
}