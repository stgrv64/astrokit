#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define DEGRAD 57.2957792
#define RADDEG 1.57079632679

int main(int argc, char ** argv) {

  uint32_t xl, yl, zl, xh, yh, zh  ;
    
  float  ax,   ay,   az ;
  float  vx,   vy,   vz ;
  float  ax1,  ay1,  az1 ;
  float  phi,  psi,  the ;
  float  phi1, psi1, the1 ;
  float  N,    N1 ;
  
  // printf("a = %s %s %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
  // printf("a = %s %ld\n", argv[1], strtoul(argv[1],NULL,16));

  xl = strtoul(argv[1],NULL,16) ;
  yl = strtoul(argv[2],NULL,16) ;
  zl = strtoul(argv[3],NULL,16) ;
  
  xh = strtoul(argv[4],NULL,16) ;
  yh = strtoul(argv[5],NULL,16) ;
  zh = strtoul(argv[6],NULL,16) ;
  
  //------------------------------------------------------
  // calcul avec complement signe a deux
  // methode avec complement a deux apres calcul du decalage et du ou : 
  // correct
  //------------------------------------------------------
  
  ax1 = (float)((int16_t)((xh << 8 ) | xl ))  ;
  ay1 = (float)((int16_t)((yh << 8 ) | yl ))  ;
  az1 = (float)((int16_t)((zh << 8 ) | zl ))  ;
  
  N = sqrt( ax1 * ax1 + ay1 * ay1 + az1 * az1 ) ;
  
  phi1   = atan2f(ax1,ay1) * 360 / ( 2 * M_PI ) ;   // donne l'angle entre g et sa projection sur xoy
  psi1   = atan2f(ay1,az1) * 360 / ( 2 * M_PI ) ;   
  the1   = atan2f(az1,ax1) * 360 / ( 2 * M_PI )  ;  
  
  //printf("%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\n", phi1, psi1, the1, ax1  , ay1 , az1 );
  printf("%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\t%.0f\n", phi1, psi1, the1, ax1 * 90 / 16384 , ay1 * 90 / 16384, az1 * 90 / 16384 );
  
  //------------------------------------------------------
  // Coordonnees polaires du vecteur Force
  // Deduction du Roll et du Pitch
  // Par convention --------------------------
  // - axe des X en avant dans la direction du scope
  // - axe Y diriger vers le bas telescope en position horizontal
  // - axe Z le plus horizontal possible, l'algorithme soit pouvoir 
  // corriger une erreur concernant l'angle ( axe z - horizontal)
  //------------------------------------------------------
  
  vx = cosf( ax1 * 180 / ( 16384 * M_PI)) ;
  vy = cosf( ay1 * 360.0 / ( 65536.0 * 2.0 * M_PI )) ;
  vz = cosf( az1 * 360.0 / ( 65536.0 * 2.0 * M_PI )) ;
  
  printf("%.0f\t%.0f\t%.0f\n", vx, vy, vz );
  
  return 0;
}
