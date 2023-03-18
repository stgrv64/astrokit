/* General definitions for aa program */

#include <stdio.h>
#include <string.h>

#define DIURPX_MIN_DIST_TO_CONSIDER 1758.8

#define ASTRO_LOG_DEBUG_ARBO_APPEL_FCTS  0 /* valeurs possibles => 0    : fonctions de base , > 0 : suivantes  */
#define DEBUG 0

// ------------------------------------------------------------------------
// TraceArbo : traces suivants le niveau d arborescence en parametre ..
// exemple :   TraceArbo(__func__,0,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
// ------------------------------------------------------------------------

#define TraceArbo(fct,nb,ch) \
do { \
  if (nb<=ASTRO_LOG_DEBUG_ARBO_APPEL_FCTS) { \
    if ( strlen(ch) < 2 ) { \
      if ( nb == 1 ) fprintf(stdout, "\n%-36s : ----> (%s)", __func__, ch) ; \
      if ( nb == 2 ) fprintf(stdout, "\n%-34s-- : ----> (%s)", __func__, ch) ; \
      if ( nb == 3 ) fprintf(stdout, "\n%-32s---- : ----> (%s)", __func__, ch) ; \
      if ( nb == 4 ) fprintf(stdout, "\n%-30s------ : ----> (%s)", __func__, ch) ; \
    } else  { \
      if ( nb == 1 ) fprintf(stdout, "\n%-36s : ----> (%s)", __func__, ch) ; \
      if ( nb == 2 ) fprintf(stdout, "\n%-34s-- : ----> (%s)", __func__, ch) ; \
      if ( nb == 3 ) fprintf(stdout, "\n%-32s---- : ----> (%s)", __func__, ch) ; \
      if ( nb == 4 ) fprintf(stdout, "\n%-30s------ : ----> (%s)", __func__, ch) ; \
    } \
  } \
} \
while (0)

#ifndef DEBUG
#define Trace(fmt, args...)  while(0) {  fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#define Trace1(fmt, args...) while(0) {  fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#define Trace2(fmt, args...) while(0) {  fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#endif

#if defined(DEBUG) && DEBUG == 0 
#define Trace(fmt, args...)              fprintf(stderr, "\n%-20s : " fmt, __func__, ##args)
#define Trace1(fmt, args...) while(0) {  fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#define Trace2(fmt, args...) while(0) {  fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#endif

#if defined(DEBUG) && DEBUG == 1
#define Trace(fmt, args...)              fprintf(stderr, "\n%s : " fmt, __func__, ##args)
#define Trace1(fmt, args...){            fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#define Trace2(fmt, args...) while(0) {  fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#endif

#if defined(DEBUG) && DEBUG == 2
#define Trace(fmt, args...)              fprintf(stderr, "\n%s:%d:%s() : " fmt, __FILE__,__LINE__, __func__, ##args)
#define Trace1(fmt, args...)             fprintf(stderr, "\n%s : " fmt, __func__, ##args) 
#define Trace2(fmt, args...)             fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; 
#endif

typedef struct   {
  double asc ;
  double dec ;
  double azi ;
  double alt ;
  char   nom[32] ;
} 
TOPOCENTRIC ;

