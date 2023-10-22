/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-06-01  | * creation
# 2022-06-01  | * rapatriement code depuis astro_config.h
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_LCD_H
#define ASTRO_LCD_H

#include "astro_global.h"

#include "lirc_client.h"
#include "libfahw-pcf8574.h"

#define LCD_LINES_CHAR_NUMBERS        16
#define LCD_LINES_CHAR_NUMBERS_secure 8
#define LCD_USLEEP_AFTER_CLEARING     10000
#define LCD_I2C_DEFAULT_DEV_PORT      1

typedef enum {
  LCD_MONTH_1=0,
  LCD_MONTH_2,
  LCD_MONTH_3,
  LCD_MONTH_4,
  LCD_MONTH_5,
  LCD_MONTH_6,
  LCD_MONTH_7,
  LCD_MONTH_8,
  LCD_MONTH_9,
  LCD_MONTH_10,
  LCD_MONTH_11,
  LCD_MONTH_12
} t_en_Lcd_Display_Months ;

typedef enum {
  LCD_HOUR_0=0,
  LCD_HOUR_1,
  LCD_HOUR_2,
  LCD_HOUR_3,
  LCD_HOUR_4,
  LCD_HOUR_5,
  LCD_HOUR_6,
  LCD_HOUR_7,
  LCD_HOUR_8,
  LCD_HOUR_9,
  LCD_HOUR_10,
  LCD_HOUR_11,
  LCD_HOUR_12,
  LCD_HOUR_13,
  LCD_HOUR_14,
  LCD_HOUR_15,
  LCD_HOUR_16,
  LCD_HOUR_17,
  LCD_HOUR_18,
  LCD_HOUR_19,
  LCD_HOUR_20,
  LCD_HOUR_21,
  LCD_HOUR_22,
  LCD_HOUR_23,
  LCD_HOUR_24
}
t_en_Lcd_Display_Hours ;

typedef enum {
  LCD_MIN_0=0, LCD_MIN_1,  LCD_MIN_2,  LCD_MIN_3,  LCD_MIN_4,  LCD_MIN_5,
  LCD_MIN_6,   LCD_MIN_7,  LCD_MIN_8,  LCD_MIN_9,  LCD_MIN_10, LCD_MIN_11,
  LCD_MIN_12,  LCD_MIN_13, LCD_MIN_14, LCD_MIN_15, LCD_MIN_16, LCD_MIN_17, 
  LCD_MIN_18,  LCD_MIN_19, LCD_MIN_20, LCD_MIN_21, LCD_MIN_22, LCD_MIN_23,
  LCD_MIN_24,  LCD_MIN_25, LCD_MIN_26, LCD_MIN_27, LCD_MIN_28, LCD_MIN_29,
  LCD_MIN_30,  LCD_MIN_31, LCD_MIN_32, LCD_MIN_33, LCD_MIN_34, LCD_MIN_35,
  LCD_MIN_36,  LCD_MIN_37, LCD_MIN_38, LCD_MIN_39, LCD_MIN_40, LCD_MIN_41, 
  LCD_MIN_42,  LCD_MIN_43, LCD_MIN_44, LCD_MIN_45, LCD_MIN_46, LCD_MIN_47,
  LCD_MIN_48,  LCD_MIN_49, LCD_MIN_50, LCD_MIN_51, LCD_MIN_52, LCD_MIN_53,
  LCD_MIN_54,  LCD_MIN_55, LCD_MIN_56, LCD_MIN_57, LCD_MIN_58, LCD_MIN_59
}
t_en_Lcd_Display_Minutes ;

typedef enum {
 LCD_AFFICHAGE_DEFINITIF=0,
 LCD_AFFICHAGE_PROVISOIRE
}
t_en_Lcd_Type_Affichage ; 

typedef struct STR_LCD STRUCT_LCD ;

struct STR_LCD {

  pthread_mutex_t  lcd_mutex ;
  STR_EXT_TIMEVAL  lcd_tval ; 
  FILE            *lcd_file ; 
  void           (*lcd_log)     ( STRUCT_LCD * ) ;
  void           (*lcd_display) ( STRUCT_LCD * ) ;
  void           (*lcd_lock)    ( STRUCT_LCD * ) ;
  void           (*lcd_unlock)  ( STRUCT_LCD * ) ;  
  int              lcd_loglevel ;
  char             lcd_dis_for [ CONFIG_TAILLE_BUFFER_256 ] ;
  char             lcd_dis_cmd [ CONFIG_TAILLE_BUFFER_256 ] ;

  int  lcd_fd ; 
  int  lcd_i2c_add ; 
  char lcd_l0_cur[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ;
  char lcd_l1_cur[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ;
  char lcd_l0_def[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ;
  char lcd_l1_def[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ;
  int  lcd_change_current ;
  int  lcd_change_default ;
  int  lcd_change_display_us ; 

  void (*default_set)  (char * ,char * )  ;

  void (*current_change)  (const int, const char*, const char*) ;
  void (*current_display) (void) ;
  
  void (*default_change)  (const int, const char*, const char*) ;
  void (*default_refresh)  (void) ;
  void (*default_display) (void) ;

  void (*display_str_str) ( const int , const char* , const char * );
  void (*display_str_int) ( const int , const char* , int );
  void (*display_int_int) ( const int , const int   , const int );
  void (*display_str_lng) ( const int , const char* , const long );
  void (*display_tps_lie)           ( const int ) ;
  void (*display_ast_vit)           ( const int ) ;
  void (*display_azi_alt)           ( const int ) ;
  void (*display_agh_dec)           ( const int ) ;
  void (*display_asc_dec)           ( const int ) ;
  void (*display_mod_ste)           ( const int ) ;
  void (*display_ast_fre)           ( const int ) ;
  void (*display_ast_per)           ( const int ) ;
  void (*display_gpios_alt_azi) ( const int ) ;
  void (*display_gpios_mas_fre) ( const int ) ;
  void (*display_gpios_leds)    ( const int ) ;
  void (*display_reductions)     ( const int ) ;
  void (*display_informations)      ( const int ) ;
  void (*display_acc_alt_azi)       ( const int ) ;
}  ;

void   LCD_INIT                      (STRUCT_LCD *) ;

void   LCD_DEFINE_DEFAULT            ( char * ,  char * ) ;
void   LCD_REFRESH_DEFAULT           (void) ;

void   LCD_DISPLAY_DEFAULT           ( void ) ;
void   LCD_DISPLAY_CURRENT           ( void ) ;

void   LCD_CHANGE_CURRENT            ( const int, const char* , const char * ) ;
void   LCD_CHANGE_DEFAULT            ( const int, const char* , const char * ) ;

void   LCD_DISPLAY_STRING_STRING     ( const int , const char* , const char * ) ;
void   LCD_DISPLAY_STRING_INT        ( const int , const char* , const int ) ;
void   LCD_DISPLAY_INT_INT           ( const int , const int, const int ) ;
void   LCD_DISPLAY_TEMPS_LIEU        ( const int ) ;
void   LCD_DISPLAY_ASTRE_VITESSES    ( const int ) ;
void   LCD_DISPLAY_AZI_ALT           ( const int ) ;
void   LCD_DISPLAY_AGH_DEC           ( const int ) ;
void   LCD_DISPLAY_ASC_DEC           ( const int ) ;
void   LCD_DISPLAY_MODE_STELLARIUM   ( const int ) ;
void   LCD_DISPLAY_CFG_GPIOS_ALT_AZI ( const int ) ;
void   LCD_DISPLAY_CFG_GPIOS_MAS_FRE ( const int ) ;
void   LCD_DISPLAY_CFG_GPIOS_LEDS    ( const int ) ;
void   LCD_DISPLAY_CFG_REDUCTION     ( const int ) ;
void   LCD_DISPLAY_AST_FREQUENCES    ( const int ) ;
void   LCD_DISPLAY_AST_PERIODES      ( const int ) ;
void   LCD_DISPLAY_INFORMATIONS      ( const int ) ;
void   LCD_DISPLAY_ACC_ALT_AZI       ( const int ) ;

#endif