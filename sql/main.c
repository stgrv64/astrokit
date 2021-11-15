#include "main.h"

void main(int argc, char ** argv) {

   unsigned long i_incr ;
   char          c_dep1[1024] ;
   char          c_dep2[1024] ;
   char          c_dep3[1024] ;
   char          c_depZ[1024] ;

   memset( tuning_ch_mesures_out, 0, sizeof( tuning_ch_mesures_out )  ) ;
   memset( tuning_ch_datas_out,   0, sizeof( tuning_ch_datas_out )  ) ;

   if ( argc != 5  ) {
      printf("Usage\t: %s <path fic mesures> <path fic datas> <taille buffer float* (exemple = decimated_size = 25)> <iterations number>\n",argv[0]) ;
      printf("\t: total_number_received_buffers : [1 - oo] \n") ;
      printf("\t: 0 = oo = infini while(1) ===> pour mesure CPU en mode permanent \n") ;
      exit(1) ;
   }
   else {
      ul_buffer_size      = atoi( argv[3] ) ;
      ul_iteration_number = atoi( argv[4] ) ;
   }

   OPEN_DATABASE( SQL_DATABASE );

   DROP_TABLE( SQL_TABLE_MIRZA_BUFFERS ) ;
   CREATE_TABLE( SQL_TABLE_MIRZA_BUFFERS ) ; 

   DROP_TABLE( SQL_TABLE_MIRZA_BUFFERS1 ) ;
   CREATE_TABLE( SQL_TABLE_MIRZA_BUFFERS1 ) ; 
   
   // INSERT_INTO(3, SQL_TABLE_MIRZA_BUFFERS,2,"dep1",5655656542.56) ; // test insertion

   Traces("path fic mesures = %s", argv[1] ) ;
   Traces("path fic datas   = %s", argv[2] ) ;
   Traces("ul_buffer_size = %lu", ul_buffer_size) ;
   Traces("ul_iteration_number = %lu", ul_iteration_number) ;
   
   dep1 = Tuning_Init_Buffer_RandowSize( ul_buffer_size) ;
   dep2 = Tuning_Init_Buffer_RandowSize( ul_buffer_size) ;
   dep3 = Tuning_Init_Buffer_RandowSize( ul_buffer_size) ;
   depZ = Tuning_Init_Buffer_RandowSize( ul_buffer_size) ;  
  
   Tuning_Mesures_Init() ;
   Tuning_Open_Mesures_ToFs(argv[1]);
   Tuning_Open_Datas_ToFs(argv[2]);  

   for( ul_iteration_number_incr = 0 ; ul_iteration_number_incr < ul_iteration_number ; ul_iteration_number_incr++ ) {

     Tuning_Mesures_Echantillon_Suivant() ;

     Traces("ul_iteration_number_incr =  %lu", ul_iteration_number_incr) ;
     
     Tuning_Mesure_Suivante(0) ;

     Traces("ul_iteration_number_incr =  %lu", ul_iteration_number_incr) ;

     //Tuning_Open_Datas_ToFs(argv[2]);
     Tuning_Write_Datas_ToFs( ul_buffer_size, dep1, dep2, dep3, depZ) ; // on ecrit les DATAS
     //Tuning_Close_Datas_ToFs() ;

     Traces("ul_iteration_number_incr =  %lu", ul_iteration_number_incr) ;

     Tuning_Mesure_Suivante(1) ;

     // insertion en base de donneees avec une ligne par champs de buffer
     // DONC on fait 25* une insertion pour des tableaux de flottants de 25

     for(i_incr=0;i_incr < ul_buffer_size;i_incr++) {
       Traces1("INSERT_INTO => SQL_TABLE_MIRZA_BUFFERS") ;
       INSERT_INTO(6, SQL_TABLE_MIRZA_BUFFERS, ul_iteration_number_incr, i_incr, *dep1[i_incr], *dep2[i_incr], *dep3[i_incr], *depZ[i_incr]) ;
     }

     // INSERT_INTO(3, SQL_TABLE_MIRZA_BUFFERS, ul_iteration_number_incr ,"dep1",(double)dep1) ;
     // INSERT_INTO(3, SQL_TABLE_MIRZA_BUFFERS, ul_iteration_number_incr ,"dep2",(double)dep2) ;
     // INSERT_INTO(3, SQL_TABLE_MIRZA_BUFFERS, ul_iteration_number_incr ,"dep3",(double)dep3) ;
     // INSERT_INTO(3, SQL_TABLE_MIRZA_BUFFERS, ul_iteration_number_incr ,"depZ",(double)depZ) ;*/

     Tuning_Mesure_Suivante(2) ;

     // insertion en base de donneees avec une ligne pour l'ensemble d'une buffer 
     // DONC on ne fait qu'une seule insertion pour tout le tableau de flottant

     memset(c_dep1,0,sizeof(c_dep1)) ;
     memset(c_dep2,0,sizeof(c_dep2)) ;
     memset(c_dep3,0,sizeof(c_dep3)) ;
     memset(c_depZ,0,sizeof(c_depZ)) ;

     for(i_incr=0;i_incr < ul_buffer_size;i_incr++) {
       sprintf(c_dep1, "%s %.2f",c_dep1, *dep1[i_incr] ) ;
       sprintf(c_dep2, "%s %.2f",c_dep2, *dep2[i_incr] ) ;
       sprintf(c_dep3, "%s %.2f",c_dep3, *dep3[i_incr] ) ;
       sprintf(c_depZ, "%s %.2f",c_depZ, *depZ[i_incr] ) ;
     } 
     Traces("c_dep1=%s\n", c_dep1) ;
     Traces("c_dep2=%s\n", c_dep2) ;
     Traces("c_dep3=%s\n", c_dep3) ;
     Traces("c_depZ=%s\n", c_depZ) ;    

     INSERT_INTO(5,  SQL_TABLE_MIRZA_BUFFERS1 , ul_iteration_number_incr, c_dep1, c_dep2, c_dep3, c_depZ ) ;
     
     Tuning_Mesure_Suivante(3) ;

     Traces("ul_iteration_number_incr =  %lu", ul_iteration_number_incr) ;
   }
   Traces("fin boucle for") ;
   Tuning_Mesures_Echantillon_Suivant() ;
   Tuning_Mesure_Finale() ;

   Tuning_Write_Mesures_ToFs() ;

   Tuning_Close_Mesures_ToFs() ;
   Tuning_Close_Datas_ToFs() ;

   Tuning_Free_Buffer_RandowSize( dep1, ul_buffer_size) ;
   Tuning_Free_Buffer_RandowSize( dep2, ul_buffer_size) ;
   Tuning_Free_Buffer_RandowSize( dep3, ul_buffer_size) ;
   Tuning_Free_Buffer_RandowSize( depZ, ul_buffer_size) ;

   CLOSE_DATABASE()  ;
}
// ==============================================================================================
// FIN MAIN - FIN MAIN -FIN MAIN -FIN MAIN -FIN MAIN -FIN MAIN -FIN MAIN -FIN MAIN -FIN MAIN -
// ==============================================================================================













