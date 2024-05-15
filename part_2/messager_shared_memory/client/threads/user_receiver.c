#include "threads.h"

extern pthread_mutex_t g_ncurses_mutex;

void *UserReceiver(void *argv) {
  MessagerCtl *ctl = (MessagerCtl *)argv;
  WINDOW      *user_win;
  int prev_usrlen = 0;

  pthread_mutex_lock(&g_ncurses_mutex);
  user_win = newwin(LINES - 3, USERNAME_MAX, 0, COLS - USERNAME_MAX);
  box(user_win, 0, 0);
  wrefresh(user_win);
  pthread_mutex_unlock(&g_ncurses_mutex);

  while (!ctl->th_stop) {
    usleep(10000);
    sem_wait(ctl->sem_client);
    pthread_mutex_lock(&g_ncurses_mutex);
    wclear(user_win);
    box(user_win, 0, 0);
    wrefresh(user_win);
    int index = 1;
    for (int i = 0; i < USER_MAX; ++i) {
      User *user = &ctl->shared_data->usr_list.list[i];
      if (user->status != STAT_EXEC) {
        continue;
      }
      wmove(user_win, index++, 1);
      wprintw(user_win, "%s", ctl->shared_data->usr_list.list[i].username);
    }
    wrefresh(user_win);
    pthread_mutex_unlock(&g_ncurses_mutex);
    sem_post(ctl->sem_client);
  }

  pthread_exit(NULL);
}