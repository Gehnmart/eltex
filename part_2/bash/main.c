#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_MAX 1024
#define ARG_LEN_MAX 256
#define ARGV_MAX 32
#define EXEC_MAX 16

typedef struct {
  int argv_len;
  char path[BUF_MAX];
  char *argv[ARGV_MAX];
} Executable;

typedef struct {
  int exec_len;
  Executable exec_list[EXEC_MAX];
} Transporter;

int TokenizeArgv(Executable *executable, char *str) {
  char *save;
  char *ptr = strtok_r(str, " ", &save);
  while (ptr != NULL) {
    if (executable->argv_len < ARGV_MAX) {
      executable->argv[executable->argv_len++] = ptr;
    } else {
      fprintf(stderr, "argv overflow\n");
      return -1;
    }
    ptr = strtok_r(NULL, " ", &save);
  }
  executable->argv[executable->argv_len] = NULL;
  strncpy(executable->path, executable->argv[0], ARG_LEN_MAX);
  executable->path[ARG_LEN_MAX - 1] = '\0';
  return 0;
}

int TokenizePipeline(Transporter *transporter, char *pipeline) {
  char *save = NULL;
  char *ptr = strtok_r(pipeline, "|", &save);
  while (ptr != NULL) {
    char err =
        TokenizeArgv(&(transporter->exec_list[transporter->exec_len++]), ptr);
    if (err == -1) {
      return -1;
    }
    ptr = strtok_r(NULL, "|", &save);
  }
  return 0;
}

void TransporterExec(Transporter *transporter) {
  pid_t cpid;
  int prev = -1, pipefd[2];

  for (int i = 0; i < transporter->exec_len; ++i) {
    if (pipe(pipefd) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
    cpid = fork();
    if (cpid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }

    if (cpid == 0) {
      if (i < transporter->exec_len - 1) {
        dup2(pipefd[1], STDOUT_FILENO);
      }
      if (i > 0) {
        dup2(prev, STDIN_FILENO);
      }

      execvp(transporter->exec_list[i].path, transporter->exec_list[i].argv);
      perror("execv");
      exit(EXIT_FAILURE);
    } else {
      wait(NULL);
      if (prev >= 0) {
        close(prev);
      }
      prev = pipefd[0];
      close(pipefd[1]);
    }
  }
  if (prev >= 0) {
    close(prev);
  }
}

int main() {
  Transporter transporter = {0};
  char buf[BUF_MAX];
  char err = 0;
  while (1) {
    if (fgets(buf, BUF_MAX, stdin) == NULL) {
      perror("fgets error\n");
      exit(EXIT_FAILURE);
    }
    buf[strlen(buf) - 1] = '\0';
    if (strlen(buf) <= 1) {
      continue;
    }

    if (buf[0] == 'e' && strncmp(buf, "exit", 4) == 0) {
      exit(EXIT_SUCCESS);
    }

    err = TokenizePipeline(&transporter, buf);
    if (err != -1) {
      TransporterExec(&transporter);
    }
    memset(transporter.exec_list, 0, sizeof(transporter.exec_list));
    transporter.exec_len = 0;
  }
}