#ifndef VIRTUAL_FS_UI_H
#define VIRTUAL_FS_UI_H

#include <curses.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>

#define MAX_WINDOW_COUNT 6

#define C_UP 1
#define C_DOWN 2

typedef struct {
  char absolute_path[PATH_MAX];
  struct dirent **dir_list;
  int selected_item;
  int size;
} wcontext_t;

typedef struct {
  WINDOW *win;
  wcontext_t wcontext;
} window_t;

typedef struct {
  window_t windows[MAX_WINDOW_COUNT];
  int current_window;
} win_controller_t;

void Run();

#endif  // VIRTUAL_FS_UI_H