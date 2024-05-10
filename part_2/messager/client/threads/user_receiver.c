#include "threads.h"

void *UserReceiver(void *argv) {
  ClientController *controller = (ClientController *)argv;

  pthread_mutex_lock(&controller->ncurses_mutex);
  WINDOW *user_win = newwin(LINES - 3, USERNAME_MAX, 0, COLS - USERNAME_MAX);
  box(user_win, 0, 0);
  wrefresh(user_win);
  pthread_mutex_unlock(&controller->ncurses_mutex);

  int prev_user_list_len = 0;
  while (!controller->client_stop) {
    pthread_mutex_lock(&controller->user_list->mutex);
    if (prev_user_list_len != controller->user_list->len) {
      prev_user_list_len = controller->user_list->len;
      pthread_mutex_lock(&controller->ncurses_mutex);
      wclear(user_win);
      box(user_win, 0, 0);
      wrefresh(user_win);
      int index = 1;
      for (int i = 0; i < USER_MAX; ++i) {
        if (controller->user_list->users[i].name[0] == 0) {
          continue;
        }
        wmove(user_win, index++, 1);
        wprintw(user_win, "%s", controller->user_list->users[i].name);
      }
      wrefresh(user_win);
      pthread_mutex_unlock(&controller->ncurses_mutex);
    }
    pthread_mutex_unlock(&controller->user_list->mutex);
    usleep(TIMEOUT);
  }

  pthread_mutex_lock(&controller->ncurses_mutex);
  delwin(user_win);
  pthread_mutex_unlock(&controller->ncurses_mutex);

  pthread_exit(NULL);
}