#define _GNU_SOURCE

/* -------------------------------------------------------------
# astrokit @ 2021  - lGPLv2 - Stephane Gravois - 
# --------------------------------------------------------------
# date        | commentaires 
# --------------------------------------------------------------
# 03/04/2021  | * ajout entete
#               * ajout entree chemins de config.txt
# -------------------------------------------------------------- 
*/

#include <cat.h>

//============================================================================
int CAT_FIN_MOT(char c) {
  int i=(int)c ;
  int j ;
  j=0;
  if ( i==59)j=1;  // point virgule
  return j ; 
}
//============================================================================
void CAT_AFFICHER(char catalogue[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {
  int l, c ;
  char buffer[ CAT_TAILLE_BUFFER * CAT_NB_COLONNES ] ;
  /* modif stgrv 01/2022 : avoid -Wrestrict passing pointers */ 
  char buffer_recopie [ CAT_TAILLE_BUFFER * CAT_NB_COLONNES ] ;
  l=0 ;
  while( strcmp( catalogue[l][3], "_" ) ) {
   memset( buffer,ZERO_CHAR, CAT_TAILLE_BUFFER * CAT_NB_COLONNES) ;
   for(c=0;c<CAT_NB_COLONNES;c++) {
    /* modif stgrv 01/2022 : avoid -Wrestrict passing pointers */ 
    memset( buffer_recopie, 0, sizeof(buffer_recopie) ) ;
    strcpy( buffer_recopie, buffer ) ;
    /* sprintf( buffer, "%-10s %-10s", buffer, catalogue[l][c] ) ; */
    sprintf( buffer, "%-10s %-10s", buffer_recopie, catalogue[l][c] ) ; 
   }
   TRACE("%5d = %s ",l,buffer) ;
   l++;
   if( l>=CAT_NB_LIGNES) break ;
  }
}
//============================================================================
void CAT_READ(char * catalogue_txt, char g_Datas[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {

  FILE *fin ;
  char buf[CAT_TAILLE_BUFFER * CAT_NB_COLONNES] ;
  int  C,L ;
  char *str1, *token, *sptr ;
  
  for(L=0;L<CAT_NB_LIGNES;L++)
    for(C=0;C<CAT_NB_COLONNES;C++) {
      memset(g_Datas[L][C],ZERO_CHAR,CAT_TAILLE_BUFFER);
      strcpy(g_Datas[L][C],"_") ;
    }
  
  memset(buf,ZERO_CHAR,CAT_TAILLE_BUFFER * CAT_NB_COLONNES);
  sprintf(buf,"%s/%s/%s",CONFIG_REP_HOME, CONFIG_REP_CAT,catalogue_txt) ;
  
  if ( (fin=fopen(buf,"r")) == NULL)  {
    // completer et modifier
    SyslogErrFmt("probleme ouverture 0 %s\n",buf) ;
    exit(2) ;
  }
  else TRACE1("open %s ok", buf) ;

  L=0;C=0;
  
  while( fgets( buf, CAT_TAILLE_BUFFER * CAT_NB_COLONNES , fin ) != NULL) {
    for (C = 0, str1 = buf ; ; C++, str1 = NULL) {
      token = strtok_r(str1, ";", &sptr);
      if (token == NULL) break ;
      strcpy( g_Datas[L][C],token);
    }
    L++;
    if( L>=CAT_NB_LIGNES) break ;
  }
  fclose(fin);
}
//============================================================================
// retourne tous les objets dans une zone de deg degres autour de ASTRE
//============================================================================

void CAT_ZONE(ASTRE *as, double deg, char cat[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {
  
  int    L,C ;
  double asc, asc_b  ;
  double dec, dec_b ;
  double d_angulaire ;
  double d_min ;
  
  asc = as->AGH0 ;
  dec = as->DEC ;
  L=0 ;
  d_min=deg ;
  TRACE("Recherche dans la zone de %s : ASC=%f DEC=%f", as->nom, as->AGH0, as->DEC) ;
  
  while(strcmp(cat[L][3],"_") && strcmp(as->nom,cat[L][0]) && strcmp(as->nom,cat[L][1])) {
   
    asc_b = atof( cat[L][2] ) ;
    dec_b = atof( cat[L][3] ) ;
    d_angulaire = sqrt(sqr( asc_b - asc ) + sqr ( dec_b -dec )) ;
    
    if (  d_angulaire < d_min )     // si objet dans le cercle recherche
    if ( deg > d_angulaire )  {
      // TRACE("dans la zone => %s=%s : ASC=%s DEC=%s DIST=%f",cat[L][0],cat[L][1],cat[L][2],cat[L][3], d_angulaire) ;
      if ( d_min > d_angulaire ) {  // Si objet encore plus proche trouve
        d_min = d_angulaire ;
        for(C=0;C<CAT_NB_COLONNES;C++) { 
          memset( as->plus_proche[C], ZERO_CHAR, CAT_TAILLE_BUFFER);
          strcpy( as->plus_proche[C], cat[L][C]) ;
        }
      }
    }
    L++;
  }
  TRACE("Le plus proche => %s=%s : ASC=%s DEC=%s DIST=%f",\
  as->plus_proche[0], as->plus_proche[1], as->plus_proche[2], as->plus_proche[3], d_min) ;
}
//============================================================================
void  CAT_FIND(ASTRE *as, char cat[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {
  
  int    L=0 ;
  int    i_ligne=0 ;
  
  // dans les catalogues, coordonnnees en H et MIN pour ascension droite
  // et degres minutes pour declinaison
  // conversion en degres decimaux dans cat.dec
  // Dans tout le programme, coordonnees en radians
  
  L=0 ;
  
  memset( as->infos, 0 , sizeof( as->infos) ) ;
  as->ASC=0;
  as->DEC=0 ;
  
  i_ligne = L ;

  while( strcmp(cat[L][3],"_") && L < CAT_NB_LIGNES ) {
    //usleep(10000) ;
    TRACE1("L=%d %s %s %s %s" , L , cat[L][0], cat[L][1] , cat[L][2] , cat[L][3] );

    if(!strcmp(cat[L][0],as->nom)) {

      /* -----------------------------------------------
       * Sauvegarde des coordonnees equatoriales du catalogue 
       * dans la structure ASTRE 
       **************************************************/

      as->ASC = atof( cat[L][2] ) / DEGRES ;
      as->DEC = atof( cat[L][3] ) / DEGRES ;

      strcpy( as->infos, cat[L][1] ) ;
      i_ligne = L ;break  ;
    }

    if(!strcmp(cat[L][1],as->nom)) {

      as->ASC = atof( cat[L][2] ) / DEGRES ;
      as->DEC = atof( cat[L][3] ) / DEGRES ;
      
      strcpy( as->infos, cat[L][1] ) ;
      i_ligne = L ; break ;
    }
    L++;
  }
  if ( L < CAT_NB_LIGNES ) {
    TRACE(" %s : trouve dans catalogue",as->nom) ;

    CALCUL_CONVERSIONS_ANGLES( as ) ;

    TRACE(" %s : ASC = %d.%d.%d (hms) %.2f (deg) %.2f (rad)", \
       as->nom , \
       as->ASCt.HH, \
       as->ASCt.MM, \
       as->ASCt.SS, \
       as->ASC * DEGRES, \
       as->ASC \
    ) ; 

    TRACE(" %s : DEC = %.2f  (deg) %.2f (rad)", \
       as->nom , \
       as->DEC * DEGRES , \
       as->DEC \
    ) ; 

    TRACE(" %s : INFOS : %s", \
      as->nom , \
      as->infos \
    ) ; 
  }
  else {
    TRACE(" %s : non trouve dans catalogue",as->nom) ;
  }

  return ; 
}
//============================================================================
// Genere a la fois un catalogue avec des angles uniquement et au format decimal
// et aussi un fichier dans le repertoire des catalogues
//   format entree : NGC ; MESSIER (si existe sinon 0) ; ASC (HH) ; ASC(MM) ; DEC(DEG) ; DEG(MIN)
//   format sortie : NGC ; MESSIER (si existe sinon 0) ; ASC (degres decimaux) ; DEC(degres decimaux); ASC (heures decimales) ; DEC(heures decimales);     
//============================================================================

void CAT_FORMAT_DECIMAL( \
  char * catalogue_txt, \
  char g_Datas    [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER], \
  char datas_dec[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {
  
  FILE * fout ;
  char   buf[CAT_TAILLE_BUFFER] ;
  int    C,L ;
  int    ASC_HH,ASC_MM ;   // la 3eme et 4eme colonne sont HH  et MM  de la declinaison
  int    DEC_DEG,DEG_MIN ; // la 5eme et 6eme colonne sont DEG et MIN de l'ascension droite
  double asc, dec ;        // ascension droite et declinaison sous forme decimale
  
  for(L=0;L<CAT_NB_LIGNES;L++)
    for(C=0;C<CAT_NB_COLONNES;C++)  {
      memset(datas_dec[L][C],ZERO_CHAR,CAT_TAILLE_BUFFER-1);
      strcpy(datas_dec[L][C],"_") ;
  } 
  
  memset(buf,ZERO_CHAR,CAT_TAILLE_BUFFER-1);
  sprintf(buf,"%s/%s/%s", CONFIG_REP_HOME, CONFIG_REP_CAT,catalogue_txt) ;
  
  if ( (fout=fopen(buf,"w")) == NULL) {
    // completer et modifier
    SyslogErrFmt("probleme ouverture 1 %s\n",buf) ;
    exit(2) ;
  }
  L=0 ;
  while( strcmp(g_Datas[L][0],"_") ) {
   
   strcpy( datas_dec[L][0], g_Datas[L][0]) ;
   strcpy( datas_dec[L][1], g_Datas[L][1]) ;
   
   ASC_HH  = atoi( g_Datas[L][2] ) ;
   ASC_MM  = atoi( g_Datas[L][3] ) ;
   
   DEC_DEG = atoi( g_Datas[L][4] ) ;
   DEG_MIN = atoi( g_Datas[L][5] ) ;
   
   asc = ( (double)ASC_HH + SGN(ASC_HH) *(double)ASC_MM  / 60.0) * 15.0 ; // conversion en degres decimaux de type double
   dec =  (double)DEC_DEG + SGN(DEC_DEG)*(double)DEG_MIN / 60.0 ;
   
   sprintf(datas_dec[L][2],"%f",asc) ;
   sprintf(datas_dec[L][3],"%f",dec) ;
   
   asc = asc / 15 ;
   dec = dec / 15 ;
   
   sprintf(datas_dec[L][4],"%f",asc) ;
   sprintf(datas_dec[L][5],"%f",dec) ;
   
   fprintf(fout,"%s;%s;%s;%s;%s;%s\n",\
     datas_dec[L][0], \
     datas_dec[L][1], \
     datas_dec[L][2], \
     datas_dec[L][3], \
     datas_dec[L][4], \
     datas_dec[L][5]) ;   
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

void CAT_FORMAT_DECIMAL_2( \
  char * catalogue_txt, \
  char g_Datas    [CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER], \
  char datas_dec[CAT_NB_LIGNES][CAT_NB_COLONNES][CAT_TAILLE_BUFFER]) {
  
  FILE * fout ;
  char   buf[CAT_TAILLE_BUFFER] ;
  int    C,L ;
  int    ASC_HH,ASC_MM ;   // la 3eme et 4eme colonne sont HH  et MM  de la declinaison
  double asc, dec ;        // ascension droite et declinaison sous forme decimale
  
  for(L=0;L<CAT_NB_LIGNES;L++)
    for(C=0;C<CAT_NB_COLONNES;C++)  {
      memset(datas_dec[L][C],ZERO_CHAR,CAT_TAILLE_BUFFER-1);
      strcpy(datas_dec[L][C],"_") ;
  } 
  
  memset(buf,ZERO_CHAR,CAT_TAILLE_BUFFER-1);
  sprintf(buf,"%s/%s/%s",CONFIG_REP_HOME,CONFIG_REP_CAT,catalogue_txt) ;
  
  if ( (fout=fopen(buf,"w")) == NULL) {
    // completer et modifier
    SyslogErrFmt("probleme ouverture 2 %s\n",buf) ;
    exit(2) ;
  }
  L=0 ;
  while( strcmp(g_Datas[L][0],"_") ) {
   
   strcpy(  datas_dec[L][0], g_Datas[L][0]) ;
   strcpy(  datas_dec[L][1], g_Datas[L][1]) ;
   
   ASC_HH  = atoi( g_Datas[L][2] ) ;
   ASC_MM  = atoi( g_Datas[L][3] ) ;
   
   dec = atof( g_Datas[L][4] ) ;
   //DEG_MIN = atoi( g_Datas[L][5] ) ;
   
   asc = ( ASC_HH + ( ASC_MM / 60.0 ) )   * 15.0 ; // conversion en degres decimaux de type double
   
   TRACE1("aschh=%d\tascmm=%d\tdec=%f\tasc_calculer=%f", ASC_HH,ASC_MM,dec,asc) ;
   
   //dec =  (double)DEC_DEG + SGN(DEC_DEG)*(double)DEG_MIN / 60.0 ;
   
   sprintf(datas_dec[L][2],"%f",asc) ;
   sprintf(datas_dec[L][3],"%f",dec) ;
   
   asc = asc / 15 ;
   dec = dec / 15 ;
   
   sprintf(datas_dec[L][4],"%f",asc) ;
   sprintf(datas_dec[L][5],"%f",dec) ;
   
   fprintf(fout,"%s;%s;%s;%s;%s;%s\n",\
     datas_dec[L][0], \
     datas_dec[L][1], \
     datas_dec[L][2], \
     datas_dec[L][3], \
     datas_dec[L][4], \
     datas_dec[L][5]) ;   
   L++ ;
   if( L>=CAT_NB_LIGNES) break ;
  }
  fclose(fout) ;
}
//============================================================================




