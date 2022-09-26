#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
  double f_mod ;
  if( argc<3 ){
    printf("%s <x> <y>\n",argv[0]) ;
    exit(1) ;
  }
  f_mod = fmod( \
     atof( argv[1] ) , \
     atof(argv[2] ) ) ;
  
  printf("%f\n", f_mod ) ;
  
  return 0 ;
}
