
/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 2022-10-08  | * creation
# 2022-10-08  | * rapatriement code depuis astro_global.h et autre
# -------------------------------------------------------------- 
*/

#ifndef ASTRO_LOG_H
#define ASTRO_LOG_H

#include "astro_global.h"

#define DEBUG       0
#define DEBUG_ARBO  2
#define DEBUG_LOG   0

// quelques macros de debugging

#ifndef DEBUG
  #define TRACE(fmt, args...)      while(0) { } 
  #define TRACE1(fmt, args...)     while(0) { } 
  #define TRACE2(fmt, args...)     while(0) { } 
#endif

// ------------------------------------------------------------------------
// Raccourci
// ----------------------------------------------------------------

#define If_Mot_Is(s)    if(!strcmp(gp_Key->mot,s))

// ------------------------------------------------------------------------
// Syslog
// ----------------------------------------------------------------------

#define SyslogInf(string0) { syslog( LOG_INFO,    "%s : ligne %d : %s\n", __func__, __LINE__ ,string0); }
#define SyslogErr(string0) { syslog( LOG_ERR,     "%s : ligne %d : %s\n", __func__, __LINE__ ,string0); }
#define SyslogWar(string0) { syslog( LOG_WARNING, "%s : ligne %d : %s\n", __func__, __LINE__ ,string0); }
#define SyslogEno(string0) { syslog( LOG_ERR,     "%s : ligne %d : %s : %s\n", __func__, __LINE__ ,strerror(errno), string0 ); }
#define SyslogErrFmt(fmt, args...) { syslog( LOG_ERR,"%s %s " fmt, __func__, strerror(errno), ##args) ; }

// ------------------------------------------------------------------------
// Exemple de MACRO avec expression
// le do while semble necessaire .. 
// ---------------------------------------------------------------------

/* #define ARBO(__func__,EXP) do { if (EXP) fprintf (stdout, "Warning: \n"); } while (0) */
/* #define ARBO(__func__,fmt, EXP, args...) do { if (EXP) fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; } while (0) */
#define ARBO_TEST(EXP,string) do { if (EXP) fprintf(stderr, "\n%-30s : %s", __func__, string) ; } while (0)

#define ARBO(fct,EXP,ch) \
do { \
  if (EXP<=DEBUG_ARBO) { \
    if ( EXP == 0 ) fprintf(stderr, "\n%-20s : %-25s", __func__, ch) ; \
    if ( EXP == 1 ) fprintf(stderr, "\n%-30s : %-27s", __func__, ch) ; \
    if ( EXP == 2 ) fprintf(stderr, "\n%-30s : %-29s", __func__, ch) ; \
    if ( EXP == 3 ) fprintf(stderr, "\n%-30s : %-31s", __func__, ch) ; \
  } \
} \
while (0)

// ------------------------------------------------------------------------
// Niveau -1 
// ------------------------------------------------------------------------

#if defined(DEBUG) && DEBUG < 0

#define Debug(fmt, args...) if(g_i_trace) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }

#define Trace(fmt, args...) while(0) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }
#define Trace1(fmt, args...) while(0) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }
#define Trace2(fmt, args...) while(0) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }

#define TRACE(fmt, args...) while(0) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }
#define TRACE1(fmt, args...) while(0) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; } 
#define TRACE2(fmt, args...) while(0) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }

#endif

// ------------------------------------------------------------------------
// Niveau 0 
// ------------------------------------------------------------------------

#if defined(DEBUG) && defined(DEBUG_LOG) && DEBUG == 0 && DEBUG_LOG < 1

#define Trace(fmt, args...)             { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }
#define Debug(fmt, args...) if(g_i_trace) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }
#define Trace1(fmt, args...) while(0) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }
#define Trace2(fmt, args...) while(0) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }
#define TRACE(fmt, args...)           { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }
#define TRACE1(fmt, args...) while(0) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; } 
#define TRACE2(fmt, args...) while(0) { fprintf(stderr, "\n%s\t:" fmt, __func__, ##args) ; }

#endif

#if defined(DEBUG) && defined(DEBUG_LOG) && DEBUG == 0 && DEBUG_LOG >= 1

#define Trace(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define TRACE(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%s : " fmt, __func__, ##args) ; } 
#define TRACE1(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%s : " fmt, __func__, ##args) ; } 
#define TRACE2(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%s : " fmt, __func__, ##args) ; }

#endif

// ------------------------------------------------------------------------
// Niveau 1
// ------------------------------------------------------------------------

#if defined(DEBUG) && defined(DEBUG_LOG) && DEBUG == 1 && DEBUG_LOG < 1

#define Trace(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define TRACE(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define TRACE1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#define TRACE2(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }

#endif

#if defined(DEBUG) && defined(DEBUG_LOG) && DEBUG == 1 && DEBUG_LOG >= 1

#define Trace(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define TRACE(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%s : " fmt, __func__, ##args) ; } 
#define TRACE1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%s : " fmt, __func__, ##args) ; } 
#define TRACE2(fmt, args...) while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%s : " fmt, __func__, ##args) ; }

#endif

// ------------------------------------------------------------------------
// Niveau 2 
// ------------------------------------------------------------------------

#if defined(DEBUG) && defined(DEBUG_LOG) && DEBUG == 2 && DEBUG_LOG < 1

#define Trace(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define TRACE(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define TRACE1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#define TRACE2(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }

#endif

#if defined(DEBUG) && defined(DEBUG_LOG) && DEBUG == 2 && DEBUG_LOG >= 1

#define Trace(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#define TRACE(fmt, args...)           { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%s : " fmt, __func__, ##args) ; } 
#define TRACE1(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%s : " fmt, __func__, ##args) ; } 
#define TRACE2(fmt, args...)          { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%s : " fmt, __func__, ##args) ; }

#endif

void   LOG_INIT         ( void )  ;
void   LOG_SYSTEM_LOG_0 ( int * ) ;
void   LOG_SYSTEM_LOG_1 ( char * )  ;

#endif