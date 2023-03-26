/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
--------------------------------------------------------------
date        | commentaires 
--------------------------------------------------------------
07/10/2022  | * creation a partir du code de astro.c et autre
-------------------------------------------------------------- 
*/

#include "astro_voute.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/*****************************************************************************************
* @fn     : VOUTE_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_VOUTE *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void VOUTE_DISPLAY_PREPARE ( STRUCT_VOUTE * lp_Vou) {

  TraceArbo(__func__,2,"astre format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Vou->vou_mutex ) ;
/*
   unsigned long   vou_tempo ; 
   long long       vou_num ; 
   double          vou_tempo_percent ;
   double          vou_calibration_delta_t ;
   double          vou_begin ;
   double          vou_end ;
   double          vou_pas ;
   double          vou_dt ;
   double          vou_acc ;
   double          vou_temps_ecoule ;   
   int             vou_run ;
*/
  sprintf( lp_Vou->vou_dis_cmd , STR_VOU_FORMAT_0,\
    lp_Vou->vou_num, \
    lp_Vou->vou_run, \
    lp_Vou->vou_temps_ecoule );

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Vou->vou_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static VOUTE_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_VOUTE *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void VOUTE_DISPLAY(STRUCT_VOUTE *lp_Vou) {

  char c_cmd[CONFIG_TAILLE_BUFFER_256]={0} ;

  TraceArbo(__func__,2,"display informations on Voure") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  VOUTE_DISPLAY_PREPARE( lp_Vou ) ;

  ASTRO_GLOBAL_LOG_ON ( &  lp_Vou->vou_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Vou->vou_loglevel , 1 , "%s", lp_Vou->vou_dis_cmd ) ;
  ASTRO_GLOBAL_LOG_OFF( &  lp_Vou->vou_loglevel ) ;

  return ;
}

/*****************************************************************************************
* @fn     : VOUTE_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_VOUTE *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void VOUTE_LOCK ( STRUCT_VOUTE * lp_Vou) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Vou->vou_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : VOUTE_LOG
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_VOUTE *
* @date   : 2023-01-26 creation
*****************************************************************************************/

static void VOUTE_LOG ( STRUCT_VOUTE * lp_Vou) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /* TODO : completer */

  return ;
}
/*****************************************************************************************
* @fn     : VOUTE_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_VOUTE *
* @date   : 2022-12-20 creation
*****************************************************************************************/

static void VOUTE_UNLOCK ( STRUCT_VOUTE * lp_Vou) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Vou->vou_mutex ) ;

  return ;
}

/*****************************************************************************************
* @fn     : VOUTE_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_VOUTE *lp_Vou
* @param  : STRUCT_VOUTE *lp_Vou
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           vou_tempo_percent et vou_calibration_delta_t
*****************************************************************************************/

void VOUTE_INIT(STRUCT_VOUTE *lp_Vou) {
  
  char buf[CONFIG_TAILLE_BUFFER_256] ;

  TraceArbo(__func__,0,"init voute") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Vou->vou_mutex ) ;
                                     lp_Vou->vou_log      = VOUTE_LOG ;
                                     lp_Vou->vou_lock     = VOUTE_LOCK ;
                                     lp_Vou->vou_unlock   = VOUTE_UNLOCK ;
                                     lp_Vou->vou_display  = VOUTE_DISPLAY ;
                                     lp_Vou->vou_loglevel = 0 ; 
                                     lp_Vou->vou_file     = NULL ;
  gettimeofday (                   & lp_Vou->vou_tval, NULL ) ;  

  lp_Vou->vou_temps_ecoule        = 0 ;
  lp_Vou->vou_tempo_percent       = 0.96 ;   
  lp_Vou->vou_calibration_delta_t = 0.97 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
  lp_Vou->vou_num                 = 0 ;
  lp_Vou->vou_begin               = 0 ;
  lp_Vou->vou_end                 = 0 ;
  lp_Vou->vou_acc                 = 1 ;
  lp_Vou->vou_run                 = 1 ;

  lp_Vou->vou_dt     = 0  ;
  lp_Vou->vou_pas    = lp_Vou->vou_dt * CALCULS_VIT_ROT_EARTH_RAD_PER_SEC ;
  lp_Vou->vou_tempo  = (unsigned long)( lp_Vou->vou_dt * TIME_MICRO_SEC / lp_Vou->vou_acc ) ;   

  if ( ASTRO_LOG_DEBUG_VOUTE ) {
  
    memset( buf, CONFIG_ZERO_CHAR, sizeof(buf));
    sprintf(buf,"%s/%s/%s", gp_Con_Par->con_par_rep_home, gp_Con_Par->con_par_rep_log, gp_Con_Par->con_par_fic_vou) ;
    
    if ( (gp_File_Flog=fopen(buf,"a")) == NULL) {
      // completer et modifier
      Trace("probleme ouverture 3 %s",buf) ;
      SyslogErrFmt("probleme ouverture %s", buf) ;
      exit(2) ;
    }
    else {
      Trace("open %s ok", buf) ;
    }
  }

  return ;
}

/*****************************************************************************************
* @fn     : VOUTE_CONFIG
* @author : s.gravois
* @brief  : Cette fonction configure la structure STRUCT_VOUTE *lp_Vou
* @param  : STRUCT_VOUTE *lp_Vou
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : passer par une lecture de parametre dans config.txt pour \
*           vou_tempo_percent et vou_calibration_delta_t
*****************************************************************************************/

void VOUTE_CONFIG( STRUCT_VOUTE *lp_Vou, double dt, double acc, double percent ) {
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  lp_Vou->vou_temps_ecoule        = 0 ;
  lp_Vou->vou_tempo_percent       = percent ; 
  lp_Vou->vou_calibration_delta_t = 0.99 ; // permet de calibrer la boucle de calcul voute pour qu'elle fasse pile une seconde
  lp_Vou->vou_begin               = 0 ;
  lp_Vou->vou_end                 = CALCULS_PI_FOIS_DEUX ;
  lp_Vou->vou_num                 = 0 ;
  lp_Vou->vou_acc                 = acc ;

  lp_Vou->vou_dt     = dt  ;
  lp_Vou->vou_pas    = lp_Vou->vou_dt * CALCULS_VIT_ROT_EARTH_RAD_PER_SEC ;
  lp_Vou->vou_tempo  = (unsigned long)( lp_Vou->vou_dt * TIME_MICRO_SEC / lp_Vou->vou_acc ) ;
}

/*****************************************************************************************
* @fn     : VOUTE_TEMPORISATION
* @author : s.gravois
* @brief  : permet de faire une temporisation (1s) dans la boucle infinie de _SUIVI_VOUTE
* @brief  : utilise un usleep() sur 99% du temps de la temporisation (cf vou_calibration_delta_t)
* @param  : struct timeval *t00
* @date   : 2022-03-20 creation entete
* @todo   : verifier si on peut augmenter la valeur de vou_calibration_delta_t en augmentant
*           la priorite du thread egalement
*****************************************************************************************/

long VOUTE_TEMPORISATION(STRUCT_VOUTE *lp_Vou, struct timeval t00) {
  
  struct timeval t01 ;
  double  t_diff ;
  
  TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  usleep ( (long)(lp_Vou->vou_tempo * lp_Vou->vou_tempo_percent) ) ;
  
  t_diff=0;
  
  // FIXME : rappel : lp_Vou->vou_calibration_delta_t = 0.99 normalement
  // FIXME : pour eviter de faire appel a la boucle while 99% du temps
  
  while( t_diff < ( lp_Vou->vou_tempo * lp_Vou->vou_calibration_delta_t ) ) {
     
   gettimeofday(&t01,NULL) ;
   t_diff = (double)(( t01.tv_sec - t00.tv_sec)  * TIME_MICRO_SEC ) + t01.tv_usec - t00.tv_usec;
  }
  return t_diff ;
}


/*****************************************************************************************
* @fn     : _SUIVI_VOUTE
* @author : s.gravois
* @brief  : Ce mode permet de gerer la voute c'est a dire le rafraichissement des calculs
* @param  : STRUCT_SUIVI   *gp_Sui
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
* @date   : 2022-05-26 ajout temporisation par usleep  plutot que sleep avant start
* @todo   : 
*****************************************************************************************/

/*
   _SUIVI_VOUTE :
   le but de la fonction est de rafraichir a intervalles reguliers (1 seconde)
   tous les calculs relatifs a la vitesse de l'as suivi
*/

void * _SUIVI_VOUTE(STRUCT_VOUTE * lp_Vou) {
  
  long long ll_inrc=0 ;
  char c_l0[LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure] ={0};
  char c_l1[LCD_LINES_CHAR_NUMBERS + LCD_LINES_CHAR_NUMBERS_secure] ={0};

  unsigned long  ul_vou_while_incr ;
  struct timeval t00 ;
  struct sched_param param;

  TraceArbo(__func__,1,"pthread_create_callback_fct") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_VOUTE ) ;
  /* 2023 : deport du sleep dans la fonction PTHREADS_CONFIG */ 
  /*
  usleep( PTHREAD_USLEEP_BEFORE_START_SUIVI_VOUTE ) ;
  */
  memset( c_l0, CONFIG_ZERO_CHAR, sizeof( c_l0 )) ;
  memset( c_l1, CONFIG_ZERO_CHAR, sizeof( c_l1 )) ;
  
  ul_vou_while_incr =0 ;
  
  gettimeofday(&t00,NULL) ;
  
  VOUTE_CONFIG( lp_Vou, 1, 1, 0.985 ) ;
  
  // FIXME : 
  // en mode equatorial, pas besoin de _SUIVI_VOUTE, en effet la vitesse ne change pas
  // Va=15"/s et Vh=0 (le moteur en ascension droite est commande par azimut)
  
  //-------------------------------------------------------------------------------
  // FIXME : debut boucle infinie du thread _SUIVI_VOUTE
  //-------------------------------------------------------------------------------
  
  gp_Ast->ast_new = TRUE ;

  /* Debut boucle _SUIVI_VOUTE */
  while(TRUE) {
    
        /* Creee un point d 'annulation pour la fonction pthread_cancel */
    pthread_testcancel() ;

    Trace1("while") ;

    if ( lp_Vou->vou_run ) {
      
      Trace1("lp_Vou->vou_run == true") ;

      /* FIXME : modification 20121225 : tous les calculs generiques dans CALCULS_TOUT */
      
      CALCULS_TOUT() ;
      
      /* Exceptionnellement , utilisation variables globales */ 
      /* LCD_DISPLAY_TEMPS_LIEU(0,gp_Lie,gp_Tim) ;*/

      if ( gp_Ast->ast_new ) { 

        ASTRE_FORMATE_DONNEES_AFFICHAGE(gp_Ast) ;
        CONFIG_DISPLAY_MODE_LONG(gp_Ast,gp_Lie,gp_Cal) ; 
        ASTRE_STELLARIUM_VIEW(gp_Ast) ;
        
        gp_Lcd->display_ast_vit(2000000) ;
        gp_Lcd->default_refresh() ;
        
        gp_Ast->ast_new = FALSE ;
      }
/*
      if ( gp_Sui->sui_menu_old != gp_Sui->sui_menu  ) {
        CONFIG_DISPLAY_TOUT() ;
      }
*/
      gp_Ast->ast_agh += lp_Vou->vou_pas ;
      lp_Vou->vou_begin += lp_Vou->vou_pas ;

      Trace1("voute : temporisation") ;

      lp_Vou->vou_temps_ecoule += VOUTE_TEMPORISATION( lp_Vou, t00 ) ; 
      gettimeofday(&t00,NULL) ;

			lp_Vou->vou_num ++ ;
      ul_vou_while_incr++ ;
      // attention cet appel systeme genere une interuption
      // uniquement utiliser pour les tests
      // system("/bin/date >> /root/astrokit.begin.log") ;
    }
    
    // TRES IMPORTANT !!
    // permet d eviter au thread de consommer toute la CPU
    // si jamais la condition _SUIVI_VOUTE = 1 n est pas realisee
    
    else {
      // attention cet appel systeme genere une interuption
      // uniquement utiliser pour les tests
      // system("/bin/date >> /root/astrokit.begin.log") ;
      //Trace1("La voute ne tourne pas") ;

      Trace("usleep %ld", lp_Vou->vou_tempo ) ;

      usleep( lp_Vou->vou_tempo );
    }
  }
  Trace1("Stop") ;

  return NULL ; 
}
/* =====================================================================================
 *
 * FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - 
 * 
 * ===================================================================================== */