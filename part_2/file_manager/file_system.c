#include "file_system.h"

/**
 * @brief Appends an element to the end of a file path.
 * @param path The file path to append to.
 * @param elem The element to append.
 *
 * This function appends an element to the end of a file path. It ensures that
 * the path is not longer than `PATH_MAX` characters and appends a '/' before
 * the element.
 */

void AppendElemToPath(char *path, const char *elem) {
  strncat(path, "/", PATH_MAX - 1);
  strncat(path, elem, PATH_MAX - 1);
}

/**
 * @brief Deletes the last directory from a file path.
 * @param path The file path to modify.
 *
 * This function deletes the last directory from a file path. It removes all
 * characters from the last '/' to the end of the string.
 */
void DeleteEndDir(char *path) {
  char ch = 0;
  int i = strlen(path) - 1;
  for (; i > 0 && path[i] != '/'; i--) {
    path[i] = '\0';
  }
  if (strlen(path) > 1) path[i] = '\0';
}

/**
 * @brief Checks if a directory exists.
 * @param absolute_path The absolute path of the directory to check.
 * @param d_name The name of the directory to check.
 * @return 1 if the directory exists, 0 otherwise.
 *
 * This function checks if a directory exists. It constructs the absolute path
 */
int IsDirectory(const char *absolute_path, const char *d_name) {
  char temp_buf[PATH_MAX] = {0};
  strncpy(temp_buf, absolute_path, PATH_MAX - 1);
  AppendElemToPath(temp_buf, d_name);

  struct stat statbuf;
  if (stat(temp_buf, &statbuf) != 0) return 0;
  return S_ISDIR(statbuf.st_mode);
}

int IsExecutable(const char *absolute_path, const char *d_name) {
  char temp_buf[PATH_MAX] = {0};
  strncpy(temp_buf, absolute_path, PATH_MAX - 1);
  AppendElemToPath(temp_buf, d_name);

  struct stat statbuf;
  if (stat(temp_buf, &statbuf) != 0) return 0;
  return statbuf.st_mode & S_IXUSR;
}