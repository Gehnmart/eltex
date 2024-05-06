#include "threads.h"

extern pthread_mutex_t g_mutex_ncurses;
extern char g_stop;
extern int g_new_user_flag;

int AddUser(MessagerController *controller, char *username) {
  g_new_user_flag = 1;
  UserList *user_list = controller->user_list;
  char ret_status = FAILURE;
  pthread_mutex_lock(&user_list->mutex);
  for (int i = 0; i < USER_MAX; ++i) {
    User *user = &user_list->users[i];
    if (user->name[0] == 0) {
      strncpy(user->name, username, USERNAME_MAX - 1);
      ret_status = SUCCESS;
      break;
    }
  }
  user_list->len++;
  pthread_mutex_unlock(&user_list->mutex);

  return ret_status;
}

int DelUser(MessagerController *controller, char *username) {
  g_new_user_flag = 1;
  UserList *user_list = controller->user_list;
  char ret_status = SUCCESS;
  pthread_mutex_lock(&user_list->mutex);
  for (int i = 0; i < USER_MAX; ++i) {
    User *user = &user_list->users[i];
    if (user->name[0] == 0) continue;
    if (strncmp(user->name, username, NAME_MAX - 1) == 0) {
      memset(user->name, 0, sizeof(user->name));
      break;
    }
  }
  user_list->len--;
  pthread_mutex_unlock(&user_list->mutex);

  return ret_status;
}

void *MessageReceiver(void *argv) {
  MessagerController *controller = (MessagerController *)argv;

  pthread_mutex_lock(&g_mutex_ncurses);
  WINDOW *chat_win = newwin(LINES - 3, COLS - USERNAME_MAX, 0, 0);
  box(chat_win, 0, 0);
  wrefresh(chat_win);
  pthread_mutex_unlock(&g_mutex_ncurses);

  while (!g_stop) {
    Message message = {0};
    int byte_read = mq_receive(controller->user->user_mq, (char *)&message,
                               sizeof(Message), NULL);
    if (byte_read < 0) {
      if (errno == EAGAIN) {
        usleep(TIMEOUT);
        continue;
      }
      PrintError("mq_receive", __LINE__);
      break;
    }
    memcpy((void *)&controller->message_list
               ->messages[controller->message_list->len],
           (void *)&message, sizeof(Message));
    controller->message_list->len++;

    if (message.metadata.data.type == APPEND_USER) {
      AddUser(controller, message.metadata.data.username);
    } else if (message.metadata.data.type == DELETE_USER) {
      DelUser(controller, message.metadata.data.username);
    }

    pthread_mutex_lock(&g_mutex_ncurses);
    int maxy = getmaxy(chat_win) - 2;
    int list_len = controller->message_list->len;
    int start = list_len > maxy ? list_len - maxy : 0;
    int end = Min(list_len, maxy + start);
    int index = 1;
    for (int i = start; i < end; i++) {
      wmove(chat_win, index++, 1);
      wprintw(chat_win, "%s: %s", controller->message_list->messages[i].user,
              controller->message_list->messages[i].text);
    }
    wrefresh(chat_win);
    pthread_mutex_unlock(&g_mutex_ncurses);
  }
  pthread_mutex_lock(&g_mutex_ncurses);
  delwin(chat_win);
  pthread_mutex_unlock(&g_mutex_ncurses);
  return NULL;
}