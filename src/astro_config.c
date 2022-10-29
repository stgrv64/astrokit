/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | * ajout entete
#  * ajout entree chemins de config.txt
# --------------------------------------------------------------
# 01/05/2021  | 
#       * modification chemin config.txt :
#       * le chemin est directement le repertoire courant
#         lue grace a getcwd, les autres chemins dependent
#         de ce chemin getcwd
#         ajoutee des valeurs lues dans config.txt
#       * mise a jour de la fonction CONFIG_PARAMETRES_AFFICHER 
#         avec nouvelles variables
# --------------------------------------------------------------
# 15/11/2021  | * (astro_global.h) modification des types enum et contenu enum
#               * (astro_global.h) modification ordre des menus (MENU_AZIMUTAL=0)
#  => modification ordre MENUS dans switch
# 17/01/2022  | * ajout gp_Dev_Par->par_use_Keyboard pour utilisation du clavier
# 18/01/2022  | * ajout gp_Con_Par->par_src_ker pour execution script avec droits
#                 root via systemd / execve / execl
#               * remplacement fonctions Trace par Trace 
#                 (evite utilisation fichier log alors que celui ci n'est pas encore ouvert)
# 20/01/2022  |  * creation des entetes doxygen des fonctions
#                * ajout mode et type de STRUCT_ASTRE dans ASTRE_INIT
# 23/01/2022  |  * suppression MODE_EQUATORIAL
#                * changement type gp_Con_Par->par_default_menu
# 21/03/2022  |  * creation fonction CONFIG_INTI_CODES (deport depuis ir.c/h)
#                * creation fonction CODES_INIT_CODE
# avril 2002  |  * debut codage fonctionnalites LCD1602 :
#                * fonctions LCD_xxxx
#                * ajout mutex init sur mutex_datas_infrarouge
#                * => protection zone de code avec dat_inf
#                * ajout fonction LCD_DISPLAY_AGH_DEC / LCD_DISPLAY_EQU_DEC
#                * ASTRE_FORMATE_DONNEES_AFFICHAGE : ajout de 2 resolutions supplementaires
#                * protection zones de code impliquant LCD* (mut_lcd)
# mai  2022      * reprise intégralité code pour utilisation pointeur fct (gp_Lcd->display_xxx)
# juin 2022      * deport du code concernant LCD dans lcd.c
#                * ajout prise en charge fichier gp_Con_Par->par_fic_led (param config.txt)
#                  pour lecture numero led gpio IR , ce paramatre est defini 
#                  dans /boot/config.txt
#                * ajout gp_Pid_Par->par_pid_ech gp_Pid_Par->par_pid_kp gp_Pid_Par->par_pid_ki gp_Pid_Par->par_pid_kd
#
# octobre 2022   ! refonte complete du code 
# -------------------------------------------------------------- 
*/

#include "astro_config.h"

// ajout variable globale de chemin rep home lue (2021)
// avec la commande getcwd 

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_OTHER ;

static const char * c_Bin_Possible_Paths[] = {
  "/bin",
  "/sbin",
  "/usr/bin",
  "/usr/sbin",
  "/usr/local/bin",
  "/usr/local/sbin"
} ;

char      g_Path_Cmd_Stty[ CONFIG_TAILLE_BUFFER_32 ] ;

// ------------------------------------------------------------------------
// FIN definition des variables dependant du fichier de conf --------------
// ------------------------------------------------------------------------

int NOR_EXCLUSIF(int i,int j) { return !i^j ;};

// R = R1.R2.R3.R4
// F/(2^N.D.R) = (1/2Pi)vitesse
// cas particulier equateur omega = 2Pi / 86164 rad.s-1
// ==> 86164.F = 2^N.D.R


/*****************************************************************************************
* @fn     : CONFIG_FORMAT_ADMIS
* @author : s.gravois
* @brief  : Cette fonction verifie si caractere lu est correct (CONFIG_READ)
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_FORMAT_ADMIS(char c) {
  
  int i=(int)c ;
  int j ;
  
  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  j=0;
  
  if ( i>45 && i<58 )j=1;  // chiffres et le point
  if ( i>64 && i<91 )j=1;  // MAJUSCULES  
  if ( i>96 && i<123)j=1;  // minuscules
  if ( i==95)j=1;          // underscore
  if ( i==44)j=1;          // virgule
  if ( i==45)j=1;          // signe -

  return j ; 
}
/*****************************************************************************************
* @fn     : CONFIG_FIN_LIGNE
* @author : s.gravois
* @brief  : Cette fonction verifie si caractere est un caracter fin de ligne (\n=10)
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_FIN_LIGNE(char c) {
  int i=(int)c ;
  int j ;
  
  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  j=0;
  if ( i==10)j=1;  // \n
  return j ; 
}
/*****************************************************************************************
* @fn     : CONFIG_DEB_COM
* @author : s.gravois
* @brief  : Cette fonction verifie si caractere un debut de commentaire '#'
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_DEB_COM(char c) {
  int i=(int)c ;
  int j ;

  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  j=0;
  if ( i==35)j=1 ; 
  return j ; 
}
/*****************************************************************************************
* @fn     : CONFIG_FIN_MOT
* @author : s.gravois
* @brief  : Cette fonction verifie si caractere est apres un mot (espace ou \t)
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_FIN_MOT(char c) {
  int i=(int)c ;
  int j ;

  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  j=0;
  if ( i==9 )j=1;  // \t
  if ( i==32)j=1;  // space
  return j ; 
}
/*****************************************************************************************
* @fn     : CONFIG_FIN_MOT
* @author : s.gravois
* @brief  : Cette fonction verifie si caractere est une fin de fichier (EOF) ou non admis
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_FIN_FICHIER(char c) {
  int i=(int)c ;
  int j ;
  
  ARBO(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  j=0;
  if ( i>122 )j=1;      // caracteres non admis ascii > 122
  if ( i==-1) j=1;      // EOF
  if ( i<32 && i!=9 && i!=10 ) j=1; // caracteres non admis ascii < 32
  return j ; 
}
/*****************************************************************************************
* @fn     : CONFIG_PARAMETRES_CONFIG
* @author : s.gravois
* @brief  : Cette fonction initialise les parametres a l'aide d'un tableau precedemment lu
            par CONFIG_READ / GPIO_READ
* @param  : l_Char_Params[CONFIG_DATAS_NB_COLONNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : juin 2022 : ajout parametres STRUCT_PID
* @todo   : 
*****************************************************************************************/

void CONFIG_PARAMETRES_CONFIG(char lc_Params[CONFIG_DATAS_NB_COLONNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]) {

  int l ;

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
   //----------------------------------------------------------------------
   // Variables susceptibles de ne pas etre lues (doivent etre mise a zero)
   //----------------------------------------------------------------------
   
   gp_Cal_Par->par_alt_red_tot = 0 ;
   gp_Cal_Par->par_azi_red_tot = 0 ;
   
   /* Les coefficients de reduction sont egaux a 1 par defaut */ 
   /* car leur presence n est pas obligatoires dans le fichier config.txt */

   gp_Cal_Par->par_alt_red_1 = 1.0 ;  // reduction liee a la monture ()
   gp_Cal_Par->par_alt_red_2 = 1.0;   // reducteur du moteur (nombre de pas)
   gp_Cal_Par->par_alt_red_3 = 1.0;   // reducteur du moteur (gearbox)
   gp_Cal_Par->par_alt_red_4 = 1.0;   // mode micro pas utilisee (1/R4)
   gp_Cal_Par->par_alt_red_5 = 1.0;   // reduction liee a la poulie
   gp_Cal_Par->par_alt_red_6 = 1.0;   // reduction liee au cpu
   gp_Cal_Par->par_alt_red_7 = 1.0;   // reduction non decrite plus haut

   gp_Cal_Par->par_azi_red1 = 1.0 ;  // reduction liee a la monture
   gp_Cal_Par->par_azi_red2 = 1.0;   // reducteur du moteur
   gp_Cal_Par->par_azi_red3 = 1.0;   // nombre de pas du moteur en azimut
   gp_Cal_Par->par_azi_red4 = 1.0;   // mode micro pas utilisee (1/R4)
   gp_Cal_Par->par_azi_red5 = 1.0;   // reduction liee a la poulie
   gp_Cal_Par->par_azi_red6 = 1.0;   // reduction liee au cpu
   gp_Cal_Par->par_azi_red7 = 1.0;   // reduction non decrite plus haut

   gp_Dev_Par->par_use_Capteurs = 0    ;
   gp_Dev_Par->par_use_Raquette = 0    ;
   gp_Dev_Par->par_use_Bluetooth = 0   ;
   gp_Dev_Par->par_use_Infrared = 0  ;
   gp_Dev_Par->par_use_Controler = 0 ;
   gp_Dev_Par->par_use_Keyboard = 0 ;
   gp_Dev_Par->par_use_Lcd = 0 ;

   gp_Gpi_Par->par_led_etat=0;

   /* Definition de valeurs par defauts pour les TEMPO */ 

  gp_Tim_Par->par_tempo_Menu     = 50000;
  gp_Tim_Par->par_tempo_Raq      = 51000; /* est utilisee uniquement dans SUIVI_MANUEL_ASSERVI */
  gp_Tim_Par->par_tempo_Ir       = 52000;
  gp_Tim_Par->par_tempo_Termios  = 53000;
  gp_Tim_Par->par_tempo_Clavier  = 25000;
  gp_Tim_Par->par_tempo_Capteurs = 50000;
  gp_Tim_Par->par_tempo_Lcd_Loop = 250000 ; 
  gp_Tim_Par->par_tempo_Lcd_Disp = 100000 ; 

  gp_Pid_Par->par_pid_ech        = 3 ;

   //----------------------------------------------------------------------
   // Lecture des variables dans la config lue dans le fichier de config
   //-----------gp_Ast_Par->par_default_object-------------------------------------------

  memset( gp_Ast_Par->par_default_object, CALCUL_ZERO_CHAR, sizeof( gp_Ast_Par->par_default_object ) ) ;

  memset( gp_Con_Par->par_rep_cat, CALCUL_ZERO_CHAR, sizeof( gp_Con_Par->par_rep_cat ) ) ;
  memset( gp_Con_Par->par_rep_cfg, CALCUL_ZERO_CHAR, sizeof( gp_Con_Par->par_rep_cfg ) ) ;
  memset( gp_Con_Par->par_rep_log, CALCUL_ZERO_CHAR, sizeof( gp_Con_Par->par_rep_log ) ) ;
  memset( gp_Con_Par->par_rep_in,  CALCUL_ZERO_CHAR, sizeof( gp_Con_Par->par_rep_in ) ) ;
  memset( gp_Con_Par->par_rep_scr, CALCUL_ZERO_CHAR, sizeof( gp_Con_Par->par_rep_scr ) ) ;
  memset( gp_Con_Par->par_fic_led, CALCUL_ZERO_CHAR, sizeof( gp_Con_Par->par_fic_led ) ) ;
  memset( gp_Con_Par->par_fic_log, CALCUL_ZERO_CHAR, sizeof( gp_Con_Par->par_fic_log ) ) ;
  memset( gp_Con_Par->par_fic_pid, CALCUL_ZERO_CHAR, sizeof( gp_Con_Par->par_fic_pid ) ) ;
  memset( gp_Con_Par->par_fic_dat, CALCUL_ZERO_CHAR, sizeof( gp_Con_Par->par_fic_dat ) ) ;
  memset( gp_Con_Par->par_fic_hhm, CALCUL_ZERO_CHAR, sizeof( gp_Con_Par->par_fic_hhm ) ) ;
  memset( gp_Con_Par->par_src_ker, CALCUL_ZERO_CHAR, sizeof( gp_Con_Par->par_src_ker ) ) ;

  for(l=0;l<CONFIG_DATAS_NB_COLONNES;l++) {
     
      // FIXME : note 2021 : les variables GPIO_xxx sont gérées dans le ficheir gpio.c

     if(!strcmp("gp_Ast_Par->par_default_object",lc_Params[l][0])) strcpy( gp_Ast_Par->par_default_object, lc_Params[l][1]) ;

     if(!strcmp("gp_Con_Par->par_default_menu",lc_Params[l][0])) {
      if(!strcmp(lc_Params[l][1],"MENU_AZIMUTAL" ))           gp_Con_Par->par_default_menu = MENU_AZIMUTAL ;
      if(!strcmp(lc_Params[l][1],"MENU_EQUATORIAL" ))         gp_Con_Par->par_default_menu = MENU_EQUATORIAL ;
      if(!strcmp(lc_Params[l][1],"MENU_MANUEL_BRUT" ))        gp_Con_Par->par_default_menu = MENU_MANUEL_BRUT ;
      if(!strcmp(lc_Params[l][1],"MENU_MANUEL_NON_ASSERVI" )) gp_Con_Par->par_default_menu = MENU_MANUEL_NON_ASSERVI ;
      if(!strcmp(lc_Params[l][1],"MENU_MANUEL_ASSERVI" ))     gp_Con_Par->par_default_menu = MENU_MANUEL_ASSERVI ;
      if(!strcmp(lc_Params[l][1],"MENU_GOTO" ))               gp_Con_Par->par_default_menu = MENU_GOTO ;
      if(!strcmp(lc_Params[l][1],"MENU_INFO" ))               gp_Con_Par->par_default_menu = MENU_INFO ;
      if(!strcmp(lc_Params[l][1],"MENU_RESEAU_UP" ))          gp_Con_Par->par_default_menu = MENU_RESEAU_UP ;
      if(!strcmp(lc_Params[l][1],"MENU_RESEAU_DOWN" ))        gp_Con_Par->par_default_menu = MENU_RESEAU_DOWN ;
      if(!strcmp(lc_Params[l][1],"MENU_PROGRAMME_DOWN" ))     gp_Con_Par->par_default_menu = MENU_PROGRAMME_DOWN ;
      if(!strcmp(lc_Params[l][1],"MENU_DOWN" ))               gp_Con_Par->par_default_menu = MENU_DOWN ;
     } 
      /* Definition de valeurs par defauts pour les TEMPO */ 
     if(!strcmp("gp_Tim_Par->par_tempo_Raq",lc_Params[l][0]))      gp_Tim_Par->par_tempo_Raq=atol(lc_Params[l][1]);
     if(!strcmp("gp_Tim_Par->par_tempo_Menu",lc_Params[l][0]))     gp_Tim_Par->par_tempo_Menu=atol(lc_Params[l][1]);
     if(!strcmp("gp_Tim_Par->par_tempo_Ir",lc_Params[l][0]))       gp_Tim_Par->par_tempo_Ir=atol(lc_Params[l][1]);
     if(!strcmp("gp_Tim_Par->par_tempo_Termios",lc_Params[l][0]))  gp_Tim_Par->par_tempo_Termios=atol(lc_Params[l][1]);
     if(!strcmp("gp_Tim_Par->par_tempo_Clavier",lc_Params[l][0]))  gp_Tim_Par->par_tempo_Clavier=atol(lc_Params[l][1]);
     if(!strcmp("gp_Tim_Par->par_tempo_Capteurs",lc_Params[l][0])) gp_Tim_Par->par_tempo_Capteurs=atol(lc_Params[l][1]);
     if(!strcmp("gp_Tim_Par->par_tempo_Lcd_Loop",lc_Params[l][0])) gp_Tim_Par->par_tempo_Lcd_Loop=atol(lc_Params[l][1]);
     if(!strcmp("gp_Tim_Par->par_tempo_Lcd_Disp",lc_Params[l][0])) gp_Tim_Par->par_tempo_Lcd_Disp=atol(lc_Params[l][1]);

     if(!strcmp("gp_Pid_Par->par_pid_ech",lc_Params[l][0])) gp_Pid_Par->par_pid_ech=atof(lc_Params[l][1]);
     if(!strcmp("gp_Pid_Par->par_pid_kp", lc_Params[l][0])) gp_Pid_Par->par_pid_kp =atof(lc_Params[l][1]);
     if(!strcmp("gp_Pid_Par->par_pid_ki", lc_Params[l][0])) gp_Pid_Par->par_pid_kp =atof(lc_Params[l][1]);
     if(!strcmp("gp_Pid_Par->par_pid_kd", lc_Params[l][0])) gp_Pid_Par->par_pid_kd =atof(lc_Params[l][1]);

     if(!strcmp("gp_Gpi_Par->par_led_etat",lc_Params[l][0]))  gp_Gpi_Par->par_led_etat=atoi(lc_Params[l][1]);

     if(!strcmp("gp_Dev_Par->par_use_Controler",lc_Params[l][0]))  gp_Dev_Par->par_use_Controler=atoi(lc_Params[l][1]);
     if(!strcmp("gp_Dev_Par->par_use_Capteurs",lc_Params[l][0]))   gp_Dev_Par->par_use_Capteurs=atoi(lc_Params[l][1]);
     if(!strcmp("gp_Dev_Par->par_use_Raquette",lc_Params[l][0]))   gp_Dev_Par->par_use_Raquette=atoi(lc_Params[l][1]);
     if(!strcmp("gp_Dev_Par->par_use_Bluetooth",lc_Params[l][0]))  gp_Dev_Par->par_use_Bluetooth=atoi(lc_Params[l][1]);
     if(!strcmp("gp_Dev_Par->par_use_Infrared",lc_Params[l][0])) gp_Dev_Par->par_use_Infrared=atoi(lc_Params[l][1]);
     if(!strcmp("gp_Dev_Par->par_use_Keyboard",lc_Params[l][0]))   gp_Dev_Par->par_use_Keyboard=atoi(lc_Params[l][1]);
     if(!strcmp("gp_Dev_Par->par_use_Lcd",lc_Params[l][0]))        gp_Dev_Par->par_use_Lcd=atoi(lc_Params[l][1]);

     if(!strcmp("gp_Cal_Par->par_alt_rev",lc_Params[l][0]))      gp_Cal_Par->par_alt_rev=atoi(lc_Params[l][1]);
     if(!strcmp("gp_Cal_Par->par_azi_rev",lc_Params[l][0]))      gp_Cal_Par->par_azi_rev=atoi(lc_Params[l][1]);
     
     if(!strcmp("gp_Cal_Par->par_alt_f",lc_Params[l][0]))        gp_Cal_Par->par_alt_f=atol(lc_Params[l][1]);
     if(!strcmp("gp_Cal_Par->par_azi_f",lc_Params[l][0]))        gp_Cal_Par->par_azi_f=atol(lc_Params[l][1]);
     
     if(!strcmp("gp_Cal_Par->par_alt_n",lc_Params[l][0]))        gp_Cal_Par->par_alt_n=atoi(lc_Params[l][1]);
     if(!strcmp("gp_Cal_Par->par_azi_n",lc_Params[l][0]))        gp_Cal_Par->par_azi_n=atoi(lc_Params[l][1]);
     
     if(!strcmp("gp_Cal_Par->par_alt_red_tot",lc_Params[l][0]))        gp_Cal_Par->par_alt_red_tot=atof(lc_Params[l][1]);
     if(!strcmp("gp_Cal_Par->par_azi_red_tot",lc_Params[l][0]))        gp_Cal_Par->par_azi_red_tot=atof(lc_Params[l][1]);
     
     if(!strcmp("GPIO_RAQ_O",lc_Params[l][0]))   GPIO_RAQ_O=atoi(lc_Params[l][1]);
     if(!strcmp("GPIO_RAQ_E",lc_Params[l][0]))   GPIO_RAQ_E=atoi(lc_Params[l][1]);
     if(!strcmp("GPIO_RAQ_S",lc_Params[l][0]))   GPIO_RAQ_S=atoi(lc_Params[l][1]);
     if(!strcmp("GPIO_RAQ_N",lc_Params[l][0]))   GPIO_RAQ_N=atoi(lc_Params[l][1]);
     if(!strcmp("GPIO_RAQ_V",lc_Params[l][0]))   GPIO_RAQ_V=atoi(lc_Params[l][1]);
     
     if(!strcmp("GPIO_KEY_L1",lc_Params[l][0]))   GPIO_KEY_L1=atoi(lc_Params[l][1]);
     if(!strcmp("GPIO_KEY_L2",lc_Params[l][0]))   GPIO_KEY_L2=atoi(lc_Params[l][1]);
     if(!strcmp("GPIO_KEY_L3",lc_Params[l][0]))   GPIO_KEY_L3=atoi(lc_Params[l][1]);
     if(!strcmp("GPIO_KEY_L4",lc_Params[l][0]))   GPIO_KEY_L4=atoi(lc_Params[l][1]);
     
     if(!strcmp("GPIO_KEY_C1",lc_Params[l][0]))   GPIO_KEY_C1=atoi(lc_Params[l][1]);
     if(!strcmp("GPIO_KEY_C2",lc_Params[l][0]))   GPIO_KEY_C2=atoi(lc_Params[l][1]);
     if(!strcmp("GPIO_KEY_C3",lc_Params[l][0]))   GPIO_KEY_C3=atoi(lc_Params[l][1]);
     if(!strcmp("GPIO_KEY_C4",lc_Params[l][0]))   GPIO_KEY_C4=atoi(lc_Params[l][1]);

     if(!strcmp("LIEU_LONGITUDE",lc_Params[l][0]))     LIEU_LONGITUDE=atof(lc_Params[l][1]);
     if(!strcmp("LIEU_LATITUDE",lc_Params[l][0]))      LIEU_LATITUDE=atof(lc_Params[l][1]);
     if(!strcmp("LIEU_ALTITUDE",lc_Params[l][0]))      LIEU_ALTITUDE=atof(lc_Params[l][1]);

     // devices de altitude

     if(!strcmp("gp_Cal_Par->par_alt_red_1",lc_Params[l][0]))       gp_Cal_Par->par_alt_red_1 = atof(lc_Params[l][1]);         
     if(!strcmp("gp_Cal_Par->par_alt_red_2",lc_Params[l][0]))       gp_Cal_Par->par_alt_red_2 = atof(lc_Params[l][1]);
     if(!strcmp("gp_Cal_Par->par_alt_red_3",lc_Params[l][0]))       gp_Cal_Par->par_alt_red_3 = atof(lc_Params[l][1]);         
     if(!strcmp("gp_Cal_Par->par_alt_red_4",lc_Params[l][0]))       gp_Cal_Par->par_alt_red_4 = atof(lc_Params[l][1]);  
     if(!strcmp("gp_Cal_Par->par_alt_red_5",lc_Params[l][0]))       gp_Cal_Par->par_alt_red_5 = atof(lc_Params[l][1]);
     if(!strcmp("gp_Cal_Par->par_alt_red_6",lc_Params[l][0]))       gp_Cal_Par->par_alt_red_6 = atof(lc_Params[l][1]);       
     if(!strcmp("gp_Cal_Par->par_alt_red_7",lc_Params[l][0]))       gp_Cal_Par->par_alt_red_7 = atof(lc_Params[l][1]);

     if(!strcmp("gp_Cal_Par->par_alt_acc",lc_Params[l][0]))      gp_Cal_Par->par_alt_acc= atof(lc_Params[l][1])      ;

     if(!strcmp("gp_Gpi_Par_Old->GPIO_DIR_ALT",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_DIR_ALT=atoi(lc_Params[l][1]);  
     if(!strcmp("gp_Gpi_Par_Old->GPIO_CLK_ALT",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_CLK_ALT=atoi(lc_Params[l][1]);  
     if(!strcmp("gp_Gpi_Par_Old->GPIO_SLP_ALT",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_SLP_ALT=atoi(lc_Params[l][1]);  
     if(!strcmp("gp_Gpi_Par_Old->GPIO_RST_ALT",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_RST_ALT=atoi(lc_Params[l][1]);  
     if(!strcmp("gp_Gpi_Par_Old->GPIO_MMM_ALT",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_MMM_ALT=atoi(lc_Params[l][1]);  
     if(!strcmp("gp_Gpi_Par_Old->GPIO_ENA_ALT",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_ENA_ALT=atoi(lc_Params[l][1]);  
     if(!strcmp("gp_Gpi_Par_Old->GPIO_M2_ALT",lc_Params[l][0]))  gp_Gpi_Par_Old->GPIO_M2_ALT=atoi(lc_Params[l][1]);  
     if(!strcmp("gp_Gpi_Par_Old->GPIO_M1_ALT",lc_Params[l][0]))  gp_Gpi_Par_Old->GPIO_M1_ALT=atoi(lc_Params[l][1]);  
     if(!strcmp("gp_Gpi_Par_Old->GPIO_M0_ALT",lc_Params[l][0]))  gp_Gpi_Par_Old->GPIO_M0_ALT=atoi(lc_Params[l][1]);  

     if(!strcmp("MCP_DIR_ALT",lc_Params[l][0]))  MCP_DIR_ALT=atoi(lc_Params[l][1])  ;  
     if(!strcmp("MCP_CLK_ALT",lc_Params[l][0]))  MCP_CLK_ALT=atoi(lc_Params[l][1])  ;  
     if(!strcmp("MCP_SLP_ALT",lc_Params[l][0]))  MCP_SLP_ALT=atoi(lc_Params[l][1])  ;  
     if(!strcmp("MCP_RST_ALT",lc_Params[l][0]))  MCP_RST_ALT=atoi(lc_Params[l][1])  ;  
     if(!strcmp("MCP_M2_ALT",lc_Params[l][0]))   MCP_M2_ALT=atoi(lc_Params[l][1])   ;  
     if(!strcmp("MCP_M1_ALT",lc_Params[l][0]))   MCP_M1_ALT=atoi(lc_Params[l][1])   ;  
     if(!strcmp("MCP_M0_ALT",lc_Params[l][0]))   MCP_M0_ALT=atoi(lc_Params[l][1])   ;  
     
     // devices de azimut

     if(!strcmp("gp_Cal_Par->par_azi_red1",lc_Params[l][0]))       gp_Cal_Par->par_azi_red1 = atof(lc_Params[l][1])      ; 
     if(!strcmp("gp_Cal_Par->par_azi_red2",lc_Params[l][0]))       gp_Cal_Par->par_azi_red2 = atof(lc_Params[l][1])      ; 
     if(!strcmp("gp_Cal_Par->par_azi_red3",lc_Params[l][0]))       gp_Cal_Par->par_azi_red3 = atof(lc_Params[l][1])      ; 
     if(!strcmp("gp_Cal_Par->par_azi_red4",lc_Params[l][0]))       gp_Cal_Par->par_azi_red4 = atof(lc_Params[l][1])      ; 
     if(!strcmp("gp_Cal_Par->par_azi_red5",lc_Params[l][0]))       gp_Cal_Par->par_azi_red5 = atof(lc_Params[l][1])      ; 
     if(!strcmp("gp_Cal_Par->par_azi_red6",lc_Params[l][0]))       gp_Cal_Par->par_azi_red6 = atof(lc_Params[l][1])      ; 
     if(!strcmp("gp_Cal_Par->par_azi_red7",lc_Params[l][0]))       gp_Cal_Par->par_azi_red7 = atof(lc_Params[l][1])      ; 

     if(!strcmp("gp_Cal_Par->par_azi_acc",lc_Params[l][0]))      gp_Cal_Par->par_azi_acc= atof(lc_Params[l][1])      ;

     if(!strcmp("gp_Gpi_Par_Old->GPIO_DIR_AZI",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_DIR_AZI=atoi(lc_Params[l][1]) ; 
     if(!strcmp("gp_Gpi_Par_Old->GPIO_CLK_AZI",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_CLK_AZI=atoi(lc_Params[l][1]) ; 
     if(!strcmp("gp_Gpi_Par_Old->GPIO_SLP_AZI",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_SLP_AZI=atoi(lc_Params[l][1]) ;
     if(!strcmp("gp_Gpi_Par_Old->GPIO_RST_AZI",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_RST_AZI=atoi(lc_Params[l][1]);  
     if(!strcmp("gp_Gpi_Par_Old->GPIO_MMM_AZI",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_MMM_AZI=atoi(lc_Params[l][1]);  
     if(!strcmp("gp_Gpi_Par_Old->GPIO_ENA_AZI",lc_Params[l][0])) gp_Gpi_Par_Old->GPIO_ENA_AZI=atoi(lc_Params[l][1]);
     if(!strcmp("gp_Gpi_Par_Old->GPIO_M2_AZI",lc_Params[l][0]))  gp_Gpi_Par_Old->GPIO_M2_AZI=atoi(lc_Params[l][1])  ;
     if(!strcmp("gp_Gpi_Par_Old->GPIO_M1_AZI",lc_Params[l][0]))  gp_Gpi_Par_Old->GPIO_M1_AZI=atoi(lc_Params[l][1])  ; 
     if(!strcmp("gp_Gpi_Par_Old->GPIO_M0_AZI",lc_Params[l][0]))  gp_Gpi_Par_Old->GPIO_M0_AZI=atoi(lc_Params[l][1])  ; 

     if(!strcmp("MCP_DIR_AZI",lc_Params[l][0]))  MCP_DIR_AZI=atoi(lc_Params[l][1])   ;
     if(!strcmp("MCP_CLK_AZI",lc_Params[l][0]))  MCP_CLK_AZI=atoi(lc_Params[l][1])   ;
     if(!strcmp("MCP_SLP_AZI",lc_Params[l][0]))  MCP_SLP_AZI=atoi(lc_Params[l][1])   ;
     if(!strcmp("MCP_RST_AZI",lc_Params[l][0]))  MCP_RST_AZI=atoi(lc_Params[l][1])   ; 
     if(!strcmp("MCP_M2_AZI",lc_Params[l][0]))   MCP_M2_AZI=atoi(lc_Params[l][1])    ;
     if(!strcmp("MCP_M1_AZI",lc_Params[l][0]))   MCP_M1_AZI=atoi(lc_Params[l][1])    ;
     if(!strcmp("MCP_M0_AZI",lc_Params[l][0]))   MCP_M0_AZI=atoi(lc_Params[l][1])   ;

     // devices de azimut et altitude (qui concernent les 2 en mm temps)
     
     if(!strcmp("gp_Cal_Par->par_altaz_slow_forward",lc_Params[l][0])) gp_Cal_Par->par_altaz_slow_forward= atof(lc_Params[l][1])      ;
     if(!strcmp("gp_Cal_Par->par_altaz_slow_rewind",lc_Params[l][0]))  gp_Cal_Par->par_altaz_slow_rewind= atof(lc_Params[l][1])      ;

     if(!strcmp("gp_Cal_Par->par_altaz_fast_forward",lc_Params[l][0])) gp_Cal_Par->par_altaz_fast_forward= atof(lc_Params[l][1])      ;
     if(!strcmp("gp_Cal_Par->par_altaz_fast_rewind",lc_Params[l][0]))  gp_Cal_Par->par_altaz_fast_rewind= atof(lc_Params[l][1])      ;

     // chemins des repertoires et fichiers (2021)

    if(!strcmp("gp_Con_Par->par_rep_cat",lc_Params[l][0]))  strcpy( gp_Con_Par->par_rep_cat, lc_Params[l][1]) ;
    if(!strcmp("gp_Con_Par->par_rep_cfg",lc_Params[l][0]))  strcpy( gp_Con_Par->par_rep_cfg, lc_Params[l][1]) ;
    if(!strcmp("gp_Con_Par->par_rep_log",lc_Params[l][0]))  strcpy( gp_Con_Par->par_rep_log, lc_Params[l][1]) ;
    if(!strcmp("gp_Con_Par->par_rep_in",lc_Params[l][0]))   strcpy( gp_Con_Par->par_rep_in, lc_Params[l][1]) ;
    if(!strcmp("gp_Con_Par->par_fic_log",lc_Params[l][0]))  strcpy( gp_Con_Par->par_fic_log, lc_Params[l][1]) ;
    if(!strcmp("gp_Con_Par->par_fic_pid",lc_Params[l][0]))  strcpy( gp_Con_Par->par_fic_pid, lc_Params[l][1]) ;
    if(!strcmp("gp_Con_Par->par_fic_led",lc_Params[l][0]))  strcpy( gp_Con_Par->par_fic_led, lc_Params[l][1]) ;
    if(!strcmp("gp_Con_Par->par_fic_dat",lc_Params[l][0])) strcpy( gp_Con_Par->par_fic_dat, lc_Params[l][1]) ;
    if(!strcmp("gp_Con_Par->par_fic_hhm",lc_Params[l][0])) strcpy( gp_Con_Par->par_fic_hhm, lc_Params[l][1]) ;  

    if(!strcmp("gp_Con_Par->par_rep_scr",lc_Params[l][0]))    strcpy( gp_Con_Par->par_rep_scr, lc_Params[l][1]) ;  
    if(!strcmp("gp_Con_Par->par_src_ker",lc_Params[l][0])) strcpy( gp_Con_Par->par_src_ker, lc_Params[l][1]) ;  
  }

  //if ( gp_Cal_Par->par_alt_red_tot == 0 ) gp_Cal_Par->par_alt_red_tot = gp_Cal_Par->par_alt_red_1 * gp_Cal_Par->par_alt_red_2 * gp_Cal_Par->par_alt_red_3 * gp_Cal_Par->par_alt_red_4 ;
  //if ( gp_Cal_Par->par_azi_red_tot == 0 ) gp_Cal_Par->par_azi_red_tot = gp_Cal_Par->par_azi_red1 * gp_Cal_Par->par_azi_red2 * gp_Cal_Par->par_azi_red3 * gp_Cal_Par->par_azi_red4 ;
  
  /* Attention gp_Cal_Par->par_alt_red_4 (micro pas) est traite independamment */

  if ( gp_Cal_Par->par_alt_red_tot == 0 ) {
    gp_Cal_Par->par_alt_red_tot = gp_Cal_Par->par_alt_red_1 * gp_Cal_Par->par_alt_red_2 * gp_Cal_Par->par_alt_red_3 * gp_Cal_Par->par_alt_red_5 * gp_Cal_Par->par_alt_red_6 * gp_Cal_Par->par_alt_red_7 ;
  }
  /* Attention gp_Cal_Par->par_alt_red_4 (micro pas) est traite independamment */

  if ( gp_Cal_Par->par_azi_red_tot == 0 ) {
    gp_Cal_Par->par_azi_red_tot = gp_Cal_Par->par_azi_red1 * gp_Cal_Par->par_azi_red2 * gp_Cal_Par->par_azi_red3 * gp_Cal_Par->par_azi_red5 * gp_Cal_Par->par_azi_red6 * gp_Cal_Par->par_azi_red7 ;
  }
}

/*****************************************************************************************
* @fn     : CONFIG_PARAMETRES_AFFICHER
* @author : s.gravois
* @brief  : Cette fonction affiche les parametres 
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-05-30 ajout gp_Tim_Par->par_tempo_LCD_xxx 
* @date   : 2022-06-17 ajout gp_Pid_Par->par_pid_ech
* @todo   : 
*****************************************************************************************/

void   CONFIG_PARAMETRES_AFFICHER(void) {

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("gp_Tim_Par->par_tempo_Raq = %ld",  gp_Tim_Par->par_tempo_Raq);
  Trace("gp_Tim_Par->par_tempo_Menu = %ld",  gp_Tim_Par->par_tempo_Menu);
  Trace("gp_Tim_Par->par_tempo_Ir = %ld",  gp_Tim_Par->par_tempo_Ir);
  Trace("gp_Tim_Par->par_tempo_Termios = %ld",  gp_Tim_Par->par_tempo_Termios);
  Trace("gp_Tim_Par->par_tempo_Clavier = %ld",  gp_Tim_Par->par_tempo_Clavier);
  Trace("gp_Tim_Par->par_tempo_Capteurs = %ld",  gp_Tim_Par->par_tempo_Capteurs);
  Trace("gp_Tim_Par->par_tempo_Lcd_Loop = %ld",  gp_Tim_Par->par_tempo_Lcd_Loop);
  Trace("gp_Tim_Par->par_tempo_Lcd_Disp = %ld",  gp_Tim_Par->par_tempo_Lcd_Disp);

  Trace("gp_Pid_Par->par_pid_ech = %f",  gp_Pid_Par->par_pid_ech);
  Trace("gp_Pid_Par->par_pid_ki = %f",  gp_Pid_Par->par_pid_ki);
  Trace("gp_Pid_Par->par_pid_kp = %f",  gp_Pid_Par->par_pid_kp);
  Trace("gp_Pid_Par->par_pid_kd = %f",  gp_Pid_Par->par_pid_kd);

  Trace1("gp_Dev_Par->par_use_Controler = %d",  gp_Dev_Par->par_use_Controler);
  Trace1("gp_Dev_Par->par_use_Capteurs = %d",  gp_Dev_Par->par_use_Capteurs);
  Trace1("gp_Dev_Par->par_use_Bluetooth = %d",  gp_Dev_Par->par_use_Bluetooth);
  Trace1("gp_Dev_Par->par_use_Infrared = %d",  gp_Dev_Par->par_use_Infrared);
  Trace1("gp_Dev_Par->par_use_Raquette = %d",  gp_Dev_Par->par_use_Raquette);
  Trace1("gp_Dev_Par->par_use_Keyboard = %d",  gp_Dev_Par->par_use_Keyboard);
  Trace1("gp_Dev_Par->par_use_Lcd = %d",  gp_Dev_Par->par_use_Lcd);

  Trace1("gp_Ast_Par->par_default_object = %s",  gp_Ast_Par->par_default_object );
  Trace1("gp_Con_Par->par_default_menu = %s",  c_Menus[gp_Con_Par->par_default_menu]);

  Trace1("LIEU_LATITUDE  = %f",          LIEU_LATITUDE );
  Trace1("LIEU_LONGITUDE = %f",          LIEU_LONGITUDE );
  Trace1("LIEU_ALTITUDE  = %f",          LIEU_ALTITUDE );

  Trace1("gp_Gpi_Par->par_led_etat = %d", gp_Gpi_Par->par_led_etat );

  Trace1("gp_Cal_Par->par_alt_red_1 = %f",       gp_Cal_Par->par_alt_red_1);         
  Trace1("gp_Cal_Par->par_alt_red_2 = %f",       gp_Cal_Par->par_alt_red_2);
  Trace1("gp_Cal_Par->par_alt_red_3 = %f",       gp_Cal_Par->par_alt_red_3);         
  Trace1("gp_Cal_Par->par_alt_red_4 = %f",       gp_Cal_Par->par_alt_red_4);         
  Trace1("gp_Cal_Par->par_alt_rev = %d",      gp_Cal_Par->par_alt_rev);
  Trace1("gp_Cal_Par->par_alt_acc = %f",      gp_Cal_Par->par_alt_acc);

  Trace1("gp_Cal_Par->par_azi_red1 = %f",       gp_Cal_Par->par_azi_red1)      ; 
  Trace1("gp_Cal_Par->par_azi_red2 = %f",       gp_Cal_Par->par_azi_red2)      ; 
  Trace1("gp_Cal_Par->par_azi_red3 = %f",       gp_Cal_Par->par_azi_red3)      ; 
  Trace1("gp_Cal_Par->par_azi_red4 = %f",       gp_Cal_Par->par_azi_red4)      ; 
  Trace1("gp_Cal_Par->par_azi_rev = %d",      gp_Cal_Par->par_azi_rev)     ;
  Trace1("gp_Cal_Par->par_azi_acc = %f",      gp_Cal_Par->par_azi_acc)     ; 

  Trace1("gp_Con_Par->par_rep_cat = %s", gp_Con_Par->par_rep_cat)  ;
  Trace1("gp_Con_Par->par_rep_cfg = %s", gp_Con_Par->par_rep_cfg)  ; 
  Trace1("gp_Con_Par->par_rep_log = %s", gp_Con_Par->par_rep_log)  ; 
  Trace1("gp_Con_Par->par_rep_in = %s", gp_Con_Par->par_rep_in)  ; 
  Trace1("gp_Con_Par->par_fic_log = %s", gp_Con_Par->par_fic_log)  ; 
  Trace1("gp_Con_Par->par_fic_pid = %s", gp_Con_Par->par_fic_pid)  ; 
  Trace1("gp_Con_Par->par_fic_led = %s", gp_Con_Par->par_fic_led)  ; 
  Trace1("gp_Con_Par->par_fic_dat = %s", gp_Con_Par->par_fic_dat)  ; 
  Trace1("gp_Con_Par->par_fic_hhm = %s", gp_Con_Par->par_fic_hhm)  ;  

  Trace1("gp_Con_Par->par_rep_scr = %s", gp_Con_Par->par_rep_scr)  ; 
  Trace1("gp_Con_Par->par_src_ker = %s", gp_Con_Par->par_src_ker)  ; 

  Trace1("gp_Gpi_Par->par_alt = %s", gp_Gpi_Par->par_alt)  ;  
  Trace1("gp_Gpi_Par->par_azi = %s", gp_Gpi_Par->par_azi)  ;  
  Trace1("gp_Gpi_Par->par_mas = %s", gp_Gpi_Par->par_mas)  ;  
  Trace1("gp_Gpi_Par->par_fre_pwm = %s", gp_Gpi_Par->par_fre_pwm)  ;  

  Trace2("anciennes variables\n");
  Trace2("GPIO_RAQ_O   = %d",  GPIO_RAQ_O);
  Trace2("GPIO_RAQ_E   = %d",  GPIO_RAQ_E);
  Trace2("GPIO_RAQ_S   = %d",  GPIO_RAQ_S);
  Trace2("GPIO_RAQ_N   = %d",  GPIO_RAQ_N);
  Trace2("GPIO_RAQ_V   = %d",  GPIO_RAQ_V);
  Trace2("GPIO_KEY_L1  = %d",  GPIO_KEY_L1);
  Trace2("GPIO_KEY_L2  = %d",  GPIO_KEY_L2);
  Trace2("GPIO_KEY_L3  = %d",  GPIO_KEY_L3);
  Trace2("GPIO_KEY_L4  = %d",  GPIO_KEY_L4);
  Trace2("GPIO_KEY_C1  = %d",  GPIO_KEY_C1);
  Trace2("GPIO_KEY_C2  = %d",  GPIO_KEY_C2);
  Trace2("GPIO_KEY_C3  = %d",  GPIO_KEY_C3);
  Trace2("GPIO_KEY_C4  = %d",  GPIO_KEY_C4);
  Trace2("gp_Gpi_Par_Old->GPIO_DIR_ALT = %d", gp_Gpi_Par_Old->GPIO_DIR_ALT);  
  Trace2("gp_Gpi_Par_Old->GPIO_CLK_ALT = %d", gp_Gpi_Par_Old->GPIO_CLK_ALT);  
  Trace2("gp_Gpi_Par_Old->GPIO_SLP_ALT = %d", gp_Gpi_Par_Old->GPIO_SLP_ALT);  
  Trace2("gp_Gpi_Par_Old->GPIO_RST_ALT = %d", gp_Gpi_Par_Old->GPIO_RST_ALT);  
  Trace2("gp_Gpi_Par_Old->GPIO_MMM_ALT = %d", gp_Gpi_Par_Old->GPIO_MMM_ALT);  
  Trace2("gp_Gpi_Par_Old->GPIO_ENA_ALT = %d", gp_Gpi_Par_Old->GPIO_ENA_ALT);  
  Trace2("gp_Gpi_Par_Old->GPIO_M2_ALT = %d",  gp_Gpi_Par_Old->GPIO_M2_ALT)  ;  
  Trace2("gp_Gpi_Par_Old->GPIO_M1_ALT = %d",  gp_Gpi_Par_Old->GPIO_M1_ALT)  ;  
  Trace2("gp_Gpi_Par_Old->GPIO_M0_ALT = %d",  gp_Gpi_Par_Old->GPIO_M0_ALT)  ;  
  Trace2("gp_Gpi_Par_Old->GPIO_DIR_AZI = %d", gp_Gpi_Par_Old->GPIO_DIR_AZI) ; 
  Trace2("gp_Gpi_Par_Old->GPIO_CLK_AZI = %d", gp_Gpi_Par_Old->GPIO_CLK_AZI) ; 
  Trace2("gp_Gpi_Par_Old->GPIO_SLP_AZI = %d", gp_Gpi_Par_Old->GPIO_SLP_AZI) ;
  Trace2("gp_Gpi_Par_Old->GPIO_RST_AZI = %d", gp_Gpi_Par_Old->GPIO_RST_AZI);  
  Trace2("gp_Gpi_Par_Old->GPIO_MMM_AZI = %d", gp_Gpi_Par_Old->GPIO_MMM_AZI);  
  Trace2("gp_Gpi_Par_Old->GPIO_ENA_AZI = %d", gp_Gpi_Par_Old->GPIO_ENA_AZI);  
  Trace2("gp_Gpi_Par_Old->GPIO_M2_AZI = %d",  gp_Gpi_Par_Old->GPIO_M2_AZI)  ;
  Trace2("gp_Gpi_Par_Old->GPIO_M1_AZI = %d",  gp_Gpi_Par_Old->GPIO_M1_AZI)  ; 
  Trace2("gp_Gpi_Par_Old->GPIO_M0_AZI = %d",  gp_Gpi_Par_Old->GPIO_M0_AZI)  ; 
  Trace2("=====================================================\n");
  Trace2("MCP_DIR_AZI = %d",  MCP_DIR_AZI)   ;
  Trace2("MCP_CLK_AZI = %d",  MCP_CLK_AZI)   ;
  Trace2("MCP_SLP_AZI = %d",  MCP_SLP_AZI)   ;
  Trace2("MCP_RST_AZI = %d",  MCP_RST_AZI)   ; 
  Trace2("MCP_M2_AZI = %d",   MCP_M2_AZI)    ;
  Trace2("MCP_M1_AZI = %d",   MCP_M1_AZI)    ;
  Trace2("MCP_M0_AZI = %d",   MCP_M0_AZI)   ;
  Trace2("MCP_DIR_ALT = %d",  MCP_DIR_ALT)  ;  
  Trace2("MCP_CLK_ALT = %d",  MCP_CLK_ALT)  ;  
  Trace2("MCP_SLP_ALT = %d",  MCP_SLP_ALT)  ;  
  Trace2("MCP_RST_ALT = %d",  MCP_RST_ALT)  ;  
  Trace2("MCP_M2_ALT = %d",   MCP_M2_ALT)   ;  
  Trace2("MCP_M1_ALT = %d",   MCP_M1_ALT)   ;  
  Trace2("MCP_M0_ALT = %d",   MCP_M0_ALT)   ;  
}
/*****************************************************************************************
* @fn     : CONFIG_GETCWD
* @author : s.gravois
* @brief  : Cette fonction obtient le nom du repertoire courant d execution
* @param  : char * c_getcwd
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : (obsolete) re-verifier mais a priori BUG lors de l'appel 
*           => appel direct a getcwd dans astro.c
*****************************************************************************************/

int CONFIG_GETCWD(char * c_getcwd) {

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if (getcwd(c_getcwd, sizeof(c_getcwd)) != NULL) {
     Trace("Current working dir: %s\n", c_getcwd);
  } else {
     perror("getcwd() error");
     return 1;
  }
  return 0 ;
} 
/*****************************************************************************************
* @fn     : CONFIG_READ
* @author : s.gravois
* @brief  : Cette fonction lit les parametres  dans le fichier de configuration
* @param  : lc_Params[CONFIG_DATAS_NB_COLONNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : voir si un passage par librairie JSON plus pratique (comme pour mesDep)
*****************************************************************************************/

int CONFIG_READ(char lc_Params[CONFIG_DATAS_NB_COLONNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]) {

  FILE * fin ;
  char buf   [CONFIG_TAILLE_BUFFER_256] ;
  char buffer[CONFIG_TAILLE_BUFFER_256] ;
  char c ;
  int i,C,L ;

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // FIXME : initialisation du tableau de valeurs lues dans le fichier = lc_Params

  Trace1(" CONFIG_DATAS_NB_COLONNES   = %d",   CONFIG_DATAS_NB_COLONNES) ;
  Trace1(" CONFIG_DATAS_NB_COLONNES = %d", CONFIG_DATAS_NB_COLONNES) ;

  for(L=0;L<CONFIG_DATAS_NB_COLONNES;L++) {
    for(C=0;C<CONFIG_DATAS_NB_COLONNES;C++) { 
      memset(lc_Params[L][C],CALCUL_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256-1);
    }
  }
  
  // FIXME : construction du chemin du fichier de configuration
  // FIXME : la variable gp_Con_Par->par_rep_home doit etre lue auparavant (getcwd) (2021)

  memset(buf,CALCUL_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256-1);
  sprintf(buf,"%s/%s",gp_Con_Par->par_rep_home, CONFIG_FIC_CFG) ;
  
  // FIXME : ouverture du fichier de configuration


  if ( (fin=fopen(buf,"r")) == NULL) {
   memset(buffer,CALCUL_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256);
   sprintf(buffer,"Pbme ouverture %s",buf) ;
   //LOG(buffer) ;
   Trace("probleme ouverture 4 %s",buf) ; 
   exit(2) ;
  }
  else Trace("open %s ok", buf) ;

  memset(buf,CALCUL_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256-1);
  i=0;
  L=0;
  C=0;
  
  // FIXME : lecture du fichier de configuration 

  while(1) {
   
   c=fgetc(fin) ;
   if (CONFIG_FIN_FICHIER(c)) break ;
   if (CONFIG_FIN_LIGNE(c) && i==0 ) continue ;
   if (CONFIG_FIN_MOT(c) && i==0 ) continue ;
   if (CONFIG_DEB_COM(c)) {
     while(!CONFIG_FIN_LIGNE(fgetc(fin)));
     L++;
     C=0;
   }
   if (CONFIG_FIN_MOT(c)||(CONFIG_FIN_LIGNE(c)&& i!=0)||i==CONFIG_TAILLE_BUFFER_256) {
    
    if (CONFIG_FIN_MOT(c)) {
      memset(lc_Params[L][C],CALCUL_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256);
      strcpy(lc_Params[L][C],buf);

      Trace1("lc_Params fin mot[%d][%d]=(%s)",L,C,lc_Params[L][C] );
      
      C++;
    }
    if ((CONFIG_FIN_LIGNE(c)&& i!=0)||i==CONFIG_TAILLE_BUFFER_256) {
      memset(lc_Params[L][C],CALCUL_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256);
      strcpy(lc_Params[L][C],buf);
      
      Trace1("lc_Params fin lig[%d][%d]=(%s)",L,C,lc_Params[L][C] );
      
      L++;
      C=0;
    }
    memset(buf,CALCUL_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256);
    i=0;
    
   }
   if (CONFIG_FORMAT_ADMIS(c)){buf[i]=(char)c;i++;}
  }
  fclose(fin);

  return 0 ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_DATAS
* @author : s.gravois
* @brief  : Cette fonction affiche le tabeau de parametres lu a partir du fichier de config
* @param  : char lc_Params[CONFIG_DATAS_NB_COLONNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_AFFICHER_DATAS(char lc_Params[CONFIG_DATAS_NB_COLONNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]) {
  int L, C ;  

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  for(L=0;L<CONFIG_DATAS_NB_COLONNES;L++) {
    for(C=0;C<CONFIG_DATAS_NB_COLONNES;C++) { 
      if ( C>0 ) {
         Trace2("") ;
      }
      if (strlen(lc_Params[L][C])) {
        Trace2(" %s " , lc_Params[L][C]) ;
      }
    }
    Trace2("") ;
  }
}


/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_MODE_LONG
* @author : s.gravois
* @brief  : Cette fonction affiche les informations relatives a l as observee
* @param  : STRUCT_ASTRE *gp_Ast
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
*****************************************************************************************/

void CONFIG_AFFICHER_MODE_LONG(void) {
  
  const char * c_nom  = gp_Ast->nom ;
  const char * c_type = c_Astre_Type [ gp_Ast->en_type ] ;
  const char * c_mode = c_Calculs_Mode[ gp_Ast->en_mode ] ;

  char  c_hhmmss_agh0[ 16] ;
  char  c_hhmmss_agh1[ 16] ;
  char  c_hhmmss_agh2[ 16] ;

  char  c_hhmmss_azi0[ 16] ;
  char  c_hhmmss_azi1[ 16] ;
  char  c_hhmmss_azi2[ 16] ;

  char  c_hhmmss_agh[ 16] ;
  char  c_hhmmss_asc[ 16] ;

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset( c_hhmmss_agh0, 0, sizeof(c_hhmmss_agh0) ) ;
  memset( c_hhmmss_agh1, 0, sizeof(c_hhmmss_agh1) ) ;
  memset( c_hhmmss_agh2, 0, sizeof(c_hhmmss_agh2) ) ;
  memset( c_hhmmss_agh, 0, sizeof(c_hhmmss_agh) ) ;
  memset( c_hhmmss_asc, 0, sizeof(c_hhmmss_asc) ) ;

  sprintf( c_hhmmss_agh, "%dh%dm%ds",   gp_Ast->AGHt.HH,  gp_Ast->AGHt.MM,  gp_Ast->AGHt.SS  ) ;
  sprintf( c_hhmmss_asc,  "%dh%dm%ds",  gp_Ast->ASCt.HH,   gp_Ast->ASCt.MM,   gp_Ast->ASCt.SS  ) ;

  sprintf( c_hhmmss_agh0, "%dh%dm%ds", gp_Ast->AGH0t.HH, gp_Ast->AGH0t.MM, gp_Ast->AGH0t.SS  ) ;
  sprintf( c_hhmmss_agh1, "%dh%dm%ds", gp_Ast->AGH1t.HH, gp_Ast->AGH1t.MM, gp_Ast->AGH1t.SS  ) ;
  sprintf( c_hhmmss_agh2, "%dh%dm%ds", gp_Ast->AGH2t.HH, gp_Ast->AGH2t.MM, gp_Ast->AGH2t.SS  ) ;

  sprintf( c_hhmmss_azi0, "%dh%dm%ds", gp_Ast->AZI0t.HH, gp_Ast->AZI0t.MM, gp_Ast->AZI0t.SS  ) ;
  sprintf( c_hhmmss_azi1, "%dh%dm%ds", gp_Ast->AZI1t.HH, gp_Ast->AZI1t.MM, gp_Ast->AZI1t.SS  ) ;
  sprintf( c_hhmmss_azi2, "%dh%dm%ds", gp_Ast->AZI2t.HH, gp_Ast->AZI2t.MM, gp_Ast->AZI2t.SS  ) ;

  Trace(" %s : infos         : %s", c_nom , gp_Ast->infos ) ;
  Trace(" %s : type          : %s", c_nom , c_type ) ;
  Trace(" %s : mode calcul   : %s", c_nom , c_mode ) ;
  Trace(" %s : latitude      : %.2f (deg) ", c_nom, gp_Lie->lat    * CALCUL_UN_RADIAN_EN_DEGRES ) ;
  Trace(" %s : longitude     : %.2f (deg) ", c_nom, gp_Lie->lon    * CALCUL_UN_RADIAN_EN_DEGRES ) ;
  Trace(" %s : vitesses      : %.2f (Va) %.2f (Vh)", c_nom, gp_Ast->Va,  gp_Ast->Vh ) ; 
  Trace(" %s : azimut        : %.2f (deg) ", c_nom, gp_Ast->a    * CALCUL_UN_RADIAN_EN_DEGRES ) ;
  Trace(" %s : altitude      : %.2f (deg) ", c_nom, gp_Ast->h    * CALCUL_UN_RADIAN_EN_DEGRES ) ;
  Trace(" %s : declinaison   : %.2f (deg) ", c_nom, gp_Ast->DEC  * CALCUL_UN_RADIAN_EN_DEGRES  ) ;
  Trace(" %s : ascension dro : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->ASC    * CALCUL_UN_RADIAN_EN_DEGRES, c_hhmmss_asc ) ;
  Trace(" %s : angle horaire : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH   * CALCUL_UN_RADIAN_EN_DEGRES, c_hhmmss_agh ) ;

  Trace1(" %s : Agh0          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH0  * CALCUL_UN_RADIAN_EN_DEGRES, c_hhmmss_agh0 ) ;
  Trace1(" %s : Agh1          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH1  * CALCUL_UN_RADIAN_EN_DEGRES, c_hhmmss_agh1 ) ;
  Trace1(" %s : Agh2          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH2  * CALCUL_UN_RADIAN_EN_DEGRES, c_hhmmss_agh2 ) ;

  Trace1(" %s : Azi0          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AZI0  * CALCUL_UN_RADIAN_EN_DEGRES, c_hhmmss_azi0 ) ;
  Trace1(" %s : Azi1          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AZI1  * CALCUL_UN_RADIAN_EN_DEGRES, c_hhmmss_azi1 ) ;
  Trace1(" %s : Azi2          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AZI2  * CALCUL_UN_RADIAN_EN_DEGRES, c_hhmmss_azi2 ) ;

  Trace("----------------------------") ;
}

/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_TOUT
* @author : s.gravois
* @brief  : Cette fonction appelle toutes les autres fonctions d'affichage
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_AFFICHER_TOUT(void) {

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  TEMPS_AFFICHER(   gp_Tim ) ;
  LIEU_AFFICHER(    gp_Lie );
  CONFIG_AFFICHER_MODE_LONG(  ) ;
  VOUTE_AFFICHER(   gp_Vou ) ;

  Trace("\n") ;
}
/*****************************************************************************************
* @fn     : CONFIG_AFFICHER_CHANGEMENTS
* @author : s.gravois
* @brief  : Cette fonction detecte et affiche les changements 
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : a consolider et voir utilite
*****************************************************************************************/

void CONFIG_AFFICHER_CHANGEMENTS (void)  {

  char s_menu[256] ;

  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  Trace("start") ;

  memset(s_menu, 0, sizeof(s_menu)) ;

  switch(gp_Sui->menu) {
     
    case MENU_AZIMUTAL             :strcpy(s_menu,"MENU_AZIMUTAL") ; break ; 
    case MENU_EQUATORIAL           :strcpy(s_menu,"MENU_EQUATORIAL") ; break ; 
    case MENU_MANUEL_BRUT          :strcpy(s_menu,"MENU_MANUEL_BRUT") ; break ; 
    case MENU_MANUEL_NON_ASSERVI   :strcpy(s_menu,"MENU_MANUEL_NON_ASSERVI") ; break ; 
    case MENU_MANUEL_ASSERVI       :strcpy(s_menu,"MENU_MANUEL_ASSERVI") ; break ; 
    case MENU_GOTO                 :strcpy(s_menu,"MENU_GOTO") ; break ; 
    case MENU_INFO                 :strcpy(s_menu,"MENU_INFO") ; break ; 
    case MENU_RESEAU_UP            :strcpy(s_menu,"MENU_RESEAU_UP") ; break ; 
    case MENU_RESEAU_DOWN          :strcpy(s_menu,"MENU_RESEAU_DOWN") ; break ; 
    case MENU_PROGRAMME_DOWN       :strcpy(s_menu,"MENU_PROGRAMME_DOWN") ; break ; 
    case MENU_DOWN                 :strcpy(s_menu,"MENU_DOWN") ; break ; 
  }

  if ( gp_Sui->menu_old != gp_Sui->menu ) {

    Trace("appel : %d : %s" , gp_Sui->menu, s_menu) ;
    GPIO_CLIGNOTE(gp_Gpi_Par->par_led_etat, 1, 100) ;
  }
  Trace("end") ;

  return ;
}

/*****************************************************************************************
* @fn     : CONFIG_PATH_FIND
* @author : s.gravois
* @brief  : Cette fonction test un chemin d inclusion pour une commande
* @param  : char * g_paths
* @param  : char * cmd
* @date   : 2022-04-09 creation 
*****************************************************************************************/

int   CONFIG_PATH_FIND (char * g_paths, char *cmd) {

  int i=-1;
  int i_retour = FALSE ;
  char c_path[ CONFIG_TAILLE_BUFFER_32 ] ;

  while( ++i < CONFIG_C_BIN_POSSIBLE_PATHS_LENGTH && i_retour == FALSE ) {
    sprintf( c_path, "%s/%s", c_Bin_Possible_Paths[i], cmd) ;
    Trace("Test chemin %s", c_path) ;
    if( access( c_path, F_OK ) == 0 ) { 
      i_retour = TRUE ; 
      strcpy( g_paths, c_path ) ;
    }
  }
  Trace1("Chemin retenu : %s", g_paths) ;

  return i_retour ; 
}
//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================



