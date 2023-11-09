/* #define _GNU_SOURCE */

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | * ajout entete
#               * ajout entree chemins de config.txt
# -------------------------------------------------------------- 
*/

#include "astro_cat.h"

MACRO_ASTRO_GLOBAL_EXTERN_STRUCT ;
MACRO_ASTRO_GLOBAL_EXTERN_STRUCT_PARAMS ;
MACRO_ASTRO_GLOBAL_EXTERN_GPIOS ;

/*****************************************************************************************
* @fn     : CAT_DISPLAY_PREPARE
* @author : s.gravois
* @brief  : Fonction qui formate les donnees a afficher pour la fct DISPLAY
* @param  : STRUCT_CAT *
* @date   : 2023-01-08 creation
*****************************************************************************************/

static void CAT_DISPLAY_PREPARE ( STRUCT_CAT * lp_Cat) {

  TraceArbo(__func__,2,"astre format display") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Cat->cat_mutex ) ;

  sprintf( lp_Cat->cat_dis_cmd , STR_CAT_FORMAT_0,\
    lp_Cat->cat_path) ;

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Cat->cat_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static CAT_DISPLAY
* @author : s.gravois
* @brief  : Cette fonction affiche les informations sur astre sur commande
* @param  : STRUCT_CAT *
* @date   : 2023-01-07 creation 
*****************************************************************************************/

static void CAT_DISPLAY(STRUCT_CAT *lp_Cat) {

  TraceArbo(__func__,2,"display informations on Astre") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  CAT_DISPLAY_PREPARE( lp_Cat ) ;

  ASTRO_GLOBAL_LOG_ON ( &  lp_Cat->cat_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Cat->cat_loglevel , 1 , "%s", lp_Cat->cat_dis_cmd ) ;
  ASTRO_GLOBAL_LOG_OFF( &  lp_Cat->cat_loglevel ) ;

  return ;
}
/*****************************************************************************************
* @fn     : static CAT_LOG
* @author : s.gravois
* @brief  : Cette fonction log les informations de la structure concernee
* @param  : STRUCT_CAT *
* @date   : 2023-01-09 creation 
*****************************************************************************************/

static void CAT_LOG(STRUCT_CAT *lp_Cat) {

  TraceArbo(__func__,2,"display informations on Catalogue") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  CAT_DISPLAY_PREPARE( lp_Cat ) ;

  ASTRO_GLOBAL_LOG_ON ( &  lp_Cat->cat_loglevel ) ;
  MACRO_ASTRO_GLOBAL_LOG    ( lp_Cat->cat_loglevel , 1 , "%s", lp_Cat->cat_dis_cmd ) ;
  ASTRO_GLOBAL_LOG_OFF( &  lp_Cat->cat_loglevel ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CAT_LOCK
* @author : s.gravois
* @brief  : Lock le mutex de la structure en parametre
* @param  : STRUCT_CAT *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void CAT_LOCK ( STRUCT_CAT * lp_Cat) {

  TraceArbo(__func__,2,"lock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Cat->cat_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CAT_UNLOCK
* @author : s.gravois
* @brief  : Unlock le mutex de la structure en parametre
* @param  : STRUCT_CAT *
* @date   : 2022-12-20 creation
*****************************************************************************************/

void CAT_UNLOCK ( STRUCT_CAT * lp_Cat) {

  TraceArbo(__func__,2,"unlock mutex") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Cat->cat_mutex ) ;

  return ;
}
/*****************************************************************************************
* @fn     : CAT_FIN_MOT
* @author : s.gravois
* @brief  : Cette fonction definit le caractere de fin d un mot 
* @param  : char c
* @date   : 2022-10-14 creation
* @todo   : ras
*****************************************************************************************/

int CAT_FIN_MOT(char c) {
  int i=(int)c ;
  int j ;
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  j=0;
  if ( i==59)j=1;  // point virgule
  return j ; 
}
/*****************************************************************************************
* @fn     : CAT_INIT
* @author : s.gravois
* @brief  : Cette fonction initialise la structure de catalogues
* @param  : char c
* @date   : 2022-11-21 creation
* @todo   : inclure le path du cataogue dans la structure pour fct lecture +loin
*****************************************************************************************/

void CAT_INIT (STRUCT_CAT * lp_Cat ) {
 
  TraceArbo(__func__,0,"init catalogues") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_INIT( & lp_Cat->cat_mutex ) ;
                                     lp_Cat->cat_lock     = CAT_LOCK ;
                                     lp_Cat->cat_unlock   = CAT_UNLOCK ;
                                     lp_Cat->cat_log      = CAT_LOG ;
                                     lp_Cat->cat_display  = CAT_DISPLAY ;
                                     lp_Cat->cat_loglevel = 0 ; 
                                     lp_Cat->cat_file     = NULL ;
  gettimeofday ( &                   lp_Cat->cat_tval, NULL ) ;
  
  for(int L=0;L<CAT_NB_LIGNES;L++) {
    for(int C=0;C<CAT_NB_COLONNES;C++) {
      memset(lp_Cat->cat_dat[L][C], CONFIG_ZERO_CHAR, sizeof(lp_Cat->cat_dat[L][C]));
      memset(lp_Cat->cat_dec[L][C], CONFIG_ZERO_CHAR, sizeof(lp_Cat->cat_dat[L][C]));
    }
  }

  memset(lp_Cat->cat_path, CONFIG_ZERO_CHAR, sizeof( lp_Cat->cat_path));

  return ;
}
/*****************************************************************************************
* @fn     : CAT_DISPLAY_DAT
* @author : s.gravois
* @brief  : Cette fonction affiche le catalogue "datas" passe en parametre
* @param  : STRUCT_CAT * lp_Cat 
* @date   : 2022-10-14 creation
* @date   : 2022-22-13 modification suite a passage struct STR_CAT
* @todo   : fusionner les fcts CAT_DISPLAY_DAT & CAT_DISPLAY_DEC
*****************************************************************************************/

void CAT_DISPLAY_DAT(STRUCT_CAT * lp_Cat ) {

  int l, c ;
  char buffer[ CAT_TAILLE_BUFFER * CAT_NB_COLONNES ] ;
  /* modif stgrv 01/2022 : avoid -Wrestrict passing pointers */ 
  char buffer_recopie [ CAT_TAILLE_BUFFER * CAT_NB_COLONNES ] ;
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  l=0 ;
  while( strcmp( lp_Cat->cat_dat[l][3], "_" ) ) {
   memset( buffer,CONFIG_ZERO_CHAR, sizeof(buffer)) ;
   for(c=0;c<CAT_NB_COLONNES;c++) {
    /* modif stgrv 01/2022 : avoid -Wrestrict passing pointers */ 
    memset( buffer_recopie, 0, sizeof(buffer_recopie) ) ;
    strcpy( buffer_recopie, buffer ) ;
    /* sprintf( buffer, "%-10s %-10s", buffer, lp_Cat->cat_dat[l][c] ) ; */
    sprintf( buffer, "%-10s %-10s", buffer_recopie, lp_Cat->cat_dat[l][c] ) ; 
   }
   Trace1("%5d = %s ",l,buffer) ;
   l++;
   if( l>=CAT_NB_LIGNES) break ;
  }
}
/*****************************************************************************************
* @fn     : CAT_DISPLAY_DEC
* @author : s.gravois
* @brief  : Cette fonction affiche le catalogue "decimal" passe en parametre
* @param  : STRUCT_CAT * lp_Cat 
* @date   : 2022-22-13 creation
* @todo   : fusionner les fcts CAT_DISPLAY_DAT & CAT_DISPLAY_DEC
*****************************************************************************************/

void CAT_DISPLAY_DEC(STRUCT_CAT * lp_Cat ) {

  int l, c ;
  char buffer[ CAT_TAILLE_BUFFER * CAT_NB_COLONNES ] ;
  /* modif stgrv 01/2022 : avoid -Wrestrict passing pointers */ 
  char buffer_recopie [ CAT_TAILLE_BUFFER * CAT_NB_COLONNES ] ;
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  l=0 ;
  while( strcmp( lp_Cat->cat_dec[l][3], "_" ) ) {
   memset( buffer,CONFIG_ZERO_CHAR, sizeof(buffer)) ;
   for(c=0;c<CAT_NB_COLONNES;c++) {
    /* modif stgrv 01/2022 : avoid -Wrestrict passing pointers */ 
    memset( buffer_recopie, 0, sizeof(buffer_recopie) ) ;
    strcpy( buffer_recopie, buffer ) ;
    /* sprintf( buffer, "%-10s %-10s", buffer, lp_Cat->cat_dac[l][c] ) ; */
    sprintf( buffer, "%-10s %-10s", buffer_recopie, lp_Cat->cat_dec[l][c] ) ; 
   }
   Trace1("%5d = %s ",l,buffer) ;
   l++;
   if( l>=CAT_NB_LIGNES) break ;
  }
}

/*****************************************************************************************
* @fn     : CAT_READ
* @author : s.gravois
* @brief  : Cette fonction lit le catalogue "datas" passe en parametre
* @param  : STRUCT_CAT * lp_Cat 
* @date   : 2022-22-13 creation entete doxygen
* @todo   : passer char * lc_file_name dans la structure et faire une fct dediee
*****************************************************************************************/

void CAT_READ(STRUCT_CAT * lp_Cat, char * lc_file_name) {

  FILE * fin ;
  char buffer[CAT_TAILLE_BUFFER * CAT_NB_COLONNES] ;
  int  C,L ;
  char *str1, *token, *sptr ;
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Cat->cat_mutex ) ;

  for(L=0;L<CAT_NB_LIGNES;L++)
    for(C=0;C<CAT_NB_COLONNES;C++) {
      memset( lp_Cat->cat_dat[L][C], CONFIG_ZERO_CHAR, sizeof( lp_Cat->cat_dat[L][C] ));
      strcpy( lp_Cat->cat_dat[L][C], "_") ;
    }
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Cat->cat_mutex ) ;

  memset(buffer,CONFIG_ZERO_CHAR, sizeof(buffer) );
  sprintf(buffer,"%s/%s/%s",gp_Con_Par->con_par_rep_home,gp_Con_Par->con_par_rep_cat,lc_file_name) ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Cat->cat_mutex ) ;

  if ( lp_Cat->cat_file != NULL ) {

  }

  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Cat->cat_mutex ) ;

  if ( (fin=fopen(buffer,"r")) == NULL)  {
    // completer et modifier
    SyslogErrFmt("probleme ouverture 0 %s\n",buffer) ;
    Trace("probleme ouverture 0 %s",buffer) ;
    exit(2) ;
  }
  else Trace1("open %s ok", buffer) ;

  L=0;C=0;
  
  while( fgets( buffer, CAT_TAILLE_BUFFER * CAT_NB_COLONNES , fin ) != NULL) {
    for (C = 0, str1 = buffer ; ; C++, str1 = NULL) {
      token = strtok_r(str1, ";", &sptr);
      if (token == NULL) break ;
      strcpy( lp_Cat->cat_dat[L][C],token);
    }
    L++;
    if( L>=CAT_NB_LIGNES) break ;
  }
  fclose(fin);

  return ;
}
/*****************************************************************************************
* @fn     : CAT_ZONE
* @author : s.gravois
* @brief  : Cette fonction recherche les objets dans un cercle autour de l astre en parametre
* @param  : STRUCT_CAT * lp_Cat 
* @param  : STRUCT_ASTRE * gp_Ast 
* @date   : 2022-22-13 creation entete doxygen
* @todo   : passer char * lc_file_name dans la structure et faire une fct dediee
*****************************************************************************************/

void CAT_ZONE( STRUCT_CAT * lp_Cat, STRUCT_ASTRE *lp_Ast, double deg) {
  
  int    L,C ;
  double asc, asc_b  ;
  double dec, dec_b ;
  double d_angulaire ;
  double d_min ;
  
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  asc = lp_Ast->ast_agh0 ;
  dec = lp_Ast->ast_dec ;
  L=0 ;
  d_min=deg ;

  Trace1("Recherche dans la zone de %s : ASC=%f DEC=%f",\
    lp_Ast->ast_nom, \
    lp_Ast->ast_agh0, \
    lp_Ast->ast_dec) ;
  
  while( strcmp( lp_Cat->cat_dec[L][3],"_") \
      && strcmp( lp_Ast->ast_nom,lp_Cat->cat_dec[L][0]) \
      && strcmp( lp_Ast->ast_nom,lp_Cat->cat_dec[L][1])) {
   
    asc_b = atof( lp_Cat->cat_dec[L][2] ) ;
    dec_b = atof( lp_Cat->cat_dec[L][3] ) ;
    
    d_angulaire = sqrt(sqr( asc_b - asc ) + sqr ( dec_b -dec )) ;
    
    if (  d_angulaire < d_min )     // si objet dans le cercle recherche
    if ( deg > d_angulaire )  {
      // Trace1("dans la zone => %s=%s : ASC=%s DEC=%s DIST=%f",lp_Cat->cat_dec[L][0],lp_Cat->cat_dec[L][1],lp_Cat->cat_dec[L][2],lp_Cat->cat_dec[L][3], d_angulaire) ;
      if ( d_min > d_angulaire ) {  // Si objet encore plus proche trouve
        d_min = d_angulaire ;
        for(C=0;C<CAT_NB_COLONNES;C++) { 
          memset( lp_Ast->ast_plus_proche[C], CONFIG_ZERO_CHAR, sizeof( lp_Ast->ast_plus_proche[C] ));
          strcpy( lp_Ast->ast_plus_proche[C], lp_Cat->cat_dec[L][C]) ;
        }
      }
    }
    L++;
  }
  Trace1("Le plus proche => %s=%s : ASC=%s DEC=%s DIST=%f",\
  lp_Ast->ast_plus_proche[0], lp_Ast->ast_plus_proche[1], lp_Ast->ast_plus_proche[2], lp_Ast->ast_plus_proche[3], d_min) ;
}
/*****************************************************************************************
* @fn     : CAT_FIND
* @author : s.gravois
* @brief  : Cette fonction recherche l astre en parametre dans le catalogue 'decimal'
* @param  : STRUCT_CAT * lp_Cat 
* @date   : 2022-22-13 creation entete doxygen
* @todo   : passer char * lc_file_name dans la structure et faire une fct dediee
*****************************************************************************************/

void  CAT_FIND(STRUCT_CAT * lp_Cat, STRUCT_ASTRE *lp_Ast) {
  
  int    L=0 ;
  int    i_ligne=0 ;
  int    i_trouve =FALSE ;

  TraceArbo(__func__,0,"debut") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  Trace("recherche %s infos %s", lp_Ast->ast_nom, lp_Ast->ast_infos ) ;

  // dans les catalogues, coordonnnees en H et MIN pour ascension droite
  // et degres minutes pour declinaison
  // conversion en degres decimaux dans le catalogue aux valeurs deceimales
  // Dans tout le programme, coordonnees en radians
  
  L=0 ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex ) ;
  
  memset( lp_Ast->ast_infos, 0 , sizeof( lp_Ast->ast_infos) ) ;
  lp_Ast->ast_asc=0;
  lp_Ast->ast_dec=0 ;
  
  HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex ) ;

  i_ligne = L ;

  while( strcmp(lp_Cat->cat_dec[L][3],"_") && L < CAT_NB_LIGNES ) {
    
    Trace2("L=%d %s %s %s %s" , \
      L , \
      lp_Cat->cat_dec[L][0], \
      lp_Cat->cat_dec[L][1] , \
      lp_Cat->cat_dec[L][2] , \
      lp_Cat->cat_dec[L][3] );

    if(!strcmp(lp_Cat->cat_dec[L][0],lp_Ast->ast_nom)) {

      /* -----------------------------------------------
       * Sauvegarde des coordonnees equatoriales du catalogue 
       * dans la structure STRUCT_ASTRE 
       **************************************************/

      HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex ) ;

      lp_Ast->ast_asc = atof( lp_Cat->cat_dec[L][2] ) / CALCULS_UN_RADIAN_EN_DEGRES ;
      lp_Ast->ast_dec = atof( lp_Cat->cat_dec[L][3] ) / CALCULS_UN_RADIAN_EN_DEGRES ;

      memset( lp_Ast->ast_infos, 0 , sizeof( lp_Ast->ast_infos) ) ;
      strcpy( lp_Ast->ast_infos, lp_Cat->cat_dec[L][0] ) ;

      HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex ) ;

      i_ligne = L ;
      i_trouve = TRUE ;

      Trace("ligne %d : astre %s : cat asc %s cat dec %s",i_ligne, lp_Ast->ast_nom , lp_Cat->cat_dec[L][2] , lp_Cat->cat_dec[L][3] ) ;
      Trace("ligne %d : astre %s : ast asc %f ast dec %f infos %s",i_ligne, lp_Ast->ast_nom , lp_Ast->ast_asc , lp_Ast->ast_dec, lp_Ast->ast_infos) ;

      break  ;
    }

    if(!strcmp(lp_Cat->cat_dec[L][1],lp_Ast->ast_nom)) {

      HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex ) ;

      lp_Ast->ast_asc = atof( lp_Cat->cat_dec[L][2] ) / CALCULS_UN_RADIAN_EN_DEGRES ;
      lp_Ast->ast_dec = atof( lp_Cat->cat_dec[L][3] ) / CALCULS_UN_RADIAN_EN_DEGRES ;
      
      memset( lp_Ast->ast_infos, 0 , sizeof( lp_Ast->ast_infos) ) ;
      strcpy( lp_Ast->ast_infos, lp_Cat->cat_dec[L][1] ) ;

      HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex ) ;
      
      i_ligne = L ; 
      i_trouve = TRUE ;

      Trace("ligne %d : astre %s : cat asc %s cat dec %s",i_ligne, lp_Ast->ast_nom , lp_Cat->cat_dec[L][2] , lp_Cat->cat_dec[L][3] ) ;
      Trace("ligne %d : astre %s : ast asc %f ast dec %f infos %s",i_ligne, lp_Ast->ast_nom , lp_Ast->ast_asc , lp_Ast->ast_dec, lp_Ast->ast_infos) ;

      break ;
    }
    L++;
  }
  /* fin while*/

  if ( L < CAT_NB_LIGNES && i_trouve == TRUE ) {
    Trace("(trouve) : (nom) %s (asc) %-5.2f (dec) %-5.2f: ",lp_Ast->ast_nom , lp_Ast->ast_asc, lp_Ast->ast_dec  ) ;

    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex ) ;

    Trace("affectation lp_Ast->ast_new = TRUE") ;
    lp_Ast->ast_new = TRUE ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex ) ;

  }
  else {
    
    HANDLE_ERROR_PTHREAD_MUTEX_LOCK( & lp_Ast->ast_mutex ) ;

    lp_Ast->ast_asc = 0.0 ;
    lp_Ast->ast_dec = 0.0 ;

    strcpy( lp_Ast->ast_nom, "undefined" ) ;
    strcpy( lp_Ast->ast_infos, "undefined" ) ;

    HANDLE_ERROR_PTHREAD_MUTEX_UNLOCK( & lp_Ast->ast_mutex ) ;

    Trace(" %s : non trouve dans catalogue",lp_Ast->ast_nom) ;
  }

  TraceArbo(__func__,0,"fin") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  return ; 
}
/*****************************************************************************************
 * // Genere a la fois un catalogue avec des angles uniquement et au format decimal
// et aussi un fichier dans le repertoire des catalogues
//   format entree : 
      NGC ; 
      MESSIER (si existe sinon 0) ; 
      ASC (HH) ; 
      ASC (MM) ; 
      DEC (DEG) ; 
      DEG (MIN)
//   format sortie : 
      NGC ; 
      MESSIER (si existe sinon 0) ; 
      ASC (degres decimaux) ; 
      DEC (degres decimaux); 
      ASC (heures decimales) ; 
      DEC (heures decimales);     
*****************************************************************************************/

/*****************************************************************************************
* @fn     : CAT_FORMAT_DECIMAL_NGC
* @author : s.gravois
* @brief  : Genere un catalogue avec des angles uniquement et au format decimal
            pour le catalogue de type "ngc.txt"
* @param  : STRUCT_CAT * lp_Cat 
* @date   : 2022-22-13 creation entete doxygen
* @todo   : passer char * lc_file_name dans la structure et faire une fct dediee
*****************************************************************************************/

void CAT_FORMAT_DECIMAL_NGC(STRUCT_CAT * lp_Cat, char * lc_file_name) {
  
  FILE * fout ;
  char   buffer[CAT_TAILLE_BUFFER] ;
  int    C,L ;
  int    ASC_HH,ASC_MM ;   // la 3eme et 4eme colonne sont HH  et MM  de la declinaison
  int    DEC_DEG,DEG_MIN ; // la 5eme et 6eme colonne sont DEG et MIN de l'ascension droite
  double asc, dec ;        // ascension droite et declinaison sous forme decimale
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  for(L=0;L<CAT_NB_LIGNES;L++)
    for(C=0;C<CAT_NB_COLONNES;C++)  {
      memset(lp_Cat->cat_dec[L][C],CONFIG_ZERO_CHAR, sizeof(lp_Cat->cat_dec[L][C]) );
      strcpy(lp_Cat->cat_dec[L][C],"_") ;
  } 
  
  memset(buffer,CONFIG_ZERO_CHAR, sizeof( buffer) );
  sprintf(buffer,"%s/%s/%s",gp_Con_Par->con_par_rep_home,gp_Con_Par->con_par_rep_cat,lc_file_name) ;
  
  if ( (fout=fopen(buffer,"w")) == NULL) {
    // completer et modifier
    SyslogErrFmt("probleme ouverture 1 %s\n",buffer) ;
    Trace("probleme ouverture 0 %s",buffer) ;
    exit(2) ;
  }
  L=0 ;
  while( strcmp(lp_Cat->cat_dat[L][0],"_") ) {
   
   strcpy( lp_Cat->cat_dec[L][0], lp_Cat->cat_dat[L][0]) ;
   strcpy( lp_Cat->cat_dec[L][1], lp_Cat->cat_dat[L][1]) ;
   
   ASC_HH  = atoi( lp_Cat->cat_dat[L][2] ) ;
   ASC_MM  = atoi( lp_Cat->cat_dat[L][3] ) ;
   
   DEC_DEG = atoi( lp_Cat->cat_dat[L][4] ) ;
   DEG_MIN = atoi( lp_Cat->cat_dat[L][5] ) ;
   
   asc = ( (double)ASC_HH + SGN(ASC_HH) *(double)ASC_MM  / 60.0) * 15.0 ; // conversion en degres decimaux de type double
   dec =  (double)DEC_DEG + SGN(DEC_DEG)*(double)DEG_MIN / 60.0 ;
   
   sprintf(lp_Cat->cat_dec[L][2],"%f",asc) ;
   sprintf(lp_Cat->cat_dec[L][3],"%f",dec) ;
   
   asc = asc / 15 ;
   dec = dec / 15 ;
   
   sprintf(lp_Cat->cat_dec[L][4],"%f",asc) ;
   sprintf(lp_Cat->cat_dec[L][5],"%f",dec) ;
   
   fprintf(fout,"%s;%s;%s;%s;%s;%s\n",\
     lp_Cat->cat_dec[L][0], \
     lp_Cat->cat_dec[L][1], \
     lp_Cat->cat_dec[L][2], \
     lp_Cat->cat_dec[L][3], \
     lp_Cat->cat_dec[L][4], \
     lp_Cat->cat_dec[L][5]) ;   
   L++ ;
   if( L>=CAT_NB_LIGNES) break ;
  }
  fclose(fout) ;
}
/*****************************************************************************************
Genere a la fois un catalogue avec des angles uniquement et au format decimal
et aussi un fichier dans le repertoire des catalogues
   format entree : NOM (etoile) ; ASC (HH) ; ASC(MM) ; DEC(degres decimaux); 
   format sortie : NUMERO ; NOM (etoile) ; ASC (degres decimaux) ; DEC(degres decimaux);     
*****************************************************************************************/

/*****************************************************************************************
* @fn     : CAT_FORMAT_DECIMAL_NGC
* @author : s.gravois
* @brief  : Genere un catalogue avec des angles uniquement et au format decimal
            pour le catalogue de type "ngc.txt"
* @param  : STRUCT_CAT * lp_Cat 
* @date   : 2022-22-13 creation entete doxygen
* @todo   : passer char * lc_file_name dans la structure et faire une fct dediee
*****************************************************************************************/

void CAT_FORMAT_DECIMAL_ETO(STRUCT_CAT * lp_Cat, char * lc_file_name ) {
  
  FILE * fout ;
  char   buffer[CAT_TAILLE_BUFFER] ;
  int    C,L ;
  int    ASC_HH,ASC_MM ;   // la 3eme et 4eme colonne sont HH  et MM  de la declinaison
  double asc, dec ;        // ascension droite et declinaison sous forme decimale
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  for(L=0;L<CAT_NB_LIGNES;L++)
    for(C=0;C<CAT_NB_COLONNES;C++)  {
      memset(lp_Cat->cat_dec[L][C],CONFIG_ZERO_CHAR, sizeof(buffer) );
      strcpy(lp_Cat->cat_dec[L][C],"_") ;
  } 
  
  memset(buffer,CONFIG_ZERO_CHAR, sizeof(buffer));
  sprintf(buffer,"%s/%s/%s",gp_Con_Par->con_par_rep_home,gp_Con_Par->con_par_rep_cat,lc_file_name) ;
  
  if ( (fout=fopen(buffer,"w")) == NULL) {
    // completer et modifier
    SyslogErrFmt("probleme ouverture 2 %s\n",buffer) ;
    Trace("probleme ouverture 0 %s",buffer) ;
    exit(2) ;
  }
  L=0 ;
  while( strcmp(lp_Cat->cat_dat[L][0],"_") ) {
   
   strcpy(  lp_Cat->cat_dec[L][0], lp_Cat->cat_dat[L][0]) ;
   strcpy(  lp_Cat->cat_dec[L][1], lp_Cat->cat_dat[L][1]) ;
   
   ASC_HH  = atoi( lp_Cat->cat_dat[L][2] ) ;
   ASC_MM  = atoi( lp_Cat->cat_dat[L][3] ) ;
   
   dec = atof( lp_Cat->cat_dat[L][4] ) ;
   //DEG_MIN = atoi( lp_Cat->cat_dat[L][5] ) ;
   
   asc = ( ASC_HH + ( ASC_MM / 60.0 ) )   * 15.0 ; // conversion en degres decimaux de type double
   
   Trace1("aschh=%d\tascmm=%d\tdec=%f\tasc_calculer=%f", ASC_HH,ASC_MM,dec,asc) ;
   
   //dec =  (double)DEC_DEG + SGN(DEC_DEG)*(double)DEG_MIN / 60.0 ;
   
   sprintf(lp_Cat->cat_dec[L][2],"%f",asc) ;
   sprintf(lp_Cat->cat_dec[L][3],"%f",dec) ;
   
   asc = asc / 15 ;
   dec = dec / 15 ;
   
   sprintf(lp_Cat->cat_dec[L][4],"%f",asc) ;
   sprintf(lp_Cat->cat_dec[L][5],"%f",dec) ;
   
   fprintf(fout,"%s;%s;%s;%s;%s;%s\n",\
     lp_Cat->cat_dec[L][0], \
     lp_Cat->cat_dec[L][1], \
     lp_Cat->cat_dec[L][2], \
     lp_Cat->cat_dec[L][3], \
     lp_Cat->cat_dec[L][4], \
     lp_Cat->cat_dec[L][5]) ;   
   L++ ;
   if( L>=CAT_NB_LIGNES) break ;
  }
  fclose(fout) ;
}
//============================================================================




