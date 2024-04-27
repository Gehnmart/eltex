#ifndef VIRTUAL_FS_CONTROLLER_H
#define VIRTUAL_FS_CONTROLLER_H

#include <linux/limits.h>
#include <malloc.h>
#include <ncurses.h>
#include <stdlib.h>

#include "file_system.h"

typedef struct {
  char absolute_path[PATH_MAX];
  struct dirent **dir_list;
  int dir_list_size;
  int selected_item;
} wcontext_t;

typedef struct {
  WINDOW *window;
  wcontext_t wcontext;
} window_t;

typedef struct {
  window_t *win_list;
  int current_window;
  int win_list_size;
} win_controller_t;

win_controller_t **GetGlobalController();

void SuccessfulExit(int exit_type);

void InitGlobalController(win_controller_t *controller);
void InitControllerWindows(win_controller_t *controller);
void InitDirOnWindow(wcontext_t *context, const char *dirname);
void InitWindow(win_controller_t *controller, int index, int win_list_size);

void FreeDirList(wcontext_t *context);
void FreeWindow(window_t *window);
void FreeController(win_controller_t *controller);

void ControllerMalloc(win_controller_t *controller);
window_t *WinRealloc(win_controller_t *controller, int next_size);
void ControllerRealloc(win_controller_t *controller, int next_size);

#endif  // VIRTUAL_FS_CONTROLLER_H