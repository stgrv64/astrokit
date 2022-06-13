#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char * ephe="/home/stef/astrokit/ephemerides/aa_moi" ;
char cmd[256] ;
pid_t my_pid, parent_pid, child_pid;
int status;

char *patisserie[5]={"farine", "sucre", "beurre" , "oeufs", "canelle"} ;

// =========================================================================================================
int main(void) {


  memset( cmd, 0, sizeof(cmd)) ;

  if((child_pid = fork()) < 0 ){perror("fork failure");exit(1);}
  if(child_pid == 0) {  TRACE1("\nChild: I am a new-born process!\n\n");
    my_pid = getpid();    parent_pid = getppid();
    TRACE1("Child: my pid is: %d\n\n", my_pid);
    TRACE1("Child: my parent's pid is: %d\n\n", parent_pid);
    sprintf(cmd, "%s %f %f %f %f %f",ephe, -0.39, 43.3, 200.0, 2477343.7, 1.0 ) ;
    TRACE1("cmd=%s\n",cmd);
    execl(ephe, cmd,"-0.39", "43.3", "200", "2477343.7", "1", (char*)0);
    perror("execl() failure!\n\n");
    TRACE1("This print is after execl() and should not have been executed if execl were successful! \n\n");
    exit(2);
  }
  else
  {
    TRACE1("\nParent: I created a child process.\n\n");
    TRACE1("Parent: my child's pid is: %d\n\n", child_pid);
    system("ps -acefl | egrep \"angie|aa_moi\" ");  TRACE1("\n \n");
    wait(&status); /* can use wait(NULL) since exit status from child is not used. */
    TRACE1("\n Parent: my child is dead. I am going to leave.\n \n ");
  }
}
// =========================================================================================================
