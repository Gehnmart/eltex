#include "controller.h"

/**
 * @brief Retrieves a pointer to the global WindowController instance
 * @return Address of the global WindowController pointer
 */
WindowController **GetGlobalController() {
  static WindowController *controller = NULL;
  return &controller;
}

/**
 * @brief Initializes the global WindowController with a given controller
 *        if it is not already initialized, otherwise exits with failure
 * @param controller - Pointer to WindowController to set as the global instance
 */
void InitGlobalController(WindowController *controller) {
  WindowController **temp = GetGlobalController();
  if (*temp == NULL) {
    *temp = controller;
  } else {
    endwin();
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Frees the memory allocated for the directory list within a
 * DirectoryContext
 * @param dir_context - Pointer to DirectoryContext containing the directory
 * list to free
 */
void FreeDirList(DirectoryContext *dir_context) {
  if (dir_context != NULL) {
    for(int i = 0; i < dir_context->dir_list_size; i++){
      free(dir_context->dir_list[i]);
    }
    free(dir_context->dir_list);
  }
}

/**
 * @brief Frees the memory for a WindowContext, including its associated window
 *        and directory list
 * @param win_context - Pointer to WindowContext to be freed
 */
void FreeWindow(WindowContext *win_context) {
  if (win_context != NULL) {
    delwin(win_context->window);
    FreeDirList(&win_context->dir_context);
  }
}

/**
 * @brief Frees the memory for a WindowController, including all its
 * WindowContexts
 * @param controller - Pointer to WindowController to be freed
 */
void FreeController(WindowController *controller) {
  if (controller != NULL) {
    for (int i = 0; i < controller->win_list_size; i++) {
      FreeWindow(&controller->win_list[i]);
    }
    free(controller->win_list);
  }
}

/**
 * @brief Exits the program after freeing the global WindowController and
 *        terminating the ncurses window
 * @param exit_type - Exit status code
 */
void SuccessfulExit(int exit_type) {
  WindowController **controller = GetGlobalController();

  FreeController(*controller);
  endwin();
  exit(exit_type);
}

/**
 * @brief Allocates memory for the window list in the controller
 * @param controller - Pointer to WindowController whose window list needs
 * allocation
 */
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

/**
 * @brief Initializes a window in the controller at a given index
 * @param controller - Pointer to WindowController containing the window to
 * initialize
 * @param index - Index of the window to initialize
 * @param size - Number of windows to split the screen into
 */
void InitWindow(WindowController *controller, int index, int win_list_size) {
  WindowContext *window = &controller->win_list[index];
  window->window =
      newwin(LINES, COLS / win_list_size, 0, COLS / win_list_size * index);
  if (window->window == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }

  char *err = realpath(".", window->dir_context.absolute_path);
  if (err == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }
  InitDirOnWindow(&(window->dir_context), ".");
}

/**
 * @brief Reallocates the window list of the controller to a new size
 * @param controller - Pointer to WindowController whose window list is to be
 * reallocated
 * @param next_size - New size for the window list
 * @return Pointer to the newly allocated WindowContext array
 */
WindowContext *WinRealloc(WindowController *controller, int next_size) {
  WindowContext *new_windows =
      realloc(controller->win_list, sizeof(WindowContext) * next_size);
  if (new_windows == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }

  return new_windows;
}

/**
 * @brief Reallocates the controller's window list to a new size and
 *        initializes any new windows as necessary
 * @param controller - Pointer to WindowController to be reallocated
 * @param next_size - The new size for the window list
 */
void ControllerRealloc(WindowController *controller, int next_size) {
  if (next_size > 0 && next_size != controller->win_list_size) {
    if (next_size > controller->win_list_size) {
      controller->win_list = WinRealloc(controller, next_size);
      for (int i = controller->win_list_size; i < next_size; i++) {
        InitWindow(controller, i, next_size);
      }
      controller->win_list_size = next_size;
    } else {
      controller->win_list = WinRealloc(controller, next_size);
      controller->win_list_size = next_size;
    }
  }
  if (controller->current_window >= next_size) {
    controller->current_window = next_size - 1;
  }
}

/**
 * @brief Initializes all windows in the controller's window list
 * @param controller - Pointer to WindowController whose windows are to be
 * initialized
 */
void InitControllerWindows(WindowController *controller) {
  ControllerMalloc(controller);

  for (int i = 0; i < controller->win_list_size; i++) {
    InitWindow(controller, i, controller->win_list_size);
  }
}

/**
 * @brief Initializes the directory context for a window with the directory
 * contents of a given dirname
 * @param dir_context - Pointer to DirectoryContext to initialize
 * @param dirname - Name of the directory to load into the DirectoryContext
 */
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