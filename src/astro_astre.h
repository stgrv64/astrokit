/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-10-07  | * creation
# 2022-10-07  | * rapatriement code depuis partout
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_ASTRE_H
#define ASTRO_ASTRE_H

#include "astro_global.h"

#define  ASTRE_TAILLE_BUFFER  256
#define  ASTRE_NB_COLONNES    10000

static const char * gc_hach_astre_types[] = {
  "ASTRE_INDETERMINE",
  "ASTRE_CIEL_PROFOND",
  "ASTRE_PLANETE",
  "ASTRE_COMETE",
  "ASTRE_ASTEROIDE",
  "ASTRE_SATELLITE"
} ;

typedef enum {

  ASTRE_INDETERMINE=0,
  ASTRE_CIEL_PROFOND,
  ASTRE_PLANETE,
  ASTRE_COMETE,
  ASTRE_ASTEROIDE,
  ASTRE_SATELLITE
}
t_en_Astre_Type ;

typedef enum t_en_Astre_Type ENUM_ASTRE_TYPE ;

/*---------------------------------------------------*/
/* Parametres de fichier config (modif octobre 2022) */ 
/*---------------------------------------------------*/

struct STR_ASTRE_PARAMS {
  pthread_mutex_t ast_par_mutex ;
  char ast_par_default_object[ CONFIG_TAILLE_BUFFER_256 ] ;
} ;

typedef struct STR_ASTRE_PARAMS STRUCT_ASTRE_PARAMS ;

/*---------------------------------------------------*/
/* Fin parametres de fichier config                  */ 
/*---------------------------------------------------*/

struct STR_ASTRE {

  pthread_mutex_t ast_mutex ;

  STRUCT_TIME  ast_at ;
  STRUCT_TIME  ast_ht ;
  STRUCT_TIME  ast_azi_t ;
  STRUCT_TIME  ast_alt_t ;
  STRUCT_TIME  ast_dec_t ;
  STRUCT_TIME  ast_asc_t ;   
  STRUCT_TIME  ast_agh_t ;
  STRUCT_TIME  ast_agh0_t ;  
  STRUCT_TIME  ast_agh1_t ; 
  STRUCT_TIME  ast_agh2_t ;  
  STRUCT_TIME  ast_azi0_t ;
  STRUCT_TIME  ast_azi1_t ;
  STRUCT_TIME  ast_azi2_t ;
  STRUCT_ANGLE ast_azi_a ; 
  STRUCT_ANGLE ast_alt_a ;
  STRUCT_ANGLE ast_dec_a ; 
  STRUCT_ANGLE ast_asc_a ;
  STRUCT_ANGLE ast_agh_a ;
  
  /* Numero de l objet dans un catalogue */
  
  int    ast_num ; 
  int    ast_new ; /* ajout 2022-11 */
  /* les 2 structures sont placees ici en attendant une structure dediee */
  int    ast_typ ;

  char   nom         [ ASTRE_TAILLE_BUFFER ] ;
  char   infos       [ ASTRE_TAILLE_BUFFER ] ;  
  char   plus_proche [ ASTRE_NB_COLONNES   ][ ASTRE_TAILLE_BUFFER ] ;
  
  /* --------------------------------------------
  *  on deduit de l'azimut(h) et de l'altitude (a)
  *  les coordonnees x y et z dans la geode d'observation de rayon 1
  --------------------------------------------- */

  double x ;    // cos(h)cos(a)  = abscisse du point sur la sphere de rayon UN (voute celeste) 
  double y ;    // cos(h)sin(a)  = ordonnee du point sur la sphere de rayon UN (voute celeste) 
  double z ;    // sin(h)        = z        du point sur la sphere de rayon UN (voute celeste) 

  /* --------------------------------------------
  *  on deduit de l'azimut et de l'altitude
  *  les coordonnees xx yy et zz dans la geode d'observation
  * de rayon sqrt(xx²+yy²+zz²) 
  * => permet de representer la norme d'un vecteur par rapport a (a,h)<=>(x,y,z)
  --------------------------------------------- */

  double xx ;   // donneee permettant de representer une valeur par OM * val (sphere de unite UN multipliee par la valeur) _ abscisse
  double yy ;   // idem - ordonnee
  double zz ; 
  
  double a ;    // azimut
  double h ;    // altitude

  double a0 ;   // valeur precedente de l'azimut
  double h0 ;   // valeur precedente de l'altitude
  
  double AGH ;    // angle horaire  ( = gp_Lie->tps_mic sideral - ASC)
  double H    ;    // declinaison
  
  double DEC  ;  // un resultat de calcul de declinaison
  double ASC  ;   // un resultat de calcul de asc

  double AGH0 ;  // un autre resultat de calcul de asc
  double AGH1 ;  // un autre resultat de calcul de asc
  double AGH2 ;  // un autre resultat de calcul de asc

  double ALT ;   // un resultat de calcul de ALT

  double AZI0 ;   // un resultat de calcul de AZI
  double AZI1 ;  // un autre resultat de calcul de AZI
  double AZI2 ;  // un autre resultat de calcul de AZI

  double A0 ;   // valeur precedente de l'angle horaire
  double H0 ;   // valeur precedente de la declinaison
  
  double V  ;   // norme du vecteur vitesse
  double An  ;  // angle du vecteur vitesse par rapport a horizontal droite
  double Va ;   // vitesse de l'azimut   en rad par seconde calculee par une methode
  double Vh ;   // vitesse de l'altitude en rad par seconde calculee par une methode
  
  // ceci sert pour les tests 
  // a modifier et effacer en fin de developpement
  
  double da ;   // differentiel azimut           en rad par seconde (a - a0 )
  double dh ;   // differentiel altitude         en rad par seconde (h - h0 )
  double dA ;   // differentiel ascension droite en rad par seconde (A - A0 )
  double dH ;   // differentiel declinaison      en rad par seconde (H - H0 )
  double dVa ;  // differentiel de la vitesse en azimut utile pour calcul    (acceleration en azimut)
  double dVh ;  // differentiel de la vitesse en altitude utilde pour calcul (acceleratnio en altitude)
  double dVam ; // maximum du differentiel pour tests et calcul
  double dVhm ; // maximum du differentiel pour tests et calcul

  /* formatage des informations de coordonnnes sur l astre */

  char  c_hhmmss_agh[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_hhmmss_asc[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_hhmmss_azi[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_hhmmss_alt[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_hhmmss_dec[ CONFIG_TAILLE_BUFFER_16] ;

  char  c_hhmm_agh[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_hhmm_asc[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_hhmm_azi[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_hhmm_alt[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_hhmm_dec[ CONFIG_TAILLE_BUFFER_16] ;

  char  c_ddmm_agh[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_ddmm_asc[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_ddmm_azi[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_ddmm_alt[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_ddmm_dec[ CONFIG_TAILLE_BUFFER_16] ;

  char  c_dd_agh[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_dd_asc[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_dd_azi[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_dd_alt[ CONFIG_TAILLE_BUFFER_16] ;
  char  c_dd_dec[ CONFIG_TAILLE_BUFFER_16] ;

} ;
typedef struct STR_ASTRE STRUCT_ASTRE ;

/* Le contenu de cette variable permet le hachage avec 
 les valeurs de l enum t_en_Astre_Type defini dans astre.h */
 
void ASTRE_PARAMS_INIT               ( STRUCT_ASTRE_PARAMS * ) ;
void ASTRE_PARAMS_DISPLAY            ( STRUCT_ASTRE_PARAMS * ) ;

void ASTRE_INIT                      ( STRUCT_ASTRE * ) ;
void ASTRE_RESET                     ( STRUCT_ASTRE *) ;
void ASTRE_FORMATE_DONNEES_AFFICHAGE ( STRUCT_ASTRE *) ;
void ASTRE_DISPLAY_MODE_STELLARIUM   ( STRUCT_ASTRE *) ;

#endif