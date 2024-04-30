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

  if ((mkfifo("pipe", 0666)) == -1) {
    perror("mkfifo");
    exit(EXIT_FAILURE);
  }

  int fd = open("pipe", O_WRONLY);
  write(fd, "Hi!", 4);
  close(fd);
  unlink("pipe");

  exit(EXIT_SUCCESS);
}