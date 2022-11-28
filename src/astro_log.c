/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2022  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 07/10/2022  | * creation
# -------------------------------------------------------------- 
*/

#include "astro_log.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

FILE * gp_File_Flog ; 

/*****************************************************************************************
* @fn     : LOG_SYSTEM_LOG_0
* @author : s.gravois
* @brief  : Cette fonction ecrit dans la log via un appel systeme avec system
* @param  : int *incrlog
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-10-07 deplacement fonction depuis config.c (renomme en astro_config.c)
* @todo   : (obsolete)
*****************************************************************************************/

void LOG_SYSTEM_LOG_0(int *incrlog) {
  int ret ;

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  (*incrlog)++ ;
  char cmd[255] ;
  if ( ASTRO_LOG_DEBUG_WRITE_FS ) {
    // FIXME 01 mai 2021 : modification chemin relatif
    sprintf(cmd,"echo %d >> %s/%s/%s",*incrlog, gp_Con_Par->par_rep_home, gp_Con_Par->par_rep_log, gp_Con_Par->par_fic_log) ;
    ret = system(cmd) ;
    if ( ret < 0 ) Trace("Probleme avec %s : retourner avec error negative",cmd) ;
    //if ( ret == 0 ) Trace("Probleme avec %s : shell non disponible",cmd) ;
  }
}
/*****************************************************************************************
* @fn     : LOG_SYSTEM_LOG_1
* @author : s.gravois
* @brief  : Cette fonction ecrit dans la log via un strtime et system
* @param  : int *incrlog
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-10-07 deplacement fonction depuis config.c (renomme en astro_config.c)
* @todo   : (obsolete)
*****************************************************************************************/
/* obsolete */

void LOG_SYSTEM_LOG_1(char *txt) {

  int ret ;
  char cmd[255] ;
  char c_out[255] ;
  time_t now = time (NULL);
  struct tm *tmp;
  tmp = localtime(&now);
  char s_date[16];

  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset( s_date, 0, sizeof( s_date)) ;
  memset( c_out, 0, sizeof( c_out)) ;
  
  if (tmp == NULL) {
    perror("localtime");
    exit(EXIT_FAILURE);
  }

  if ( ASTRO_LOG_DEBUG_WRITE_FS ) {
    if ( strftime(s_date, sizeof(s_date), "%Y%m%d.%H%M", tmp) == 0 ) {
        fprintf(stderr, "strftime a renvoyé 0");
        exit(EXIT_FAILURE);
    }
    sprintf( c_out, "%s : %s", s_date, txt ) ;
    sprintf( cmd,"echo %s >> %s/%s/%s",c_out, gp_Con_Par->par_rep_home, gp_Con_Par->par_rep_log, gp_Con_Par->par_fic_log) ;

    ret =  system(cmd) ;
    
    if ( ret < 0 ) { 
      Trace("Probleme avec %s : retourner avec error negative",cmd) ;
    }
    //if ( ret == 0 ) Trace("Probleme avec %s : shell non disponible",cmd) ;
  }
}
/*****************************************************************************************
* @fn     : LOG_INIT
* @author : s.gravois
* @brief  : Cette fonction ouvre le fichier de log defini par 
*           gp_Con_Par->par_rep_home / gp_Con_Par->par_rep_log / gp_Con_Par->par_fic_log
*           definis dans le fichier de configuration
* @param  : void
* @date   : 2022-01-20 creation entete de la fonction au format doxygen
* @date   : 2022-10-07 deplacement fonction depuis config.c (renomme en astro_config.c)
* @todo   : rappatrier syslog init ICI ?
*****************************************************************************************/

void LOG_INIT(STRUCT_LOG* lp_Log) {

  int i_error=0 ; 
  int i_errno=0 ;
  char buf[255] ;
  
  TraceArbo(__func__,0,"--------------") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("") ;
  
  if ( ASTRO_LOG_DEBUG_WRITE_FS ) {
    
    memset(buf, CALCULS_ZERO_CHAR, sizeof(buf));
    sprintf(buf,"%s/%s/%s", gp_Con_Par->par_rep_home, gp_Con_Par->par_rep_log, gp_Con_Par->par_fic_log) ;
    
    if ( (gp_File_Flog=fopen(buf,"a")) == NULL) {
      // completer et modifier
      Trace("probleme ouverture 3 %s",buf) ;
      SyslogErrFmt("probleme ouverture %s", buf) ;
      exit(2) ;
    }
    else Trace("open %s ok", buf) ;
  }

  if ( ( i_error = pthread_mutex_init( & lp_Log->log_mut, NULL )) != 0 ) {
    i_errno=errno;        
    Trace("i_error : %d i_errno : %d", i_error, i_errno ) ;
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE); 
  }

  lp_Log->log_level = ASTRO_LOG_DEBUG ;

  return ;
}

/*****************************************************************************************
* @fn     : LOG_TRACE
* @author : s.gravois
* @brief  : Fonction de trace dynamique qui utilise niveau de debug dynamiquement 
*           modifee par le clavier en cours d execution
* @param  : void
* @date   : 2022-11-03 creation 
* @todo   : rappatrier syslog init ICI ?
*****************************************************************************************/

void   LOG_TRACE  ( char *l_String , ... ) {
  va_list valist;
  va_start(valist, l_String);
	unsigned long long u=0 ;
	long long ll =0;
	long l=0;
	int i=0;
	char *s ;
	double f=0 ;
	char c=0 ;
	int d=0 ;
  char c_out [ CONFIG_TAILLE_BUFFER_256 ]={0};
  char c_out_tmp [ CONFIG_TAILLE_BUFFER_256 ]={0};

  memset( c_out,0, sizeof(c_out));

  va_start(valist, l_String);

  while (*l_String) {
    switch (*l_String++) {
      case 'l':
        l = va_arg(valist, long );
        strcpy( c_out_tmp, c_out ) ;
        sprintf(c_out,"%s %ld",c_out_tmp,l) ;
        break;

      case 'm':
        ll = va_arg(valist, long long);
        strcpy( c_out_tmp, c_out ) ;
        sprintf(c_out,"%s %lld",c_out_tmp,ll) ;
        break;

      case 'u':
        u = va_arg(valist, unsigned long long );
        strcpy( c_out_tmp, c_out ) ;
        sprintf(c_out,"%s %lld",c_out_tmp,u) ;
        break;

      case 'f':
        f = va_arg(valist, double);
        strcpy( c_out_tmp, c_out ) ;
        sprintf(c_out,"%s %f",c_out_tmp,f) ;
        break;

      case 's':
        s = va_arg(valist, char *);
        strcpy( c_out_tmp, c_out ) ;
        sprintf(c_out,"%s %s",c_out_tmp,s) ;
        break;

      case 'd':
        d = va_arg(valist, int);
        strcpy( c_out_tmp, c_out ) ;
        sprintf(c_out,"%s %d",c_out_tmp,d) ;
        break;

      case 'c':
        c = (char)va_arg(valist, int);
        strcpy( c_out_tmp, c_out ) ;
        sprintf(c_out,"%s %c",c_out_tmp,c) ;
        break;

      default:
          break;
    }
  }
  va_end(valist);

  Trace("%s",c_out);

  return ;
}
