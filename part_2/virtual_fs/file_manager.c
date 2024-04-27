#include "file_manager.h"

static void Wprintnw(WindowContext *win_context, const char *str, int start,
                     int end) {
  if (start < end) {
    for (int i = start; i < end - 2; i++) {
      wprintw(win_context->window, "%c", str[i]);
    }
  }
}

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

static void CurrentWinRefresh(WindowController *controller) {
  WinRefresh(&controller->win_list[controller->current_window], 1);
}

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

static void ResizeWin(WindowController *controller, int y, int x) {
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

  WindowController **global_controller = GetGlobalController();
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

static void InitFileManager(WindowController *controller) {
  controller->win_list_size = START_WINDOW_COUNT;
  InitNcurses();
  InitGlobalController(controller);
  InitControllerWindows(controller);
}

static void SwitchCurWin(WindowController *controller) {
  controller->current_window++;
  if (controller->current_window >= controller->win_list_size) {
    controller->current_window = 0;
  }
}

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

static void ChangeSelectedItem(DirectoryContext *context, int action) {
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

static void Renderer(WindowController *controller) {
  WinRefreshAll(controller);
  while (1) {
    HandleInput(getch(), controller);
  }
}

void Run() {
  WindowController controller = {0};
  InitFileManager(&controller);
  Renderer(&controller);

  SuccessfulExit(EXIT_SUCCESS);
}