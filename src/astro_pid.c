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
* @fn     : PID_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure lcd *l
* @param  : STRUCT_PID *gp_Pid
* @date   : 2022-06 creation
*****************************************************************************************/

void PID_INIT(STRUCT_PID * p_pid, double d_ech, double d_kp, double d_ki, double d_kd ) {

  char c_path_file_out[ CONFIG_TAILLE_BUFFER_64] ; 

  Trace1("") ;

  /* Initialisation fichier */

  memset(c_path_file_out, 0, sizeof(c_path_file_out)) ;
  sprintf( c_path_file_out, "%s/%s" , gp_Con_Par->par_rep_log, gp_Con_Par->par_fic_pid) ;

  if ( ( p_pid->f_out = fopen( c_path_file_out, "w" ) ) == NULL ) {
    Trace("ouverture %s (KO)",c_path_file_out );
  }
  else {
    Trace("ouverture %s (OK)",c_path_file_out );
  }
  /* Initialisation mutex */

  pthread_mutex_init( & p_pid->mut_pid, NULL ) ;
  
  /* Parametres de reglages du PID proportionnel integral derive*/

  p_pid->ech = d_ech ;
  p_pid->Kp  = d_kp ;
  p_pid->Ki  = d_ki ;
  p_pid->Kd  = d_kd ;

  /* Initialisation autres champs */

  p_pid->incr = 0 ;
  p_pid->inp = 0 ;  /* consigne */ 
  p_pid->out = 0;   /* sortie */ 
  p_pid->err = 0;   /* erreur  = out - inp */ 

  p_pid->err_sum = 0 ; /* calcul de la somme des erreurs */ 
  p_pid->err_int_Ki = 0 ; /* calcul de la partie integrale */  
  p_pid->err_der_Kd = 0 ; /* calcul de la partie derivee */
  p_pid->err_pre = 0 ;
  
  /* Reglages des coefficients Kp, Ki, Kd */

  /* Initialisation pointeurs de fonctions */
  
  /* p_pid->pid_init */ 
  p_pid->pid_reset = PID_RESET ;
  p_pid->pid_run   = PID_CALCULATE ;
  p_pid->pid_test  = PID_TEST ;

  return ;
}


/*****************************************************************************************
* @fn     : PID_CALCULATE
* @author : s.gravois
* @brief  : Cette fonction calcule a partir de Ki, Kp, Kd, Ech, et de E(t) depuis 0->t
* @brief  : les valeurs des termes proportionnel , integral, et derivee
* @param  : double input  (consigne)
* @param  : double output
* @date   : 2022-06 creation
*****************************************************************************************/

void   PID_CALCULATE (double d_inp, double d_out) {

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
  pthread_mutex_lock( & gp_Pid->mut_pid ) ;

  gp_Pid->err  = d_out - d_inp ;

  /* calcul terme proportionnel */

  gp_Pid->err_pro_Kp = gp_Pid->Kp * gp_Pid->err ;

  /* Si on a atteint au moins la deuxieme boucle */
  /* on calcule tout de qui est relatif a ID de PID : 
    * integral 
    * proportionnel 
  */

  if ( gp_Pid->incr > 0 ) {

    /* calcul terme derivee */
    /* la division par deltat est ponderee par le parametre Kd */
    /* Etant donne que l'echantillonage est constant */

    d_err_delta  = gp_Pid->err - gp_Pid->err_pre ;
    gp_Pid->err_der_Kd = gp_Pid->Kd * d_err_delta ;

    /* calcul terme integral */
    /* Le calcul du terme INTEGRAL est la somme des rectangles depuis t=0 */
    /* TODO : methofde alternative methode des trapezes */
    /* la multiplication par deltat est ponderee par le parametre Ki */
    /* Etant donne que l'echantillonage est constant */

    gp_Pid->err_sum   += gp_Pid->err ;
    gp_Pid->err_int_Ki = gp_Pid->Ki * gp_Pid->err_sum ;
  }

  /* calcul complet comande PID et mise a edchelle pour consigne et output */

  gp_Pid->pid = gp_Pid->err_pro_Kp + gp_Pid->err_int_Ki + gp_Pid->err_der_Kd ;

  /* mise en conformite pour COmmande Sortie */
  /* TODO : voir si une addition avec 1 suffit */
  
  gp_Pid->pid   = ( (gp_Pid->pid-1.0) / 2.0 )+ 1.0 ;

  /* sauvegarde de erreur dans erreur precedente pour calcul suivant */

  gp_Pid->err_pre = gp_Pid->err ;
  gp_Pid->incr++ ;

  pthread_mutex_unlock( & gp_Pid->mut_pid ) ;

  return ;
}


/*****************************************************************************************
* @fn     : PID_RESET
* @author : s.gravois
* @brief  : Cette fonction reset le calcul de la algorithme PID 
* @param  : void
* @date   : 2022-06 creation
*****************************************************************************************/

void   PID_RESET         ( void) {

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