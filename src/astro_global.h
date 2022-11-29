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
#               * remplacement fonctions Trace1 par Trace 
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
#               * ajout constantes char * en fonction gc_hach_suivi_menus
# mars  2022  | * ajout signe a structure STRUCT_TIME
#               * ajout c_si signe sous forme char
# avril 2022  | * ajout type pour gestion ecran LCD1602 + PCA8574
#               * ajout type enum pour les mois (affichage LCD)
#               * ajout tempo_voute et tempo_lcd_loop
#               * ajout c_hhmmss_agh etc... a strcuture STRUCT_ASTRE
#               * ajout mutex pth_mut->mut_dat pour proteger
#                 lecture / exriture sur cette donnee
#               * creation static char * c_Bin_Possible_Paths
#               * remplacement PATH_CMD_STTY par var globale
# mai 2022      * ajout var glob gc_config_path_cmd_stty
#               * ajout type enum pour les chemins de bin (/bin,/sbin,etc..)
#               * ajout enum pour le masque
#               * correction code tremios pour 'FIN'
#               * modif PTHREADS_MAX_THREADS a 20
#               * ajout gp_Tim_Par->tim_par_tpo_lcd_loop et gp_Tim_Par->tim_par_tpo_lcd_disp en remplacement
#                 de gp_Tim_Par->tim_par_tpo_LCD (remaniement affichage LCD)
# juin 2022     * ajout paramatres pid_azi et pid_alt 
#                 et autres params pour tenter un asservissement des frequences
#                 (voir code gpio.c et calcul.c sur les frequences et periodes)
#               * ajout gp_Pid_Par->par_pid_ech
#               * ajout structure PID 
# 29/09/2022    * ajout type enum et tableaux associes pour les 
#                 attributs de threads
# 30/09/2022    * ajout systeme de trace par arbo (ASTRO_LOG_DEBUG_ARBO_APPEL_FCTS)
# 
# 07/10/2022    * refonte complete du code (consolidation .c / .h )
#               * dispatch de tous les types dans leur .h respectif
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_GLOBAL_H
#define ASTRO_GLOBAL_H

#include <curses.h>
#include <errno.h>
#include <ctype.h>
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

#define TIME_MILLI_SEC    1000                // frequence d'une microseconde (pour les usleep et calculs)
#define TIME_MICRO_SEC    1000000             // frequence d'une microseconde (pour les usleep et calculs)
#define TIME_NANO_SEC     1000000000          // frequence d'une nanoseconde (pour les nanosleep et calculs) 


/*
#define GPIO_DEFAULT_ALT_MOTOR_GPIOS  "21,26,19,13"
#define GPIO_DEFAULT_AZI_MOTOR_GPIOS  "6,5,7,11"
#define GPIO_DEFAULT_MAS_MOTOR_ORDER  "3,0,2,1"
*/
#define GPIO_CURRENT_FONCTION          3      // 0 : triangle - 1 : sinus - 2 : mixte - 3  : sinus arrondi (best)
#define GPIO_CURRENT_FONCTION_PARAM0   0.65   // ce parametre deforme la fonction 2 et 3
#define GPIO_CURRENT_FONCTION_PARAM1   1.0    // ce parametre multiplie la fonction pour l'ajuster a 1 (5v)
#define GPIO_SUIVI_MAIN_ATTENTE_MAX    10     // attente maximum exprimee en seconde pour non blocage
#define GPIO_MICROPAS_MAX              500
#define GPIO_NB_MOTEURS                2
#define GPIO_NB_PHASES_PAR_MOTEUR      4
#define GPIO_FREQ_MAX                  1000.0
#define GPIO_VARG_STRING               115
#define GPIO_VARG_INT                  100
#define GPIO_VARG_CHAR                 99 
#define GPIO_TIMEOUT                   30 
#define GPIO_SIZE                      16
#define GPIO_FREQ_IR                   38000.0
#define GPIO_TEMPORISATION_PERCENT     0.5
#define GPIO_BUFFER_SIZE_256           256
#define GPIO_PATH                      "/sys/class/gpio" 
#define GPIO_TAILLE_BUFFER             16

#define STATS_ASS                      3  

#define MUTEX_GLO_ANG_LOCK            pthread_mutex_lock(   & gp_Ang->ang_mutex ) ;  
#define MUTEX_GLO_ANG_UNLOCK          pthread_mutex_unlock( & gp_Ang->ang_mutex ) ;
#define MUTEX_GLO_AST_LOCK            pthread_mutex_lock(   & gp_Ast->ast_mutex ) ;  
#define MUTEX_GLO_AST_UNLOCK          pthread_mutex_unlock( & gp_Ast->ast_mutex ) ; 
#define MUTEX_GLO_CAL_LOCK            pthread_mutex_lock(   & gp_Cal->cal_mutex ) ;  
#define MUTEX_GLO_CAL_UNLOCK          pthread_mutex_unlock( & gp_Cal->cal_mutex ) ;
#define MUTEX_GLO_CAT_LOCK            pthread_mutex_lock(   & gp_Cat->cat_mutex ) ;  
#define MUTEX_GLO_CAT_UNLOCK          pthread_mutex_unlock( & gp_Cat->cat_mutex ) ; 
#define MUTEX_GLO_COD_LOCK            pthread_mutex_lock(   & gp_Cod->cod_mutex ) ;  
#define MUTEX_GLO_COD_UNLOCK          pthread_mutex_unlock( & gp_Cod->cod_mutex ) ; 
#define MUTEX_GLO_CON_LOCK            pthread_mutex_lock(   & gp_Con->con_mutex ) ;  
#define MUTEX_GLO_CON_UNLOCK          pthread_mutex_unlock( & gp_Con->con_mutex ) ; 
#define MUTEX_GLO_DAT_LOCK            pthread_mutex_lock(   & gp_Dat->dat_mutex ) ;  
#define MUTEX_GLO_DAT_UNLOCK          pthread_mutex_unlock( & gp_Dat->dat_mutex ) ;
#define MUTEX_GLO_DEV_LOCK            pthread_mutex_lock(   & gp_Dev->dev_mutex ) ;  
#define MUTEX_GLO_DEV_UNLOCK          pthread_mutex_unlock( & gp_Dev->dev_mutex ) ;
#define MUTEX_GLO_KEY_LOCK            pthread_mutex_lock(   & gp_Key->key_mutex ) ;  
#define MUTEX_GLO_KEY_UNLOCK          pthread_mutex_unlock( & gp_Key->key_mutex ) ;
#define MUTEX_GLO_LCD_LOCK            pthread_mutex_lock(   & gp_Lcd->lcd_mutex ) ;  
#define MUTEX_GLO_LCD_UNLOCK          pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;
#define MUTEX_GLO_LIE_LOCK            pthread_mutex_lock(   & gp_Lie->lie_mutex ) ;  
#define MUTEX_GLO_LIE_UNLOCK          pthread_mutex_unlock( & gp_Lie->lie_mutex ) ;
#define MUTEX_GLO_LOG_LOCK            pthread_mutex_lock(   & gp_Log->log_mutex ) ;  
#define MUTEX_GLO_LOG_UNLOCK          pthread_mutex_unlock( & gp_Log->log_mutex ) ;
#define MUTEX_GLO_PID_LOCK            pthread_mutex_lock(   & gp_Pid->pid_mutex ) ;  
#define MUTEX_GLO_PID_UNLOCK          pthread_mutex_unlock( & gp_Pid->pid_mutex ) ;
#define MUTEX_GLO_MUT_LOCK            pthread_mutex_lock(   & gp_Mut->mut_mutex ) ;  
#define MUTEX_GLO_MUT_UNLOCK          pthread_mutex_unlock( & gp_Mut->mut_mutex ) ;
#define MUTEX_GLO_MOT_LOCK            pthread_mutex_lock(   & gp_Mot->mot_mutex ) ;
#define MUTEX_GLO_MOT_UNLOCK          pthread_mutex_unlock( & gp_Mot->mot_mutex ) ;
#define MUTEX_GLO_PHA_LOCK            pthread_mutex_lock(   & gp_Pha->pha_mutex ) ;
#define MUTEX_GLO_PHA_UNLOCK          pthread_mutex_unlock( & gp_Pha->pha_mutex ) ;
#define MUTEX_GLO_I2C_DEV_LOCK        pthread_mutex_lock(   & gp_I2c_Dev->i2c_dev_mutex ) ;
#define MUTEX_GLO_I2C_DEV_UNLOCK      pthread_mutex_unlock( & gp_I2c_Dev->i2c_dev_mutex ) ;
#define MUTEX_GLO_I2C_ACC_LOCK        pthread_mutex_lock(   & gp_I2c_Acc->i2c_acc_mag_mutex ) ;
#define MUTEX_GLO_I2C_ACC_UNLOCK      pthread_mutex_unlock( & gp_I2c_Acc->i2c_acc_mag_mutex ) ;
#define MUTEX_GLO_INF_LOCK            pthread_mutex_lock(   & gp_Inf->inf_mutex ) ;
#define MUTEX_GLO_INF_UNLOCK          pthread_mutex_unlock( & gp_Inf->inf_mutex ) ;
#define MUTEX_GLO_SUI_LOCK            pthread_mutex_lock(   & gp_Sui->sui_mutex ) ;  
#define MUTEX_GLO_SUI_UNLOCK          pthread_mutex_unlock( & gp_Sui->sui_mutex ) ;
#define MUTEX_GLO_FRE_LOCK            pthread_mutex_lock(   & gp_Fre->fre_mutex ) ;
#define MUTEX_GLO_FRE_UNLOCK          pthread_mutex_unlock( & gp_Fre->fre_mutex ) ;
#define MUTEX_GLO_PAS_LOCK            pthread_mutex_lock(   & gp_Pas->pas_mutex ) ;
#define MUTEX_GLO_PAS_UNLOCK          pthread_mutex_unlock( & gp_Pas->pas_mutex ) ;
#define MUTEX_GLO_STA_LOCK            pthread_mutex_lock(   & gp_Sta->sta_mutex ) ;
#define MUTEX_GLO_STA_UNLOCK          pthread_mutex_unlock( & gp_Sta->sta_mutex ) ;
#define MUTEX_GLO_PTH_LOCK            pthread_mutex_lock(   & gp_Pth->pth_mutex ) ;
#define MUTEX_GLO_PTH_UNLOCK          pthread_mutex_unlock( & gp_Pth->pth_mutex ) ;
#define MUTEX_GLO_TIM_LOCK            pthread_mutex_lock(   & gp_Tim->tim_mutex ) ;
#define MUTEX_GLO_TIM_UNLOCK          pthread_mutex_unlock( & gp_Tim->tim_mutex ) ;
#define MUTEX_GLO_TPO_LOCK            pthread_mutex_lock(   & gp_Tpo->tpo_mutex ) ;
#define MUTEX_GLO_TPO_UNLOCK          pthread_mutex_unlock( & gp_Tpo->tpo_mutex ) ;
#define MUTEX_GLO_VOU_LOCK            pthread_mutex_lock(   & gp_Vou->vou_mutex ) ;
#define MUTEX_GLO_VOU_UNLOCK          pthread_mutex_unlock( & gp_Vou->vou_mutex ) ;
#define MUTEX_GLO_AST_PAR_LOCK        pthread_mutex_lock(   & gp_Ast_Par->ast_par_mutex) ;  
#define MUTEX_GLO_AST_PAR_UNLOCK      pthread_mutex_unlock( & gp_Ast_Par->ast_par_mutex ) ; 
#define MUTEX_GLO_CAL_PAR_LOCK        pthread_mutex_lock(   & gp_Cal_Par->cal_par_mutex) ;  
#define MUTEX_GLO_CAL_PAR_UNLOCK      pthread_mutex_unlock( & gp_Cal_Par->cal_par_mutex ) ; 
#define MUTEX_GLO_CON_PAR_LOCK        pthread_mutex_lock(   & gp_Con_Par->con_par_mutex) ;  
#define MUTEX_GLO_CON_PAR_UNLOCK      pthread_mutex_unlock( & gp_Con_Par->con_par_mutex ) ;
#define MUTEX_GLO_DEV_PAR_LOCK        pthread_mutex_lock(   & gp_Dev_Par->dev_par_mutex ) ;  
#define MUTEX_GLO_DEV_PAR_UNLOCK      pthread_mutex_unlock( & gp_Dev_Par->dev_par_mutex ) ; 
#define MUTEX_GLO_LIE_PAR_LOCK        pthread_mutex_lock(   & gp_Lie_Par->lie_par_mutex ) ;  
#define MUTEX_GLO_LIE_PAR_UNLOCK      pthread_mutex_unlock( & gp_Lie_Par->lie_par_mutex ) ; 
#define MUTEX_GLO_PID_PAR_LOCK        pthread_mutex_lock(   & gp_Pid_Par->pid_par_mutex ) ;  
#define MUTEX_GLO_PID_PAR_UNLOCK      pthread_mutex_unlock( & gp_Pid_Par->pid_par_mutex ) ; 
#define MUTEX_GLO_TIM_PAR_LOCK        pthread_mutex_lock(   & gp_Tim_Par->tim_par_mutex ) ;  
#define MUTEX_GLO_TIM_PAR_UNLOCK      pthread_mutex_unlock( & gp_Tim_Par->tim_par_mutex ) ; 
#define MUTEX_GLO_GPIO_PAR_PWM_LOCK   pthread_mutex_lock(   & gp_Gpi_Par_Pwm->pha_mutex ) ;
#define MUTEX_GLO_GPIO_PAR_PWM_UNLOCK pthread_mutex_unlock( & gp_Gpi_Par_Pwm->pha_mutex ) ;
#define MUTEX_GLO_GPIO_PAR_MAT_LOCK   pthread_mutex_lock(   & gp_Gpi_Par_Mat->par_mutex ) ;
#define MUTEX_GLO_GPIO_PAR_MAT_UNLOCK pthread_mutex_unlock( & gp_Gpi_Par_Mat->par_mutex ) ;
#define MUTEX_GLO_GPIO_PAR_RAQ_LOCK   pthread_mutex_lock(   & gp_Gpi_Par_Raq->par_mutex ) ;
#define MUTEX_GLO_GPIO_PAR_RAQ_UNLOCK pthread_mutex_unlock( & gp_Gpi_Par_Raq->par_mutex ) ;
#define MUTEX_GLO_GPIO_PAR_CON_LOCK   pthread_mutex_lock(   & gp_Gpi_Par_Con->par_mutex ) ;
#define MUTEX_GLO_GPIO_PAR_CON_UNLOCK pthread_mutex_unlock( & gp_Gpi_Par_Con->par_mutex ) ;

#define MUTEX_LOC_ANG_LOCK            pthread_mutex_lock(   & lp_Ang->ang_mutex ) ;  
#define MUTEX_LOC_ANG_UNLOCK          pthread_mutex_unlock( & lp_Ang->ang_mutex ) ;
#define MUTEX_LOC_AST_LOCK            pthread_mutex_lock(   & lp_Ast->ast_mutex ) ;  
#define MUTEX_LOC_AST_UNLOCK          pthread_mutex_unlock( & lp_Ast->ast_mutex ) ; 
#define MUTEX_LOC_CAL_LOCK            pthread_mutex_lock(   & lp_Cal->cal_mutex ) ;  
#define MUTEX_LOC_CAL_UNLOCK          pthread_mutex_unlock( & lp_Cal->cal_mutex ) ;
#define MUTEX_LOC_CAT_LOCK            pthread_mutex_lock(   & lp_Cat->cat_mutex ) ;  
#define MUTEX_LOC_CAT_UNLOCK          pthread_mutex_unlock( & lp_Cat->cat_mutex ) ; 
#define MUTEX_LOC_COD_LOCK            pthread_mutex_lock(   & lp_Cod->cod_mutex ) ;  
#define MUTEX_LOC_COD_UNLOCK          pthread_mutex_unlock( & lp_Cod->cod_mutex ) ; 
#define MUTEX_LOC_CON_LOCK            pthread_mutex_lock(   & lp_Con->con_mutex ) ;  
#define MUTEX_LOC_CON_UNLOCK          pthread_mutex_unlock( & lp_Con->con_mutex ) ; 
#define MUTEX_LOC_DAT_LOCK            pthread_mutex_lock(   & lp_Dat->dat_mutex ) ;  
#define MUTEX_LOC_DAT_UNLOCK          pthread_mutex_unlock( & lp_Dat->dat_mutex ) ;
#define MUTEX_LOC_DEV_LOCK            pthread_mutex_lock(   & lp_Dev->dev_mutex ) ;  
#define MUTEX_LOC_DEV_UNLOCK          pthread_mutex_unlock( & lp_Dev->dev_mutex ) ;
#define MUTEX_LOC_KEY_LOCK            pthread_mutex_lock(   & lp_Key->key_mutex ) ;  
#define MUTEX_LOC_KEY_UNLOCK          pthread_mutex_unlock( & lp_Key->key_mutex ) ;
#define MUTEX_LOC_LCD_LOCK            pthread_mutex_lock(   & lp_Lcd->lcd_mutex ) ;  
#define MUTEX_LOC_LCD_UNLOCK          pthread_mutex_unlock( & lp_Lcd->lcd_mutex ) ;
#define MUTEX_LOC_LIE_LOCK            pthread_mutex_lock(   & lp_Lie->lie_mutex ) ;  
#define MUTEX_LOC_LIE_UNLOCK          pthread_mutex_unlock( & lp_Lie->lie_mutex ) ;
#define MUTEX_LOC_LOG_LOCK            pthread_mutex_lock(   & lp_Log->log_mutex ) ;  
#define MUTEX_LOC_LOG_UNLOCK          pthread_mutex_unlock( & lp_Log->log_mutex ) ;
#define MUTEX_LOC_PID_LOCK            pthread_mutex_lock(   & lp_Pid->pid_mutex ) ;  
#define MUTEX_LOC_PID_UNLOCK          pthread_mutex_unlock( & lp_Pid->pid_mutex ) ;
#define MUTEX_LOC_MUT_LOCK            pthread_mutex_lock(   & lp_Mut->mut_mutex ) ;  
#define MUTEX_LOC_MUT_UNLOCK          pthread_mutex_unlock( & lp_Mut->mut_mutex ) ;
#define MUTEX_LOC_MOT_LOCK            pthread_mutex_lock(   & lp_Mot->mot_mutex ) ;
#define MUTEX_LOC_MOT_UNLOCK          pthread_mutex_unlock( & lp_Mot->mot_mutex ) ;
#define MUTEX_LOC_PHA_LOCK            pthread_mutex_lock(   & lp_Pha->pha_mutex ) ;
#define MUTEX_LOC_PHA_UNLOCK          pthread_mutex_unlock( & lp_Pha->pha_mutex ) ;
#define MUTEX_LOC_I2C_DEV_LOCK        pthread_mutex_lock(   & lp_I2c_Dev->i2c_dev_mutex ) ;
#define MUTEX_LOC_I2C_DEV_UNLOCK      pthread_mutex_unlock( & lp_I2c_Dev->i2c_dev_mutex ) ;
#define MUTEX_LOC_I2C_ACC_LOCK        pthread_mutex_lock(   & lp_I2c_Acc->i2c_acc_mag_mutex ) ;
#define MUTEX_LOC_I2C_ACC_UNLOCK      pthread_mutex_unlock( & lp_I2c_Acc->i2c_acc_mag_mutex ) ;
#define MUTEX_LOC_INF_LOCK            pthread_mutex_lock(   & lp_Inf->inf_mutex ) ;
#define MUTEX_LOC_INF_UNLOCK          pthread_mutex_unlock( & lp_Inf->inf_mutex ) ;
#define MUTEX_LOC_SUI_LOCK            pthread_mutex_lock(   & lp_Sui->sui_mutex ) ;  
#define MUTEX_LOC_SUI_UNLOCK          pthread_mutex_unlock( & lp_Sui->sui_mutex ) ;
#define MUTEX_LOC_FRE_LOCK            pthread_mutex_lock(   & lp_Fre->fre_mutex ) ;
#define MUTEX_LOC_FRE_UNLOCK          pthread_mutex_unlock( & lp_Fre->fre_mutex ) ;
#define MUTEX_LOC_PAS_LOCK            pthread_mutex_lock(   & lp_Pas->pas_mutex ) ;
#define MUTEX_LOC_PAS_UNLOCK          pthread_mutex_unlock( & lp_Pas->pas_mutex ) ;
#define MUTEX_LOC_STA_LOCK            pthread_mutex_lock(   & lp_Sta->sta_mutex ) ;
#define MUTEX_LOC_STA_UNLOCK          pthread_mutex_unlock( & lp_Sta->sta_mutex ) ;
#define MUTEX_LOC_TIM_LOCK            pthread_mutex_lock(   & lp_Tim->tim_mutex ) ;
#define MUTEX_LOC_TIM_UNLOCK          pthread_mutex_unlock( & lp_Tim->tim_mutex ) ;
#define MUTEX_LOC_TPO_LOCK            pthread_mutex_lock(   & lp_Tpo->tpo_mutex ) ;
#define MUTEX_LOC_TPO_UNLOCK          pthread_mutex_unlock( & lp_Tpo->tpo_mutex ) ;
#define MUTEX_LOC_VOU_LOCK            pthread_mutex_lock(   & lp_Vou->vou_mutex ) ;
#define MUTEX_LOC_VOU_UNLOCK          pthread_mutex_unlock( & lp_Vou->vou_mutex ) ;
#define MUTEX_LOC_AST_PAR_LOCK        pthread_mutex_lock(   & lp_Ast_Par->ast_par_mutex) ;  
#define MUTEX_LOC_AST_PAR_UNLOCK      pthread_mutex_unlock( & lp_Ast_Par->ast_par_mutex ) ; 
#define MUTEX_LOC_CAL_PAR_LOCK        pthread_mutex_lock(   & lp_Cal_Par->cal_par_mutex) ;  
#define MUTEX_LOC_CAL_PAR_UNLOCK      pthread_mutex_unlock( & lp_Cal_Par->cal_par_mutex ) ; 
#define MUTEX_LOC_CON_PAR_LOCK        pthread_mutex_lock(   & lp_Con_Par->con_par_mutex) ;  
#define MUTEX_LOC_CON_PAR_UNLOCK      pthread_mutex_unlock( & lp_Con_Par->con_par_mutex ) ;
#define MUTEX_LOC_DEV_PAR_LOCK        pthread_mutex_lock(   & lp_Dev_Par->dev_par_mutex ) ;  
#define MUTEX_LOC_DEV_PAR_UNLOCK      pthread_mutex_unlock( & lp_Dev_Par->dev_par_mutex ) ; 
#define MUTEX_LOC_LIE_PAR_LOCK        pthread_mutex_lock(   & lp_Lie_Par->lie_par_mutex ) ;  
#define MUTEX_LOC_LIE_PAR_UNLOCK      pthread_mutex_unlock( & lp_Lie_Par->lie_par_mutex ) ; 
#define MUTEX_LOC_PID_PAR_LOCK        pthread_mutex_lock(   & lp_Pid_Par->pid_par_mutex ) ;  
#define MUTEX_LOC_PID_PAR_UNLOCK      pthread_mutex_unlock( & lp_Pid_Par->pid_par_mutex ) ; 
#define MUTEX_LOC_TIM_PAR_LOCK        pthread_mutex_lock(   & lp_Tim_Par->tim_par_mutex ) ;  
#define MUTEX_LOC_TIM_PAR_UNLOCK      pthread_mutex_unlock( & lp_Tim_Par->tim_par_mutex ) ; 
#define MUTEX_LOC_GPIO_PAR_PWM_LOCK   pthread_mutex_lock(   & lp_Gpi_Par_Pwm->pha_mutex ) ;
#define MUTEX_LOC_GPIO_PAR_PWM_UNLOCK pthread_mutex_unlock( & lp_Gpi_Par_Pwm->pha_mutex ) ;
#define MUTEX_LOC_GPIO_PAR_MAT_LOCK   pthread_mutex_lock(   & lp_Gpi_Par_Mat->par_mutex ) ;
#define MUTEX_LOC_GPIO_PAR_MAT_UNLOCK pthread_mutex_unlock( & lp_Gpi_Par_Mat->par_mutex ) ;
#define MUTEX_LOC_GPIO_PAR_RAQ_LOCK   pthread_mutex_lock(   & lp_Gpi_Par_Raq->par_mutex ) ;
#define MUTEX_LOC_GPIO_PAR_RAQ_UNLOCK pthread_mutex_unlock( & lp_Gpi_Par_Raq->par_mutex ) ;
#define MUTEX_LOC_GPIO_PAR_CON_LOCK   pthread_mutex_lock(   & lp_Gpi_Par_Con->par_mutex ) ;
#define MUTEX_LOC_GPIO_PAR_CON_UNLOCK pthread_mutex_unlock( & lp_Gpi_Par_Con->par_mutex ) ;

typedef enum    t_en_Reduction_Type         ENUM_CALCULS_REDUCTION_TYPE ;
typedef enum    t_en_Calculs_Mode           ENUM_CALCULS_MODE ;

typedef struct  lirc_config                 INFRARED_LIRC_CONFIG ;

typedef struct  pthread_t                   STRUCT_PTHREAD_T ;
typedef struct  STR_ANGLE                   STRUCT_ANGLE ;
typedef struct  STR_ASTRE                   STRUCT_ASTRE ;
typedef struct  STR_CALCULS                 STRUCT_CALCULS ;
typedef struct  STR_CAT                     STRUCT_CAT ;
typedef struct  STR_CODES                   STRUCT_CODES ;
typedef struct  STR_CONFIG                  STRUCT_CONFIG ;
typedef struct  STR_DATAS                   STRUCT_DATAS ;
typedef struct  STR_DEVICES                 STRUCT_DEVICES ;
typedef struct  STR_KEYS                    STRUCT_KEYS ;
typedef struct  STR_LCD                     STRUCT_LCD ;
typedef struct  STR_LIEU                    STRUCT_LIEU ;
typedef struct  STR_LOG                     STRUCT_LOG ;
typedef struct  STR_PID                     STRUCT_PID ;
typedef struct  STR_MUTEXS                  STRUCT_MUTEXS ;
typedef struct  STR_PTHREADS                STRUCT_PTHREADS ;
typedef struct  STR_SUIVI                   STRUCT_SUIVI ;
typedef struct  STR_SUIVI_PAS               STRUCT_SUIVI_PAS ;
typedef struct  STR_SUIVI_FREQUENCES        STRUCT_SUIVI_FREQUENCES ;
typedef struct  STR_SUIVI_STATS             STRUCT_SUIVI_STATS ;
typedef struct  STR_TIME                    STRUCT_TIME ;
typedef struct  STR_TIME_TEMPOS             STRUCT_TIME_TEMPOS ;
typedef struct  STR_VOUTE                   STRUCT_VOUTE ;

typedef struct  STR_I2C_DEVICE              STRUCT_I2C_DEVICE ;
typedef struct  STR_I2C_ACC_MAG             STRUCT_I2C_ACC_MAG ;
typedef struct  STR_I2C_MCP23017            STRUCT_I2C_MCP23017 ;
typedef struct  STR_GPIO_PWM_PHASE          STRUCT_GPIO_PWM_PHASE ;
typedef struct  STR_GPIO_PWM_MOTEUR         STRUCT_GPIO_PWM_MOTEUR ;

typedef struct  STR_ASTRE_PARAMS            STRUCT_ASTRE_PARAMS ;
typedef struct  STR_CALCULS_PARAMS          STRUCT_CALCULS_PARAMS ;
typedef struct  STR_CONFIG_PARAMS           STRUCT_CONFIG_PARAMS ;
typedef struct  STR_DEVICES_PARAMS          STRUCT_DEVICES_PARAMS ;
typedef struct  STR_LIEU_PARAMS             STRUCT_LIEU_PARAMS ;
typedef struct  STR_PID_PARAMS              STRUCT_PID_PARAMS ;
typedef struct  STR_TIME_PARAMS             STRUCT_TIME_PARAMS ;
typedef struct  STR_GPIO_PARAMS_PWM         STRUCT_GPIO_PARAMS_PWM ;
typedef struct  STR_GPIO_PARAMS_MATRICIEL   STRUCT_GPIO_PARAMS_MAT ;
typedef struct  STR_GPIO_PARAMS_RAQUETTE    STRUCT_GPIO_PARAMS_RAQ ;
typedef struct  STR_GPIO_PARAMS_CONTROLER   STRUCT_GPIO_PARAMS_CON ;

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

#define MACRO_ASTRO_GLOBAL_EXTERN_INFRARED \
  extern INFRARED_LIRC_CONFIG * gp_LircConfig ; \

#define MACRO_ASTRO_GLOBAL_EXTERN_STRUCT \
  extern STRUCT_ANGLE            g_Angle,              *gp_Ang ; \
  extern STRUCT_ASTRE            g_Astre,              *gp_Ast ; \
  extern STRUCT_CALCULS          g_Calculs,            *gp_Cal ; \
  extern STRUCT_CAT              g_Catalogue,          *gp_Cat ; \
  extern STRUCT_CODES            g_Codes,              *gp_Cod ; \
  extern STRUCT_CONFIG           g_Config,             *gp_Con ; \
  extern STRUCT_DATAS            g_Datas,              *gp_Dat ; \
  extern STRUCT_DEVICES          g_Devices,            *gp_Dev ; \
  extern STRUCT_I2C_DEVICE       g_I2cDev,             *gp_I2c_Dev ; \
  extern STRUCT_I2C_ACC_MAG      g_I2cAcc,             *gp_I2c_Acc ; \
  extern STRUCT_I2C_MCP23017     g_I2cMcp,             *gp_I2c_Mcp ; \
  extern STRUCT_KEYS             g_Keys,               *gp_Key ; \
  extern STRUCT_LCD              g_Lcd,                *gp_Lcd ; \
  extern STRUCT_LIEU             g_Lieu,               *gp_Lie ; \
  extern STRUCT_LOG              g_Log,                *gp_Log; \
  extern STRUCT_PID              g_Pid,                *gp_Pid ; \
  extern STRUCT_MUTEXS           g_Mutexs,             *gp_Mut ; \
  extern STRUCT_PTHREADS         g_Pthreads,           *gp_Pth ; \
  extern STRUCT_SUIVI            g_Suivi,              *gp_Sui ; \
  extern STRUCT_SUIVI_PAS        g_Suivi_Pas,          *gp_Pas ; \
  extern STRUCT_SUIVI_FREQUENCES g_Suivi_Frequences,   *gp_Fre ; \
  extern STRUCT_SUIVI_STATS      g_Suivi_Statistiques, *gp_Sta ; \
  extern STRUCT_TIME             g_Time,               *gp_Tim ; \
  extern STRUCT_TIME_TEMPOS      g_Time_Tempos,        *gp_Tpo ; \
  extern STRUCT_VOUTE            g_Voute,              *gp_Vou ; \
  extern STRUCT_GPIO_PWM_MOTEUR  g_Mot_Alt,            *gp_Gpio_Pwm_Mot_Alt ; \
  extern STRUCT_GPIO_PWM_MOTEUR  g_Mot_Azi,            *gp_Gpio_Pwm_Mot_Azi ; \
  
#define MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS \
  extern STRUCT_ASTRE_PARAMS     g_Astre_Params,     *gp_Ast_Par ; \
  extern STRUCT_CALCULS_PARAMS   g_Calculs_Params,   *gp_Cal_Par ; \
  extern STRUCT_CONFIG_PARAMS    g_Config_Params,    *gp_Con_Par ; \
  extern STRUCT_DEVICES_PARAMS   g_Devices_Params,   *gp_Dev_Par ; \
  extern STRUCT_LIEU_PARAMS      g_Lieu_Params,      *gp_Lie_Par ; \
  extern STRUCT_PID_PARAMS       g_Pid_Params,       *gp_Pid_Par ; \
  extern STRUCT_TIME_PARAMS      g_Time_Params,      *gp_Tim_Par ; \
  extern STRUCT_GPIO_PARAMS_PWM  g_Gpio_Params_Pwm,  *gp_Gpi_Par_Pwm ; \
  extern STRUCT_GPIO_PARAMS_MAT  g_Gpio_Params_Mat,  *gp_Gpi_Par_Mat ; \
  extern STRUCT_GPIO_PARAMS_RAQ  g_Gpio_Params_Raq,  *gp_Gpi_Par_Raq ; \
  extern STRUCT_GPIO_PARAMS_CON  g_Gpio_Params_Con,  *gp_Gpi_Par_Con ; \


#define MACRO_ASTRO_GLOBAL_EXTERN_GPIOS \
  extern double gd_gpio_frequence_pwm ; \
  extern int    gi_gpio_fd     [ GPIO_SIZE ] ; \
  extern int    gi_gpio_in   	 [ GPIO_SIZE]  ; \
  extern int    gi_gpio_out    [ GPIO_SIZE] ; \
  extern int    gi_gpio_alt    [ GPIO_NB_PHASES_PAR_MOTEUR ]  ; \
  extern int    gi_gpio_azi    [ GPIO_NB_PHASES_PAR_MOTEUR ] ; \
  extern int    gi_gpio_mas    [ GPIO_NB_PHASES_PAR_MOTEUR ] ; \
  extern double gd_gpio_frequence_pwm ; \
  extern int    gi_gpio_timeout ; \
  extern int    gi_gpio_max_nb_pas ; \
  extern int    gi_gpio_max_nb_upas ; \

#define MACRO_ASTRO_GLOBAL_EXTERN_PTHREADS \
  extern int    gi_pth_numero ; \
  extern int    gi_pthread_getuid ; \
  extern int    gi_pthread_nb_cpu ; \

#define MACRO_ASTRO_GLOBAL_EXTERN_PID \
  extern int    gi_pid_trace ; \
  extern int    gi_pid_trace_alt ; \
  extern int    gi_pid_trace_azi ; \

#define MACRO_ASTRO_GLOBAL_EXTERN_CONFIG \
  extern char   gc_config_path_cmd_stty[ CONFIG_TAILLE_BUFFER_32 ] ; \

#define MACRO_ASTRO_GLOBAL_EXTERN_CONST \
  extern const char * gc_hach_suivi_menus[]  ; \

void ASTRO_GLOBAL_INIT(void) ;

#endif

