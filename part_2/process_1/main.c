#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
  pid_t child_pid;
  int status = 0;

  child_pid = fork();
  if(child_pid == 0){
    printf("child_pid  = %d\n", getpid());
    printf("child_ppid = %d\n", getppid());
    exit(5);
  } else {
    printf("main_pid   = %d\n", getpid());
    printf("main_ppid  = %d\n", getppid());
    wait(&status);
    printf("exit status= %d\n", WEXITSTATUS(status));
  }
}