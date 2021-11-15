#ifndef SRC_SQL_H_
#define SRC_SQL_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "debug.h"

#define SQL_CHAR_ESPACE                 ' '
#define SQL_CHAR_PARENTHESE_OUVRANTE    '('
#define SQL_CHAR_PARENTHESE_FERMANTE    ')'
#define SQL_CHAR_VIRGULE                ','
#define SQL_CHAR_POINT_VIRGULE          ';'

#define SQL_TYPE_INT                    "int"
#define SQL_TYPE_CHAR                   "char"
#define SQL_TYPE_CHAR_ETOILE            "char*"
#define SQL_TYPE_FLOAT                  "float"
#define SQL_TYPE_DOUBLE                 "double"

#define SQL_MACRO                1
#define SQL_STATEMENT_SIZE_MAX   4048

#define SQL_DATABASE      "mirza_sqlite3.db"

#define SQL_TABLE_MIRZA_BUFFERS  "mirza_buffers"
#define SQL_TABLE_MIRZA_BUFFERS1 "mirza_buffers1"

#define typename(x) _Generic((x), \
    int:     "int", \
    float:   "float", \
    double:  "double", \
    char:    "char", \
    int*:    "pint", \
    float*:  "pfloat", \
    double*: "pdouble", \
    char*:   "pchar", \
    default: "other")

#define MACRO_CREATE_TABLE(fmt, args...) { \
  memset( c_sql, 0 , sizeof(SQL_STATEMENT_SIZE_MAX ));\
  sprintf(c_sql, "%s:" fmt, __func__, ##args) ; \
  c_sql[6] = SQL_CHAR_ESPACE ;\
  c_sql[12] = SQL_CHAR_ESPACE ;} ;

#define MACRO_INSERT_INTO(fmt, args...) { \
  memset( c_sql, 0 , sizeof(SQL_STATEMENT_SIZE_MAX ));\
  sprintf(c_sql, "%s:" fmt, __func__, ##args) ; \
  c_sql[6] = SQL_CHAR_ESPACE ;\
  c_sql[11] = SQL_CHAR_ESPACE ;} ;

#define ARRAY_SIZE( array ) ( sizeof( array ) / sizeof( array[0] ) )

// ======================================================================================
// Prototypes fonctions
// ======================================================================================

void EXEC_SQL(char*) ;

void OPEN_DATABASE(char*) ;
void DROP_TABLE(char*) ;
void CREATE_TABLE(char*);
void CLOSE_DATABASE(void) ;
void INSERT_INTO(int, ...) ;
void INSERT_INTO_TABLE_(char * c_table, char * c_statement) ;

#endif /* SRC_SQL_H_ */

// ======================================================================================
// FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - FIN FICHIER - 
// ======================================================================================
