#include <stdio.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void CreateArgv(char *str, char **argv) {
  int index = 0;
  char *save;
  char *ptr = strtok_r(str, " ", &save);
  do {
    argv[index++] = ptr;
  } while((ptr = strtok_r(NULL, " ", &save)) != NULL);
}

void RunNewProcess(const char *path, char **argv) {
  int status = 0;
  int child_proc = fork();
  if(!child_proc) {
    execv(path, argv);
    fprintf(stderr, "%s", path);
    perror("");
    exit(EXIT_FAILURE);
  }
  if(wait(&status) == -1){
    perror("wait error");
  }
}

int main() {
  char buf[PATH_MAX];
  while(1){
    fgets(buf, PATH_MAX, stdin);
    buf[strlen(buf) - 1] = '\0';

    if(strcmp(buf, "exit") == 0){
      exit(EXIT_SUCCESS);
    }
    char *argv[512] = {0};
    CreateArgv(buf, argv);
    RunNewProcess(buf, argv);
  }
}