#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <libfahw.h>
#include <time.h>

#define I2C_DEV 1

int main(int argc, char ** argv)
{
  int devFD ;

  if (argc <3 ) {
    printf("Usage: %s\n \
       <ligne1 (char[16] max)>\n\t \
       <ligne2 char[16] max>\n\t \
       <displaying          (0/1) (fac)>\n\t \
       <clear at each talk  (0/1) (fac)>\n\t \
       <sleep between clear and display (value us) (fac)>\n", argv[0]);
    exit(0) ;
  }

  if ((devFD = LCD1602Init(I2C_DEV)) == -1) {printf("Fail to init LCD1602\n");return -1;}
  else { printf("LCD1602Init (OK)\n"); }
  
  if ( argc >= 5 && argv[4][0] == '1') {
    if ( LCD1602Clear(devFD) == -1) {
      printf("Fail to LCD1602Clear\n");
      return -1;
    }
    else {
      printf("LCD1602Clear (OK)\n");
    }
  }
  else {
    printf("no LCD1602Clear\n");
  }
  if ( argc >= 6 && atol(argv[5]) > 0 ) {
    /* entre 2500 et 5000 semble une bonne valeur de usleep */
    /* si on ne fait pas de usleep , l ecran ne se clear pas completement (teste) */
    usleep(atol(argv[5])) ;
  }
  else {
   printf("no usleep\n");
  } 
  
  if ( LCD1602DispLines(devFD, argv[1], argv[2]) == -1) { 
    printf("Fail to Display String\n");
    return -1;
  }

  if ( argc >= 4 && argv[3][0] == '1') {
    LCD1602Close(devFD);
  }
  else {
    printf("no LCD1602Close\n");
  }
  
  return 0;
}



/* Fin fichier ---------------------------
*
*
* */
