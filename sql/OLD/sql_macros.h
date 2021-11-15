#ifndef SRC_SQL_MACROS_H_
#define SRC_SQL_MACROS_H_

#include <stdio.h>
#include <stdlib.h>

#define SQL_MACRO                1
#define SQL_STATEMENT_SIZE_MAX   4048

char    *c_macro_sql_statement = NULL ;
// quelques macros de formatage de chaine de caractere
// Avec nombre d'arguments variables

#if defined(SQL_MACRO) && SQL_MACRO == 1
  #define Sql_Macro(fmt, args...) { \
  memset( c_macro_sql_statement, 0 , sizeof(SQL_STATEMENT_SIZE_MAX ));\
  sprintf(c_macro_sql_statement, "%s:" fmt, __func__, ##args) ; } ;
#endif

#endif /* SRC_SQL_MACROS_H_ */
