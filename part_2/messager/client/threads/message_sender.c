#include "threads.h"

void *MessageSender(void *argv) {
  ClientController *controller = (ClientController *)argv;

  mqd_t chat_mq = mq_open(CHAT_MQ, O_WRONLY);
  if (chat_mq < 0) {
    PrintError("mq_open", __LINE__);
    controller->client_stop = 1;
    return NULL;
  }

  pthread_mutex_lock(&controller->ncurses_mutex);
  WINDOW *input_win = newwin(3, COLS, LINES - 3, 0);
  box(input_win, 0, 0);
  wrefresh(input_win);
  pthread_mutex_unlock(&controller->ncurses_mutex);

  char message_buf[MESSAGE_LEN_MAX] = {0};
  Message text = {0};
  strncpy(text.user, controller->user->name, USERNAME_MAX - 1);
  while (!controller->client_stop) {
    pthread_mutex_lock(&controller->ncurses_mutex);
    curs_set(TRUE);
    wmove(input_win, 1, 1);
    pthread_mutex_unlock(&controller->ncurses_mutex);

    wgetnstr(input_win, message_buf, MESSAGE_LEN_MAX - 1);

    pthread_mutex_lock(&controller->ncurses_mutex);
    curs_set(FALSE);
    wclear(input_win);
    box(input_win, 0, 0);
    wrefresh(input_win);
    pthread_mutex_unlock(&controller->ncurses_mutex);

    if (strncmp(message_buf, "/exit", MESSAGE_LEN_MAX) == 0) {
      strncpy(text.text, "/exit", 6);
      mq_send(chat_mq, (char *)&text, sizeof(Message), 0);
      controller->client_stop = 1;
      break;
    } else if (strlen(message_buf) > 0) {
      strncpy(text.text, message_buf, MESSAGE_LEN_MAX);
      mq_send(chat_mq, (char *)&text, sizeof(Message), 0);
    }
  }
  if (mq_close(chat_mq) < 0) {
    PrintError("mq_close", __LINE__);
  }
  if (mq_unlink(controller->user->name) < 0) {
    PrintError("mq_unlink", __LINE__);
  }
  pthread_mutex_lock(&controller->ncurses_mutex);
  delwin(input_win);
  pthread_mutex_unlock(&controller->ncurses_mutex);

  pthread_exit(NULL);
}