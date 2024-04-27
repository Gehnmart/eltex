#ifndef VIRTUAL_FILE_SYSTEM_ADDITIONAL_H
#define VIRTUAL_FILE_SYSTEM_ADDITIONAL_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SUCCESS 0
#define FAILURE 1

#define ABS(a) a < 0 ? -a : a

int PrintStringFromFile(int fd, int start, int end);

#endif  // VIRTUAL_FILE_SYSTEM_ADDITIONAL_H