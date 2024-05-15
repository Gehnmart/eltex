#include "threads.h"

extern pthread_mutex_t g_ncurses_mutex;\

int Min(int a, int b) { return a > b ? b : a; }

void UpdateChatWindow(WINDOW *message_win, MessagerCtl *ctl) {
  pthread_mutex_lock(&g_ncurses_mutex);
  int maxy = getmaxy(message_win) - 2;
  int list_len = ctl->shared_data->msg_list.len;
  int start = list_len > maxy ? list_len - maxy : 0;
  int end = Min(list_len, maxy + start);
  int index = 1;
  for (int i = start; i < end; i++) {
    wclrtobot(message_win);
    wmove(message_win, index++, 1);
    wprintw(message_win, "%s: %s", ctl->shared_data->msg_list.list[i].username,
            ctl->shared_data->msg_list.list[i].text);
  }
  box(message_win, 0, 0);
  wrefresh(message_win);
  pthread_mutex_unlock(&g_ncurses_mutex);
}

void *MessageReceiver(void *argv) {
  MessagerCtl *ctl = (MessagerCtl *)argv;
  WINDOW      *message_win;
  int prev_msglen = 0;

  pthread_mutex_lock(&g_ncurses_mutex);
  message_win = newwin(LINES - 3, COLS - USERNAME_MAX, 0, 0);
  box(message_win, 0, 0);
  wrefresh(message_win);
  pthread_mutex_unlock(&g_ncurses_mutex);

  while(!ctl->th_stop){
    usleep(10000);
    if(prev_msglen == ctl->shared_data->msg_list.len)
      continue;
    else
      prev_msglen = ctl->shared_data->msg_list.len;
    sem_wait(ctl->sem_client);
    UpdateChatWindow(message_win, ctl);
    sem_post(ctl->sem_client);
  }

  pthread_mutex_lock(&g_ncurses_mutex);
  delwin(message_win);
  pthread_mutex_unlock(&g_ncurses_mutex);
  pthread_exit(NULL);
}