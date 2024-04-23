#include "ui.h"

static void SigWinch(int signo) {
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}

static void InitDirOnWindow(wcontext_t *context){
    struct dirent *de;
    DIR *dirp = opendir(".");

    if(dirp == NULL){
        return 0;
    }
    context->current_dir = dirp;

    while((de = readdir(dirp)) != NULL) {
        context->size++;
    }
}

static void WprintAllElements(window_t *win) {
    struct dirent *de;

    for()
}

static void InitFman(win_controller_t *controller){
    initscr();
    signal(SIGWINCH, SigWinch);
    cbreak();
    noecho();
    curs_set(0);
    refresh();

    for(int i = 0; i < MAX_WINDOW_COUNT; i++){
        controller->windows[i].win = newwin(LINES, COLS / MAX_WINDOW_COUNT, 0, COLS / MAX_WINDOW_COUNT * i);
        InitDirOnWindow(&controller->windows[i].wcontext);
        box(controller->windows[i].win, '|', '-');
        wrefresh(controller->windows[i].win);
    }
}

static void FreeController(win_controller_t *controller){
    for(int i = 0; i < MAX_WINDOW_COUNT; i++){
        delwin(controller->windows[i].win);
        if(controller->windows[i].wcontext.current_dir != NULL){
            closedir(controller->windows[i].wcontext.current_dir);
        }
    }
}

static void SwitchCurWin(win_controller_t *controller) {
    controller->current_window++;
    if(controller->current_window >= MAX_WINDOW_COUNT){
        controller->current_window = 0;
    }
}

static int ChangeSelectedItem(wcontext_t *context, int action) {
    if(context->selected_item < 0 || context->selected_item > context->size){
        return 0;
    }
    
    switch (action) {
        case C_UP:
            context->selected_item++;   
            break;
        case C_DOWN:
            context->selected_item--;
            break;
    }
}

static int HandleInput(char ch, win_controller_t *controller){
    switch(ch) {
        case 'w':
            ChangeSelectedItem(&controller->windows[controller->current_window].wcontext, C_UP);
            break;
        case 's':
            ChangeSelectedItem(&controller->windows[controller->current_window].wcontext, C_DOWN);
            break;
        case '\t':
            SwitchCurWin(controller);
            break;
        case '\n':
            break;
        case 'q':
            FreeController(controller);
            endwin();
            exit(EXIT_SUCCESS);
    }
}

static void Renderer(win_controller_t *controller) {
    WINDOW *cur_win;
    while(1){
        cur_win = controller->windows[controller->current_window].win;

        wclear(cur_win);
        box(cur_win, '|', '-');
        wrefresh(cur_win);
        refresh();

        HandleInput(getch(), controller);
    }
}

void Run() {
    win_controller_t controller = {0};
    controller.current_window = 0;
    InitFman(&controller);
    Renderer(&controller);

    FreeController(&controller);
    endwin();
    exit(EXIT_SUCCESS);
}
