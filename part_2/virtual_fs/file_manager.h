#ifndef VIRTUAL_FS_FILE_MANAGER_H
#define VIRTUAL_FS_FILE_MANAGER_H

#include "file_system.h"
#include "controller.h"

#include <linux/limits.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#define MAX_WINDOW_COUNT 8
#define MIN_WINDOW_COUNT 1
#define START_WINDOW_COUNT 2

#define REFRESH_CURRENT 1
#define REFRESH_CURRENT_AND_PREV 2
#define REFRESH_ALL 4

#define C_UP 1
#define C_DOWN 2

#define MIN(a, b) (a > b ? b : a)

void Run();

#endif  // VIRTUAL_FS_FILE_MANAGER_H