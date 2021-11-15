#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const float f_facteur_multiplicatif = 1000000.0 / (1024.0 *1024.0) ;

float main(int argc, char ** argv) {
   if ( argc != 3  ) {
      printf("Usage\t: %s <taille des ecritures en OCTETS> <duree des ecritures en MILLISECONDES>\n",argv[0]) ;
      printf("\t: ==> renvoie le debit en Mo/s\n") ;
      exit(1) ;
   }
   return f_facteur_multiplicatif * atof(argv[1]) / atof(argv[2]) ;
}
