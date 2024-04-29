#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARGV_MAX 256

void CreateArgv(char *str, char **argv) {
  int index = 0;
  char *save;
  char *ptr = strtok_r(str, " ", &save);
  do {
    argv[index++] = ptr;
  } while ((ptr = strtok_r(NULL, " ", &save)) != NULL);
}

void RunNewProcess(const char *path, char **argv) {
  int status = 0;
  int child_proc = fork();
  if (!child_proc) {
    execv(path, argv);
    fprintf(stderr, "%s", path);
    perror("");
    exit(EXIT_FAILURE);
  }
  if (wait(&status) == -1) {
    perror("wait error");
    exit(EXIT_FAILURE);
  }
}

int main() {
  char buf[PATH_MAX];
  char *argv[ARGV_MAX] = {0};
  while (1) {
    fgets(buf, PATH_MAX, stdin);
    buf[strlen(buf) - 1] = '\0';

    if (strcmp(buf, "exit") == 0) {
      exit(EXIT_SUCCESS);
    }

    CreateArgv(buf, argv);
    RunNewProcess(buf, argv);

    memset(argv, 0, sizeof(char *) * ARGV_MAX);
  }
}