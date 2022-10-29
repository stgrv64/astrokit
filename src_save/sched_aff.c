#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void main(int argc, char ** argv) {

 int cpu = atoi(argv[1]) ;
 
 cpu_set_t cpuset ;
 
 if ( cpu >= 0 ) {   
    CPU_ZERO(& cpuset ) ;
    CPU_SET( cpu, &cpuset) ;
    if ( sched_setaffinity( getpid() , sizeof( cpuset ), & cpuset) != 0) { perror("sched_setaffinity"); exit(EXIT_FAILURE); }
    else printf("modification du cpu du precessus :  %d\n", cpu) ;
  }
}
