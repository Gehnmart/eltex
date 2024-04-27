#ifndef VIRTUAL_FS_CONTROLLER_H
#define VIRTUAL_FS_CONTROLLER_H

#include <linux/limits.h>
#include <malloc.h>
#include <ncurses.h>
#include <stdlib.h>

#include "file_system.h"

typedef struct {
  struct dirent **dir_list;
  char absolute_path[PATH_MAX];
  int dir_list_size;
  int selected_item;
} DirectoryContext;

typedef struct {
  WINDOW *window;
  DirectoryContext dir_context;
} WindowContext;

typedef struct {
  WindowContext *win_list;
  int current_window;
  int win_list_size;
} WindowController;

WindowController **GetGlobalController();

void SuccessfulExit(int exit_type);

void InitGlobalController(WindowController *controller);
void InitControllerWindows(WindowController *controller);
void InitDirOnWindow(DirectoryContext *dir_context, const char *dirname);
void InitWindow(WindowController *controller, int index, int win_list_size);

void FreeDirList(DirectoryContext *dir_context);
void FreeWindow(WindowContext *win_context);
void FreeController(WindowController *controller);

void ControllerMalloc(WindowController *controller);
WindowContext *WinRealloc(WindowController *controller, int next_size);
void ControllerRealloc(WindowController *controller, int next_size);

#endif  // VIRTUAL_FS_CONTROLLER_H