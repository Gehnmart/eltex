#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t cpid;
  int status = 0;
  char buf;

  int fd = open("pipe", O_RDONLY);
  while (read(fd, &buf, 1) > 0) {
    write(STDOUT_FILENO, &buf, 1);
  }
  write(STDOUT_FILENO, "\n", 1);
  close(fd);

  exit(EXIT_SUCCESS);
}