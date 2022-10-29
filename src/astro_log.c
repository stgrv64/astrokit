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

  ARBO(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  (*incrlog)++ ;
  char cmd[255] ;
  if ( DEBUG_LOG ) {
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

  ARBO(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  memset( s_date, 0, sizeof( s_date)) ;
  memset( c_out, 0, sizeof( c_out)) ;
  
  if (tmp == NULL) {
    perror("localtime");
    exit(EXIT_FAILURE);
  }

  if ( DEBUG_LOG ) {
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

void LOG_INIT(void) {

  char buf[255] ;
  
  ARBO(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  Trace("") ;
  
  if ( DEBUG_LOG ) {
    
    memset(buf, CALCUL_ZERO_CHAR, sizeof(buf));
    sprintf(buf,"%s/%s/%s", gp_Con_Par->par_rep_home, gp_Con_Par->par_rep_log, gp_Con_Par->par_fic_log) ;
    
    if ( (gp_File_Flog=fopen(buf,"a")) == NULL) {
      // completer et modifier
      Trace("probleme ouverture 3 %s",buf) ;
      SyslogErrFmt("probleme ouverture %s", buf) ;
      exit(2) ;
    }
    else Trace("open %s ok", buf) ;
  }
  return ;
}
