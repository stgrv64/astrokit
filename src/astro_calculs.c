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
* @fn     : CALCULS_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_CALCULS *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void CALCULS_DISPLAY_PREPARE ( STRUCT_CALCULS * lp_Cal) {

  TraceArbo(__func__,2,"astre format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Cal->cal_mutex ) ;

  sprintf( lp_Cal->cal_dis_cmd , STR_CAL_FORMAT_0,\
    gc_hach_calcul_mode[ lp_Cal->cal_mode ], \
    gc_hach_calcul_mode[ lp_Cal->cal_type ]) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Cal->cal_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static CALCULS_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_CALCULS *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void CALCULS_DISPLAY(STRUCT_CALCULS *lp_Cal) {

  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,2,"display informations on Calculs") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  CALCULS_DISPLAY_PREPARE( lp_Cal ) ;

  MACRO_ASTRO_GLOBAL_LOG_ON ( lp_Cal->cal_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Cal->cal_loglevel , 1 , "%s", lp_Cal->cal_dis_cmd ) ;
  MACRO_ASTRO_GLOBAL_LOG_OFF( lp_Cal->cal_loglevel ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CALCULS_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_CALCULS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void CALCULS_LOCK ( STRUCT_CALCULS * lp_Cal) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Cal->cal_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CALCULS_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_CALCULS *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void CALCULS_UNLOCK ( STRUCT_CALCULS * lp_Cal) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Cal->cal_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CALCULS_LOCK
* @author : s.gravois
* @brief  : Log la structure
* @param  : STRUCT_CALCULS *
* @date   : 2023-01-11 creation
*****************************************************************************************/

void CALCULS_LOG ( STRUCT_CALCULS * lp_Cal) {

  TraceArbo(__func__,2,"log") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  return ;
}
/*****************************************************************************************
* @fn     : CALCULS_REDUCTION_TOTALE
* @author : s.gravois
* @brief  : permet de calculer unela reduction totale de la vitesse 
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
*****************************************************************************************/

double CALCULS_REDUCTION_TOTALE(void) {

  TraceArbo(__func__,3,"calcul red totale") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

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

void   CALCULS_INIT( STRUCT_CALCULS * lp_Cal ) {

  TraceArbo(__func__,0,"init calculs") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Cal->cal_mutex ) ;
                                     lp_Cal->cal_lock     = CALCULS_LOCK ;
                                     lp_Cal->cal_unlock   = CALCULS_UNLOCK ;
                                     lp_Cal->cal_log      = CALCULS_LOG ;
                                     lp_Cal->cal_display  = CALCULS_DISPLAY ;
                                     lp_Cal->cal_file     = NULL ;
                                     lp_Cal->cal_loglevel = 0 ;
  gettimeofday ( &                   lp_Cal->cal_tval, NULL ) ;

  lp_Cal->cal_type = CALCULS_TYPE_EQU_VERS_AZI ;
  lp_Cal->cal_mode = CALCULS_MODE_AZIMUTAL ;

  return ;
}
/*****************************************************************************************
* @fn     : CALCULS_PARAMS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_CALCULS_PARAMS *
* @param  : void
* @date   : 2022-12-11 creation 
* @todo   : 
*****************************************************************************************/

void CALCULS_PARAMS_INIT(STRUCT_CALCULS_PARAMS *lp_Cal_Par ) {
  
  TraceArbo(__func__,1,"init calculs params") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Cal_Par->cal_par_mutex ) ;

  /* Les deux parametres suivants servent a calculer la reduction
     totale. 
     S ils sont positifs, leur valeur est prise en compte via config.txt
     S ils sont egaus a zero, ils sont affectes par la somme 
     des reductions lues dans config.txt
  */

  lp_Cal_Par->cal_par_alt_red_tot = 0.0 ;
  lp_Cal_Par->cal_par_azi_red_tot = 0.0 ;

  /* Les coefficients de reduction sont egaux a 1 par defaut */ 
  /* car leur presence n est pas obligatoires dans le fichier config.txt */

  lp_Cal_Par->cal_par_alt_red_1          = 1.0 ;  // reduction liee a la monture ()
  lp_Cal_Par->cal_par_alt_red_2          = 1.0;   // reducteur du moteur (nombre de pas)
  lp_Cal_Par->cal_par_alt_red_3          = 1.0;   // reducteur du moteur (gearbox)
  lp_Cal_Par->cal_par_alt_red_4          = 1.0;   // mode micro pas utilisee (1/R4)
  lp_Cal_Par->cal_par_alt_red_5          = 1.0;   // reduction liee a la poulie
  lp_Cal_Par->cal_par_alt_red_6          = 1.0;   // reduction liee au cpu
  lp_Cal_Par->cal_par_alt_red_7          = 1.0;   // reduction non decrite plus haut
  lp_Cal_Par->cal_par_alt_acc            = 1.0 ; 
  lp_Cal_Par->cal_par_alt_f              = 1.0 ; /* frequence du diviseur de frequence eventuel */
  lp_Cal_Par->cal_par_azi_f              = 1.0 ; /* frequence du diviseur de frequence eventuel */

  lp_Cal_Par->cal_par_azi_red_1          = 1.0 ;  // reduction liee a la monture
  lp_Cal_Par->cal_par_azi_red_2          = 1.0;   // reducteur du moteur
  lp_Cal_Par->cal_par_azi_red_3          = 1.0;   // nombre de pas du moteur en azimut
  lp_Cal_Par->cal_par_azi_red_4          = 1.0;   // mode micro pas utilisee (1/R4)
  lp_Cal_Par->cal_par_azi_red_5          = 1.0;   // reduction liee a la poulie
  lp_Cal_Par->cal_par_azi_red_6          = 1.0;   // reduction liee au cpu
  lp_Cal_Par->cal_par_azi_red_7          = 1.0;   // reduction non decrite plus haut
  lp_Cal_Par->cal_par_azi_acc            = 1.0 ; 
  lp_Cal_Par->cal_par_alt_n              = 1.0 ; /* frequence du diviseur de frequence eventuel */
  lp_Cal_Par->cal_par_azi_n              = 1.0 ; /* frequence du diviseur de frequence eventuel */ 
  
  lp_Cal_Par->cal_par_altaz_fast_forward = 1.0 ; 
  lp_Cal_Par->cal_par_altaz_fast_rewind  = 1.0 ; 
  lp_Cal_Par->cal_par_altaz_slow_forward = 1.0 ; 
  lp_Cal_Par->cal_par_altaz_slow_rewind  = 1.0 ; 

  /* TODO : finalize */

  return ; 
}
/*****************************************************************************************
* @fn     : CALCULS_PARAMS_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche la structure STRUCT_CALCULS_PARAMS *
* @param  : void
* @date   : 2022-12-11 creation 
* @todo   : 
*****************************************************************************************/

void CALCULS_PARAMS_DISPLAY(STRUCT_CALCULS_PARAMS *lp_Cal_Par ) {

  TraceArbo(__func__,1,"params display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("gp_Log->log_level = %d", gp_Log->log_level) ;

  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_alt_red_1",          lp_Cal_Par->cal_par_alt_red_1);         
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_alt_red_2",          lp_Cal_Par->cal_par_alt_red_2);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_alt_red_3",          lp_Cal_Par->cal_par_alt_red_3);         
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_alt_red_4",          lp_Cal_Par->cal_par_alt_red_4);         
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_alt_red_5",          lp_Cal_Par->cal_par_alt_red_5);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_alt_red_6",          lp_Cal_Par->cal_par_alt_red_6);         
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_alt_red_7",          lp_Cal_Par->cal_par_alt_red_7);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %d",  "lp_Cal_Par->cal_par_alt_inv  ",          lp_Cal_Par->cal_par_alt_inv  );   
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_alt_acc  ",          lp_Cal_Par->cal_par_alt_acc ) ;
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %ld", "lp_Cal_Par->cal_par_alt_f    ",          lp_Cal_Par->cal_par_alt_f ) ;
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %d",  "lp_Cal_Par->cal_par_alt_n    ",          lp_Cal_Par->cal_par_alt_n ) ;
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_azi_red_1",          lp_Cal_Par->cal_par_azi_red_1);         
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_azi_red_2",          lp_Cal_Par->cal_par_azi_red_2);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_azi_red_3",          lp_Cal_Par->cal_par_azi_red_3);         
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_azi_red_4",          lp_Cal_Par->cal_par_azi_red_4);         
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_azi_red_5",          lp_Cal_Par->cal_par_azi_red_5);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_azi_red_6",          lp_Cal_Par->cal_par_azi_red_6);         
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_azi_red_7",          lp_Cal_Par->cal_par_azi_red_7);
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %d",  "lp_Cal_Par->cal_par_azi_inv  ",          lp_Cal_Par->cal_par_azi_inv  );   
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_azi_acc  ",          lp_Cal_Par->cal_par_azi_acc ) ;
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %ld", "lp_Cal_Par->cal_par_azi_f    ",          lp_Cal_Par->cal_par_azi_f ) ;
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %d",  "lp_Cal_Par->cal_par_azi_n    ",          lp_Cal_Par->cal_par_azi_n ) ;
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_altaz_slow_forward", lp_Cal_Par->cal_par_altaz_slow_forward ) ;  
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_altaz_slow_rewind ", lp_Cal_Par->cal_par_altaz_slow_rewind ) ;   
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_altaz_fast_forward", lp_Cal_Par->cal_par_altaz_fast_forward ) ;  
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %f",  "lp_Cal_Par->cal_par_altaz_fast_rewind ", lp_Cal_Par->cal_par_altaz_fast_rewind ) ;  

  return ;
}
/*****************************************************************************************
* @fn     : CALCULS_INIT_ANGLE
* @author : s.gravois
* @brief  : Initialise la structure calculs STRUCT_ANGLE
* @param  : STRUCT_ANGLE * gp_Cal
* @date   : 2022-12-11 creation
* @todo   : 
*****************************************************************************************/

void   CALCULS_INIT_ANGLE( STRUCT_ANGLE * lp_Ang ) {

  TraceArbo(__func__,0,"init calculs : struct angle") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Ang->ang_mutex ) ;

  lp_Ang->ang_sig     = '+' ; /* signe sous forme charactere '+' ou '-' */
  lp_Ang->ang_tempo   = 0 ;
  lp_Ang->ang_si      = 0 ; /* signe sous forme entiere '-1' ou '+1' */
  lp_Ang->ang_dec_rad = 0 ; 
  lp_Ang->ang_dec_deg = 0 ; 
  lp_Ang->ang_dd      = 0 ;      
  lp_Ang->ang_mm      = 0 ;      
  lp_Ang->ang_ss      = 0 ; 

  return ;
}

/*****************************************************************************************
* @fn     : CALCULS_INIT_PTR
* @author : s.gravois
* @brief  : Initialise la structure calculs STRUCT_CALCULS
* @brief  :  en supposant la voute de rayon 1
* @param  : STRUCT_CALCULS * lp_Cal
* @date   : 2022-11-17 creation entete doxygen
* @date   : 2022-12-17 renmmage CALCULS_INIT_PTR
* @todo   : voir si utile (but : simplification lecture du code)
* @todo   : (non utilisé actuellement , IE Jan 2023)
* @todo   : (obsolete - 2023)
*****************************************************************************************/

void   CALCULS_INIT_PTR( STRUCT_CALCULS * lp_Cal ) {

  TraceArbo(__func__,0,"init pointeurs") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
/*
  lp_Cal->cal_p_Fre = gp_Fre ;
  lp_Cal->cal_p_Sta = gp_Sta ;
  lp_Cal->cal_p_Pas = gp_Pas ;
  lp_Cal->cal_p_Ast = gp_Ast ;
  lp_Cal->cal_p_Vou = gp_Vou ;
  lp_Cal->cal_p_Lie = gp_Lie ;
  lp_Cal->cal_p_Dev = gp_Dev ;
  lp_Cal->cal_p_Mut = gp_Mut ;
  lp_Cal->cal_p_Tim = gp_Tim ;
  lp_Cal->cal_p_Sui = gp_Sui ;
*/
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

void CALCULS_COORD_R3(STRUCT_ASTRE * lp_Ast) {

  TraceArbo(__func__,3,"calculate r3 coords") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // on deduit de l'azimut et de l'altitude
  // les coordonnees x y et z dans la geode d'observation
  
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Ast->ast_mutex ) ;

  gp_Ast->ast_r3_x = cos( gp_Ast->ast_alt ) * cos( gp_Ast->ast_azi )  ;
  gp_Ast->ast_r3_y = cos( gp_Ast->ast_alt ) * sin( gp_Ast->ast_azi ) ;
  gp_Ast->ast_r3_z = sin( gp_Ast->ast_alt ) ;
  
  // On projette la sphere de rayon=1 sur une autre sphere de rayon la valeur qu'on veut
  // ce systeme permet de voir en 3D une valeur en fonction de 3 autres
  // Ici on veut representer la vitesse (norme du vecteur) 
  // en fonction de x y et z (x y z directions du vecteur sur la sphere unite)
  
  gp_Ast->ast_r3_xx = gp_Ast->ast_r3_x * gp_Ast->ast_vit ;
  gp_Ast->ast_r3_yy = gp_Ast->ast_r3_y * gp_Ast->ast_vit ;
  gp_Ast->ast_r3_zz = gp_Ast->ast_r3_z * gp_Ast->ast_vit ;

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

void CALCULS_AZIMUT(STRUCT_ASTRE * lp_Ast) {
  
  // transforme les coordonnees "sid�rales" en coordonnees azimutales
  // permet de trouver l'azimut et l'altitude en fonction de l'angle horaire
  // par rapport au m�ridien (SUD) et la declinaison
  // ATTENTION !! angle horaire = ascension droite sid�rale corrig�e
  // avec le temps sid�ral : une conversion doit etre faite pour trouver
  // gp_Ast->ast_agh  = TS - (asc droite absolue)
  // Une fonction de calcul du temps sid�ral doit etre faite ult�rieurement 
  
  double lat,A,H,a0,h,a1,a2,af ;
  
  TraceArbo(__func__,2,"calculate azimut") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /* recupere information latitude dans une var locale */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Lie->lie_mutex ) ;

  lat= gp_Lie->lie_lat ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Lie->lie_mutex ) ;

  /* recupere information astre dans une var locale */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Ast->ast_mutex ) ;

  A  = gp_Ast->ast_agh ; // FIXME : angle horaire calcule prealablement dans astro.c en theorie
  H  = gp_Ast->ast_dec ; // FIXME : declinaison - seule valeur qui ne change pas 
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Ast->ast_mutex ) ;

  Trace2("agh = %2.3f\tH = %2.3f\t", \
    (gp_Ast->ast_agh)*CALCULS_UN_RADIAN_EN_DEGRES, \
    (gp_Ast->ast_dec)*CALCULS_UN_RADIAN_EN_DEGRES) ;

  /* calculs a partir des variables locales recuperees */

  h  = ALT(lat,A,H) ;
  a0 = AZI0(A,H,h) ;
  a1 = AZI1(lat,A,H,h) ;
  a2 = AZI2(lat,h,H) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Ast->ast_mutex ) ;

  gp_Ast->ast_alt = h ;
  gp_Ast->ast_azi = SGN(a0)*a2 ;

  /* Si azimut negatif on ajoute 360 degres */  
  
  if ( gp_Ast->ast_azi < 0 ) {
    gp_Ast->ast_azi += 2*M_PI ;
  }

   // resultats de calculs : pour tests (a modifier : supprimer)
  
  gp_Ast->ast_azi0 = a0 ;
  gp_Ast->ast_azi1 = a1 ;
  gp_Ast->ast_azi2 = a2 ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Ast->ast_mutex ) ;

  CALCULS_CONVERSIONS_ANGLES(gp_Ast) ;

  Trace2("AZI0 = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_azi0_t.tim_HH, \
    gp_Ast->ast_azi0_t.tim_MM , \
    gp_Ast->ast_azi0_t.tim_SS, \
    gp_Ast->ast_azi0 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace2("AZI1 = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_azi1_t.tim_HH, \
    gp_Ast->ast_azi1_t.tim_MM , \
    gp_Ast->ast_azi1_t.tim_SS, \
    gp_Ast->ast_azi1 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace2("AZI2 = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_azi2_t.tim_HH, \
    gp_Ast->ast_azi2_t.tim_MM , \
    gp_Ast->ast_azi2_t.tim_SS, \
    gp_Ast->ast_azi2 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("azi = %d.%d.%d (hms) %.2f (deg)", \
    gp_Ast->ast_agh_t.tim_HH, \
    gp_Ast->ast_agh_t.tim_MM, \
    gp_Ast->ast_agh_t.tim_SS, \
    gp_Ast->ast_agh * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("alt = %f (deg)" , (gp_Ast->ast_alt)*CALCULS_UN_RADIAN_EN_DEGRES) ;

  return ;
}

/*****************************************************************************************
* @fn     : CALCULS_EQUATEUR
* @author : s.gravois
* @brief  : Transforme les coordonnees azimutales en coordonnees siderales (angle horaire)
* @param  : STRUCT_LIEU  * gp_Lie
* @param  : STRUCT_ASTRE * lp_Ast
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : TODO : verifier a nouveau que les calculs sont corrects (2022)
*****************************************************************************************/

/* FIXME : cette fonction servira quand on devra avoir les coordonnees equatoriales
  en fonction des coordonnnees azimutales , pour le calcul des VITESSES, qui utilise 
  les coordonnees equatoriales 
*/

void CALCULS_EQUATEUR(STRUCT_ASTRE * lp_Ast) {

  // transforme les coordonnees azimutales en coordonnees siderales (angle horaire)
  // permet de trouver les coordonn�es siderales en fonction de l'azimut et de l'altitude
  // Pour avoir la correspondance avec les coordonnn�es �quatoriales brutes, il faut
  // utiliser la conversion AH = TS - ASC
  // AH  : angle horaire
  // TS  : temps sid�ral
  // ASC : ascension droite brute
  
  double A0,A2,A1,a,h,lat,Af,agh,H ;

  TraceArbo(__func__,2,"calculate equatorial coords") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1("avant calcul => a = %2.3f\th = %2.3f\t=>agh = %2.3f\tH=%2.3f",\
    (lp_Ast->ast_azi)   * CALCULS_UN_RADIAN_EN_DEGRES,\
    (lp_Ast->ast_alt)   * CALCULS_UN_RADIAN_EN_DEGRES,\
    (lp_Ast->ast_agh) * CALCULS_UN_RADIAN_EN_DEGRES,\
    (lp_Ast->ast_dec) * CALCULS_UN_RADIAN_EN_DEGRES) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Lie->lie_mutex ) ;

  lat = gp_Lie->lie_lat ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Lie->lie_mutex ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Ast->ast_mutex ) ;

  a = lp_Ast->ast_azi ;
  h = lp_Ast->ast_alt ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Ast->ast_mutex ) ;

  // FIXME : obtention de declinaison et ascension droite (d'apres formules usuelles de Gauss)
  
  H  = DEC(lat,a,h) ;
  A0 = AGH0(a,h,H) ;       /* correct , ajouter 2*M_PI si negatif */
  A1 = AGH1(lat,a,h,H)+M_PI; /* correct , ajouter M_PI */ 
  A2 = AGH2(lat,H,h) ;     /* a anlyser = mauvais */ 
  
  if ( A0 < 0 ) {
    A0 += 2 * M_PI ;
  }
  // lp_Ast->ast_agh = SGN(A0)*A1  ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Ast->ast_mutex ) ;

  lp_Ast->ast_dec  = H ;
  lp_Ast->ast_agh = A0 ;
  lp_Ast->ast_agh0 = A0 ;
  lp_Ast->ast_agh1 = A1 ;
  lp_Ast->ast_agh2 = A2 ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Ast->ast_mutex ) ;

  CALCULS_CONVERSIONS_ANGLES(lp_Ast) ;
  
  Trace1(" %s : ASC = %d.%d.%d (hms) %.2f (deg) %.2f (rad)", \
       lp_Ast->ast_nom , \
       lp_Ast->ast_asc_t.tim_HH, \
       lp_Ast->ast_asc_t.tim_MM, \
       lp_Ast->ast_asc_t.tim_SS, \
       lp_Ast->ast_asc * CALCULS_UN_RADIAN_EN_DEGRES, \
       lp_Ast->ast_asc \
  ) ; 

  Trace1("AH0   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    lp_Ast->ast_agh0_t.tim_HH, \
    lp_Ast->ast_agh0_t.tim_MM , \
    lp_Ast->ast_agh0_t.tim_SS, \
    lp_Ast->ast_agh0 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("AH1   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    lp_Ast->ast_agh1_t.tim_HH, \
    lp_Ast->ast_agh1_t.tim_MM , \
    lp_Ast->ast_agh1_t.tim_SS, \
    lp_Ast->ast_agh1 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("AH2   (deg) = %d.%d.%d (hms) %.2f (deg)", \
    lp_Ast->ast_agh2_t.tim_HH, \
    lp_Ast->ast_agh2_t.tim_MM , \
    lp_Ast->ast_agh2_t.tim_SS, \
    lp_Ast->ast_agh2 * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("AH    (deg) = %d.%d.%d (hms) %.2f (deg)", \
    lp_Ast->ast_agh_t.tim_HH, \
    lp_Ast->ast_agh_t.tim_MM , \
    lp_Ast->ast_agh_t.tim_SS, \
    lp_Ast->ast_agh * CALCULS_UN_RADIAN_EN_DEGRES );

  Trace1("DEC   (deg) = %f" , (lp_Ast->ast_dec)*CALCULS_UN_RADIAN_EN_DEGRES) ;

  Trace1("apres calcul =>a = %2.3f\th = %2.3f\t=>agh = %2.3f\tH=%2.3f",\
   (lp_Ast->ast_azi)*CALCULS_UN_RADIAN_EN_DEGRES,\
   (lp_Ast->ast_alt)*CALCULS_UN_RADIAN_EN_DEGRES,\
   (lp_Ast->ast_agh)*CALCULS_UN_RADIAN_EN_DEGRES,\
   (lp_Ast->ast_dec)*CALCULS_UN_RADIAN_EN_DEGRES) ;

   return ;
} 

/*****************************************************************************************
* @fn     : CALCULS_VITESSES
* @author : s.gravois
* @brief  : MAJ de Va et Vh (vitesses sur les moteurs azimut et altitude) en vit equatoriales
* @param  : STRUCT_LIEU  * lp_Lie
* @param  : STRUCT_ASTRE * lp_Ast
* @param  : STRUCT_SUIVI * gp_Sui
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : 
*****************************************************************************************/

void CALCULS_VITESSES(STRUCT_ASTRE * lp_Ast, STRUCT_LIEU * lp_Lie, STRUCT_SUIVI * lp_Sui) {

  // le calcul des vitesses est fait avec
  // l'angle horaire et la declinaison (azimut et altitude inutiles)
 
  double G, Va, Vh, angle ;
  
  TraceArbo(__func__,3,"calculate speeds") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Cal->cal_mode == CALCULS_MODE_EQUATORIAL ) {

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Ast->ast_mutex ) ;

    lp_Ast->ast_azi_vit = -15.0 ;
    lp_Ast->ast_alt_vit = -15.0 ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Ast->ast_mutex ) ;
  }
  else {
    angle=CALCULS_NB_SECARC_PAR_S_EQU ;
    G  = sqr(sin(lp_Ast->ast_agh))*sqr(cos(lp_Ast->ast_dec))+sqr(sin(lp_Lie->lie_lat)*cos(lp_Ast->ast_dec)*cos(lp_Ast->ast_agh)-cos(lp_Lie->lie_lat)*sin(lp_Ast->ast_dec)) ;
    Va = angle*(cos(lp_Ast->ast_dec)*(cos(lp_Lie->lie_lat)*cos(lp_Ast->ast_agh)*sin(lp_Ast->ast_dec)-sin(lp_Lie->lie_lat)*cos(lp_Ast->ast_dec)))/G ;
    Vh = angle*sin(lp_Ast->ast_agh)*cos(lp_Ast->ast_dec)*cos(lp_Lie->lie_lat)/sqrt(G) ;

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Ast->ast_mutex ) ;

    lp_Ast->ast_azi_vit = Va ;
    lp_Ast->ast_alt_vit = Vh ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Ast->ast_mutex ) ;
  }
  
  Trace2("%2.0f\t%2.8f\t%2.4f\t=>\t%2.8f\t%2.8f\n", \
     (lp_Lie->lie_lat)*CALCULS_UN_RADIAN_EN_DEGRES, \
     (lp_Ast->ast_agh)*CALCULS_UN_RADIAN_EN_DEGRES, \
     (lp_Ast->ast_dec)*CALCULS_UN_RADIAN_EN_DEGRES, \
     Va, \
     Vh ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CALCULS_DIVISEUR_FREQUENCE
* @author : s.gravois
* @brief  : calcule les nombres a injecter dans des diviseurs de frequence 
* @brief  : (frequence de oscillateur a quartz par exemple de reference avant division)
* @param  : STRUCT_SUIVI * gp_Sui
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-10-11 creation entete doxygen 
* @todo   : TODO :  * a supprimer ? inutilise (a servi au debut du projet) 
*****************************************************************************************/

void CALCULS_DIVISEUR_FREQUENCE(void) {
  
  TraceArbo(__func__,3,"calculate divide params of divide controler frequency") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Sui->sui_mutex ) ;

  gp_Sui->sui_Da = gp_Cal_Par->cal_par_azi_f * 2.0 * M_PI / ( gp_Cal_Par->cal_par_azi_red_tot * pow(2.0, gp_Cal_Par->cal_par_azi_n) * ( gp_Ast->ast_azi_vit / ( 60 * 60 * CALCULS_UN_RADIAN_EN_DEGRES ) )) ;
  gp_Sui->sui_Dh = gp_Cal_Par->cal_par_alt_f * 2.0 * M_PI / ( gp_Cal_Par->cal_par_alt_red_tot * pow(2.0, gp_Cal_Par->cal_par_alt_n) * ( gp_Ast->ast_alt_vit / ( 60 * 60 * CALCULS_UN_RADIAN_EN_DEGRES ) )) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Sui->sui_mutex ) ;

  return ;
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

void CALCULS_PERIODE(STRUCT_ASTRE * lp_Ast) {

  double freq_alt_mic, freq_azi_mic ;
  double freq_alt_mot, freq_azi_mot ;
  double freq_alt_bru, freq_azi_bru ;
  double azi_rot,      alt_rot ;
  
  TraceArbo(__func__,2,"calculate periods") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // Calcul de la periode necessaire si pas de terme D ni 2 puissance N
  // ceci dans le cas ou on genere directement la frequence de reference
  // depuis la RASPI, au lieu d'utiliser 2 diviseurs de frequence en serie
  // qui correspondent au diviseur 2^N et diviseur programmable
  // Ou bien ces periodes / frequences servent pour la generation des signaux PWM
  
  // Les flags de inversion *_ROT permettent de ne pas etre dependant du sens de branchement des moteurs !

  if ( gp_Cal_Par->cal_par_azi_inv == 0 )       azi_rot = -1.0 ; else azi_rot = 1.0 ;
  if ( gp_Cal_Par->cal_par_alt_inv == 0 )       alt_rot = -1.0 ; else alt_rot = 1.0 ;

  // La periode de base en mode controleur est directement geree par ce controleur
  // La periode de base en mode PWM est le quart d'une sinusoide
  
  Trace2("%f %f %f %f %f",gp_Pas->pas_acc_azi, gp_Vou->vou_acc, gp_Cal_Par->cal_par_azi_red_tot, gp_Ast->ast_azi_vit, azi_rot);
  Trace2("%f %f %f %f %f",gp_Pas->pas_acc_alt, gp_Vou->vou_acc, gp_Cal_Par->cal_par_alt_red_tot, gp_Ast->ast_alt_vit, alt_rot);

  /*------------------------*/
  /* Calculs des frequences */
  /*------------------------*/

  /* calcul des frequences brutes non corrigees = brutes */
  /* ces frequences ne sont pas corrigees par les accelerations diverses */

  freq_azi_bru = gp_Cal_Par->cal_par_azi_red_tot * gp_Ast->ast_azi_vit * azi_rot / CALCULS_DIVISEUR_SEPCIFIQUE / CALCULS_PI_FOIS_DEUX / 4  ;
  freq_alt_bru = gp_Cal_Par->cal_par_alt_red_tot * gp_Ast->ast_alt_vit * alt_rot / CALCULS_DIVISEUR_SEPCIFIQUE / CALCULS_PI_FOIS_DEUX / 4  ;

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

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Fre->fre_mutex );
      
    gp_Fre->fre_sa_old = gp_Fre->fre_sa ; 
    gp_Fre->fre_sa     = (int)SGN(freq_azi_mic)  ;

    gp_Fre->fre_fa_mic = fabs(freq_azi_mic )  ;
    gp_Fre->fre_fa_bru = fabs(freq_azi_bru )  ;    
    gp_Fre->fre_fa_mot = fabs(freq_azi_mot) ;

    gp_Fre->fre_ta_mic = 1 / gp_Fre->fre_fa_mic ;
    gp_Fre->fre_ta_bru = 1 / gp_Fre->fre_fa_bru ;
    gp_Fre->fre_ta_mot = 1 / gp_Fre->fre_fa_mot ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Fre->fre_mutex );
  

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK(& gp_Fre->fre_mutex );

    gp_Fre->fre_sh_old = gp_Fre->fre_sh ; 
    gp_Fre->fre_sh     = (int)SGN(freq_alt_mic) ;

    gp_Fre->fre_fh_mic = fabs(freq_alt_mic)  ;
    gp_Fre->fre_fh_bru = fabs(freq_alt_bru) ;
    gp_Fre->fre_fh_mot = fabs(freq_alt_mot) ;
    
    gp_Fre->fre_th_mic = 1 / gp_Fre->fre_fh_mic ;
    gp_Fre->fre_th_bru = 1 / gp_Fre->fre_fh_bru ;
    gp_Fre->fre_th_mot = 1 / gp_Fre->fre_fh_mot ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK(& gp_Fre->fre_mutex );

  return ;
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

void CALCULS_PERIODES_SUIVI_MANUEL(STRUCT_ASTRE * lp_Ast, STRUCT_SUIVI_PAS * lp_Pas, STRUCT_SUIVI_FRE * lp_Fre) {

  double frequence ;
  double azi_rot, alt_rot ;
  
  TraceArbo(__func__,3,"calculate periods with manual controls") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // calcul de la periode en fonction des informations de suivi
  // renvoyer par les boutons de la raquette
  // on deduit la frequence du nombre de pas comptes et de la periode ecoulee
  
  if ( lp_Pas->pas_azi !=  lp_Pas->pas_azi_old ) {

    if ( lp_Pas->pas_azi !=0 ) {

      if ( gp_Cal_Par->cal_par_azi_inv == 0 ) azi_rot = -1 ; else azi_rot = 1 ;
    
      //frequence = azi_rot * (double)lp_Pas->pas_azi / t_appui_raq_azi ;
      frequence = azi_rot * (double)lp_Pas->pas_azi ;
  
      gp_Ast->ast_azi_vit = frequence * CALCULS_DIVISEUR_SEPCIFIQUE * CALCULS_PI_FOIS_DEUX / ( gp_Vou->vou_acc * gp_Cal_Par->cal_par_azi_red_tot ) ;

      HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Fre->fre_mutex );

      lp_Fre->fre_sa_old = lp_Fre->fre_sa ; 
      lp_Fre->fre_sa     = (int)SGN(frequence)  ;
      lp_Fre->fre_fa_mic = fabs(frequence) ;
      lp_Fre->fre_ta_mic = 1 / lp_Fre->fre_fa_mic ;
    
      HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Fre->fre_mutex );
    
      Trace1("fre_ta_mic = %2.4f fre_th_mic = %2.4f fre_fa_mic = %2.4f fre_fh_mic = %2.4f\n", \
        lp_Fre->fre_ta_mic, \
        lp_Fre->fre_th_mic, \
        lp_Fre->fre_fa_mic, \
        lp_Fre->fre_fh_mic) ;
    }
  }

  if ( lp_Pas->pas_alt !=  lp_Pas->pas_alt_old ) {

    if ( lp_Pas->pas_alt !=0 ) {

      if ( gp_Cal_Par->cal_par_alt_inv == 0 ) alt_rot = -1 ; else alt_rot = 1 ;
    
      //frequence = alt_rot * (double)lp_Pas->pas_alt / t_appui_raq_alt ;
      frequence = alt_rot * (double)lp_Pas->pas_alt ;
        
      gp_Ast->ast_alt_vit = frequence * CALCULS_DIVISEUR_SEPCIFIQUE * CALCULS_PI_FOIS_DEUX / ( gp_Vou->vou_acc * gp_Cal_Par->cal_par_alt_red_tot ) ;

      HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Fre->fre_mutex );

      lp_Fre->fre_sh_old = lp_Fre->fre_sh ; 
      lp_Fre->fre_sh         = (int)SGN(frequence)  ;
      lp_Fre->fre_fh_mic = fabs(frequence) ;
      lp_Fre->fre_th_mic = 1 / lp_Fre->fre_fh_mic ;
  
      HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Fre->fre_mutex );
    
      Trace1("fre_ta_mic = %2.4f fre_th_mic = %2.4f fre_fa_mic = %2.4f fre_fh_mic = %2.4f\n", \
        lp_Fre->fre_ta_mic, \
        lp_Fre->fre_th_mic, \
        lp_Fre->fre_fa_mic, \
        lp_Fre->fre_fh_mic) ;
    }
  }

  return ;
}

/*****************************************************************************************
* @fn     : CALCULS_ANGLE_VERS_DMS
* @author : s.gravois
* @brief  : Convertit angle en heure minutes secondes decimales
* @param  : STRUCT_ANGLE *lp_Ang
* @date   : 2022-03-18 creation 
* @todo   : ras
*****************************************************************************************/

void CALCULS_ANGLE_VERS_DMS(STRUCT_ANGLE *lp_Ang) {
  
  TraceArbo(__func__,3,"calculate angle to dms") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace2("angle decimal (radian)  = %.4f" , lp_Ang->ang_dec_rad) ;
  Trace2("angle decimal (degres)  = %.4f" , lp_Ang->ang_dec_deg) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ang->ang_mutex ) ;

  if ( lp_Ang->ang_dec_rad >=0 ) { 

    lp_Ang->ang_si =  1 ;
    lp_Ang->ang_sig = '+' ;
  }
  else {
    lp_Ang->ang_si = -1 ;
    lp_Ang->ang_sig = '-' ;
  }

  lp_Ang->ang_dd = (int)fabs(       lp_Ang->ang_dec_deg ) ;
  lp_Ang->ang_mm = (int)fabs( (fabs(lp_Ang->ang_dec_deg) - lp_Ang->ang_dd ) * 60.0 ) ;
  lp_Ang->ang_ss = (int)fabs(((fabs(lp_Ang->ang_dec_deg) - lp_Ang->ang_dd ) * 60.0 - lp_Ang->ang_mm ) * 60.0 ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ang->ang_mutex ) ;

  Trace2("minutes  decimale = %d" , lp_Ang->ang_mm) ;
  Trace2("heure    decimale = %d" , lp_Ang->ang_dd) ;
  Trace2("secondes decimale = %d" , lp_Ang->ang_ss) ;

  return ;
}

/*****************************************************************************************
* @fn     : CALCULS_CONVERSIONS_ANGLES
* @author : s.gravois
* @brief  : Convertit angles en heure  minutes secondes (exemple 23h58m12s)
* @brief  : Convertit angles en degres minutes secondes (exemple 11°58'12")
* @param  : STRUCT_ASTRE *lp_Ast
* @date   : 2022-03-18 creation 
* @todo   : ras
*****************************************************************************************/

void CALCULS_CONVERSIONS_ANGLES(STRUCT_ASTRE * lp_Ast) {
  
  TraceArbo(__func__,3,"calculate astre angles converrsions") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /* ----------------------------*/
  /* azimut et altitude          */
  /* ----------------------------*/

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex) ;

  (lp_Ast->ast_at).tim_hd         = lp_Ast->ast_azi * 24.0 / CALCULS_PI_FOIS_DEUX ;
  (lp_Ast->ast_ht).tim_hd         = lp_Ast->ast_alt * 24.0 / CALCULS_PI_FOIS_DEUX ;

  (lp_Ast->ast_azi_a).ang_dec_rad = lp_Ast->ast_azi ;
  (lp_Ast->ast_azi_a).ang_dec_deg = lp_Ast->ast_azi * CALCULS_UN_RADIAN_EN_DEGRES ;

  (lp_Ast->ast_alt_a).ang_dec_rad = lp_Ast->ast_alt ;
  (lp_Ast->ast_alt_a).ang_dec_deg = lp_Ast->ast_alt * CALCULS_UN_RADIAN_EN_DEGRES ;

  /* -----------------------------*/
  /* angle horaire et declinaison */
  /* ascension droite             */
  /* -----------------------------*/

  (lp_Ast->ast_agh_t).tim_hd = lp_Ast->ast_agh * 24.0 / CALCULS_PI_FOIS_DEUX ;
  (lp_Ast->ast_dec_t).tim_hd = lp_Ast->ast_dec * 24.0 / CALCULS_PI_FOIS_DEUX ;
  (lp_Ast->ast_asc_t).tim_hd  = lp_Ast->ast_asc * 24.0 / CALCULS_PI_FOIS_DEUX ;

  /* -------------------------------*/

  (lp_Ast->ast_agh_a).ang_dec_rad = lp_Ast->ast_agh ;
  (lp_Ast->ast_agh_a).ang_dec_deg = lp_Ast->ast_agh * CALCULS_UN_RADIAN_EN_DEGRES ;
  (lp_Ast->ast_asc_a).ang_dec_rad = lp_Ast->ast_asc ;
  (lp_Ast->ast_asc_a).ang_dec_deg = lp_Ast->ast_asc * CALCULS_UN_RADIAN_EN_DEGRES ;
  (lp_Ast->ast_dec_a).ang_dec_rad = lp_Ast->ast_dec ;
  (lp_Ast->ast_dec_a).ang_dec_deg = lp_Ast->ast_dec * CALCULS_UN_RADIAN_EN_DEGRES ;

  /* -------------------------------*/
  /* les calculs intermediaires     */
  /* -------------------------------*/

  (lp_Ast->ast_agh0_t).tim_hd = lp_Ast->ast_agh0 * 24.0 / CALCULS_PI_FOIS_DEUX ;
  (lp_Ast->ast_agh1_t).tim_hd = lp_Ast->ast_agh1 * 24.0 / CALCULS_PI_FOIS_DEUX ;
  (lp_Ast->ast_agh2_t).tim_hd = lp_Ast->ast_agh2 * 24.0 / CALCULS_PI_FOIS_DEUX ;
  (lp_Ast->ast_azi0_t).tim_hd = lp_Ast->ast_azi0 * 24.0 / CALCULS_PI_FOIS_DEUX ;
  (lp_Ast->ast_azi1_t).tim_hd = lp_Ast->ast_azi1 * 24.0 / CALCULS_PI_FOIS_DEUX ;
  (lp_Ast->ast_azi2_t).tim_hd = lp_Ast->ast_azi2 * 24.0 / CALCULS_PI_FOIS_DEUX ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex) ;

  TIME_CALCULS_DEC_VERS_HMS ( & lp_Ast->ast_at    ) ;
  TIME_CALCULS_DEC_VERS_HMS ( & lp_Ast->ast_ht    ) ;
  TIME_CALCULS_DEC_VERS_HMS ( & lp_Ast->ast_agh_t ) ;
  TIME_CALCULS_DEC_VERS_HMS ( & lp_Ast->ast_dec_t ) ;
  TIME_CALCULS_DEC_VERS_HMS ( & lp_Ast->ast_asc_t ) ;

  CALCULS_ANGLE_VERS_DMS    ( & lp_Ast->ast_azi_a ) ;
  CALCULS_ANGLE_VERS_DMS    ( & lp_Ast->ast_alt_a ) ;
  CALCULS_ANGLE_VERS_DMS    ( & lp_Ast->ast_agh_a ) ;
  CALCULS_ANGLE_VERS_DMS    ( & lp_Ast->ast_asc_a ) ;
  CALCULS_ANGLE_VERS_DMS    ( & lp_Ast->ast_dec_a ) ;

  TIME_CALCULS_DEC_VERS_HMS ( & lp_Ast->ast_agh0_t) ;
  TIME_CALCULS_DEC_VERS_HMS ( & lp_Ast->ast_agh1_t) ;
  TIME_CALCULS_DEC_VERS_HMS ( & lp_Ast->ast_agh2_t) ;
  TIME_CALCULS_DEC_VERS_HMS ( & lp_Ast->ast_azi0_t) ;
  TIME_CALCULS_DEC_VERS_HMS ( & lp_Ast->ast_azi1_t) ;
  TIME_CALCULS_DEC_VERS_HMS ( & lp_Ast->ast_azi2_t) ;

  return ;
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

void CALCULS_ANGLE_HORAIRE(STRUCT_ASTRE * lp_Ast) {
  
  TraceArbo(__func__,2,"calculate horaire angle") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // en radians 

  /* correction ascension droite si negatif (on ajoute 360 degres) */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex );

  if ( lp_Ast->ast_asc < 0 ) {
    lp_Ast->ast_asc += 2*M_PI ;
  }

  /* calcul */ 

  lp_Ast->ast_agh = gp_Lie->lie_tsr - lp_Ast->ast_asc ;
  
  /* correction angle horaire si negatif (on ajoute 360 degres) */

  if ( lp_Ast->ast_agh < 0 ) {
    lp_Ast->ast_agh += 2*M_PI ;
  }

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex );

  CALCULS_CONVERSIONS_ANGLES(lp_Ast) ;
  
  Trace1("ascension droite (deg)   = %.2f", lp_Ast->ast_asc * CALCULS_UN_RADIAN_EN_DEGRES) ;
  /* Trace1("temps sideral (rad)      = %.2f", gp_Lie->lie_tsr ) ; */
  Trace1("angle horaire (deg)      = %.2f", lp_Ast->ast_agh * CALCULS_UN_RADIAN_EN_DEGRES) ;

  /*
    ASTRE_STELLARIUM_VIEW(gp_Ast) ;
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

void CALCULS_ASCENSION_DROITE(STRUCT_ASTRE * lp_Ast) {
  
  TraceArbo(__func__,2,"calculate ascension droite") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex );

  // en radians 
  /* correction angle horaire si negatif (on ajoute 360 degres) */

  if ( lp_Ast->ast_agh < 0 ) {
    lp_Ast->ast_agh += 2*M_PI ;
  }

  /* calcul */ 

  lp_Ast->ast_asc = gp_Lie->lie_tsr - lp_Ast->ast_agh ;
  
  /* correction ascension droite si negatif (on ajoute 360 degres) */

  if ( lp_Ast->ast_asc < 0 ) {
    lp_Ast->ast_asc += 2*M_PI ;
  }

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex );

  CALCULS_CONVERSIONS_ANGLES(lp_Ast) ;
  
  Trace1("ascension droite (deg)   = %.2f", lp_Ast->ast_asc * CALCULS_UN_RADIAN_EN_DEGRES) ;
  /* Trace1("temps sideral (rad)      = %.2f", gp_Lie->lie_tsr ) ; */
  Trace1("angle horaire (deg)      = %.2f", lp_Ast->ast_agh * CALCULS_UN_RADIAN_EN_DEGRES) ;

  /*
    ASTRE_STELLARIUM_VIEW(gp_Ast) ;
  */

  return ;
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

  TraceArbo(__func__,2,"get astre type and calcultation mode") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

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

  gp_Cal->cal_type = CALCULS_TYPE_AZI_VERS_EQU ;   

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK(&gp_Cal->cal_mutex) ;

  len = strlen(gp_Ast->ast_nom)-i_len_prefixe ;

  Trace1("nom %s i_len_prefixe %d len %d", gp_Ast->ast_nom, i_len_prefixe, len ) ;
  Trace1("gp_Ast->ast_nom %s CONFIG_MES %s", gp_Ast->ast_nom, CONFIG_NGC ) ;
  Trace1("strstr( gp_Ast->ast_nom, CONFIG_MES ) = %s", strstr( gp_Ast->ast_nom, CONFIG_NGC )) ;

  /* La chaine doit etre superieure en longueur a i_len_prefixe */

  if ( strlen(gp_Ast->ast_nom) > i_len_prefixe ) {

    Trace1("len>i_len_prefixe") ;

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK(&gp_Ast->ast_mutex) ;

    if      ( strstr( gp_Ast->ast_nom, CONFIG_MES ) != NULL ) gp_Ast->ast_typ = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->ast_nom, CONFIG_NGC ) != NULL ) gp_Ast->ast_typ = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->ast_nom, CONFIG_ETO ) != NULL ) gp_Ast->ast_typ = ASTRE_CIEL_PROFOND ;
    else if ( strstr( gp_Ast->ast_nom, CONFIG_PLA ) != NULL ) gp_Ast->ast_typ = ASTRE_PLANETE ;
    else if ( strstr( gp_Ast->ast_nom, CONFIG_AZI ) != NULL ) gp_Ast->ast_typ = ASTRE_INDETERMINE ;
    else if ( strstr( gp_Ast->ast_nom, CONFIG_EQU ) != NULL ) gp_Ast->ast_typ = ASTRE_INDETERMINE ;
    else                                                  gp_Ast->ast_typ = ASTRE_INDETERMINE ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK(&gp_Ast->ast_mutex) ;

    /* TODO : completer avec les types manquants */ 

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK(&gp_Cal->cal_mutex) ;

    if      ( strstr( gp_Ast->ast_nom, CONFIG_MES ) != NULL ) gp_Cal->cal_type = CALCULS_TYPE_EQU_VERS_AZI ;
    else if ( strstr( gp_Ast->ast_nom, CONFIG_NGC ) != NULL ) gp_Cal->cal_type = CALCULS_TYPE_EQU_VERS_AZI ;
    else if ( strstr( gp_Ast->ast_nom, CONFIG_ETO ) != NULL ) gp_Cal->cal_type = CALCULS_TYPE_EQU_VERS_AZI ;
    else if ( strstr( gp_Ast->ast_nom, CONFIG_PLA ) != NULL ) gp_Cal->cal_type = CALCULS_TYPE_EQU_VERS_AZI ;
    else if ( strstr( gp_Ast->ast_nom, CONFIG_AZI ) != NULL ) gp_Cal->cal_type = CALCULS_TYPE_EQU_VERS_AZI ;
    else if ( strstr( gp_Ast->ast_nom, CONFIG_EQU ) != NULL ) gp_Cal->cal_type = CALCULS_TYPE_AZI_VERS_EQU ;
    else                                                  gp_Cal->cal_type = CALCULS_TYPE_AZI_VERS_EQU ; 

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

    if( strlen(gp_Ast->ast_nom) > i_len_prefixe ) {

      if ( len > sizeof(c_sub) ) { Trace("erreur : %d >  %d", len,(int)sizeof(c_sub) ) ;}
      if ( len <= 0 )            { Trace("erreur : %d <= 0",   len) ;}

      memcpy( c_sub, &gp_Ast->ast_nom[i_len_prefixe], len );

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

  Trace("mode %-30s type %-30s", gc_hach_calcul_type[ gp_Cal->cal_type ] , gc_hach_astre_types[ gp_Ast->ast_typ ] ) ;

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

  TraceArbo(__func__,1,"calculate all") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;

  switch (gp_Ast->ast_typ) {

    /* ----------------------------------------------------------------- */
      /* pour un as indetermine 
         on calcule les coordonnees equatoriales 
         necessaires au calcul des vitesses 
      */

    case ASTRE_INDETERMINE :
    
      if ( gp_Cal->cal_type == CALCULS_TYPE_AZI_VERS_EQU ) {
        
        CALCULS_EQUATEUR(gp_Ast) ;
        CALCULS_ASCENSION_DROITE(gp_Ast) ;
      }
      else {
        CALCULS_ANGLE_HORAIRE(gp_Ast) ;
        CALCULS_AZIMUT(gp_Ast) ;
      }
      CALCULS_VITESSES(gp_Ast,gp_Lie,gp_Sui) ;
      CALCULS_PERIODE(gp_Ast) ;
            
      break ;
    /* ----------------------------------------------------------------- */

    case ASTRE_CIEL_PROFOND :
    
      CALCULS_ANGLE_HORAIRE(gp_Ast) ;
      CALCULS_AZIMUT(gp_Ast) ;
      CALCULS_VITESSES(gp_Ast,gp_Lie,gp_Sui) ;
      CALCULS_PERIODE(gp_Ast) ;
      
      break ;
    /* ----------------------------------------------------------------- */
    
    case ASTRE_PLANETE :

      if ( gp_Ast->ast_num > 9 ) {
        Trace("numero de planete interdit = %d", gp_Ast->ast_num ) ;
        Trace("=> forçage a zero (soleil)"
         ) ;
        gp_Ast->ast_num = 0 ; 
      }

      SOLAR_SYSTEM(     gp_Ast->ast_infos, \
                      & gp_Ast->ast_asc,  \
                      & gp_Ast->ast_dec, \
                      & gp_Ast->ast_azi, \
                      & gp_Ast->ast_alt , \
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
      
      Trace2("(infos) %s (asc) %.2f (dec) %.2f (azi) %.2f (alt) %.2f", \
        gp_Ast->ast_infos, \
        gp_Ast->ast_asc * CALCULS_UN_RADIAN_EN_DEGRES, \
        gp_Ast->ast_dec * CALCULS_UN_RADIAN_EN_DEGRES, \
        gp_Ast->ast_azi * CALCULS_UN_RADIAN_EN_DEGRES, \
        gp_Ast->ast_alt * CALCULS_UN_RADIAN_EN_DEGRES )  ;

      CALCULS_ANGLE_HORAIRE(gp_Ast) ;
      
      // FIXME : pour les planetes , le calcul de l'azimut / altitude 
      // FIXME : ainsi que coordonnees equatoriales / declinaison
      // FIXME : est directement retourner par la fonction SOLAR_SYSTEM
      // TODO  : verifier que le CALCULS_AZIMUT en fonction des coordonnees siderales
      // TODO  : ne modifie pas azimut et altitude calcules par SOLAR_SYSTEM
      
      // CALCULS_AZIMUT  ( gp_Lie, gp_Ast) ;
      
      CALCULS_VITESSES(gp_Ast,gp_Lie,gp_Sui) ;
      CALCULS_PERIODE(gp_Ast) ;

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
         (nouveau code 2022) CALCULS_TYPE_AZI_VERS_EQU
         ou bien CALCULS_TYPE_AZI_VERS_EQU */

      /* 
      case CAPTEURS :

        TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;
        
        // TODO : modifier / completer / corriger ..
        
        if ( gp_Dev->dev_use_capteurs ) { 
          gp_Ast->ast_azi = gp_Sui->pitch ;         // FIXME : donne azimut
          gp_Ast->ast_alt = gp_Sui->acc_heading ;       // FIXME : donne altitude 
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

  TraceArbo(__func__,3,"calculate a 3d voute") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
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
     
     gp_Ast->ast_azi=a ;
     gp_Ast->ast_alt=h ;
     
     CALCULS_EQUATEUR(gp_Ast) ; 
     CALCULS_ASCENSION_DROITE(gp_Ast) ;
     CALCULS_VITESSES(gp_Ast,gp_Lie,gp_Sui) ;
     CALCULS_PERIODE(gp_Ast) ;

     gp_Ast->ast_vit  = sqrt( sqr( gp_Ast->ast_azi_vit ) + sqr( gp_Ast->ast_alt_vit )) ;
     
     if ( gp_Ast->ast_azi_vit != 0) 
       gp_Ast->ast_ah = atan( gp_Ast->ast_alt_vit / gp_Ast->ast_azi_vit ) ;
     else
       gp_Ast->ast_ah = M_PI/2 ;
     
     CALCULS_COORD_R3(gp_Ast) ;
     
     Trace1("%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f\n", \
       gp_Ast->ast_azi * CALCULS_UN_RADIAN_EN_DEGRES, \
       gp_Ast->ast_alt * CALCULS_UN_RADIAN_EN_DEGRES, \
       gp_Ast->ast_agh * CALCULS_UN_RADIAN_EN_DEGRES, \
       gp_Ast->ast_dec * CALCULS_UN_RADIAN_EN_DEGRES, \
       gp_Ast->ast_r3_x , \
       gp_Ast->ast_r3_y, \
       gp_Ast->ast_r3_z, \
       gp_Ast->ast_azi_vit, \
       gp_Ast->ast_alt_vit, \
       gp_Ast->ast_vit, \
       gp_Ast->ast_ah ) ;
     
     /*  	
     printf("%.15f %.15f %.15f %.15f %.15f %.15f\n", \
       gp_Ast->ast_r3_xx , \
       gp_Ast->ast_r3_yy , \
       gp_Ast->ast_r3_zz , \
       gp_Ast->ast_vit ) ;
     
     printf("%.1f %.1f %.1f %.1f %.1f %.1f\n", \
       gp_Ast->ast_r3_xx , \
       gp_Ast->ast_r3_yy , \
       gp_Ast->ast_r3_zz , \
       gp_Ast->ast_azi_vit, \
       gp_Ast->ast_alt_vit, \
       gp_Ast->ast_vit ) ;
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
          gp_Ast->ast_nom,\
          (int)((gp_Ast->ast_azi)*CALCULS_UN_RADIAN_EN_DEGRES),\
          (int)((gp_Ast->ast_alt)*CALCULS_UN_RADIAN_EN_DEGRES),\
          (int)((gp_Ast->ast_agh)*CALCULS_UN_RADIAN_EN_DEGRES),\
          (int)((gp_Ast->ast_dec)*CALCULS_UN_RADIAN_EN_DEGRES),\
          gp_Ast->ast_azi_vit,\
          gp_Ast->ast_alt_vit,\
          gp_Fre->fre_ta_mic,\
          gp_Fre->fre_th_mic,\
          gp_Fre->fre_fa_mic,\
          gp_Fre->fre_fh_mic ) ;

        Trace1("Va=%2.4f Vh=%2.4f fre_ta_mic=%2.4f fre_th_mic=%2.4f fre_fa_mic=%2.4f fre_fh_mic=%2.4f Fam = %ld Fhm = %ld sta_Tac = %f sta_Thc = %f\n",\
              gp_Ast->ast_azi_vit,\
              gp_Ast->ast_alt_vit,\
              gp_Fre->fre_ta_mic,\
              gp_Fre->fre_th_mic,\
              gp_Fre->fre_fa_mic,\
              gp_Fre->fre_fh_mic,\
              (gp_Sta->sta_Ia - gp_Sta->sta_Ia_prec),(gp_Sta->sta_Ih - gp_Sta->sta_Ih_prec ),gp_Sta->sta_Tac, gp_Sta->sta_Thc) ;
        
        gp_Ast->ast_new = 0 ;

      }
      */




