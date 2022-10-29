/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# Ce fichier definit les types non specifiques 
# et utilises en tant que membres de structures
# Permet d'eviter les inclusions multiples 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-10-27  | creation
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_TYPES_H
#define ASTRO_TYPES_H

struct STR_ANGLE {
  char   c_si ; 
  double AD ;   
  double ad ;   
  int    si ;      
  int    DD ;      
  int    MM ;      
  int    SS ; 
} ;

typedef struct STR_ANGLE STRUCT_ANGLE ;

#endif

