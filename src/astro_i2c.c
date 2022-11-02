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
STRUCT_I2C_ACC_MAG accmag,  * am ;

void I2C_CALCULS_ACCMAG(STRUCT_I2C_ACC_MAG *am) {
  
  float norme_acc, norme_mag  ;
  // float phi, psi , the ;
  
  //---------------------------------------------
  // Normalisation des mesures de l'accelerometre
  //---------------------------------------------
  
  am->Ax = (float)((int16_t)((am->axh << 8 ) | am->axl ))  ;
  am->Ay = (float)((int16_t)((am->ayh << 8 ) | am->ayl ))  ;
  am->Az = (float)((int16_t)((am->azh << 8 ) | am->azl ))  ;
  
  norme_acc = sqrt((am->Ax*am->Ax)+(am->Ay*am->Ay)+(am->Az*am->Az) ) ;
  
  if ( norme_acc > am->A_norme_max ) am->A_norme_max = norme_acc ; 
  
  am->Xa = am->Ax / norme_acc ;
  am->Ya = am->Ay / norme_acc ;
  am->Za = am->Az / norme_acc ;
  
  //---------------------------------------------
  // Normalisation des mesures de la boussole
  //---------------------------------------------
  
  am->Mx = (float)((int16_t)((am->mxh << 8 ) | am->mxl ))  ;
  am->My = (float)((int16_t)((am->myh << 8 ) | am->myl ))  ;
  am->Mz = (float)((int16_t)((am->mzh << 8 ) | am->mzl ))  ;
  
  norme_mag = sqrt((am->Mx*am->Mx)+(am->My*am->My)+(am->Mz*am->Mz) ) ;
  
  if ( norme_mag > am->M_norme_max ) am->M_norme_max = norme_mag ; 
  
  am->Xm = am->Mx / norme_mag ;
  am->Ym = am->My / norme_mag ;
  am->Zm = am->Mz / norme_mag ;
  
  //---------------------------------------------
  // Calcul pitch and roll
  //---------------------------------------------
  
  am->pitch = asin (-1.0 * am->Xa ) ;
  am->roll  = asin ( am->Ya / cos ( am->pitch) ) ;
  
  //---------------------------------------------
  // Calcul heading
  //---------------------------------------------
  
  // phi = atan2f(am->Mx,am->My) * 360 / ( 2 * M_PI ) ;  
  // psi = atan2f(am->My,am->Mz) * 360 / ( 2 * M_PI ) ;   
  // the = atan2f(am->Mz,am->Mx) * 360 / ( 2 * M_PI )  ;  
  
  am->Xh = am->Xm * cos ( am->pitch ) + am->Zm * sin (am->pitch ) ;
  am->Yh = am->Xm * sin(am->roll) * sin(am->pitch) + am->Ym - am->Zm * sin(am->roll)*cos( am->pitch) ;
  
  if ( am->Xh > 0 && am->Yh >= 0 ) am->heading = atan( am->Yh / am->Xh ) ;
  if ( am->Xh > 0 && am->Yh < 0 )  am->heading = 2 * M_PI + atan( am->Yh / am->Xh ) ;
  if ( am->Xh < 0 )                am->heading = M_PI + atan( am->Yh / am->Xh ) ;
  if ( am->Xh == 0 && am->Yh < 0 ) am->heading = M_PI / 2.0 ;
  if ( am->Xh == 0 && am->Yh > 0 ) am->heading = M_PI * 1.5  ;
  
  // printf("%.0f\t%.0f\t%.0f\t%.0f\n", am->Ax , am->Ay , am->Az, norme_acc) ;
  // printf("%.0f\t%.0f\t%.0f\t%.0f\n", am->Mx , am->My , am->Mz, norme_mag) ;
  // printf("%.2f\t%.2f\t%.2f\t%.2f\n", am->Xm , am->Ym , am->Zm, sqrt( (am->Xm * am->Xm) + (am->Ym * am->Ym) + ( am->Zm * am->Zm) ) ) ;
  
  //printf("%.0f\t%.0f\t%.0f\n", am->pitch * I2C_DEGRAD, am->roll * I2C_DEGRAD, am->heading * I2C_DEGRAD) ;
  //printf("%.0f\t%.0f\t%.0f\n", phi, psi, the) ;
  //printf("%.0f\t%.0f\t%.0f\n",am->Mx * 90 / 16384 , am->My * 90 / 16384, am->Mz * 90 / 16384 );
  //printf("---------\n") ;
}
//===========================================================
int I2C_INIT( STRUCT_I2C_DEVICE * p_i2c_dev, char * c_i2c_device_name, char * adress) {
   
  int ret ;
  ret =0 ;
  
  if ( ( p_i2c_dev->fd = open(c_i2c_device_name, O_RDWR)) < 0)  ret = -1 ;
  else if ( ioctl( p_i2c_dev->fd, I2C_SLAVE, strtoul(adress,NULL,16) ) < 0) ret = -2 ;
  else p_i2c_dev->adress = strtoul(adress,NULL,16) ;
  
  p_i2c_dev->statut = ret ;  
  p_i2c_dev->usleep  = I2C_SLEEP_MICRO ;
  
  Trace1("I2C_INIT : %ld : statut : %d\n", strtoul(adress,NULL,16) , ret);
  printf("I2C_INIT : fd = %d adress = %d statut : %d\n", p_i2c_dev->fd , p_i2c_dev->adress,  p_i2c_dev->statut);
  
  return ret ;
}
//===========================================================
int I2C_SET_3( STRUCT_I2C_DEVICE * p_i2c_dev, unsigned long output) {

  int ret ;
  unsigned long output1 ;
  unsigned long output2 ;
  
  output1 = (output / 16);
  output2 = (output % 16) << 4;
  //output1 =0 ;
  //output2 =0;
  usleep( p_i2c_dev->usleep ) ;
  ret=0 ;
  memset( p_i2c_dev->buf, 0 , I2C_BUFFER_SIZE ) ;
  
  //p_i2c_dev->buf[0] = p_i2c_dev->adress ;
  
  p_i2c_dev->buf[0] = (unsigned long)strtoul( MCP4726_CMD_WRITEDAC, NULL, 16 ) ;
  p_i2c_dev->buf[1] = output1 ;
  p_i2c_dev->buf[2] = output2 ;
  
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
  
  if ((write( p_i2c_dev->fd, p_i2c_dev->buf, 3)) != 3 ) {
    printf("Error when writing to i2c slave\n");
    ret = -1 ;
    p_i2c_dev->statut = ret ;
  }
  
  return ret ;
}
//===========================================================
int I2C_SET( STRUCT_I2C_DEVICE * p_i2c_dev, char * registre, char * value) {

  int ret ;
  
  usleep( p_i2c_dev->usleep ) ;
  ret=0 ;
  memset( p_i2c_dev->buf, 0, I2C_BUFFER_SIZE ) ;
  
  p_i2c_dev->buf[0] = strtoul( registre, NULL, 16 ) ;
  p_i2c_dev->buf[1] = strtoul( value, NULL, 16 ) ;
  
  if ((write( p_i2c_dev->fd, p_i2c_dev->buf, 2)) != 2 ) {
    printf("Error writing to i2c slave\n");
    ret = -1 ;
    p_i2c_dev->statut = ret ;
  }
  
  return ret ;
}
//===========================================================
uint16_t I2C_GET( STRUCT_I2C_DEVICE * p_i2c_dev, char * registre) {
  
  int ret ;
  
  usleep( p_i2c_dev->usleep) ;
  ret=0 ;
  memset( p_i2c_dev->buf, CONFIG_ZERO_CHAR, I2C_BUFFER_SIZE ) ;
  
  p_i2c_dev->buf[0] = strtoul( registre, NULL, 16 ) ;
  
  if ((write( p_i2c_dev->fd, p_i2c_dev->buf, 1)) != 1 ) {
    printf("Error writing to i2c slave register %s\n", registre);
    ret = -1 ;
    p_i2c_dev->statut = ret ;
  }
  
  if ((read( p_i2c_dev->fd, p_i2c_dev->buf, 1)) != 1 ) {
    printf("Error reading to i2c slave register\n");
    ret = -2 ;
    p_i2c_dev->statut = ret ;
  }
  if ( ret == 0 )  return (uint16_t)p_i2c_dev->buf[0] ;
  else            return (uint16_t)ret ;
}
//===========================================================
void I2C_GET_6( STRUCT_I2C_DEVICE * p_i2c_dev, char * registre) {
  
  int ret ;
  
  ret=0 ;
  usleep( p_i2c_dev->usleep) ;
  memset( p_i2c_dev->buf, 0 , I2C_BUFFER_SIZE ) ;
  
  p_i2c_dev->buf[0] = strtoul( registre, NULL, 16 ) ;
  
  if ((write( p_i2c_dev->fd, p_i2c_dev->buf, 1)) != 1 ) {
    printf("Error writing to i2c slave register %s\n", registre);
    p_i2c_dev->statut = ret ;
  }
  if ((read( p_i2c_dev->fd, p_i2c_dev->buf, 6)) != 6 ) {
    printf("Error reading to i2c slave register %s\n", registre);
    p_i2c_dev->statut = ret ;
  }
}
//===========================================================

void I2C_GET_ACC( STRUCT_I2C_DEVICE *ex, STRUCT_I2C_ACC_MAG *am ) {

      am->axl = I2C_GET( ex, REG_OUT_ACC_X_L ) ;
      am->axh = I2C_GET( ex, REG_OUT_ACC_X_H ) ; 
      am->ayl = I2C_GET( ex, REG_OUT_ACC_Y_L ) ;
      am->ayh = I2C_GET( ex, REG_OUT_ACC_Y_H ) ;
      am->azl = I2C_GET( ex, REG_OUT_ACC_Z_L ) ;
      am->azh = I2C_GET( ex, REG_OUT_ACC_Z_H ) ;
             
      am->mxl = I2C_GET( ex, REG_OUT_MAG_X_L ) ;
      am->mxh = I2C_GET( ex, REG_OUT_MAG_X_H ) ; 
      am->myl = I2C_GET( ex, REG_OUT_MAG_Y_L ) ;
      am->myh = I2C_GET( ex, REG_OUT_MAG_Y_H ) ;
      am->mzl = I2C_GET( ex, REG_OUT_MAG_Z_L ) ;
      am->mzh = I2C_GET( ex, REG_OUT_MAG_Z_H ) ;
}
//===========================================================
