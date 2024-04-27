#ifndef VIRTUAL_FS_CONTROLLER_H
#define VIRTUAL_FS_CONTROLLER_H

#include "file_system.h"

#include <stdlib.h>
#include <malloc.h>
#include <linux/limits.h>
#include <ncurses.h>

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
  window_t *windows;
  int current_window;
  int size;
} win_controller_t;

win_controller_t **GetGlobalController();
void InitGlobalController(win_controller_t *controller);
void FreeDirList(wcontext_t *context);
void FreeWindow(window_t *window);
void FreeController(win_controller_t *controller);
void SuccessfulExit(int exit_type);
void ControllerMalloc(win_controller_t *controller);
void InitWindow(win_controller_t *controller, int index, int size);
window_t *WinRealloc(win_controller_t *controller, int next_size);
void ControllerRealloc(win_controller_t *controller, int next_size);
void InitControllerWindows(win_controller_t *controller);
void InitDirOnWindow(wcontext_t *context, const char *dirname);

#endif  // VIRTUAL_FS_CONTROLLER_H