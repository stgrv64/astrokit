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

const char * gc_hach_time_Display_Mon[] = {
 "---", 
 "Jan",
 "Feb",
 "Mar",
 "Apr",
 "May",
 "Jun",
 "Jul",
 "Aug",
 "Sep",
 "Oct",
 "Nov",
 "Dec"
} ;

const char * gc_hach_time_Display_Day[] = {
 "--", "01", "02", "03", "04", "05", "06", "07", "08", "09",
 "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
 "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
 "30", "31"
} ;

const char * gc_hach_time_Display_Hou[] = {
 "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
 "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
 "20", "21", "22", "23", "24"
} ;

const char * gc_hach_time_Display_Min[] = {
 "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
 "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
 "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
 "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
 "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
 "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"
} ;

const char * gc_hach_time_Display_Sec[] = {
 "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
 "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
 "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
 "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
 "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
 "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"
} ;

/*****************************************************************************************
* @fn     : TIME_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_TIME *
* @date   : 2023-01-08 creation
* @date   : 2023-03-08 rempalcement STR_TIM_FORMAT_0 par STR_TIM_FORMAT_1
*****************************************************************************************/

static void TIME_DISPLAY_PREPARE ( STRUCT_TIME * lp_Tim) {

  TraceArbo(__func__,2,"astre format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Tim->tim_mutex ) ;

  sprintf( lp_Tim->tim_dis_cmd, STR_TIM_FORMAT_1, \
    lp_Tim->tim_yy , \
    gc_hach_time_Display_Mon [ lp_Tim->tim_mm ] , \
    gc_hach_time_Display_Day [ lp_Tim->tim_dd ] , \
    gc_hach_time_Display_Hou [ lp_Tim->tim_HH ] , \
    gc_hach_time_Display_Sec [ lp_Tim->tim_MM ] ) ;
/*
  sprintf( lp_Tim->tim_dis_cmd , STR_TIM_FORMAT_0,\
    lp_Tim->tim_hd, \
    lp_Tim->tim_sig, \
    lp_Tim->tim_yy, \
    lp_Tim->tim_mm, \
    lp_Tim->tim_dd, \
    lp_Tim->tim_HH, \
    lp_Tim->tim_MM, \
    lp_Tim->tim_SS ) ;
*/
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Tim->tim_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static TIME_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_TIME *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void TIME_DISPLAY( STRUCT_TIME *lp_Tim) {

  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,2,"display informations on Astre") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  TIME_DISPLAY_PREPARE( lp_Tim ) ;

  ASTRO_GLOBAL_LOG_ON ( &  lp_Tim->tim_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Tim->tim_loglevel , 1 , "%s", lp_Tim->tim_dis_cmd ) ;
  ASTRO_GLOBAL_LOG_OFF( &  lp_Tim->tim_loglevel ) ;

  return ;
}
/*****************************************************************************************
* @fn     : TIME_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_TIME *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void TIME_LOCK ( STRUCT_TIME * lp_Tim) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Tim->tim_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : TIME_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_TIME *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void TIME_UNLOCK ( STRUCT_TIME * lp_Tim) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Tim->tim_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : TIME_LOG
* @author : s.gravois
* @brief  : Log la structure en parametre
* @param  : STRUCT_TIME *
* @date   : 2023-01-11 creation
* @todo   : (completer)
*****************************************************************************************/

static void TIME_LOG ( STRUCT_TIME * lp_Tim) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /* TODO : completer */

  return ;
}
/*****************************************************************************************
* @fn     : TIME_CALCULS_LOCAL_DATE
* @author : s.gravois
* @brief  : Calcule la date locale servant a tous les calculs ulterieurs
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-11-30 creation entete format doxygen
* @todo   : est appelle par TIME_CALCULS_SIDERAL_TIME
*****************************************************************************************/

int TIME_CALCULS_LOCAL_DATE(STRUCT_TIME * lp_Tim) {
  
  time_t t ;
  struct tm *tm ;
  STRUCT_TIME l_Tim  ;

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Tim->tim_mutex ) ;

  lp_Tim->tim_HH = 0 ;
  lp_Tim->tim_MM = 0 ;
  lp_Tim->tim_SS = 0 ;
  lp_Tim->tim_yy = 0 ;
  lp_Tim->tim_dd = 0 ;
  lp_Tim->tim_mm = 0 ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Tim->tim_mutex ) ;

  time(&t) ;
  // tzset() ;
  // tm=localtime(&t) ; // pour le temps local (UTC+1 en hiver et UTC+2 en ete)
  tm=gmtime(&t);   // pour UTC
  
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Tim->tim_mutex ) ;

  lp_Tim->tim_HH = tm->tm_hour ;
  lp_Tim->tim_MM = tm->tm_min ;
  lp_Tim->tim_SS = tm->tm_sec ;  
  lp_Tim->tim_yy = 1900 + tm->tm_year ;
  lp_Tim->tim_dd = tm->tm_mday ;
  lp_Tim->tim_mm = tm->tm_mon + 1 ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Tim->tim_mutex ) ;

  TIME_CALCULS_HMS_VERS_DEC (lp_Tim ) ;
  
  //Trace("%d %d %d %d %d %d\n",tm->tm_sec,tm->tm_min,tm->tm_hour,tm->tm_mday,tm->tm_mon,tm->tm_year ) ;
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

/*****************************************************************************************
* @fn     : TIME_CALCULS_JULIAN_DAY
* @author : s.gravois
* @brief  : Calcule le jour julien 
* @param  : STRUCT_LIEU * lp_Lie
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-11-30 creation entete format doxygen
* @todo   : valider / comparer avec valeurs de logiciels astronomiques (stellarium etc..)
*****************************************************************************************/

int TIME_CALCULS_JULIAN_DAY(STRUCT_TIME * lp_Tim, STRUCT_LIEU* lp_Lie) {
  
  int    yy,mm,A,B,C,Y,M,D ;
  double T ;          // fraction de jour a ajouter au jour Julien	
  double JJ ;         // jour julien
  STRUCT_TIME l_Tim  ;

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Tim->tim_mutex ) ;

  memcpy( &l_Tim , lp_Tim, sizeof( STRUCT_TIME) ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Tim->tim_mutex ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Tim->tim_mutex ) ;

  Y  = lp_Tim->tim_yy ;
  M  = lp_Tim->tim_mm ;
  D  = lp_Tim->tim_dd ;
  yy = lp_Tim->tim_yy ;
  mm = lp_Tim->tim_mm ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Tim->tim_mutex ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Lie->lie_mutex ) ;

  lp_Lie->lie_jj = 0 ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Lie->lie_mutex ) ;
  
  JJ=0 ;
  
  if ( lp_Tim->tim_mm < 3) {
    yy = lp_Tim->tim_yy  - 1 ;
    mm = lp_Tim->tim_mm + 12 ;
  }
  A  = yy ;
  C  = floor(yy / 100  ) ;  
  B  = 2-C+floor(C/4) ;
    
  T  = (double)lp_Tim->tim_HH / 24.0 + (double)lp_Tim->tim_MM / 1440.0 + (double)lp_Tim->tim_SS / 86400.0 ;
  
  Trace2("A B C T = %d %d %d %8.8f",A,B,C,T) ;
  
  JJ = floor(365.25*(A+4716.0))+floor(30.6001*(mm+1))+(lp_Tim->tim_dd)+T+B-1524.5 ;
  
  Trace2("Date Julienne (Jour Julien corriger avec le temps local) = %7.9f\n",JJ) ;
  
  JJ = 367*Y - 7*(Y + ( M + 9) / 12 ) / 4 + 275 * M / 9 + D + 1721014 - 0.5 + T ;
  
  Trace1("Date Julienne = %7.9f",JJ) ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Lie->lie_mutex ) ;

  lp_Lie->lie_jj = JJ ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Lie->lie_mutex ) ;

  return 0 ;
}
//========================================================================================
// FIXME : TIME_CALCULS_SIDERAL_TIME
// FIXME :  * calcul le temps sideral local en fonction du jour julien, de la longitude
// FIXME :  * a besoin de TIME_CALCULS_LOCAL_DATE  et de TIME_CALCULS_JULIAN_DAY et du STRUCT_LIEU

/* La fonction comprend l appel aux deux fonctions suivantes dans le corps de la fonction : 
  TIME_CALCULS_LOCAL_DATE( lp_Tim ) ;   
  TIME_CALCULS_JULIAN_DAY  ( lp_Lie, lp_Tim ) ;
*/

// TODO  : valider 1ere methode / 2eme methode et tester sur le ciel ou comparer
// TODO  : avec valeurs de logiciels astronomiques (stellarium etc..)

/* septembre 2022 : correction de deux BUGS passes inapercus car la longitude
   utilisee etait proche de ce celle de GREENWICH ( -0.36 pour escout) => 
   La correction local / greenwich n 'etait pas correct
   bug 1 : 
     dans methode 1 et 2 , le terme lp_Lie->lie_lon est a multiplier par CALCULS_UN_RADIAN_EN_DEGRES
     bug constate quand on passait a un endroit de la Terre eloigne de Greenwich
     (verification sur stellarium)
   bug 2 :
     remplacement de 24h -> 23h56m04s dans les calculs de conversion angles vers H dec
    on doit utiliser le temps sideral et non pas le temps solaire
*/
//========================================================================================

/*****************************************************************************************
* @fn     : TIME_CALCULS_SIDERAL_TIME
* @author : s.gravois
* @brief  : Calcule le temps sideral local 
* @param  : STRUCT_LIEU * lp_Lie
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-09-xx correction de 2 bugs en utilisant TIME_CALCULS_HMS_VERS_DEC_DIR
* @date   : 2022-11-30 creation entete format doxygen
* @todo   : 
*****************************************************************************************/

int TIME_CALCULS_SIDERAL_TIME(STRUCT_TIME * lp_Tim, STRUCT_LIEU* lp_Lie ) {
  
  double d, T, TSMG, TSMG2, TSH, TSMGH, TSMGH2, TSMH ;
  double TSMGS, TSMH2,TSMH3, GMSTO, GMST, LMST ;
  double jour_sideral_decimal ;
  const double P_2000 = 2451545.5 ; 
  const double P_1900 = 2415020.0 ;

  STRUCT_TIME st_TSMG3 ;
  STRUCT_TIME st_TSMGH3 ;
  STRUCT_TIME st_TSR2 ;
  STRUCT_TIME st_TSR3 ;

  TIME_INIT ( & st_TSMG3 ) ;
  TIME_INIT ( & st_TSMGH3 ) ;
  TIME_INIT ( & st_TSR2 ) ;
  TIME_INIT ( & st_TSR3 ) ;

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  TIME_CALCULS_HMS_VERS_DEC_DIR(&jour_sideral_decimal , 23 , 56 , 4.09054) ;

  Trace1("jour sideral : %f", jour_sideral_decimal) ;
  
  TIME_CALCULS_LOCAL_DATE( lp_Tim ) ;   
  TIME_CALCULS_JULIAN_DAY( lp_Tim, lp_Lie ) ;
  
  if ( lp_Lie->lie_jj == 0 ) return 1 ;
  
  d = lp_Lie->lie_jj - P_2000 ;
  T = d / 36525.0 ;
  T = ( lp_Lie->lie_jj - P_2000) / 36525.0 ; 
  
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
  st_TSMG3.tim_hd = TSMG2 * jour_sideral_decimal / 360 ; /* pour ramener en heures */
  
  TIME_CALCULS_DEC_VERS_HMS( & st_TSMG3 ) ;
  /* *15 = pour ramener en degres */
  TSH       = lp_Tim->tim_hd * 1.00273790935 * 15 ;
  TSMGH     = TSMG + TSH ;
  TSMGH2    = fmod(TSMGH, 360.0) ;
  st_TSMGH3.tim_hd = TSMGH2 * jour_sideral_decimal / 360 ;

  TIME_CALCULS_DEC_VERS_HMS( & st_TSMGH3) ;

  TSMH  = TSMGH + ( lp_Lie->lie_lon * CALCULS_UN_RADIAN_EN_DEGRES )  ;
  /*
  Trace("TSMGH  TSMGH2  st_TSMGH3.tim_hd    : %0.5f   %0.5f   %0.5f", TSMGH, TSMGH2, st_TSMGH3.tim_hd) ;
  Trace("TSMH = TSMGH - lp_Lie->lie_lon : %0.5f = %0.5f - %0.5f ", TSMH, TSMGH, lp_Lie->lie_lon) ; 
  */
  TSMH2 = fmod(TSMH, 360.0) ;
  st_TSR2.tim_hd = TSMH2 * jour_sideral_decimal / 360 ;
 
  TIME_CALCULS_DEC_VERS_HMS( & st_TSR2 ) ;
  
  TIME_DISPLAY_MSG_HHMMSS( &st_TSMG3,  "1ere methode - Temps sideral Greenwich 0hTU") ;
  TIME_DISPLAY_MSG_HHMMSS( &st_TSMGH3, "1ere methode - Temps sideral Greenwich tps local (TU)") ;
  TIME_DISPLAY_MSG_HHMMSS( &st_TSR2,   "1ere methode - Temps sideral Lieu Local + Heure Locale (TU)") ;
  
  // =======================  Deuxieme methode ==========================
  /* cette methode ne tient pas compte du terme 36525 */
  /* en degres */
  /* calcul du temps sideral moyen de greenwich a 0h TU */
  GMSTO = 100.460618375 + 0.985647366286 * d ;
  GMST  = GMSTO + 1.00273790935 * 15 * lp_Tim->tim_hd  ;
  
  LMST = GMST - ( lp_Lie->lie_lon * CALCULS_UN_RADIAN_EN_DEGRES )  ; // FIXME : attention !! ici la longitude du lien intervient !!
  /*
  Trace("GMST  GMSTO  lp_Tim->tim_hd    : %0.5f   %0.5f   %0.5f", GMST, GMSTO, lp_Tim->tim_hd) ;
  Trace("LMST = GMST  - lp_Lie->lie_lon : %0.5f = %0.5f - %0.5f ", LMST, GMST, lp_Lie->lie_lon) ; 
  */
  TSMG2  = fmod(GMSTO, 360.0) ;
  TSMGH2 = fmod(GMST,  360.0) ;
  TSMH2  = fmod(LMST,  360.0) ;
  
  st_TSMG3.tim_hd  = TSMG2  * jour_sideral_decimal / 360.0 ;
  st_TSMGH3.tim_hd = TSMGH2 * jour_sideral_decimal / 360.0 ;
  st_TSR3.tim_hd  = TSMH2  * jour_sideral_decimal / 360.0 ;
  
  TIME_CALCULS_DEC_VERS_HMS( & st_TSMG3) ;
  TIME_CALCULS_DEC_VERS_HMS( & st_TSMGH3 ) ;
  TIME_CALCULS_DEC_VERS_HMS( & st_TSR3) ;
  
  TIME_DISPLAY_MSG_HHMMSS( &st_TSMG3,  "2eme methode - Temps sideral Greenwich 0hTU") ;
  TIME_DISPLAY_MSG_HHMMSS( &st_TSMGH3, "2eme methode - Temps sideral Greenwich tps local (TU)") ;
  TIME_DISPLAY_MSG_HHMMSS( &st_TSR3,   "2eme methode - Temps sideral Lieu Local + Heure Locale (TU)") ;
  
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

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Lie->lie_mutex ) ;

  lp_Lie->lie_ts  = st_TSR2.tim_hd ;
  // lp_Lie->lie_tsr = (st_TSR3.tim_hd / jour_sideral_decimal) * CALCULS_PI_FOIS_DEUX ;
  lp_Lie->lie_tsr = (st_TSR2.tim_hd / jour_sideral_decimal) * CALCULS_PI_FOIS_DEUX ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Lie->lie_mutex ) ;

  Trace1("JJ %f lp_Lie->lie_tsr = %f",lp_Lie->lie_jj, lp_Lie->lie_tsr ) ;

  TIME_RELEASE ( & st_TSMG3 ) ;
  TIME_RELEASE ( & st_TSMGH3 ) ;
  TIME_RELEASE ( & st_TSR2 ) ;
  TIME_RELEASE ( & st_TSR3 ) ;

  return 0 ;
}
/*****************************************************************************************
* @fn     : TIME_CALCULS_HMS_VERS_DEC_DIR
* @author : s.gravois
* @brief  : Convertit heure minutes secondes en heure decimale 
* @param  : double * hdec, double hou, double min, double sec
* @date   : 2022-03-18 creation 
* @date   : 2022-09-26 remplacement du nom HDEC -> TIME_VERS_HDEC
* @date   : 2022-11-30 mise a jour entete format doxygen
* @todo   : 
*****************************************************************************************/

void TIME_CALCULS_HMS_VERS_DEC_DIR(double * hdec, double hou, double min, double sec) {
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  *hdec = hou + ( min / 60.0) + ( sec / 3600.0)  ;

  return ;
}
/*****************************************************************************************
* @fn     : TIME_CALCULS_DEC_VERS_HMS
* @author : s.gravois
* @brief  : Convertit heure non decimale en heure minutes secondes decimales
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-03-18 creation 
* @date   : 2023-03-19 trap le cas ou heure decimale a des valeurs farfelues
* @todo   : 
*****************************************************************************************/

void TIME_CALCULS_DEC_VERS_HMS(STRUCT_TIME * lp_Tim) {
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace2("TEMPS non decimal  = %.4f" , lp_Tim->tim_hd) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Tim->tim_mutex ) ;

  if ( lp_Tim->tim_hd >=0 ) { 
    lp_Tim->tim_si   =  1 ;
    lp_Tim->tim_sig = '+' ;
  }
  else {
    lp_Tim->tim_si   = -1 ;
    lp_Tim->tim_sig = '-' ; 
  }
  /* ajout 2023 */

  if ( lp_Tim->tim_hd > 1000 ) {
    lp_Tim->tim_hd = 0 ;
  } else if ( lp_Tim->tim_hd > 24 ) {
    lp_Tim->tim_hd = (int)lp_Tim->tim_hd % 24 ;
  }
  
  lp_Tim->tim_HH = (int)fabs(  lp_Tim->tim_hd ) ;
  lp_Tim->tim_MM = (int)fabs( (fabs(lp_Tim->tim_hd) - lp_Tim->tim_HH ) * 60.0 ) ;
  lp_Tim->tim_SS = (int)fabs(((fabs(lp_Tim->tim_hd) - lp_Tim->tim_HH ) * 60.0 - lp_Tim->tim_MM ) * 60.0 ) ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Tim->tim_mutex ) ;

  Trace2("heure non decimal = %f" , lp_Tim->tim_hd) ;
  Trace2("heure    decimale = %d" , lp_Tim->tim_HH) ;
  Trace2("minutes  decimale = %d" , lp_Tim->tim_MM) ;
  Trace2("secondes decimale = %d" , lp_Tim->tim_SS) ;

  return ;
}
/*****************************************************************************************
* @fn     : TIME_TEMPORISATION_MICROSEC
* @author : s.gravois
* @brief  : Permet de faire une temporisation
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-03-18 creation 
* @date   : 2023-01-11 creation entete
* @todo   : 
*****************************************************************************************/

long TIME_TEMPORISATION_MICROSEC(struct timeval t00, double microsecondes, double percent) {
  
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
* @fn     : TIME_TEMPOS_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations de temps
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-10-08 rapatriement depuis config.c
* @todo   : ras
*****************************************************************************************/

void TIME_TEMPOS_DISPLAY(STRUCT_TIME_TEMPOS * lp_Tim_Tpo) {

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("tpo_ir        = %ld", lp_Tim_Tpo->tpo_ir       ); 
  Trace("tpo_menu      = %ld", lp_Tim_Tpo->tpo_menu     );
  Trace("tpo_voute     = %ld", lp_Tim_Tpo->tpo_voute    ); /* non utilise pour l instant (vou_tempo a la place ))*/
  Trace("tpo_raq       = %ld", lp_Tim_Tpo->tpo_raq      );
  Trace("tpo_termios   = %ld", lp_Tim_Tpo->tpo_termios  ); 
  Trace("tpo_capteurs  = %ld", lp_Tim_Tpo->tpo_capteurs ); 
  Trace("tpo_lcd_loop  = %ld", lp_Tim_Tpo->tpo_lcd_loop ) ;
  Trace("tpo_lcd_disp  = %ld", lp_Tim_Tpo->tpo_lcd_disp ) ;
  Trace("tpo_pid_loop  = %ld", lp_Tim_Tpo->tpo_pid_loop ) ;

  return ;
}

/*****************************************************************************************
* @fn     : TIME_DISPLAY_2
* @author : s.gravois
* @brief  : Cette fonction affiche les informations de temps
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-10-08 rapatriement depuis config.c
* @todo   : ras
*****************************************************************************************/

static void TIME_DISPLAY_2(STRUCT_TIME * lp_Tim) {
  
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

  return ;
}
/*****************************************************************************************
* @fn     : TIME_PARAMS_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les parametres de temps
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-10-08 rapatriement depuis config.c
* @todo   : ras
*****************************************************************************************/

void TIME_PARAMS_DISPLAY(STRUCT_TIME_PARAMS * lp_Tim_Par) {
  
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %ld", "lp_Tim_Par->tim_par_tpo         ", lp_Tim_Par->tim_par_tpo);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %ld", "lp_Tim_Par->tim_par_tpo_raq     ", lp_Tim_Par->tim_par_tpo_raq);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %ld", "lp_Tim_Par->tim_par_tpo_menu    ", lp_Tim_Par->tim_par_tpo_menu);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %ld", "lp_Tim_Par->tim_par_tpo_ir      ", lp_Tim_Par->tim_par_tpo_ir);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %ld", "lp_Tim_Par->tim_par_tpo_termios ", lp_Tim_Par->tim_par_tpo_termios);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %ld", "lp_Tim_Par->tim_par_tpo_capteurs", lp_Tim_Par->tim_par_tpo_capteurs);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %ld", "lp_Tim_Par->tim_par_tpo_lcd_loop", lp_Tim_Par->tim_par_tpo_lcd_loop);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %ld", "lp_Tim_Par->tim_par_tpo_lcd_disp", lp_Tim_Par->tim_par_tpo_lcd_disp);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %ld", "lp_Tim_Par->tim_par_tpo_pid_loop", lp_Tim_Par->tim_par_tpo_pid_loop);

  return ;
}
/*****************************************************************************************
* @fn     : TIME_CALCULS_HMS_VERS_DEC
* @author : s.gravois
* @brief  : Convertit heure minutes secondes en heure decimale 
* @param  : STRUCT_ANGLE *angle
* @param  : char * mesg
* @date   : 2022-10-11 creation entete doxygen 
* @date   : 2022-09-26 : remplacement du nom HDEC -> TIME_VERS_HDEC 
* @todo   : ras
*****************************************************************************************/

void TIME_CALCULS_HMS_VERS_DEC(STRUCT_TIME * lp_Tim) {
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Tim->tim_mutex ) ;

  lp_Tim->tim_hd = lp_Tim->tim_HH + (lp_Tim->tim_MM / 60.0) + (lp_Tim->tim_SS / 3600.0)  ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Tim->tim_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : TIME_DISPLAY_MSG_HHMMSS
* @author : s.gravois
* @brief  : Affiche un message suivi du temps
* @param  : STRUCT_TIME * lp_Tim
* @param  : char * mesg
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : ras
*****************************************************************************************/

void TIME_DISPLAY_MSG_HHMMSS( STRUCT_TIME * lp_Tim, char * mesg ) {

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace2("%s : %dh%dmn%ds : %f", mesg, lp_Tim->tim_HH, lp_Tim->tim_MM, lp_Tim->tim_SS, lp_Tim->tim_hd ) ;

  return ;
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
  memset( buf,   CONFIG_ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  memset( year,  CONFIG_ZERO_CHAR, CONFIG_TAILLE_BUFFER_8 ) ;
  memset( month, CONFIG_ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  memset( day,   CONFIG_ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  
  for(i=0;i<4;i++) year[i]  = s_data[i] ;
  for(i=0;i<2;i++) month[i] = s_data[i+4] ;
  for(i=0;i<2;i++) day[i]   = s_data[i+6] ;

  sprintf(buf, "/bin/date -s %s-%s-%s", year, month, day ) ;
  if ( system( buf ) < 0 ) perror( buf) ;
	Trace("buf = %s", buf) ;

  memset( buf, CONFIG_ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  sprintf(buf, "/bin/echo %s-%s-%s > %s/%s/%s ", \
    year, \
    month, \
    day, \
    gp_Con_Par->con_par_rep_home, \
    gp_Con_Par->con_par_rep_cfg, \
    gp_Con_Par->con_par_fic_dat ) ;

  Trace("buf = %s", buf) ;

  if ( system( buf ) < 0 ) perror( buf) ;

  Trace("Nouvelle date : %s", buf) ;

  return ;
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

  memset( buf, CONFIG_ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;
  memset( hou, CONFIG_ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  memset( min, CONFIG_ZERO_CHAR, CONFIG_TAILLE_BUFFER_4 ) ;
  
  for(i=0;i<2;i++) hou[i]=s_data[i] ;
  for(i=0;i<2;i++) min[i]=s_data[i+2] ;
  
  sprintf(buf, "/bin/date -s %s:%s",hou, min ) ;
  Trace("buf = %s", buf) ;
  if ( system( buf ) < 0 ) perror( buf) ;

  memset( buf, CONFIG_ZERO_CHAR, CONFIG_TAILLE_BUFFER_64 ) ;

  sprintf(buf, "/bin/echo %s:%s > %s/%s/%s ", \
    hou, \
    min, \
    gp_Con_Par->con_par_rep_home, \
    gp_Con_Par->con_par_rep_cfg, \
    gp_Con_Par->con_par_fic_hhm ) ;

  Trace("buf = %s", buf) ;
  if ( system( buf ) < 0 ) perror( buf) ;

  return ;
}

/*

void   TIME_INIT                      ( STRUCT_TIME * ) ;
void   TIME_INIT_TPO                  ( STRUCT_TIME_TEMPOS * ) ;
void   TIME_PARAMS_INIT               ( STRUCT_TIME_PARAMS * ) ;

*/

/*****************************************************************************************
* @fn     : TIME_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_TIME * 
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-12- 
* @todo   : configurer directement heure locale (?)
*****************************************************************************************/

void TIME_INIT( STRUCT_TIME * lp_Tim) {
  
  TraceArbo(__func__,2,"init time") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Tim->tim_mutex ) ;
                                     lp_Tim->tim_log      = TIME_LOG ;
                                     lp_Tim->tim_lock     = TIME_LOCK ;
                                     lp_Tim->tim_unlock   = TIME_UNLOCK ;
                                     lp_Tim->tim_display  = TIME_DISPLAY ;
                                     lp_Tim->tim_loglevel = 0 ; 
                                     lp_Tim->tim_file     = NULL ;
  gettimeofday (                   & lp_Tim->tim_tval, NULL ) ;  

  lp_Tim->tim_sig = '+' ; // signe 
  lp_Tim->tim_mm  = 0 ;  // month
  lp_Tim->tim_yy  = 0 ;  // year
  lp_Tim->tim_dd  = 0 ;  // day
  lp_Tim->tim_HH  = 0 ;  // hour
  lp_Tim->tim_MM  = 0 ;  // minutes
  lp_Tim->tim_SS  = 0 ;  // secondes
  lp_Tim->tim_hd  = 0 ;  // heure decimale (double)

  return ;
}
/*****************************************************************************************
* @fn     : TIME_RELEASE
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_TIME * 
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-12-11 creation
* @todo   : 
*****************************************************************************************/

void TIME_RELEASE( STRUCT_TIME * lp_Tim) {
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_DESTROY( & lp_Tim->tim_mutex ) ;
}
/*****************************************************************************************
* @fn     : TIME_PARAMS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_TIME * 
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-12-11 creation
* @todo   : 
*****************************************************************************************/

void TIME_PARAMS_INIT( STRUCT_TIME_PARAMS * lp_Tim_Par) {
  
  TraceArbo(__func__,0,"init time params") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Tim_Par->tim_par_mutex ) ;

  lp_Tim_Par->tim_par_tpo_menu     = 0 ;
  lp_Tim_Par->tim_par_tpo_raq      = 0 ;
  lp_Tim_Par->tim_par_tpo_ir       = 0 ;
  lp_Tim_Par->tim_par_tpo_termios  = 0 ;
  lp_Tim_Par->tim_par_tpo_capteurs = 0 ;
  lp_Tim_Par->tim_par_tpo_lcd_loop = 0 ;
  lp_Tim_Par->tim_par_tpo_lcd_disp = 0 ;
  lp_Tim_Par->tim_par_tpo_pid_loop = 0 ;

  return ;
}

/*****************************************************************************************
* @fn     : TIME_INIT_TEMPOS
* @author : s.gravois
* @brief  : Cette fonction initialise la structure TIME_INIT_TEMPOS *
* @brief  : !! suppose  que TIME_PARAMS_INIT deja traitee & les parametres sont lues !!
* @param  : STRUCT_TIME * lp_Tpo
* @date   : 2022-12-11
* @todo   : 
*****************************************************************************************/

void TIME_INIT_TEMPOS( STRUCT_TIME_TEMPOS * lp_Tpo) {
  
  TraceArbo(__func__,0,"init time tempos") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Tpo->tpo_mutex ) ;

  /* Suppose que la lecture du fichier de config a ete faite */
  /* cf fonction CONFIG_PARAMETRES_CONFIG */

  lp_Tpo->tpo_menu     = 0 ;
  lp_Tpo->tpo_raq      = 0 ;
  lp_Tpo->tpo_ir       = 0 ;  
  lp_Tpo->tpo_termios  = 0 ;
  lp_Tpo->tpo_capteurs = 0 ;
  lp_Tpo->tpo_voute    = 0 ;
  lp_Tpo->tpo_lcd_disp = 0 ;
  lp_Tpo->tpo_lcd_loop = 0 ;
  lp_Tpo->tpo_pid_loop = 0 ;

  return ;
}

/*****************************************************************************************
* @fn     : TIME_TEMPOS_CONFIG
* @author : s.gravois
* @brief  : Cette fonction configure la structure STRUCT_TIME_TEMPOS en fct des parametres 
* @param  : STRUCT_TIME * lp_Tim
* @date   : 2022-12-11 creation
* @todo   : 
*****************************************************************************************/

void TIME_TEMPOS_CONFIG( STRUCT_TIME_TEMPOS * lp_Tpo) {
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /* Donne des valeurs par defaut avant de se baser sur la lecture */

  lp_Tpo->tpo_menu     = TIME_DEFAULT_TEMPO_NEMU ;
  lp_Tpo->tpo_raq      = TIME_DEFAULT_TEMPO_RAQ ;
  lp_Tpo->tpo_ir       = TIME_DEFAULT_TEMPO_IR ;  
  lp_Tpo->tpo_termios  = TIME_DEFAULT_TEMPO_TERMIOS ;
  lp_Tpo->tpo_capteurs = TIME_DEFAULT_TEMPO_CAPTEURS ;
  lp_Tpo->tpo_lcd_loop = TIME_DEFAULT_TEMPO_LCD_LOOP ;
  lp_Tpo->tpo_lcd_disp = TIME_DEFAULT_TEMPO_LCD_DISP ;
  lp_Tpo->tpo_voute    = TIME_DEFAULT_TEMPO_VOUTE ;

  /* Si la valeur de TEMPO est > 0 , on utlilse juste TEMPO et les coefficients multiplicatifs 
     pour ajuster les temporisations du programme */

  if ( gp_Tim_Par->tim_par_tpo > 0 )  {

    lp_Tpo->tpo_voute    = TIME_DEFAULT_TEMPO_VOUTE ;
    lp_Tpo->tpo_menu     = gp_Tim_Par->tim_par_tpo * TIME_DEFAULT_TEMPO_COEFF_MULT_MENU ;
    lp_Tpo->tpo_raq      = gp_Tim_Par->tim_par_tpo * TIME_DEFAULT_TEMPO_COEFF_MULT_RAQ ; /* est utilisee uniquement dans SUIVI_MANUEL_ASSERVI */
    lp_Tpo->tpo_ir       = gp_Tim_Par->tim_par_tpo * TIME_DEFAULT_TEMPO_COEFF_MULT_IR ;
    lp_Tpo->tpo_termios  = gp_Tim_Par->tim_par_tpo * TIME_DEFAULT_TEMPO_COEFF_MULT_TERMIOS ;
    lp_Tpo->tpo_capteurs = gp_Tim_Par->tim_par_tpo * TIME_DEFAULT_TEMPO_COEFF_MULT_CAPTEURS ;
    lp_Tpo->tpo_lcd_loop = gp_Tim_Par->tim_par_tpo * TIME_DEFAULT_TEMPO_COEFF_MULT_LCD_LOOP ; 
    lp_Tpo->tpo_lcd_disp = gp_Tim_Par->tim_par_tpo * TIME_DEFAULT_TEMPO_COEFF_MULT_LCD_DISP ; 
    lp_Tpo->tpo_pid_loop = gp_Tim_Par->tim_par_tpo * TIME_DEFAULT_TEMPO_COEFF_MULT_PID_LOOP ;
  }
  else {
    lp_Tpo->tpo_voute    = TIME_DEFAULT_TEMPO_VOUTE ;
    lp_Tpo->tpo_menu     = gp_Tim_Par->tim_par_tpo_menu     ;
    lp_Tpo->tpo_raq      = gp_Tim_Par->tim_par_tpo_raq      ; /* est utilisee uniquement dans SUIVI_MANUEL_ASSERVI */
    lp_Tpo->tpo_ir       = gp_Tim_Par->tim_par_tpo_ir       ;
    lp_Tpo->tpo_termios  = gp_Tim_Par->tim_par_tpo_termios  ;
    lp_Tpo->tpo_capteurs = gp_Tim_Par->tim_par_tpo_capteurs ;
    lp_Tpo->tpo_lcd_loop = gp_Tim_Par->tim_par_tpo_lcd_loop ; 
    lp_Tpo->tpo_lcd_disp = gp_Tim_Par->tim_par_tpo_lcd_disp ; 
    lp_Tpo->tpo_pid_loop = gp_Tim_Par->tim_par_tpo_pid_loop ; 
  }

  return ;
}

/* =====================================================================================
 *
 * FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - 
 * 
 * ===================================================================================== */
