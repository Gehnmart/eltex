#include "file_manager.h"

static void Wprintnw(window_t *window, const char *str, int start, int end) {
  if (start < end) {
    for (int i = start; i < end - 2; i++) {
      wprintw(window->window, "%c", str[i]);
    }
  }
}

static void WprintAllElements(window_t *window) {
  int start_index = window->wcontext.selected_item < LINES - 2
                        ? 0
                        : window->wcontext.selected_item - (LINES - 3);
  int end_index = MIN(window->wcontext.dir_list_size, start_index + LINES - 2);
  int width = getmaxx(window->window);

  for (int i = start_index; i < end_index; i++) {
    int slen = strlen(window->wcontext.dir_list[i]->d_name);
    wmove(window->window, i - start_index + 1, 1);
    if (i == window->wcontext.selected_item) {
      wattron(window->window, A_REVERSE | A_BOLD);
      if (slen > width - 2) {
        Wprintnw(window, window->wcontext.dir_list[i]->d_name, 0,
                 slen - (slen - width));
      } else {
        wprintw(window->window, "%s", window->wcontext.dir_list[i]->d_name);
      }
      wattroff(window->window, A_REVERSE | A_BOLD);
    } else {
      if (slen > width - 2) {
        Wprintnw(window, window->wcontext.dir_list[i]->d_name, 0,
                 slen - (slen - width));
      } else {
        wprintw(window->window, "%s", window->wcontext.dir_list[i]->d_name);
      }
    }
  }
  wrefresh(window->window);
}

static void WinRefresh(window_t *window, int is_current) {
  wclear(window->window);
  if (is_current) {
    wattron(window->window, A_BOLD);
    box(window->window, 0, 0);
    wattroff(window->window, A_BOLD);
  } else {
    wattron(window->window, A_DIM);
    box(window->window, 0, 0);
    wattroff(window->window, A_DIM);
  }
  WprintAllElements(window);
  wrefresh(window->window);
}

static void CurrentWinRefresh(win_controller_t *controller) {
  WinRefresh(&controller->win_list[controller->current_window], 1);
}

static void WinRefreshAll(win_controller_t *controller) {
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

static void ResizeWin(win_controller_t *controller, int y, int x) {
  clear();
  refresh();
  for (int i = 0; i < controller->win_list_size; i++) {
    wclear(controller->win_list[i].window);
    mvwin(controller->win_list[i].window, 0, x / controller->win_list_size * i);
    wresize(controller->win_list[i].window, y, x / controller->win_list_size);
  }
}

static void SigWinch(int signo) {
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

  resizeterm(size.ws_row, size.ws_col);

  win_controller_t **global_controller = GetGlobalController();
  ResizeWin(*global_controller, size.ws_row, size.ws_col);
  WinRefreshAll(*global_controller);
}

static void InitNcurses() {
  initscr();
  signal(SIGWINCH, SigWinch);
  cbreak();
  noecho();
  curs_set(0);
  refresh();
}

static void InitFileManager(win_controller_t *controller) {
  controller->win_list_size = START_WINDOW_COUNT;
  InitNcurses();
  InitGlobalController(controller);
  InitControllerWindows(controller);
}

static void SwitchCurWin(win_controller_t *controller) {
  controller->current_window++;
  if (controller->current_window >= controller->win_list_size) {
    controller->current_window = 0;
  }
}

static void SwitchDir(window_t *window) {
  if (window->wcontext.dir_list[window->wcontext.selected_item] != NULL) {
    switch (window->wcontext.selected_item) {
      case 0:
        break;
      case 1:
        FreeDirList(&window->wcontext);
        DeleteEndDir(window->wcontext.absolute_path);
        InitDirOnWindow(&window->wcontext, window->wcontext.absolute_path);
        break;
      default:
        if (IsDirectory(
                window->wcontext.absolute_path,
                window->wcontext.dir_list[window->wcontext.selected_item]->d_name)) {
          AppendElemToPath(
              window->wcontext.absolute_path,
              window->wcontext.dir_list[window->wcontext.selected_item]->d_name);
          FreeDirList(&window->wcontext);
          InitDirOnWindow(&window->wcontext, window->wcontext.absolute_path);
        }
        break;
    }
  }
}

static void ChangeSelectedItem(wcontext_t *context, int action) {
  int choise = context->selected_item;

  switch (action) {
    case C_UP:
      choise--;
      break;
    case C_DOWN:
      choise++;
      break;
  }

  if (choise >= 0 && choise < context->dir_list_size) {
    context->selected_item = choise;
  }
}

static void HandleInput(char ch, win_controller_t *controller) {
  int prev_win_index = controller->current_window;
  int refresh_type = 0;
  switch (ch) {
    case 'w':
      ChangeSelectedItem(
          &controller->win_list[controller->current_window].wcontext, C_UP);
      refresh_type = REFRESH_CURRENT;
      break;
    case 's':
      ChangeSelectedItem(
          &controller->win_list[controller->current_window].wcontext, C_DOWN);
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

static void Renderer(win_controller_t *controller) {
  WinRefreshAll(controller);
  while (1) {
    HandleInput(getch(), controller);
  }
}

void Run() {
  win_controller_t controller = {0};
  InitFileManager(&controller);
  Renderer(&controller);

  SuccessfulExit(EXIT_SUCCESS);
}