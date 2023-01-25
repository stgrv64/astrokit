/* #define _GNU_SOURCE */

#ifndef ASTRO_I2C_H
#define ASTRO_I2C_H

#include "astro_global.h"

#define CD4059_BASE 	10
#define	CD4059_EL 	0
#define	CD4059_Ka 	1
#define	CD4059_Kb 	1
#define	CD4059_Kc	0

#define	PORT_K_ALT 	0
#define	PORT_K_AZI	1

// description du device dans /dev utilise 
// sur la 1ere raspi il s'agit de /dev/i2c-0
// sur les autres    il s'agit de /dev/i2c-1

#define DEVICE_RASPI_A    "/dev/i2c-0" 
#define DEVICE_RASPI_B    "/dev/i2c-1"
#define DEVICE_RASPI_BP   "/dev/i2c-1"
#define DEVICE_RASPI_2    "/dev/i2c-1"

// definition de tous les address servant dans les sources(au fil de l'eau) 

#define DEVICE_LSM_ADRESS "0x1d"  // adress d'un accelerometre + boussole numerique LSM303D
#define DEVICE_MCP_ADRESS "0x20"  // adress des extandeurs de ports I2C de la famille MCP
#define DEVICE_RTC_ADRESS "0x68"  // adress des RTC basee sur le DS1307
#define DEVICE_MCP4725_0  "0x60"  // adress MCP4725, DAC 12 bits
#define DEVICE_MCP4725_1  "0x61"  // adress MCP4725, DAC 12 bits, A0 = +5v

// definition des registres utiles dans les sources

#define MCP4725_ADDRESS                 "0xC0"   // 1100000x - Assumes A0 is GND and A2,A1 are 0 (MCP4725A0T-E/CH)
#define MCP4725_READ                    "0x01"
#define MCP4726_CMD_WRITEDAC            "0x40"  // Writes data to the DAC
#define MCP4726_CMD_WRITEDACEEPROM      "0x60"  // Writes data to the DAC and the EEPROM (persisting the assigned value after reset)

#define REG_CTRL1       "0x20"
#define REG_CTRL2       "0x21"
#define REG_CTRL5       "0x24"
#define REG_CTRL6       "0x25"
#define REG_CTRL7       "0x26"

#define REG_OUT_ACC_X_L "0x28"
#define REG_OUT_ACC_X_H "0x29"
#define REG_OUT_ACC_Y_L "0x2a"
#define REG_OUT_ACC_Y_H "0x2b"
#define REG_OUT_ACC_Z_L "0x2c"
#define REG_OUT_ACC_Z_H "0x2d"

#define REG_OUT_MAG_X_L "0x08"
#define REG_OUT_MAG_X_H "0x09"
#define REG_OUT_MAG_Y_L "0x0a"
#define REG_OUT_MAG_Y_H "0x0b"
#define REG_OUT_MAG_Z_L "0x0c"
#define REG_OUT_MAG_Z_H "0x0d"

// definition des ports pour le MCP23017 / MCP23008

#define	PORT_I2C_ALT	"0x20"
#define	PORT_I2C_AZI	"0x21"

// definition des ports pour le MCP23008

#define IODIR           "0x00"
#define GP              "0x0A"

// definition des ports pour le MCP23017

#define IODIRA          "0x00"
#define IODIRB          "0x01"

#define	GPA             "0x14"
#define	GPB             "0x15"

// taille du buffer size

#define I2C_DEGRAD      57.2957792
#define I2C_BUFFER_SIZE 10 
#define I2C_SLEEP_MICRO 10000        

/* ---------------------------------
  la structure suivante sert si un expandeur I2C 
   de port est utilise. 
   Exemple = MCP23017 ou MCP23008 
 ----------------------------------- */

typedef struct STR_I2C_MCP23017  STRUCT_I2C_MCP23017 ;

struct STR_I2C_MCP23017 {

  unsigned char mcp_alt_dir   ; // numero de port GPIO pour la direction de l'altitude
  unsigned char mcp_alt_clk   ; // numero de port GPIO pour l'horloge de l'altitude
  unsigned char mcp_alt_slp   ; // numero de port GPIO pour le sleep en altitude
  unsigned char mcp_alt_rst   ; // numero de port GPIO pour le reset en altitude
  unsigned char mcp_alt_m2    ; // numero de port GPIO pour le choix du micro pas en altitude
  unsigned char mcp_alt_m1    ; // numero de port GPIO pour le choix du micro pas en altitude 
  unsigned char mcp_alt_m0   ;  // numero de port GPIO pour le choix du micro pas en altitude
  
  unsigned char mcp_azi_dir   ; // numero de port GPIO pour la direction de l'azimut
  unsigned char mcp_azi_clk   ; // numero de port GPIO pour l'horloge de l'azimut
  unsigned char mcp_azi_slp   ; // numero de port GPIO pour le sleep en azimut
  unsigned char mcp_azi_rst   ; // numero de port GPIO pour le reset en azimut
  unsigned char mcp_azi_m2    ; // numero de port GPIO pour le choix du micro pas en azimut
  unsigned char mcp_azi_m1    ; // numero de port GPIO pour le choix du micro pas en azimut
  unsigned char mcp_azi_m0   ;  // numero de port GPIO pour le choix du micro pas en azimut
} ;

// structure decrivant un device
//---------------------------------------------------

typedef struct STR_I2C_DEVICE  STRUCT_I2C ;


struct STR_I2C_DEVICE {

  pthread_mutex_t  i2c_dev_mutex ;
  STR_EXT_TIMEVAL  i2c_dev_tval ; 
  FILE            *i2c_dev_file ; 
  char             i2c_dev_loglevel ;
  void           (*i2c_dev_lock)       ( STRUCT_I2C * ) ;
  void           (*i2c_dev_unlock)     ( STRUCT_I2C * ) ;  
  void           (*i2c_dev_log)        ( STRUCT_I2C * ) ;
  void           (*i2c_dev_display)    ( STRUCT_I2C * ) ;
  char             i2c_dev_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char             i2c_dev_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;

  char             i2c_dev_file_path[ CONFIG_TAILLE_BUFFER_256  ] ;
  unsigned char    i2c_dev_buf[ I2C_BUFFER_SIZE ] ; // taille du buffer size pour echanges des g_Datas en read / write
  unsigned long    i2c_dev_usleep ;         // sleep entre chaque lecture (microseconde)
  int              i2c_dev_statut ;                 // statut (0 = OK, toutes autres valeurs = KO) 
  int              i2c_dev_fd ;                     // file descriptor pour le device utilise
  int              i2c_dev_registre ;               // registre
  int              i2c_dev_adress ;                 // adress du device (en hexa)
} ;

//---------------------------------------------------

typedef struct STR_I2C_ACC_MAG STRUCT_I2C_ACC_MAG ;

struct STR_I2C_ACC_MAG {
  
  pthread_mutex_t  acc_mutex ;
  STR_EXT_TIMEVAL  acc_tval ; 
  FILE            *acc_file ; 
  char             acc_loglevel ;
  void           (*acc_lock)       ( STRUCT_I2C_ACC_MAG * ) ;
  void           (*acc_unlock)     ( STRUCT_I2C_ACC_MAG * ) ;  
  void           (*acc_log)        ( STRUCT_I2C_ACC_MAG * ) ;
  void           (*acc_display)    ( STRUCT_I2C_ACC_MAG * ) ;
  char             acc_dis_for     [ CONFIG_TAILLE_BUFFER_256 ] ;
  char             acc_dis_cmd     [ CONFIG_TAILLE_BUFFER_256 ] ;

  float acc_acc_norme_max ; 
  float acc_mag_norme_max ;
  float acc_roll ;
  float acc_pitch ;
  float acc_heading ;
  
  // Measerements for Magnetometer 
  
  float acc_mag_x;
  float acc_mag_y ;
  float acc_mag_z ;
  
  // Measerements for Accelerometer 
  
  float acc_acc_x;
  float acc_acc_y ;
  float acc_acc_z ;
  
  // Normalized values for Magnetometer sqrt( acc_gauss_field_x*acc_gauss_field_x + acc_gauss_field_y*acc_gauss_field_y + acc_gauss_field_z*acc_gauss_field_z ) = 1
  
  float acc_mag_norm_x ;
  float acc_mag_norm_y ;
  float acc_mag_norm_z ;
  
  // Normalized values for Accelerometer sqrt( Ax1*acc_acc_x + acc_gauss_field_y*acc_gauss_field_y + acc_gauss_field_z*acc_gauss_field_z ) = 1
  
  float acc_acc_norm_x ;
  float acc_acc_norm_y ;
  float acc_acc_norm_z ;
  
  // Values of projection of gauss field component into (XYZ) plane 
  
  float acc_gauss_field_x;
  float acc_gauss_field_y ;
  float acc_gauss_field_z ;
  
  uint16_t acc_axl ;
  uint16_t acc_ayl ;
  uint16_t acc_azl ;
  
  uint16_t acc_axh ;
  uint16_t acc_ayh ;
  uint16_t acc_azh ;
  
  uint16_t acc_mxl ;
  uint16_t acc_myl ;
  uint16_t acc_mzl ; 
  
  uint16_t acc_mxh ;
  uint16_t acc_myh ; 
  uint16_t acc_mzh ;
  
  int acc_gpio ; // port GPIO pour tester la sortie son 
  
} ;

int 	   I2C_INIT               ( STRUCT_I2C *, char * , char * ) ;
void     I2C_CALCULS_ACCMAG     ( STRUCT_I2C_ACC_MAG * ) ;
uint16_t I2C_READ_1_BYTES       ( STRUCT_I2C *, char * ) ;
void     I2C_READ_6_BYTES       ( STRUCT_I2C *, char * ) ;
int      I2C_WRITE                ( STRUCT_I2C *, char * , char * ) ;
void     I2C_ACC_READ            ( STRUCT_I2C *, STRUCT_I2C_ACC_MAG * )  ;
int      I2C_WRITE_DAC_MCP4726  ( STRUCT_I2C *, unsigned long ) ;

#endif
