#include "threads.h"

extern pthread_mutex_t g_mutex_ncurses;
extern char g_stop;
extern int g_new_user_flag;

void *UserReceiver(void *argv) {
  MessagerController *controller = (MessagerController *)argv;

  pthread_mutex_lock(&g_mutex_ncurses);
  WINDOW *user_win = newwin(LINES - 3, USERNAME_MAX, 0, COLS - USERNAME_MAX);
  box(user_win, 0, 0);
  wrefresh(user_win);
  pthread_mutex_unlock(&g_mutex_ncurses);

  while (!g_stop) {
    pthread_mutex_lock(&controller->user_list->mutex);
    if (g_new_user_flag) {
      g_new_user_flag = 0;
      wclear(user_win);
      box(user_win, 0, 0);
      wrefresh(user_win);
      pthread_mutex_lock(&g_mutex_ncurses);
      int index = 1;
      for (int i = 0; i < USER_MAX; ++i) {
        if (controller->user_list->users[i].name[0] == 0) {
          continue;
        }
        wmove(user_win, index++, 1);
        wprintw(user_win, "%s", controller->user_list->users[i].name);
      }
      wrefresh(user_win);
      pthread_mutex_unlock(&g_mutex_ncurses);
    }
    pthread_mutex_unlock(&controller->user_list->mutex);
    usleep(TIMEOUT);
  }

  pthread_mutex_lock(&g_mutex_ncurses);
  delwin(user_win);
  pthread_mutex_unlock(&g_mutex_ncurses);

  return NULL;
}