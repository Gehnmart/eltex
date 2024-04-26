#include "ui.h"

static win_controller_t **GetGlobalController() {
  static win_controller_t *controller = NULL;
  return &controller;
}

static void InitGlobalController(win_controller_t *controller) {
  win_controller_t **temp = GetGlobalController();
  if (*temp == NULL) {
    *temp = controller;
  } else {
    exit(EXIT_FAILURE);
  }
}

static void FreeDirList(wcontext_t *context) {
  if (context->dir_list != NULL) {
    free(context->dir_list);
  }
}

static void FreeWindow(window_t *window) {
  if(window != NULL){
    delwin(window->win);
  }
  FreeDirList(&window->wcontext);
}

static void FreeController(win_controller_t *controller) {
  for (int i = 0; i < controller->size; i++) {
    FreeWindow(&controller->windows[i]);
  }
  if(controller != NULL){
    free(controller->windows);
  }
}

static void SuccessfulExit(int exit_type) {
  win_controller_t **controller = GetGlobalController();

  FreeController(*controller);
  endwin();
  exit(exit_type);
}

static void InitDirOnWindow(wcontext_t *context, const char *dirname) {
  struct dirent **dt;
  int num_entries = scandir(dirname, &dt, NULL, alphasort);
  if (num_entries < 0) {
    context->dir_list = NULL;
    SuccessfulExit(EXIT_FAILURE);
  }
  context->dir_list = dt;
  context->selected_item = 0;
  context->size = num_entries;
}

static void Wprintnw(window_t *win, const char *str, int start, int end) {
  if (start < end) {
    for (int i = start; i < end - 2; i++) {
      wprintw(win->win, "%c", str[i]);
    }
  }
}

static void WprintAllElements(window_t *win) {
  int start_index = win->wcontext.selected_item < LINES - 2
                        ? 0
                        : win->wcontext.selected_item - (LINES - 3);
  int end_index = MIN(win->wcontext.size, start_index + LINES - 2);
  int width = getmaxx(win->win);

  for (int i = start_index; i < end_index; i++) {
    int slen = strlen(win->wcontext.dir_list[i]->d_name);
    wmove(win->win, i - start_index + 1, 1);
    if (i == win->wcontext.selected_item) {
      wattron(win->win, A_REVERSE | A_BOLD);
      if (slen > width - 2) {
        Wprintnw(win, win->wcontext.dir_list[i]->d_name, 0,
                 slen - (slen - width));
      } else {
        wprintw(win->win, "%s", win->wcontext.dir_list[i]->d_name);
      }
      wattroff(win->win, A_REVERSE | A_BOLD);
    } else {
      if (slen > width - 2) {
        Wprintnw(win, win->wcontext.dir_list[i]->d_name, 0,
                 slen - (slen - width));
      } else {
        wprintw(win->win, "%s", win->wcontext.dir_list[i]->d_name);
      }
    }
  }
  wrefresh(win->win);
}

static void WinRefresh(window_t *win, int is_current) {
  wclear(win->win);
  if (is_current) {
    wattron(win->win, A_BOLD);
    box(win->win, 0, 0);
    wattroff(win->win, A_BOLD);
  } else {
    wattron(win->win, A_DIM);
    box(win->win, 0, 0);
    wattroff(win->win, A_DIM);
  }
  WprintAllElements(win);
  wrefresh(win->win);
}

static void CurrentWinRefresh(win_controller_t *controller) {
  WinRefresh(&controller->windows[controller->current_window], 1);
}

static void WinRefreshAll(win_controller_t *controller) {
  for (int i = 0; i < controller->size; i++) {
    WINDOW *cur_win = controller->windows[i].win;
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
    WprintAllElements(&controller->windows[i]);
    wrefresh(cur_win);
  }
}

static void ResizeWin(win_controller_t *controller, int y, int x) {
  clear();
  refresh();
  for (int i = 0; i < controller->size; i++) {
    wclear(controller->windows[i].win);
    mvwin(controller->windows[i].win, 0, x / controller->size * i);
    wresize(controller->windows[i].win, y, x / controller->size);
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

static void ControllerMalloc(win_controller_t *controller) {
  if (controller->size > 0) {
    window_t *temp = calloc(sizeof(window_t), controller->size);
    if (temp == NULL) {
      SuccessfulExit(EXIT_FAILURE);
    } else {
      controller->windows = temp;
    }
  }
}

static void InitWindow(win_controller_t *controller, int index, int size) {
  window_t *window = &controller->windows[index];
  window->win = newwin(LINES, COLS / size, 0, COLS / size * index);
  if (window->win == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }

  char *err = realpath(".", window->wcontext.absolute_path);
  if (err == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }
  InitDirOnWindow(&(window->wcontext), ".");
}

static window_t *WinRealloc(win_controller_t *controller, int next_size) {
  window_t *new_windows =
      realloc(controller->windows, sizeof(window_t) * next_size);
  if (new_windows == NULL) {
    SuccessfulExit(EXIT_FAILURE);
  }

  return new_windows;
}

static void ControllerRealloc(win_controller_t *controller, int next_size) {
  if (next_size > 0 && next_size != controller->size) {
    if (next_size > controller->size) {
      controller->windows = WinRealloc(controller, next_size);
      for (int i = controller->size; i < next_size; i++) {
        InitWindow(controller, i, next_size);
      }
      controller->size = next_size;
    } else if (next_size < controller->size) {
      controller->windows = WinRealloc(controller, next_size);
      controller->size = next_size;
    }
  }
}

static void InitControllerWindows(win_controller_t *controller) {
  ControllerMalloc(controller);

  for (int i = 0; i < controller->size; i++) {
    InitWindow(controller, i, controller->size);
  }
}

static void InitFman(win_controller_t *controller) {
  InitNcurses();
  InitGlobalController(controller);
  InitControllerWindows(controller);
}

static void SwitchCurWin(win_controller_t *controller) {
  controller->current_window++;
  if (controller->current_window >= controller->size) {
    controller->current_window = 0;
  }
}

static int CurrentItemIsDirectory(window_t *win) {
  char temp_buf[PATH_MAX] = {0};
  strcpy(temp_buf, win->wcontext.absolute_path);
  AppendElemToPath(temp_buf,
                   win->wcontext.dir_list[win->wcontext.selected_item]->d_name);

  struct stat statbuf;
  if (stat(temp_buf, &statbuf) != 0) return 0;
  return S_ISDIR(statbuf.st_mode);
}

static void SwitchDir(window_t *win) {
  if (win->wcontext.dir_list[win->wcontext.selected_item] != NULL) {
    switch (win->wcontext.selected_item) {
      case 0:
        break;
      case 1:
        FreeDirList(&win->wcontext);
        DeleteEndDir(win->wcontext.absolute_path);
        InitDirOnWindow(&win->wcontext, win->wcontext.absolute_path);
        break;
      default:
        if (CurrentItemIsDirectory(win)) {
          AppendElemToPath(
              win->wcontext.absolute_path,
              win->wcontext.dir_list[win->wcontext.selected_item]->d_name);
          FreeDirList(&win->wcontext);
          InitDirOnWindow(&win->wcontext, win->wcontext.absolute_path);
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

  if (choise >= 0 && choise < context->size) {
    context->selected_item = choise;
  }
}

static void HandleInput(char ch, win_controller_t *controller) {
  int prev_win_index = controller->current_window;
  int refresh_type = 0;
  switch (ch) {
    case 'w':
      ChangeSelectedItem(
          &controller->windows[controller->current_window].wcontext, C_UP);
          refresh_type = 1;
      break;
    case 's':
      ChangeSelectedItem(
          &controller->windows[controller->current_window].wcontext, C_DOWN);
          refresh_type = 1;
      break;
    case 'n':
      if (controller->size < MAX_WINDOW_COUNT) {
        ControllerRealloc(controller, controller->size + 1);
        refresh_type = 2;
      }
      break;
    case 'd':
      if (controller->size > MIN_WINDOW_COUNT) {
        ControllerRealloc(controller, controller->size - 1);
        refresh_type = 2;
      }
      break;
    case '\t':
      SwitchCurWin(controller);
      refresh_type = 1;
      break;
    case '\n':
      SwitchDir(&controller->windows[controller->current_window]);
      refresh_type = 1;
      break;
    case 'q':
      SuccessfulExit(EXIT_SUCCESS);
  }
  if (prev_win_index != controller->current_window) {
    WinRefresh(&controller->windows[prev_win_index], 0);
    WinRefresh(&controller->windows[controller->current_window], 1);
  } else if(refresh_type == 1) {
    CurrentWinRefresh(controller);
  } else if(refresh_type == 2) {
    ResizeWin(controller, LINES, COLS);
    WinRefreshAll(controller);
  }
}

static void Renderer(win_controller_t *controller) {
  WinRefreshAll(controller);
  while (1) {
    HandleInput(getch(), controller);
    usleep(10000);
  }
}

void Run() {
  win_controller_t controller = {0};
  controller.current_window = 0;
  controller.size = 2;
  InitFman(&controller);
  Renderer(&controller);

  SuccessfulExit(EXIT_SUCCESS);
}