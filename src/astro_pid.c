/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date           | commentaires 
# ######################################################################
# 18 juin 2022   * creation
# -------------------------------------------------------------- 
*/

#include "astro_pid.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

int  gi_pid_trace ;
int  gi_pid_trace_alt ;
int  gi_pid_trace_azi ;


/*****************************************************************************************
* @fn     : PID_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_PID *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void PID_LOCK ( STRUCT_PID * lp_Pid) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Pid->pid_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : PID_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_PID *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void PID_UNLOCK ( STRUCT_PID * lp_Pid) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Pid->pid_mutex ) ;

  return ;
}

/*****************************************************************************************
* @fn     : PID_PARAMS_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure STRUCT_PID_PARAMS *
* @param  : STRUCT_PID_PARAMS * lp_Pid_Par
* @date   : 2022-12-12 creation
*****************************************************************************************/

void PID_PARAMS_INIT(STRUCT_PID_PARAMS * lp_Pid_Par ) {

  char c_path_file_out[ CONFIG_TAILLE_BUFFER_256] ; 

  TraceArbo(__func__,1,"init pid arams") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Pid_Par->pid_par_mutex ) ;

  lp_Pid_Par->par_pid_ech = 0 ;
  lp_Pid_Par->par_pid_kd = 0 ;
  lp_Pid_Par->par_pid_ki = 0 ;
  lp_Pid_Par->par_pid_kp = 0 ;

  return ;
}
/*****************************************************************************************
* @fn     : PID_PARAMS_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche la structure STRUCT_PID_PARAMS *
* @param  : STRUCT_PID_PARAMS * lp_Pid_Par
* @date   : 2022-12-12 creation
*****************************************************************************************/

void PID_PARAMS_DISPLAY(STRUCT_PID_PARAMS * lp_Pid_Par ) {

  char c_path_file_out[ CONFIG_TAILLE_BUFFER_256] ; 

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  TraceLogLevel(gp_Log->log_level,1,"%-50s = %f", "lp_Pid_Par->par_pid_ech" , lp_Pid_Par->par_pid_ech);
  TraceLogLevel(gp_Log->log_level,1,"%-50s = %f", "lp_Pid_Par->par_pid_ki " , lp_Pid_Par->par_pid_ki);
  TraceLogLevel(gp_Log->log_level,1,"%-50s = %f", "lp_Pid_Par->par_pid_kp " , lp_Pid_Par->par_pid_kp);
  TraceLogLevel(gp_Log->log_level,1,"%-50s = %f", "lp_Pid_Par->par_pid_kd " , lp_Pid_Par->par_pid_kd);

  return ;
}
/*****************************************************************************************
* @fn     : PID_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure lcd *l
* @param  : STRUCT_PID *lp_Pid
* @date   : 2022-06 creation
*****************************************************************************************/

void PID_INIT(STRUCT_PID * lp_Pid ) {

  char c_path_file_out[ CONFIG_TAILLE_BUFFER_256] ; 

  TraceArbo(__func__,0,"init pid") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  /* Initialisation fichier */

  memset(c_path_file_out, 0, sizeof(c_path_file_out)) ;

  sprintf( c_path_file_out, "%s/%s" , \
    gp_Con_Par->con_par_rep_log, \
    gp_Con_Par->con_par_fic_pid) ;

  if ( ( lp_Pid->pid_file = fopen( c_path_file_out, "w" ) ) == NULL ) {
    Trace("ouverture %s (KO)",c_path_file_out );
  }
  else {
    Trace("ouverture %s (OK)",c_path_file_out );
  }
  /* Initialisation mutex */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Pid->pid_mutex ) ;
  
  /* Parametres de reglages du PID proportionnel integral derive*/

  lp_Pid->pid_ech = gp_Pid_Par->par_pid_ech ;
  lp_Pid->pid_kp  = gp_Pid_Par->par_pid_kp ;
  lp_Pid->pid_ki  = gp_Pid_Par->par_pid_ki ;
  lp_Pid->pid_kd  = gp_Pid_Par->par_pid_kd ;

  /* Initialisation autres champs */

  lp_Pid->pid_long_incr      = 0 ;
  lp_Pid->pid_input_consigne = 0 ;  /* consigne */ 
  lp_Pid->pid_output         = 0;   /* sortie */ 
  lp_Pid->pid_err            = 0;   /* erreur  = out - inp */ 
  lp_Pid->pid_err_sum        = 0 ; /* calcul de la somme des erreurs */ 
  lp_Pid->pid_err_ki         = 0 ; /* calcul de la partie integrale */  
  lp_Pid->pid_err_kd         = 0 ; /* calcul de la partie derivee */
  lp_Pid->pid_err_pre        = 0 ;
  lp_Pid->pid_acc_azi        = 1.0 ; // acceleration PID par retour boucle des vitesses brutes
  lp_Pid->pid_acc_alt        = 1.0 ; // acceleration PID par retour boucle des vitesses brutes

  /* Initialisation pointeurs de fonctions */
  
  /* lp_Pid->pid_init */ 
  lp_Pid->pid_reset = PID_RESET ;
  lp_Pid->pid_run   = PID_RUN ;
  lp_Pid->pid_test  = PID_TEST ;

  return ;
}


/*****************************************************************************************
* @fn     : PID_RUN
* @author : s.gravois
* @brief  : Cette fonction calcule a partir de Ki, Kp, Kd, Ech, et de E(t) depuis 0->t
* @brief  : les valeurs des termes proportionnel , integral, et derivee
* @param  : double input  (consigne)
* @param  : double output
* @date   : 2022-06 creation
*****************************************************************************************/

void   PID_RUN (double d_input, double d_output) {

  double d_err_delta ; 

/* ancien code */
/*
if ( periode_bru != 0 ) {
  d_ecart = periode_ree / periode_bru ; 
  d_pid   = ( (d_ecart-1.0) / 2.0 )+ 1.0 ;
}
else {
  Trace("division par zero") ;
}
*/
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Pid->pid_mutex ) ;

  gp_Pid->pid_err  = d_output - d_input ;

  /* calcul terme proportionnel */

  gp_Pid->pid_err_kp = gp_Pid->pid_kp * gp_Pid->pid_err ;

  /* Si on a atteint au moins la deuxieme boucle */
  /* on calcule tout de qui est relatif a ID de PID : 
    * integral 
    * derivate 
  */

  if ( gp_Pid->pid_long_incr > 0 ) {

    /* calcul terme derivee */

    /* la division par deltat est ponderee par le parametre Kd */
    /* Etant donne que l'echantillonage est constant */

    d_err_delta  = gp_Pid->pid_err - gp_Pid->pid_err_pre ;
    gp_Pid->pid_err_kd = gp_Pid->pid_kd * d_err_delta ;

    /* calcul terme integral */

    /* Le calcul du terme INTEGRAL est la somme des rectangles depuis t=0 */
    /* TODO : methofde alternative methode des trapezes */
    /* la multiplication par deltat est ponderee par le parametre Ki */
    /* Etant donne que l'echantillonage est constant */

    gp_Pid->pid_err_sum   += gp_Pid->pid_err ;
    gp_Pid->pid_err_ki     = gp_Pid->pid_ki * gp_Pid->pid_err_sum ;
  }

  /* calcul complet comande PID et mise a edchelle pour consigne et output */

  gp_Pid->pid_result = gp_Pid->pid_err_kp + gp_Pid->pid_err_ki + gp_Pid->pid_err_kd ;

  /* mise en conformite pour COmmande Sortie */
  /* TODO : voir si une addition avec 1 suffit */
  
  gp_Pid->pid_result   = ( (gp_Pid->pid_result-1.0) / 2.0 )+ 1.0 ;

  /* sauvegarde de erreur dans erreur precedente pour calcul suivant */

  gp_Pid->pid_err_pre = gp_Pid->pid_err ;
  gp_Pid->pid_long_incr++ ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Pid->pid_mutex ) ;

  return ;
}


/*****************************************************************************************
* @fn     : PID_RESET
* @author : s.gravois
* @brief  : Cette fonction reset le calcul de la algorithme PID 
* @param  : void
* @date   : 2022-06    creation
* @date   : 2022-12-02 
*****************************************************************************************/

void PID_RESET (void) {

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & gp_Pid->pid_mutex ) ;

  gp_Pid->pid_long_incr      = 0 ;
  gp_Pid->pid_input_consigne = 0 ;  /* consigne */ 
  gp_Pid->pid_output         = 0;   /* sortie */ 
  gp_Pid->pid_err            = 0;   /* erreur  = out - inp */ 
  gp_Pid->pid_err_sum        = 0 ; /* calcul de la somme des erreurs */ 
  gp_Pid->pid_err_ki         = 0 ; /* calcul de la partie integrale */  
  gp_Pid->pid_err_kd         = 0 ; /* calcul de la partie derivee */
  gp_Pid->pid_err_pre        = 0 ;
  gp_Pid->pid_acc_azi        = 1.0 ; // acceleration PID par retour boucle des vitesses brutes
  gp_Pid->pid_acc_alt        = 1.0 ; // acceleration PID par retour boucle des vitesses brutes

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & gp_Pid->pid_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : PID_TEST
* @author : s.gravois
* @brief  : Cette fonction test la algorithme PID 
* @param  : void
* @date   : 2022-06 creation
*****************************************************************************************/

void   PID_TEST         ( void) {

  return ;
}
/*****************************************************************************************
* @fn     : PID_ACTIVATE
* @author : s.gravois
* @brief  : Cette fonction calcule 
* @param  : void
* @date   : 2022-06 creation
*****************************************************************************************/

void   PID_ACTIVATE         ( void) {

  return ;
}
/*****************************************************************************************
* @fn     : PID_IS_IN_CONDITION
* @author : s.gravois
* @brief  : Cette fonction determine si on peut lancer l algorithme PID 
* @param  : void
* @date   : 2022-06 creation
*****************************************************************************************/

int    PID_IS_IN_CONDITION  ( void) {

  int i_is_in_condition = 0 ;

  return i_is_in_condition ;
}
/* ----------------------------------------------------------------------

FIN FICHIER - FIN FICHIER  FIN FICHIER  FIN FICHIER  FIN FICHIER 

------------------------------------------------------------------------*/