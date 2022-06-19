#define _GNU_SOURCE

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date           | commentaires 
# ######################################################################
# 18 juin 2022   * creation
# -------------------------------------------------------------- 
*/

#include <pid.h>

/*****************************************************************************************
* @fn     : PID_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure lcd *l
* @param  : PID *gp_Pid
* @date   : 2022-06 creation
*****************************************************************************************/

void PID_INIT(PID * p_pid) {

  Trace1("") ;

  /* Initialisation mutex */

  pthread_mutex_init( & p_pid->mutex_pid, NULL ) ;
  
  /* Initialisation pointeurs de fonctions */
/*
  p_pid->display_default = LCD_DISPLAY_DEFAULT ; */
  

  /* Initialisation autres champs */

  /* Reglages des coefficients Kp, Ki, Kd */

  return ;
}

/*****************************************************************************************
* @fn     : PID_RUN
* @author : s.gravois
* @brief  : Cette fonction calcule 
* @param  : void
* @date   : 2022-06 creation
*****************************************************************************************/


/* ----------------------------------------------------------------------

FIN FICHIER - FIN FICHIER  FIN FICHIER  FIN FICHIER  FIN FICHIER 

------------------------------------------------------------------------*/