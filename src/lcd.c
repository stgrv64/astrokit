#define _GNU_SOURCE

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
#                * CONFIG_FORMATE_DONNEES_AFFICHAGE : ajout de 2 resolutions supplementaires
#                * protection zones de code impliquant LCD* (mutex_lcd)
# mai  2022      * reprise intégralité code pour utilisation pointeur fct (gp_Lcd->display_xxx)
# ######################################################################
# juin 2022   * creation
# juin 2022   * deport du code concernant LCD dans lcd.c
# -------------------------------------------------------------- 
*/

#include <lcd.h>
#include <pid.h>

/*****************************************************************************************
* @fn     : LCD_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure lcd *l
* @param  : LIEU *gp_Lie
* @date   : 2022-04-08 creation
* @date   : 2022-04-08 ajout sauvegarde des lignes dans old
* @date   : 2022-04-27 mise en commentaire de CALCUL_TEMPS_SIDERAL
*****************************************************************************************/

void LCD_INIT(LCD * p_lcd) {

  Trace("") ;

  /* Initialisation mutex */

  pthread_mutex_init( & p_lcd->mutex_lcd, NULL ) ;
  
  /* Initialisation pointeurs de fonctions */

  p_lcd->define_default  = LCD_DEFINE_DEFAULT ;
  p_lcd->refresh_default = LCD_REFRESH_DEFAULT ;

  p_lcd->display_default = LCD_DISPLAY_DEFAULT ;
  p_lcd->display_current = LCD_DISPLAY_CURRENT ;

  p_lcd->change_default = LCD_CHANGE_DEFAULT ;
  p_lcd->change_current = LCD_CHANGE_CURRENT ;

  p_lcd->display_str_str = LCD_DISPLAY_STRING_STRING ;
  p_lcd->display_str_int = LCD_DISPLAY_STRING_INT ;
  p_lcd->display_int_int = LCD_DISPLAY_INT_INT ;
  /* p_lcd->display_str_lng =  */
  p_lcd->display_tps_lie = LCD_DISPLAY_TEMPS_LIEU ;
  p_lcd->display_ast_vit = LCD_DISPLAY_ASTRE_VITESSES ;
  p_lcd->display_azi_alt = LCD_DISPLAY_AZI_ALT ;
  p_lcd->display_agh_dec = LCD_DISPLAY_AGH_DEC ;
  p_lcd->display_asc_dec = LCD_DISPLAY_ASC_DEC ;

  p_lcd->display_ast_fre = LCD_DISPLAY_AST_FREQUENCES ;
  p_lcd->display_ast_per = LCD_DISPLAY_AST_PERIODES ;

  p_lcd->display_mod_ste = LCD_DISPLAY_MODE_STELLARIUM ;

  p_lcd->display_cfg_gpios_alt_azi  = LCD_DISPLAY_CFG_GPIOS_ALT_AZI ;
  p_lcd->display_cfg_gpios_mas_fre  = LCD_DISPLAY_CFG_GPIOS_MAS_FRE ;
  p_lcd->display_cfg_gpios_leds     = LCD_DISPLAY_CFG_GPIOS_LEDS ;
  p_lcd->display_cfg_reduction      = LCD_DISPLAY_CFG_REDUCTION ;

  p_lcd->display_informations       = LCD_DISPLAY_INFORMATIONS ;
  p_lcd->display_acc_alt_azi        = LCD_DISPLAY_ACC_ALT_AZI ;

  /* Initialisation autres champs */

  memset( p_lcd->c_l0cur, 0 , sizeof( p_lcd->c_l0cur )) ;
  memset( p_lcd->c_l1cur, 0 , sizeof( p_lcd->c_l1cur )) ;
  memset( p_lcd->c_l0def, 0 , sizeof( p_lcd->c_l0def )) ;
  memset( p_lcd->c_l1def, 0 , sizeof( p_lcd->c_l1def )) ;

  strcpy( p_lcd->c_l0cur, "") ; 
  strcpy( p_lcd->c_l1cur, "") ; 
  strcpy( p_lcd->c_l0def, "") ; 
  strcpy( p_lcd->c_l1def, "") ; 

  p_lcd->i_board = 0 ;
  p_lcd->i_i2c_num = LCD_I2C_DEFAULT_DEV_PORT ; 
  
  /*--------------------------*/
  /* Initialisation ecran LCD */ 
  /*--------------------------*/

  if ( gp_Dev->DEVICE_USE_LCD ) {

    if ((p_lcd->i_fd = LCD1602Init(p_lcd->i_i2c_num)) == -1) {
      SyslogErr("Fail to init LCD1602Init\n");
      Trace("Fail to init LCD1602Init") ;
      return ;
    }
    else {
      Trace("LCD1602Init(ok)") ;
    }
  }
  /*--------------------------*/
  /* Affichage phrases init   */ 
  /*--------------------------*/

  sprintf(p_lcd->c_l0cur, "%d%s%d %-2d:%-2d", \
    gp_Tim->yy , \
    c_Lcd_Display_Months[ gp_Tim->mm -1  ] , \
    gp_Tim->dd, \
    gp_Tim->HH, \
    gp_Tim->MM ) ;
  
  sprintf(p_lcd->c_l1cur, "%.2f %.2f", \
    gp_Lie->lat * DEGRES, \
    gp_Lie->lon * DEGRES ) ;

  /* ajout 26 mai 2022 */
  strcpy( p_lcd->c_l0def, p_lcd->c_l0cur ) ;
  strcpy( p_lcd->c_l1def, p_lcd->c_l1cur ) ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    if ( LCD1602Clear(p_lcd->i_fd) == -1) {
      SyslogErr("Fail to LCD1602Clear\n");
      Trace("Fail to LCD1602Clear") ;
      return ;
    }
    else {
      Trace1("LCD1602Clear : (OK)") ;
    }
  
    usleep(LCD_USLEEP_AFTER_CLEARING) ;
  
    if (LCD1602DispLines(p_lcd->i_fd, p_lcd->c_l0cur, p_lcd->c_l1cur ) == -1) {
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

  Trace("") ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    memset( gp_Lcd->c_l0cur, 0, sizeof( gp_Lcd->c_l0cur )) ;
    memset( gp_Lcd->c_l1cur, 0, sizeof( gp_Lcd->c_l1cur )) ;

    strcpy( gp_Lcd->c_l0cur, c_l0 ) ;
    strcpy( gp_Lcd->c_l1cur, c_l1 ) ;

    gp_Lcd->i_duree_us = i_duree_us ;
    gp_Lcd->i_change_current = TRUE ; 

    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
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

  Trace("") ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    memset( gp_Lcd->c_l0def, 0, sizeof( gp_Lcd->c_l0def )) ;
    memset( gp_Lcd->c_l1def, 0, sizeof( gp_Lcd->c_l1def )) ;

    /* OLD -> NEW ; au final OLD OLD */
    strcpy( gp_Lcd->c_l0def, c_l0 ) ;
    strcpy( gp_Lcd->c_l1def, c_l1 ) ;

    gp_Lcd->i_duree_us = i_duree_us ;
    gp_Lcd->i_change_default = TRUE ;

    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
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

  Trace("") ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    /*---------------------------------------------------------------------------------*/
    /* les valeurs par defaut de la ffichage sont la date l heure et l objet en cours */
    /*---------------------------------------------------------------------------------*/

    CALCUL_TEMPS_SIDERAL( gp_Lie, gp_Tim ) ;

    sprintf( c_l0, "%d%s%d %-2d:%-2d", \
      gp_Tim->yy , \
      c_Lcd_Display_Months[ gp_Tim->mm -1  ] , \
      gp_Tim->dd, \
      gp_Tim->HH, \
      gp_Tim->MM ) ;

    sprintf( c_l1, "%s", gp_Ast->nom ) ;

    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
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

  char c_l0[16] ={0};
  char c_l1[16] ={0};

  Trace("") ;
  
  memset( c_l0, 0, sizeof( c_l0) ) ;
  memset( c_l1, 0, sizeof( c_l1) ) ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;
    
    gp_Lcd->define_default( c_l0, c_l1 ) ;

    /* Si les valeurs definis par default ont change */ 

    if ( ( strcmp ( gp_Lcd->c_l0def, c_l0 ) != 0 ) ||
         ( strcmp ( gp_Lcd->c_l1def, c_l1 ) != 0 ) ) {

      gp_Lcd->change_default( 2000000, c_l0 , c_l1 ) ;
      gp_Lcd->display_default() ;
    }

    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
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

  Trace("%d",gp_Dev->DEVICE_USE_LCD) ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    if ( LCD1602Clear(gp_Lcd->i_fd) == -1) {
      SyslogErr("LCD1602Clear : FAILED\n");
      Trace("LCD1602Clear : FAILED");

      pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;

      return ;
    }
    else {
      Trace1("LCD1602Clear : OK")
    }
    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
    /* entre 2500 et 5000 semble une bonne valeur de usleep */
    /* si on ne fait pas de usleep , l ecran ne se clear pas completement (teste) */
    usleep( LCD_USLEEP_AFTER_CLEARING ) ;
    
    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    if ( LCD1602DispLines(\
        gp_Lcd->i_fd, \
        gp_Lcd->c_l0def, \
        gp_Lcd->c_l1def ) \
        == -1) { 

      SyslogErr("LCD1602DispLines : FAILED\n");
      Trace("LCD1602DispLines : FAILED");
      
      pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;

      return ;
    }
    else {
      Trace1("LCD1602DispLines : OK");
    }
    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
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

  Trace("%d",gp_Dev->DEVICE_USE_LCD) ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    if ( LCD1602Clear(gp_Lcd->i_fd) == -1) {
      SyslogErr("LCD1602Clear : FAILED\n");
      Trace("LCD1602Clear : FAILED");

      pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;

      return ;
    }
    else {
      Trace1("LCD1602Clear : OK")
    }
    /* TODO  : mettre le usleep a interieur de mutex */
    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
    /* entre 2500 et 5000 semble une bonne valeur de usleep */
    /* si on ne fait pas de usleep , l ecran ne se clear pas completement (teste) */
    usleep( LCD_USLEEP_AFTER_CLEARING ) ;
    
    pthread_mutex_lock( & gp_Lcd->mutex_lcd ) ;

    if ( LCD1602DispLines(\
        gp_Lcd->i_fd, \
        gp_Lcd->c_l0cur, \
        gp_Lcd->c_l1cur ) \
        == -1) { 

      SyslogErr("LCD1602DispLines : FAILED\n");
      Trace("LCD1602DispLines : FAILED");
      
      pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;

      return ;
    }
    else {
      Trace1("LCD1602DispLines : OK");
    }
    pthread_mutex_unlock( & gp_Lcd->mutex_lcd ) ;
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

  gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
  gp_Lcd->display_current() ;

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

void   LCD_DISPLAY_STRING_INT      ( const int i_duree_us, const char* c_l0, const int i) {
  char c_l1[16] = {0} ;
  memset(c_l1,0,sizeof(c_l1));
  sprintf( c_l1, "%d", i) ;

  gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
  gp_Lcd->display_current() ;
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
  char c_l0[16] = {0} ;
  char c_l1[16] = {0} ;

  if ( gp_Dev->DEVICE_USE_LCD ) {
    sprintf( c_l0, "%d", i_l0) ;
    sprintf( c_l1, "%d", i_l1) ;
    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
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

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0) ) ; 
    memset( c_l1, 0, sizeof(c_l1) ) ; 

    CALCUL_TEMPS_SIDERAL(gp_Lie, gp_Tim) ;

    /* Remplissage de line 0 et line 1 */

    sprintf( c_l0, "%d%s%d %d:%d", \
      gp_Tim->yy ,\
      c_Lcd_Display_Months[ gp_Tim->mm -1  ] , \
      gp_Tim->dd, \
      gp_Tim->HH, \
      gp_Tim->MM ) ;
    
    sprintf( c_l1, "%.2f %.2f", \
      gp_Lie->lat * DEGRES, \
      gp_Lie->lon * DEGRES ) ;

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
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

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    strcpy(  c_l0, gp_Ast->nom ) ;
    sprintf( c_l1,"%.2f %.2f", gp_Ast->Va, gp_Ast->Vh);

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_AZI_ALT
* @author : s.gravois
* @brief  : Cette fonction affiche les coordonnees azimutales en cours
* @param  : LCD * lcd
* @param  : int i_duree_us
* @param  : ASTRE * gp_Ast
* @date   : 2022-04-09 creation 
*****************************************************************************************/

void LCD_DISPLAY_AZI_ALT( const int i_duree_us ) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "AZI %s", gp_Ast->c_ddmm_azi ) ;
    sprintf( c_l1, "ALT %s", gp_Ast->c_ddmm_alt );

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_AGH_DEC
* @author : s.gravois
* @brief  : Cette fonction affiche les coordonnees horaires en cours
* @param  : LCD * lcd
* @param  : int i_duree_us
* @param  : ASTRE * gp_Ast
* @date   : 2022-04-28 creation 
*****************************************************************************************/

void LCD_DISPLAY_AGH_DEC( const int i_duree_us) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]  ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]  ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "AGH %s", gp_Ast->c_hhmm_agh ) ;
    sprintf( c_l1, "DEC %s", gp_Ast->c_ddmm_dec ) ;

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_ASC_DEC
* @author : s.gravois
* @brief  : Cette fonction affiche les coordonnees horaires en cours
* @param  : LCD * lcd
* @param  : int i_duree_us
* @param  : ASTRE * gp_Ast
* @date   : 2022-04-28 creation 
*****************************************************************************************/

void LCD_DISPLAY_ASC_DEC( const int i_duree_us ) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "ASC %s", gp_Ast->c_hhmm_asc ) ;
    sprintf( c_l1, "DEC %s", gp_Ast->c_ddmm_dec ) ;

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
  }
  return ;
}

/*****************************************************************************************
* @fn     : LCD_DISPLAY_MODE_STELLARIUM
* @author : s.gravois
* @brief  : Cette fonction s inspire de CONFIG_AFFICHER_MODE_STELLARIUM pour le LCD
* @param  : 
* @date   : 2022-03-18 creation
* @date   : 2022-03-28 simplification
*****************************************************************************************/

void LCD_DISPLAY_MODE_STELLARIUM( const int i_duree_us ) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(AZI) %s", gp_Ast->c_ddmm_azi ) ;
    sprintf( c_l1, "(ALT) %s", gp_Ast->c_ddmm_alt );

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
    /*
    Trace("Va / Vh    : %3.2f / %3.2f" , gp_Ast->Va           , gp_Ast->Vh ) ;
    Trace("AD / Dec   : %s / %s"       , gp_Ast->c_hhmmss_asc , gp_Ast->c_ddmm_dec ) ;
    Trace("AH / Dec   : %s / %s"       , gp_Ast->c_hhmmss_agh , gp_Ast->c_ddmm_dec ) ;
    Trace("AZ./ Haut. : %s / %s"       , gp_Ast->c_ddmm_azi   , gp_Ast->c_ddmm_alt ) ;
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

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(INFO) ..." ) ;
    sprintf( c_l1, "(INFO) ..." );

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
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

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(h) %s", GPIO_ALT ) ;
    sprintf( c_l1, "(a) %s", GPIO_AZI );

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
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
*****************************************************************************************/

void   LCD_DISPLAY_CFG_GPIOS_MAS_FRE ( const int i_duree_us ) {

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]={0}  ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(Masq) %s", GPIO_MAS ) ;
    sprintf( c_l1, "(Fpwm) %s", GPIO_FREQUENCE_PWM );

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;

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

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    Trace("CONFIG_FIC_LED = %s", CONFIG_FIC_LED) ;

    /* Lecture LED IR */
    /* Le service systemd astrokit-boot / script_astrokit_boot.sh cree 
      normalement ce fichier */

    if ( ( f_led_ir = fopen( CONFIG_FIC_LED, "r" ) ) != NULL ) {
      pc_gets = fgets( c_gets, CONFIG_TAILLE_BUFFER_16, f_led_ir ) ;
      if ( pc_gets != NULL ) {
        sprintf( c_l1, "(LED IR) %s", c_gets );
      }
      else {
        sprintf( c_l1, "(LED IR) cf logs" );
        Trace("fichier %s non lu", CONFIG_FIC_LED) ;
      }
    }
    else {
      sprintf( c_l1, "(LED IR) cf logs" );
      Trace("erreur ouverture %s", CONFIG_FIC_LED) ;
    }
    /* Lecture LED ETAT */

    sprintf( c_l0, "(LED ETAT) %d", GPIO_LED_ETAT ) ;
    

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;

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

  int h1=ALT_R1, h2= ALT_R2, h3= ALT_R3, h4= ALT_R4, h5= ALT_R5, h6= ALT_R6 ;
  int a1=AZI_R1, a2= AZI_R2, a3= AZI_R3, a4= AZI_R4, a5= AZI_R5, a6= AZI_R6 ;

  char c_l0[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ] ={0} ;
  char c_l1[ LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure ]  ={0};

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "%d %d %d %d %d %d", h1,h2,h3,h4,h5,h6 ) ;
    sprintf( c_l1, "%d %d %d %d %d %d", a1,a2,a3,a4,a5,a6 );

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
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

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(F alt) %.2f", gp_Sui->Fh_mic ) ;
    sprintf( c_l1, "(F azi) %.2f", gp_Sui->Fa_mic ) ;

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
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

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(T alt) %.2f", gp_Sui->Th_mic ) ;
    sprintf( c_l1, "(T azi) %.2f", gp_Sui->Ta_mic ) ;

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
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

  if ( gp_Dev->DEVICE_USE_LCD ) {

    memset( c_l0, 0, sizeof(c_l0)) ; 
    memset( c_l1, 0, sizeof(c_l1)) ;

    sprintf( c_l0, "(Acc Alt) %.4f", gp_Sui->acc_alt ) ;
    sprintf( c_l1, "(Acc Azi) %.4f", gp_Sui->acc_azi ) ;

    gp_Lcd->change_current( i_duree_us, c_l0, c_l1) ;
    gp_Lcd->display_current() ;
  }
  return ;
}

/* ----------------------------------------------------------------------

FIN FICHIER - FIN FICHIER  FIN FICHIER  FIN FICHIER  FIN FICHIER 

------------------------------------------------------------------------*/