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

unsigned short sym[16] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15} ;

STRUCT_I2C_DEVICE  lsm303d,   *lsm ;
STRUCT_I2C_DEVICE  mcp23008,  *mcp ;
STRUCT_I2C_DEVICE  rtcds1307, *rtc ;
STRUCT_I2C_DEVICE  exemple,   *ex ;
STRUCT_I2C_ACC_MAG accmag,    *lp_i2c_acc ;

void I2C_CALCULS_ACCMAG(STRUCT_I2C_ACC_MAG *lp_i2c_acc) {
  
  float norme_acc, norme_mag  ;
  // float phi, psi , the ;
  
  //---------------------------------------------
  // Normalisation des mesures de l'accelerometre
  //---------------------------------------------
  
  lp_i2c_acc->Ax = (float)((int16_t)((lp_i2c_acc->axh << 8 ) | lp_i2c_acc->axl ))  ;
  lp_i2c_acc->Ay = (float)((int16_t)((lp_i2c_acc->ayh << 8 ) | lp_i2c_acc->ayl ))  ;
  lp_i2c_acc->Az = (float)((int16_t)((lp_i2c_acc->azh << 8 ) | lp_i2c_acc->azl ))  ;
  
  norme_acc = sqrt((lp_i2c_acc->Ax*lp_i2c_acc->Ax)+(lp_i2c_acc->Ay*lp_i2c_acc->Ay)+(lp_i2c_acc->Az*lp_i2c_acc->Az) ) ;
  
  if ( norme_acc > lp_i2c_acc->A_norme_max ) lp_i2c_acc->A_norme_max = norme_acc ; 
  
  lp_i2c_acc->Xa = lp_i2c_acc->Ax / norme_acc ;
  lp_i2c_acc->Ya = lp_i2c_acc->Ay / norme_acc ;
  lp_i2c_acc->Za = lp_i2c_acc->Az / norme_acc ;
  
  //---------------------------------------------
  // Normalisation des mesures de la boussole
  //---------------------------------------------
  
  lp_i2c_acc->Mx = (float)((int16_t)((lp_i2c_acc->mxh << 8 ) | lp_i2c_acc->mxl ))  ;
  lp_i2c_acc->My = (float)((int16_t)((lp_i2c_acc->myh << 8 ) | lp_i2c_acc->myl ))  ;
  lp_i2c_acc->Mz = (float)((int16_t)((lp_i2c_acc->mzh << 8 ) | lp_i2c_acc->mzl ))  ;
  
  norme_mag = sqrt((lp_i2c_acc->Mx*lp_i2c_acc->Mx)+(lp_i2c_acc->My*lp_i2c_acc->My)+(lp_i2c_acc->Mz*lp_i2c_acc->Mz) ) ;
  
  if ( norme_mag > lp_i2c_acc->M_norme_max ) lp_i2c_acc->M_norme_max = norme_mag ; 
  
  lp_i2c_acc->Xm = lp_i2c_acc->Mx / norme_mag ;
  lp_i2c_acc->Ym = lp_i2c_acc->My / norme_mag ;
  lp_i2c_acc->Zm = lp_i2c_acc->Mz / norme_mag ;
  
  //---------------------------------------------
  // Calcul pitch and roll
  //---------------------------------------------
  
  lp_i2c_acc->pitch = asin (-1.0 * lp_i2c_acc->Xa ) ;
  lp_i2c_acc->roll  = asin ( lp_i2c_acc->Ya / cos ( lp_i2c_acc->pitch) ) ;
  
  //---------------------------------------------
  // Calcul heading
  //---------------------------------------------
  
  // phi = atan2f(lp_i2c_acc->Mx,lp_i2c_acc->My) * 360 / ( 2 * M_PI ) ;  
  // psi = atan2f(lp_i2c_acc->My,lp_i2c_acc->Mz) * 360 / ( 2 * M_PI ) ;   
  // the = atan2f(lp_i2c_acc->Mz,lp_i2c_acc->Mx) * 360 / ( 2 * M_PI )  ;  
  
  lp_i2c_acc->Xh = lp_i2c_acc->Xm * cos ( lp_i2c_acc->pitch ) + lp_i2c_acc->Zm * sin (lp_i2c_acc->pitch ) ;
  lp_i2c_acc->Yh = lp_i2c_acc->Xm * sin(lp_i2c_acc->roll) * sin(lp_i2c_acc->pitch) + lp_i2c_acc->Ym - lp_i2c_acc->Zm * sin(lp_i2c_acc->roll)*cos( lp_i2c_acc->pitch) ;
  
  if ( lp_i2c_acc->Xh > 0 && lp_i2c_acc->Yh >= 0 ) lp_i2c_acc->heading = atan( lp_i2c_acc->Yh / lp_i2c_acc->Xh ) ;
  if ( lp_i2c_acc->Xh > 0 && lp_i2c_acc->Yh < 0 )  lp_i2c_acc->heading = 2 * M_PI + atan( lp_i2c_acc->Yh / lp_i2c_acc->Xh ) ;
  if ( lp_i2c_acc->Xh < 0 )                lp_i2c_acc->heading = M_PI + atan( lp_i2c_acc->Yh / lp_i2c_acc->Xh ) ;
  if ( lp_i2c_acc->Xh == 0 && lp_i2c_acc->Yh < 0 ) lp_i2c_acc->heading = M_PI / 2.0 ;
  if ( lp_i2c_acc->Xh == 0 && lp_i2c_acc->Yh > 0 ) lp_i2c_acc->heading = M_PI * 1.5  ;
  
  // printf("%.0f\t%.0f\t%.0f\t%.0f\n", lp_i2c_acc->Ax , lp_i2c_acc->Ay , lp_i2c_acc->Az, norme_acc) ;
  // printf("%.0f\t%.0f\t%.0f\t%.0f\n", lp_i2c_acc->Mx , lp_i2c_acc->My , lp_i2c_acc->Mz, norme_mag) ;
  // printf("%.2f\t%.2f\t%.2f\t%.2f\n", lp_i2c_acc->Xm , lp_i2c_acc->Ym , lp_i2c_acc->Zm, sqrt( (lp_i2c_acc->Xm * lp_i2c_acc->Xm) + (lp_i2c_acc->Ym * lp_i2c_acc->Ym) + ( lp_i2c_acc->Zm * lp_i2c_acc->Zm) ) ) ;
  
  //printf("%.0f\t%.0f\t%.0f\n", lp_i2c_acc->pitch * I2C_DEGRAD, lp_i2c_acc->roll * I2C_DEGRAD, lp_i2c_acc->heading * I2C_DEGRAD) ;
  //printf("%.0f\t%.0f\t%.0f\n", phi, psi, the) ;
  //printf("%.0f\t%.0f\t%.0f\n",lp_i2c_acc->Mx * 90 / 16384 , lp_i2c_acc->My * 90 / 16384, lp_i2c_acc->Mz * 90 / 16384 );
  //printf("---------\n") ;
}
//===========================================================
int I2C_INIT( STRUCT_I2C_DEVICE * lp_i2c_dev, char * c_i2c_device_name, char * adress) {
   
  int ret ;
  ret =0 ;
  
  pthread_mutex_init(&lp_i2c_dev->i2c_dev_mutex,NULL) ;
  
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

void I2C_GET_ACC( STRUCT_I2C_DEVICE *ex, STRUCT_I2C_ACC_MAG *lp_i2c_acc ) {

      lp_i2c_acc->axl = I2C_GET( ex, REG_OUT_ACC_X_L ) ;
      lp_i2c_acc->axh = I2C_GET( ex, REG_OUT_ACC_X_H ) ; 
      lp_i2c_acc->ayl = I2C_GET( ex, REG_OUT_ACC_Y_L ) ;
      lp_i2c_acc->ayh = I2C_GET( ex, REG_OUT_ACC_Y_H ) ;
      lp_i2c_acc->azl = I2C_GET( ex, REG_OUT_ACC_Z_L ) ;
      lp_i2c_acc->azh = I2C_GET( ex, REG_OUT_ACC_Z_H ) ;
             
      lp_i2c_acc->mxl = I2C_GET( ex, REG_OUT_MAG_X_L ) ;
      lp_i2c_acc->mxh = I2C_GET( ex, REG_OUT_MAG_X_H ) ; 
      lp_i2c_acc->myl = I2C_GET( ex, REG_OUT_MAG_Y_L ) ;
      lp_i2c_acc->myh = I2C_GET( ex, REG_OUT_MAG_Y_H ) ;
      lp_i2c_acc->mzl = I2C_GET( ex, REG_OUT_MAG_Z_L ) ;
      lp_i2c_acc->mzh = I2C_GET( ex, REG_OUT_MAG_Z_H ) ;
}
//===========================================================
