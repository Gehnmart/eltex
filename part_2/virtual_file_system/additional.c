#include "additional.h"

int PrintStringFromFile(int fd, int start, int end) {
  int index = start < end ? 0 : -start;
  char ch = 0;
  for (; index <= end; ++index) {
    if (lseek(fd, ABS(index), SEEK_SET) == -1) {
      perror("lseek error\n");
      return FAILURE;
    } else {
      int err = read(fd, &ch, 1);
      if (err == -1) {
        perror("read error");
        return FAILURE;
      } else if (err == 0) {
        break;
      }
      printf("%c", ch);
    }
  }

  return SUCCESS;
}