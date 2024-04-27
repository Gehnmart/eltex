#include "file_system.h"

void AppendElemToPath(char *path, const char *elem) {
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

int IsDirectory(const char *absolute_path, const char *d_name) {
  char temp_buf[PATH_MAX] = {0};
  strcpy(temp_buf, absolute_path);
  AppendElemToPath(temp_buf, d_name);

  struct stat statbuf;
  if (stat(temp_buf, &statbuf) != 0) return 0;
  return S_ISDIR(statbuf.st_mode);
}

