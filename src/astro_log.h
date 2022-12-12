
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

#define ASTRO_LOG_DEBUG                  0 /* valeurs possibles => 0 : pas de traces  , > 0 : traces progressives*/
#define ASTRO_LOG_DEBUG_PID              0 /* valeurs possibles => 0 : pas de traces  ,   1 : traces */
#define ASTRO_LOG_DEBUG_WRITE_FS         0 /* valeurs possibles => 0 : pas d ecriture ,   1 : ecriture */
#define ASTRO_LOG_DEBUG_ARBO_APPEL_FCTS  0 /* valeurs possibles => 0 : fcontions de base , > 0 : suivantes */
/* gp_File_Flog */

// ------------------------------------------------------------------------
// Raccourci
// ----------------------------------------------------------------

#define KEYS_If_Mot_Is(s)  if(!strcmp(gp_Key->key_mot,s))

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
// ------------------------------------------------------------------------

/* #define TraceArbo(__func__,exp) do { if (exp) fprintf (stdout, "Warning: \n"); } while (0) */
/* #define TraceArbo(__func__,fmt, exp, args...) do { if (exp) fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; } while (0) */
#define ARBO_TEST(exp,string) \
do { \
    if (exp) fprintf(stdout, "\n%-30s : %s", __func__, string) ; } \
while (0)

// ------------------------------------------------------------------------
// TraceArbo : traces suivants le niveau d arborescence en parametre ..
// exemple :   TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
// ------------------------------------------------------------------------

#define TraceArbo(fct,nb,ch) \
do { \
  if (nb<=ASTRO_LOG_DEBUG_ARBO_APPEL_FCTS) { \
    if ( nb == 0 ) fprintf(stdout, "\n%-36s : %-50s", __func__, ch) ; \
    if ( nb == 1 ) fprintf(stdout, "\n%-34s-- : %-50s", __func__, ch) ; \
    if ( nb == 2 ) fprintf(stdout, "\n%-32s---- : %-50s", __func__, ch) ; \
    if ( nb == 3 ) fprintf(stdout, "\n%-30s------ : %-50s", __func__, ch) ; \
  } \
} \
while (0)

// ------------------------------------------------------------------------
// TracePid : Niveau 0 : pas de traces 
// ------------------------------------------------------------------------

#define TracePid(fmt, args...) if(gi_pid_trace) { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }
#define TracePid(fmt, args...) if(gi_pid_trace) { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }

// ------------------------------------------------------------------------
// ASTRO_LOG_DEBUG : Niveau 0 : pas de traces 
// ------------------------------------------------------------------------

#if defined(ASTRO_LOG_DEBUG) && ASTRO_LOG_DEBUG < 0

#define Trace(fmt, args...)  while(0) { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...) while(0) { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...) while(0) { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }

#endif

// ------------------------------------------------------------------------
// ASTRO_LOG_DEBUG : Niveau 0
// ------------------------------------------------------------------------

#if defined(ASTRO_LOG_DEBUG) && defined(ASTRO_LOG_DEBUG_WRITE_FS) && ASTRO_LOG_DEBUG == 0 && ASTRO_LOG_DEBUG_WRITE_FS == 0

#define Trace(fmt, args...)           { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...) while(0) { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...) while(0) { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }

#endif

#if defined(ASTRO_LOG_DEBUG) && defined(ASTRO_LOG_DEBUG_WRITE_FS) && ASTRO_LOG_DEBUG == 0 && ASTRO_LOG_DEBUG_WRITE_FS == 1

#define Trace(fmt, args...)           { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...) while(0) { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...) while(0) { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%-36s : " fmt, __func__, ##args) ; }

#endif

// ------------------------------------------------------------------------
// ASTRO_LOG_DEBUG : Niveau 1
// ------------------------------------------------------------------------

#if defined(ASTRO_LOG_DEBUG) && defined(ASTRO_LOG_DEBUG_WRITE_FS) && ASTRO_LOG_DEBUG == 1 && ASTRO_LOG_DEBUG_WRITE_FS == 0

#define Trace(fmt, args...)           { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...)          { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...) while(0) { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }

#endif

#if defined(ASTRO_LOG_DEBUG) && defined(ASTRO_LOG_DEBUG_WRITE_FS) && ASTRO_LOG_DEBUG == 1 && ASTRO_LOG_DEBUG_WRITE_FS == 1

#define Trace(fmt, args...)           { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...)          { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...) while(0) { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%-36s : " fmt, __func__, ##args) ; }

#endif

// ------------------------------------------------------------------------
// ASTRO_LOG_DEBUG : Niveau 3
// ------------------------------------------------------------------------

#if defined(ASTRO_LOG_DEBUG) && defined(ASTRO_LOG_DEBUG_WRITE_FS) && ASTRO_LOG_DEBUG == 2 && ASTRO_LOG_DEBUG_WRITE_FS == 0

#define Trace(fmt, args...)           { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...)          { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...)          { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; }

#endif

#if defined(ASTRO_LOG_DEBUG) && defined(ASTRO_LOG_DEBUG_WRITE_FS) && ASTRO_LOG_DEBUG == 2 && ASTRO_LOG_DEBUG_WRITE_FS == 1

#define Trace(fmt, args...)           { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace1(fmt, args...)          { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%-36s : " fmt, __func__, ##args) ; }
#define Trace2(fmt, args...)          { fprintf(stdout, "\n%-36s : " fmt, __func__, ##args) ; fprintf(gp_File_Flog, "\n%-36s : " fmt, __func__, ##args) ; }

#endif

#define HANDLE_ERROR_PTHREAD_MUTEX_INIT(string) { \
 int i_error=0 ; \
 int i_errno=0 ; \
 if ( string == NULL ) { \
  i_errno=errno; \
  Trace("i_error : %d i_errno : %d", i_error, i_errno ) ; \
  perror("pointer null"); \
  exit(EXIT_FAILURE); } \
 if ( ( i_error = pthread_mutex_init( string, NULL )) != 0 ) { \
  i_errno=errno; \
  Trace("i_error : %d i_errno : %d", i_error, i_errno ) ; \
  perror("pthread_mutex_init"); \
  exit(EXIT_FAILURE); }}

#define HANDLE_ERROR_PTHREAD_MUTEX_LOCK(string) { \
 int i_error=0 ; \
 int i_errno=0 ; \
 if ( string == NULL ) { \
  i_errno=errno; \
  Trace("i_error : %d i_errno : %d", i_error, i_errno ) ; \
  perror("pointer null"); \
  exit(EXIT_FAILURE); } \
 if ( ( i_error = pthread_mutex_lock( string )) != 0 ) { \
  i_errno=errno; \
  Trace("i_error : %d i_errno : %d", i_error, i_errno ) ; \
  perror("pthread_mutex_lock"); \
  exit(EXIT_FAILURE); }}

#define HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK(string) { \
 int i_error=0 ; \
 int i_errno=0 ; \
 if ( string == NULL ) { \
  i_errno=errno; \
  Trace("i_error : %d i_errno : %d", i_error, i_errno ) ; \
  perror("pointer null"); \
  exit(EXIT_FAILURE); } \
 if ( ( i_error = pthread_mutex_unlock( string )) != 0 ) { \
  i_errno=errno; \
  Trace("i_error : %d i_errno : %d", i_error, i_errno ) ; \
  perror("pthread_mutex_unlock"); \
  exit(EXIT_FAILURE); }}

#define HANDLE_ERROR_PTHREAD_MUTEX_DESTROY(string) { \
 int i_error=0 ; \
 int i_errno=0 ; \
 if ( string == NULL ) { \
  i_errno=errno; \
  Trace("i_error : %d i_errno : %d", i_error, i_errno ) ; \
  perror("pointer null"); \
  exit(EXIT_FAILURE); } \
 if ( ( i_error = pthread_mutex_destroy( string )) != 0 ) { \
  i_errno=errno; \
  Trace("i_error : %d i_errno : %d", i_error, i_errno ) ; \
  perror("pthread_mutex_destroy"); \
  exit(EXIT_FAILURE); }}                                     

// ------------------------------------------------------------------------
// ASTRO_LOG_DEBUG : fin macros
// ------------------------------------------------------------------------

struct STR_LOG {
 pthread_mutex_t log_mutex ;
 int             log_level ;
} ;
typedef struct STR_LOG STRUCT_LOG ;

void   LOG_INIT         ( STRUCT_LOG * )  ;
void   LOG_TRACE        ( char *l_String , ... ) ;
void   LOG_SYSTEM_LOG_0 ( int * ) ;
void   LOG_SYSTEM_LOG_1 ( char * )  ;

#endif