#include <astro_i2c.h>
#include <astro_gpios.h>
#include <pthread.h>
//===========================================================
// Thread de creation d'un son 
//===========================================================

void * SUIVI_GPIO( I2C_ACC_MAG  *am ) {
  
  int ret, dir ;
  double frequence ;
  int gpio ;
  
  sleep(2) ; // on laisse le temps au capteur de commencer a travailler
  dir=1 ;
  frequence = 1000000 ;
  gpio = 21 ;
  
  printf("ret=%d\n",GPIO_CLOSE_BROCHE( gpio )) ;
  printf("ret=%d\n",GPIO_OPEN_BROCHE(gpio,dir)) ;
  
  if ( dir ) {
   while(1) {
    //pthread_mutex_lock(& mutex_i2c );
    frequence = am->heading * 100 ;
    //Trace(""); pthread_mutex_unlock(& mutex_i2c );   
    
    //printf(" ... frequence %.2f = HZ sur la broche %d \n",frequence, gpio) ;
    if ( frequence > 10 && frequence < 1000 ) {
    GPIO_SET( gpio , 1 ) ; usleep((int)(300000.0/(frequence))) ;
    GPIO_SET( gpio , 0 ) ; usleep((int)(700000.0/(frequence))) ; 
   }
   else
     usleep(50000) ;
   GPIO_SET(gpio,0);
  }
  printf("ret=%d\n",GPIO_CLOSE_BROCHE( gpio )) ;
 }
}
//===========================================================
// Thread de suivi de I2C 
//===========================================================

void * SUIVI_I2C( I2C_ACC_MAG  *am ) {
  
  int delai =  50000 ;
   while(1) { 
      usleep(delai) ;
      //pthread_mutex_lock(& mutex_i2c ); 
      I2C_GET_ACC( ex, am )   ;   
      I2C_CALCULS_ACCMAG( am ) ;
      //Trace(""); pthread_mutex_unlock(& mutex_i2c );
    
      //printf("pitch = %.0f\troll = %.0f\theading = %.0f\n", am->pitch * I2C_DEGRAD, am->roll * I2C_DEGRAD, am->heading * I2C_DEGRAD) ;
      //printf("%d.%d\t%d.%d\t%d.%d\n", am->roll,am->pitch, am->heading,am->ayl,am->azh,am->azl );
      //printf("%d.%d\t%d.%d\t%d.%d\n", am->mxh,am->mxl, am->myh,am->myl,am->mzh,am->mzl );
    }
}

//===========================================================
// Main de test pour cette librairie
//===========================================================

void main(int argc, char ** argv) {
  
  int io ;
 
  pthread_t p_thread_gpio ;
  pthread_t p_thread_i2c ;
 
  ex = & exemple ;
  am = & accmag ;
  
  am->gpio = 21 ;
  
  ex->fd = 0 ;
  if ( argc < 2 ) {
    printf("Voici les device disponibles sur le port I2C : \n") ;
    system("/usr/sbin/i2cdetect -y 1") ;
    printf("Usage de %s : <adresse> <registre (facultatif)> <I/O : R:-1 W:other(facultatif)>\n", argv[0]) ;
    exit(0) ;
  }
  
  // ----------------------------------------------------
  // Initialisation du port I2C : open + ioctl (cf fonction)
  // ----------------------------------------------------
  
  I2C_INIT(ex, DEVICE_RASPI_2, argv[1]) ;
  
  // ----------------------------------------------------
  // Exemple expandeur de port MCPxxxxxx
  // ----------------------------------------------------
  
  if ( ! strcmp ( DEVICE_MCP4725_0 , argv[1] ) ) I2C_SET_3( ex, atoi(argv[2]) ) ;
  if ( ! strcmp ( DEVICE_MCP4725_1 , argv[1] ) ) {
    int i ;
    for(i=1000;i<2000;i++) {
      usleep(10000) ;
      I2C_SET_3( ex, i ) ;
    }
  }
  // ----------------------------------------------------
  // Exemple accelerometer + boussole  numerique
  // ----------------------------------------------------
  
  if ( ! strcmp ( DEVICE_LSM_ADRESS , argv[1] ) ) {
  
    I2C_SET( ex, REG_CTRL1, "0x57" ) ;
    I2C_SET( ex, REG_CTRL2, "0x00" ) ;
    I2C_SET( ex, REG_CTRL5, "0x64" ) ;
    I2C_SET( ex, REG_CTRL6, "0x20" ) ;
    I2C_SET( ex, REG_CTRL7, "0x00" ) ;
    
    am->A_norme_max = 0 ;
    am->M_norme_max = 0 ;
    
    // creation du thread sonore : on passe la struture am des devices du capteur
    // dans le thread
    
    pthread_create( &p_thread_i2c,  NULL, (void*)SUIVI_I2C, am ) ;
    pthread_create( &p_thread_gpio,  NULL, (void*)SUIVI_GPIO, am ) ;
    pthread_join( p_thread_gpio, NULL) ;   
    pthread_join( p_thread_i2c, NULL) ;   
  }
  
  // ----------------------------------------------------
  // Exemple expandeur de port MCPxxxxxx
  // ----------------------------------------------------
  
  if ( ! strcmp ( DEVICE_MCP_ADRESS , argv[1] ) ) {
     
     I2C_SET( ex, IODIR, "0x00" ) ;
     if ( strtoul( argv[3], NULL, 16 ) >= 0 )
       I2C_SET( ex, argv[2], argv[3] ) ;
     else {
       I2C_GET( ex, argv[2]) ;
       
     }
  }
}
