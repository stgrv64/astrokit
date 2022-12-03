/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | ajout entete
# 01/05/2021  | ajout fonction CONFIG_GETCWD
# 21/03/2022  | ajout fonctions initialisations des codes 
# 22/03/2022  | ajout fonction SUIVI_MAJ_PAS (anciennement 
#               dans ir.c / .h)
# 2022-04-28  | ajout de 2 resolutions plus simple (affichage contraint par LCD) :
# 2022-04-28  | - c_hhmm_*
# 2022-04-28  | - c_dd_*  
# 2022-05-02  | ajout fonction CONFIG_PATH_FIND
# 2022-05-24  | ajout fonction PTHREADS_AFFICHER_ETAT
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_CONFIG_H
#define ASTRO_CONFIG_H

#include "astro_global.h"

/* exemple de fichier de configuration -----------------------------------

MENU_PAR_DEFAUT  MENU_AZIMUTAL
ASTRE_PAR_DEFAUT NGC6755

//  ----------------------- parametres repertoires et fichiers 

CONFIG_REP_CAT       cat
CONFIG_REP_CFG       cfg
CONFIG_REP_LOG       log
CONFIG_REP_IN        inp
CONFIG_REP_OUT       out
CONFIG_REP_SCR       scr
CONFIG_FIC_LED       /tmp/astrokit-boo-recup-led-ir.txt
CONFIG_FIC_LOG       astrokit.log.new
CONFIG_FIC_DATE      date.txt
CONFIG_FIC_HHMM      hhmm.txt

CONFIG_SCR_KERNEL   script_kernel.sh

//  ----------------------- parametres temporisation                       
//  tempo_menu     doit etre legerement inferieur aux autres tempo         
//  tempo_raq      doit etre etre deux fois inferieur aux autres tempos    

TEMPO_MENU      1000000
TEMPO_RAQ       500000
TEMPO_IR        1000010
TEMPO_TERMIOS   1000020
TEMPO_CAPTEURS  50000
TEMPO_LCD_LOOP  1000000
TEMPO_LCD_DISP  1000000
TEMPO_PID_LOOP  5       

//  ----------------------- parametres asservissement de frequence PID 

PID_ECH 1.0
PID_KP  1.0
PID_KI  0.05 
PID_KD  0.05

//  ----------------------- parametres acquisitions 

DEVICE_USE_INFRAROUGE       1  //  flag qui indique utilisation de infrarouge comme source input
DEVICE_USE_KEYBOARD         1  //  flag qui indique utilisation du clavier comme source input
DEVICE_USE_LCD              0  //  flag qui indique utilisation de ecran LCD1602 / PCA8574
DEVICE_USE_CONTROLER        0  //  flag qui indique utilisation des controleurs de moteur externes
DEVICE_USE_CAPTEURS         0  //  flag qui indique utilisation de capteurs comme source input
DEVICE_USE_RAQUETTE         0  //  flag qui indique utilisation d une raquette (gpio) comme source input
DEVICE_USE_BLUETOOTH        0  //  flag qui indique utilisation du bluetooth comme source input

//  ----------------------- parametres geographiques 

//  Latitude et longitude Escout (64) 

LATITUDE   43.28
LONGITUDE  -0.37
ALTITUDE   100

//  Latitude et longitude Orgeres (35) 
//  LATITUDE   48.0 
//  LONGITUDE  -1.67 
//  ALTITUDE   100 

//  Latitude et longitude de test   
//  LATITUDE   61.0 
//  LONGITUDE  -149.9 
//  ALTITUDE   100 

// -------------------------------------------------------------------------- 
//  Conf corecte pour carte essai carre (celle qui s enfiche sur rpi3aplus) 
//  et moteurs en metal NEMA 11 (pas ceux qui sont sur la monture vixen gp) 
// -------------------------------------------------------------------------- 

GPIO_LED_ETAT   4
GPIO_ALT        21,26,19,13
GPIO_AZI        6,5,7,11
//  GPIO_MASQUE     0,1,2,3 

// -------------------------------------------------------------------------- 
//  Masque corect pour carte essai carre (celle qui s enfiche sur rpi3aplus) 
// -------------------------------------------------------------------------- 

//  GPIO_LED_ETAT   4 
//  GPIO_ALT        21,26,19,13 
//  GPIO_AZI        6,5,7,11 

GPIO_MASQUE     3,1,0,2

//  ----------------------- parametres pwm 

GPIO_FREQUENCE_PWM  250 

//  ----------------------- parametres reductions altitude 
//  Parametres des moteurs NEQ corriges a la date du 27 mars 2022 : reduction = 5760  
//  mesure precisemebnt sur 5 tours en 28800 pas avec le binaire /usr/local/bin/gpio23  

ALT_R1  144      //  reduction liee a la monture (nombre de dents de la roue dentee de l axe) 
ALT_R2  100       //  reducteur du moteur (nombre de pas) : R total = 5760 (64*90) (32*180) 
ALT_R3  90       //  reducteur du moteur (gearbox) : R total = 5760 (64*90) (32*180) 
ALT_R4  8       //  mode micro pas utilise (1/x) 
ALT_R5  1        //  reduction liee a la poulie 
ALT_R6  1        //  reduction liee au cpu 
ALT_R7  1        //  reduction non decrite plus haut .... 
ALT_ROT 0        //  Flag de reversibilite du sens de rotation 
ALT_ACC 2        //  Facteur de multiplication en mode MANUEL 

//  ----------------------- parametres reductions azimut 

AZI_R1  144      //  reduction lee a la monture (nombre de dents de la roue dentee de l axe) 
AZI_R2  100       //  reducteur du moteur (nombre de pas) : R total = 5760 (64*90) (32*180) 
AZI_R3  90       //  reducteur du moteur (gearbox) : R total = 5760 (64*90) (32*180) 
AZI_R4  8       //  mode micro pas utilise (1/x) 
AZI_R5  1        //  reduction liee a la poulie 
AZI_R6  1        //  reduction liee au cpu 
AZI_R7  1        //  reduction non decrite plus haut .... 
AZI_ROT 1        //  Flag de reversibilite du sens de rotation 
AZI_ACC 2        //  Facteur de multiplication en mode MANUEL 

//  ----------------------- parametres reductions altaz 

ALTAZ_FORWARD        1.005   //  affinage vitesses : acceleration globale tres lente (azi + alt) 
ALTAZ_REWIND         1.005   //  affinage vitesses : acceleration globale tres lente (azi + alt) 

ALTAZ_FORWARD_FAST   1.5     //  acceleration globale rapide (azi + alt) 
ALTAZ_REWIND_FAST    1.5     //  acceleration globale rapide (azi + alt) 

*/
typedef enum {
  MENU_PAR_DEFAUT=0,
  ASTRE_PAR_DEFAUT,
  CONFIG_REP_CAT,
  CONFIG_REP_CFG,
  CONFIG_REP_LOG,
  CONFIG_REP_IN,
  CONFIG_REP_OUT,
  CONFIG_REP_SCR,
  CONFIG_FIC_LED,
  CONFIG_FIC_LOG,
  CONFIG_FIC_DATE,
  CONFIG_FIC_HHMM,
  CONFIG_SCR_KERNEL,
  TEMPO_MENU,
  TEMPO_RAQ,
  TEMPO_IR,
  TEMPO_TERMIOS,
  TEMPO_CAPTEURS,
  TEMPO_LCD_LOOP,
  TEMPO_LCD_DISP,
  TEMPO_PID_LOOP,
  PID_ECH,
  PID_KP,
  PID_KI,
  PID_KD,
  DEVICE_USE_INFRAROUGE,
  DEVICE_USE_KEYBOARD,
  DEVICE_USE_LCD,
  DEVICE_USE_CONTROLER,
  DEVICE_USE_CAPTEURS,
  DEVICE_USE_RAQUETTE,
  DEVICE_USE_BLUETOOTH,
  LATITUDE,
  LONGITUDE,
  ALTITUDE,
  GPIO_LED_ETAT,
  GPIO_ALT,
  GPIO_AZI,
  GPIO_MASQUE,
  GPIO_FREQUENCE_PWM,
  ALT_R1,
  ALT_R2,
  ALT_R3,
  ALT_R4,
  ALT_R5,
  ALT_R6,
  ALT_R7,
  ALT_ROT,
  ALT_ACC,
  AZI_R1,
  AZI_R2,
  AZI_R3,
  AZI_R4,
  AZI_R5,
  AZI_R6,
  AZI_R7,
  AZI_ROT,
  AZI_ACC,
  ALTAZ_FORWARD,
  ALTAZ_REWIND,
  ALTAZ_FORWARD_FAST,
  ALTAZ_REWIND_FAST,
  CONFIG_ENUM_NUMERO_DERNIER_PARAMETRE_OBLIGATOIRE
}
t_en_Config_Parametres_Obligatoires ;

typedef enum {
  CONFIG_TYPE_INDETERMINE=0,
  CONFIG_TYPE_CHAR,
  CONFIG_TYPE_CHARTAB,
  CONFIG_TYPE_INT,
  CONFIG_TYPE_LONG,
  CONFIG_TYPE_FLOAT,
  CONFIG_TYPE_DOUBLE,
  CONFIG_TYPE_UNSIGNED_INT,
  CONFIG_TYPE_UNSIGNED_LONG
}
t_en_Config_Parametres_Obligatoires_Types ;

/*---------------------------------------------------*/
/* Parametres de fichier config                      */ 
/*---------------------------------------------------*/

struct STR_CONFIG_PARAMS {

  pthread_mutex_t con_par_mutex ;
  int             par_default_menu ;
  char            par_rep_home   [ CONFIG_TAILLE_BUFFER_64 ] ;
  char            par_rep_cat    [ CONFIG_TAILLE_BUFFER_64 ] ;
  char            par_rep_cfg    [ CONFIG_TAILLE_BUFFER_64 ] ;      
  char            par_rep_log    [ CONFIG_TAILLE_BUFFER_64 ] ;      
  char            par_rep_in     [ CONFIG_TAILLE_BUFFER_64 ] ;    
  char            par_rep_out    [ CONFIG_TAILLE_BUFFER_64 ] ;    
  char            par_rep_scr    [ CONFIG_TAILLE_BUFFER_64 ] ; 
  char            par_fic_pid    [ CONFIG_TAILLE_BUFFER_64 ] ;
  char            par_fic_log    [ CONFIG_TAILLE_BUFFER_64 ] ;            
  char            par_fic_dat    [ CONFIG_TAILLE_BUFFER_64 ] ;            
  char            par_fic_hhm    [ CONFIG_TAILLE_BUFFER_64 ] ;        
  char            par_fic_led    [ CONFIG_TAILLE_BUFFER_64 ] ; 
  char            par_src_ker    [ CONFIG_TAILLE_BUFFER_64 ] ;
} ;

/*---------------------------------------------------*/
/* Fin parametres de fichier config                  */ 
/*---------------------------------------------------*/

struct STR_CONFIG {
 pthread_mutex_t con_mutex ;
 char con_params  [CONFIG_DATAS_NB_LIGNES] [CONFIG_DATAS_NB_COLONNES] [CONFIG_TAILLE_BUFFER_256] ;
} ;
typedef struct  STR_CONFIG STRUCT_CONFIG ;

static const char * c_Bin_Possible_Paths[] = {
  "/bin",
  "/sbin",
  "/usr/bin",
  "/usr/sbin",
  "/usr/local/bin",
  "/usr/local/sbin"
} ;

static const char * gc_Config_Params_Obligatoires[] = {
  "MENU_PAR_DEFAUT",
  "ASTRE_PAR_DEFAUT",
  "CONFIG_REP_CAT",
  "CONFIG_REP_CFG",
  "CONFIG_REP_LOG",
  "CONFIG_REP_IN",
  "CONFIG_REP_OUT",
  "CONFIG_REP_SCR",
  "CONFIG_FIC_LED",
  "CONFIG_FIC_LOG",
  "CONFIG_FIC_DATE",
  "CONFIG_FIC_HHMM",
  "CONFIG_SCR_KERNEL",
  "TEMPO_MENU",
  "TEMPO_RAQ",
  "TEMPO_IR",
  "TEMPO_TERMIOS",
  "TEMPO_CAPTEURS",
  "TEMPO_LCD_LOOP",
  "TEMPO_LCD_DISP",
  "TEMPO_PID_LOOP",
  "PID_ECH",
  "PID_KP",
  "PID_KI",
  "PID_KD",
  "DEVICE_USE_INFRAROUGE",
  "DEVICE_USE_KEYBOARD",
  "DEVICE_USE_LCD",
  "DEVICE_USE_CONTROLER",
  "DEVICE_USE_CAPTEURS",
  "DEVICE_USE_RAQUETTE",
  "DEVICE_USE_BLUETOOTH",
  "LATITUDE",
  "LONGITUDE",
  "ALTITUDE",
  "GPIO_LED_ETAT",
  "GPIO_ALT",
  "GPIO_AZI",
  "GPIO_MASQUE",
  "GPIO_FREQUENCE_PWM",
  "ALT_R1",
  "ALT_R2",
  "ALT_R3",
  "ALT_R4",
  "ALT_R5",
  "ALT_R6",
  "ALT_R7",
  "ALT_ROT",
  "ALT_ACC",
  "AZI_R1",
  "AZI_R2",
  "AZI_R3",
  "AZI_R4",
  "AZI_R5",
  "AZI_R6",
  "AZI_R7",
  "AZI_ROT",
  "AZI_ACC",
  "ALTAZ_FORWARD",
  "ALTAZ_REWIND",
  "ALTAZ_FORWARD_FAST",
  "ALTAZ_REWIND_FAST"
} ;

static const int gi_Config_Params_Obligatoires_Type[] = {
  CONFIG_TYPE_INT, // "MENU_PAR_DEFAUT",
  CONFIG_TYPE_INT, // "ASTRE_PAR_DEFAUT",
  CONFIG_TYPE_INT, // "CONFIG_REP_CAT",
  CONFIG_TYPE_INT, // "CONFIG_REP_CFG",
  CONFIG_TYPE_INT, // "CONFIG_REP_LOG",
  CONFIG_TYPE_INT, // "CONFIG_REP_IN",
  CONFIG_TYPE_INT, // "CONFIG_REP_OUT",
  CONFIG_TYPE_INT, // "CONFIG_REP_SCR",
  CONFIG_TYPE_INT, // "CONFIG_FIC_LED",
  CONFIG_TYPE_INT, // "CONFIG_FIC_LOG",
  CONFIG_TYPE_INT, // "CONFIG_FIC_DATE",
  CONFIG_TYPE_INT, // "CONFIG_FIC_HHMM",
  CONFIG_TYPE_INT, // "CONFIG_SCR_KERNEL",
  CONFIG_TYPE_INT, // "TEMPO_MENU",
  CONFIG_TYPE_INT, // "TEMPO_RAQ",
  CONFIG_TYPE_INT, // "TEMPO_IR",
  CONFIG_TYPE_INT, // "TEMPO_TERMIOS",
  CONFIG_TYPE_INT, // "TEMPO_CAPTEURS",
  CONFIG_TYPE_INT, // "TEMPO_LCD_LOOP",
  CONFIG_TYPE_INT, // "TEMPO_LCD_DISP",
  CONFIG_TYPE_INT, // "TEMPO_PID_LOOP",
  CONFIG_TYPE_INT, // "PID_ECH",
  CONFIG_TYPE_INT, // "PID_KP",
  CONFIG_TYPE_INT, // "PID_KI",
  CONFIG_TYPE_INT, // "PID_KD",
  CONFIG_TYPE_INT, // "DEVICE_USE_INFRAROUGE",
  CONFIG_TYPE_INT, // "DEVICE_USE_KEYBOARD",
  CONFIG_TYPE_INT, // "DEVICE_USE_LCD",
  CONFIG_TYPE_INT, // "DEVICE_USE_CONTROLER",
  CONFIG_TYPE_INT, // "DEVICE_USE_CAPTEURS",
  CONFIG_TYPE_INT, // "DEVICE_USE_RAQUETTE",
  CONFIG_TYPE_INT, // "DEVICE_USE_BLUETOOTH",
  CONFIG_TYPE_INT, // "LATITUDE",
  CONFIG_TYPE_INT, // "LONGITUDE",
  CONFIG_TYPE_INT, // "ALTITUDE",
  CONFIG_TYPE_INT, // "GPIO_LED_ETAT",
  CONFIG_TYPE_INT, // "GPIO_ALT",
  CONFIG_TYPE_INT, // "GPIO_AZI",
  CONFIG_TYPE_INT, // "GPIO_MASQUE",
  CONFIG_TYPE_INT, // "GPIO_FREQUENCE_PWM",
  CONFIG_TYPE_INT, // "ALT_R1",
  CONFIG_TYPE_INT, // "ALT_R2",
  CONFIG_TYPE_INT, // "ALT_R3",
  CONFIG_TYPE_INT, // "ALT_R4",
  CONFIG_TYPE_INT, // "ALT_R5",
  CONFIG_TYPE_INT, // "ALT_R6",
  CONFIG_TYPE_INT, // "ALT_R7",
  CONFIG_TYPE_INT, // "ALT_ROT",
  CONFIG_TYPE_INT, // "ALT_ACC",
  CONFIG_TYPE_INT, // "AZI_R1",
  CONFIG_TYPE_INT, // "AZI_R2",
  CONFIG_TYPE_INT, // "AZI_R3",
  CONFIG_TYPE_INT, // "AZI_R4",
  CONFIG_TYPE_INT, // "AZI_R5",
  CONFIG_TYPE_INT, // "AZI_R6",
  CONFIG_TYPE_INT, // "AZI_R7",
  CONFIG_TYPE_INT, // "AZI_ROT",
  CONFIG_TYPE_INT, // "AZI_ACC",
  CONFIG_TYPE_INT, // "ALTAZ_FORWARD",
  CONFIG_TYPE_INT, // "ALTAZ_REWIND",
  CONFIG_TYPE_INT, // "ALTAZ_FORWARD_FAST",
  CONFIG_TYPE_INT, // "ALTAZ_REWIND_FAST"
} ;

void   CONFIG_INIT                  (STRUCT_CONFIG * ) ;

void   CONFIG_PARAMETRES_CONFIG     (STRUCT_CONFIG * ) ;
void   CONFIG_FIC_DISPLAY           (STRUCT_CONFIG * ) ;
int    CONFIG_FIC_READ              (STRUCT_CONFIG * ) ;

void   CONFIG_AFFICHER_MODE_LONG    (STRUCT_ASTRE *, STRUCT_LIEU *, STRUCT_CALCULS *) ;

void   CONFIG_PARAMETRES_AFFICHER   (void) ;
void   CONFIG_AFFICHER_TOUT         (void) ;  /* FIXME ajout 20191228 */
void   CONFIG_MENU_CHANGE_DETECT    (void) ;  /* FIXME ajout 20200102 */

int    CONFIG_FORMAT_ADMIS          (char c) ;
int    CONFIG_FIN_LIGNE             (char c) ;
int    CONFIG_DEB_COM               (char c);
int    CONFIG_FIN_MOT               (char c) ;
int    CONFIG_FIN_FICHIER           (char c) ;

int    CONFIG_PATH_FIND             ( char * , char *) ;
void   CONFIG_FIC_VERIFY              (STRUCT_CONFIG *) ;

#endif

//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================

