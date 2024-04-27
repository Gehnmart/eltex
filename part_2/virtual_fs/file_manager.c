#include "file_manager.h"

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

static void InitFileManager(win_controller_t *controller) {
  controller->size = START_WINDOW_COUNT;
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
        if (IsDirectory(win->wcontext.absolute_path, win->wcontext.dir_list[win->wcontext.selected_item]->d_name)) {
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
      refresh_type = REFRESH_CURRENT;
      break;
    case 's':
      ChangeSelectedItem(
          &controller->windows[controller->current_window].wcontext, C_DOWN);
      refresh_type = REFRESH_CURRENT;
      break;
    case 'n':
      if (controller->size < MAX_WINDOW_COUNT) {
        ControllerRealloc(controller, controller->size + 1);
        refresh_type = REFRESH_ALL;
      }
      break;
    case 'd':
      if (controller->size > MIN_WINDOW_COUNT) {
        ControllerRealloc(controller, controller->size - 1);
        refresh_type = REFRESH_ALL;
      }
      break;
    case '\t':
      SwitchCurWin(controller);
      refresh_type = REFRESH_CURRENT_AND_PREV;
      break;
    case '\n':
      SwitchDir(&controller->windows[controller->current_window]);
      refresh_type = REFRESH_CURRENT;
      break;
    case 'q':
      SuccessfulExit(EXIT_SUCCESS);
  }
  switch(refresh_type){
    case REFRESH_ALL:
      ResizeWin(controller, LINES, COLS);
      WinRefreshAll(controller);
      break;
    case REFRESH_CURRENT_AND_PREV:
      WinRefresh(&controller->windows[prev_win_index], 0);
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