/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# Fichier     | gpio.c
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | ajout entete
# 01/05/2021  | ajout commentaire sur fonction GPIO_READ2
#               synthese traces sur une ligne (2021)
# 18/01/2022  | * les lignes close( fd ) posent un probleme
# (issue)  lors de utilisation de la librairie ncurses (getch)
# ainsi que termios (cf fichier keyboard.c / .h ) :
# creation entete de la fonction au format doxygen du reliquat de close dans le code
# 23/01/2022  | * ajout condition pour entree dans GPIO_CLAVIER_MATRICIEL_READ
#   =>  if ( gp_Dev->dev_use_raquette ) {  
# 20 mai 2022 | * reprise de la fonction main Gpio pour effectuer
#                 des fonctions de deplacements utiles      
# 24 mai 2022 | * reprise des fonctions PWM
#               * mise en place de marqueurs de temps et de pas
# -------------------------------------------------------------- 
*/

#include "astro_gpio.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
/* MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ; */
MACRO_ASTRO_GLOBAL_EXTERN_CONFIG ;
MACRO_ASTRO_GLOBAL_EXTERN_PID ;
MACRO_ASTRO_GLOBAL_EXTERN_PTHREADS ;

int GPIO_OPEN_STATUT ;

/* Variables concernant les broches */ 

int             gi_gpio_fd     [ GPIO_SIZE ] ;
int             gi_gpio_in   	[ GPIO_SIZE]  ;
int             gi_gpio_out    [ GPIO_SIZE] ; 

/* Numeros de broches specifiques pour les sorties PWM */ 

int             gi_alt_gpios    [ GPIO_NB_PHASES_PAR_MOTEUR ]  ;  // phases du moteur (utilise si CONTROLEUR vaut 0 dans config.txt)
int             gi_azi_gpios    [ GPIO_NB_PHASES_PAR_MOTEUR ] ;   // phases du moteur (utilise si CONTROLEUR vaut 0 dans config.txt)

/* masque de redefinition des gpios des phases : 
  evite soucis de branchement */

int             gi_alt_masque    [ GPIO_NB_PHASES_PAR_MOTEUR ] ;   
int             gi_azi_masque    [ GPIO_NB_PHASES_PAR_MOTEUR ] ;   // masque de redefinition des gpios des phases : evite soucis de branchement

double          gd_alt_fpwm ;
double          gd_azi_fpwm ;
int             gi_gpio_timeout ;
int             gi_gpio_max_nb_pas ;
int             gi_gpio_max_nb_upas ;

sem_t           gpio_sem  ;
struct timespec gpio_tm_now;
struct timespec gpio_tm_nxt;

/*****************************************************************************************
* @fn     :  GPIO_PWM_PHASE_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_GPIO_PWM_PHASE *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void  GPIO_PWM_PHASE_LOCK ( STRUCT_GPIO_PWM_PHASE * lp_Pha) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pha->pha_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     :  GPIO_PWM_PHASE_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_GPIO_PWM_PHASE *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void  GPIO_PWM_PHASE_UNLOCK ( STRUCT_GPIO_PWM_PHASE * lp_Pha) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pha->pha_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     :  GPIO_PWM_MOT_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_GPIO_PWM_MOTEUR *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void  GPIO_PWM_MOT_LOCK ( STRUCT_GPIO_PWM_MOTEUR * lp_Mot) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Mot->mot_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     :  GPIO_PWM_MOT_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_GPIO_PWM_MOTEUR *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void  GPIO_PWM_MOT_UNLOCK ( STRUCT_GPIO_PWM_MOTEUR * lp_Mot) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Mot->mot_mutex ) ;

  return ;
}

/*****************************************************************************************
* @fn     : GPIO_PWM_PARAMS_INIT
* @author : s.gravois
* @brief  : fonction qui initialise les params pwm STRUCT_GPIO_PARAMS_PWM *
* @param  : STRUCT_GPIO_PARAMS_PWM *
* @date   : 2022-12-15 creation
* @todo   : 
*****************************************************************************************/

void   GPIO_PWM_PARAMS_INIT (STRUCT_GPIO_PARAMS_PWM * lp_Pwm_Par) {

  TraceArbo(__func__,0,"init gpio pwm params") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Pwm_Par->par_mutex) ;

  memset( lp_Pwm_Par->gpi_pwm_par_alt_gpio, CONFIG_ZERO_CHAR, sizeof( lp_Pwm_Par->gpi_pwm_par_alt_gpio ))  ;  
  memset( lp_Pwm_Par->gpi_pwm_par_azi_gpio, CONFIG_ZERO_CHAR, sizeof( lp_Pwm_Par->gpi_pwm_par_azi_gpio ))  ;  
  memset( lp_Pwm_Par->gpi_pwm_par_alt_mask, CONFIG_ZERO_CHAR, sizeof( lp_Pwm_Par->gpi_pwm_par_alt_mask ))  ;
  memset( lp_Pwm_Par->gpi_pwm_par_azi_mask, CONFIG_ZERO_CHAR, sizeof( lp_Pwm_Par->gpi_pwm_par_azi_mask ))  ;
  memset( lp_Pwm_Par->gpi_pwm_par_alt_fpwm, CONFIG_ZERO_CHAR, sizeof( lp_Pwm_Par->gpi_pwm_par_alt_fpwm ))  ;  
  memset( lp_Pwm_Par->gpi_pwm_par_azi_fpwm, CONFIG_ZERO_CHAR, sizeof( lp_Pwm_Par->gpi_pwm_par_azi_fpwm ))  ;  

  return ;

}
/*****************************************************************************************
* @fn     : GPIO_PWM_PARAMS_DISPLAY
* @author : s.gravois
* @brief  : fonction qui affiche les params pwm STRUCT_GPIO_PARAMS_PWM *
* @param  : STRUCT_GPIO_PARAMS_PWM *
* @date   : 2022-12-15 creation
* @todo   : 
*****************************************************************************************/

void   GPIO_PWM_PARAMS_DISPLAY (STRUCT_GPIO_PARAMS_PWM * lp_Pwm_Par) {

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %s", "lp_Pwm_Par->gpi_pwm_par_alt_gpio", lp_Pwm_Par->gpi_pwm_par_alt_gpio)  ;  
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %s", "lp_Pwm_Par->gpi_pwm_par_azi_gpio", lp_Pwm_Par->gpi_pwm_par_azi_gpio)  ;  
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %s", "lp_Pwm_Par->gpi_pwm_par_alt_mask", lp_Pwm_Par->gpi_pwm_par_alt_mask)  ;
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %s", "lp_Pwm_Par->gpi_pwm_par_azi_mask", lp_Pwm_Par->gpi_pwm_par_azi_mask)  ;
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %s", "lp_Pwm_Par->gpi_pwm_par_alt_fpwm", lp_Pwm_Par->gpi_pwm_par_alt_fpwm)  ;  
  MACRO_ASTRO_GLOBAL_LOG(gp_Log->log_level,1,"%-50s = %s", "lp_Pwm_Par->gpi_pwm_par_azi_fpwm", lp_Pwm_Par->gpi_pwm_par_azi_fpwm)  ;  

  return ;

}
/*****************************************************************************************
* @fn     : GPIO_TEST_CONTROLER
* @author : s.gravois
* @brief  : fonction de test simple des moteurs avec un controleur externe
* @param  : void
* @date   : 2022-11-30 creation entete de la fonction au format doxygen
* @todo   : verifier code pour test moteur sans controleur externe
*****************************************************************************************/

void  GPIO_TEST_CONTROLER(void ) {

  int i ;
  struct timeval t00,t01 ;
  long   t_diff , nb_puls;
  double t_diff_sec,  t_diff_moy ;
  double f_fin, f_deb, delai ;
  double FREQ[5][3] = { \
    {10,  100,   3}, \
    {100, 1000,  3}, \
    {1000,5000,  3}, \
    {5000,10000, 3}, \
    {10000,20000, 3}, \
  } ;  
  // signal(SIGTERM,TRAP_SUIVI_TEST) ; 

  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gi_gpio_in,gi_gpio_out)) ;
    
  GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
  GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
  
  gettimeofday(&t00,NULL) ;
  
  i=0;
  delai=0.0 ;
  t_diff_moy = 0.0 ;
  
  GPIO_SET_ALT( 0,0,1,1,1,0 ) ;
  GPIO_SET_AZI( 0,0,1,1,1,0 ) ;
    
  Trace("%-10s%-10s%-10s%-10s%-10s%-10s\n","freq_deb","freq_fin","delai","constate","diff","nbpuls") ;
  while (i<5) {
    
    f_deb = FREQ[i][0] ;
    f_fin = FREQ[i][1] ;
    delai = FREQ[i][2] ;
  
    nb_puls = GPIO_ACCELERATION_2( gp_Ctl_Par->par_alt_clk, gp_Ctl_Par->par_azi_clk, f_deb, f_fin, delai, 0) ;
    
    gettimeofday(&t01,NULL) ;

    t_diff = (( t01.tv_sec - t00.tv_sec ) * TIME_MICRO_SEC) + t01.tv_usec - t00.tv_usec ;
    t_diff_sec = (double)t_diff / (double)TIME_MICRO_SEC ;
    gettimeofday(&t00,NULL) ;
    Trace("%4.4f%2s%4.4f%2s%4.4f%2s%4.4f%2s%4.4f%2s%10ld\n",f_deb,"", f_fin,"", delai,"", t_diff_sec,"", t_diff_sec-delai,"",nb_puls ) ;
    t_diff_moy += t_diff_sec -  delai ;
    i++ ;
  }
  Trace("Moyenne des differences = %2.4f\n",  t_diff_moy/i ) ;
  
  GPIO_SET_ALT( 0,0,0,0,1,1 ) ;
  GPIO_SET_AZI( 0,0,0,0,1,1 ) ;
  
  Trace("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;
  
  GPIO_STATUT() ;
}

/*****************************************************************************************
* @fn     : GPIO_TRAP_SIG
* @author : s.gravois
* @brief  : fonction appelle quand un signal est trape dans main gpio
* @param  : int     sig
* @param  : char[]  gc_config_path_cmd_stty (chemin de stty)
* @date   : 2022-05-20 creation 
* @todo   : voir s il est preferable de trapper egalement depuis les threads (si c possible)
*****************************************************************************************/

void GPIO_TRAP_SIG(int sig) {

  char c_cmd[ 64 ] ;
  int i ;

  /*----------------------------------*/
  /* Rendre le terminal propre (sane) */
  /*----------------------------------*/

  memset( c_cmd, 0, sizeof(c_cmd)) ;
  sprintf(c_cmd,"%s sane", gc_config_path_cmd_stty ) ;

  if ( system( c_cmd ) < 0 ) {

    SyslogErr("Probleme avec stty sane\n") ;
    Trace("Probleme avec stty sane") ;
    exit(2) ;
  } 
  Trace("appel exit(0)") ;
  
  exit(0) ;
}
/*****************************************************************************************
* @fn     : GPIO_LED_ETAT_CLIGNOTE
* @author : s.gravois
* @brief  : Cette fonction permet de faire clignoter la LED d ETAT 
* @param  : int nombre_clignotement
* @param  : int duree_clignotement
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : effectuer la meme fonction mais avec variation continue PWM
* @todo   : (necessite utiliser les threads rapports cycliques)
* @todo   : ATTENTION : participe aux temporisations avec usleep !
* @todo   : devrait etre place dans un thread dedie
* @todo   : (remettre le usleep en fin de test)
*****************************************************************************************/

void GPIO_LED_ETAT_CLIGNOTE(int nombre_clignotement, int duree_clignotement) {
  
 int i ;
 i=-1; 
 
 TraceArbo(__func__,3,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

 while(++i<nombre_clignotement) {
  GPIO_SET( gp_Pwm_Par->gpi_pwm_par_led_etat, 1 ) ; 
  /* usleep(duree_clignotement*10000); */  
  
  GPIO_SET( gp_Pwm_Par->gpi_pwm_par_led_etat, 0 ) ; 
  /* usleep(duree_clignotement*10000); */
 }

 return;
}
/*****************************************************************************************
* @fn     : GPIO_TAB_TOKEN
* @author : s.gravois
* @brief  : Cette fonction permet de lire les entrees de type 0,1,2,3 (token)
*           a deplacer eventuellement dans un fichier util.c / util.h
* @param  : int    tab[4]
* @param  : char * buffer
* @param  : char * separator
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : voir si realisable sous forme de MACRO
*****************************************************************************************/

void GPIO_TAB_TOKEN(int tab[4],char * buffer, char * separator) {
  int j ;
  char *str1, *token, *sptr ;
  
  for (j = 0, str1 = buffer ; ; j++, str1 = NULL) {
    token = strtok_r(str1, separator, &sptr);
    if (token == NULL || j == 4) break ;
    tab[j]=atoi(token);
   }
}
/*****************************************************************************************
* @fn     : GPIO_CONFIG_FIC_READ
* @author : s.gravois
* @brief  : Cette fonction lit les parametres GPIO dans le fichier de configuration
*           (gp_Pwm_Par->gpi_pwm_par_alt_gpio / gp_Pwm_Par->gpi_pwm_par_azi_gpio / ALT_MASK )
*           Initilise la frequence PWM a 1000 si aucune entree gd_alt_fpwm
* @param  : lp_Con->con_params[CONFIG_DATAS_NB_LIGNES][CONFIG_DATAS_NB_LIGNES][CONFIG_TAILLE_BUFFER_256]
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : 
*****************************************************************************************/

void GPIO_CONFIG_FIC_READ(STRUCT_CONFIG * lp_Con) {
  int l,i, j ;
  char *str1, *token, *sptr ;
  
  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  for(l=0;l<CONFIG_DATAS_NB_LIGNES;l++) {

    /*-------------------------------------------------------*/
    /* Lecture du parametre des GPIOS utilises pour altitude */
    /*-------------------------------------------------------*/

    if(!strcmp("ALT_GPIO",lp_Con->con_params[l][0])) {

      // FIXME ajout stephane 2021
      memset( gp_Pwm_Par->gpi_pwm_par_alt_gpio, CONFIG_ZERO_CHAR, sizeof(gp_Pwm_Par->gpi_pwm_par_alt_gpio)) ;
      strcpy( gp_Pwm_Par->gpi_pwm_par_alt_gpio, lp_Con->con_params[l][1] ) ;

      Trace1("ALT_GPIO trouve ligne %d = (%s)", l,gp_Pwm_Par->gpi_pwm_par_alt_gpio) ;

      for(j=0;j<GPIO_NB_PHASES_PAR_MOTEUR;j++) gi_gpio_in[j]=-1 ;

      for (j = 0, str1 = lp_Con->con_params[l][1]; ; j++, str1 = NULL) {
        token = strtok_r(str1, ",", &sptr);
        if (token == NULL) break ;
        gi_alt_gpios[j]=atoi(token);
      }
    }

    /*-------------------------------------------------------*/
    /* Lecture du parametre des GPIOS utilises pour azimut */
    /*-------------------------------------------------------*/

    if(!strcmp("AZI_GPIO",lp_Con->con_params[l][0])) {

      // FIXME ajout stephane 2021
      memset( gp_Pwm_Par->gpi_pwm_par_azi_gpio, CONFIG_ZERO_CHAR, sizeof(gp_Pwm_Par->gpi_pwm_par_azi_gpio)) ;
      strcpy( gp_Pwm_Par->gpi_pwm_par_azi_gpio, lp_Con->con_params[l][1] ) ;

      Trace1("AZI_GPIO trouve ligne %d = (%s)", l,gp_Pwm_Par->gpi_pwm_par_azi_gpio) ;

      for(i=0; i < GPIO_NB_PHASES_PAR_MOTEUR ; i++) gi_gpio_out[i]=-1 ;

      for (j = 0, str1 = lp_Con->con_params[l][1]; ; j++, str1 = NULL) {
        token = strtok_r(str1, ",", &sptr);
        if (token == NULL) break ;
        gi_azi_gpios[j]=atoi(token);
      }
    }

    /*-------------------------------------------------------*/
    /* Lecture du parametre MASQUE en altitude   */
    /*-------------------------------------------------------*/

    if(!strcmp("ALT_MASK",lp_Con->con_params[l][0])) {

      // FIXME ajout stephane 2021
      memset( gp_Pwm_Par->gpi_pwm_par_alt_mask, CONFIG_ZERO_CHAR, sizeof(gp_Pwm_Par->gpi_pwm_par_alt_mask)) ;
      strcpy( gp_Pwm_Par->gpi_pwm_par_alt_mask, lp_Con->con_params[l][1] ) ;

      Trace1("ALT_MASK trouve ligne %d = (%s)", l,gp_Pwm_Par->gpi_pwm_par_alt_mask) ;

      for(i=0; i < GPIO_NB_PHASES_PAR_MOTEUR ; i++) gi_alt_masque[i]=-1 ;

      for (j = 0, str1 = lp_Con->con_params[l][1]; ; j++, str1 = NULL) {
        token = strtok_r(str1, ",", &sptr);
        if (token == NULL) break ;
        gi_alt_masque[j]=atoi(token);
      }
    }

    /*-------------------------------------------------------*/
    /* Lecture du parametre MASQUE en azimut   */
    /*-------------------------------------------------------*/

    if(!strcmp("AZI_MASK",lp_Con->con_params[l][0])) {

      // FIXME ajout stephane 2021
      memset( gp_Pwm_Par->gpi_pwm_par_azi_mask, CONFIG_ZERO_CHAR, sizeof(gp_Pwm_Par->gpi_pwm_par_azi_mask)) ;
      strcpy( gp_Pwm_Par->gpi_pwm_par_azi_mask, lp_Con->con_params[l][1] ) ;

      Trace1("AZI_MASK trouve ligne %d = (%s)", l,gp_Pwm_Par->gpi_pwm_par_azi_mask) ;

      for(i=0; i < GPIO_NB_PHASES_PAR_MOTEUR ; i++) gi_azi_masque[i]=-1 ;

      for (j = 0, str1 = lp_Con->con_params[l][1]; ; j++, str1 = NULL) {
        token = strtok_r(str1, ",", &sptr);
        if (token == NULL) break ;
        gi_azi_masque[j]=atoi(token);
      }
    }

    /*-------------------------------------------------------*/
    /* Lecture du parametre FREQUENCE PWM en altitude   */
    /*-------------------------------------------------------*/

    if(!strcmp("ALT_FPWM",lp_Con->con_params[l][0])) {

      // FIXME ajout stephane 2021
      memset( gp_Pwm_Par->gpi_pwm_par_alt_fpwm, CONFIG_ZERO_CHAR, sizeof(gp_Pwm_Par->gpi_pwm_par_alt_fpwm)) ;
      strcpy( gp_Pwm_Par->gpi_pwm_par_alt_fpwm, lp_Con->con_params[l][1] ) ;

      gd_alt_fpwm = 1000 ;

      Trace1("gp_Pwm_Par->gpi_pwm_par_alt_fpwm trouve ligne %d = (%s)", l,gp_Pwm_Par->gpi_pwm_par_alt_fpwm) ;

      for (j = 0, str1 = lp_Con->con_params[l][1]; ; j++, str1 = NULL) {
        token = strtok_r(str1, ",", &sptr);
        if (token == NULL) break ;
        gd_alt_fpwm=(double)atoi(token);
      }
    }

    /*-------------------------------------------------------*/
    /* Lecture du parametre FREQUENCE PWM en azimut   */
    /*-------------------------------------------------------*/

    if(!strcmp("AZI_FPWM",lp_Con->con_params[l][0])) {

      // FIXME ajout stephane 2021
      memset( gp_Pwm_Par->gpi_pwm_par_azi_fpwm, CONFIG_ZERO_CHAR, sizeof(gp_Pwm_Par->gpi_pwm_par_azi_fpwm)) ;
      strcpy( gp_Pwm_Par->gpi_pwm_par_azi_fpwm, lp_Con->con_params[l][1] ) ;

      gd_azi_fpwm = 1000 ;

      Trace1("gp_Pwm_Par->gpi_pwm_par_azi_fpwm trouve ligne %d = (%s)", l,gp_Pwm_Par->gpi_pwm_par_azi_fpwm) ;

      for (j = 0, str1 = lp_Con->con_params[l][1]; ; j++, str1 = NULL) {
        token = strtok_r(str1, ",", &sptr);
        if (token == NULL) break ;
        gd_azi_fpwm=(double)atoi(token);
      }
    }
  }
  printf("gi_azi_gpios[]=>");
  for(i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++) { 
    printf("%d ",gi_azi_gpios[i]);
  }
  printf("\n") ;

  for(i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++) Trace("gi_alt_gpios[%d]=%d",i,gi_alt_gpios[i]);
  
  for(i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++) Trace("gi_alt_masque[%d]=%d",i,gi_alt_masque[i]);
  for(i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++) Trace("gi_azi_masque[%d]=%d",i,gi_azi_masque[i]);

  Trace("gd_alt_fpwm = %f", gd_alt_fpwm ) ;
  Trace("gd_azi_fpwm = %f", gd_azi_fpwm ) ;
}
/*****************************************************************************************
* @fn     : GPIO_READ2
* @author : s.gravois
* @brief  : Cette fonction lit GPIO_INPUT et GPIO_OUTPUT
* @param  : lp_Con->con_params[CONFIG_DATAS_NB_LIGNES][CONFIG_DATAS_NB_COLONNES][CONFIG_TAILLE_BUFFER_256]
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @todo   : (obsolete) fonction ancienne, remplace explicitement par GPIO_LED_xx etc..
*****************************************************************************************/

void GPIO_READ2(STRUCT_CONFIG *lp_Con) {

  int l,i, j ;
  char *str1, *token, *sptr ;
  
  for(l=0;l<CONFIG_DATAS_NB_LIGNES;l++) {
   if(!strcmp("GPIO_INPUT",lp_Con->con_params[l][0])) {
    for(j=0;j<GPIO_SIZE;j++) gi_gpio_in[j]=-1 ;
    for (j = 0, str1 = lp_Con->con_params[l][1]; ; j++, str1 = NULL) {
      token = strtok_r(str1, ",", &sptr);
      if (token == NULL) break ;
      gi_gpio_in[j]=atoi(token);
    }
   }
   if(!strcmp("GPIO_OUTPUT",lp_Con->con_params[l][0])) {
    for(i=0;i<GPIO_SIZE;i++) gi_gpio_out[i]=-1 ;
    for (j = 0, str1 = lp_Con->con_params[l][1]; ; j++, str1 = NULL) {
      token = strtok_r(str1, ",", &sptr);
      if (token == NULL) break ;
      gi_gpio_out[j]=atoi(token);
    }
   }
  }
   for(i=0;i<GPIO_SIZE;i++) Trace1("gi_gpio_in[%d] =%d\n",i,gi_gpio_in[i]);
   for(i=0;i<GPIO_SIZE;i++) Trace1("gi_gpio_out[%d]=%d\n",i,gi_gpio_out[i]);
}

/*****************************************************************************************
* @fn     : GPIO_STATUT
* @author : s.gravois
* @brief  : fonction qui obtient le statut d'une broche gpio
* @param  : int     sig
* @date   : 2022-05-20 creation 
* @todo   : voir s il est preferable de trapper egalement depuis les threads (si c possible)
*****************************************************************************************/

void GPIO_STATUT(void) {

  int i ;
  
  Trace("ret GPIO_OPEN  = %d\n",GPIO_OPEN(gi_gpio_in,gi_gpio_out)) ;
      
  for(i=0;gi_gpio_in[i]>0 ;i++) 
    Trace("statut de la broche gi_gpio_in[%3d]=%3d ==> %3d\n", i,\
      gi_gpio_in[i], GPIO_GET( gi_gpio_in[i] ) ) ;
  
  Trace("ret GPIO_CLOSE = %d\n",GPIO_CLOSE(gi_gpio_in,gi_gpio_out)) ;
}

/*****************************************************************************************
* @fn     : GPIO_GET
* @author : s.gravois
* @brief  : Lit la broche gpio de numero gpio
* @param  : int[] gi_gpio_fd
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : (obsolete a priori)
*****************************************************************************************/

int GPIO_GET(int gpio) {
  
  char buf[GPIO_BUFFER_SIZE_256] ;
  int  val ;
  memset(buf, CONFIG_ZERO_CHAR, sizeof(buf));
  
  if ( GPIO_OPEN_STATUT == 0 ) {
    if(pread(gi_gpio_fd[gpio],buf,GPIO_BUFFER_SIZE_256,0)>0)
      if(sscanf(buf,"%d",&val)==1)
        return val ;
  }
  else {
    Trace2("GPIO non dispos : %d\n",gpio) ;
  }
  return 0 ;
}
/*****************************************************************************************
* @fn     : GPIO_GET
* @author : s.gravois
* @brief  : Ecrit sur la broche gpio de numero gpio la valeur val (0 ou 1)
* @brief  :   est utilisé pour la led d etat (ecriture simple) par exemple
* @param  : int gpio
* @param  : int val
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : (cette fonction encapsule l'appel a pwrite, qui est utilise
* @todo   :   directement dans _GPIO_PWM_PHASE
*****************************************************************************************/

int GPIO_SET(int gpio,int val) {
  
  char buf[GPIO_BUFFER_SIZE_256] ;
  int retour ;
  retour=0 ;

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset(buf, CONFIG_ZERO_CHAR, sizeof(buf));
  snprintf(buf,GPIO_BUFFER_SIZE_256,"%d\n",val) ;

 /* PWM :   pwrite( lp_Pha->pha_gpio_fd,c_buf0,strlen(c_buf0),0) ; */

  if ( GPIO_OPEN_STATUT == 0 ) {
     retour=pwrite(gi_gpio_fd[gpio],buf,strlen(buf),0) ;
  }
  else {
    Trace2("GPIO non dispo : simulation : gpio %d set %d\n",gi_gpio_fd[gpio], val) ;
  }
  return retour ;
}
/*****************************************************************************************
* @fn     : GPIO_OPEN_BROCHE
* @author : s.gravois
* @brief  : Ouvre la broche gpio de numero gpio en input ou output
* @param  : int gpio
* @param  : int output
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : fusionner le if et le else (seul le fprintf sur "dir" differe)
*****************************************************************************************/

int GPIO_OPEN_BROCHE(int gpio,int output) {
  
  FILE *f ;
  
  char dir[GPIO_BUFFER_SIZE_256] ;
  char val[GPIO_BUFFER_SIZE_256] ;
  char exp[GPIO_BUFFER_SIZE_256] ;
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  GPIO_OPEN_STATUT=0 ;
  
  memset( exp, CONFIG_ZERO_CHAR, sizeof( exp ) );
  memset( dir, CONFIG_ZERO_CHAR, sizeof( dir ) );
  memset( val, CONFIG_ZERO_CHAR, sizeof( val ) );
  
  memset(gi_gpio_fd, CONFIG_ZERO_CHAR, sizeof( gi_gpio_fd) );
    
  sprintf(exp,"%s/export",GPIO_PATH) ;
  sprintf(dir,"%s/gpio%d/direction",GPIO_PATH,gpio) ;
  sprintf(val,"%s/gpio%d/value",GPIO_PATH,gpio) ;
  
  Trace1("exp=%s",exp);
  Trace1("dir=%s",dir);
  Trace1("val=%s",val);
    
  if ( output ) {
    
    Trace1("gpio ouverture en OUTPUT : dir= %s exp=%s val=%s",dir,exp,val);
    
    if ((f=fopen(exp,"w")) == NULL )                      { GPIO_OPEN_STATUT = -1  ; }
    else if ((fprintf(f,"%d\n",gpio))<=0)                 { GPIO_OPEN_STATUT = -2  ; }
    else if ((fclose(f)) != 0 )                           { GPIO_OPEN_STATUT = -3  ; }
    else if ((f=fopen(dir,"w")) == NULL)                  { GPIO_OPEN_STATUT = -4  ; }
    else if ((fprintf(f,"out\n")<=0))                     { GPIO_OPEN_STATUT = -5  ; }
    else if ((fclose(f)!=0))                              { GPIO_OPEN_STATUT = -6  ; }
    else if ((gi_gpio_fd[ gpio ]=open(val, O_WRONLY))<0)     { GPIO_OPEN_STATUT = -7  ; }
  }
  else {

    Trace1("gpio ouverture en INPUT : dir=%s exp=%s val=%s",dir,exp,val);
    
    if ((f=fopen(exp,"w")) == NULL )                      { GPIO_OPEN_STATUT = -8   ; }
    else if ((fprintf(f,"%d\n",gpio))<=0)                 { GPIO_OPEN_STATUT = -9   ; }
    else if ((fclose(f)) != 0 )                           { GPIO_OPEN_STATUT = -10  ; }
    else if ((f=fopen(dir,"w")) == NULL)                  { GPIO_OPEN_STATUT = -11  ; }
    else if ((fprintf(f,"in\n")<=0))                      { GPIO_OPEN_STATUT = -12  ; }
    else if ((fclose(f)!=0))                              { GPIO_OPEN_STATUT = -13  ; }
    else if ((gi_gpio_fd[ gpio ]=open(val, O_RDONLY))<0)     { GPIO_OPEN_STATUT = -14  ; }
  }
  
  return GPIO_OPEN_STATUT ;
}
/*****************************************************************************************
* @fn     : GPIO_OPEN_BROCHE_PWM
* @author : s.gravois
* @brief  : Meme fonction que precedemment mais en utilisant la struct STRUCT_GPIO_PWM_PHASE
* @brief  :  seule l ouverture en ecriture est fonctionnelle (par definition de out+pwm)
* @param  : STRUCT_GPIO_PWM_PHASE *lp_Pha
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : 
*****************************************************************************************/

void GPIO_OPEN_BROCHE_PWM(STRUCT_GPIO_PWM_PHASE *lp_Pha) {
  
  FILE *f ;
  char dir[GPIO_BUFFER_SIZE_256] ;
  char val[GPIO_BUFFER_SIZE_256] ;
  char exp[GPIO_BUFFER_SIZE_256] ;
  char unexport[GPIO_BUFFER_SIZE_256] ;
  
  memset(unexport,CONFIG_ZERO_CHAR, sizeof( unexport ));
  memset(exp,     CONFIG_ZERO_CHAR, sizeof( exp ));
  memset(dir,     CONFIG_ZERO_CHAR, sizeof( dir ));
  memset(val,     CONFIG_ZERO_CHAR, sizeof( val ));
  
  sprintf(unexport,"%s/unexport",GPIO_PATH) ;
  //close( lp_Pha->pha_gpio_fd ) ;
  
  lp_Pha->pha_gpio_open_statut = 0 ;
  
  if ((f=fopen(unexport,"w")) == NULL )        { lp_Pha->pha_gpio_open_statut = -1 ;}
  else if ((fprintf(f,"%d\n",lp_Pha->pha_gpio))<=0)  { lp_Pha->pha_gpio_open_statut = -2 ;}
  else if ((fclose(f)) != 0 )                  { lp_Pha->pha_gpio_open_statut = -3 ;}
  
  lp_Pha->pha_gpio_open_statut = 0 ;
  
  lp_Pha->pha_gpio_fd=0;
    
  sprintf(exp,"%s/export",          GPIO_PATH ) ;
  sprintf(dir,"%s/gpio%d/direction",GPIO_PATH, lp_Pha->pha_gpio) ;
  sprintf(val,"%s/gpio%d/value",    GPIO_PATH, lp_Pha->pha_gpio) ;
  
  Trace1("exp=%s",exp);
  Trace1("dir=%s",dir);
  Trace1("val=%s",val);
    
  if      ((f=fopen(exp,"w")) == NULL )                 { lp_Pha->pha_gpio_open_statut = -4  ; }
  else if ((fprintf(f,"%d\n",lp_Pha->pha_gpio))<=0)         { lp_Pha->pha_gpio_open_statut = -5  ; }
  else if ((fclose(f)) != 0 )                           { lp_Pha->pha_gpio_open_statut = -6  ; }
  else if ((f=fopen(dir,"w")) == NULL)                  { lp_Pha->pha_gpio_open_statut = -7  ; }
  else if ((fprintf(f,"out\n")<=0))                     { lp_Pha->pha_gpio_open_statut = -8  ; }
  else if ((fclose(f)!=0))                              { lp_Pha->pha_gpio_open_statut = -9  ; }
  else if ((lp_Pha->pha_gpio_fd =open(val, O_WRONLY))<0)      { lp_Pha->pha_gpio_open_statut = -10  ; }
  
  // FIXME : synthese traces sur une ligne (2021)
  Trace1("open gpio %d : dir= %s exp=%s val=%s sta=%d",\
    lp_Pha->pha_gpio,\
    dir,\
    exp,\
    val,\
    lp_Pha->pha_gpio_open_statut);
  
  if ( lp_Pha->pha_gpio_open_statut != 0 ) {
    SyslogErrFmt("ouverture du gpio %d en erreur %d",lp_Pha->pha_gpio,lp_Pha->pha_gpio_open_statut);
    Trace("ouverture du gpio %d (KO) : %d",lp_Pha->pha_gpio,lp_Pha->pha_gpio_open_statut) ;
  }
  else {
    Trace("ouverture du gpio %d (OK)",lp_Pha->pha_gpio) ;  
  }

}
/*****************************************************************************************
* @fn     : GPIO_OPEN
* @author : s.gravois
* @brief  : Ouvre plusieurs broches GPIO, en utilisant gi_gpio_in[] & gi_gpio_out[]
* @param  : int[] gi_gpio_in
* @param  : int[] gi_gpio_out
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : (est utilisee par GPIO_STATUT et GPIO_TEST_CONTROLER)
*****************************************************************************************/

int GPIO_OPEN(int gi_gpio_in[GPIO_SIZE],int gi_gpio_out[GPIO_SIZE]) {

  FILE *f ;
  char exp[GPIO_BUFFER_SIZE_256] ;
  char dir[GPIO_BUFFER_SIZE_256] ;
  char val[GPIO_BUFFER_SIZE_256] ;
  char err[GPIO_BUFFER_SIZE_256] ;
  int i ;
  
  i=-1;
  
  GPIO_OPEN_STATUT=0 ;
  
  while(gi_gpio_out[++i]>0) {
  
    memset( err, CONFIG_ZERO_CHAR, sizeof( err ) ) ;
    memset( exp, CONFIG_ZERO_CHAR, sizeof( exp ) );
    memset( dir, CONFIG_ZERO_CHAR, sizeof( dir ) );
    memset( val, CONFIG_ZERO_CHAR, sizeof( val ) );
    
    sprintf( exp,"%s/export",           GPIO_PATH) ;
    sprintf( dir,"%s/gpio%d/direction", GPIO_PATH, gi_gpio_out[i]) ;
    sprintf( val,"%s/gpio%d/value",     GPIO_PATH, gi_gpio_out[i]) ;

    if ((f=fopen(exp,"w")) == NULL )                      { GPIO_OPEN_STATUT = -1 ; break ; }
    else if ((fprintf(f,"%d\n",gi_gpio_out[i]))<=0)          { GPIO_OPEN_STATUT = -2 ; break ; }
    else if ((fclose(f)) != 0 )                           { GPIO_OPEN_STATUT = -3 ; break ; }
    else if ((f=fopen(dir,"w")) == NULL)                  { GPIO_OPEN_STATUT = -4 ; break ; }
    else if ((fprintf(f,"out\n")<=0))                     { GPIO_OPEN_STATUT = -5 ; break ; }
    else if ((fclose(f)!=0))                              { GPIO_OPEN_STATUT = -6 ; break ; }
    else if ((gi_gpio_fd[gi_gpio_out[i]]=open(val,O_WRONLY))<0) { GPIO_OPEN_STATUT = -7 ; break ; }
  }
  
  i=-1;
  
  if ( GPIO_OPEN_STATUT == 0 )
   while(gi_gpio_in[++i]>0) {
    
    memset(dir, CONFIG_ZERO_CHAR, sizeof( dir ) );
    memset(exp, CONFIG_ZERO_CHAR, sizeof( exp ) );
    memset(val, CONFIG_ZERO_CHAR, sizeof( val ) );
    
    sprintf(exp,"%s/export",GPIO_PATH) ;
    sprintf(dir,"%s/gpio%d/direction",GPIO_PATH,gi_gpio_in[i]) ;
    sprintf(val,"%s/gpio%d/value",GPIO_PATH,gi_gpio_in[i]) ;
    
    Trace("gpio ouverture en INPUT : exp=%s val=%s\n",exp,val);
    
    if ((f=fopen(exp,"w")) == NULL )                      { GPIO_OPEN_STATUT = -8 ; break ; }
    else if ((fprintf(f,"%d\n",gi_gpio_in[i]))<=0)           { GPIO_OPEN_STATUT = -9 ; break ; }
    else if ((fclose(f)) != 0 )                           { GPIO_OPEN_STATUT = -10 ; break ; }
    else if ((f=fopen(dir,"w")) == NULL)                  { GPIO_OPEN_STATUT = -11 ; break ; }
    else if ((fprintf(f,"in\n")<=0))                      { GPIO_OPEN_STATUT = -12 ; break ; }
    else if ((fclose(f)!=0))                              { GPIO_OPEN_STATUT = -13 ; break ; }
    else if ((gi_gpio_fd[gi_gpio_in[i]]=open(val,O_RDONLY))<0)  { GPIO_OPEN_STATUT = -14 ; break ; }
   }
  
  return GPIO_OPEN_STATUT ;
}
/*****************************************************************************************
* @fn     : GPIO_CLOSE_BROCHE
* @author : s.gravois
* @brief  : Ferme une broche GPIO, en utilisant son numero gpio
* @param  : int gpio
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : 
*****************************************************************************************/

int GPIO_CLOSE_BROCHE(int gpio) {
  
  FILE *f ;
  char une[GPIO_BUFFER_SIZE_256] ;
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset( une, CONFIG_ZERO_CHAR, sizeof( une ));
  sprintf(une,"%s/unexport",GPIO_PATH) ;

  Trace("fermeture GPIO %d", gi_gpio_fd [ gpio ] ) ;

  if ((f=fopen(une,"w")) == NULL )   { return -1 ;}
  if ((fprintf(f,"%d\n",gpio))<=0) { return -2 ;}
  if ((fclose(f)) != 0 ) { return -3 ;}

  return 0 ;
}
/*****************************************************************************************
* @fn     : GPIO_CLOSE
* @author : s.gravois
* @brief  : Ferme plusieurs broches GPIO, en utilisant gi_gpio_in[] & gi_gpio_out[]
* @param  : int[] gi_gpio_in
* @param  : int[] gi_gpio_out
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : 
*****************************************************************************************/

int GPIO_CLOSE(int gi_gpio_in[GPIO_SIZE],int gi_gpio_out[GPIO_SIZE]) {
  
  FILE *f ;
  char une[GPIO_BUFFER_SIZE_256] ;
  int i ;
  
  i=-1;
  while(gi_gpio_out[++i]>0) {
    memset(une, CONFIG_ZERO_CHAR, sizeof(une));
    sprintf(une,"%s/unexport",GPIO_PATH) ;
    close( gi_gpio_fd [ gi_gpio_out [ i ] ] ) ;
    if ((f=fopen(une,"w")) == NULL )   { return -1 ;}
    if ((fprintf(f,"%d\n",gi_gpio_out[i]))<=0) { return -2 ;}
    if ((fclose(f)) != 0 ) { return -3 ;}
  }
  i=-1;
  while(gi_gpio_in[++i]>0) {
    memset(une, CONFIG_ZERO_CHAR, sizeof(une));
    sprintf(une,"%s/unexport",GPIO_PATH) ;
    close( gi_gpio_fd [ gi_gpio_in [ i ] ] ) ;
    if ((f=fopen(une,"w")) == NULL )   { return -4 ;}
    if ((fprintf(f,"%d\n",gi_gpio_in[i]))<=0) { return -5 ;}
    if ((fclose(f)) != 0 ) { return -6 ;}
  }
  return 0 ;
}
/*****************************************************************************************
* @fn     : GPIO_SET_ALT
* @author : s.gravois
* @brief  : Ecrit sur 6 broches gpio via la struct gp_Ctl_Par (parametres controleur)
* @brief  :  (prevu ici pour "altitude")
* @param  : int dir, int slp, int clk, int rst, int mmm, int ena
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : renommer par un nom plus approprie
* @todo   : passer par un argument pour gp_Ctl_Par
*****************************************************************************************/

void GPIO_SET_ALT(int dir, int slp, int clk, int rst, int mmm, int ena) {

  GPIO_SET( gp_Ctl_Par->par_alt_dir,dir);   
  GPIO_SET( gp_Ctl_Par->par_alt_clk,slp);      
  GPIO_SET( gp_Ctl_Par->par_alt_slp,clk);     
  GPIO_SET( gp_Ctl_Par->par_alt_rst,rst);     
  GPIO_SET( gp_Ctl_Par->par_alt_mmm,mmm);      
  GPIO_SET( gp_Ctl_Par->par_alt_ena,ena);  
}

/*****************************************************************************************
* @fn     : GPIO_SET_AZI
* @author : s.gravois
* @brief  : Ecrit sur 6 broches gpio via la struct gp_Ctl_Par (parametres controleur)
* @brief  :  (prevu ici pour "azimut")
* @param  : int dir, int slp, int clk, int rst, int mmm, int ena
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : renommer par un nom plus approprie
* @todo   : passer par un argument pour gp_Ctl_Par
*****************************************************************************************/

void GPIO_SET_AZI(int dir, int slp, int clk, int rst, int mmm, int ena) {

  GPIO_SET( gp_Ctl_Par->par_azi_dir,dir);   
  GPIO_SET( gp_Ctl_Par->par_azi_clk,slp);      
  GPIO_SET( gp_Ctl_Par->par_azi_slp,clk);     
  GPIO_SET( gp_Ctl_Par->par_azi_rst,rst);     
  GPIO_SET( gp_Ctl_Par->par_azi_mmm,mmm);      
  GPIO_SET( gp_Ctl_Par->par_azi_ena,ena);  
}
/*****************************************************************************************
* @fn     : GPIO_MOVE_1
* @author : s.gravois
* @brief  : Deplace un moteur en utilisant GPIO_SET et plusieurs parametres dont 
* @param  : int sens, double periode,double nb_pulse, int gpio_dir, int gpio_clk
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : renommer : fonction prevue avec controleur externe
*****************************************************************************************/

void GPIO_MOVE_1(int sens, double periode,double nb_pulse, int gpio_dir, int gpio_clk) {
  
  unsigned long demi_periode ;
  unsigned long incr ;
  
  //periode_micro = ( periode * TIME_MICRO_SEC ) ;
  demi_periode = (long) (periode * TIME_MICRO_SEC / 2.0) ;
  
  Trace("deplacement sens=%d / demi_periode(us)=%ld / nombre impulsions=%f\n",sens ,demi_periode , nb_pulse) ;
  
  incr=0 ;
  
  if (sens <=0) GPIO_SET( gpio_dir, 0 );
  else          GPIO_SET( gpio_dir, 1 );
  
  // nb_pulse = 10000 ;  
  
  while( ++incr < (unsigned long)nb_pulse ) {
    //Trace("deplacement sens=%d / demi_periode(us)=%ld / nombre impulsions=%f\n",sens ,demi_periode , nb_pulse) ;
    usleep( demi_periode) ;
    GPIO_SET( gpio_clk, 0 ) ;
    
    usleep( demi_periode ) ;
    GPIO_SET( gpio_clk, 1 ) ;
  }
  Trace("Fin GPIO_MOVE_1\n ") ;
}
/*****************************************************************************************
* @fn     : GPIO_MOVE_2
* @author : s.gravois
* @brief  : Deplace un moteur en utilisant GPIO_SET et plusieurs parametres dont 
* @param  : int sens, double periode,double nb_pulse
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : renommer : fonction prevue avec controleur externe
* @todo   : (utilise struct gp_Ctl_Par <=> controleur )
*****************************************************************************************/

void GPIO_MOVE_2(int sens, double periode,unsigned  long nb_pulse) {
  
  double periode_micro ;

  unsigned long incr ;
  
  incr=0 ;
  
  if (sens <=0) {
    GPIO_SET( gp_Ctl_Par->par_alt_dir, 0 );
    GPIO_SET( gp_Ctl_Par->par_azi_dir, 0 );
  }
  else {
    GPIO_SET( gp_Ctl_Par->par_alt_dir, 1 );
    GPIO_SET( gp_Ctl_Par->par_azi_dir, 1 );
  } 
  periode_micro = ( periode * TIME_MICRO_SEC ) ;
  
  while( ++incr < nb_pulse+1 ) {
  
    usleep( (long) (periode_micro / 2.0) ) ;
    
    GPIO_SET( gp_Ctl_Par->par_alt_clk, 0 );
    GPIO_SET( gp_Ctl_Par->par_azi_clk, 0 );
    
    usleep( (long) (periode_micro / 2.0) ) ;
    
    GPIO_SET( gp_Ctl_Par->par_alt_clk, 1 );
    GPIO_SET( gp_Ctl_Par->par_azi_clk, 1 );
  }
}
/*****************************************************************************************
* @fn     : GPIO_MOVE_3
* @author : s.gravois
* @brief  : Deplace un moteur en utilisant GPIO_SET et plusieurs parametres dont 
* @param  : int sens, double periode,double nb_pulse, int gpio_dir, int gpio_clk
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : renommer : fonction prevue avec controleur externe
* @todo   : analyser a quoi sert cette fonction (1 dure plus longtemps que 0)
*****************************************************************************************/

void GPIO_MOVE_3(int sens, double periode,double nb_pulse, int gpio_dir, int gpio_clk) {
  
  double         demi_periode_sleep ;
  double         t_diff, nbpulse ;
  struct timeval t00,t01 ;
  double         duree_microsec ;
  
  gettimeofday(&t00,NULL) ;
  
  duree_microsec       = periode * TIME_MICRO_SEC ;    
  demi_periode_sleep   = duree_microsec / 2.2 ;
  
  Trace("deplacement sens=%d / demi_periode(us)=%f / nombre impulsions=%f\n",sens ,demi_periode_sleep	 , nb_pulse) ;
    
  if (sens <=0) GPIO_SET( gpio_dir, 0 );
  else          GPIO_SET( gpio_dir, 1 );
    
  nbpulse=0 ;
  
  while( nbpulse < nb_pulse) {
    
    t_diff=0 ; 
    nbpulse = nbpulse +1 ;
    
    usleep( demi_periode_sleep) ;
    GPIO_SET( gpio_clk, 0 ) ;
    
    usleep( demi_periode_sleep ) ;
    GPIO_SET( gpio_clk, 1 ) ;
      
    while( t_diff < duree_microsec ) {
      t_diff = (( t01.tv_sec - t00.tv_sec ) * TIME_MICRO_SEC) + t01.tv_usec - t00.tv_usec ;
      gettimeofday(&t01,NULL) ;
    }
    gettimeofday(&t00,NULL) ;
  }
  Trace("Fin GPIO_MOVE_3\n ") ;
}
/*****************************************************************************************
* @fn     : GPIO_ACCELERATION_1
* @author : s.gravois
* @brief  : Accelere un moteur en utilisant GPIO_SET et plusieurs parametres dont 
* @param  : int gpio, double f_deb,double f_fin, double delai,long nano_moins
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : renommer : fonction prevue avec controleur externe
* @todo   : analyser le contionnement
*****************************************************************************************/

long GPIO_ACCELERATION_1(int gpio, double f_deb,double f_fin, double delai,long nano_moins)
{
  long   incr, demi_slp ;
  double per_deb, per_fin, incr_per, nb_pulsation_totales ;
  struct timespec nt,ntt ;
  
  per_deb = 1/f_deb ;
  per_fin = 1/f_fin ;
  
  nb_pulsation_totales = delai * ( f_deb + f_fin ) / 2.0 ;
  
  if( per_deb > per_fin )  incr_per = per_deb * ( per_deb - per_fin ) / ( delai + per_fin + per_deb ) ; 
  else                     incr_per = per_fin * ( per_fin - per_deb ) / ( delai + per_deb + per_fin ); 
  
  incr=0 ;
  if ( per_deb < per_fin )
  while( per_deb < per_fin ) {
    demi_slp = (long)((double)(TIME_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(gpio,0) ; 
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(gpio,1) ;  
    nanosleep(&nt,&ntt) ;
    
    per_deb += incr_per ;
    incr++ ;
  }
  if ( per_deb > per_fin )

  while( per_deb > per_fin ) {
    demi_slp = (long)((double)(TIME_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec  = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(gpio,0) ;  
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(gpio,1) ;  
    nanosleep(&nt,&ntt) ;
    
    per_deb -= incr_per ;
    incr++ ;
  }
  if ( per_deb == per_fin )

  while( incr < nb_pulsation_totales ) {
    demi_slp = (long)((double)(TIME_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec  = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(gpio,0) ;  
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(gpio,1) ;  
    nanosleep(&nt,&ntt) ;
    
    per_deb += incr_per ;
    incr++ ;
  }
  return incr ;
}
/*****************************************************************************************
* @fn     : GPIO_ACCELERATION_2
* @author : s.gravois
* @brief  : Idem GPIO_ACCELERATION_1 mais sur altitude & azimut en meme temps
* @param  : int alt, int azi, double f_deb,double f_fin, double delai,long nano_moins
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : renommer : fonction prevue avec controleur externe
* @todo   : analyser le contionnement
*****************************************************************************************/

long GPIO_ACCELERATION_2(int alt, int azi, double f_deb,double f_fin, double delai,long nano_moins)
{
  long   incr, demi_slp ;
  double per_deb, per_fin, incr_per, nb_pulsation_totales ;
  struct timespec nt,ntt ;
  
  per_deb = 1/f_deb ;
  per_fin = 1/f_fin ;
  
  nb_pulsation_totales = delai * ( f_deb + f_fin ) / 2.0 ;
  
  if( per_deb > per_fin )  incr_per = per_deb * ( per_deb - per_fin ) / ( delai + per_fin + per_deb ) ; 
  else                     incr_per = per_fin * ( per_fin - per_deb ) / ( delai + per_deb + per_fin ); 
  
  incr=0 ;
  if ( per_deb < per_fin )
  while( per_deb < per_fin ) {
    demi_slp = (long)((double)(TIME_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(alt,1) ; 
    GPIO_SET(azi,1) ; 
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(alt,0) ; 
    GPIO_SET(azi,0) ; 
    nanosleep(&nt,&ntt) ;
    
    per_deb += incr_per ;
    incr++ ;
  }
  if ( per_deb > per_fin )
  while( per_deb > per_fin ) {
    demi_slp = (long)((double)(TIME_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec  = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(alt,1) ; 
    GPIO_SET(azi,1) ; 
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(alt,0) ; 
    GPIO_SET(azi,0) ; 
    nanosleep(&nt,&ntt) ;
    
    per_deb -= incr_per ;
    incr++ ;
  }
  if ( per_deb == per_fin )
  while( incr < nb_pulsation_totales ) {
    demi_slp = (long)((double)(TIME_NANO_SEC-nano_moins) * (double)per_deb / 2.0 ) ;
    nt.tv_sec  = 0 ;
    nt.tv_nsec = demi_slp ;
    
    GPIO_SET(alt,1) ; 
    GPIO_SET(azi,1) ; 
    nanosleep(&nt,&ntt) ;
    
    GPIO_SET(alt,0) ; 
    GPIO_SET(azi,0) ; 
    nanosleep(&nt,&ntt) ;
    
    per_deb += incr_per ;
    incr++ ;
  }
  return incr ;
}
/*****************************************************************************************
* @fn     : GPIO_FONCTION_RAPPORT_CYCLIQUE
* @author : s.gravois
* @brief  : Fonction qui calcule la forme pseudo sinusoidale appropriee pour la modulation PWM
* @param  : int num_bobine
* @param  : double t
* @param  : STRUCT_GPIO_PWM_MOTEUR *lp_Mot
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : renommer : fonction prevue avec PWM 4 broches
* @todo   : axe d amelioration possible sur generation de la fonction mixte
*****************************************************************************************/

double GPIO_FONCTION_RAPPORT_CYCLIQUE(int num_bobine, double t, STRUCT_GPIO_PWM_MOTEUR *lp_Mot) {
  
  double rc  ;         // rapport cyclique a calculer
  double d = M_PI/2.0 ;  // dephasage entre bobines
  double w = 2*M_PI*lp_Mot->mot_fm ; // pulsation du courant dans les bobines
  double sigc, trc, sc, dc , pi = M_PI ;
  rc = 0 ;

  if ( num_bobine == 0 ) { 
    
    Trace1("%f %f %f %d %f %f", t, w, d, lp_Mot->type_fonction, lp_Mot->param0, lp_Mot->param1 ) ;
    
    switch ( lp_Mot->type_fonction) {  
     case 1 : rc = lp_Mot->param1 * sin(w*t) ;                                      Trace1("fonction sinusoidale") ;break;
     case 0 : rc = lp_Mot->param1 * acos(cos(2*w*t))/(M_PI/2)-1 ;                     Trace1("fonction triangulaire") ;break ;
     case 2 : rc = lp_Mot->param1 * sin(w*t)+SGN(cos(w*t))* lp_Mot->param0 *sin(4*w*t) ;Trace1("fonction mixte") ;break;
     case 3 : 
// a=0.5 b=2
// a=0.6 b=1.66
// a=0.7 b=1.43
// a=0.9 b=1.1
// a=1.0 b=1.0
       sigc = SGN(sin(w*t)) ;
       trc  = -(1+lp_Mot->param0)*(acos(cos(w*t))/(pi/2)-1)	 ;
       sc   = cos(w*t) ;
       if (lp_Mot->param0 != 0 ) dc   = ((-sc*(trc-sc))+(lp_Mot->param0))/lp_Mot->param0 ;
       else                  dc   = (-sc*(trc-sc))+(lp_Mot->param0) ;
       rc   = lp_Mot->param1 *  dc * sigc ;
       Trace1("fonction la meilleure pour les micro pas") ;
       break ;
    }
  }
  else {
    switch ( lp_Mot->type_fonction) {  
     case 1 : rc = lp_Mot->param1 * sin(w*t+d) ;                                         Trace1("fonction sinusoidale") ;break;
     case 0 : rc = lp_Mot->param1 * acos(cos(2*w*t+d))/(M_PI/2)-1 ;                        Trace1("fonction triangulaire") ;break;
     case 2 : rc = lp_Mot->param1 * sin(w*t+d)+SGN(cos(w*t+d))* lp_Mot->param0 *sin(4*w*t) ; Trace1("fonction mixte") ;break;
     case 3 : 
       sigc = SGN(cos(w*t)) ;
       trc  = -(1+lp_Mot->param0)*(acos(cos(w*t-pi/2))/(pi/2)-1)	 ;
       sc   = sin(w*t) ; 
       if (lp_Mot->param0 != 0 ) dc   = ((-sc*(trc-sc))+(lp_Mot->param0))/lp_Mot->param0 ;
       else                  dc   = (-sc*(trc-sc))+(lp_Mot->param0) ;
       
       rc   = lp_Mot->param1 * dc * sigc ;
       Trace1("fonction la meilleure pour les micro pas") ;
       break ;
    }
  }
  
  return rc ;

  // BOBINE1
  // rc = sin( 2 * M_PI * lp_Mot->mot_fm * t ) ;
  // rc = sin(2*M_PI*lp_Mot->mot_fm*t)+SGN(cos(2*M_PI*lp_Mot->mot_fm*t))*0.15*sin(2*M_PI*4*lp_Mot->mot_fm*t) ;
  // rc = acos(cos(2*M_PI*lp_Mot->mot_fm*2*t))/(M_PI/2)-1 ; // fonction triangulaire

  // BOBINE2
  // rc = -1.0 * cos( 2 * M_PI * lp_Mot->mot_fm * t ) ;
  // rc = sin(2*M_PI*lp_Mot->mot_fm*t+M_PI/2)+SGN(cos(2*M_PI*lp_Mot->mot_fm*t+M_PI/2))*0.15*sin(2*M_PI*4*lp_Mot->mot_fm*t) ;
  // rc =  acos(cos(2*M_PI*lp_Mot->mot_fm*2*t+M_PI/2))/(M_PI/2)-1 ; // fonction triangulaire
}
/*****************************************************************************************
* @fn     : GPIO_CALCULS_PWM_RAPPORTS_CYCLIQUES
* @author : s.gravois
* @brief  : Fonction qui calcule les rapports cycliques et les charge en memoire 
* @brief  : en utilisant GPIO_FONCTION_RAPPORT_CYCLIQUE pour la forme de la fct sin.
* @param  : STRUCT_GPIO_PWM_MOTEUR *lp_Mot
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : renommer : fonction prevue avec PWM 4 broches
* @todo   : se base sur un echantilonnage du temps sur un segment borne par lp_Mot->mot_per_mic
*****************************************************************************************/

void GPIO_CALCULS_PWM_RAPPORTS_CYCLIQUES(STRUCT_GPIO_PWM_MOTEUR *lp_Mot) {
  
  int i, j;
  double t  ;
  double rc  ;         // rapport cyclique a calculer

  j=0 ;
  
  for( i=0 ; i < GPIO_NB_PHASES_PAR_MOTEUR ; i++ ) 
    for( j=0;j < GPIO_MICROPAS_MAX; j++ )
      lp_Mot->mot_pha[i]->pha_rap[j] = 0 ;
    
  j=0 ; 

  Trace1("lp_Mot->mot_per_mic = %f", lp_Mot->mot_per_mic ) ;

  for( t=0 ; t< lp_Mot->mot_tm ;  t += lp_Mot->mot_per_mic ) {
    
    Trace2("t = %f\t%f\t%f",t,lp_Mot->mot_tm ,lp_Mot->mot_per_mic) ;
    
    rc = GPIO_FONCTION_RAPPORT_CYCLIQUE(0, t, lp_Mot )  ;

    if ( rc >  0)  { 
      lp_Mot->mot_pha[1]->pha_rap[j] = rc ;        
      lp_Mot->mot_pha[0]->pha_rap[j] = 0.0 ; 
    }
    else  { 
      lp_Mot->mot_pha[0]->pha_rap[j] = rc * -1.0 ; 
      lp_Mot->mot_pha[1]->pha_rap[j] = 0.0 ; 
    }

    rc = GPIO_FONCTION_RAPPORT_CYCLIQUE(1, t, lp_Mot )  ;    

    if ( rc >  0)  { 
      lp_Mot->mot_pha[3]->pha_rap[j] = rc ;        
      lp_Mot->mot_pha[2]->pha_rap[j] = 0.0 ; 
    }
    else { 
      lp_Mot->mot_pha[2]->pha_rap[j] = rc * -1.0 ; 
      lp_Mot->mot_pha[3]->pha_rap[j] = 0.0 ; 
    }
 
    j++ ;
  }
  for( i=0 ; i< j ; i++ ) {
    if( ( lp_Mot->mot_pha[0]->pha_rap[i]+ \
          lp_Mot->mot_pha[1]->pha_rap[i]+ \
	  lp_Mot->mot_pha[2]->pha_rap[i]+ \
	  lp_Mot->mot_pha[3]->pha_rap[i] ) != 0 )

    Trace2("%d\t%f\t%f\t%f\t%f",i,lp_Mot->mot_pha[0]->pha_rap[i],lp_Mot->mot_pha[1]->pha_rap[i],lp_Mot->mot_pha[2]->pha_rap[i],lp_Mot->mot_pha[3]->pha_rap[i]) ;
  }
}
/*****************************************************************************************
 Cette fonction, pour chaque phase, en parallele dans 4 threads de meme priorite,
 module les etats hauts et bas de la phase
 avec les rapports cycliques calcules pour chaque micro pas
 et suivant une frequence fPWM superieure a la frequence max potentielle de deplacement des axes
 @date   : 2022-05-24 ajout protection par mutex des threads[ gi_pth_numero++ ]
 TODO : gerer autrement GPIO_SUIVI_MAIN_ATTENTE_MAX
 *****************************************************************************************/

/*****************************************************************************************
* @fn     : _GPIO_PWM_PHASE
* @author : s.gravois
* @brief  : Fonction de callback de thread qui gere la generation
* @brief  : des signaux sur les phases par modulation PWM 
* @param  : STRUCT_GPIO_PWM_PHASE *lp_Pha
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : 
*****************************************************************************************/

void * _GPIO_PWM_PHASE(STRUCT_GPIO_PWM_PHASE *lp_Pha ) {
  
  int    i_gpio_fd = 0 ;
  char   c_buf0[GPIO_BUFFER_SIZE_256] ;
  char   c_buf1[GPIO_BUFFER_SIZE_256] ;
  double d_TUpwm, d_TUpwm_haut, d_TUpwm_bas, d_rap ;
  double pmot =0 ;
  // struct sched_param param;

  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_PWM_PHASE ) ;

  TraceArbo(__func__,1,"pthread_create_callback_fct") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  d_TUpwm = d_TUpwm_haut  = d_TUpwm_bas = d_rap = 0 ;

  memset(c_buf0, CONFIG_ZERO_CHAR, sizeof( c_buf0 ) ); 
  memset(c_buf1, CONFIG_ZERO_CHAR, sizeof( c_buf1 ) ); 

  snprintf(c_buf0,GPIO_BUFFER_SIZE_256,"%d\n",0) ;
  snprintf(c_buf1,GPIO_BUFFER_SIZE_256,"%d\n",1) ;
 
  Trace("debut while") ;

  while(1) {
    
    /* Creee un point d 'annulation pour la fonction pthread_cancel */
    pthread_testcancel() ;

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &lp_Pha->pha_mutex ) ;
    
    d_rap     = lp_Pha->pha_rap[ lp_Pha->pha_upas ] ;
    d_TUpwm   = lp_Pha->pha_per_pwm * (double)TIME_MICRO_SEC ;
    pmot      = lp_Pha->pha_per_mot ;
    i_gpio_fd = lp_Pha->pha_gpio_fd ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &lp_Pha->pha_mutex ) ;
    
    d_TUpwm_haut = d_TUpwm * d_rap  ; 
    d_TUpwm_bas  = d_TUpwm - d_TUpwm_haut ;
    
    Trace1("Phid%-3d upas %-3d rapc %.3f Tpwmh %-3f Tpwmb %-3f ",lp_Pha->pha_id, lp_Pha->pha_upas, d_rap, d_TUpwm_haut, d_TUpwm_bas) ;

    // =================================================================
    // ecriture sur GPIO
    // si la periode moteur est trop longue (> 1 seconde) 
    // on n'attend pas et on n'ecrit pas sur les GPIOS
    // Cela evite de mettre le thread moteur en etat d'attente dans un usleep long
    // =================================================================

    if ( d_rap > 0 ) {
      // if ( lp_Pha->pha_per_mot < GPIO_SUIVI_MAIN_ATTENTE_MAX )
      pwrite( i_gpio_fd, c_buf1, strlen(c_buf1),0) ;
    }
    usleep( d_TUpwm_haut ) ;  

    pwrite( i_gpio_fd, c_buf0,strlen(c_buf0),0) ;
    
    usleep( d_TUpwm_bas ) ;
  }

  /* TODO : prevoir modif gp_Pth statut en cas de retour */
  
  return NULL ;
}

/*****************************************************************************************
* @fn     : _GPIO_PWM_MOT
* @author : s.gravois
* @brief  : Fonction de callback de thread qui gere la generation des signaux sur les phases
* @brief  : par modulation PWM (en collaboration avec _GPIO_PWM_PHASE*)
* @brief  : Rafraichis les rapports cycliques de tous les micro pas
* @brief  : et gestion du temps reel par algorithme PID
* @param  : STRUCT_GPIO_PWM_MOTEUR *lp_Mot
* @date   : 2022-05    ajout non prise en compte des micro-pas pour la frequence moteur
* @date   : 2022-05-24 correction lp_Mot->tps_mic += lp_Mot->periode ;  + m->tps_mic += lp_Mot->mot_per_mot
* @date   : 2022-06-14 correction petite erreur sur fre_ta_mot / fre_th_mot
* @todo   : avancement sur algo PID => terminer / evoluer
*****************************************************************************************/

void * _GPIO_PWM_MOT(STRUCT_GPIO_PWM_MOTEUR *lp_Mot) {

  int i_pas_change=0;
  int i=0 , sens=0 ;

  double d_ecart =0;       /* ecart entre Periodes attendues et reelles */
  double d_pid = 0 ;         /* parametre PID a appliquer */

  double periode_bru=0 ; /* periode brute calculee dans CALCULS_PERIODE */
  double periode_eff=0 ; /* periode effective pour la temporisation */
  double periode_ree=0 ; /* periode effective reelle consommee pour la temporisation */
  double periode_mic=0 ; /* periode de referefence (micro pas) pour la temporisation */
  double periode_mot=0 ; /* periode de referrence (sans micro pas ) du moteur */
  double per_tpwm = 0; 

  double ld_par_alt_red_4 = 0 ;
  double ld_par_azi_red_4 = 0 ;

  double rc=0 ;
  //double tdiff=0 ;
  long long   i_incr=0 ; 
  struct sched_param param;
  int     micropas=0 ;

  PTHREADS_CONFIG( gp_Pth, pthread_self(), PTHREAD_TYPE_PWM_MOTOR  ) ;

  TraceArbo(__func__,1,"pthread_create_callback_fct") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  periode_mic = 0 ;
  periode_mot = 0 ;
  periode_bru = 0 ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Mot->mot_mutex ) ;

  lp_Mot->mot_upas = 0 ;
  lp_Mot->mot_pas = 0 ;
  lp_Mot->mot_t = 0 ;

  gettimeofday(&lp_Mot->mot_tval,NULL) ; 

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Mot->mot_mutex ) ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Cal_Par->cal_par_mutex ) ;

  ld_par_alt_red_4 = gp_Cal_Par->cal_par_alt_red_4 ;
  ld_par_azi_red_4 = gp_Cal_Par->cal_par_azi_red_4 ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Cal_Par->cal_par_mutex ) ;

  while (1) {

    /* Creee un point d 'annulation pour la fonction pthread_cancel */
    pthread_testcancel() ;

    /* -----------------------------------------------------
       Recopie des donnees de suivi dans les variables locales 
       pour utilisation ulterieure 
       En fonction de l'id moteur (ALT ou AZI) 
       -----------------------------------------------------*/ 

    switch ( lp_Mot->mot_id ) {

      case 0 : 
        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Fre->fre_mutex ) ;

          periode_mic         = gp_Fre->fre_th_mic ;
          periode_mot         = gp_Fre->fre_th_mot / ld_par_alt_red_4;
          periode_bru         = gp_Fre->fre_th_bru / ld_par_alt_red_4 ;
          sens                = gp_Fre->fre_sh ;

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Fre->fre_mutex ) ;
        break ;

      case 1 :
        HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Fre->fre_mutex ) ;

          periode_mic         = gp_Fre->fre_ta_mic ; 
          periode_mot         = gp_Fre->fre_ta_mot / ld_par_azi_red_4 ;
          periode_bru         = gp_Fre->fre_ta_bru / ld_par_azi_red_4 ;
          sens                = gp_Fre->fre_sa ;

        HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Fre->fre_mutex ) ;

        break ;
    }

    i_pas_change=0;

    if ( sens > 0 ) {

      HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Mot->mot_mutex) ;

      if ( lp_Mot->mot_upas == lp_Mot->mot_nb_upas - 1 ) { 
        lp_Mot->mot_upas = 0 ; 
        /* Si lp_Mot->mot_upas = 0 , cela veut dire que tout le cycle de micro pas a ete ecoule */
        lp_Mot->mot_pas ++ ;
        i_pas_change=1;        

        /* TODO : vois si mettre ailleurs (thread dédié) */
        if ( gi_gpio_max_nb_pas >0 && lp_Mot->mot_pas > gi_gpio_max_nb_pas ) {

          GPIO_TRAP_SIG(0);
        }
      }
      else { 
        lp_Mot->mot_upas++ ; 
      }

      HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Mot->mot_mutex) ;
    }
    else {

      HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Mot->mot_mutex) ;

      if ( lp_Mot->mot_upas == 0 ) { 
        lp_Mot->mot_upas = lp_Mot->mot_nb_upas-1 ;
        /* Si lp_Mot->mot_upas = lp_Mot->mot_nb_upas-1 , cela veut dire que tout le cycle de micro pas a ete ecoule */
        lp_Mot->mot_pas -- ;
        i_pas_change=1;

        /* TODO : vois si mettre ailleurs (thread dédié) */

        if ( gi_gpio_max_nb_pas >0 && lp_Mot->mot_pas > gi_gpio_max_nb_pas ) {
          
          GPIO_TRAP_SIG(0);
        }
      } 
      else { 
        lp_Mot->mot_upas-- ; 
      }

      HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Mot->mot_mutex) ;
    }

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Mot->mot_mutex) ;

    micropas  =  lp_Mot->mot_upas ;
    per_tpwm  =  lp_Mot->mot_per_pwm ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Mot->mot_mutex) ;

    //Trace("%d\t",lp_Mot->mot_upas) ;

    // On recopie les devices de la structure moteur qui vont servir dans 
    // la structure phase (4 phases a priori dans un moteur pas a pas)

    for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {

      HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Mot->mot_pha[i]->pha_mutex ) ;

        lp_Mot->mot_pha[i]->pha_upas    = micropas ;
        lp_Mot->mot_pha[i]->pha_per_pwm    = per_tpwm ;
        lp_Mot->mot_pha[i]->pha_rc      = lp_Mot->mot_pha[i]->pha_rap[ micropas ] ;
        lp_Mot->mot_pha[i]->pha_per_mot = periode_mot ; 
        lp_Mot->mot_pha[i]->pha_per_mic = periode_mic ; 

      HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Mot->mot_pha[i]->pha_mutex ) ;
      
    }

    /*  ----------------------------------------------------------
        gestion des temporisations correspondant aux periodes 
        moteurs : on change de rapport cycliques tous les micropas
        ----------------------------------------------------------*/

    periode_eff = periode_mic  ;

    if (periode_mic >= GPIO_SUIVI_MAIN_ATTENTE_MAX ) {      
      periode_eff = GPIO_SUIVI_MAIN_ATTENTE_MAX  ;
      Trace("depassement periode") ;
    }

    usleep( periode_eff * TIME_MICRO_SEC ) ;
    
    periode_ree = TIME_CALCULS_DUREE_SECONDES( & lp_Mot->mot_tval ) ;

    /* les temps sont mis a jour en considerant que le passage
       dans la boucle while se fait au regard des temps micro pas */

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK(  & lp_Mot->mot_mutex ) ;

    lp_Mot->tps_mot += ( periode_mot ) ;
    lp_Mot->tps_bru += ( periode_bru ) ;
    lp_Mot->tps_mic += ( periode_eff ) ;
    lp_Mot->tps_ree += ( periode_ree ) ;
    
    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK(& lp_Mot->mot_mutex ) ;

    gp_Pid->pid_run( gp_Pid, periode_bru, periode_ree ) ;

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Pid->pid_mutex ) ;

    d_ecart = gp_Pid->pid_err ; /* TODO a mettre dans pid_run */
    
    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Pid->pid_mutex ) ;

    if ( gi_pid_trace>0 && i_pas_change && ( lp_Mot->mot_pas % gi_pid_trace ) == 0 ) {

      /* Le temps usleep est toujours inferieur au temps "reel" consomme */ 
      /* on compense par PID en utilisant l ecart sur les periodes brutes calculees avec le temps reel */
      /* cela ajuste la frequence deduite et la periode deduite pour etre en phase avec teps reel */

      /* Correction du calcul asservissement juin 2022 */

      switch ( lp_Mot->mot_id ) {

        case 0 : 
          // Trace("acc alt %f tps_reel %f tps_mic %f as %lld", lp_Mot->p_sui->pas_acc_alt, lp_Mot->tps_ree, lp_Mot->tps_mic, lp_Mot->mot_pas ) ;
          HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Pid->pid_mutex ) ;

          gp_Pid->pid_acc_alt *= gp_Pid->pid_result ;

          HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Pid->pid_mutex ) ;
          // Trace("acc alt %f tps_reel %f tps_mic %f", lp_Mot->p_sui->pas_acc_alt, lp_Mot->tps_ree, lp_Mot->tps_mic ) ;

          break ;

        case 1 : 
          // Trace("acc azi %f tps_reel %f tps_mic %f pas %lld ", lp_Mot->p_sui->pas_acc_azi, lp_Mot->tps_ree, lp_Mot->tps_mic, lp_Mot->mot_pas ) ;
          HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Pid->pid_mutex ) ;

          gp_Pid->pid_acc_azi *= gp_Pid->pid_result ;
          
          HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Pid->pid_mutex ) ;
          // Trace("acc azi %f tps_reel %f tps_mic %f", lp_Mot->p_sui->pas_acc_azi, lp_Mot->tps_ree, lp_Mot->tps_mic ) ;
          break ;
      }

      /* Fin Correction du calcul asservissement juin 2022 */
    }

    Trace("i_pas_change %d lp_Mot->mot_pas %lld gp_Tpo->tpo_pid_loop %ld gp_Pid_Par->par_pid_ech %f" , \
      i_pas_change, \
      lp_Mot->mot_pas, \
      gp_Tpo->tpo_pid_loop, \
      gp_Pid_Par->par_pid_ech \
    ) ;

    if ( i_pas_change && ( lp_Mot->mot_pas % gp_Tpo->tpo_pid_loop ) == 0 ) {

      switch ( lp_Mot->mot_id ) {

        case 0 : 
          if ( gi_pid_trace_alt ) {
            Trace("mot %-3d => pas %-5lld : tps_bru %f tps_ree %f : per_bru %f per_ree %f ecart %f",\
              lp_Mot->mot_id,       \
              lp_Mot->mot_pas,      \
              lp_Mot->tps_bru,  \
              lp_Mot->tps_ree,  \
              periode_bru,  \
              periode_ree,  \
              d_ecart         \
            ) ;
          }
          break ;

        case 1 : 
          if ( gi_pid_trace_azi ) {
            Trace("mot %-3d => pas %-5lld : tps_bru %f tps_ree %f : per_bru %f per_ree %f ecart %f",\
              lp_Mot->mot_id,       \
              lp_Mot->mot_pas,      \
              lp_Mot->tps_bru,  \
              lp_Mot->tps_ree,  \
              periode_bru,  \
              periode_ree,  \
              d_ecart         \
            ) ;
          }
          break ;
      }
    }

    // FIXME : calcule la duree reelle d une iteration dans la boucle

    //tdiff = TIME_CALCULS_DUREE_MICROSEC( &lp_Mot->mot_tval ) ; 
    i_incr++ ;
  } // FIXME : fin boucle while 

  /* TODO : prevoir modif gp_Pth statut en cas de retour */

  return NULL ;
}
/*****************************************************************************************
* @fn     : GPIO_INIT_PWM_MOTEUR
* @author : s.gravois
* @brief  : Initialise la structure lp_Mot
* @brief  : des signaux sur les phases par modulation PWM 
* @param  : STRUCT_GPIO_PWM_MOTEUR *lp_Mot
* @param  : int[] gpios
* @param  : int[] masque
* @param  : double upas, double fm, double fpwm, int id, int type_fonction, double param0, double param1
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : 
*****************************************************************************************/

void GPIO_INIT_PWM_MOTEUR(STRUCT_GPIO_PWM_MOTEUR *lp_Mot, int gpios[ GPIO_NB_PHASES_PAR_MOTEUR ], int masque[ GPIO_NB_PHASES_PAR_MOTEUR ],  double upas, double fm, double fpwm, int id, int type_fonction, double param0, double param1) {

  int i ;
  char cmd[255] ;
  
  TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Mot->mot_mutex ) ;
  gettimeofday( &                    lp_Mot->mot_tval,NULL) ;

  memset(cmd, CONFIG_ZERO_CHAR ,sizeof(cmd)) ;

  lp_Mot->type_fonction = type_fonction ;
  lp_Mot->param0        = param0 ;
  lp_Mot->param1        = param1 ;
  lp_Mot->mot_id        = id ;
  lp_Mot->mot_fre_pwm   = fpwm ;
  lp_Mot->mot_fm        = fm / 8 ;
  lp_Mot->mot_nb_upas   = upas ;
  lp_Mot->mot_tm        = 1 / lp_Mot->mot_fm ;
  lp_Mot->mot_per_pwm   = 1 / lp_Mot->mot_fre_pwm ;
  lp_Mot->nbdeltat      = (long)(lp_Mot->mot_fm * lp_Mot->mot_nb_upas) ;
  lp_Mot->mot_per_mot   = 1 / lp_Mot->mot_fm ;
  lp_Mot->mot_per_mic   = 1 / ( lp_Mot->mot_fm * lp_Mot->mot_nb_upas ) ;

  lp_Mot->tps_mic       = 0 ; 
  lp_Mot->tps_ree       = 0 ; 
  lp_Mot->tps_bru       = 0 ; 
  lp_Mot->tps_mot       = 0 ;

  lp_Mot->temps_moyen      = 0 ; 
  lp_Mot->temps_reel_moyen = 0 ; 

  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    
    lp_Mot->mot_pha[i]=(STRUCT_GPIO_PWM_PHASE *)malloc(sizeof(STRUCT_GPIO_PWM_PHASE)) ;
    
    HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Mot->mot_pha[i]->pha_mutex ) ;

    /* 2022-11-01 : p_pth > non utilise pour l'instant (preparation portage) */    
    /* les threads sont utilises et initialises via  pthread_t [] dans main */
 /* lp_Mot->mot_pha[i]->pha_p_pth            = lp_Mot->p_pth; */
    
    /* lp_Mot->mot_pha[i]->pha_p_sui            = lp_Mot->p_sui ;*/ 
    lp_Mot->mot_pha[i]->pha_gpio             = gpios[ masque[i] ]  ;
    lp_Mot->mot_pha[i]->pha_id               = i ;
    lp_Mot->mot_pha[i]->pha_per_pwm             = 0 ;
    lp_Mot->mot_pha[i]->pha_upas         = 0 ;
    lp_Mot->mot_pha[i]->pha_gpio_open_statut = 0 ;
    lp_Mot->mot_pha[i]->pha_gpio_fd       = 0 ;
    
    GPIO_OPEN_BROCHE_PWM( lp_Mot->mot_pha[i] ) ;
    /*
    if ( lp_Mot->mot_pha[i]->pha_gpio_open_statut < 0 )  {
      sprintf(cmd,"Trace \"GPIO_OPEN_BROCHE_PWM : ouverture gpio %d = %d\" > /tmp/astrokit.log", i, lp_Mot->mot_pha[i]->pha_gpio_open_statut ) ;
      perror(cmd) ;
      system(cmd) ;
    }*/
    /* Trace1("") ; */
    usleep(100000) ;
  }
  GPIO_CALCULS_PWM_RAPPORTS_CYCLIQUES( lp_Mot )  ;

  return ;
}
/*****************************************************************************************
* @fn     : suivi_clavier
* @author : s.gravois
* @brief  : Callback de phtread generique (utilise par mainG )
* @brief  : des signaux sur les phases par modulation PWM 
* @param  : void
* @date   : 2022-11-30 creation entete au format doxygen
* @todo   : recopier le code de SUIVI_CLAVIER (?) en adaptant
*****************************************************************************************/

void * suivi_clavier(void) {
  Trace("------ %c -------\n", getc(stdin));
  return NULL ;
}
// ##########################################################################################################
// Main10 : Idem main9 mais avec choix du NOMBRE DE MOTEURS (pour test sur raspi b+ = un CPU)
//          ET choix de la fonction de rapport cyclique
// ##########################################################################################################

/*****************************************************************************************
* @fn     : main(Gpio)xxx : main
* @author : s.gravois
* @brief  : point entree du programme GPIO (se concentre sur le test des moteurs)
* @param  : int     argc
* @param  : char ** argv
* @date   : 2022-01-18 creation entete de la fonction au format doxygen
* @date   : 2022-05-24 amelioration globale du code pour pouvoir faire des mesures
* @date   : 2022-05-24  et d invoquer de facon plus simple les arguments
* @fixme  : les autres main ont ete sauvegardes dnas gpio.fonctions.utiles.c
* @todo   : amelioration continue
*****************************************************************************************/

int mainG(int argc, char **argv) {

  char c='0', ch='0' ;
  int nread =0 ; 
  int         i, pid, nbcpus, nbm , type_fonction;
  int         Fpwm , priority , upas ;
  char        c_gpio_alt [ 16 ] ;
  char        c_gpio_azi [ 16 ] ;
  char        c_masque [ 16 ] ;
  int         gpiosM1[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  int         gpiosM2[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  int         masque[  GPIO_NB_PHASES_PAR_MOTEUR ] ;
  float       param0, param1 ;
  float       fr_alt, fr_azi ;
  struct sched_param param ;
  int incr=0 ;
  //cpu_set_t          cpuset ;
  
  pthread_t p_thread_pha_alt[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  pthread_t p_thread_pha_azi[ GPIO_NB_PHASES_PAR_MOTEUR ] ;
  pthread_t p_thread_mot_alt ;
  pthread_t p_thread_mot_azi ;
  pthread_t p_thread_getc ;
  
  STRUCT_GPIO_PWM_MOTEUR *gp_Alt_Mot , m0 ; 
  STRUCT_GPIO_PWM_MOTEUR *gp_Azi_Mot , m1 ;

  Trace("BEGIN GPIO programme") ;  

  /* affectation / dereferencement de divers pointeurs */

  gp_Alt_Mot  = & m0 ;
  gp_Azi_Mot  = & m1 ;
  gp_Sui      = & g_Suivi ;
  gp_Pth      = & g_Pthreads ;
  gp_Mut      = & g_Mutexs ;

  /* Pour permettre acces a STRUCT_PTHREADS* via struct STR_SUIVI* */

  gp_Pth = (STRUCT_PTHREADS*) gp_Pth ; 

  /* Pour permettre acces a STRUCT_SUIVI* via struct STRUCT_GPIO_PWM_MOTEUR* */
  /* obsolete dec 2022 */
  /*
  gp_Azi_Mot->p_sui = (STRUCT_SUIVI*)gp_Sui ;   
  gp_Alt_Mot->p_sui = (STRUCT_SUIVI*)gp_Sui ;   
  */
  signal(SIGINT,GPIO_TRAP_SIG) ;
  signal(SIGALRM,GPIO_TRAP_SIG) ;

  CONFIG_PATH_FIND( gc_config_path_cmd_stty, "stty") ;

  Trace1("Chemin retenu : %s", gc_config_path_cmd_stty) ;

  sem_init( & gpio_sem , 0, 0 ) ;
  clock_gettime(CLOCK_REALTIME, & gpio_tm_now ) ; 

  /* Definition des valeurs par defaut */

  gi_pid_trace = 0 ;
  gi_gpio_timeout = 0 ;
  gi_gpio_max_nb_pas = 0 ;
  gi_gpio_max_nb_upas = 0 ;
  gi_pth_numero = 0 ;

  strcpy( c_gpio_alt , GPIO_DEFAULT_ALT_MOTOR_GPIOS ) ;
  strcpy( c_gpio_azi , GPIO_DEFAULT_AZI_MOTOR_GPIOS ) ;
  strcpy( c_masque ,   GPIO_DEFAULT_MAS_MOTOR_ORDER ) ;

  fr_alt        = 5 ;
  fr_azi        = 5 ;
  upas          = 100 ;
  priority      = 1 ;      
  nbm           = 0 ;
  Fpwm          = 1000 ;
  type_fonction = GPIO_CURRENT_FONCTION ;
  param0        = GPIO_CURRENT_FONCTION_PARAM0 ;
  param1        = GPIO_CURRENT_FONCTION_PARAM1 ;

   if( argc == 2 && (! strcmp(argv[1],"-h"))) { 

    Trace("Usage : %s\n\
      -L log / traces            ( exemple 1 )             - (facultatif, defaut %d) \n\
      -T timeout  (max temps)    ( exemple 1 )             - (facultatif, defaut %d) \n\
      -P pas      (max nb pas)   ( exemple 1000 )          - (facultatif, defaut %d) \n\
      -M micropas (max nb upas)  ( exemple 1000 )          - (facultatif, defaut %d) \n\
      -u nb micro pas            ( exemple 64 )            - (facultatif, defaut %d) \n\
      -g gpios moteur1           ( exemple 26,19,13,6 )    - (facultatif, defaut %s) \n\
      -G gpios moteur2           ( exemple 5,11,9,10 )     - (facultatif, defaut %s) \n\
      -m masque                  ( exemple 3,1,0,2 )       - (facultatif, defaut %s) \n\
      -f fr MOTEUR1 = alt (Hz)   ( exemple 10 )            - (facultatif, defaut %f) \n\
      -F fr MOTEUR2 = azi (Hz)   ( exemple 20 )            - (facultatif, defaut %f) \n\
      -w fr Fpwm                 ( exemple 1000 )          - (facultatif, defaut %d) \n\
      -n nb moteurs a utiliser   ( 0=1 moteur, 1=les deux) - (facultatif, defaut %d) \n\
      -c priority CPU (0-99)     ( exemple 1 )             - (facultatif, defaut %d) \n\
      -x type_fonction           ( exemple 3 = sin applati)- (facultatif, defaut %d) \n\
      -y param0 aplatissement    ( exemple 0.7 )           - (facultatif, defaut %f) \n\
      -z param1 ajustement a 5v  ( exemple 0.99 )          - (facultatif, defaut %f) \n\
    ", \
      argv[0], \
      gi_pid_trace, \
      gi_gpio_timeout, \
      gi_gpio_max_nb_pas, \
      gi_gpio_max_nb_upas, \
      upas, \
      c_gpio_alt, \
      c_gpio_azi, \
      c_masque, \
      fr_alt, \
      fr_azi, \
      Fpwm, \
      priority, \
      nbm, \
      type_fonction, \
      param0, \
      param1 \
    ) ;

    Trace("Commande DEUX moteurs en utilisant une modulation PWM\n") ;
    Trace("Utilisation des logs") ;
    Trace("0 : pas de logs / traces") ;
    Trace("x : affiche des stats (nb pas , temps moyen moteur, temps moyen reel, ..) par multiple de x secondes") ;

    exit(1) ;
  }
  /* GPIO_GETOPT(argc,argv) ; */

  while ((c = getopt (argc, argv, "hg:G:H:m:f:F:u:p:n:P:T:L:x:y:z:")) < 255 && c > 0) {
      incr++ ;
      switch (c) {

        case 'L':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          gi_pid_trace = atoi(optarg) ; 
          break ;  

        case 'T':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          gi_gpio_timeout = atoi(optarg) ; 
          break ;   

        case 'P':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          gi_gpio_max_nb_pas = atoi(optarg) ; 
          break ;   

        case 'M':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          gi_gpio_max_nb_upas = atoi(optarg) ; 
          break ;   
          
        case 'g':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          strcpy( c_gpio_alt , optarg ) ;
          break ;

        case 'G':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          strcpy( c_gpio_azi , optarg ) ;
          break ;       

        case 'm':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          strcpy( c_masque , optarg ) ;
          break ;    

        case 'f':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          fr_alt = atof(optarg) ; 
          break ;    

        case 'F':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          fr_azi = atof(optarg) ; 
          break ;    

        case 'u':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          upas = atoi(optarg) ; 
          break ;    

        case 'p':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          priority = atoi(optarg) ; 
          break ;    
  
        case 'n':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          nbm = atoi(optarg) ; 
          break ;    

        case 'w':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          Fpwm = atoi(optarg) ; 
          break ;    

        case 'x':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          type_fonction = atof(optarg) ; 
          break ;   

        case 'y':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          param0 = atof(optarg) ; 
          break ;   

        case 'z':
          Trace("%d : Prise en compte argument %c %s",incr,c,optarg) ;
          param0 = atof(optarg) ; 
          break ;   

        default :
          Trace("c=%d %c", c,c);
          break ;
      }
  }

  /* Affichage des valeurs retenues */

  Trace("\n\
    traces / log          %d \n\
    timeout               %d \n\
    gpios moteur1         %s \n\
    gpios moteur2         %s \n\
    masque                %s \n\
    fr MOTEUR1 = alt (Hz) %f \n\
    fr MOTEUR2 = azi (Hz) %f \n\
    micro pas             %d \n\
    priority CPU (0-99)   %d \n\
    nb moteurs a utiliser %d \n\
    Fpwm                  %d \n\
    type_fonction         %d \n\
    param0 aplatissement  %f \n\
    param1 ajustement a 5v%f \n\
    \n",\
    gi_pid_trace, \
    gi_gpio_timeout, \
    c_gpio_alt, \
    c_gpio_azi, \
    c_masque, \
    fr_alt, \
    fr_azi, \
    upas, \
    priority, \
    nbm, \
    Fpwm, \
    type_fonction, \
    param0, \
    param1 \
  ) ;
    
  GPIO_TAB_TOKEN( gpiosM1, c_gpio_alt , "," ) ;
  GPIO_TAB_TOKEN( gpiosM2, c_gpio_azi , "," ) ;
  GPIO_TAB_TOKEN( masque,  c_masque , "," ) ;

  for(i=0;i<4;i++) Trace1("gpio %d : %d",i,gpiosM1[i]) ;
  for(i=0;i<4;i++) Trace1("gpio %d : %d",i,gpiosM2[i]) ;
  for(i=0;i<4;i++) Trace1("masq %d : %d",i,masque[i]) ;

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( &gp_Fre->fre_mutex ) ;

  gp_Fre->fre_fa_mot = (double)fr_azi ;
  gp_Fre->fre_fh_mot = (double)fr_alt ;
  gp_Fre->fre_fa_bru = (double)fr_azi ;
  gp_Fre->fre_fh_bru = (double)fr_alt ;
  gp_Fre->fre_fa_mic =     gp_Fre->fre_fa_mot * (double)upas  ;
  gp_Fre->fre_fh_mic =     gp_Fre->fre_fh_mot * (double)upas ;
  gp_Fre->fre_ta_mot = 1 / gp_Fre->fre_fa_mot ;
  gp_Fre->fre_th_mot = 1 / gp_Fre->fre_fh_mot ;
  gp_Fre->fre_ta_bru = 1 / gp_Fre->fre_fa_mot ;
  gp_Fre->fre_th_bru = 1 / gp_Fre->fre_fh_mot ;
  gp_Fre->fre_ta_mic = 1 / gp_Fre->fre_fa_mic ;
  gp_Fre->fre_th_mic = 1 / gp_Fre->fre_fh_mic ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( &gp_Fre->fre_mutex ) ;

  pid=getpid() ;
  nbcpus = sysconf(_SC_NPROCESSORS_ONLN) ;
  
  Trace("gp_Lie_Par->par_altitude : periode mot = %.2f f = %.f ", \
    gp_Fre->fre_ta_mot, \
    gp_Fre->fre_fa_mot ) ; 

  Trace("AZIMUT   : periode mot = %.2f f = %.f ", \
    gp_Fre->fre_th_mot, \
    gp_Fre->fre_fh_mot ) ; 

  Trace("gp_Lie_Par->par_altitude : periode mic = %.2f f = %.f ", \
    gp_Fre->fre_ta_mic, \
    gp_Fre->fre_fa_mic ) ; 

  Trace("AZIMUT   : periode mic = %.2f f = %.f ", \
    gp_Fre->fre_th_mic, \
    gp_Fre->fre_fh_mic ) ; 
/*
  Trace("correct(y/n)?") ;
  nread = read( 0, &ch, 1)  ;
  Trace("ch = %c nread = %d", ch, nread) ;
*/
  Trace("pid processus = %d", pid) ; 
  Trace("nb cppus      = %d", nbcpus ) ; 

  if ( gi_gpio_timeout > 0) {
    alarm( gi_gpio_timeout ) ;
  }

  if ( priority > 0)  {
    mlockall(MCL_CURRENT | MCL_FUTURE);
    Trace("mise en place temps reel : echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ;
    system("echo -1 >/proc/sys/kernel/sched_rt_runtime_us") ;
    param.sched_priority = priority ;
    Trace("mise en place temps reel : param.sched_priority = %d", priority) ;
    if ( sched_setscheduler( pid, SCHED_RR, & param) != 0) { perror("setschedparam"); exit(EXIT_FAILURE);  }
    else Trace("modification du processus avec priorite = %d\n", priority) ;
  } 
 
  GPIO_INIT_PWM_MOTEUR(\
    gp_Alt_Mot, \
    gpiosM1, \
    masque, \
    upas, \
    fr_alt, \
    Fpwm, \
    0, \
    type_fonction, \
    param0, \
    param1) ;

  if(nbm) {
    GPIO_INIT_PWM_MOTEUR(\
    gp_Azi_Mot, \
    gpiosM2, \
    masque, \
    upas, \
    fr_azi, \
    Fpwm, \
    1, \
    type_fonction, \
    param0, \
    param1) ;
  }
  
  // lancement des pthreads moteurs ----------------------------------------------------
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_create( &p_thread_pha_alt[i], NULL,(void*)_GPIO_PWM_PHASE, gp_Alt_Mot->mot_pha[i] ) ;
  }
  pthread_create( &p_thread_mot_alt, NULL, (void*)_GPIO_PWM_MOT, gp_Alt_Mot ) ;
  
  if(nbm) {
    for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
      pthread_create( &p_thread_pha_azi[i], NULL,(void*)_GPIO_PWM_PHASE, gp_Azi_Mot->mot_pha[i] ) ;
    }
    pthread_create( &p_thread_mot_azi, NULL, (void*)_GPIO_PWM_MOT, gp_Azi_Mot ) ;
  }

  // lancement des autres pthreads  ----------------------------------------------------

  pthread_create( &p_thread_getc, NULL, (void*)suivi_clavier, NULL ) ;

  PTHREADS_DISPLAY_ETAT(gp_Pth);

  // Join des threads -------------------------------------------------------------------
  
  for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
    pthread_join( p_thread_pha_alt[i], NULL) ; 
  }
  pthread_join( p_thread_mot_alt, NULL) ;

  if(nbm) {  
   for( i=0;i<GPIO_NB_PHASES_PAR_MOTEUR;i++ ) {
     pthread_join( p_thread_pha_azi[i], NULL) ; 
   }
   pthread_join( p_thread_mot_azi, NULL) ;
  }
  pthread_join( p_thread_getc, NULL) ;

  return 0 ;
}
/* -----------------------------------------------------------------------------------------

   FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN

   -----------------------------------------------------------------------------------------*/

