#include "ui.h"

static win_controller_t **GetGlobalController() {
  static win_controller_t *controller = NULL;
  return &controller;
}

static void InitGlobalController(win_controller_t *controller) {
  win_controller_t **temp = GetGlobalController();
  *temp = controller;
}

static void FreeDirList(wcontext_t *context) {
  if (context->dir_list != NULL) {
    for (int j = 0; j < context->size; j++) {
      free(context->dir_list[j]);
    }
    free(context->dir_list);
  }
}

static void FreeController(win_controller_t *controller) {
  for (int i = 0; i < controller->size; i++) {
    delwin(controller->windows[i].win);
    FreeDirList(&controller->windows[i].wcontext);
  }
  free(controller->windows);
}

static void SuccessfulExit(int exit_type) {
  win_controller_t **controller = GetGlobalController();

  FreeController(*controller);
  endwin();
  exit(exit_type);
}

static void InitDirOnWindow(wcontext_t *context, const char *dirname) {
  int num_entries = scandir(dirname, &(context->dir_list), NULL, alphasort);
  context->selected_item = 0;
  context->size = num_entries;
}

static void WprintAllElements(window_t *win) {
  int start_index = win->wcontext.selected_item < LINES - 2
                        ? 0
                        : win->wcontext.selected_item - (LINES - 3);
  int end_index = MIN(win->wcontext.size, start_index + LINES - 2);

  for (int i = start_index; i < end_index; i++) {
    wmove(win->win, i - start_index + 1, 1);
    if (i == win->wcontext.selected_item) {
      wattron(win->win, A_REVERSE | A_BOLD);
      wprintw(win->win, "%s", win->wcontext.dir_list[i]->d_name);
      wattroff(win->win, A_REVERSE | A_BOLD);
    } else {
      wprintw(win->win, "%s", win->wcontext.dir_list[i]->d_name);
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
  if(controller->size > 0){
    window_t *temp = calloc(sizeof(window_t), controller->size);
    if(temp == NULL) {
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
    box(window->win, 0, 0);
    wrefresh(window->win);
}

static void ControllerRealloc(win_controller_t *controller, int next_size) {
  if (next_size > 0 && next_size != controller->size) {
    if (next_size > controller->size) {
      window_t *new_windows = realloc(controller->windows, sizeof(window_t) * next_size);
      if (new_windows == NULL) {
        SuccessfulExit(EXIT_FAILURE);
      } else {
        controller->windows = new_windows;
        for (int i = controller->size; i < next_size; i++) {
          InitWindow(controller, i, next_size);
        }
        controller->size = next_size;
      }
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
          FreeDirList(&win->wcontext);
          AppendElemToPath(
              win->wcontext.absolute_path,
              win->wcontext.dir_list[win->wcontext.selected_item]->d_name);
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
  switch (ch) {
    case 'w':
      ChangeSelectedItem(
          &controller->windows[controller->current_window].wcontext, C_UP);
      break;
    case 's':
      ChangeSelectedItem(
          &controller->windows[controller->current_window].wcontext, C_DOWN);
      break;
    case 'n':
      ControllerRealloc(controller, controller->size + 1);
      ResizeWin(controller, LINES, COLS);
      WinRefreshAll(controller);
      break;
    case 'd':
      ControllerRealloc(controller, controller->size - 1);
      ResizeWin(controller, LINES, COLS);
      WinRefreshAll(controller);
      break;
    case '\t':
      SwitchCurWin(controller);
      break;
    case '\n':
      SwitchDir(&controller->windows[controller->current_window]);
      break;
    case 'q':
      SuccessfulExit(EXIT_SUCCESS);
  }
  if(prev_win_index != controller->current_window) {
    WinRefresh(&controller->windows[prev_win_index], 0);
    WinRefresh(&controller->windows[controller->current_window], 1);
  } else {
    CurrentWinRefresh(controller);
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
  controller.current_window = 0;
  controller.size = 2;
  InitFman(&controller);
  Renderer(&controller);

  SuccessfulExit(EXIT_SUCCESS);
}