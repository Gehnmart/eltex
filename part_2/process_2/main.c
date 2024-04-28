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
    child_pid = fork();
    if(child_pid == 0){
      printf("process3_pid  = %d\n", getpid());
      printf("  process3_ppid = %d\n", getppid());
      fflush(stdout);
      exit(5);
    } else {
      child_pid = fork();
      if(child_pid == 0){
        printf("process4_pid  = %d\n", getpid());
        printf("  process4_ppid = %d\n", getppid());
        fflush(stdout);
        exit(5);
      } else {
        printf("process1_pid  = %d\n", getpid());
        printf("  process1_ppid = %d\n", getppid());
        fflush(stdout);
        wait(&status);
        wait(&status);
        exit(5);
      }
    }
  } else {
    child_pid = fork();
    if(child_pid == 0){
      child_pid = fork();
      if(child_pid == 0) {
        printf("process5_pid  = %d\n", getpid());
        printf("  process5_ppid = %d\n", getppid());
        fflush(stdout);
        exit(5);
      } else {
        printf("process2_pid  = %d\n", getpid());
        printf("  process2_ppid = %d\n", getppid());
        fflush(stdout);
        wait(&status);
        exit(5);
      }
    } else {
      printf("main_pid   = %d\n", getpid());
      printf("  main_ppid  = %d\n", getppid());
      fflush(stdout);
      wait(&status);
      wait(&status);
      printf("exit status= %d\n", WEXITSTATUS(status));
    }
  }
}