/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
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
# 17/01/2022  | * ajout gestion touches clavier (*_CLAVIER)
#               * nouveaux fichiers astro_keyboard.h / .c
# 18/01/2022  | * ajout parametre gp_Con_Par->par_rep_scr
#               * ajout parametre gp_Con_Par->par_src_ker
#               * remplacement fonctions TRACE par Trace 
#               * refonte des macros de Trace
# 19/01/2022  | * ajout ASTRE_INDETERMINE pour pouvoir calculer
#                 meme sans nom d as => cela permettra a terme
#                 d'entrer l azimut et l altitude directement
#                 pour effectuer le gp_Sui
# 20/01/2022  | * ajout de tous les types d 'as a t_en_Astre_Type
#               * ajout d'un enum t_en_Calculs_Mode
#               * ajout structure STRUCT_DEVICES et var don, *devices ;
#               * ajout CONFIG_AZI et CONFIG_EQU
# 21/01/2022  | * ajout constantes char * en fonction des enum
#                 pour avoir une representation char de l enum
#               * ajout ASCx comme STRUCT_TIME dans STRUCT_ASTRE
# 23/01/2022  | * suppression MODE_EQUATORIAL
#               * changement type gp_Con_Par->par_default_menu
#               * ajout constantes char * en fonction c_Menus
# mars  2022  | * ajout signe a structure STRUCT_TIME
#               * ajout c_si signe sous forme char
# avril 2022  | * ajout type pour gestion ecran LCD1602 + PCA8574
#               * ajout type enum pour les mois (affichage LCD)
#               * ajout temporisation_voute et temporisation_lcd_loop
#               * ajout c_hhmmss_agh etc... a strcuture STRUCT_ASTRE
#               * ajout mutex pth_mut->mut_dat pour proteger
#                 lecture / exriture sur cette donnee
#               * creation static char * c_Bin_Possible_Paths
#               * remplacement PATH_CMD_STTY par var globale
# mai 2022      * ajout var glob g_Path_Cmd_Stty
#               * ajout type enum pour les chemins de bin (/bin,/sbin,etc..)
#               * ajout enum pour le masque
#               * correction code tremios pour 'FIN'
#               * modif PTHREADS_MAX_THREADS a 20
#               * ajout gp_Tim_Par->par_tempo_Lcd_Loop et gp_Tim_Par->par_tempo_Lcd_Disp en remplacement
#                 de gp_Tim_Par->par_tempo_LCD (remaniement affichage LCD)
# juin 2022     * ajout paramatres pid_azi et pid_alt 
#                 et autres params pour tenter un asservissement des frequences
#                 (voir code gpio.c et calcul.c sur les frequences et periodes)
#               * ajout gp_Pid_Par->par_pid_ech
#               * ajout structure PID 
# 29/09/2022    * ajout type enum et tableaux associes pour les 
#                 attributs de threads
# 30/09/2022    * ajout systeme de trace par arbo (DEBUG_ARBO)
# 
# 07/10/2022    * refonte complete du code (consolidation .c / .h )
#               * dispatch de tous les types dans leur .h respectif
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_GLOBAL_H
#define ASTRO_GLOBAL_H

#include <curses.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <ncurses.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/i2c-dev.h>

#include "lirc_client.h"
#include "libfahw.h"

/*------------------------------------------------------------------------------*/
/* Definition des constantes globales                                           */ 
/* Ces constantes sont definies ici pour eviter les tests d inclusion multiples */
/*------------------------------------------------------------------------------*/

#define CODES_CODE_BUFFER_SIZE               255 
#define CODES_CODE_NB_CODES                  70 
#define CODES_NB_IN_OUT                      3

#define CONFIG_ZERO_CHAR                     '\0'
#define CONFIG_FIC_CFG                       "config.txt"
#define CONFIG_MES                           "MES"
#define CONFIG_NGC                           "NGC"
#define CONFIG_ETO                           "ETO"
#define CONFIG_PLA                           "PLA"
#define CONFIG_AZI                           "AZI"
#define CONFIG_EQU                           "EQU"
#define CONFIG_CAP                           "CAP"
#define CONFIG_TAILLE_BUFFER_4               4
#define CONFIG_TAILLE_BUFFER_5               5
#define CONFIG_TAILLE_BUFFER_8               8
#define CONFIG_TAILLE_BUFFER_16              16
#define CONFIG_TAILLE_BUFFER_32              32
#define CONFIG_TAILLE_BUFFER_64              64
#define CONFIG_TAILLE_BUFFER_256             256
#define CONFIG_C_BIN_POSSIBLE_PATHS_LENGTH   6
#define CONFIG_DATAS_NB_LIGNES               200 
#define CONFIG_DATAS_NB_COLONNES             2

#define GPIO_DEFAULT_ALT_MOTOR_GPIOS         "26,19,13,6"
#define GPIO_DEFAULT_AZI_MOTOR_GPIOS         "5,7,11,10"
#define GPIO_DEFAULT_MAS_MOTOR_ORDER         "3,0,2,1"

/*
#define GPIO_DEFAULT_ALT_MOTOR_GPIOS  "21,26,19,13"
#define GPIO_DEFAULT_AZI_MOTOR_GPIOS  "6,5,7,11"
#define GPIO_DEFAULT_MAS_MOTOR_ORDER  "3,0,2,1"
*/
#define GPIO_CURRENT_FONCTION          3       // 0 : triangle - 1 : sinus - 2 : mixte - 3  : sinus arrondi (best)
#define GPIO_CURRENT_FONCTION_PARAM0   0.65    // ce parametre deforme la fonction 2 et 3
#define GPIO_CURRENT_FONCTION_PARAM1   1.0     // ce parametre multiplie la fonction pour l'ajuster a 1 (5v)

#define GPIO_SUIVI_PWM_PHASE_PRIORITY  10
#define GPIO_SUIVI_MAIN_PRIORITY       5
#define GPIO_SUIVI_MAIN_ATTENTE_MAX    10     // attente maximum exprimee en seconde pour non blocage

#define GPIO_SUIVI_PWM_PHASE_SCHED   SCHED_RR
#define GPIO_SUIVI_MAIN_SCHED        SCHED_RR

#define GPIO_MICROPAS_MAX           500
// FIXME : creation entete de la fonction au format doxygen de la ligne suivante (2021)
// #define gp_Gpi_Par->par_fre_pwm          750 
#define GPIO_NB_PHASES_PAR_MOTEUR   4
#define GPIO_FREQ_MAX               1000.0
#define GPIO_VARG_STRING            115
#define GPIO_VARG_INT               100
#define GPIO_VARG_CHAR              99 
#define GPIO_TIMEOUT                30 
#define GPIO_SIZE                   16
#define GPIO_FREQ_IR                38000.0
#define GPIO_TEMPORISATION_PERCENT  0.5

#define GPIO_BUFFER_SIZE_256       256
#define GPIO_PATH                  "/sys/class/gpio" 
#define GPIO_TAILLE_BUFFER         16

typedef enum t_en_Reduction_Type            ENUM_CALCULS_REDUCTION_TYPE ;
typedef enum t_en_Calculs_Mode              ENUM_CALCULS_MODE ;
       
typedef struct  STR_TIME                    STRUCT_TIME ;
typedef struct  STR_ANGLE                   STRUCT_ANGLE ;
typedef struct  STR_ASTRE                   STRUCT_ASTRE ;
typedef struct  STR_CAT                     STRUCT_CAT ;
typedef struct  STR_CODES                   STRUCT_CODES ;
typedef struct  STR_CONFIG                  STRUCT_CONFIG ;
typedef struct  STR_DATAS                   STRUCT_DATAS ;
typedef struct  STR_DEVICES                 STRUCT_DEVICES ;
typedef struct  STR_I2C_DEVICE              STRUCT_I2C_DEVICE ;
typedef struct  STR_I2C_ACC_MAG             STRUCT_STR_I2C_ACC_MAG ;
typedef struct  STR_KEYS                    STRUCT_KEYS ;
typedef struct  STR_LCD                     STRUCT_LCD ;
typedef struct  STR_LIEU                    STRUCT_LIEU ;
typedef struct  STR_PID                     STRUCT_PID ;
typedef struct  STR_MUTEXS                  STRUCT_MUTEXS ;
typedef struct  STR_PTHREADS                STRUCT_PTHREADS ;
typedef struct  STR_SUIVI                   STRUCT_SUIVI ;
typedef struct  STR_VOUTE                   STRUCT_VOUTE ;
       
typedef struct  STR_GPIO_BROCHES            STRUCT_GPIO_BROCHES ;
typedef struct  STR_GPIO_PWM_PHASE          STRUCT_GPIO_PWM_PHASE ;
typedef struct  STR_GPIO_PWM_MOTEUR         STRUCT_GPIO_PWM_MOTEUR ;
       
typedef struct  STR_ASTRE_PARAMS            STRUCT_ASTRE_PARAMS ;
typedef struct  STR_CALCULS_PARAMS          STRUCT_CALCULS_PARAMS ;
typedef struct  STR_CONFIG_PARAMS           STRUCT_CONFIG_PARAMS ;
typedef struct  STR_DEVICES_PARAMS          STRUCT_DEVICES_PARAMS ;
typedef struct  STR_GPIO_PARAMS_PWM             STRUCT_GPIO_PARAMS ;
typedef struct  STR_PID_PARAMS              STRUCT_PID_PARAMS ;
       
typedef struct  STR_GPIO_PARAMS_MATRICIEL   STRUCT_GPIO_PAR_MAT ;
typedef struct  STR_GPIO_PARAMS_RAQUETTE    STRUCT_GPIO_PAR_RAQ ;


#include "astro_types.h"
#include "astro_config.h"
#include "astro_log.h"
#include "astro_time.h"
#include "astro_calculs.h"
#include "astro_arguments.h"
#include "astro_cat.h"
#include "astro_codes.h"
#include "astro_datas.h"
#include "astro_devices.h"
#include "astro_gpio.h"
#include "astro_i2c.h"
#include "astro_infrared.h"
#include "astro_keyboard.h"
#include "astro_keys.h"
#include "astro_lcd.h"
#include "astro_lieu.h"
#include "astro_main.h"
#include "astro_pid.h"
#include "astro_pthreads.h"
#include "astro_stats.h"
#include "astro_suivi.h"
#include "astro_voute.h"
#include "astro_astre.h"

#define MACRO_ASTRO_GLOBAL_EXTERN_STRUCT \
  extern STRUCT_ANGLE *gp_Ang ; \
  extern STRUCT_ASTRE *gp_Ast ; \
  extern STRUCT_CAT *gp_Cat ; \
  extern STRUCT_CODES *gp_Cod ; \
  extern STRUCT_DATAS *gp_Dat ; \
  extern STRUCT_DEVICES *gp_Dev ; \
  extern STRUCT_I2C_DEVICE *gp_I2c_Dev ; \
  extern STRUCT_STR_I2C_ACC_MAG *gp_I2c_Acc ; \
  extern STRUCT_KEYS *gp_Key ; \
  extern STRUCT_LCD *gp_Lcd ; \
  extern STRUCT_LIEU *gp_Lie ; \
  extern STRUCT_PID *gp_Pid ; \
  extern STRUCT_MUTEXS *gp_Mut ; \
  extern STRUCT_PTHREADS *gp_Pth ; \
  extern STRUCT_SUIVI *gp_Sui ; \
  extern STRUCT_TIME *gp_Tim ; \
  extern STRUCT_VOUTE *gp_Vou ; \
  extern STRUCT_GPIO_BROCHES *gp_Gpi_Bro ; \
  extern STRUCT_GPIO_PWM_MOTEUR *gp_Mot_Alt ; \
  extern STRUCT_GPIO_PWM_MOTEUR *gp_Mot_Azi ; \
  extern STRUCT_ASTRE_PARAMS *gp_Ast_Par ; \
  extern STRUCT_CONFIG_PARAMS *gp_Con_Par ; \
  extern STRUCT_DEVICES_PARAMS *gp_Dev_Par ; \
  extern STRUCT_GPIO_PARAMS *gp_Gpi_Par ; \
  extern STRUCT_CALCULS_PARAMS*gp_Cal_Par ; \
  extern STRUCT_GPIO_PAR_OLD *gp_Gpi_Par_Old ; \
  extern STRUCT_TIME_PARAMS *gp_Tim_Par ; \
  extern STRUCT_PID_PARAMS *gp_Pid_Par ; \

#define MACRO_ASTRO_GLOBAL_EXTERN_GPIOS \
  extern double gd_gpio_frequence_pwm ; \
  extern int    gi_gpio_fd     [ GPIO_SIZE ] ; \
  extern int    gi_gpio_in   	 [ GPIO_SIZE]  ; \
  extern int    gi_gpio_out    [ GPIO_SIZE] ; \
  extern int    gi_gpio_alt    [ GPIO_NB_PHASES_PAR_MOTEUR ]  ; \
  extern int    gi_gpio_azi    [ GPIO_NB_PHASES_PAR_MOTEUR ] ; \
  extern int    gi_gpio_mas    [ GPIO_NB_PHASES_PAR_MOTEUR ] ; \

#define MACRO_ASTRO_GLOBAL_EXTERN_GPIOS_OLD \

/*
#ifndef CONFIG_DATAS_NB_LIGNES
#define CONFIG_DATAS_NB_LIGNES 200
#endif

#ifndef CONFIG_DATAS_NB_COLONNES
#define CONFIG_DATAS_NB_COLONNES 2
#endif

#ifndef CONFIG_TAILLE_BUFFER_4
#define  CONFIG_TAILLE_BUFFER_4   4
#endif

#ifndef CONFIG_TAILLE_BUFFER_5
#define  CONFIG_TAILLE_BUFFER_5   5
#endif

#ifndef CONFIG_TAILLE_BUFFER_8
#define  CONFIG_TAILLE_BUFFER_8   8
#endif

#ifndef CONFIG_TAILLE_BUFFER_16
#define  CONFIG_TAILLE_BUFFER_16  16
#endif

#ifndef CONFIG_TAILLE_BUFFER_32
#define  CONFIG_TAILLE_BUFFER_32  32
#endif

#ifndef CONFIG_TAILLE_BUFFER_64
#define  CONFIG_TAILLE_BUFFER_64  64
#endif

#ifndef CONFIG_TAILLE_BUFFER_256
#define  CONFIG_TAILLE_BUFFER_256 256
#endif
*/

void GLOBAL_INIT(void) ;

#endif

