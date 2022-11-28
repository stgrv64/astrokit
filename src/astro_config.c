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
#                * protection zones de code impliquant LCD* (lcd_mutex)
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
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;
MACRO_ASTRO_GLOBAL_EXTERN_CONST ;

char  gc_config_path_cmd_stty[ CONFIG_TAILLE_BUFFER_32 ] ;

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
* @brief  : Cette fonction verifie si caractere lu est correct (CONFIG_FIC_READ)
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_FORMAT_ADMIS(char c) {
  
  int i=(int)c ;
  int j ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

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
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

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

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

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

  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  j=0;
  if ( i==9 )j=1;  // \t
  if ( i==32)j=1;  // space
  return j ; 
}
/*****************************************************************************************
* @fn     : CONFIG_FIN_FICHIER
* @author : s.gravois
* @brief  : Cette fonction verifie si caractere est une fin de fichier (EOF) ou non admis
* @param  : char c
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

int CONFIG_FIN_FICHIER(char c) {
  int i=(int)c ;
  int j ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

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
            par CONFIG_FIC_READ / GPIO_CONFIG_FIC_READ
* @param  : l_Char_Params[CONFIG_DATAS_NB_LIGNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : juin 2022 : ajout parametres STRUCT_PID
* @date   : nov 2022 : suite a refonte complete du code, passage par enum / g_char* / hachage ..
* @todo   : 
*****************************************************************************************/

void CONFIG_PARAMETRES_CONFIG(STRUCT_CONFIG * lp_Con) {

  int l ;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

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

   gp_Cal_Par->par_azi_red_1 = 1.0 ;  // reduction liee a la monture
   gp_Cal_Par->par_azi_red_2 = 1.0;   // reducteur du moteur
   gp_Cal_Par->par_azi_red_3 = 1.0;   // nombre de pas du moteur en azimut
   gp_Cal_Par->par_azi_red_4 = 1.0;   // mode micro pas utilisee (1/R4)
   gp_Cal_Par->par_azi_red_5 = 1.0;   // reduction liee a la poulie
   gp_Cal_Par->par_azi_red_6 = 1.0;   // reduction liee au cpu
   gp_Cal_Par->par_azi_red_7 = 1.0;   // reduction non decrite plus haut

   gp_Dev_Par->par_use_Capteurs = 0    ;
   gp_Dev_Par->par_use_Raquette = 0    ;
   gp_Dev_Par->par_use_Bluetooth = 0   ;
   gp_Dev_Par->par_use_Infrared = 0  ;
   gp_Dev_Par->par_use_Controler = 0 ;
   gp_Dev_Par->par_use_Keyboard = 0 ;
   gp_Dev_Par->par_use_Lcd = 0 ;

   gp_Gpi_Par_Pwm->par_led_etat=0;

   /* Definition de valeurs par defauts pour les TEMPO */ 

  gp_Tim_Par->par_tpo_Menu     = 50000;
  gp_Tim_Par->par_tpo_Raq      = 51000; /* est utilisee uniquement dans SUIVI_MANUEL_ASSERVI */
  gp_Tim_Par->par_tpo_Ir       = 52000;
  gp_Tim_Par->par_tpo_Termios  = 53000;
  gp_Tim_Par->par_tpo_Capteurs = 50000;
  gp_Tim_Par->par_tpo_Lcd_Loop = 250000 ; 
  gp_Tim_Par->par_tpo_Lcd_Disp = 100000 ; 

  gp_Pid_Par->par_pid_ech = 3 ;

   //----------------------------------------------------------------------
   // Lecture des variables dans la config lue dans le fichier de config
   //-----------gp_Ast_Par->par_default_object-------------------------------------------

  memset( gp_Ast_Par->par_default_object, CALCULS_ZERO_CHAR, sizeof( gp_Ast_Par->par_default_object ) ) ;

  memset( gp_Con_Par->par_rep_cat, CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_rep_cat ) ) ;
  memset( gp_Con_Par->par_rep_cfg, CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_rep_cfg ) ) ;
  memset( gp_Con_Par->par_rep_log, CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_rep_log ) ) ;
  memset( gp_Con_Par->par_rep_in,  CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_rep_in ) ) ;
  memset( gp_Con_Par->par_rep_out, CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_rep_out ) ) ;
  memset( gp_Con_Par->par_rep_scr, CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_rep_scr ) ) ;

  memset( gp_Con_Par->par_fic_led, CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_fic_led ) ) ;
  memset( gp_Con_Par->par_fic_log, CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_fic_log ) ) ;
  memset( gp_Con_Par->par_fic_pid, CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_fic_pid ) ) ;
  memset( gp_Con_Par->par_fic_dat, CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_fic_dat ) ) ;
  memset( gp_Con_Par->par_fic_hhm, CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_fic_hhm ) ) ;
  memset( gp_Con_Par->par_src_ker, CALCULS_ZERO_CHAR, sizeof( gp_Con_Par->par_src_ker ) ) ;

  for(l=0;l<CONFIG_DATAS_NB_LIGNES;l++) {

    // FIXME : note 2021 : les variables GPIO_xxx sont gérées dans le ficheir gpio.c

    if(!strcmp("ASTRE_PAR_DEFAUT",lp_Con->con_params[l][0])) strcpy( gp_Ast_Par->par_default_object, lp_Con->con_params[l][1]) ;

    if(!strcmp("MENU_PAR_DEFAUT",lp_Con->con_params[l][0])) {

      if(!strcmp(lp_Con->con_params[l][1],"MENU_AZIMUTAL" ))           gp_Con_Par->par_default_menu = MENU_AZIMUTAL ;
      if(!strcmp(lp_Con->con_params[l][1],"MENU_EQUATORIAL" ))         gp_Con_Par->par_default_menu = MENU_EQUATORIAL ;
      if(!strcmp(lp_Con->con_params[l][1],"MENU_MANUEL_BRUT" ))        gp_Con_Par->par_default_menu = MENU_MANUEL_BRUT ;
      if(!strcmp(lp_Con->con_params[l][1],"MENU_MANUEL_NON_ASSERVI" )) gp_Con_Par->par_default_menu = MENU_MANUEL_NON_ASSERVI ;
      if(!strcmp(lp_Con->con_params[l][1],"MENU_MANUEL_ASSERVI" ))     gp_Con_Par->par_default_menu = MENU_MANUEL_ASSERVI ;
      if(!strcmp(lp_Con->con_params[l][1],"MENU_GOTO" ))               gp_Con_Par->par_default_menu = MENU_GOTO ;
      if(!strcmp(lp_Con->con_params[l][1],"MENU_INFO" ))               gp_Con_Par->par_default_menu = MENU_INFO ;
      if(!strcmp(lp_Con->con_params[l][1],"MENU_RESEAU_UP" ))          gp_Con_Par->par_default_menu = MENU_RESEAU_UP ;
      if(!strcmp(lp_Con->con_params[l][1],"MENU_RESEAU_DOWN" ))        gp_Con_Par->par_default_menu = MENU_RESEAU_DOWN ;
      if(!strcmp(lp_Con->con_params[l][1],"MENU_PROGRAMME_DOWN" ))     gp_Con_Par->par_default_menu = MENU_PROGRAMME_DOWN ;
      if(!strcmp(lp_Con->con_params[l][1],"MENU_DOWN" ))               gp_Con_Par->par_default_menu = MENU_DOWN ;
    } 
    /* Definition de valeurs par defauts pour les TEMPO */ 
    
    if(!strcmp("TEMPO_RAQ",        lp_Con->con_params[l][0]))      gp_Tim_Par->par_tpo_Raq=atol(lp_Con->con_params[l][1]);
    if(!strcmp("TEMPO_MENU",       lp_Con->con_params[l][0])) gp_Tim_Par->par_tpo_Menu=atol(lp_Con->con_params[l][1]);
    if(!strcmp("TEMPO_IR",         lp_Con->con_params[l][0])) gp_Tim_Par->par_tpo_Ir=atol(lp_Con->con_params[l][1]);
    if(!strcmp("TEMPO_TERMIOS",    lp_Con->con_params[l][0])) gp_Tim_Par->par_tpo_Termios=atol(lp_Con->con_params[l][1]);
    if(!strcmp("TEMPO_CAPTEURS",   lp_Con->con_params[l][0])) gp_Tim_Par->par_tpo_Capteurs=atol(lp_Con->con_params[l][1]);
    if(!strcmp("TEMPO_LCD_LOOP",   lp_Con->con_params[l][0])) gp_Tim_Par->par_tpo_Lcd_Loop=atol(lp_Con->con_params[l][1]);
    if(!strcmp("TEMPO_LCD_DISP",   lp_Con->con_params[l][0])) gp_Tim_Par->par_tpo_Lcd_Disp=atol(lp_Con->con_params[l][1]);

    // chemins des repertoires et fichiers (2021)

    if(!strcmp("CONFIG_REP_CAT",   lp_Con->con_params[l][0]))  strcpy( gp_Con_Par->par_rep_cat, lp_Con->con_params[l][1]) ;
    if(!strcmp("CONFIG_REP_CFG",   lp_Con->con_params[l][0]))  strcpy( gp_Con_Par->par_rep_cfg, lp_Con->con_params[l][1]) ;
    if(!strcmp("CONFIG_REP_LOG",   lp_Con->con_params[l][0]))  strcpy( gp_Con_Par->par_rep_log, lp_Con->con_params[l][1]) ;
    if(!strcmp("CONFIG_REP_IN",    lp_Con->con_params[l][0]))   strcpy( gp_Con_Par->par_rep_in, lp_Con->con_params[l][1]) ;
    if(!strcmp("CONFIG_REP_OUT",   lp_Con->con_params[l][0]))  strcpy( gp_Con_Par->par_rep_out, lp_Con->con_params[l][1]) ;
    if(!strcmp("CONFIG_REP_LOG",   lp_Con->con_params[l][0]))  strcpy( gp_Con_Par->par_fic_log, lp_Con->con_params[l][1]) ;
    if(!strcmp("CONFIG_REP_SCR",   lp_Con->con_params[l][0]))  strcpy( gp_Con_Par->par_rep_scr, lp_Con->con_params[l][1]) ;  
    if(!strcmp("CONFIG_FIC_PID",   lp_Con->con_params[l][0]))  strcpy( gp_Con_Par->par_fic_pid, lp_Con->con_params[l][1]) ;
    if(!strcmp("CONFIG_FIC_LED",   lp_Con->con_params[l][0]))  strcpy( gp_Con_Par->par_fic_led, lp_Con->con_params[l][1]) ;
    if(!strcmp("CONFIG_FIC_DATE",  lp_Con->con_params[l][0])) strcpy( gp_Con_Par->par_fic_dat, lp_Con->con_params[l][1]) ;
    if(!strcmp("CONFIG_FIC_HHMM",  lp_Con->con_params[l][0])) strcpy( gp_Con_Par->par_fic_hhm, lp_Con->con_params[l][1]) ;  
    if(!strcmp("CONFIG_SCR_KERNEL",lp_Con->con_params[l][0])) strcpy( gp_Con_Par->par_src_ker, lp_Con->con_params[l][1]) ;  

    if(!strcmp("PID_ECH",       lp_Con->con_params[l][0])) gp_Pid_Par->par_pid_ech=atof(lp_Con->con_params[l][1]);
    if(!strcmp("PID_KP",        lp_Con->con_params[l][0])) gp_Pid_Par->par_pid_ki =atof(lp_Con->con_params[l][1]);
    if(!strcmp("PID_KI",        lp_Con->con_params[l][0])) gp_Pid_Par->par_pid_kp =atof(lp_Con->con_params[l][1]);
    if(!strcmp("PID_KD",        lp_Con->con_params[l][0])) gp_Pid_Par->par_pid_kd =atof(lp_Con->con_params[l][1]);

    if(!strcmp("LED_ETAT",      lp_Con->con_params[l][0]))     gp_Gpi_Par_Pwm->par_led_etat=atoi(lp_Con->con_params[l][1]);

    if(!strcmp("DEVICE_USE_CONTROLER", lp_Con->con_params[l][0])) gp_Dev_Par->par_use_Controler=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("DEVICE_USE_CAPTEURS",  lp_Con->con_params[l][0])) gp_Dev_Par->par_use_Capteurs=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("DEVICE_USE_RAQUETTE",  lp_Con->con_params[l][0])) gp_Dev_Par->par_use_Raquette=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("DEVICE_USE_BLUETOOTH", lp_Con->con_params[l][0])) gp_Dev_Par->par_use_Bluetooth=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("DEVICE_USE_INFRAROUGE",lp_Con->con_params[l][0])) gp_Dev_Par->par_use_Infrared=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("DEVICE_USE_KEYBOARD",  lp_Con->con_params[l][0])) gp_Dev_Par->par_use_Keyboard=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("DEVICE_USE_LCD",       lp_Con->con_params[l][0])) gp_Dev_Par->par_use_Lcd=atoi(lp_Con->con_params[l][1]);

    if(!strcmp("LONGITUDE",     lp_Con->con_params[l][0])) gp_Lie_Par->par_longitude=atof(lp_Con->con_params[l][1]);
    if(!strcmp("LATITUDE",      lp_Con->con_params[l][0])) gp_Lie_Par->par_latitude=atof(lp_Con->con_params[l][1]);
    if(!strcmp("ALTITUDE",      lp_Con->con_params[l][0])) gp_Lie_Par->par_altitude=atof(lp_Con->con_params[l][1]);

    if(!strcmp("ALT_R1",     lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_red_1 = atof(lp_Con->con_params[l][1]) ;         
    if(!strcmp("ALT_R2",     lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_red_2 = atof(lp_Con->con_params[l][1]) ;
    if(!strcmp("ALT_R3",     lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_red_3 = atof(lp_Con->con_params[l][1]) ;         
    if(!strcmp("ALT_R4",     lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_red_4 = atof(lp_Con->con_params[l][1]) ;  
    if(!strcmp("ALT_R5",     lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_red_5 = atof(lp_Con->con_params[l][1]) ;
    if(!strcmp("ALT_R6",     lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_red_6 = atof(lp_Con->con_params[l][1]) ;       
    if(!strcmp("ALT_R7",     lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_red_7 = atof(lp_Con->con_params[l][1]) ;
    if(!strcmp("ALT_REV",    lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_rev   = atoi(lp_Con->con_params[l][1]);
    if(!strcmp("ALT_ACC",    lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_acc   = atof(lp_Con->con_params[l][1]) ;
        
    if(!strcmp("AZI_R1",     lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_red_1  = atof(lp_Con->con_params[l][1]) ; 
    if(!strcmp("AZI_R2",     lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_red_2  = atof(lp_Con->con_params[l][1]) ; 
    if(!strcmp("AZI_R3",     lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_red_3  = atof(lp_Con->con_params[l][1]) ; 
    if(!strcmp("AZI_R4",     lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_red_4  = atof(lp_Con->con_params[l][1]) ; 
    if(!strcmp("AZI_R5",     lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_red_5  = atof(lp_Con->con_params[l][1]) ; 
    if(!strcmp("AZI_R6",     lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_red_6  = atof(lp_Con->con_params[l][1]) ; 
    if(!strcmp("AZI_R7",     lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_red_7  = atof(lp_Con->con_params[l][1]) ; 
    if(!strcmp("AZI_REV",    lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_rev   = atoi(lp_Con->con_params[l][1]);
    if(!strcmp("AZI_ACC",    lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_acc   = atof(lp_Con->con_params[l][1]) ;

    // devices de azimut et altitude (qui concernent les 2 en mm temps)

    if(!strcmp("ALTAZ_FORWARD",     lp_Con->con_params[l][0])) gp_Cal_Par->par_altaz_slow_forward = atof(lp_Con->con_params[l][1])      ;
    if(!strcmp("ALTAZ_REWIND",      lp_Con->con_params[l][0])) gp_Cal_Par->par_altaz_slow_rewind = atof(lp_Con->con_params[l][1])      ;
    if(!strcmp("ALTAZ_FORWARD_FAST",lp_Con->con_params[l][0])) gp_Cal_Par->par_altaz_fast_forward = atof(lp_Con->con_params[l][1])      ;
    if(!strcmp("ALTAZ_REWIND_FAST", lp_Con->con_params[l][0])) gp_Cal_Par->par_altaz_fast_rewind = atof(lp_Con->con_params[l][1])      ;

    if(!strcmp("GPIO_ALT_DIR", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_alt_dir=atoi(lp_Con->con_params[l][1]);  
    if(!strcmp("GPIO_ALT_CLK", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_alt_clk=atoi(lp_Con->con_params[l][1]);  
    if(!strcmp("GPIO_ALT_SLP", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_alt_slp=atoi(lp_Con->con_params[l][1]);  
    if(!strcmp("GPIO_ALT_RST", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_alt_rst=atoi(lp_Con->con_params[l][1]);  
    if(!strcmp("GPIO_ALT_MMM", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_alt_mmm=atoi(lp_Con->con_params[l][1]);  
    if(!strcmp("GPIO_ALT_ENA", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_alt_ena=atoi(lp_Con->con_params[l][1]);  
    if(!strcmp("GPIO_ALT_M2", lp_Con->con_params[l][0]))  gp_Gpi_Par_Con->par_alt_m2=atoi(lp_Con->con_params[l][1]);  
    if(!strcmp("GPIO_ALT_M1", lp_Con->con_params[l][0]))  gp_Gpi_Par_Con->par_alt_m1=atoi(lp_Con->con_params[l][1]);  
    if(!strcmp("GPIO_ALT_M0", lp_Con->con_params[l][0]))  gp_Gpi_Par_Con->par_alt_m0=atoi(lp_Con->con_params[l][1]);  
    
    if(!strcmp("GPIO_AZI_", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_azi_dir=atoi(lp_Con->con_params[l][1]) ; 
    if(!strcmp("GPIO_AZI_", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_azi_clk=atoi(lp_Con->con_params[l][1]) ; 
    if(!strcmp("GPIO_AZI_", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_azi_slp=atoi(lp_Con->con_params[l][1]) ;
    if(!strcmp("GPIO_AZI_", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_azi_rst=atoi(lp_Con->con_params[l][1]);  
    if(!strcmp("GPIO_AZI_", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_azi_mmm=atoi(lp_Con->con_params[l][1]);  
    if(!strcmp("GPIO_AZI_", lp_Con->con_params[l][0])) gp_Gpi_Par_Con->par_azi_ena=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_AZI_", lp_Con->con_params[l][0]))  gp_Gpi_Par_Con->par_azi_m2=atoi(lp_Con->con_params[l][1])  ;
    if(!strcmp("GPIO_AZI_", lp_Con->con_params[l][0]))  gp_Gpi_Par_Con->par_azi_m1=atoi(lp_Con->con_params[l][1])  ; 
    if(!strcmp("GPIO_AZI_", lp_Con->con_params[l][0]))  gp_Gpi_Par_Con->par_azi_m0=atoi(lp_Con->con_params[l][1])  ; 

    if(!strcmp("MCP_ALT_DIR", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_alt_dir=atoi(lp_Con->con_params[l][1])  ;  
    if(!strcmp("MCP_ALT_CLK", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_alt_clk=atoi(lp_Con->con_params[l][1])  ;  
    if(!strcmp("MCP_ALT_SLP", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_alt_slp=atoi(lp_Con->con_params[l][1])  ;  
    if(!strcmp("MCP_ALT_RST", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_alt_rst=atoi(lp_Con->con_params[l][1])  ;  
    if(!strcmp("MCP_ALT_M2", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_alt_m2=atoi(lp_Con->con_params[l][1])   ;  
    if(!strcmp("MCP_ALT_M1", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_alt_m1=atoi(lp_Con->con_params[l][1])   ;  
    if(!strcmp("MCP_ALT_M0", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_alt_m0=atoi(lp_Con->con_params[l][1])   ;  

    if(!strcmp("MCP_AZI_DIR", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_azi_dir=atoi(lp_Con->con_params[l][1])   ;
    if(!strcmp("MCP_AZI_CLK", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_azi_clk=atoi(lp_Con->con_params[l][1])   ;
    if(!strcmp("MCP_AZI_SLP", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_azi_slp=atoi(lp_Con->con_params[l][1])   ;
    if(!strcmp("MCP_AZI_RST", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_azi_rst=atoi(lp_Con->con_params[l][1])   ; 
    if(!strcmp("MCP_AZI_M2", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_azi_m2=atoi(lp_Con->con_params[l][1])    ;
    if(!strcmp("MCP_AZI_M1", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_azi_m1=atoi(lp_Con->con_params[l][1])    ;
    if(!strcmp("MCP_AZI_M0", lp_Con->con_params[l][0]))  gp_I2c_Mcp->mcp_azi_m0=atoi(lp_Con->con_params[l][1])   ;

    if(!strcmp("ALT_F",         lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_f=atol(lp_Con->con_params[l][1]);
    if(!strcmp("AZI_F",         lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_f=atol(lp_Con->con_params[l][1]);
    if(!strcmp("ALT_N",         lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_n=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("AZI_N",         lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_n=atoi(lp_Con->con_params[l][1]);
    /* non obligatoire et/ou obsolete : */
    if(!strcmp("ALT_RED_TOT",   lp_Con->con_params[l][0])) gp_Cal_Par->par_alt_red_tot=atof(lp_Con->con_params[l][1]);
    if(!strcmp("AZI_RED_TOT",   lp_Con->con_params[l][0])) gp_Cal_Par->par_azi_red_tot=atof(lp_Con->con_params[l][1]);
    /* non obligatoire et/ou obsolete : */
    if(!strcmp("GPIO_RAQ_OUEST",lp_Con->con_params[l][0]))  gp_Gpi_Par_Raq->par_raq_ouest=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_RAQ_EST",  lp_Con->con_params[l][0]))  gp_Gpi_Par_Raq->par_raq_est=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_RAQ_SUD",  lp_Con->con_params[l][0]))  gp_Gpi_Par_Raq->par_raq_sud=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_RAQ_NORD", lp_Con->con_params[l][0]))  gp_Gpi_Par_Raq->par_raq_nord=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_RAQ_V",     lp_Con->con_params[l][0])) gp_Gpi_Par_Raq->par_raq_v=atoi(lp_Con->con_params[l][1]);
    /* non obligatoire et/ou obsolete : */
    if(!strcmp("GPIO_KEY_L1",        lp_Con->con_params[l][0])) gp_Gpi_Par_Mat->par_l1=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_KEY_L2",        lp_Con->con_params[l][0])) gp_Gpi_Par_Mat->par_l2=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_KEY_L3",        lp_Con->con_params[l][0])) gp_Gpi_Par_Mat->par_l3=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_KEY_L4",        lp_Con->con_params[l][0])) gp_Gpi_Par_Mat->par_l4=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_KEY_C1",        lp_Con->con_params[l][0])) gp_Gpi_Par_Mat->par_c1=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_KEY_C2",        lp_Con->con_params[l][0])) gp_Gpi_Par_Mat->par_c2=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_KEY_C3",        lp_Con->con_params[l][0])) gp_Gpi_Par_Mat->par_c3=atoi(lp_Con->con_params[l][1]);
    if(!strcmp("GPIO_KEY_C4",        lp_Con->con_params[l][0])) gp_Gpi_Par_Mat->par_c4=atoi(lp_Con->con_params[l][1]);
  }

  //if ( gp_Cal_Par->par_alt_red_tot == 0 ) gp_Cal_Par->par_alt_red_tot = gp_Cal_Par->par_alt_red_1 * gp_Cal_Par->par_alt_red_2 * gp_Cal_Par->par_alt_red_3 * gp_Cal_Par->par_alt_red_4 ;
  //if ( gp_Cal_Par->par_azi_red_tot == 0 ) gp_Cal_Par->par_azi_red_tot = gp_Cal_Par->par_azi_red_1 * gp_Cal_Par->par_azi_red_2 * gp_Cal_Par->par_azi_red_3 * gp_Cal_Par->par_azi_red_4 ;
  
  /* Attention gp_Cal_Par->par_alt_red_4 (micro pas) est traite independamment */

  if ( gp_Cal_Par->par_alt_red_tot == 0 ) {
    gp_Cal_Par->par_alt_red_tot = gp_Cal_Par->par_alt_red_1 * gp_Cal_Par->par_alt_red_2 * gp_Cal_Par->par_alt_red_3 * gp_Cal_Par->par_alt_red_5 * gp_Cal_Par->par_alt_red_6 * gp_Cal_Par->par_alt_red_7 ;
  }
  /* Attention gp_Cal_Par->par_alt_red_4 (micro pas) est traite independamment */

  if ( gp_Cal_Par->par_azi_red_tot == 0 ) {
    gp_Cal_Par->par_azi_red_tot = gp_Cal_Par->par_azi_red_1 * gp_Cal_Par->par_azi_red_2 * gp_Cal_Par->par_azi_red_3 * gp_Cal_Par->par_azi_red_5 * gp_Cal_Par->par_azi_red_6 * gp_Cal_Par->par_azi_red_7 ;
  }
}

/*****************************************************************************************
* @fn     : CONFIG_PARAMETRES_AFFICHER
* @author : s.gravois
* @brief  : Cette fonction affiche les parametres 
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-05-30 ajout gp_Tim_Par->par_tpo_LCD_xxx 
* @date   : 2022-06-17 ajout gp_Pid_Par->par_pid_ech
* @todo   : 
*****************************************************************************************/

void   CONFIG_PARAMETRES_AFFICHER(void) {

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace1("gp_Con_Par->par_default_menu = %s",   gc_hach_suivi_menus[ gp_Con_Par->par_default_menu ] ) ;
  
  Trace1("gp_Tim_Par->par_tpo_Raq = %ld",  gp_Tim_Par->par_tpo_Raq);
  Trace1("gp_Tim_Par->par_tpo_Menu = %ld",  gp_Tim_Par->par_tpo_Menu);
  Trace1("gp_Tim_Par->par_tpo_Ir = %ld",  gp_Tim_Par->par_tpo_Ir);
  Trace1("gp_Tim_Par->par_tpo_Termios = %ld",  gp_Tim_Par->par_tpo_Termios);
  Trace1("gp_Tim_Par->par_tpo_Capteurs = %ld",  gp_Tim_Par->par_tpo_Capteurs);
  Trace1("gp_Tim_Par->par_tpo_Lcd_Loop = %ld",  gp_Tim_Par->par_tpo_Lcd_Loop);
  Trace1("gp_Tim_Par->par_tpo_Lcd_Disp = %ld",  gp_Tim_Par->par_tpo_Lcd_Disp);

  Trace1("gp_Pid_Par->par_pid_ech = %f",  gp_Pid_Par->par_pid_ech);
  Trace1("gp_Pid_Par->par_pid_ki = %f",  gp_Pid_Par->par_pid_ki);
  Trace1("gp_Pid_Par->par_pid_kp = %f",  gp_Pid_Par->par_pid_kp);
  Trace1("gp_Pid_Par->par_pid_kd = %f",  gp_Pid_Par->par_pid_kd);

  Trace("gp_Dev_Par->par_use_Controler = %d",  gp_Dev_Par->par_use_Controler);
  Trace("gp_Dev_Par->par_use_Capteurs = %d",  gp_Dev_Par->par_use_Capteurs);
  Trace("gp_Dev_Par->par_use_Bluetooth = %d",  gp_Dev_Par->par_use_Bluetooth);
  Trace("gp_Dev_Par->par_use_Infrared = %d",  gp_Dev_Par->par_use_Infrared);
  Trace("gp_Dev_Par->par_use_Raquette = %d",  gp_Dev_Par->par_use_Raquette);
  Trace("gp_Dev_Par->par_use_Keyboard = %d",  gp_Dev_Par->par_use_Keyboard);
  Trace("gp_Dev_Par->par_use_Lcd = %d",  gp_Dev_Par->par_use_Lcd);

  Trace("gp_Ast_Par->par_default_object = %s",  gp_Ast_Par->par_default_object );

  Trace1("gp_Lie_Par->par_latitude  = %f",          gp_Lie_Par->par_latitude );
  Trace1("gp_Lie_Par->par_longitude = %f",          gp_Lie_Par->par_longitude );
  Trace1("gp_Lie_Par->par_altitude  = %f",          gp_Lie_Par->par_altitude );

  Trace1("gp_Gpi_Par_Pwm->par_led_etat = %d", gp_Gpi_Par_Pwm->par_led_etat );

  Trace1("gp_Cal_Par->par_alt_red_1 = %f",       gp_Cal_Par->par_alt_red_1);         
  Trace1("gp_Cal_Par->par_alt_red_2 = %f",       gp_Cal_Par->par_alt_red_2);
  Trace1("gp_Cal_Par->par_alt_red_3 = %f",       gp_Cal_Par->par_alt_red_3);         
  Trace1("gp_Cal_Par->par_alt_red_4 = %f",       gp_Cal_Par->par_alt_red_4);         
  Trace1("gp_Cal_Par->par_alt_rev = %d",      gp_Cal_Par->par_alt_rev);
  Trace1("gp_Cal_Par->par_alt_acc = %f",      gp_Cal_Par->par_alt_acc);

  Trace1("gp_Cal_Par->par_azi_red_1 = %f",       gp_Cal_Par->par_azi_red_1)      ; 
  Trace1("gp_Cal_Par->par_azi_red_2 = %f",       gp_Cal_Par->par_azi_red_2)      ; 
  Trace1("gp_Cal_Par->par_azi_red_3 = %f",       gp_Cal_Par->par_azi_red_3)      ; 
  Trace1("gp_Cal_Par->par_azi_red_4 = %f",       gp_Cal_Par->par_azi_red_4)      ; 
  Trace1("gp_Cal_Par->par_azi_rev = %d",      gp_Cal_Par->par_azi_rev)     ;
  Trace1("gp_Cal_Par->par_azi_acc = %f",      gp_Cal_Par->par_azi_acc)     ; 

  Trace1("gp_Con_Par->par_rep_cat = %s", gp_Con_Par->par_rep_cat)  ;
  Trace1("gp_Con_Par->par_rep_cfg = %s", gp_Con_Par->par_rep_cfg)  ; 
  Trace1("gp_Con_Par->par_rep_log = %s", gp_Con_Par->par_rep_log)  ; 
  Trace1("gp_Con_Par->par_rep_in = %s",  gp_Con_Par->par_rep_in)  ; 
  Trace1("gp_Con_Par->par_rep_out = %s",  gp_Con_Par->par_rep_out)  ; 
  Trace1("gp_Con_Par->par_fic_log = %s", gp_Con_Par->par_fic_log)  ; 
  Trace1("gp_Con_Par->par_fic_pid = %s", gp_Con_Par->par_fic_pid)  ; 
  Trace1("gp_Con_Par->par_fic_led = %s", gp_Con_Par->par_fic_led)  ; 
  Trace1("gp_Con_Par->par_fic_dat = %s", gp_Con_Par->par_fic_dat)  ; 
  Trace1("gp_Con_Par->par_fic_hhm = %s", gp_Con_Par->par_fic_hhm)  ;  

  Trace1("gp_Con_Par->par_rep_scr = %s", gp_Con_Par->par_rep_scr)  ; 
  Trace1("gp_Con_Par->par_src_ker = %s", gp_Con_Par->par_src_ker)  ; 

  Trace1("gp_Gpi_Par_Pwm->par_alt = %s", gp_Gpi_Par_Pwm->par_alt)  ;  
  Trace1("gp_Gpi_Par_Pwm->par_azi = %s", gp_Gpi_Par_Pwm->par_azi)  ;  
  Trace1("gp_Gpi_Par_Pwm->par_mas = %s", gp_Gpi_Par_Pwm->par_mas)  ;  
  Trace1("gp_Gpi_Par_Pwm->par_fre_pwm = %s", gp_Gpi_Par_Pwm->par_fre_pwm)  ;  

  Trace2("anciennes variables\n");
  Trace2("gp_Gpi_Par_Raq->par_raq_ouest   = %d",  gp_Gpi_Par_Raq->par_raq_ouest);
  Trace2("gp_Gpi_Par_Raq->par_raq_est   = %d",  gp_Gpi_Par_Raq->par_raq_est);
  Trace2("gp_Gpi_Par_Raq->par_raq_sud   = %d",  gp_Gpi_Par_Raq->par_raq_sud);
  Trace2("gp_Gpi_Par_Raq->par_raq_nord   = %d",  gp_Gpi_Par_Raq->par_raq_nord);
  Trace2("gp_Gpi_Par_Raq->par_raq_v   = %d",  gp_Gpi_Par_Raq->par_raq_v);
  Trace2("gp_Gpi_Par_Mat->par_l1  = %d",  gp_Gpi_Par_Mat->par_l1);
  Trace2("gp_Gpi_Par_Mat->par_l2  = %d",  gp_Gpi_Par_Mat->par_l2);
  Trace2("gp_Gpi_Par_Mat->par_l3  = %d",  gp_Gpi_Par_Mat->par_l3);
  Trace2("gp_Gpi_Par_Mat->par_l4  = %d",  gp_Gpi_Par_Mat->par_l4);
  Trace2("gp_Gpi_Par_Mat->par_c1  = %d",  gp_Gpi_Par_Mat->par_c1);
  Trace2("gp_Gpi_Par_Mat->par_c2  = %d",  gp_Gpi_Par_Mat->par_c2);
  Trace2("gp_Gpi_Par_Mat->par_c3  = %d",  gp_Gpi_Par_Mat->par_c3);
  Trace2("gp_Gpi_Par_Mat->par_c4  = %d",  gp_Gpi_Par_Mat->par_c4);
  Trace2("gp_Gpi_Par_Con->par_alt_dir = %d", gp_Gpi_Par_Con->par_alt_dir);  
  Trace2("gp_Gpi_Par_Con->par_alt_clk = %d", gp_Gpi_Par_Con->par_alt_clk);  
  Trace2("gp_Gpi_Par_Con->par_alt_slp = %d", gp_Gpi_Par_Con->par_alt_slp);  
  Trace2("gp_Gpi_Par_Con->par_alt_rst = %d", gp_Gpi_Par_Con->par_alt_rst);  
  Trace2("gp_Gpi_Par_Con->par_alt_mmm = %d", gp_Gpi_Par_Con->par_alt_mmm);  
  Trace2("gp_Gpi_Par_Con->par_alt_ena = %d", gp_Gpi_Par_Con->par_alt_ena);  
  Trace2("gp_Gpi_Par_Con->par_alt_m2 = %d",  gp_Gpi_Par_Con->par_alt_m2)  ;  
  Trace2("gp_Gpi_Par_Con->par_alt_m1 = %d",  gp_Gpi_Par_Con->par_alt_m1)  ;  
  Trace2("gp_Gpi_Par_Con->par_alt_m0 = %d",  gp_Gpi_Par_Con->par_alt_m0)  ;  
  Trace2("gp_Gpi_Par_Con->par_azi_dir = %d", gp_Gpi_Par_Con->par_azi_dir) ; 
  Trace2("gp_Gpi_Par_Con->par_azi_clk = %d", gp_Gpi_Par_Con->par_azi_clk) ; 
  Trace2("gp_Gpi_Par_Con->par_azi_slp = %d", gp_Gpi_Par_Con->par_azi_slp) ;
  Trace2("gp_Gpi_Par_Con->par_azi_rst = %d", gp_Gpi_Par_Con->par_azi_rst);  
  Trace2("gp_Gpi_Par_Con->par_azi_mmm = %d", gp_Gpi_Par_Con->par_azi_mmm);  
  Trace2("gp_Gpi_Par_Con->par_azi_ena = %d", gp_Gpi_Par_Con->par_azi_ena);  
  Trace2("gp_Gpi_Par_Con->par_azi_m2 = %d",  gp_Gpi_Par_Con->par_azi_m2)  ;
  Trace2("gp_Gpi_Par_Con->par_azi_m1 = %d",  gp_Gpi_Par_Con->par_azi_m1)  ; 
  Trace2("gp_Gpi_Par_Con->par_azi_m0 = %d",  gp_Gpi_Par_Con->par_azi_m0)  ; 
  Trace2("=====================================================\n");
  Trace2("gp_I2c_Mcp->mcp_azi_dir = %d",  gp_I2c_Mcp->mcp_azi_dir)   ;
  Trace2("gp_I2c_Mcp->mcp_azi_clk = %d",  gp_I2c_Mcp->mcp_azi_clk)   ;
  Trace2("gp_I2c_Mcp->mcp_azi_slp = %d",  gp_I2c_Mcp->mcp_azi_slp)   ;
  Trace2("gp_I2c_Mcp->mcp_azi_rst = %d",  gp_I2c_Mcp->mcp_azi_rst)   ; 
  Trace2("gp_I2c_Mcp->mcp_azi_m2 = %d",   gp_I2c_Mcp->mcp_azi_m2)    ;
  Trace2("gp_I2c_Mcp->mcp_azi_m1 = %d",   gp_I2c_Mcp->mcp_azi_m1)    ;
  Trace2("gp_I2c_Mcp->mcp_azi_m0 = %d",   gp_I2c_Mcp->mcp_azi_m0)   ;
  Trace2("gp_I2c_Mcp->mcp_alt_dir = %d",  gp_I2c_Mcp->mcp_alt_dir)  ;  
  Trace2("gp_I2c_Mcp->mcp_alt_clk = %d",  gp_I2c_Mcp->mcp_alt_clk)  ;  
  Trace2("gp_I2c_Mcp->mcp_alt_slp = %d",  gp_I2c_Mcp->mcp_alt_slp)  ;  
  Trace2("gp_I2c_Mcp->mcp_alt_rst = %d",  gp_I2c_Mcp->mcp_alt_rst)  ;  
  Trace2("gp_I2c_Mcp->mcp_alt_m2 = %d",   gp_I2c_Mcp->mcp_alt_m2)   ;  
  Trace2("gp_I2c_Mcp->mcp_alt_m1 = %d",   gp_I2c_Mcp->mcp_alt_m1)   ;  
  Trace2("gp_I2c_Mcp->mcp_alt_m0 = %d",   gp_I2c_Mcp->mcp_alt_m0)   ;  
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

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if (getcwd(c_getcwd, sizeof(c_getcwd)) != NULL) {
     Trace("Current working dir: %s\n", c_getcwd);
  } else {
     perror("getcwd() error");
     return 1;
  }
  return 0 ;
} 
/*****************************************************************************************
* @fn     : CONFIG_FIC_READ
* @author : s.gravois
* @brief  : Cette fonction lit les parametres  dans le fichier de configuration
* @param  : lp_Con->con_params[CONFIG_DATAS_NB_LIGNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]
* @date   : 2022-01-20 creation entete de la fonction au format doxygen 
* @date   : 2022-11-02 correction bug quand espace apres 2 eme colonne (valeur parametre) avant \n (FIN_LIGNE)
* @todo   : voir si un passage par librairie JSON plus pratique (comme pour mesDep)
*****************************************************************************************/

int CONFIG_FIC_READ(STRUCT_CONFIG * lp_Con) {

  FILE * fin ;
  char buf   [CONFIG_TAILLE_BUFFER_256] ;
  char buffer[CONFIG_TAILLE_BUFFER_256] ;
  char c ;
  int i,C,L ;
  int i_trace_ligne_max=45 ;
  int i_Mot_Caractere_Lu_En_cours=0 ;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // FIXME : initialisation du tableau de valeurs lues dans le fichier = lp_Con->con_params

  Trace2(" CONFIG_DATAS_NB_LIGNES   = %d",   CONFIG_DATAS_NB_LIGNES) ;
  Trace2(" CONFIG_DATAS_NB_COLONNES = %d", CONFIG_DATAS_NB_COLONNES) ;

  for(L=0;L<CONFIG_DATAS_NB_LIGNES;L++) {
    for(C=0;C<CONFIG_DATAS_NB_COLONNES;C++) { 
      memset(lp_Con->con_params[L][C],CALCULS_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256-1);
    }
  }
  
  // FIXME : construction du chemin du fichier de configuration
  // FIXME : la variable gp_Con_Par->par_rep_home doit etre lue auparavant (getcwd) (2021)

  memset(buf,CALCULS_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256-1);
  sprintf(buf,"%s/%s",gp_Con_Par->par_rep_home, CONFIG_FIC_CFG) ;
  
  // FIXME : ouverture du fichier de configuration


  if ( (fin=fopen(buf,"r")) == NULL) {
   memset(buffer,CALCULS_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256);
   sprintf(buffer,"Pbme ouverture %s",buf) ;
   //LOG(buffer) ;
   Trace("probleme ouverture 4 %s",buf) ; 
   exit(2) ;
  }
  else Trace("open %s ok", buf) ;

  memset(buf,CALCULS_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256-1);
  i_Mot_Caractere_Lu_En_cours=0;
  L=0;
  C=0;
  
  // FIXME : lecture du fichier de configuration 

  while(1) {
   
    c = fgetc(fin) ;

    if (L<i_trace_ligne_max) 
      Trace2("%-2d %-2d %-2d %-30s", L, C, i_Mot_Caractere_Lu_En_cours, buf) ; 

    if (CONFIG_FIN_FICHIER(c)) { 
      if (L<i_trace_ligne_max) 
        Trace2(" --- %-2d %-2d %-2d %-30s (lecture fin fichier & continue)", L, C, i_Mot_Caractere_Lu_En_cours, buf) ; 
      break ;
    }

    if (CONFIG_DEB_COM(c)) {
      while(!CONFIG_FIN_LIGNE(fgetc(fin)));
      L++;
      C=0;
    }

    if ( CONFIG_FIN_MOT(c) ) {

      if (L<i_trace_ligne_max) 
        Trace2(" --- %-2d %-2d %-2d %-30s lecture fin mot", L, C, i_Mot_Caractere_Lu_En_cours, buf) ; 

      /* Si un mot est en cours de lecture */
      if ( i_Mot_Caractere_Lu_En_cours>0 ) {
        memset(lp_Con->con_params[L][C],CALCULS_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256);
        strcpy(lp_Con->con_params[L][C],buf);
        memset(buf,CALCULS_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256);

        Trace1("lp_Con->con_params fin lig[%d][%d]=(%s)",L,C,lp_Con->con_params[L][C] );

        i_Mot_Caractere_Lu_En_cours=0;
        C++;
      }
    }
    if ((CONFIG_FIN_LIGNE(c))) {

      if (L<i_trace_ligne_max) 
        Trace2(" --- %-2d %-2d %-2d %-30s lecture fin ligne", L, C, i_Mot_Caractere_Lu_En_cours, buf) ; 

      /* Si un mot est en cours de lecture */
      if ( i_Mot_Caractere_Lu_En_cours>0 ) {
        memset(lp_Con->con_params[L][C],CALCULS_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256);
        strcpy(lp_Con->con_params[L][C],buf);
        memset(buf,CALCULS_ZERO_CHAR,CONFIG_TAILLE_BUFFER_256);

        Trace1("lp_Con->con_params fin lig[%d][%d]=(%s)",L,C,lp_Con->con_params[L][C] );

        i_Mot_Caractere_Lu_En_cours=0;
        L++;
        C=0;
      }
      else {
        /* Si deux parametres ont ete lus */
        if ( strlen( lp_Con->con_params[L][0] ) && strlen( lp_Con->con_params[L][1] ) ) {
          L++;
          C=0;
        }
        else {
          C=0;
        }
      }
    }
    
    if (CONFIG_FORMAT_ADMIS(c)){
      buf[i_Mot_Caractere_Lu_En_cours]=(char)c;
      i_Mot_Caractere_Lu_En_cours++;
    }
  }
  fclose(fin);

  return 0 ;
}
/*****************************************************************************************
* @fn     : CONFIG_FIC_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche le tabeau de parametres lu a partir du fichier de config
* @param  : char lp_Con->con_params[CONFIG_DATAS_NB_LIGNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void CONFIG_FIC_DISPLAY(STRUCT_CONFIG *lp_Con ) {
  int L=0, C=0 ;  
  char lc_Buffer[CONFIG_TAILLE_BUFFER_256]={0} ;
  char lc_Buffer_Tmp[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  for(L=0;L<CONFIG_DATAS_NB_LIGNES;L++) {
    memset(lc_Buffer,CONFIG_ZERO_CHAR,sizeof(lc_Buffer)) ;

    if ( CONFIG_DATAS_NB_COLONNES == 2 ) {
      if (strlen(lp_Con->con_params[L][C])) {
        sprintf(lc_Buffer,"%-33s%-33s",lp_Con->con_params[L][0], lp_Con->con_params[L][1]) ;
      }
    }
    else {
      for(C=0;C<CONFIG_DATAS_NB_COLONNES;C++) {
        if (strlen(lp_Con->con_params[L][C])) {
          if ( strlen(lc_Buffer)) {
            strcpy(lc_Buffer_Tmp,lc_Buffer) ;
            sprintf(lc_Buffer,"%-33s%-33s",lc_Buffer_Tmp,lp_Con->con_params[L][C] ) ;
          }
          else {
            sprintf(lc_Buffer,"%-33s",lp_Con->con_params[L][C] ) ;
          }
        }
      }
    }
    if ( strlen(lc_Buffer) ) {
      Trace1("%s", lc_Buffer) ;
    }
  }
  return ;
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
  const char * c_type = gc_hach_astre_types [ gp_Ast->ast_typ ] ;
  const char * c_mode = gc_hach_calcul_mode[ gp_Cal->cal_mode ] ;

  char  c_hhmmss_agh0[ 16] ;
  char  c_hhmmss_agh1[ 16] ;
  char  c_hhmmss_agh2[ 16] ;

  char  c_hhmmss_azi0[ 16] ;
  char  c_hhmmss_azi1[ 16] ;
  char  c_hhmmss_azi2[ 16] ;

  char  c_hhmmss_agh[ 16] ;
  char  c_hhmmss_asc[ 16] ;

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset( c_hhmmss_agh0, 0, sizeof(c_hhmmss_agh0) ) ;
  memset( c_hhmmss_agh1, 0, sizeof(c_hhmmss_agh1) ) ;
  memset( c_hhmmss_agh2, 0, sizeof(c_hhmmss_agh2) ) ;
  memset( c_hhmmss_agh, 0, sizeof(c_hhmmss_agh) ) ;
  memset( c_hhmmss_asc, 0, sizeof(c_hhmmss_asc) ) ;

  sprintf( c_hhmmss_agh, "%dh%dm%ds",   gp_Ast->AGHt.tim_HH,  gp_Ast->AGHt.tim_MM,  gp_Ast->AGHt.tim_SS  ) ;
  sprintf( c_hhmmss_asc,  "%dh%dm%ds",  gp_Ast->ASCt.tim_HH,   gp_Ast->ASCt.tim_MM,   gp_Ast->ASCt.tim_SS  ) ;

  sprintf( c_hhmmss_agh0, "%dh%dm%ds", gp_Ast->AGH0t.tim_HH, gp_Ast->AGH0t.tim_MM, gp_Ast->AGH0t.tim_SS  ) ;
  sprintf( c_hhmmss_agh1, "%dh%dm%ds", gp_Ast->AGH1t.tim_HH, gp_Ast->AGH1t.tim_MM, gp_Ast->AGH1t.tim_SS  ) ;
  sprintf( c_hhmmss_agh2, "%dh%dm%ds", gp_Ast->AGH2t.tim_HH, gp_Ast->AGH2t.tim_MM, gp_Ast->AGH2t.tim_SS  ) ;

  sprintf( c_hhmmss_azi0, "%dh%dm%ds", gp_Ast->AZI0t.tim_HH, gp_Ast->AZI0t.tim_MM, gp_Ast->AZI0t.tim_SS  ) ;
  sprintf( c_hhmmss_azi1, "%dh%dm%ds", gp_Ast->AZI1t.tim_HH, gp_Ast->AZI1t.tim_MM, gp_Ast->AZI1t.tim_SS  ) ;
  sprintf( c_hhmmss_azi2, "%dh%dm%ds", gp_Ast->AZI2t.tim_HH, gp_Ast->AZI2t.tim_MM, gp_Ast->AZI2t.tim_SS  ) ;

  Trace(" %s : infos         : %s", c_nom , gp_Ast->infos ) ;
  Trace(" %s : type          : %s", c_nom , c_type ) ;
  Trace(" %s : mode calcul   : %s", c_nom , c_mode ) ;
  Trace(" %s : latitude      : %.2f (deg) ", c_nom, gp_Lie->lie_lat    * CALCULS_UN_RADIAN_EN_DEGRES ) ;
  Trace(" %s : longitude     : %.2f (deg) ", c_nom, gp_Lie->lie_lon    * CALCULS_UN_RADIAN_EN_DEGRES ) ;
  Trace(" %s : vitesses      : %.2f (Va) %.2f (Vh)", c_nom, gp_Ast->Va,  gp_Ast->Vh ) ; 
  Trace(" %s : azimut        : %.2f (deg) ", c_nom, gp_Ast->a    * CALCULS_UN_RADIAN_EN_DEGRES ) ;
  Trace(" %s : altitude      : %.2f (deg) ", c_nom, gp_Ast->h    * CALCULS_UN_RADIAN_EN_DEGRES ) ;
  Trace(" %s : declinaison   : %.2f (deg) ", c_nom, gp_Ast->DEC  * CALCULS_UN_RADIAN_EN_DEGRES  ) ;
  Trace(" %s : ascension dro : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->ASC    * CALCULS_UN_RADIAN_EN_DEGRES, c_hhmmss_asc ) ;
  Trace(" %s : angle horaire : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH   * CALCULS_UN_RADIAN_EN_DEGRES, c_hhmmss_agh ) ;

  Trace1(" %s : Agh0          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH0  * CALCULS_UN_RADIAN_EN_DEGRES, c_hhmmss_agh0 ) ;
  Trace1(" %s : Agh1          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH1  * CALCULS_UN_RADIAN_EN_DEGRES, c_hhmmss_agh1 ) ;
  Trace1(" %s : Agh2          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AGH2  * CALCULS_UN_RADIAN_EN_DEGRES, c_hhmmss_agh2 ) ;

  Trace1(" %s : Azi0          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AZI0  * CALCULS_UN_RADIAN_EN_DEGRES, c_hhmmss_azi0 ) ;
  Trace1(" %s : Azi1          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AZI1  * CALCULS_UN_RADIAN_EN_DEGRES, c_hhmmss_azi1 ) ;
  Trace1(" %s : Azi2          : %.2f (deg) %s (HH.MM.SS)", c_nom, gp_Ast->AZI2  * CALCULS_UN_RADIAN_EN_DEGRES, c_hhmmss_azi2 ) ;

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

  TraceArbo(__func__,1,"start") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  TEMPS_AFFICHER(   gp_Tim ) ;
  LIEU_AFFICHER(    gp_Lie );
  CONFIG_AFFICHER_MODE_LONG(  ) ;
  VOUTE_AFFICHER(   gp_Vou ) ;

  TraceArbo(__func__,1,"end") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
}
/*****************************************************************************************
* @fn     : CONFIG_MENU_CHANGE_DETECT
* @author : s.gravois
* @brief  : Cette fonction detecte et affiche les changements 
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : a consolider et voir utilite
*****************************************************************************************/

void CONFIG_MENU_CHANGE_DETECT (void)  {

  char s_menu[256] ;

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset(s_menu, 0, sizeof(s_menu)) ;

  switch(gp_Sui->menu) {
     
    case MENU_AZIMUTAL             :strcpy( s_menu, "MENU_AZIMUTAL") ; break ; 
    case MENU_EQUATORIAL           :strcpy( s_menu, "MENU_EQUATORIAL") ; break ; 
    case MENU_MANUEL_BRUT          :strcpy( s_menu, "MENU_MANUEL_BRUT") ; break ; 
    case MENU_MANUEL_NON_ASSERVI   :strcpy( s_menu, "MENU_MANUEL_NON_ASSERVI") ; break ; 
    case MENU_MANUEL_ASSERVI       :strcpy( s_menu, "MENU_MANUEL_ASSERVI") ; break ; 
    case MENU_GOTO                 :strcpy( s_menu, "MENU_GOTO") ; break ; 
    case MENU_INFO                 :strcpy( s_menu, "MENU_INFO") ; break ; 
    case MENU_RESEAU_UP            :strcpy( s_menu, "MENU_RESEAU_UP") ; break ; 
    case MENU_RESEAU_DOWN          :strcpy( s_menu, "MENU_RESEAU_DOWN") ; break ; 
    case MENU_PROGRAMME_DOWN       :strcpy( s_menu, "MENU_PROGRAMME_DOWN") ; break ; 
    case MENU_DOWN                 :strcpy( s_menu, "MENU_DOWN") ; break ; 
  }

  if ( gp_Sui->menu_old != gp_Sui->menu ) {

    Trace("appel : %d : %s" , gp_Sui->menu, s_menu) ;
    GPIO_CLIGNOTE(gp_Gpi_Par_Pwm->par_led_etat, 1, 100) ;
  }

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

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  while( ++i < CONFIG_C_BIN_POSSIBLE_PATHS_LENGTH && i_retour == FALSE ) {
    sprintf( c_path, "%s/%s", c_Bin_Possible_Paths[i], cmd) ;
    Trace("Test chemin %s", c_path) ;
    if( access( c_path, F_OK ) == 0 ) { 
      i_retour = TRUE ; 
      strcpy( g_paths, c_path ) ;
    }
  }
  Trace("%s = %s", cmd, g_paths) ;

  return i_retour ; 
}

/*****************************************************************************************
* @fn     : CONFIG_FIC_VERIFY
* @author : s.gravois
* @brief  : Cette fonction verifie si le fichier de conf contient bien les parametres attendus
* @param  : char c
* @date   : 2022-11-02 creation
* @todo   : 
*****************************************************************************************/

void CONFIG_FIC_VERIFY(STRUCT_CONFIG *lp_Con) {

  int L, C, l ;  
  char lc_Buffer[CONFIG_TAILLE_BUFFER_256]={0} ;
  char lc_Buffer_Tmp[CONFIG_TAILLE_BUFFER_256]={0} ;
  int i_Ret=0 ;
  int i_Non_Trouve=1 ;

  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  for(l=0;l<CONFIG_ENUM_NUMERO_DERNIER_PARAMETRE_OBLIGATOIRE;l++) {
    i_Non_Trouve=TRUE ;
    /* On teste toutes les lignes du fichier a la recherche du pattern */
    for(L=0;L<CONFIG_DATAS_NB_LIGNES;L++) {
      C=0 ; 
      if ( strlen( lp_Con->con_params[L][C] ) ) {
        if ( strcmp( gc_Config_Params_Obligatoires[l],lp_Con->con_params[L][C] ) == 0 ) {
          i_Non_Trouve=FALSE ;
        }
      }
    }
    if (i_Non_Trouve ) {
      Trace("parametre %s non trouve dans fichier de configguration", gc_Config_Params_Obligatoires[l] ) ;
    }
  }

  return ;
}

//========================================================================================
// FIXME : FIN FICHIER
//========================================================================================



