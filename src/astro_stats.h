/* #define _GNU_SOURCE */

#ifndef ASTRO_STATS_H
#define ASTRO_STATS_H

#include "astro_global.h"

struct STR_SUIVI_STATS {
  pthread_mutex_t sta_mutex ;
  unsigned long sta_Ia ;              // nombre d'impulsions mesurees sur azimut
  unsigned long sta_Ih ;              // nombre d'impulsions mesureees sur altitude
  unsigned long sta_Ia_prec ;         // nombre d'impulsions mesurees sur azimut
  unsigned long sta_Ih_prec ;         // nombre d'impulsions mesureees sur altitude 
  unsigned long sta_Iat [STATS_ASS] ; // tableau des nombres d'impulsions mesurees
  unsigned long sta_Iht [STATS_ASS] ; // tableau des nombres d'impulsions mesurees
  double        sta_Tac ;             // correcteur de periode, pour corriger les effets des latences du systeme, calculer par suivi voute
  double        sta_Thc ;             // correcteur de periode, pour corriger les effets des latences du systeme, calculer par suivi voute
  double        sta_Tacc ;            // correcteur de correcteur de periode, pour corriger les insuffisances du correcteur de base 
  double        sta_Thcc ;            // correcteur de correcteur de periode, pour corriger les insuffisances du correcteur de base 
  double        sta_Ias ;             // somme sur nombres d'impulsions mesurees 
  double        sta_Ihs ;             // somme sur nombres d'impulsions mesurees 
} ;

typedef struct STR_SUIVI_STATS STRUCT_SUIVI_STATS ;

void STAT_MESURE_PULSATIONS_INJECTEES(STRUCT_SUIVI * gp_Sui) ;

#endif
