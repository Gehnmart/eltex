#include "threads.h"

void UpdateChatWindow(WINDOW *chat_win, ClientController *controller) {
  pthread_mutex_lock(&controller->ncurses_mutex);
  int maxy = getmaxy(chat_win) - 2;
  int list_len = controller->message_list->len;
  int start = list_len > maxy ? list_len - maxy : 0;
  int end = Min(list_len, maxy + start);
  int index = 1;
  for (int i = start; i < end; i++) {
    wclrtobot(chat_win);
    wmove(chat_win, index++, 1);
    wprintw(chat_win, "%s: %s", controller->message_list->messages[i].user,
            controller->message_list->messages[i].text);
  }
  box(chat_win, 0, 0);
  wrefresh(chat_win);
  pthread_mutex_unlock(&controller->ncurses_mutex);
}

int AddUser(ClientController *controller, char *username) {
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

int DelUser(ClientController *controller, char *username) {
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
  ClientController *controller = (ClientController *)argv;

  pthread_mutex_lock(&controller->ncurses_mutex);
  WINDOW *chat_win = newwin(LINES - 3, COLS - USERNAME_MAX, 0, 0);
  box(chat_win, 0, 0);
  wrefresh(chat_win);
  pthread_mutex_unlock(&controller->ncurses_mutex);

  while (!controller->client_stop) {
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

    UpdateChatWindow(chat_win, controller);
  }
  pthread_mutex_lock(&controller->ncurses_mutex);
  delwin(chat_win);
  pthread_mutex_unlock(&controller->ncurses_mutex);
  return NULL;
}