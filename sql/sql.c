#include "sql.h"

// ======================================================================================
// Variables globales
// ======================================================================================

const char * const SQL_TABLE_MIRZA_BUFFERS_SCHEMA[] =  { \
   "i_ech   int not null",\
   "i_buf   int not null",\
   "c_dep1  double not null",\
   "c_dep2  double not null",\
   "c_dep3  double not null",\
   "c_depZ  double not null"\
} ;

const char * const SQL_TABLE_MIRZA_BUFFERS_TYPES[] =  { \
   "int",\
   "int",\
   "double",\
   "double",\
   "double",\
   "double"\
} ;
// ======================================================================================

const char * const SQL_TABLE_MIRZA_BUFFERS1_SCHEMA[] =  { \
   "i_ech   int not null",\
   "c_dep1  char(500) not null",\
   "c_dep2  char(500) not null",\
   "c_dep3  char(500) not null",\
   "c_depZ  char(500) not null"\
} ;

const char * const SQL_TABLE_MIRZA_BUFFERS1_TYPES[] =  { \
   "int",\
   "char",\
   "char",\
   "char",\
   "char"\
} ;

// ======================================================================================

sqlite3 *sqlite3_mirza_db = NULL ;

char      c_macro_cmd[ SQL_STATEMENT_SIZE_MAX ] ;
char      c_sql [SQL_STATEMENT_SIZE_MAX] ;
char    * zErrMsg = NULL ;

// ======================================================================================
// Fonction de statement sqlite3
// ======================================================================================

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   Traces("begin") ;
   for(i = 0; i<argc; i++) {
     printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   Traces("end") ;
   return 0;
}
// ======================================================================================
// Fonction de statement sqlite3
// ======================================================================================

void EXEC_SQL(char * c_sql) {

   int i_rc = 0 ;

   Traces1("SQL : trying %s", c_sql) ;
   
   i_rc = sqlite3_exec(sqlite3_mirza_db, c_sql, callback, 0, &zErrMsg);
   
   if( i_rc != SQLITE_OK ){ 
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
     sqlite3_free(zErrMsg); 
     perror("SQL error: ");
     exit(EXIT_FAILURE);
   } 
   else { 
     Traces1("SQL : %s = SUCCESS\n", c_sql);
   }
}
// ======================================================================================
// Open database 
// ======================================================================================

void OPEN_DATABASE(char* c_sqlite_database) {

   int   rc=0;

   Traces("SQL : trying .. sqlite3_open(c_sqlite_database, &sqlite3_mirza_db)") ;
   rc = sqlite3_open(c_sqlite_database, &sqlite3_mirza_db);

   if( rc ) {
     fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(sqlite3_mirza_db));
     perror("fopen ");
     exit(EXIT_FAILURE);
   } 
   else {
     fprintf(stdout, "Opened database successfully\n");
   }
}
// ======================================================================================
// create a table
// ======================================================================================

void CREATE_TABLE(char * c_table) {

  char i ;

  Traces("begin") ;
  
  memset( c_macro_cmd, 0 , sizeof( c_macro_cmd )) ;
  
  if ( strcmp( c_table, SQL_TABLE_MIRZA_BUFFERS) == 0 ) {
    for(i=0;i<ARRAY_SIZE(SQL_TABLE_MIRZA_BUFFERS_SCHEMA);i++) {
      if ( i == 0  ) sprintf( c_macro_cmd , "%s", SQL_TABLE_MIRZA_BUFFERS_SCHEMA[i]) ;  
      else           sprintf( c_macro_cmd , "%s,%s", c_macro_cmd, SQL_TABLE_MIRZA_BUFFERS_SCHEMA[i]) ;
    }
    MACRO_CREATE_TABLE("%s(%s);",c_table,c_macro_cmd);
    Traces("c_sql=%s\n", c_sql) ;
  }

  if ( strcmp( c_table, SQL_TABLE_MIRZA_BUFFERS1) == 0 ) {
    for(i=0;i<ARRAY_SIZE(SQL_TABLE_MIRZA_BUFFERS1_SCHEMA);i++) {
      if ( i == 0  ) sprintf( c_macro_cmd , "%s", SQL_TABLE_MIRZA_BUFFERS1_SCHEMA[i]) ;  
      else           sprintf( c_macro_cmd , "%s,%s", c_macro_cmd, SQL_TABLE_MIRZA_BUFFERS1_SCHEMA[i]) ;
    }
    MACRO_CREATE_TABLE("%s(%s);",c_table,c_macro_cmd);
    Traces("c_sql=%s\n", c_sql) ;
  }
  /* Execute SQL statement */

  EXEC_SQL(c_sql) ;

  Traces("end") ;
}
// ======================================================================================
// create a table
// ======================================================================================

void DROP_TABLE(char * c_table) {

  int i_rc =0 ;
  Traces("begin") ;
  
  memset( c_sql, 0 , sizeof( c_sql )) ;
  sprintf( c_sql , "DROP TABLE IF EXISTS %s ;", c_table ) ; 
  
  /* Execute SQL statement */

  EXEC_SQL(c_sql) ;

  Traces("end") ;
}
// ======================================================================================
// insert into a table
// ======================================================================================

void INSERT_INTO(int i_num, ...) {

   va_list valist;
   char  i;
   int   i_rc =0 ;
   char  c_table[255] ;

   Traces("begin");

   memset( c_table, 0 , sizeof( c_table )) ;
   memset( c_macro_cmd, 0 , sizeof( c_macro_cmd )) ;
   memset( c_sql, 0 , sizeof( c_sql )) ;

   Traces1("c_macro_cmd=%s", c_macro_cmd);
   Traces1("i_num=%d", i_num);

   va_start(valist, i_num);
/*
   printf("%s\n",va_arg(valist, char*)) ;
   printf("%d\n",va_arg(valist, int)) ;
   printf("%s\n",va_arg(valist, char*)) ;
   printf("%f\n",va_arg(valist, double)) ;
*/
   strcpy( c_table, va_arg(valist, char*) ) ;
   sprintf( c_sql , "INSERT INTO %s VALUES (", c_table ) ;

   if( strcmp (c_table, SQL_TABLE_MIRZA_BUFFERS ) == 0 ) {
   
      if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS_TYPES[0], SQL_TYPE_INT ) )    sprintf( c_sql , "%s%d", c_sql, va_arg(valist, int)) ;
      if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS_TYPES[0], SQL_TYPE_CHAR ) )   sprintf( c_sql , "%s'%s'", c_sql, va_arg(valist, char*)) ;
      if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS_TYPES[0], SQL_TYPE_DOUBLE ) ) sprintf( c_sql , "%s%f", c_sql, va_arg(valist, double)) ;

      for(i=1;i<i_num;i++) {
        if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS_TYPES[i], SQL_TYPE_INT ) )    sprintf( c_sql , "%s,%d", c_sql, va_arg(valist, int)) ;
        if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS_TYPES[i], SQL_TYPE_CHAR ) )   sprintf( c_sql , "%s,'%s'", c_sql, va_arg(valist, char*)) ;
        if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS_TYPES[i], SQL_TYPE_DOUBLE ) ) sprintf( c_sql , "%s,%f", c_sql, va_arg(valist, double)) ;
      }
   }

   if( strcmp (c_table, SQL_TABLE_MIRZA_BUFFERS1 ) == 0 ) {

       if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS1_TYPES[0], SQL_TYPE_INT ) )    sprintf( c_sql , "%s%d", c_sql, va_arg(valist, int)) ;
       if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS1_TYPES[0], SQL_TYPE_CHAR ) )   sprintf( c_sql , "%s'%s'", c_sql, va_arg(valist, char*)) ;
       if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS1_TYPES[0], SQL_TYPE_DOUBLE ) ) sprintf( c_sql , "%s%f", c_sql, va_arg(valist, double)) ;

       for(i=1;i<i_num;i++) {
         if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS1_TYPES[i], SQL_TYPE_INT ) )    sprintf( c_sql , "%s,%d", c_sql, va_arg(valist, int)) ;
         if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS1_TYPES[i], SQL_TYPE_CHAR ) )   sprintf( c_sql , "%s,'%s'", c_sql, va_arg(valist, char*)) ;
         if( ! strcmp( SQL_TABLE_MIRZA_BUFFERS1_TYPES[i], SQL_TYPE_DOUBLE ) ) sprintf( c_sql , "%s,%f", c_sql, va_arg(valist, double)) ;
       }
   }

   sprintf( c_sql , "%s);", c_sql) ;
   va_end(valist);
  
   Traces1("c_sql=%s\n", c_sql) ;
   Traces1("c_macro_cmd=%s", c_macro_cmd);

   /* Execute SQL statement */

   EXEC_SQL(c_sql) ;
   
   Traces("end");
}
// ======================================================================================
// insert into a table
// ======================================================================================

void INSERT_INTO_TABLE(char * c_table, char * c_statement) {

  char i ;

  Traces("begin") ;
  
  memset( c_macro_cmd, 0 , sizeof( c_macro_cmd )) ;

  if ( strcmp( c_table, SQL_TABLE_MIRZA_BUFFERS) == 0 ) {
    for(i=0;i<ARRAY_SIZE(SQL_TABLE_MIRZA_BUFFERS_SCHEMA);i++) {
      if ( i == 0  ) sprintf( c_macro_cmd , "%s VALUES ", SQL_TABLE_MIRZA_BUFFERS_SCHEMA[i]) ;  
      else           sprintf( c_macro_cmd , "%s,%s", c_macro_cmd, SQL_TABLE_MIRZA_BUFFERS_SCHEMA[i]) ;
    }
    MACRO_CREATE_TABLE("%s(%s);",c_table,c_macro_cmd);
    Traces("c_sql=%s\n", c_sql) ;
  }
  Traces("end") ;
}
// ======================================================================================
// Close database 
// ======================================================================================

void CLOSE_DATABASE(void) {

   Traces("begin") ;
   Traces("SQL : trying sqlite3_close(sqlite3_mirza_db)") ;
   sqlite3_close(sqlite3_mirza_db);
   Traces("end") ;
}
// ======================================================================================
// FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - 
// ======================================================================================





