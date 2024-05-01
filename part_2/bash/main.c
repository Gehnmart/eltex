#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_MAX  4096
#define ARGV_MAX 128
#define ARG_LEN_MAX 128
#define EXEC_MAX 16

typedef struct {
  int  argv_len;
  char path[PATH_MAX];
  char argv[ARGV_MAX][ARG_LEN_MAX];
} Executable;

typedef struct {
  int exec_len;
  Executable exec_list[EXEC_MAX];
} Transporter;

void TokenizeArgv(Executable *executable, char *str) {
  char *save;
  char *ptr = strtok_r(str, " ", &save);
  do {
    strncpy(executable->argv[executable->argv_len++], ptr, ARG_LEN_MAX);
  } while ((ptr = strtok_r(NULL, " ", &save)) != NULL);
  strncpy(executable->path, executable->argv[0], ARG_LEN_MAX);
}

void TokenizePipeline(Transporter *transporter, char *pipeline) {
  char *save;
  char *ptr = strtok_r(pipeline, "|", &save);
  do {
    TokenizeArgv(&(transporter->exec_list[transporter->exec_len++]), ptr);
  } while ((ptr = strtok_r(NULL, "|", &save)) != NULL);
}

void TransporterExec(Transporter *transporter) {
  pid_t cpid;

  if(transporter->exec_len > 1) {
    int pipefd1[2];
    int pipefd2[2];
    if (pipe(pipefd1) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
    if (pipe(pipefd2) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
    for(int i = 0; i < transporter->exec_len; ++i) {
      printf("Test\n");
      cpid = fork();
      if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
      }
      if(cpid == 0){
        if (i == 0) {
          dup2(pipefd1[STDOUT_FILENO], STDOUT_FILENO);
        } else if(i % 2 == 0){
          if(i == transporter->exec_len - 1){
            dup2(pipefd2[STDIN_FILENO], STDIN_FILENO);
          } else {
            dup2(pipefd2[STDIN_FILENO], STDIN_FILENO);
            dup2(pipefd1[STDOUT_FILENO], STDOUT_FILENO);
          }
        } else {
          if(i == transporter->exec_len - 1){
            dup2(pipefd1[STDIN_FILENO], STDIN_FILENO);
          } else {
            dup2(pipefd1[STDIN_FILENO], STDIN_FILENO);
            dup2(pipefd2[STDOUT_FILENO], STDOUT_FILENO);
          }
        }

        char *temp_argv[ARGV_MAX];
        for(int k = 0; k < transporter->exec_list[i].argv_len; k++){
          temp_argv[k] = transporter->exec_list[i].argv[k];
        }
        temp_argv[transporter->exec_list[i].argv_len] = NULL;

        execv(transporter->exec_list[i].path, temp_argv);
        perror("execv");
        exit(EXIT_FAILURE);
      } else {
        wait(NULL);
      }
    }
  }

  // for(int i = 0; i < transporter->exec_len; ++i){
  //   printf("%s\n", transporter->exec_list[i].path);
  //   for(int j = 0; j < transporter->exec_list[i].argv_len; j++){
  //     printf(" %s", transporter->exec_list[i].argv[j]);
  //   }
  //   printf("\n");
  // }
}

int main() {
  Transporter transporter = {0};
  char buf[BUF_MAX];
  while (1) {
    fgets(buf, BUF_MAX, stdin);
    buf[strlen(buf) - 1] = '\0';

    if (strcmp(buf, "exit") == 0) {
      exit(EXIT_SUCCESS);
    }

    TokenizePipeline(&transporter, buf);
    TransporterExec(&transporter);
  }
}