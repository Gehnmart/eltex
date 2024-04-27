#include "controller.h"

WindowController **GetGlobalController() {
  static WindowController *controller = NULL;
  return &controller;
}

void InitGlobalController(WindowController *controller) {
  WindowController **temp = GetGlobalController();
  if (*temp == NULL) {
    *temp = controller;
  } else {
    exit(EXIT_FAILURE);
  }
}

void FreeDirList(DirectoryContext *win_context) {
  if (win_context->dir_list != NULL) {
    free(win_context->dir_list);
  }
}

void FreeWindow(WindowContext *win_context) {
  if (win_context != NULL) {
    delwin(win_context->window);
  }
  FreeDirList(&win_context->dir_context);
}

void FreeController(WindowController *controller) {
  for (int i = 0; i < controller->win_list_size; i++) {
    FreeWindow(&controller->win_list[i]);
  }
  if (controller != NULL) {
    free(controller->win_list);
  }
}

void SuccessfulExit(int exit_type) {
  WindowController **controller = GetGlobalController();

  FreeController(*controller);
  endwin();
  exit(exit_type);
}

void ControllerMalloc(WindowController *controller) {
  if (controller->win_list_size > 0) {
    WindowContext *temp =
        calloc(sizeof(WindowContext), controller->win_list_size);
    if (temp == NULL) {
      SuccessfulExit(EXIT_FAILURE);
    } else {
      controller->win_list = temp;
    }
  }
}

void InitWindow(WindowController *controller, int index, int size) {
  WindowContext *window = &controller->win_list[index];
  window->window = newwin(LINES, COLS / size, 0, COLS / size * index);
  if (window->window == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }

  char *err = realpath(".", window->dir_context.absolute_path);
  if (err == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }
  InitDirOnWindow(&(window->dir_context), ".");
}

WindowContext *WinRealloc(WindowController *controller, int next_size) {
  WindowContext *new_windows =
      realloc(controller->win_list, sizeof(WindowContext) * next_size);
  if (new_windows == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }

  return new_windows;
}

void ControllerRealloc(WindowController *controller, int next_size) {
  if (next_size > 0 && next_size != controller->win_list_size) {
    if (next_size > controller->win_list_size) {
      controller->win_list = WinRealloc(controller, next_size);
      for (int i = controller->win_list_size; i < next_size; i++) {
        InitWindow(controller, i, next_size);
      }
      controller->win_list_size = next_size;
    } else if (next_size < controller->win_list_size) {
      controller->win_list = WinRealloc(controller, next_size);
      controller->win_list_size = next_size;
    }
  }
}

void InitControllerWindows(WindowController *controller) {
  ControllerMalloc(controller);

  for (int i = 0; i < controller->win_list_size; i++) {
    InitWindow(controller, i, controller->win_list_size);
  }
}

void InitDirOnWindow(DirectoryContext *dir_context, const char *dirname) {
  struct dirent **dt;
  int num_entries = scandir(dirname, &dt, NULL, alphasort);
  if (num_entries < 0) {
    dir_context->dir_list = NULL;
    SuccessfulExit(EXIT_FAILURE);
  }
  dir_context->dir_list = dt;
  dir_context->selected_item = 0;
  dir_context->dir_list_size = num_entries;
}