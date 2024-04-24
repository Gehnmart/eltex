#include "ui.h"

static void WinRefreshAll(win_controller_t *controller);

static win_controller_t **get_global_controller() {
    static win_controller_t *controller = NULL;
    return &controller;
}

static void SigWinch(int signo) {
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
    resizeterm(size.ws_row, size.ws_col);
    WinRefreshAll(*get_global_controller());
}

static void FreeDirList(wcontext_t *context){
    if (context->dir_list != NULL) {
        for (int j = 0; j < context->size; j++) {
            free(context->dir_list[j]);
        }
        free(context->dir_list);
    }
}

static void FreeController(win_controller_t *controller) {
    for (int i = 0; i < MAX_WINDOW_COUNT; i++) {
        delwin(controller->windows[i].win);
        FreeDirList(&controller->windows[i].wcontext);
    }
}

static void SuccesfulExit(win_controller_t *controller, int exit_type) {
    FreeController(controller);
    endwin();
    exit(exit_type);
}

static void InitDirOnWindow(wcontext_t *context, const char *dirname) {
    int num_entries = scandir(dirname, &(context->dir_list), NULL, alphasort);
    context->selected_item = 0;
    context->size = num_entries;
}

static void WprintAllElements(window_t *win) {
    int i = win->wcontext.selected_item < LINES - 2
                ? 0
                : -(LINES - 3 - win->wcontext.selected_item);
    int offset = i;
    int line = 1;
    wmove(win->win, 0, 1);
    for (; i < win->wcontext.size && i - offset < LINES - 2; i++) {
        wmove(win->win, line++, 1);
        if (i == win->wcontext.selected_item) {
            wattron(win->win, A_REVERSE);
            wprintw(win->win, "%s", win->wcontext.dir_list[i]->d_name);
            wattroff(win->win, A_REVERSE);
        } else {
            wprintw(win->win, "%s", win->wcontext.dir_list[i]->d_name);
        }
    }
    wrefresh(win->win);
}

static void InitFman(win_controller_t *controller) {
    initscr();
    signal(SIGWINCH, SigWinch);
    cbreak();
    noecho();
    curs_set(0);
    refresh();

    win_controller_t **temp = get_global_controller();
    *temp = controller;

    for (int i = 0; i < MAX_WINDOW_COUNT; i++) {
        controller->windows[i].win =
            newwin(LINES, COLS / MAX_WINDOW_COUNT, 0, COLS / MAX_WINDOW_COUNT * i);

        realpath(".", controller->windows[i].wcontext.absolute_path);
        InitDirOnWindow(&controller->windows[i].wcontext, ".");
        box(controller->windows[i].win, 0, 0);
        wrefresh(controller->windows[i].win);
    }
}

static void SwitchCurWin(win_controller_t *controller) {
    controller->current_window++;
    if (controller->current_window >= MAX_WINDOW_COUNT) {
        controller->current_window = 0;
    }
}

static int CurrentItemIsDirectory(window_t *win) {
    char temp_buf[PATH_MAX] = {0};
    strcpy(temp_buf, win->wcontext.absolute_path);
    strcat(temp_buf, "/");
    strcat(temp_buf, win->wcontext.dir_list[win->wcontext.selected_item]->d_name);

    struct stat statbuf;
    if (stat(temp_buf, &statbuf) != 0) return 0;
    return S_ISDIR(statbuf.st_mode);
}

static void DeleteEndDir(char *path) {
    char ch = 0;
    int i = strlen(path) - 1;
    for (; i > 0 && path[i] != '/'; i--) {
        path[i] = '\0';
    }
    path[i] = '\0';
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
                    strcat(win->wcontext.absolute_path, "/");
                    strcat(win->wcontext.absolute_path,
                            win->wcontext.dir_list[win->wcontext.selected_item]->d_name);
                    InitDirOnWindow(&win->wcontext, win->wcontext.absolute_path);
                }
                break;
        }
    }
}

static int ChangeSelectedItem(wcontext_t *context, int action) {
    int choise = context->selected_item;

    switch (action) {
        case C_UP:
            choise--;
            break;
        case C_DOWN:
            choise++;
            break;
    }

    if (choise < 0 || choise >= context->size) {
        return 0;
    } else {
        context->selected_item = choise;
    }
}

static void WinRefresh(win_controller_t *controller) {
    WINDOW *cur_win = controller->windows[controller->current_window].win;
    wclear(cur_win);
    box(cur_win, 0, 0);
    WprintAllElements(&controller->windows[controller->current_window]);
    wrefresh(cur_win);
}

static void WinRefreshAll(win_controller_t *controller) {
    for(int i = 0; i < MAX_WINDOW_COUNT; i++){
        WINDOW *cur_win = controller->windows[i].win;
        wclear(cur_win);
        if(i == controller->current_window){
            wattron(cur_win, A_BLINK);
            box(cur_win, 0, 0);
            wattroff(cur_win, A_BLINK);
        } else {
            box(cur_win, 0, 0);
        }
        WprintAllElements(&controller->windows[i]);
        wrefresh(cur_win);
    }
}

static int HandleInput(char ch, win_controller_t *controller) {
    switch (ch) {
        case 'w':
            ChangeSelectedItem(
                &controller->windows[controller->current_window].wcontext, C_UP);
            break;
        case 's':
            ChangeSelectedItem(
                &controller->windows[controller->current_window].wcontext, C_DOWN);
            break;
        case '\t':
            SwitchCurWin(controller);
            break;
        case '\n':
            SwitchDir(&controller->windows[controller->current_window]);
            break;
        case 'q':
            SuccesfulExit(controller, EXIT_SUCCESS);
    }

    WinRefresh(controller);
}

static void Renderer(win_controller_t *controller) {
    while (1) {
        WinRefreshAll(controller);
        HandleInput(getch(), controller);
    }
}

void Run() {
    win_controller_t controller = {0};
    controller.current_window = 0;
    InitFman(&controller);
    Renderer(&controller);

    SuccesfulExit(&controller, EXIT_SUCCESS);
}