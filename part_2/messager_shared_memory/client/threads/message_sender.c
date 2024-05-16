#include "threads.h"

extern pthread_mutex_t g_ncurses_mutex;

void *MessageSender(void *argv) {
  MessagerCtl *ctl = (MessagerCtl *)argv;
  WINDOW      *input_win;

  pthread_mutex_lock(&g_ncurses_mutex);
  input_win = newwin(3, COLS, LINES - 3, 0);
  box(input_win, 0, 0);
  wrefresh(input_win);
  pthread_mutex_unlock(&g_ncurses_mutex);
  
  Message message = {0};
  strncpy(message.username, ctl->selfname, USERNAME_MAX);
  char msg_buf[MESSAGE_LEN_MAX] = {0};
  while(!ctl->th_stop) {
    pthread_mutex_lock(&g_ncurses_mutex);
    curs_set(TRUE);
    wmove(input_win, 1, 1);
    pthread_mutex_unlock(&g_ncurses_mutex);

    wgetnstr(input_win, msg_buf, MESSAGE_LEN_MAX - 1);

    pthread_mutex_lock(&g_ncurses_mutex);
    curs_set(FALSE);
    wclear(input_win);
    box(input_win, 0, 0);
    wrefresh(input_win);
    pthread_mutex_unlock(&g_ncurses_mutex);

    if (strncmp(msg_buf, "/exit", MESSAGE_LEN_MAX) == 0) {
      strncpy(message.text, "/exit", 6);
      for (int i = 0; i < USER_MAX; ++i) {
        User *user = &ctl->shared_data->usr_list.list[i];
        if(user->status == STAT_FREE) continue;
        sem_wait(ctl->sem_msglist);
        if(strncmp(user->username, ctl->selfname, USERNAME_MAX) == 0){
          user->status = STAT_EXIT;
        }
        sem_post(ctl->sem_msglist);
      }
      ctl->th_stop = 1;
      break;
    } else if (strlen(msg_buf) > 0) {
      strncpy(message.text, msg_buf, MESSAGE_LEN_MAX);
      sem_wait(ctl->sem_msglist);
      memcpy((void *)&ctl->shared_data->msg_list.list[ctl->shared_data->msg_list.len],
             (void *)&message, sizeof(message));
      ++ctl->shared_data->msg_list.len;
      sem_post(ctl->sem_msglist);
    }
  }

  pthread_mutex_lock(&g_ncurses_mutex);
  delwin(input_win);
  pthread_mutex_unlock(&g_ncurses_mutex);

  pthread_exit(NULL);
}