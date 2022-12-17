/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
--------------------------------------------------------------
date           | commentaires 
--------------------------------------------------------------
octobre 2022 :
  * recuperation du contenu des fonctions depuis config.c               
-------------------------------------------------------------- */

#include "astro_i2c.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

// #define MUTEX_GLO_I2C_DEV_LOCK        pthread_mutex_lock(   & gp_I2c->i2c_dev_mutex ) ;
// #define MUTEX_GLO_I2C_DEV_UNLOCK      pthread_mutex_unlock( & gp_I2c->i2c_dev_mutex ) ;
// #define MUTEX_GLO_I2C_ACC_LOCK        pthread_mutex_lock(   & gp_Acc->acc_mutex ) ;
// #define MUTEX_GLO_I2C_ACC_UNLOCK      pthread_mutex_unlock( & gp_Acc->acc_mutex ) ;

unsigned short sym[16] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15} ;

/*
STRUCT_I2C_DEVICE  lsm303d,   *lsm ;
STRUCT_I2C_DEVICE  mcp23008,  *mcp ;
STRUCT_I2C_DEVICE  rtcds1307, *rtc ;
*/
STRUCT_I2C_DEVICE  exemple,   *lp_I2c ;
STRUCT_I2C_ACC_MAG accmag,    *lp_Acc ;

void I2C_CALCULS_ACCMAG(STRUCT_I2C_ACC_MAG *lp_Acc) {
  
  float norme_acc, norme_mag  ;
  // float phi, psi , the ;
  
  //---------------------------------------------
  // Normalisation des mesures de l'accelerometre
  //---------------------------------------------
  
  lp_Acc->acc_acc_x = (float)((int16_t)((lp_Acc->acc_axh << 8 ) | lp_Acc->acc_axl ))  ;
  lp_Acc->acc_acc_y = (float)((int16_t)((lp_Acc->acc_ayh << 8 ) | lp_Acc->acc_ayl ))  ;
  lp_Acc->acc_acc_z = (float)((int16_t)((lp_Acc->acc_azh << 8 ) | lp_Acc->acc_azl ))  ;
  
  norme_acc = sqrt( \
      (lp_Acc->acc_acc_x*lp_Acc->acc_acc_x) \
    + (lp_Acc->acc_acc_y*lp_Acc->acc_acc_y) \
    + (lp_Acc->acc_acc_z*lp_Acc->acc_acc_z) ) ;
  
  if ( norme_acc > lp_Acc->acc_acc_norme_max ) {
     lp_Acc->acc_acc_norme_max = norme_acc ; 
  }
  lp_Acc->acc_acc_norm_x = lp_Acc->acc_acc_x / norme_acc ;
  lp_Acc->acc_acc_norm_y = lp_Acc->acc_acc_y / norme_acc ;
  lp_Acc->acc_acc_norm_z = lp_Acc->acc_acc_z / norme_acc ;
  
  //---------------------------------------------
  // Normalisation des mesures de la boussole
  //---------------------------------------------
  
  lp_Acc->acc_mag_x = (float)((int16_t)((lp_Acc->acc_mxh << 8 ) | lp_Acc->acc_mxl ))  ;
  lp_Acc->acc_mag_y = (float)((int16_t)((lp_Acc->acc_myh << 8 ) | lp_Acc->acc_myl ))  ;
  lp_Acc->acc_mag_z = (float)((int16_t)((lp_Acc->acc_mzh << 8 ) | lp_Acc->acc_mzl ))  ;
  
  norme_mag = sqrt( \
     (lp_Acc->acc_mag_x*lp_Acc->acc_mag_x) \
   + (lp_Acc->acc_mag_y*lp_Acc->acc_mag_y) \
   + (lp_Acc->acc_mag_z*lp_Acc->acc_mag_z) ) ;
  
  if ( norme_mag > lp_Acc->acc_mag_norme_max ) {
    lp_Acc->acc_mag_norme_max = norme_mag ; 
  }
  lp_Acc->acc_mag_norm_x = lp_Acc->acc_mag_x / norme_mag ;
  lp_Acc->acc_mag_norm_y = lp_Acc->acc_mag_y / norme_mag ;
  lp_Acc->acc_mag_norm_z = lp_Acc->acc_mag_z / norme_mag ;
  
  //---------------------------------------------
  // Calcul pitch and roll
  //---------------------------------------------
  
  lp_Acc->acc_pitch = asin (-1.0 * lp_Acc->acc_acc_norm_x ) ;
  lp_Acc->acc_roll  = asin ( lp_Acc->acc_acc_norm_y / cos ( lp_Acc->acc_pitch) ) ;
  
  //---------------------------------------------
  // Calcul heading
  //---------------------------------------------
  
  // phi = atan2f(lp_Acc->acc_mag_x,lp_Acc->acc_mag_y) * 360 / ( 2 * M_PI ) ;  
  // psi = atan2f(lp_Acc->acc_mag_y,lp_Acc->acc_mag_z) * 360 / ( 2 * M_PI ) ;   
  // the = atan2f(lp_Acc->acc_mag_z,lp_Acc->acc_mag_x) * 360 / ( 2 * M_PI )  ;  
  
  lp_Acc->acc_gauss_field_x = lp_Acc->acc_mag_norm_x * cos ( lp_Acc->acc_pitch ) + lp_Acc->acc_mag_norm_z * sin (lp_Acc->acc_pitch ) ;
  lp_Acc->acc_gauss_field_y = lp_Acc->acc_mag_norm_x * sin(lp_Acc->acc_roll) * sin(lp_Acc->acc_pitch) + lp_Acc->acc_mag_norm_y - lp_Acc->acc_mag_norm_z * sin(lp_Acc->acc_roll)*cos( lp_Acc->acc_pitch) ;
  
  if ( lp_Acc->acc_gauss_field_x > 0 && lp_Acc->acc_gauss_field_y >= 0 ) lp_Acc->acc_heading = atan( lp_Acc->acc_gauss_field_y / lp_Acc->acc_gauss_field_x ) ;
  if ( lp_Acc->acc_gauss_field_x > 0 && lp_Acc->acc_gauss_field_y < 0 )  lp_Acc->acc_heading = 2 * M_PI + atan( lp_Acc->acc_gauss_field_y / lp_Acc->acc_gauss_field_x ) ;
  if ( lp_Acc->acc_gauss_field_x < 0 )                lp_Acc->acc_heading = M_PI + atan( lp_Acc->acc_gauss_field_y / lp_Acc->acc_gauss_field_x ) ;
  if ( lp_Acc->acc_gauss_field_x == 0 && lp_Acc->acc_gauss_field_y < 0 ) lp_Acc->acc_heading = M_PI / 2.0 ;
  if ( lp_Acc->acc_gauss_field_x == 0 && lp_Acc->acc_gauss_field_y > 0 ) lp_Acc->acc_heading = M_PI * 1.5  ;
  
  // printf("%.0f\t%.0f\t%.0f\t%.0f\n", lp_Acc->acc_acc_x , lp_Acc->acc_acc_y , lp_Acc->acc_acc_z, norme_acc) ;
  // printf("%.0f\t%.0f\t%.0f\t%.0f\n", lp_Acc->acc_mag_x , lp_Acc->acc_mag_y , lp_Acc->acc_mag_z, norme_mag) ;
  // printf("%.2f\t%.2f\t%.2f\t%.2f\n", lp_Acc->acc_mag_norm_x , lp_Acc->acc_mag_norm_y , lp_Acc->acc_mag_norm_z, sqrt( (lp_Acc->acc_mag_norm_x * lp_Acc->acc_mag_norm_x) + (lp_Acc->acc_mag_norm_y * lp_Acc->acc_mag_norm_y) + ( lp_Acc->acc_mag_norm_z * lp_Acc->acc_mag_norm_z) ) ) ;
  
  //printf("%.0f\t%.0f\t%.0f\n", lp_Acc->acc_pitch * I2C_DEGRAD, lp_Acc->acc_roll * I2C_DEGRAD, lp_Acc->acc_heading * I2C_DEGRAD) ;
  //printf("%.0f\t%.0f\t%.0f\n", phi, psi, the) ;
  //printf("%.0f\t%.0f\t%.0f\n",lp_Acc->acc_mag_x * 90 / 16384 , lp_Acc->acc_mag_y * 90 / 16384, lp_Acc->acc_mag_z * 90 / 16384 );
  //printf("---------\n") ;
}
//===========================================================
int I2C_INIT( STRUCT_I2C_DEVICE * lp_i2c_dev, char * c_i2c_device_name, char * adress) {
   
  int ret ;
  ret =0 ;
  
  TraceArbo(__func__,1,"init i2c") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT(&lp_i2c_dev->i2c_dev_mutex) ;
  
  if ( ( lp_i2c_dev->i2c_dev_fd = open(c_i2c_device_name, O_RDWR)) < 0)  ret = -1 ;
  else if ( ioctl( lp_i2c_dev->i2c_dev_fd, I2C_SLAVE, strtoul(adress,NULL,16) ) < 0) ret = -2 ;
  else lp_i2c_dev->i2c_dev_adress = strtoul(adress,NULL,16) ;
  
  lp_i2c_dev->i2c_dev_statut = ret ;  
  lp_i2c_dev->i2c_dev_usleep  = I2C_SLEEP_MICRO ;
  
  Trace1("I2C_INIT : %ld : statut : %d\n", strtoul(adress,NULL,16) , ret);
  printf("I2C_INIT : fd = %d adress = %d statut : %d\n", lp_i2c_dev->i2c_dev_fd , lp_i2c_dev->i2c_dev_adress,  lp_i2c_dev->i2c_dev_statut);
  
  return ret ;
}
//===========================================================
int I2C_SET_3( STRUCT_I2C_DEVICE * lp_i2c_dev, unsigned long output) {

  int ret ;
  unsigned long output1 ;
  unsigned long output2 ;
  
  output1 = (output / 16);
  output2 = (output % 16) << 4;
  //output1 =0 ;
  //output2 =0;
  usleep( lp_i2c_dev->i2c_dev_usleep ) ;
  ret=0 ;
  memset( lp_i2c_dev->i2c_dev_buf, 0 , I2C_BUFFER_SIZE ) ;
  
  //lp_i2c_dev->i2c_dev_buf[0] = lp_i2c_dev->i2c_dev_adress ;
  
  lp_i2c_dev->i2c_dev_buf[0] = (unsigned long)strtoul( MCP4726_CMD_WRITEDAC, NULL, 16 ) ;
  lp_i2c_dev->i2c_dev_buf[1] = output1 ;
  lp_i2c_dev->i2c_dev_buf[2] = output2 ;
  
  // if (writeEEPROM)                                      // command and config bits  (C2.C1.C0.x.x.PD1.PD0.x)
  // {
  //   I2CMasterBuffer[1] = (MCP4726_CMD_WRITEDACEEPROM);
  // }
  // else
  // {
  //  I2CMasterBuffer[1] = (MCP4726_CMD_WRITEDAC);
  // }
  // I2CMasterBuffer[2] = (output / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
  // I2CMasterBuffer[3] = (output % 16) << 4;              // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
  
  if ((write( lp_i2c_dev->i2c_dev_fd, lp_i2c_dev->i2c_dev_buf, 3)) != 3 ) {
    printf("Error when writing to i2c slave\n");
    ret = -1 ;
    lp_i2c_dev->i2c_dev_statut = ret ;
  }
  
  return ret ;
}
//===========================================================
int I2C_SET( STRUCT_I2C_DEVICE * lp_i2c_dev, char * registre, char * value) {

  int ret ;
  
  usleep( lp_i2c_dev->i2c_dev_usleep ) ;
  ret=0 ;
  memset( lp_i2c_dev->i2c_dev_buf, 0, I2C_BUFFER_SIZE ) ;
  
  lp_i2c_dev->i2c_dev_buf[0] = strtoul( registre, NULL, 16 ) ;
  lp_i2c_dev->i2c_dev_buf[1] = strtoul( value, NULL, 16 ) ;
  
  if ((write( lp_i2c_dev->i2c_dev_fd, lp_i2c_dev->i2c_dev_buf, 2)) != 2 ) {
    printf("Error writing to i2c slave\n");
    ret = -1 ;
    lp_i2c_dev->i2c_dev_statut = ret ;
  }
  
  return ret ;
}
//===========================================================
uint16_t I2C_GET( STRUCT_I2C_DEVICE * lp_i2c_dev, char * registre) {
  
  int ret ;
  
  usleep( lp_i2c_dev->i2c_dev_usleep) ;
  ret=0 ;
  memset( lp_i2c_dev->i2c_dev_buf, CONFIG_ZERO_CHAR, I2C_BUFFER_SIZE ) ;
  
  lp_i2c_dev->i2c_dev_buf[0] = strtoul( registre, NULL, 16 ) ;
  
  if ((write( lp_i2c_dev->i2c_dev_fd, lp_i2c_dev->i2c_dev_buf, 1)) != 1 ) {
    printf("Error writing to i2c slave register %s\n", registre);
    ret = -1 ;
    lp_i2c_dev->i2c_dev_statut = ret ;
  }
  
  if ((read( lp_i2c_dev->i2c_dev_fd, lp_i2c_dev->i2c_dev_buf, 1)) != 1 ) {
    printf("Error reading to i2c slave register\n");
    ret = -2 ;
    lp_i2c_dev->i2c_dev_statut = ret ;
  }
  if ( ret == 0 )  return (uint16_t)lp_i2c_dev->i2c_dev_buf[0] ;
  else            return (uint16_t)ret ;
}
//===========================================================
void I2C_GET_6( STRUCT_I2C_DEVICE * lp_i2c_dev, char * registre) {
  
  int ret ;
  
  ret=0 ;
  usleep( lp_i2c_dev->i2c_dev_usleep) ;
  memset( lp_i2c_dev->i2c_dev_buf, 0 , I2C_BUFFER_SIZE ) ;
  
  lp_i2c_dev->i2c_dev_buf[0] = strtoul( registre, NULL, 16 ) ;
  
  if ((write( lp_i2c_dev->i2c_dev_fd, lp_i2c_dev->i2c_dev_buf, 1)) != 1 ) {
    printf("Error writing to i2c slave register %s\n", registre);
    lp_i2c_dev->i2c_dev_statut = ret ;
  }
  if ((read( lp_i2c_dev->i2c_dev_fd, lp_i2c_dev->i2c_dev_buf, 6)) != 6 ) {
    printf("Error reading to i2c slave register %s\n", registre);
    lp_i2c_dev->i2c_dev_statut = ret ;
  }
}
//===========================================================

void I2C_GET_ACC( STRUCT_I2C_DEVICE *lp_I2c, STRUCT_I2C_ACC_MAG *lp_Acc ) {

      lp_Acc->acc_axl = I2C_GET( lp_I2c, REG_OUT_ACC_X_L ) ;
      lp_Acc->acc_axh = I2C_GET( lp_I2c, REG_OUT_ACC_X_H ) ; 
      lp_Acc->acc_ayl = I2C_GET( lp_I2c, REG_OUT_ACC_Y_L ) ;
      lp_Acc->acc_ayh = I2C_GET( lp_I2c, REG_OUT_ACC_Y_H ) ;
      lp_Acc->acc_azl = I2C_GET( lp_I2c, REG_OUT_ACC_Z_L ) ;
      lp_Acc->acc_azh = I2C_GET( lp_I2c, REG_OUT_ACC_Z_H ) ;
             
      lp_Acc->acc_mxl = I2C_GET( lp_I2c, REG_OUT_MAG_X_L ) ;
      lp_Acc->acc_mxh = I2C_GET( lp_I2c, REG_OUT_MAG_X_H ) ; 
      lp_Acc->acc_myl = I2C_GET( lp_I2c, REG_OUT_MAG_Y_L ) ;
      lp_Acc->acc_myh = I2C_GET( lp_I2c, REG_OUT_MAG_Y_H ) ;
      lp_Acc->acc_mzl = I2C_GET( lp_I2c, REG_OUT_MAG_Z_L ) ;
      lp_Acc->acc_mzh = I2C_GET( lp_I2c, REG_OUT_MAG_Z_H ) ;
}
//===========================================================
