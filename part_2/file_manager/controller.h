#ifndef VIRTUAL_FS_CONTROLLER_H
#define VIRTUAL_FS_CONTROLLER_H

#include <linux/limits.h>
#include <malloc.h>
#include <ncurses.h>
#include <stdlib.h>

#include "file_system.h"

/**
 * @brief Structure representing the directory context.
 */
typedef struct {
  struct dirent **dir_list;      ///< Array of directory entries
  char absolute_path[PATH_MAX];  ///< Absolute path of the directory
  int dir_list_size;             ///< Size of the directory list
  int selected_item;  ///< Index of the selected item in the directory
} DirectoryContext;

/**
 * @brief Structure representing the window context.
 */
typedef struct {
  WINDOW *window;  ///< Pointer to the window
  DirectoryContext
      dir_context;  ///< Directory context associated with the window
} WindowContext;

/**
 * @brief Structure representing the window controller.
 */
typedef struct {
  WindowContext *win_list;  ///< Array of window contexts
  int current_window;       ///< Index of the current window
  int win_list_size;        ///< Size of the window list
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