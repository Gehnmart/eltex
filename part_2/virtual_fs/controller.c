#include "controller.h"

win_controller_t **GetGlobalController() {
  static win_controller_t *controller = NULL;
  return &controller;
}

void InitGlobalController(win_controller_t *controller) {
  win_controller_t **temp = GetGlobalController();
  if (*temp == NULL) {
    *temp = controller;
  } else {
    exit(EXIT_FAILURE);
  }
}

void FreeDirList(wcontext_t *context) {
  if (context->dir_list != NULL) {
    free(context->dir_list);
  }
}

void FreeWindow(window_t *window) {
  if (window != NULL) {
    delwin(window->window);
  }
  FreeDirList(&window->wcontext);
}

void FreeController(win_controller_t *controller) {
  for (int i = 0; i < controller->win_list_size; i++) {
    FreeWindow(&controller->win_list[i]);
  }
  if (controller != NULL) {
    free(controller->win_list);
  }
}

void SuccessfulExit(int exit_type) {
  win_controller_t **controller = GetGlobalController();

  FreeController(*controller);
  endwin();
  exit(exit_type);
}

void ControllerMalloc(win_controller_t *controller) {
  if (controller->win_list_size > 0) {
    window_t *temp = calloc(sizeof(window_t), controller->win_list_size);
    if (temp == NULL) {
      SuccessfulExit(EXIT_FAILURE);
    } else {
      controller->win_list = temp;
    }
  }
}

void InitWindow(win_controller_t *controller, int index, int size) {
  window_t *window = &controller->win_list[index];
  window->window = newwin(LINES, COLS / size, 0, COLS / size * index);
  if (window->window == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }

  char *err = realpath(".", window->wcontext.absolute_path);
  if (err == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }
  InitDirOnWindow(&(window->wcontext), ".");
}

window_t *WinRealloc(win_controller_t *controller, int next_size) {
  window_t *new_windows =
      realloc(controller->win_list, sizeof(window_t) * next_size);
  if (new_windows == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }

  return new_windows;
}

void ControllerRealloc(win_controller_t *controller, int next_size) {
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

void InitControllerWindows(win_controller_t *controller) {
  ControllerMalloc(controller);

  for (int i = 0; i < controller->win_list_size; i++) {
    InitWindow(controller, i, controller->win_list_size);
  }
}

void InitDirOnWindow(wcontext_t *context, const char *dirname) {
  struct dirent **dt;
  int num_entries = scandir(dirname, &dt, NULL, alphasort);
  if (num_entries < 0) {
    context->dir_list = NULL;
    SuccessfulExit(EXIT_FAILURE);
  }
  context->dir_list = dt;
  context->selected_item = 0;
  context->dir_list_size = num_entries;
}