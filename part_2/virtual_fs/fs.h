#ifndef VIRTUAL_FS_FS_H
#define VIRTUAL_FS_FS_H

#include <dirent.h>
#include <string.h>

void AppendElemToPath(char *path, char *elem);
void DeleteEndDir(char *path);

#endif  // VIRTUAL_FS_FS_H