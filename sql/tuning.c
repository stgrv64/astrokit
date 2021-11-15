#include "tuning.h"

// ==============================================================================================
// Principe de utilisation de gettimeofday :
//    struct timeval t_deb
//    struct timeval t_fin 
//    ...
//    gettimeofday(&t_deb,NULL)                          # debut de la mesure de temps
//    ...
//    gettimeofday(&t_deb,NULL)                          # fin de la mesure de temps
//    t_diff = (float) (( t_fin.tv_sec \
//                      - t_deb.tv_sec ) \
//                       * MICRO_SEC) \
//                      + t_fin.tv_usec \
//                      - t_deb.tv_usec ;                # calcul de la difference
//    printf("temps de traitement = %.2f\n", t_diff) ;   # a eviter !! sauvegarder les donnees plutot dans un buffer

// ==============================================================================================
// Cette fonction affiche un double pointeur sur un flotant 
// faisant office de tableau de flottant dynamique
// ==============================================================================================

void Tuning_Affiche_Buffer(float ** f_buffer, unsigned long ul_buffer_size) {
  
  unsigned long   i = 0 ;
  
  Traces("begin") ;

  for ( i=0; i < ul_buffer_size ; i++ ) {
    
    Traces("%f", *f_buffer[i]);
  }
  Traces("end") ;
}
// ==============================================================================================
// Cette fonction initialise un double pointeur sur un flotant 
// faisant office de tableau de flottant dynamique
// ==============================================================================================

float **  Tuning_Init_Buffer_RandowSize(unsigned long ul_buffer_size) {
  unsigned long   i = 0 ;
  float       ** f_buffer ;

  Traces("begin") ;

  f_buffer = (float**)malloc( sizeof(float*) * ul_buffer_size ) ;
  memset( f_buffer, 0, sizeof( f_buffer )) ;

  for ( i=0; i < ul_buffer_size ; i++ ) {

    f_buffer[i] = (float*)malloc( sizeof(float)) ;
    *f_buffer[i] = (float)random() * 0.5 ;
  }
  Traces("end") ;

  return f_buffer ;
}
// ==============================================================================================
// Cette fonction initialise un double pointeur sur un flotant 
// faisant office de tableau de flottant dynamique
// ==============================================================================================

void  Tuning_Free_Buffer_RandowSize(float ** f_buffer, unsigned long ul_buffer_size) {
  unsigned long   i = 0 ;

  Traces("begin") ;

  for ( i=0; i < ul_buffer_size ; i++ ) {
    free(  f_buffer[i] ) ;
  }
  free ( f_buffer ) ;

  Traces("end") ;
}
// ==============================================================================================
void Tuning_Mesures_Init(void) {
  
  unsigned long i,j ;

  Traces("begin") ;

  for(i=0;i<TUNING_TIMEVAL_NOMBRE_MAX_ECHANTILLONS;i++) {
    for(j=0;j<TUNING_TIMEVAL_NOMBRE_MAX_MESURES;j++) { 

      tuning_fl_results[i][j]   = TUNING_TIMEVAL_INIT_TAB ;
      tuning_si_numero[i][j]    = TUNING_TIMEVAL_INIT_TAB ;
      tuning_fl_totaux_mes[i]   = TUNING_TIMEVAL_INIT_TAB ;
      tuning_fl_totaux_ech[j]   = TUNING_TIMEVAL_INIT_TAB ;
    }
  }
  tuning_l_incr_mesure          = TUNING_TIMEVAL_INIT_TAB ;
  tuning_l_incr_echantillon     = TUNING_TIMEVAL_INIT_TAB ;
  tuning_l_numero_gettimeofday  = TUNING_TIMEVAL_INIT_TAB ;
  tuning_fl_total = 0 ;

  Traces("end") ;
}
// ==============================================================================================
void Tuning_Mesures_Afficher(void) {
  
  unsigned long i,j ;

  Traces("begin") ;

  for(i=0;i<TUNING_TIMEVAL_NOMBRE_MAX_ECHANTILLONS;i++) {
    for(j=0;j<TUNING_TIMEVAL_NOMBRE_MAX_MESURES;j++) { 
      Traces( "%.2f ", tuning_fl_results[i][j] ) ;  
    }
    Traces("") ;
  }
  Traces("end") ;
}
// ==============================================================================================

void Tuning_Mesures_Echantillon_Suivant(void) {
  
  Traces("begin") ;

  tuning_l_incr_echantillon++ ;
  tuning_l_incr_mesure = TUNING_TIMEVAL_INIT_TAB ;
  tuning_fl_total = 0 ;

  // au premier echantillon, avant la premiere mesure, on a besoin d'une 
  // premiere date dans le tableau de mesures

  if ( tuning_l_incr_echantillon == 0  ) {
    tuning_l_numero_gettimeofday++ ;
    gettimeofday( & tuning_st_buf[ tuning_l_numero_gettimeofday ], NULL ) ;
  }
  Traces("end") ;
}
// ==============================================================================================
// toujours faire un Tuning_Mesures_Echantillon_Suivant avant toute nouvelle mesure

void Tuning_Mesure_Suivante(int i_numero_mesure) {
  
  Traces("begin") ;

  tuning_l_numero_gettimeofday++ ;  
  gettimeofday( & tuning_st_buf[ tuning_l_numero_gettimeofday ], NULL ) ;
  tuning_l_incr_mesure++ ;

  Traces("tuning_l_numero_gettimeofday = %ld", tuning_l_numero_gettimeofday) ;

  // on fait la difference uniquement si t_buf[ si_incr_mesureTUNING_TIMEVAL_INIT_TAB ] existe

  tuning_fl_diff = (float) (((   tuning_st_buf[ tuning_l_numero_gettimeofday     ].tv_sec \
                               - tuning_st_buf[ tuning_l_numero_gettimeofday - 1 ].tv_sec ) * TUNING_MICRO_SEC ) \
                               + tuning_st_buf[ tuning_l_numero_gettimeofday     ].tv_usec \
                               - tuning_st_buf[ tuning_l_numero_gettimeofday - 1 ].tv_usec ) ;

  // On sauvegarde la mesure dans le buffer (tableau a 2 dimenseions)

  Traces1("tuning_fl_diff = %f", tuning_fl_diff) ;

  tuning_fl_results[ tuning_l_incr_echantillon ][ tuning_l_incr_mesure ] = tuning_fl_diff ;
  tuning_si_numero[ tuning_l_incr_echantillon ][ tuning_l_incr_mesure ]  = i_numero_mesure ;
  tuning_fl_total += tuning_fl_diff ;
  tuning_fl_totaux_mes[ tuning_l_incr_echantillon ] = tuning_fl_total ;

  Traces("end") ;
}
// ==============================================================================================
// toujours faire un Tuning_Mesures_Echantillon_Suivant avant toute nouvelle mesure
// ==============================================================================================

void Tuning_Mesure_Finale(void) {
  
  unsigned long l_incr_echantillon=0 ;
  unsigned long l_incr_mesure=0 ;

  float         fl_somme_echantillon ;

  Traces("begin") ;

  tuning_l_numero_gettimeofday++ ;  
  gettimeofday( & tuning_st_buf[ tuning_l_numero_gettimeofday ], NULL ) ;
  tuning_l_incr_mesure++ ;

  Traces1("tuning_l_numero_gettimeofday = %ld\n", tuning_l_numero_gettimeofday) ;

  // on fait la difference avec la toute premiere mesure

  tuning_fl_diff = (float) ((( tuning_st_buf[ tuning_l_numero_gettimeofday ].tv_sec  - tuning_st_buf[ 0 ].tv_sec ) * TUNING_MICRO_SEC ) \
                             + tuning_st_buf[ tuning_l_numero_gettimeofday ].tv_usec - tuning_st_buf[ 0 ].tv_usec ) ;

  // On sauvegarde la mesure dans le buffer (tableau a 2 dimenseions)

  tuning_fl_results[ tuning_l_incr_echantillon ][ tuning_l_incr_mesure ] = 0 ;
  fl_somme_echantillon = 0 ;

  for( l_incr_mesure=0; l_incr_mesure < TUNING_TIMEVAL_NOMBRE_MAX_MESURES; l_incr_mesure++ ) {
    for( l_incr_echantillon=0; l_incr_echantillon < tuning_l_incr_echantillon; l_incr_echantillon++ )  {
      if ( tuning_fl_results[ l_incr_echantillon ][ l_incr_mesure ] >=0 ) {
        fl_somme_echantillon += tuning_fl_results[ l_incr_echantillon ][ l_incr_mesure ] ;
      }
    }
    tuning_fl_totaux_ech[ l_incr_mesure ] = fl_somme_echantillon ;
    tuning_fl_results[ tuning_l_incr_echantillon ][ l_incr_mesure ] = fl_somme_echantillon ;
    tuning_si_numero[   tuning_l_incr_echantillon ][ l_incr_mesure ]  = tuning_si_numero[   tuning_l_incr_echantillon -1 ][ l_incr_mesure ];
    fl_somme_echantillon = 0 ;
  }

  tuning_fl_total += tuning_fl_diff ;
  tuning_fl_totaux_mes[ tuning_l_incr_echantillon ] = tuning_fl_total ;

  Traces("end") ;
}
// ==============================================================================================
void Tuning_Write(float ** f_buffer,unsigned long ul_buffer_size) {

  unsigned long i ;  

  Traces("begin") ;

  for(i=0;i<ul_buffer_size;i++) {
    Traces1("fprintf => file_tuning_fic_datas_out") ;
    fprintf( file_tuning_fic_datas_out , "%.5f%c", *f_buffer[i] , TUNING_CHAR_DATAS_SEPARATOR ) ; 
  }
  fprintf( file_tuning_fic_datas_out , "\n" ) ;
  if ( TUNING_FFLUSH_USE ) fflush( file_tuning_fic_datas_out ) ;
  
  Traces("end") ;
}
// ==============================================================================================

void Tuning_Write_Datas_ToFs(unsigned long ul_buffer_size, float **dep1, float **dep2, float **dep3, float **depZ) {
  
  Traces("begin") ;

  Tuning_Write(dep1,ul_buffer_size) ;
  Tuning_Write(dep2,ul_buffer_size) ; 
  Tuning_Write(dep3,ul_buffer_size) ; 
  Tuning_Write(depZ,ul_buffer_size) ;  

  Traces("end") ;
}
// ==============================================================================================

void Tuning_Write_Mesures_ToFs(void) {
  
  unsigned long i,j, k ;
  
  i = 0 ;  // i représente les échantillons 
  j = 0 ;  // j représente les mesures 

  Traces("debut boucle while( tuning_fl_results[i][j] > TUNING_TIMEVAL_INIT_TAB.. )\n" );

  while( tuning_fl_results[i][j] > TUNING_TIMEVAL_INIT_TAB ) {

    Traces("ecriture %lu %lu %.2f", i, j,tuning_fl_results[i][j] );

    if ( tuning_si_numero[i][j] > TUNING_TIMEVAL_INIT_TAB && tuning_si_numero[i][j] != 84 ) {
      fprintf( file_tuning_fic_mesures_out , "(%d)%.0f   \t", tuning_si_numero[i][j], tuning_fl_results[i][j] ) ;  
      if ( TUNING_FFLUSH_USE ) fflush( file_tuning_fic_mesures_out ) ;
    }
     
    if ( tuning_fl_results[i][j+1] <= TUNING_TIMEVAL_INIT_TAB ) {
      fprintf( file_tuning_fic_mesures_out , "TOTAL = %.0f\n", tuning_fl_totaux_mes[ i ] ) ;
      if ( TUNING_FFLUSH_USE ) fflush( file_tuning_fic_mesures_out ) ;
      j = 0 ;
      i ++ ;
    }
    else j++ ;	
  }
}
// ==============================================================================================
void Tuning_Open_Mesures_ToFs(char * c_file_nom) {

  memset( tuning_ch_mesures_out,0,sizeof(tuning_ch_mesures_out)) ;
  strcpy( tuning_ch_mesures_out, c_file_nom ) ;

  if ( ( file_tuning_fic_mesures_out = fopen( tuning_ch_mesures_out, "a")) == NULL ) { 
    perror("fopen ");
    exit(EXIT_FAILURE);
  }
}
// ==============================================================================================
void Tuning_Close_Mesures_ToFs(void) {

  if ( fclose( file_tuning_fic_mesures_out ) != 0 ) { 
    perror("fclose ");
    exit(EXIT_FAILURE);
  }
}
// ==============================================================================================
void Tuning_Open_Datas_ToFs(char * c_file_nom ) {

  memset( tuning_ch_datas_out,0,sizeof(tuning_ch_datas_out)) ;
  strcpy( tuning_ch_datas_out, c_file_nom ) ;

  if ( ( file_tuning_fic_datas_out = fopen( tuning_ch_datas_out, "a")) == NULL ) { 
    perror("fopen ");
    exit(EXIT_FAILURE);
  }
}
// ==============================================================================================
void Tuning_Close_Datas_ToFs(void) {

  if ( fclose( file_tuning_fic_datas_out ) != 0 ) { 
    perror("fclose ");
    exit(EXIT_FAILURE);
  }
}
// ==============================================================================================





