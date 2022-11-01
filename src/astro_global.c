/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
--------------------------------------------------------------
date           | commentaires 
--------------------------------------------------------------
octobre 2022 :  * creation            
-------------------------------------------------------------- */

#include "astro_global.h"

STRUCT_ANGLE            g_Angle,            *gp_Ang ;
STRUCT_ASTRE            g_Astre,            *gp_Ast ;
STRUCT_CAT              g_Catalogue,        *gp_Cat ;
STRUCT_CODES            g_Codes,            *gp_Cod ;
STRUCT_DATAS            g_Datas,            *gp_Dat ;
STRUCT_DEVICES          g_Devices,          *gp_Dev ;
STRUCT_I2C_DEVICE       g_I2cDev,           *gp_I2c_Dev ;
STRUCT_I2C_ACC_MAG      g_I2cAcc,           *gp_I2c_Acc ;
STRUCT_I2C_MCP23017     g_I2cMcp,           *gp_I2c_Mcp ;
STRUCT_KEYS             g_Keys,             *gp_Key ;
STRUCT_LCD              g_Lcd,              *gp_Lcd ;
STRUCT_LIEU             g_Lieu,             *gp_Lie ;
STRUCT_PID              g_Pid,              *gp_Pid ;
STRUCT_MUTEXS           g_Mutexs,           *gp_Mut ;
STRUCT_PTHREADS         g_Pthreads,         *gp_Pth ;
STRUCT_SUIVI            g_Suivi,            *gp_Sui ;
STRUCT_TIME             g_Time,             *gp_Tim ;
STRUCT_VOUTE            g_Voute,            *gp_Vou ;
STRUCT_GPIO_PWM_MOTEUR  g_Mot_Alt,          *gp_Mot_Alt ; 
STRUCT_GPIO_PWM_MOTEUR  g_Mot_Azi,          *gp_Mot_Azi ;
STRUCT_ASTRE_PARAMS     g_Astre_Params,     *gp_Ast_Par ;
STRUCT_CALCULS_PARAMS   g_Calculs_Params,   *gp_Cal_Par ;
STRUCT_CONFIG_PARAMS    g_Config_Params,    *gp_Con_Par ;
STRUCT_DEVICES_PARAMS   g_Devices_Params,   *gp_Dev_Par ; 
STRUCT_LIEU_PARAMS      g_Lieu_Params,      *gp_Lie_Par ;
STRUCT_PID_PARAMS       g_Pid_Params,       *gp_Pid_Par ;
STRUCT_TIME_PARAMS      g_Time_Params,      *gp_Tim_Par ;
STRUCT_GPIO_PARAMS_PWM  g_Gpio_Params_Pwm,  *gp_Gpi_Par_Pwm ;
STRUCT_GPIO_PARAMS_MAT  g_Gpio_Params_Mat,  *gp_Gpi_Par_Mat ;
STRUCT_GPIO_PARAMS_RAQ  g_Gpio_Params_Raq,  *gp_Gpi_Par_Raq ;
STRUCT_GPIO_PARAMS_CON  g_Gpio_Params_Con,  *gp_Gpi_Par_Con ;

/*****************************************************************************************
* @fn     : ASTRO_GLOBAL_INIT
* @author : s.gravois
* @brief  : Initialise les variables globales
* @param  : 
* @date   : 2022-10-29
* @todo   : 
*****************************************************************************************/

void ASTRO_GLOBAL_INIT(void) {
  
  gl_number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);

  Trace("%ld processors dispo", gl_number_of_processors ) ;

  g_i_trace = 0 ;
  g_i_trace_alt = 0 ;
  g_i_trace_azi = 0 ;
  g_i_timeout = 0 ;
  g_i_max_nb_pas = 0 ;
  g_i_max_nb_micropas = 0 ;
  g_incrlog = 0 ;
  g_nb_threads = 0 ;
  
  // -----------------------------------------------------------------
  // Initialisations des structures et pointeurs sur structure
  // -----------------------------------------------------------------

  gp_Ast     = & g_Astre ;
  gp_Cat     = & g_Catalogue ;
  gp_Cod     = & g_Codes ;
  gp_Dat     = & g_Datas ;
  gp_Dev     = & g_Devices ;
  gp_I2c_Acc = & g_I2cAcc ;
  gp_I2c_Dev = & g_I2cDev ;
  gp_I2c_Mcp = & g_I2cMcp ;
  gp_Key     = & g_Keys ;
  gp_Lcd     = & g_Lcd ;
  gp_Lie     = & g_Lieu ;
  gp_Pid     = & g_Pid ;     
  gp_Pth     = & g_Pthreads ;
  gp_Mut     = & g_Mutexs ;
  gp_Vou     = & g_Voute ;
  gp_Tim     = & g_Time ;
  gp_Sui     = & g_Suivi ; 

  gp_Mot_Azi = & g_Mot_Azi ;
  gp_Mot_Alt = & g_Mot_Alt ;
 
  gp_Ast_Par     = & g_Astre_Params ;
  gp_Cal_Par     = & g_Calculs_Params ;
  gp_Con_Par     = & g_Config_Params ;
  gp_Dev_Par     = & g_Devices_Params ; 
  gp_Lie_Par     = & g_Lieu_Params ;
  gp_Pid_Par     = & g_Pid_Params ;
  gp_Tim_Pa      = & g_Time_Params ;
  
  gp_Gpi_Par_Pwm = & g_Gpio_Params_Pwm ;
  gp_Gpi_Par_Raq = & g_Gpio_Params_Raq ;
  gp_Gpi_Par_Con = & g_Gpio_Params_Con ;
  gp_Gpi_Par_Mat = & g_Gpio_Params_Mat ;

  // -----------------------------------------------------------------
  // Initialisations des pointeurs d acces dans les structures
  // -----------------------------------------------------------------

  /* Pour permettre acces a STRUCT_PTHREADS* via struct STR_SUIVI* */
  /* TODO : FIXME : what ? */
  /* gp_Sui-> = (STRUCT_PTHREADS*) gp_Pth ; */

  /* Pour permettre acces a STRUCT_SUIVI* via struct STRUCT_GPIO_PWM_MOTEUR* */

  gp_Mot_Azi->p_sui = (STRUCT_SUIVI*)gp_Sui ;   
  gp_Mot_Alt->p_sui = (STRUCT_SUIVI*)gp_Sui ;   
  
  /* Pour permettre acces a STRUCT_SUIVI* via struct STRUCT_GPIO_PWM_MOTEUR* */

  gp_Mot_Alt->p_pth = (STRUCT_PTHREADS*)gp_Pth ;
  gp_Mot_Azi->p_pth = (STRUCT_PTHREADS*)gp_Pth ; 

}
