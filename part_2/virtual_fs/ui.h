#ifndef VIRTUAL_FS_UI_H
#define VIRTUAL_FS_UI_H

#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <dirent.h>

#define MAX_WINDOW_COUNT 2

#define C_UP 1
#define C_DOWN 2

typedef struct {
    DIR *current_dir;
    int selected_item;
    int size;
} wcontext_t;

typedef struct {
    WINDOW *win;
    wcontext_t wcontext;
} window_t;

typedef struct {
    window_t windows[MAX_WINDOW_COUNT];
    int current_window;
} win_controller_t;

void Run();

#endif  // VIRTUAL_FS_UI_H