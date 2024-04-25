#include "fs.h"

void AppendElemToPath(char *path, char *elem) {
  strcat(path, "/");
  strcat(path, elem);
}

void DeleteEndDir(char *path) {
  char ch = 0;
  int i = strlen(path) - 1;
  for (; i > 0 && path[i] != '/'; i--) {
    path[i] = '\0';
  }
  if (strlen(path) > 1) path[i] = '\0';
}