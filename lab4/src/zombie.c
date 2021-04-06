#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

// task 2

int main(void)
{
  pid_t child_pid; 

  if ((child_pid = fork ()) < 0) {
      perror("error with fork");
      exit(1);
  }

  if (child_pid > 0) {  
      printf("zombie = %d\n", child_pid);
    sleep (10);         
  }                     
  else {
    exit (0);            
  }
  return 0;
}
