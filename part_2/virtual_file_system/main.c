#include "additional.h"

#include <string.h>

int main() {
  char exit_status = SUCCESS;
  int fd = EOF;

  if ((fd = open("output.txt", O_CREAT | O_RDWR)) < 0) {
    perror("file open error");
    exit(EXIT_FAILURE);
  }

  char str[] = "String from file";
  if (write(fd, str, strlen(str)) < 0) {
    perror("file write error");
    close(fd);
    exit(EXIT_FAILURE);
  }

  exit_status = PrintStringFromFile(fd, strlen(str) - 1, 0);
  putchar('\n');

  close(fd);
  exit(exit_status);
}