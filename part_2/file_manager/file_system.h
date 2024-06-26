#ifndef VIRTUAL_FS_FILESYSTEM_H
#define VIRTUAL_FS_FILESYSTEM_H

#include <dirent.h>
#include <linux/limits.h>
#include <string.h>
#include <sys/stat.h>

void AppendElemToPath(char *path, const char *elem);
void DeleteEndDir(char *path);
int IsDirectory(const char *absolute_path, const char *d_name);
int IsExecutable(const char *absolute_path, const char *d_name);

#endif  // VIRTUAL_FS_FILESYSTEM_H