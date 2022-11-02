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
* @fn     : CAT_AFFICHER
* @author : s.gravois
* @brief  : Cette fonction affiche le catalogue passe en parametre
* @param  : char c
* @date   : 2022-10-14 creation
* @todo   : ras
*****************************************************************************************/

void CAT_AFFICHER(char catalogue[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {
  int l, c ;
  char buffer[ CAT_TAILLE_BUFFER * CAT_NB_COLONNES ] ;
  /* modif stgrv 01/2022 : avoid -Wrestrict passing pointers */ 
  char buffer_recopie [ CAT_TAILLE_BUFFER * CAT_NB_COLONNES ] ;
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  l=0 ;
  while( strcmp( catalogue[l][3], "_" ) ) {
   memset( buffer,CALCUL_ZERO_CHAR, CAT_TAILLE_BUFFER * CAT_NB_COLONNES) ;
   for(c=0;c<CAT_NB_COLONNES;c++) {
    /* modif stgrv 01/2022 : avoid -Wrestrict passing pointers */ 
    memset( buffer_recopie, 0, sizeof(buffer_recopie) ) ;
    strcpy( buffer_recopie, buffer ) ;
    /* sprintf( buffer, "%-10s %-10s", buffer, catalogue[l][c] ) ; */
    sprintf( buffer, "%-10s %-10s", buffer_recopie, catalogue[l][c] ) ; 
   }
   Trace1("%5d = %s ",l,buffer) ;
   l++;
   if( l>=CAT_NB_LIGNES) break ;
  }
}
//============================================================================
void CAT_READ(char * catalogue_txt, char l_char_Datas[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {

  FILE * fin ;
  char buf[CAT_TAILLE_BUFFER * CAT_NB_COLONNES] ;
  int  C,L ;
  char *str1, *token, *sptr ;
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  for(L=0;L<CAT_NB_LIGNES;L++)
    for(C=0;C<CAT_NB_COLONNES;C++) {
      memset(l_char_Datas[L][C],CALCUL_ZERO_CHAR,CAT_TAILLE_BUFFER);
      strcpy(l_char_Datas[L][C],"_") ;
    }
  
  memset(buf,CALCUL_ZERO_CHAR,CAT_TAILLE_BUFFER * CAT_NB_COLONNES);
  sprintf(buf,"%s/%s/%s",gp_Con_Par->par_rep_home, gp_Con_Par->par_rep_cat,catalogue_txt) ;
  
  if ( (fin=fopen(buf,"r")) == NULL)  {
    // completer et modifier
    SyslogErrFmt("probleme ouverture 0 %s\n",buf) ;
    exit(2) ;
  }
  else Trace1("open %s ok", buf) ;

  L=0;C=0;
  
  while( fgets( buf, CAT_TAILLE_BUFFER * CAT_NB_COLONNES , fin ) != NULL) {
    for (C = 0, str1 = buf ; ; C++, str1 = NULL) {
      token = strtok_r(str1, ";", &sptr);
      if (token == NULL) break ;
      strcpy( l_char_Datas[L][C],token);
    }
    L++;
    if( L>=CAT_NB_LIGNES) break ;
  }
  fclose(fin);
}
//============================================================================
// retourne tous les objets dans une zone de deg degres autour de STRUCT_ASTRE
//============================================================================

void CAT_ZONE(STRUCT_ASTRE *gp_Ast, double deg, char lc_Cat[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {
  
  int    L,C ;
  double asc, asc_b  ;
  double dec, dec_b ;
  double d_angulaire ;
  double d_min ;
  
  TraceArbo(__func__,2,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  asc = gp_Ast->AGH0 ;
  dec = gp_Ast->DEC ;
  L=0 ;
  d_min=deg ;
  Trace1("Recherche dans la zone de %s : ASC=%f DEC=%f", gp_Ast->nom, gp_Ast->AGH0, gp_Ast->DEC) ;
  
  while(strcmp(lc_Cat[L][3],"_") && strcmp(gp_Ast->nom,lc_Cat[L][0]) && strcmp(gp_Ast->nom,lc_Cat[L][1])) {
   
    asc_b = atof( lc_Cat[L][2] ) ;
    dec_b = atof( lc_Cat[L][3] ) ;
    d_angulaire = sqrt(sqr( asc_b - asc ) + sqr ( dec_b -dec )) ;
    
    if (  d_angulaire < d_min )     // si objet dans le cercle recherche
    if ( deg > d_angulaire )  {
      // Trace1("dans la zone => %s=%s : ASC=%s DEC=%s DIST=%f",lc_Cat[L][0],lc_Cat[L][1],lc_Cat[L][2],lc_Cat[L][3], d_angulaire) ;
      if ( d_min > d_angulaire ) {  // Si objet encore plus proche trouve
        d_min = d_angulaire ;
        for(C=0;C<CAT_NB_COLONNES;C++) { 
          memset( gp_Ast->plus_proche[C], CALCUL_ZERO_CHAR, CAT_TAILLE_BUFFER);
          strcpy( gp_Ast->plus_proche[C], lc_Cat[L][C]) ;
        }
      }
    }
    L++;
  }
  Trace1("Le plus proche => %s=%s : ASC=%s DEC=%s DIST=%f",\
  gp_Ast->plus_proche[0], gp_Ast->plus_proche[1], gp_Ast->plus_proche[2], gp_Ast->plus_proche[3], d_min) ;
}
//============================================================================
void  CAT_FIND(STRUCT_ASTRE *gp_Ast, char lc_Cat[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {
  
  int    L=0 ;
  int    i_ligne=0 ;
  int    i_trouve =FALSE ;

  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  // dans les catalogues, coordonnnees en H et MIN pour ascension droite
  // et degres minutes pour declinaison
  // conversion en degres decimaux dans le catalogue aux valeurs deceimales
  // Dans tout le programme, coordonnees en radians
  
  L=0 ;
  
  memset( gp_Ast->infos, 0 , sizeof( gp_Ast->infos) ) ;

  gp_Ast->ASC=0;
  gp_Ast->DEC=0 ;
  
  i_ligne = L ;

  while( strcmp(lc_Cat[L][3],"_") && L < CAT_NB_LIGNES ) {
    //usleep(10000) ;
    Trace1("L=%d %s %s %s %s" , L , lc_Cat[L][0], lc_Cat[L][1] , lc_Cat[L][2] , lc_Cat[L][3] );

    if(!strcmp(lc_Cat[L][0],gp_Ast->nom)) {

      /* -----------------------------------------------
       * Sauvegarde des coordonnees equatoriales du catalogue 
       * dans la structure STRUCT_ASTRE 
       **************************************************/

      gp_Ast->ASC = atof( lc_Cat[L][2] ) / CALCUL_UN_RADIAN_EN_DEGRES ;
      gp_Ast->DEC = atof( lc_Cat[L][3] ) / CALCUL_UN_RADIAN_EN_DEGRES ;
      strcpy( gp_Ast->infos, lc_Cat[L][0] ) ;

      i_ligne = L ;
      i_trouve = TRUE ;
      break  ;
    }

    if(!strcmp(lc_Cat[L][1],gp_Ast->nom)) {

      gp_Ast->ASC = atof( lc_Cat[L][2] ) / CALCUL_UN_RADIAN_EN_DEGRES ;
      gp_Ast->DEC = atof( lc_Cat[L][3] ) / CALCUL_UN_RADIAN_EN_DEGRES ;
      strcpy( gp_Ast->infos, lc_Cat[L][1] ) ;

      i_ligne = L ; 
      i_trouve = TRUE ;
      break ;
    }
    L++;
  }
  if ( L < CAT_NB_LIGNES && i_trouve == TRUE ) {
    Trace("(trouve) : (nom) %s (infos) %s: ",gp_Ast->nom , gp_Ast->infos ) ;
  }
  else {
    
    gp_Ast->ASC = 0.0 ;
    gp_Ast->DEC = 0.0 ;
    Trace1(" %s : non trouve dans catalogue",gp_Ast->nom) ;
    strcpy( gp_Ast->nom, "undefined" ) ;
    strcpy( gp_Ast->infos, "undefined" ) ;
  }

  CALCUL_CONVERSIONS_ANGLES( gp_Ast ) ;

  Trace1(" %s : asc %d.%d.%d (hms) dec %.2f (deg)", \
    gp_Ast->nom , \
    gp_Ast->ASCt.HH, \
    gp_Ast->ASCt.MM, \
    gp_Ast->ASCt.SS, \
    gp_Ast->DEC * CALCUL_UN_RADIAN_EN_DEGRES
  ) ; 

  return ; 
}
//============================================================================
// Genere a la fois un catalogue avec des angles uniquement et au format decimal
// et aussi un fichier dans le repertoire des catalogues
//   format entree : NGC ; MESSIER (si existe sinon 0) ; ASC (HH) ; ASC(MM) ; DEC(DEG) ; DEG(MIN)
//   format sortie : NGC ; MESSIER (si existe sinon 0) ; ASC (degres decimaux) ; DEC(degres decimaux); ASC (heures decimales) ; DEC(heures decimales);     
//============================================================================

void CAT_FORMAT_DECIMAL_NGC( \

  char * catalogue_txt, \
  char l_char_Datas    [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER], \
  char l_char_DatasDec[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {
  
  FILE * fout ;
  char   buf[CAT_TAILLE_BUFFER] ;
  int    C,L ;
  int    ASC_HH,ASC_MM ;   // la 3eme et 4eme colonne sont HH  et MM  de la declinaison
  int    DEC_DEG,DEG_MIN ; // la 5eme et 6eme colonne sont DEG et MIN de l'ascension droite
  double asc, dec ;        // ascension droite et declinaison sous forme decimale
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */

  for(L=0;L<CAT_NB_LIGNES;L++)
    for(C=0;C<CAT_NB_COLONNES;C++)  {
      memset(l_char_DatasDec[L][C],CALCUL_ZERO_CHAR,CAT_TAILLE_BUFFER-1);
      strcpy(l_char_DatasDec[L][C],"_") ;
  } 
  
  memset(buf,CALCUL_ZERO_CHAR,CAT_TAILLE_BUFFER-1);
  sprintf(buf,"%s/%s/%s", gp_Con_Par->par_rep_home, gp_Con_Par->par_rep_cat,catalogue_txt) ;
  
  if ( (fout=fopen(buf,"w")) == NULL) {
    // completer et modifier
    SyslogErrFmt("probleme ouverture 1 %s\n",buf) ;
    exit(2) ;
  }
  L=0 ;
  while( strcmp(l_char_Datas[L][0],"_") ) {
   
   strcpy( l_char_DatasDec[L][0], l_char_Datas[L][0]) ;
   strcpy( l_char_DatasDec[L][1], l_char_Datas[L][1]) ;
   
   ASC_HH  = atoi( l_char_Datas[L][2] ) ;
   ASC_MM  = atoi( l_char_Datas[L][3] ) ;
   
   DEC_DEG = atoi( l_char_Datas[L][4] ) ;
   DEG_MIN = atoi( l_char_Datas[L][5] ) ;
   
   asc = ( (double)ASC_HH + SGN(ASC_HH) *(double)ASC_MM  / 60.0) * 15.0 ; // conversion en degres decimaux de type double
   dec =  (double)DEC_DEG + SGN(DEC_DEG)*(double)DEG_MIN / 60.0 ;
   
   sprintf(l_char_DatasDec[L][2],"%f",asc) ;
   sprintf(l_char_DatasDec[L][3],"%f",dec) ;
   
   asc = asc / 15 ;
   dec = dec / 15 ;
   
   sprintf(l_char_DatasDec[L][4],"%f",asc) ;
   sprintf(l_char_DatasDec[L][5],"%f",dec) ;
   
   fprintf(fout,"%s;%s;%s;%s;%s;%s\n",\
     l_char_DatasDec[L][0], \
     l_char_DatasDec[L][1], \
     l_char_DatasDec[L][2], \
     l_char_DatasDec[L][3], \
     l_char_DatasDec[L][4], \
     l_char_DatasDec[L][5]) ;   
   L++ ;
   if( L>=CAT_NB_LIGNES) break ;
  }
  fclose(fout) ;
}
//============================================================================
// Genere a la fois un catalogue avec des angles uniquement et au format decimal
// et aussi un fichier dans le repertoire des catalogues
//   format entree : NOM (etoile) ; ASC (HH) ; ASC(MM) ; DEC(degres decimaux); 
//   format sortie : NUMERO ; NOM (etoile) ; ASC (degres decimaux) ; DEC(degres decimaux);     
//============================================================================

void CAT_FORMAT_DECIMAL_ETO( \
  char * catalogue_txt, \
  char l_char_Datas    [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER], \
  char l_char_DatasDec[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {
  
  FILE * fout ;
  char   buf[CAT_TAILLE_BUFFER] ;
  int    C,L ;
  int    ASC_HH,ASC_MM ;   // la 3eme et 4eme colonne sont HH  et MM  de la declinaison
  double asc, dec ;        // ascension droite et declinaison sous forme decimale
  
  TraceArbo(__func__,1,"") ; /* MACRO_DEBUG_ARBO_FONCTIONS */
  
  for(L=0;L<CAT_NB_LIGNES;L++)
    for(C=0;C<CAT_NB_COLONNES;C++)  {
      memset(l_char_DatasDec[L][C],CALCUL_ZERO_CHAR,CAT_TAILLE_BUFFER-1);
      strcpy(l_char_DatasDec[L][C],"_") ;
  } 
  
  memset(buf,CALCUL_ZERO_CHAR,CAT_TAILLE_BUFFER-1);
  sprintf(buf,"%s/%s/%s",gp_Con_Par->par_rep_home,gp_Con_Par->par_rep_cat,catalogue_txt) ;
  
  if ( (fout=fopen(buf,"w")) == NULL) {
    // completer et modifier
    SyslogErrFmt("probleme ouverture 2 %s\n",buf) ;
    exit(2) ;
  }
  L=0 ;
  while( strcmp(l_char_Datas[L][0],"_") ) {
   
   strcpy(  l_char_DatasDec[L][0], l_char_Datas[L][0]) ;
   strcpy(  l_char_DatasDec[L][1], l_char_Datas[L][1]) ;
   
   ASC_HH  = atoi( l_char_Datas[L][2] ) ;
   ASC_MM  = atoi( l_char_Datas[L][3] ) ;
   
   dec = atof( l_char_Datas[L][4] ) ;
   //DEG_MIN = atoi( l_char_Datas[L][5] ) ;
   
   asc = ( ASC_HH + ( ASC_MM / 60.0 ) )   * 15.0 ; // conversion en degres decimaux de type double
   
   Trace1("aschh=%d\tascmm=%d\tdec=%f\tasc_calculer=%f", ASC_HH,ASC_MM,dec,asc) ;
   
   //dec =  (double)DEC_DEG + SGN(DEC_DEG)*(double)DEG_MIN / 60.0 ;
   
   sprintf(l_char_DatasDec[L][2],"%f",asc) ;
   sprintf(l_char_DatasDec[L][3],"%f",dec) ;
   
   asc = asc / 15 ;
   dec = dec / 15 ;
   
   sprintf(l_char_DatasDec[L][4],"%f",asc) ;
   sprintf(l_char_DatasDec[L][5],"%f",dec) ;
   
   fprintf(fout,"%s;%s;%s;%s;%s;%s\n",\
     l_char_DatasDec[L][0], \
     l_char_DatasDec[L][1], \
     l_char_DatasDec[L][2], \
     l_char_DatasDec[L][3], \
     l_char_DatasDec[L][4], \
     l_char_DatasDec[L][5]) ;   
   L++ ;
   if( L>=CAT_NB_LIGNES) break ;
  }
  fclose(fout) ;
}
//============================================================================




