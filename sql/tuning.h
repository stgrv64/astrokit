#ifndef SRC_TUNING_H_
#define SRC_TUNING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "debug.h"

//#define TUNING_FIC_MESURES_OUT     "/tmp/mirza_mesures.csv"
//#define TUNING_FIC_DATAS_OUT       "/tmp/mirza_datas.csv"

#define TUNING_FFLUSH_USE                                 1 

#define TUNING_MICRO_SEC                                  1000000.0

#define TUNING_TIMEVAL_NOMBRE_MAX_ECHANTILLONS            10000
#define TUNING_TIMEVAL_NOMBRE_MAX_MESURES                 100
#define TUNING_TIMEVAL_NOMBRE_MAX_GETTIMEOFDAY            10000
#define TUNING_TIMEVAL_INIT_TAB                           -1

#define TUNING_CHAR_BUFFER_SIZE                           5000
#define TUNING_CHAR_DATAS_SEPARATOR                       ';'

// Variables  -------------------------------------------------------------------------------------------

FILE * file_tuning_fic_mesures_out ;
FILE * file_tuning_fic_datas_out ;

char           tuning_ch_mesures_out[ TUNING_CHAR_BUFFER_SIZE ]  ;
char           tuning_ch_datas_out[ TUNING_CHAR_BUFFER_SIZE ]  ;

struct timeval tuning_st_buf        [ TUNING_TIMEVAL_NOMBRE_MAX_GETTIMEOFDAY ] ;

float          tuning_fl_results    [ TUNING_TIMEVAL_NOMBRE_MAX_ECHANTILLONS ][ TUNING_TIMEVAL_NOMBRE_MAX_MESURES ] ;
float          tuning_fl_totaux_mes [ TUNING_TIMEVAL_NOMBRE_MAX_ECHANTILLONS ] ;
float          tuning_fl_totaux_ech [ TUNING_TIMEVAL_NOMBRE_MAX_MESURES ] ;

short int      tuning_si_numero     [ TUNING_TIMEVAL_NOMBRE_MAX_ECHANTILLONS ][ TUNING_TIMEVAL_NOMBRE_MAX_MESURES ] ;

long           tuning_l_incr_echantillon ;
long           tuning_l_incr_mesure ;
long           tuning_l_numero_gettimeofday ;

float          tuning_fl_diff ;
float          tuning_fl_total ;  

// Fonctions prototypes ----------------------------------------------------------------------------------

float **  Tuning_Init_Buffer_RandowSize(unsigned long) ;
void      Tuning_Affiche_Buffer(float ** , unsigned long) ;
void      Tuning_Free_Buffer_RandowSize(float ** f_buffer, unsigned long ul_buffer_size) ;

// ==========================================================
// Fonctions prototypes de mesures  
// ==========================================================

void Tuning_Mesures_Init() ;
void Tuning_Mesures_Afficher() ;
void Tuning_Mesures_Echantillon_Suivant() ;
void Tuning_Mesure_Suivante(int) ;
void Tuning_Mesure_Finale() ;

// ==========================================================
// Fonctions prototypes d ecritures des mesures
// ==========================================================

void Tuning_Open_Mesures_ToFs(char*) ;
void Tuning_Write_Mesures_ToFs() ;
void Tuning_Close_Mesures_ToFs() ;

// ==========================================================
// Fonctions prototypes d ecritures des buffers float*
// ==========================================================

void Tuning_Open_Datas_ToFs(char*) ;
void Tuning_Write(float ** ,unsigned long );
void Tuning_Write_Datas_ToFs(unsigned long,float**,float**,float**,float**) ;
void Tuning_Close_Datas_ToFs() ;

#endif /* SRC_TUNING_H_ */
