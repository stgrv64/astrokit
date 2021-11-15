#ifndef SRC_DEBUG_H_
#define SRC_DEBUG_H_

#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

// quelques macros de debugging

#ifndef DEBUG
  #define Traces(fmt, args...)   while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
  #define Traces1(fmt, args...)  while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#endif

#if defined(DEBUG) && DEBUG == 0 
  #define Traces(fmt, args...)   while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
  #define Traces1(fmt, args...)  while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#endif

#if defined(DEBUG) && DEBUG == 1
  #define Traces(fmt, args...)            { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
  #define Traces1(fmt, args...)  while(0) { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; } 
#endif

#if defined(DEBUG) && DEBUG == 2
  #define Traces(fmt, args...)   { fprintf(stderr, "\n%s:%d:%s() : " fmt, __FILE__,__LINE__, __func__, ##args) ; }
  #define Traces1(fmt, args...)  { fprintf(stderr, "\n%s : " fmt, __func__, ##args) ; }
#endif

#endif /* SRC_DEBUG_H_ */
