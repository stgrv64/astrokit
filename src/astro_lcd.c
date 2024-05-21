/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# ######################################################################
# rappel code concernant LCD dans config.c avant migration dans lcd.c :
# --------------------------------------------------------------
# avril 2002  |  * debut codage fonctionnalites LCD1602 :
#                * fonctions LCD_xxxx
#                * ajout mutex init sur mutex_datas_infrarouge
#                * => protection zone de code avec dat_inf
#                * ajout fonction LCD_DISPLAY_AGH_DEC / LCD_DISPLAY_EQU_DEC
#                * ASTRE_FORMATE_DONNEES_AFFICHAGE : ajout de 2 resolutions supplementaires
#                * protection zones de code impliquant LCD* (lcd_mutex)
# mai  2022      * reprise intégralité code pour utilisation pointeur fct (gp_Lcd->display_xxx)
# ######################################################################
# juin 2022   * creation
# juin 2022   * deport du code concernant LCD dans lcd.c
# -------------------------------------------------------------- 
*/

#include "astro_lcd.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;
MACRO_ASTRO_GLOBAL_EXTERN_TIME ;

/*
extern STRUCT_LCD     *gp_Lcd ;
extern STRUCT_ASTRE   *gp_Ast ;
extern STRUCT_DEVICES *gp_Dev ;
extern STRUCT_TIME    *gp_Tim ;
extern STRUCT_LIEU    *gp_Lie ;
extern STRUCT_LCD     *gp_Lcd ;
*/

/*****************************************************************************************
* @fn     : LCD_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_LCD *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void LCD_DISPLAY_PREPARE ( STRUCT_LCD * lp_Lcd) {

  TraceArbo(__func__,2,"astre format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Lcd->lcd_mutex ) ;

  sprintf( lp_Lcd->lcd_dis_cmd , STR_LCD_FORMAT_0,\
    lp_Lcd->lcd_l0_cur, \
    lp_Lcd->lcd_l1_cur, \
    lp_Lcd->lcd_l0_def, \
    lp_Lcd->lcd_l1_def  ) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Lcd->lcd_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static LCD_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_LCD *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void LCD_DISPLAY(STRUCT_LCD *lp_Lcd) {

  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,2,"display informations on Astre") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  LCD_DISPLAY_PREPARE( lp_Lcd ) ;

  ASTRO_GLOBAL_LOG_ON ( &  lp_Lcd->lcd_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Lcd->lcd_loglevel , 1 , "%s", lp_Lcd->lcd_dis_cmd ) ;
  ASTRO_GLOBAL_LOG_OFF( &  lp_Lcd->lcd_loglevel ) ;

  return ;
}
/*****************************************************************************************
* @fn     : LCD_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_LCD *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void LCD_LOCK ( STRUCT_LCD * lp_Lcd) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Lcd->lcd_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : LCD_LOG
* @author : s.gravois
* @brief  : Log la structure en parametre 
* @param  : STRUCT_LCD *
* @date   : 2023-01-24 creation
* @todo   : (a completer )
*****************************************************************************************/

static void LCD_LOG ( STRUCT_LCD * lp_Lcd) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Lcd->lcd_mutex ) ;

  /* TODO : complete */ 

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Lcd->lcd_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : LCD_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_LCD *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void LCD_UNLOCK ( STRUCT_LCD * lp_Lcd) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Lcd->lcd_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : LCD_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure lcd *l
* @param  : STRUCT_LIEU *gp_Lie
* @date   : 2022-04-08 creation
* @date   : 2022-04-08 ajout sauvegarde des lignes dans old
* @date   : 2022-04-27 mise en commentaire de TIME_CALCULS_SIDERAL_TIME
*****************************************************************************************/

void LCD_INIT(STRUCT_LCD * lp_Lcd) {

  TraceArbo(__func__,0,"init lcd") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /* Initialisation mutex */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Lcd->lcd_mutex ) ;
                                     lp_Lcd->lcd_log      = LCD_LOG ;
                                     lp_Lcd->lcd_lock     = LCD_LOCK ;
                                     lp_Lcd->lcd_unlock   = LCD_UNLOCK ;
                                     lp_Lcd->lcd_display  = LCD_DISPLAY ;
                                     lp_Lcd->lcd_loglevel = 0 ; 
                                     lp_Lcd->lcd_file     = NULL ;
  gettimeofday (                   & lp_Lcd->lcd_tval, NULL ) ;  
  /* Initialisation pointeurs de fonctions */

  Trace1("pointeurs intit") ;

  lp_Lcd->default_set     = LCD_DEFINE_DEFAULT ;
  lp_Lcd->default_refresh = LCD_REFRESH_DEFAULT ;

  lp_Lcd->default_display = LCD_DISPLAY_DEFAULT ;
  lp_Lcd->current_display = LCD_DISPLAY_CURRENT ;

  lp_Lcd->default_change = LCD_CHANGE_DEFAULT ;
  lp_Lcd->current_change = LCD_CHANGE_CURRENT ;

  lp_Lcd->display_str_str        = LCD_DISPLAY_STRING_STRING ;
  lp_Lcd->display_str_int        = LCD_DISPLAY_STRING_INT ;
  lp_Lcd->display_int_int        = LCD_DISPLAY_INT_INT ;
  
  lp_Lcd->display_tps_lie        = LCD_DISPLAY_TEMPS_LIEU ;
  lp_Lcd->display_ast_vit        = LCD_DISPLAY_ASTRE_VITESSES ;
  lp_Lcd->display_azi_alt        = LCD_DISPLAY_AZI_ALT ;
  lp_Lcd->display_agh_dec        = LCD_DISPLAY_AGH_DEC ;
  lp_Lcd->display_asc_dec        = LCD_DISPLAY_ASC_DEC ;
  lp_Lcd->display_ast_fre        = LCD_DISPLAY_AST_FREQUENCES ;
  lp_Lcd->display_ast_per        = LCD_DISPLAY_AST_PERIODES ;
  lp_Lcd->display_mod_ste        = LCD_DISPLAY_MODE_STELLARIUM ;
  lp_Lcd->display_gpios_alt_azi  = LCD_DISPLAY_CFG_GPIOS_ALT_AZI ;
  lp_Lcd->display_gpios_mas_fre  = LCD_DISPLAY_CFG_GPIOS_MAS_FRE ;
  lp_Lcd->display_gpios_leds     = LCD_DISPLAY_CFG_GPIOS_LEDS ;
  lp_Lcd->display_reductions     = LCD_DISPLAY_CFG_REDUCTION ;

  lp_Lcd->display_informations       = LCD_DISPLAY_INFORMATIONS ;
  lp_Lcd->display_acc_alt_azi        = LCD_DISPLAY_ACC_ALT_AZI ;

  /* Initialisation autres champs */

  Trace1("lcd init") ;

  memset( lp_Lcd->lcd_l0_cur, CONFIG_ZERO_CHAR, sizeof( lp_Lcd->lcd_l0_cur )) ;
  memset( lp_Lcd->lcd_l1_cur, CONFIG_ZERO_CHAR, sizeof( lp_Lcd->lcd_l1_cur )) ;
  memset( lp_Lcd->lcd_l0_def, CONFIG_ZERO_CHAR, sizeof( lp_Lcd->lcd_l0_def )) ;
  memset( lp_Lcd->lcd_l1_def, CONFIG_ZERO_CHAR, sizeof( lp_Lcd->lcd_l1_def )) ;

  Trace1("strcpy init") ;

  strcpy( lp_Lcd->lcd_l0_cur, "") ; 
  strcpy( lp_Lcd->lcd_l1_cur, "") ; 
  strcpy( lp_Lcd->lcd_l0_def, "") ; 
  strcpy( lp_Lcd->lcd_l1_def, "") ; 

  lp_Lcd->lcd_i2c_add = LCD_I2C_DEFAULT_DEV_PORT ; 
  
  /*--------------------------*/
  /* Initialisation ecran LCD */ 
  /*--------------------------*/

  Trace1("LCD1602Init init") ;

  if ( gp_Dev->dev_use_lcd ) {

    if ((lp_Lcd->lcd_fd = LCD1602Init(lp_Lcd->lcd_i2c_add)) == -1) {
      SyslogErr("Fail to init LCD1602Init\n");
      Trace("Fail to init LCD1602Init") ;
      return ;
    }
    else {
      Trace("LCD1602Init(ok)") ;
    }
  }
  else {
    Trace("Lcd non utilise") ;
  }
  /*--------------------------*/
  /* Affichage phrases init   */ 
  /*--------------------------*/

  Trace1("def phrases current") ;

  gp_Tim->tim_display(gp_Tim) ;

  /* FIXME : 20230308 :  amelioration affichage*/ 

  sprintf(lp_Lcd->lcd_l0_cur, "%d%s%s %s:%s", \
    gp_Tim->tim_yy , \
    gc_hach_time_Display_Mon [ gp_Tim->tim_mm ] , \
    gc_hach_time_Display_Day [ gp_Tim->tim_dd ] , \
    gc_hach_time_Display_Hou [ gp_Tim->tim_HH ] , \
    gc_hach_time_Display_Min [ gp_Tim->tim_MM ] ) ;

  sprintf(lp_Lcd->lcd_l1_cur, "%.2f %.2f", \
    gp_Lie->lie_lat * CALCULS_UN_RADIAN_EN_DEGRES, \
    gp_Lie->lie_lon * CALCULS_UN_RADIAN_EN_DEGRES ) ;

  /* ajout 26 mai 2022 */

  Trace("def phrases defaut") ;

  strcpy( lp_Lcd->lcd_l0_def, lp_Lcd->lcd_l0_cur ) ;
  strcpy( lp_Lcd->lcd_l1_def, lp_Lcd->lcd_l1_cur ) ;

  if ( gp_Dev->dev_use_lcd ) {

    Trace("LCD1602Clear") ;

    if ( LCD1602Clear(lp_Lcd->lcd_fd) == -1) {
      SyslogErr("Fail to LCD1602Clear\n");
      Trace("Fail to LCD1602Clear") ;
      return ;
    }
    else {
      Trace1("LCD1602Clear : (OK)") ;
    }
  
    usleep(LCD_USLEEP_AFTER_CLEARING) ;
  
    Trace("LCD1602DispLines") ;

    if (LCD1602DispLines(lp_Lcd->lcd_fd, lp_Lcd->lcd_l0_cur, lp_Lcd->lcd_l1_cur ) == -1) {
      SyslogErr("Fail to LCD1602DispLines\n");
      Trace("Fail to LCD1602DispLines");
    }
    else {
      Trace1("LCD1602DispLines : (ok)") ;
    }
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_CHANGE_CURRENT
* @author : s.gravois
* @brief  : Cette fonction change les valeurs 'courantes' d'affichage par les nouvelles
* @param  : void
* @date   : 2022-04-11 creation 
*****************************************************************************************/

void LCD_CHANGE_CURRENT(const int i_duree_us, const char* c_l0, const char * c_l1) {

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    pthread_mutex_lock( & gp_Lcd->lcd_mutex ) ;

    memset( gp_Lcd->lcd_l0_cur, CONFIG_ZERO_CHAR, sizeof( gp_Lcd->lcd_l0_cur )) ;
    memset( gp_Lcd->lcd_l1_cur, CONFIG_ZERO_CHAR, sizeof( gp_Lcd->lcd_l1_cur )) ;

    strcpy( gp_Lcd->lcd_l0_cur, c_l0 ) ;
    strcpy( gp_Lcd->lcd_l1_cur, c_l1 ) ;

    gp_Lcd->lcd_change_display_us = i_duree_us ;
    gp_Lcd->lcd_change_current = TRUE ; 

    pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;
  }
  return ;
}
/*****************************************************************************************
* @fn     : LCD_CHANGE_DEFAULT
* @author : s.gravois
* @brief  : Cette fonction change les valeurs d'affichage 'par default' 
* @param  : void
* @date   : 2022-05-28 creation en remplacement de 'depiler'
*****************************************************************************************/

void LCD_CHANGE_DEFAULT(const int i_duree_us, const char* c_l0, const char * c_l1) {

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    pthread_mutex_lock( & gp_Lcd->lcd_mutex ) ;

    memset( gp_Lcd->lcd_l0_def, 0, sizeof( gp_Lcd->lcd_l0_def )) ;
    memset( gp_Lcd->lcd_l1_def, 0, sizeof( gp_Lcd->lcd_l1_def )) ;

    /* OLD -> NEW ; au final OLD OLD */
    strcpy( gp_Lcd->lcd_l0_def, c_l0 ) ;
    strcpy( gp_Lcd->lcd_l1_def, c_l1 ) ;

    gp_Lcd->lcd_change_display_us = i_duree_us ;
    gp_Lcd->lcd_change_default = TRUE ;

    pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;
  }
  return ;
}
/*****************************************************************************************
* @fn     : LCD_DEFINE_DEFAULT
* @author : s.gravois
* @brief  : Cette fonction definit les valeurs d'affichage 'par default' c_l0 et c_l1
* @param  : void
* @date   : 2022-09-27 creation 
*****************************************************************************************/

void LCD_DEFINE_DEFAULT( char * c_l0,  char * c_l1) {

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    pthread_mutex_lock( & gp_Lcd->lcd_mutex ) ;

    /*---------------------------------------------------------------------------------*/
    /* les valeurs par defaut de la ffichage sont la date l heure et l objet en cours */
    /*---------------------------------------------------------------------------------*/

    TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;

/* modif 2023
    sprintf( c_l0, "%d%s%d %-2d:%-2d", \
      gp_Tim->tim_yy , \
      gc_hach_time_Display_Mon[ gp_Tim->tim_mm -1  ] , \
      gp_Tim->tim_dd, \
      gp_Tim->tim_HH, \
      gp_Tim->tim_MM ) ;
*/
    sprintf( c_l0, "%d%s%s %s:%s", \
      gp_Tim->tim_yy , \
      gc_hach_time_Display_Mon [ gp_Tim->tim_mm ] , \
      gc_hach_time_Display_Day [ gp_Tim->tim_dd ], \
      gc_hach_time_Display_Hou [ gp_Tim->tim_HH ], \
      gc_hach_time_Display_Min [ gp_Tim->tim_MM ] ) ;

    sprintf( c_l1, "%s", gp_Ast->ast_nom ) ;

    pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;
  }
  return ;
}
/*****************************************************************************************
* @fn     : LCD_REFRESH_DEFAULT
* @author : s.gravois
* @brief  : Cette fonction rafraichit affichage des valeurs d'affichage 'par default' 
* @brief  : si celles ci ont changé
* @param  : void
* @date   : 2022-09-27 creation
*****************************************************************************************/

void LCD_REFRESH_DEFAULT(void) {

  char c_l0[LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure] ={0};
  char c_l1[LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure] ={0};

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */  

  memset( c_l0, 0, sizeof( c_l0) ) ;
  memset( c_l1, 0, sizeof( c_l1) ) ;

  if ( gp_Dev->dev_use_lcd ) {

    pthread_mutex_lock( & gp_Lcd->lcd_mutex ) ;
    
    gp_Lcd->default_set( c_l0, c_l1 ) ;

    /* Si les valeurs definis par default ont change */ 

    if ( ( strcmp ( gp_Lcd->lcd_l0_def, c_l0 ) != 0 ) ||
         ( strcmp ( gp_Lcd->lcd_l1_def, c_l1 ) != 0 ) ) {

      gp_Lcd->default_change( 2000000, c_l0 , c_l1 ) ;
      gp_Lcd->default_display() ;
    }

    pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_DEFAULT
* @author : s.gravois
* @brief  : Cette fonction affiche les deux chaines 'default' de la struct LCD*  
* @brief  : fonction de BASE pour les autres fonctions
* @param  : LCD * gp_Lcd
* @date   : 2022-04-09 creation 
* @date   : 2022-04-28 suppression fonction LCD1602Init (fait dans LCD_INIT)
* @date   : 2022-04-28 protection par un mutex des donnees de lcd*
* @date   : 2022-05-28 reprise tout code lcd pour passage par pt fct
* @todo   : verifier la valeur de usleep (parametrer ?)
*****************************************************************************************/

void LCD_DISPLAY_DEFAULT(void) {

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("%d",gp_Dev->dev_use_lcd) ;

  if ( gp_Dev->dev_use_lcd ) {

    pthread_mutex_lock( & gp_Lcd->lcd_mutex ) ;

    if ( LCD1602Clear(gp_Lcd->lcd_fd) == -1) {
      SyslogErr("LCD1602Clear : FAILED\n");
      Trace("LCD1602Clear : FAILED");

      pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;

      return ;
    }
    else {
      Trace1("LCD1602Clear : OK")
    }
    pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;
    /* entre 2500 et 5000 semble une bonne valeur de usleep */
    /* si on ne fait pas de usleep , l ecran ne se clear pas completement (teste) */
    usleep( LCD_USLEEP_AFTER_CLEARING ) ;
    
    pthread_mutex_lock( & gp_Lcd->lcd_mutex ) ;

    if ( LCD1602DispLines(\
        gp_Lcd->lcd_fd, \
        gp_Lcd->lcd_l0_def, \
        gp_Lcd->lcd_l1_def ) \
        == -1) { 

      SyslogErr("LCD1602DispLines : FAILED\n");
      Trace("LCD1602DispLines : FAILED");
      
      pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;

      return ;
    }
    else {
      Trace1("LCD1602DispLines : OK");
    }
    pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_CURRENT
* @author : s.gravois
* @brief  : Cette fonction affiche les deux chaines 'current' de la struct LCD*  
* @brief  : fonction de BASE pour les autres fonctions
* @param  : LCD * gp_Lcd
* @date   : 2022-05-28 creation (reprise code fct LCD_DISPLAY_DEFAULT)
*****************************************************************************************/

void LCD_DISPLAY_CURRENT(void) {

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("%d",gp_Dev->dev_use_lcd) ;

  if ( gp_Dev->dev_use_lcd ) {

    pthread_mutex_lock( & gp_Lcd->lcd_mutex ) ;

    if ( LCD1602Clear(gp_Lcd->lcd_fd) == -1) {
      SyslogErr("LCD1602Clear : FAILED\n");
      Trace("LCD1602Clear : FAILED");

      pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;

      return ;
    }
    else {
      Trace1("LCD1602Clear : OK")
    }
    /* TODO  : mettre le usleep a interieur de mutex */
    pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;
    /* entre 2500 et 5000 semble une bonne valeur de usleep */
    /* si on ne fait pas de usleep , l ecran ne se clear pas completement (teste) */
    usleep( LCD_USLEEP_AFTER_CLEARING ) ;
    
    pthread_mutex_lock( & gp_Lcd->lcd_mutex ) ;

    if ( LCD1602DispLines(\
        gp_Lcd->lcd_fd, \
        gp_Lcd->lcd_l0_cur, \
        gp_Lcd->lcd_l1_cur ) \
        == -1) { 

      SyslogErr("LCD1602DispLines : FAILED\n");
      Trace("LCD1602DispLines : FAILED");
      
      pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;

      return ;
    }
    else {
      Trace1("LCD1602DispLines : OK");
    }
    pthread_mutex_unlock( & gp_Lcd->lcd_mutex ) ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_STRING_STRING
* @author : s.gravois
* @brief  : Cette fonction affiche deux chaines sur ecran LCD  
* @param  : LCD * lcd
* @param  : char* c_l0
* @param  : char* c_l1
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void LCD_DISPLAY_STRING_STRING( const int i_duree_us , const char* c_l0, const char * c_l1) {

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
  gp_Lcd->current_display() ;

  return ;
}
/*****************************************************************************************
* @fn     : LCD_DISPLAY_STRING_STRING
* @author : s.gravois
* @brief  : Cette fonction affiche une chaine et un entier sur ecran LCD  
* @param  : LCD * lcd
* @param  : char* c_l0
* @param  : char* c_l1
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void   LCD_DISPLAY_STRING_INT ( const int i_duree_us, const char* c_l0, const int i) {

  char c_l1[LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure] = {0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset(c_l1,0,sizeof(c_l1));
  sprintf( c_l1, "%d", i) ;

  gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
  gp_Lcd->current_display() ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_INT_INT
* @author : s.gravois
* @brief  : Cette fonction affiche une chaine et un entier sur ecran LCD  
* @param  : LCD * lcd
* @param  : char* c_l0
* @param  : char* c_l1
* @date   : 2022-05-30 creation 
*****************************************************************************************/

void   LCD_DISPLAY_INT_INT      ( const int i_duree_us, const int i_l0, const int i_l1) {

  char c_l0[LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure] = {0} ;
  char c_l1[LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure] = {0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    sprintf( c_l0, "%d", i_l0) ;
    sprintf( c_l1, "%d", i_l1) ;

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_TEMPS_LIEU
* @author : s.gravois
* @brief  : Cette fonction calcule le temps sideral en cours puis affiche sur LCD
* @brief  : la date et l heure ainsi que la latitude et la longitude
* @param  : LCD * lcd
* @param  : char* c_l0
* @param  : char* c_l1
* @date   : 2022-04-09 creation 
* @date   : 2022-04-27 corrrection longueur de c_l0
*****************************************************************************************/

void LCD_DISPLAY_TEMPS_LIEU( const int i_duree_us ) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0) ) ; 
    memset( c_l1, 0, sizeof(c_l1) ) ; 

    TIME_CALCULS_SIDERAL_TIME( gp_Tim, gp_Lie ) ;

    /* Remplissage de line 0 et line 1 */
/*
    sprintf( c_l0, "%d%s%d %d:%d", \
      gp_Tim->tim_yy ,\
      gc_hach_time_Display_Mon[ gp_Tim->tim_mm -1  ] , \
      gp_Tim->tim_dd, \
      gp_Tim->tim_HH, \
      gp_Tim->tim_MM ) ;
*/
    sprintf( c_l0, "%d%s%s %s:%s", \
      gp_Tim->tim_yy , \
      gc_hach_time_Display_Mon [ gp_Tim->tim_mm ] , \
      gc_hach_time_Display_Day [ gp_Tim->tim_dd ] , \
      gc_hach_time_Display_Hou [ gp_Tim->tim_HH ] , \
      gc_hach_time_Display_Min [ gp_Tim->tim_MM ] ) ;

    sprintf( c_l1, "%.2f %.2f", \
      gp_Lie->lie_lat * CALCULS_UN_RADIAN_EN_DEGRES, \
      gp_Lie->lie_lon * CALCULS_UN_RADIAN_EN_DEGRES ) ;

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_ASTRE_VITESSES
* @author : s.gravois
* @brief  : Cette fonction affiche le nom de l 'astre et les vitesses
* @param  : LCD * lcd
* @param  : char* c_l0
* @param  : char* c_l1
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void LCD_DISPLAY_ASTRE_VITESSES( const int i_duree_us ) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]  ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]  ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    strcpy(  c_l0, gp_Ast->ast_nom ) ;
    sprintf( c_l1,"%.2f %.2f", gp_Ast->ast_azi_vit, gp_Ast->ast_alt_vit);

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_AZI_ALT
* @author : s.gravois
* @brief  : Cette fonction affiche les coordonnees azimutales en cours
* @param  : LCD * lcd
* @param  : int i_duree_us
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void LCD_DISPLAY_AZI_ALT( const int i_duree_us ) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "AZI %s", gp_Ast->ast_azi_ddmm ) ;
    sprintf( c_l1, "ALT %s", gp_Ast->ast_alt_ddmm );

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_AGH_DEC
* @author : s.gravois
* @brief  : Cette fonction affiche les coordonnees horaires en cours
* @param  : LCD * lcd
* @param  : int i_duree_us
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-04-28 creation 
*****************************************************************************************/

void LCD_DISPLAY_AGH_DEC( const int i_duree_us) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]  ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]  ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "AGH %s", gp_Ast->ast_agh_hhmm ) ;
    sprintf( c_l1, "DEC %s", gp_Ast->ast_dec_ddmm ) ;

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_ASC_DEC
* @author : s.gravois
* @brief  : Cette fonction affiche les coordonnees horaires en cours
* @param  : LCD * lcd
* @param  : int i_duree_us
* @param  : STRUCT_ASTRE * gp_Ast
* @date   : 2022-04-28 creation 
*****************************************************************************************/

void LCD_DISPLAY_ASC_DEC( const int i_duree_us ) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "ASC %s", gp_Ast->ast_asc_hhmm ) ;
    sprintf( c_l1, "DEC %s", gp_Ast->ast_dec_ddmm ) ;

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_MODE_STELLARIUM
* @author : s.gravois
* @brief  : Cette fonction s inspire de ASTRE_STELLARIUM_VIEW pour le LCD
* @param  : 
* @date   : 2022-03-18 creation
* @date   : 2022-03-28 simplification
*****************************************************************************************/

void LCD_DISPLAY_MODE_STELLARIUM( const int i_duree_us ) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(AZI) %s", gp_Ast->ast_azi_ddmm ) ;
    sprintf( c_l1, "(ALT) %s", gp_Ast->ast_alt_ddmm );

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
    /*
    Trace("Va / Vh    : %3.2f / %3.2f" , gp_Ast->ast_azi_vit           , gp_Ast->ast_alt_vit ) ;
    Trace("AD / Dec   : %s / %s"       , gp_Ast->ast_asc_hhmmss , gp_Ast->ast_dec_ddmm ) ;
    Trace("AH / Dec   : %s / %s"       , gp_Ast->ast_agh_hhmmss , gp_Ast->ast_dec_ddmm ) ;
    Trace("AZ./ Haut. : %s / %s"       , gp_Ast->ast_azi_ddmm   , gp_Ast->ast_alt_ddmm ) ;
    */
  }
  return ;
}
/*****************************************************************************************
* @fn     : LCD_DISPLAY_INFORMATIONS
* @author : s.gravois
* @brief  : Cette fonction affiche les informations qu on souhaite
* @param  : LCD * lcd
* @param  : int i_duree_us
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void   LCD_DISPLAY_INFORMATIONS (  const int i_duree_us) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]  ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]  ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(INFO) ..." ) ;
    sprintf( c_l1, "(INFO) ..." );

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_CFG_GPIOS_ALT_AZI
* @author : s.gravois
* @brief  : Cette fonction affiche les GPIOS azimut et altitude lus dans le fichier de CFG
* @param  : LCD * gp_Lcd
* @param  : int i_duree_us
* @date   : 2022-05-30 creation 
*****************************************************************************************/

void   LCD_DISPLAY_CFG_GPIOS_ALT_AZI ( const int i_duree_us) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(h) %s", gp_Pwm_Par->gpi_pwm_par_alt_gpio ) ;
    sprintf( c_l1, "(a) %s", gp_Pwm_Par->gpi_pwm_par_azi_gpio );

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
  return ;
}
/*****************************************************************************************
* @fn     : LCD_DISPLAY_CFG_GPIOS_MAS_FRE
* @author : s.gravois
* @brief  : Cette fonction affiche le masque et frequence GPIOS
* @param  : LCD * gp_Lcd
* @param  : int i_duree_us
* @date   : 2022-05-30 creation 
* @todo   : TODO : prendre en compte nouveaux parametres en double (gpi_pwm_par_alt_gpio/gpi_pwm_par_azi_gpio ..)
*****************************************************************************************/

void   LCD_DISPLAY_CFG_GPIOS_MAS_FRE ( const int i_duree_us ) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]={0}  ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(Masq) %s", gp_Pwm_Par->gpi_pwm_par_alt_mask ) ;
    sprintf( c_l1, "(Fpwm) %s", gp_Pwm_Par->gpi_pwm_par_alt_fpwm );

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;

  }
  return ;
}
/*****************************************************************************************
* @fn     : LCD_DISPLAY_CFG_GPIOS_LEDS
* @author : s.gravois
* @brief  : Cette fonction affiche les informations concernant les LEDs
* @param  : LCD * gp_Lcd
* @param  : int i_duree_us
* @date   : 2022-05-30 creation 
* @todo   : recuperer led ir
*****************************************************************************************/

void   LCD_DISPLAY_CFG_GPIOS_LEDS    ( const int i_duree_us ) {

  FILE * f_led_ir ;
  char c_gets[ CONFIG_TAILLE_BUFFER_16 + 1 ] ;
  char*  pc_gets ;
  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    Trace("gp_Con_Par->con_par_fic_led = %s", gp_Con_Par->con_par_fic_led) ;

    /* Lecture LED IR */
    /* Le service systemd astrokit-boot / script_astrokit_boot.sh cree 
      normalement ce fichier */

    if ( ( f_led_ir = fopen( gp_Con_Par->con_par_fic_led, "r" ) ) != NULL ) {
      pc_gets = fgets( c_gets, CONFIG_TAILLE_BUFFER_16, f_led_ir ) ;
      if ( pc_gets != NULL ) {
        sprintf( c_l1, "(LED IR) %s", c_gets );
      }
      else {
        sprintf( c_l1, "(LED IR) cf logs" );
        Trace("fichier %s non lu", gp_Con_Par->con_par_fic_led) ;
      }
    }
    else {
      sprintf( c_l1, "(LED IR) cf logs" );
      Trace("erreur ouverture %s", gp_Con_Par->con_par_fic_led) ;
    }
    /* Lecture LED ETAT */

    sprintf( c_l0, "(LED ETAT) %d", gp_Pwm_Par->gpi_pwm_par_led_etat ) ;
    

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;

  }
  return ;
}
/*****************************************************************************************
* @fn     : LCD_DISPLAY_CFG_REDUCTION
* @author : s.gravois
* @brief  : Cette fonction affiche les informations concernant les LEDs
* @param  : LCD * gp_Lcd
* @param  : int i_duree_us
* @date   : 2022-05-30 creation 
*****************************************************************************************/

void   LCD_DISPLAY_CFG_REDUCTION    ( const int i_duree_us ) {

  int h1=gp_Cal_Par->cal_par_alt_red_1, h2= gp_Cal_Par->cal_par_alt_red_2, h3= gp_Cal_Par->cal_par_alt_red_3, h4= gp_Cal_Par->cal_par_alt_red_4, h5= gp_Cal_Par->cal_par_alt_red_5, h6= gp_Cal_Par->cal_par_alt_red_6 ;
  int a1=gp_Cal_Par->cal_par_azi_red_1, a2= gp_Cal_Par->cal_par_azi_red_2, a3= gp_Cal_Par->cal_par_azi_red_3, a4= gp_Cal_Par->cal_par_azi_red_4, a5= gp_Cal_Par->cal_par_azi_red_5, a6= gp_Cal_Par->cal_par_azi_red_6 ;

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]  ={0};

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "%d %d %d %d %d %d", h1,h2,h3,h4,h5,h6 ) ;
    sprintf( c_l1, "%d %d %d %d %d %d", a1,a2,a3,a4,a5,a6 );

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
  return ;
}
/*****************************************************************************************
* @fn     : LCD_DISPLAY_AST_FREQUENCES
* @author : s.gravois
* @brief  : Cette fonction affiche les informations concernant les frequences moteurs
* @param  : LCD * gp_Lcd
* @param  : int i_duree_us
* @date   : 2022-05-30 creation 
*****************************************************************************************/

void   LCD_DISPLAY_AST_FREQUENCES    ( const int i_duree_us) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(F alt) %.2f", gp_Fre->fre_fh_mic ) ;
    sprintf( c_l1, "(F azi) %.2f", gp_Fre->fre_fa_mic ) ;

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_AST_PERIODES
* @author : s.gravois
* @brief  : Cette fonction affiche les informations concernant les periodes moteurs
* @param  : LCD * gp_Lcd
* @param  : int i_duree_us
* @date   : 2022-05-30 creation 
*****************************************************************************************/

void   LCD_DISPLAY_AST_PERIODES      ( const int i_duree_us) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(T alt) %.2f", gp_Fre->fre_th_mic ) ;
    sprintf( c_l1, "(T azi) %.2f", gp_Fre->fre_ta_mic ) ;

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_ACC_ALT_AZI
* @author : s.gravois
* @brief  : Cette fonction affiche les valeurs d acceleration
* @param  : LCD * gp_Lcd
* @param  : int i_duree_us
* @date   : 2022-06-13 creation 
*****************************************************************************************/

void   LCD_DISPLAY_ACC_ALT_AZI     ( const int i_duree_us) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0}  ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  if ( gp_Dev->dev_use_lcd ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(Acc Alt) %.4f", gp_Pas->pas_acc_alt ) ;
    sprintf( c_l1, "(Acc Azi) %.4f", gp_Pas->pas_acc_azi ) ;

    gp_Lcd->current_change( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->current_display() ;
  }
  return ;
}

/* ----------------------------------------------------------------------

FIN FICHIER - FIN FICHIER  FIN FICHIER  FIN FICHIER  FIN FICHIER 

------------------------------------------------------------------------*/