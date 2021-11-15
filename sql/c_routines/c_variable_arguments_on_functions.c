#include <stdio.h>
#include <stdarg.h>

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

double appel_fonction_vargs(int num,...) {

   va_list valist;
   double sum = 0.0;
   int i;

   /* initialize valist for num number of arguments */
   va_start(valist, num);

   /* access all the arguments assigned to valist */
   for (i = 0; i < num; i++) {
      sum += va_arg(valist, int);
   }
	
   /* clean memory reserved for valist */
   va_end(valist);

   return sum/num;
}

int main() {
   appel_fonction_vargs(4,1,23.23,"chaine",'c');
}
