/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 15/04/2021  | * ajout entete
#               * ajout variables entree chemins de config.txt
#               * suppression des constantes associees
# --------------------------------------------------------------
# xx/yy/2021  | * remplacement certains types par typedef enum
# --------------------------------------------------------------
# 15/11/2021  | * modification des types enum et contenu enum
#               * modification ordre des menus (MENU_AZIMUTAL=0)
# -------------------------------------------------------------- 
*/

#ifndef TYPES_H
#define TYPES_H

// inclusion des librairies persos

#define DEBUG     0
#define DEBUG_LOG 0

// quelques macros de debugging

#ifndef DEBUG
  #define TRACE(fmt, args...)      while(0) { } 
  #define TRACE1(fmt, args...)     while(0) { } 
  #define TRACE2(fmt, args...)     while(0) { } 
#endif

// niveau 0

#if defined(DEBUG) && DEBUG == 0 

#if defined(DEBUG_LOG) && DEBUG_LOG == 1
  #define TRACE(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(flog, "\n%s : " fmt, __func__, ##args) ; } 
  #define TRACE1(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(flog, "\n%s : " fmt, __func__, ##args) ; } 
  #define TRACE2(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(flog, "\n%s : " fmt, __func__, ##args) ; }
#else 
  #define TRACE(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
  #define TRACE1(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
  #define TRACE2(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#endif

#endif 


// niveau 1

#if defined(DEBUG) && DEBUG == 1

#if defined(DEBUG_LOG) && DEBUG_LOG == 1
  #define TRACE(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(flog, "\n%s : " fmt, __func__, ##args) ; } 
  #define TRACE1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(flog, "\n%s : " fmt, __func__, ##args) ; } 
  #define TRACE2(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(flog, "\n%s : " fmt, __func__, ##args) ; }
#else 
  #define TRACE(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
  #define TRACE1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
  #define TRACE2(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#endif

#endif 

// niveau 2

#if defined(DEBUG) && DEBUG == 2

#if defined(DEBUG_LOG) && DEBUG_LOG == 1
  #define TRACE(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(flog, "\n%s : " fmt, __func__, ##args) ; } 
  #define TRACE1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(flog, "\n%s : " fmt, __func__, ##args) ; } 
  #define TRACE2(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(flog, "\n%s : " fmt, __func__, ##args) ; }
#else 
  #define TRACE(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
  #define TRACE1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
  #define TRACE2(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#endif

#endif 
	
// ------------------------------------------------------------------------
// Constantes
// ------------------------------------------------------------------------

#define  CONFIG_FIC_CFG        "config.txt"

#define TEMPORISATION_RAQUETTE 5000
#define TEMPORISATION_MENU     1000000

#define MAIN_TA_RESET         0.1 
#define MAIN_TH_RESET         0.1 

#define MAIN_FA_RESET         10 
#define MAIN_FH_RESET         10

#define MAIN_TA_TRANSITOIRE   0.25
#define MAIN_TH_TRANSITOIRE   0.25

#define MAIN_FA_TRANSITOIRE   4
#define MAIN_FH_TRANSITOIRE   4

#define  CONFIG_ASTROLOG      1

#define  CONFIG_MES           "MES"
#define  CONFIG_NGC           "NGC"
#define  CONFIG_ETO           "ETO"
#define  CONFIG_PLA           "PLA"

#define  DATAS_NB_LIGNES         200 
#define  DATAS_NB_COLONNES       2

#define  CONFIG_TAILLE_BUFFER_4  4
#define  CONFIG_TAILLE_BUFFER_5  5
#define  CONFIG_TAILLE_BUFFER_8  8
#define  CONFIG_TAILLE_BUFFER_16 16
#define  CONFIG_TAILLE_BUFFER_32 32
#define  CONFIG_TAILLE_BUFFER_64 64

#define  CONFIG_TAILLE_BUFFER    255

#define  ZERO_CHAR           '\0'

#define  CONFIG_MILLI_SEC    1000                // frequence d'une microseconde (pour les usleep et calculs)
#define  CONFIG_MICRO_SEC    1000000             // frequence d'une microseconde (pour les usleep et calculs)
#define  CONFIG_NANO_SEC     1000000000          // frequence d'une nanoseconde (pour les nanosleep et calculs) 

#define  SIDERAL              86164.0           // duree du jour sideral en seconde
#define  PI                   3.141592653589793238462
#define  PIPI                 6.283185307179586476925
#define  OMEGA                0.000072921235169903748
#define  SIN45                0.70710678118654
#define  COS45                0.70710678118654
#define  DEGRES               57.29577951308232
#define  DIV                  206264.8062470964   // 60 * 60 * DEGRES
#define  ANGLE_S              15.04108444361914   // angle en secondes parcouru pendant une seconde a l'equateur

#define  ROT_RAD_SEC          0.000072921235170   // vitesse de rotation de la Terre (radians par seconde)
#define  ROT_DEG_SEC          0.004178079012116   // vitesse de rotation de la Terre (degres par seconde)

#define  MAX_THREADS          10
#define  ASTRE_TAILLE_BUFFER  20
#define  ASTRE_NB_COLONNES    10000

#define  CONFIG_ASS              3 
#define  CONFIG_ACTIONS_SIZE     7
#define  CONFIG_VALIDATIONS_SIZE 10

// ------------------------------------------------------------------------
// definition des structures de donnees du programme
// ------------------------------------------------------------------------

typedef enum {
  FAUX=0,
  VRAI  
}
t_en_Booleen ;

typedef enum {
  ASTRE_PLANETE=0,
  ASTRE_CIEL_PROFOND  
}
t_en_Astre ;

typedef enum { 
	SUIVI_MANUEL=0,
	SUIVI_EQUATORIAL,
	SUIVI_AZIMUTAL,
}
t_en_Suivis ;

typedef enum { 

  MENU_AZIMUTAL=0,
  MENU_EQUATORIAL,
  MENU_MANUEL_BRUT,
  MENU_MANUEL_NON_ASSSERVI,
  MENU_MANUEL_ASSERVI,
  MENU_GOTO,
  MENU_INFO,
  MENU_RESEAU_UP,
  MENU_RESEAU_DOWN,
  MENU_PROGRAMME_DOWN,
  MENU_DOWN
}
t_en_Menus ;

//=====================================================

typedef struct {

  char symbole  [CONFIG_TAILLE_BUFFER_32] ;
  char nombre   [CONFIG_TAILLE_BUFFER_32] ;
  char mot      [CONFIG_TAILLE_BUFFER_32] ;
  char phrase   [CONFIG_TAILLE_BUFFER_32] ;
  char premier  [CONFIG_TAILLE_BUFFER_32] ;
  char valider  [CONFIG_TAILLE_BUFFER_32] ;
  char menu     [CONFIG_TAILLE_BUFFER_32] ;
  
  char actions[     CONFIG_ACTIONS_SIZE     ][ CONFIG_TAILLE_BUFFER_32 ]  ;
  char validations[ CONFIG_VALIDATIONS_SIZE ][ CONFIG_TAILLE_BUFFER_32 ]  ;
  
  int  appui_en_cours ;
  int  mot_en_cours ;
  int  phrase_lue ;
  int  premier_en_cours ;
  
  unsigned long temporisation_clavier   ; 
}
CLAVIER ;

//=====================================================
typedef struct {
 
 double       lat ;  // latitude
 double       lon ;  // longitude
 double       alt ;
 double       JJ  ;  // jour julien
 double       TS ;   // temps sideral
 double       TSR ;  // temps sideral ramene en radians
 double       JD ;   // jour decimal
}
LIEU ;

//=====================================================
typedef struct {
  
  double hd ;   // heure decimale
  int mm ;      // month
  int yy ;      // year
  int dd ;      // day
  int HH ;      // hour
  int MM ;      // minutes
  int SS ;      // secondes
}
TEMPS ;

//=====================================================
typedef struct {

  int          SUIVI_MANUEL ;
  int          SUIVI_ALIGNEMENT ;
  int          SUIVI_GOTO ;
  int          SUIVI_VOUTE ;
  int          SUIVI_EQUATORIAL ;

  int          DONNEES_BLUETOOTH ;
  int          DONNEES_CAPTEURS ;
  int          DONNEES_INFRAROUGE ;
  int          DONNEES_RAQUETTE ;
  int          DONNEES_CONTROLEUR ; 

  int          init_capteurs ;
  
  pthread_mutex_t  mutex_alt  ;
  pthread_mutex_t  mutex_azi  ;
  pthread_mutex_t  mutex_cal  ;

  // Pthreads utilises comme pointeurs dans la structure passee en argument des fonctions de thread

  pthread_t    p_suivi_infrarouge ;
  pthread_t    p_suivi_capteurs ;
  pthread_t    p_suivi_calculs ;
  pthread_t    p_suivi_alt ;
  pthread_t    p_suivi_azi ;
  pthread_t    p_menu ;
  pthread_t    p_test ;
  
  pthread_t    p_threads_id[ MAX_THREADS ]  ;
  
  long         t_diff ;
  double       t_diff_sec ;
  
  double       d_temps ;   
  double       d_appui_raq_azi ;
  double       d_appui_raq_alt ;
  
  double       roll ;
  double       pitch ;
  double       heading ;
     
  long         l_NANO_MOINS ;     // a retirer sur les temporisations pour les tests
  
  unsigned int i_statut_suivi ;
  unsigned int i_threads[MAX_THREADS] ;
  
  // Variables utilisees en mode MANUEL SIMPLE
  // elles servent dans le calcul des periodes (par defaut sont egales à 1)

  double       acc_azi ;      // acceleration en azimut   utile en mode manuel simple
  double       acc_alt ;      // acceleration en altitude utile en mode manuel simple
  int          sgn_azi ;      // signe de la frequence en azimut  a modifier au besoin
  int          sgn_alt ;      // signe de la frequence en altitude a modifier au besoin

  // Variables utilisees en mode MANUEL EVOLUE
  int          pas_acc_plus ;
  int          pas_acc_moins ;
  int          pas_forward ;
  int          pas_rewind ;
  int          pas_forward_fast ;
  int          pas_rewind_fast ;
  long         pas_azi ;
  long         pas_alt ;
  long         pas_azi_old ;
  long         pas_alt_old ;
  long         pas_asc ;
  long         pas_dec ;
  long         pas_ouest ;     // flag d'appui sur le touche OUEST
  long         pas_est ;       // flag d'appui sur le touche EST
  long         pas_nord ;      // flag d'appui sur le touche NORD
  long         pas_sud ;       // flag d'appui sur le touche SUD
  
  double       Ta ;       // periode de la frequence a injecter directement
  double       Th ;       // periode de la frequence a injecter directement

  double       temps_a ;   // temps ecoule sur azimut , deduit des calculs gpio : suivi_main_M
  double       temps_h ;   // temps ecoule sur azimut , deduit des calculs gpio : suivi_main_M

  double       Fa ;  // frequence a injecter directement 
  double       Fh ;  // frequence a injecter directement 

  double        Tac ;             // correcteur de periode, pour corriger les effets des latences du systeme, calculer par suivi voute
  double        Thc ;             // correcteur de periode, pour corriger les effets des latences du systeme, calculer par suivi voute

  double        Tacc ;            // correcteur de correcteur de periode, pour corriger les insuffisances du correcteur de base 
  double        Thcc ;            // correcteur de correcteur de periode, pour corriger les insuffisances du correcteur de base 

  unsigned long Ia ;              // nombre d'impulsions mesurees sur azimut
  unsigned long Ih ;              // nombre d'impulsions mesureees sur altitude

  unsigned long Ia_prec ;         // nombre d'impulsions mesurees sur azimut
  unsigned long Ih_prec ;         // nombre d'impulsions mesureees sur altitude 

  double        Ias ;             // somme sur nombres d'impulsions mesurees 
  double        Ihs ;             // somme sur nombres d'impulsions mesurees 

  unsigned long Iat[CONFIG_ASS] ; // tableau des nombres d'impulsions mesurees
  unsigned long Iht[CONFIG_ASS] ; // tableau des nombres d'impulsions mesurees
  
  double       plus ;    // multiplicateur ajustement des periodes si besoin
  double       moins ;   // multiplicateur ajustement des periodes si besoin
  
  int          Sa ;      // signe de la vitesse (direction), tenir compte du flag de reversibilite
  int          Sh ;      // signe de la vitesse (direction), tenir compte du flag de reversibilite
  int          Sa_old ;  // flag de comparaison pour raffraichir ou non les ecritures
  int          Sh_old ;  // flag de comparaison pour raffraichir ou non les ecritures
  
  double       Da ;  // nombre a injecter dans les diviseurs de frequence
  double       Dh ;  // nombre a injecter dans les diviseurs de frequence
  
  t_en_Menus   menu;      // valeur du menu courant 
  t_en_Menus   menu_old ; // sauvegarde du menu precedent

  int          alarme ; // si different de 0 provoque une alarm(alarme) au debut de main
  int          reset ;  // sert de reset pour remettre les compteeurs a zero dans diverses parties du programme
  
  double       pourcentage_tempo ;
  double       DTh ;
  double       DTa ;
  
  unsigned long temporisation_menu  ;
  unsigned long temporisation_raq  ;
  unsigned long temporisation_ir   ; 
  unsigned long temporisation_capteurs  ; 
  unsigned long temporisation_clavier   ; 
  
  char         datas_infrarouge [ CONFIG_TAILLE_BUFFER ] ;
  char         datas_accelerometre [ CONFIG_TAILLE_BUFFER ] ;
  char         datas_boussole [ CONFIG_TAILLE_BUFFER ] ;

  struct timeval tval ; 
} 
SUIVI ;

//=====================================================
typedef struct {

 TEMPS  at ;
 TEMPS  ht ;
 
 TEMPS  At ;
 TEMPS  Ht ;
 
 char   nom         [ ASTRE_TAILLE_BUFFER ] ;
 char   infos       [ ASTRE_TAILLE_BUFFER ] ;  
 char   plus_proche [ ASTRE_NB_COLONNES   ][ ASTRE_TAILLE_BUFFER ] ;
 
 double a ;    // azimut
 double h ;    // altitude
 
 double x ;    // cos(h)cos(a)  = abscisse du point sur la sphere de rayon UN (voute celeste) 
 double y ;    // cos(h)sin(a)  = ordonnee du point sur la sphere de rayon UN (voute celeste) 
 double z ;    // sin(h)        = z        du point sur la sphere de rayon UN (voute celeste) 
  
 double xx ;   // donneee permettant de representer une valeur par OM * val (sphere de unite UN multipliee par la valeur) _ abscisse
 double yy ;   // idem - ordonnee
 double zz ; 
 
 double a0 ;   // valeur precedente de l'azimut
 double h0 ;   // valeur precedente de l'altitude
 
 double A ;    // angle horaire  ( = lieu->temps sideral - ASC)
 double H ;    // declinaison
 
 double DEC  ;  // un resultat de calcul de declinaison
 double ASC ;   // un resultat de calcul de asc
 double ASC1 ;  // un autre resultat de calcul de asc
 double ASC2 ;  // un autre resultat de calcul de asc

 double ALT ;   // un resultat de calcul de ALT
 double AZI ;   // un resultat de calcul de AZI
 double AZI1 ;  // un autre resultat de calcul de AZI
 
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

}
ASTRE ;

//=====================================================
typedef struct {

 double pourcentage_tempo ;
 double calibration_voute ;
 double deb ;
 double fin ;
 double pas ;
 double dt ;
 double acc ;
 double acc_old ;

 long long num ; 

 double DT ; // dt effectif en microsec en tenant compte de acceleration et de tous les autres parametres
}
VOUTE ;

//=====================================================

TEMPS      te, *temps ;
LIEU       li, *lieu ;
ASTRE      as, *astre ;
VOUTE      vo, *voute ;
SUIVI	     su, *suivi ;
CLAVIER    cl, *clavier ;

char   datas     [DATAS_NB_LIGNES] [DATAS_NB_COLONNES] [CONFIG_TAILLE_BUFFER] ;
FILE * flog ; 

// ------------------------------------------------------------------------
// definition des variables dependant du fichier de conf
// ------------------------------------------------------------------------

// ------------ FLAG DE CORRECTION en cas d'erreur de branchement
// exemple le moteur est brancher a l'envers par rapport a ce qui est attendu
// il faut donc inverser la direction !!!!
// valeurs possibles booleennes (0 ou 1)
// un NOR EXCLUSIF est utiliser ensuite pour determiner la bonne valeur
// du parametre concerner (a priori que la direction pour les controleurs de moteur

// ------------ MENU et RAQUETTE
char ASTRE_PAR_DEFAUT[ CONFIG_TAILLE_BUFFER ] ;

int GPIO_LED_ETAT ;

int MODE_EQUATORIAL ; 
int MENU_PAR_DEFAUT ;

int DONNEES_CAPTEURS    ;
int DONNEES_RAQUETTE     ;
int DONNEES_BLUETOOTH    ;
int DONNEES_INFRAROUGE   ;
int DONNEES_CONTROLEUR           ;

int GPIO_RAQ_O  ;
int GPIO_RAQ_E  ;
int GPIO_RAQ_S  ;
int GPIO_RAQ_N  ;
int GPIO_RAQ_V  ;

int GPIO_KEY_L1  ;
int GPIO_KEY_L2  ;
int GPIO_KEY_L3  ;
int GPIO_KEY_L4  ;

int GPIO_KEY_C1  ;
int GPIO_KEY_C2  ;
int GPIO_KEY_C3  ;
int GPIO_KEY_C4  ;

// ------------ PRIORITES DES THREADS -------

unsigned int suivi_menu_sched_priority ;
unsigned int suivi_altitude_sched_priority ;
unsigned int suivi_azimut_sched_priority ;
unsigned int suivi_calculs_sched_priority ;

// ------------ LIEU ------------------------

double LONGITUDE ;
double LATITUDE ;
double ALTITUDE ;

// ------ TEMPORISATIONS DES BOUCLES ------------

unsigned long TEMPO_MENU ;
unsigned long TEMPO_RAQ ;
unsigned long TEMPO_IR ;
unsigned long TEMPO_CLAVIER ;
unsigned long TEMPO_CAPTEURS ;

// ------ PARAMETRES DE ALT ou AZI   ------------

unsigned long ALT_F ;    // frequence de reference (utile si on utilise CALCUL_D)
unsigned int  ALT_N ;    // prediviseur de frequence si il existe (2 puissance N : 1 2 4 16 32 ..)
double        ALT_R ;    // reduction totale
double        ALT_R1 ;   // reduction liee a la monture
double        ALT_R2 ;   // reducteur du moteur
double        ALT_R3 ;   // nombre de pas du moteur en azimut
double        ALT_R4 ;   // mode micro pas utilisee (1/R4)
int           ALT_ROT ;  // Flag de reversibilitee du sens de rotation (en cas d'erreur)
double        ALT_ACC ;  // Facteur de multiplication en mode MANUEL_0

unsigned long AZI_F ;    // frequence de reference (utile si on utilise CALCUL_D)
unsigned int  AZI_N ;    // prediviseur de frequence si il existe (2 puissance N : 1 2 4 16 32 ..)
double        AZI_R ;    // reduction totale
double        AZI_R1 ;   // reduction liee\A0l axe en azimut
double        AZI_R2 ;   // reducteur du moteur
double        AZI_R3 ;   // nombre de pas du moteur en azimut
double        AZI_R4 ;   // mode micro pas utilisee (1/R4)
int           AZI_ROT ;  // Flag de reversibilitee du sens de rotation (en cas d'erreur)
double        AZI_ACC ;  // Facteur de multiplication en mode MANUEL_0

// ------ PARAMETRES DE ALT et AZI   ------------

double        ALTAZ_FORWARD ; // Facteur de multiplication des vitesses pour rattrapages tres tres lent 
double        ALTAZ_REWIND ;  // Facteur de multiplication des vitesses pour rattrapages tres tres lent 

double        ALTAZ_FORWARD_FAST ; // Facteur de multiplication des vitesses pour rattrapages tres rapides 
double        ALTAZ_REWIND_FAST ;  // Facteur de multiplication des vitesses pour rattrapages tres rapides

// les variables suivantes ne servent pas si un expandeur de port est utilise

unsigned char GPIO_DIR_ALT ;  // numero de port GPIO pour l'horloge de l'azimut
unsigned char GPIO_CLK_ALT ;  // numero de port GPIO pour l'horloge de l'azimut
unsigned char GPIO_SLP_ALT ;  // numero de port GPIO pour le sleep du controleur du moteur
unsigned char GPIO_RST_ALT ;  // numero de port GPIO pour le sleep du controleur du moteur
unsigned char GPIO_MMM_ALT ;  // numero de port GPIO pour le sleep du controleur du moteur
unsigned char GPIO_ENA_ALT ;  // numero de port GPIO pour le sleep du controleur du moteur
unsigned char GPIO_M2_ALT  ;  // numero de port GPIO pour le choix du micro pas
unsigned char GPIO_M1_ALT  ;  // numero de port GPIO pour le choix du micro pas
unsigned char GPIO_M0_ALT  ;  // numero de port GPIO pour le choix du micro pas

// les variables suivantes servent si un expandeur de port est utilise. Exemple = MCP23017 ou MCP23008

unsigned char MCP_DIR_ALT  ;  // numero de port MCP pour le sens de rotation de l'azimut
unsigned char MCP_CLK_ALT  ;  // numero de port MCP pour l'horloge de l'azimut
unsigned char MCP_SLP_ALT  ;  // numero de port MCP pour le sleep du controleur du moteur
unsigned char MCP_RST_ALT  ;  // numero de port MCP pour le reset du controleur du moteur
unsigned char MCP_M2_ALT   ;  // numero de port MCP pour le choix du micro pas
unsigned char MCP_M1_ALT   ;  // numero de port MCP pour le choix du micro pas
unsigned char MCP_M0_ALT   ;  // numero de port MCP pour le choix du micro pas

// les variables suivantes ne servent pas si un expandeur de port est utilise

unsigned char GPIO_DIR_AZI ; // numero de port GPIO pour le sens de rotation de l'azimut
unsigned char GPIO_CLK_AZI ; // numero de port GPIO pour l'horloge de l'azimut
unsigned char GPIO_SLP_AZI ; // numero de port GPIO pour le sleep du controleur du moteur
unsigned char GPIO_RST_AZI ;  // numero de port GPIO pour le sleep du controleur du moteur
unsigned char GPIO_MMM_AZI ;  // numero de port GPIO pour le sleep du controleur du moteur
unsigned char GPIO_ENA_AZI ;  // numero de port GPIO pour le sleep du controleur du moteur
unsigned char GPIO_M2_AZI  ; // numero de port GPIO pour le choix du micro pas
unsigned char GPIO_M1_AZI  ; // numero de port GPIO pour le choix du micro pas
unsigned char GPIO_M0_AZI  ; // numero de port GPIO pour le choix du micro pas

// les variables suivantes servent si un expandeur de port est utilise. Exemple = MCP23017 ou MCP23008

unsigned char MCP_DIR_AZI   ; // numero de port GPIO pour l'horloge de l'azimut
unsigned char MCP_CLK_AZI   ; // numero de port GPIO pour l'horloge de l'azimut
unsigned char MCP_SLP_AZI   ; // numero de port GPIO pour le sleep du controleur du moteur
unsigned char MCP_RST_AZI   ; // numero de port GPIO pour le sleep du controleur du moteur
unsigned char MCP_M2_AZI    ; // numero de port GPIO pour le choix du micro pas
unsigned char MCP_M1_AZI    ; // numero de port GPIO pour le choix du micro pas 
unsigned char MCP_M0_AZI   ;  // numero de port GPIO pour le choix du micro pas

// ajout entrees chemin de config.txt (2021)

char CONFIG_REP_CAT  [ CONFIG_TAILLE_BUFFER_64 ] ;
char CONFIG_REP_CFG  [ CONFIG_TAILLE_BUFFER_64 ] ;      
char CONFIG_REP_LOG  [ CONFIG_TAILLE_BUFFER_64 ] ;      
char CONFIG_REP_IN   [ CONFIG_TAILLE_BUFFER_64 ] ;    
        
char CONFIG_FIC_LOG  [ CONFIG_TAILLE_BUFFER_64 ] ;            
char CONFIG_FIC_DATE [ CONFIG_TAILLE_BUFFER_64 ] ;            
char CONFIG_FIC_HHMM [ CONFIG_TAILLE_BUFFER_64 ] ;        

// ajout variable globale de chemin rep home lue (2021)
// avec la commande getcwd 
char CONFIG_REP_HOME [ CONFIG_TAILLE_BUFFER_64 ] ;

// FIXME : ajout variables GPIO_xxx qui sont geres par gpio.c
// uniquement pour affichage via CONFIG_AFFICHER_VARIABLES

char GPIO_ALT [ CONFIG_TAILLE_BUFFER_64 ] ;
char GPIO_AZI [ CONFIG_TAILLE_BUFFER_64 ] ;
char GPIO_MASQUE [ CONFIG_TAILLE_BUFFER_64 ] ;
char GPIO_FREQUENCE_PWM [ CONFIG_TAILLE_BUFFER_64 ] ;

#endif




