#include "file_manager.h"

/**
 * @brief Prints a substring of 'str' from 'start' to 'end - 2' on the ncurses window.
 * why im using 2 magic constant unknow it but 2 is border symbols!
 * @param win_context Pointer to the window context.
 * @param str The string to print.
 * @param start The starting index from where to begin printing.
 * @param end The ending index where to stop printing.
 * 
 */
static void Wprintnw(WindowContext *win_context, const char *str, int start,
                     int end) {
  if (start < end) {
    for (int i = start; i < end - 2; i++) {
      wprintw(win_context->window, "%c", str[i]);
    }
  }
}

/**
 * @brief Prints all elements of the directory list within the window context.
 * @param win_context Pointer to the window context.
 */
static void WprintAllElements(WindowContext *win_context) {
  int start_index = win_context->dir_context.selected_item < LINES - 2
                        ? 0
                        : win_context->dir_context.selected_item - (LINES - 3);
  int end_index =
      MIN(win_context->dir_context.dir_list_size, start_index + LINES - 2);
  int width = getmaxx(win_context->window);

  for (int i = start_index; i < end_index; i++) {
    int slen = strlen(win_context->dir_context.dir_list[i]->d_name);
    wmove(win_context->window, i - start_index + 1, 1);
    if (i == win_context->dir_context.selected_item) {
      wattron(win_context->window, A_REVERSE | A_BOLD);
      if (slen > width - 2) {
        Wprintnw(win_context, win_context->dir_context.dir_list[i]->d_name, 0,
                 slen - (slen - width));
      } else {
        wprintw(win_context->window, "%s",
                win_context->dir_context.dir_list[i]->d_name);
      }
      wattroff(win_context->window, A_REVERSE | A_BOLD);
    } else {
      if (slen > width - 2) {
        Wprintnw(win_context, win_context->dir_context.dir_list[i]->d_name, 0,
                 slen - (slen - width));
      } else {
        wprintw(win_context->window, "%s",
                win_context->dir_context.dir_list[i]->d_name);
      }
    }
  }
  wrefresh(win_context->window);
}

/**
 * @brief Clears and refreshes the window with updated content.
 * @param win_context Pointer to the window context.
 * @param is_current Flag indicating if this is the currently selected window.
 */
static void WinRefresh(WindowContext *win_context, int is_current) {
  wclear(win_context->window);
  if (is_current) {
    wattron(win_context->window, A_BOLD);
    box(win_context->window, 0, 0);
    wattroff(win_context->window, A_BOLD);
  } else {
    wattron(win_context->window, A_DIM);
    box(win_context->window, 0, 0);
    wattroff(win_context->window, A_DIM);
  }
  WprintAllElements(win_context);
}

/**
 * @brief Refreshes the currently selected window.
 * @param controller Pointer to the window controller.
 */
static void CurrentWinRefresh(WindowController *controller) {
  WinRefresh(&controller->win_list[controller->current_window], 1);
}

/**
 * @brief Refreshes all windows managed by the controller.
 * @param controller Pointer to the window controller.
 */
static void WinRefreshAll(WindowController *controller) {
  for (int i = 0; i < controller->win_list_size; i++) {
    WINDOW *cur_win = controller->win_list[i].window;
    wclear(cur_win);
    if (i == controller->current_window) {
      wattron(cur_win, A_BOLD);
      box(cur_win, 0, 0);
      wattroff(cur_win, A_BOLD);
    } else {
      wattron(cur_win, A_DIM);
      box(cur_win, 0, 0);
      wattroff(cur_win, A_DIM);
    }
    WprintAllElements(&controller->win_list[i]);
    wrefresh(cur_win);
  }
}

/**
 * @brief Resizes all windows managed by the controller based on the new terminal dimensions.
 * @param controller Pointer to the window controller.
 * @param y New height of the window.
 * @param x New width of the window.
 */
static void ResizeWin(WindowController *controller, int y, int x) {
  clear();
  refresh();
  for (int i = 0; i < controller->win_list_size; i++) {
    wclear(controller->win_list[i].window);
    mvwin(controller->win_list[i].window, 0, x / controller->win_list_size * i);
    wresize(controller->win_list[i].window, y, x / controller->win_list_size);
  }
}

/**
 * @brief Signal handler for window size change (SIGWINCH).
 * @param signo Signal number.
 */
static void SigWinch(int signo) {
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

  resizeterm(size.ws_row, size.ws_col);

  WindowController **global_controller = GetGlobalController();
  ResizeWin(*global_controller, size.ws_row, size.ws_col);
  WinRefreshAll(*global_controller);
}

/**
 * @brief Initializes the ncurses library and sets up signal handling.
 */

static void InitNcurses() {
  initscr();
  signal(SIGWINCH, SigWinch);
  cbreak();
  noecho();
  curs_set(0);
  refresh();
}

/**
 * @brief Initializes the file manager and its windows.
 * @param controller Pointer to the window controller.
 */
static void InitFileManager(WindowController *controller) {
  controller->win_list_size = START_WINDOW_COUNT;
  InitNcurses();
  InitGlobalController(controller);
  InitControllerWindows(controller);
}

/**
 * @brief Switches the current window to the next one in the controller.
 * @param controller Pointer to the window controller.
 */
static void SwitchCurWin(WindowController *controller) {
  controller->current_window++;
  if (controller->current_window >= controller->win_list_size) {
    controller->current_window = 0;
  }
}

/**
 * @brief Handles changing the directory or navigating up a directory level.
 * @param win_context Pointer to the window context.
 */
static void SwitchDir(WindowContext *win_context) {
  if (win_context->dir_context
          .dir_list[win_context->dir_context.selected_item] != NULL) {
    switch (win_context->dir_context.selected_item) {
      case 0:
        break;
      case 1:
        FreeDirList(&win_context->dir_context);
        DeleteEndDir(win_context->dir_context.absolute_path);
        InitDirOnWindow(&win_context->dir_context,
                        win_context->dir_context.absolute_path);
        break;
      default:
        if (IsDirectory(win_context->dir_context.absolute_path,
                        win_context->dir_context
                            .dir_list[win_context->dir_context.selected_item]
                            ->d_name)) {
          AppendElemToPath(win_context->dir_context.absolute_path,
                           win_context->dir_context
                               .dir_list[win_context->dir_context.selected_item]
                               ->d_name);
          FreeDirList(&win_context->dir_context);
          InitDirOnWindow(&win_context->dir_context,
                          win_context->dir_context.absolute_path);
        }
        break;
    }
  }
}

/**
 * @brief Changes the selected item in a directory context based on the action.
 * @param dir_context Pointer to the directory dir_context.
 * @param action The action to be taken (e.g., move up or down).
 */
static void ChangeSelectedItem(DirectoryContext *dir_context, int action) {
  int choise = dir_context->selected_item;

  switch (action) {
    case C_UP:
      choise--;
      break;
    case C_DOWN:
      choise++;
      break;
  }

  if (choise >= 0 && choise < dir_context->dir_list_size) {
    dir_context->selected_item = choise;
  }
}

/**
 * @brief Handles user input and performs actions based on the input character.
 * @param ch The input character.
 * @param controller Pointer to the window controller.
 */
static void HandleInput(char ch, WindowController *controller) {
  int prev_win_index = controller->current_window;
  int refresh_type = 0;
  switch (ch) {
    case 'w':
      ChangeSelectedItem(
          &controller->win_list[controller->current_window].dir_context, C_UP);
      refresh_type = REFRESH_CURRENT;
      break;
    case 's':
      ChangeSelectedItem(
          &controller->win_list[controller->current_window].dir_context,
          C_DOWN);
      refresh_type = REFRESH_CURRENT;
      break;
    case 'n':
      if (controller->win_list_size < MAX_WINDOW_COUNT) {
        ControllerRealloc(controller, controller->win_list_size + 1);
        refresh_type = REFRESH_ALL;
      }
      break;
    case 'd':
      if (controller->win_list_size > MIN_WINDOW_COUNT) {
        ControllerRealloc(controller, controller->win_list_size - 1);
        refresh_type = REFRESH_ALL;
      }
      break;
    case '\t':
      SwitchCurWin(controller);
      refresh_type = REFRESH_CURRENT_AND_PREV;
      break;
    case '\n':
      SwitchDir(&controller->win_list[controller->current_window]);
      refresh_type = REFRESH_CURRENT;
      break;
    case 'q':
      SuccessfulExit(EXIT_SUCCESS);
  }
  switch (refresh_type) {
    case REFRESH_ALL:
      ResizeWin(controller, LINES, COLS);
      WinRefreshAll(controller);
      break;
    case REFRESH_CURRENT_AND_PREV:
      WinRefresh(&controller->win_list[prev_win_index], 0);
      CurrentWinRefresh(controller);
      break;
    case REFRESH_CURRENT:
      CurrentWinRefresh(controller);
      break;
  }
}

/**
 * @brief Main render loop for the file manager, handles input and window refreshing.
 * @param controller Pointer to the window controller.
 */
static void Renderer(WindowController *controller) {
  WinRefreshAll(controller);
  while (1) {
    HandleInput(getch(), controller);
  }
}

/**
 * @brief Entry point function for running the file manager.
 */
void Run() {
  WindowController controller = {0};
  InitFileManager(&controller);
  Renderer(&controller);

  SuccessfulExit(EXIT_SUCCESS);
}