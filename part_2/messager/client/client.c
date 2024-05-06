#include "client.h"

#include "../general_resource.h"

pthread_mutex_t g_mutex_ncurses = PTHREAD_MUTEX_INITIALIZER;
char g_stop;
int g_new_user_flag = 1;

int Register(User *user) {
  WINDOW *register_win;
  char status = SUCCESS;
  mqd_t register_mq;
  struct mq_attr attr = {0, 10, sizeof(Message), 0};
  int flags = O_RDWR | O_CREAT | O_NONBLOCK;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  register_mq = mq_open(REGISTER_MQ, O_WRONLY);
  if (register_mq < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_open: %s\n", __LINE__, strerror(errno));
    return FAILURE;
  }
  register_win = newwin(3, USERNAME_MAX, LINES / 2, COLS / USERNAME_MAX / 2);

  while (!g_stop) {
    box(register_win, 0, 0);
    wrefresh(register_win);
    curs_set(TRUE);
    wmove(register_win, 0, 1);
    wprintw(register_win, "Enter the username:");
    wmove(register_win, 1, 1);
    wgetnstr(register_win, user->name, USERNAME_MAX - 1);
    curs_set(FALSE);
    wrefresh(register_win);

    if (strncmp(user->name, "/exit", 6) == 0) {
      status = FAILURE;
      break;
    }

    char temp[BUF_MAX] = {0};
    temp[0] = '/';
    strcpy(temp + 1, user->name);
    strcpy(user->name, temp);
    printf("%s\n", user->name);
    user->user_mq = mq_open(user->name, flags, mode, &attr);
    if (user->user_mq < 0) {
      fprintf(stderr, "ERROR LINE-%d mq_open: %s\n", __LINE__, strerror(errno));
      status = FAILURE;
      break;
    }

    mq_send(register_mq, user->name, strlen(user->name), 0);

    int bytes;
    do {
      bytes = mq_receive(user->user_mq, &status, BUF_MAX, 0);
      if (bytes < 0) {
        if (errno == EAGAIN) continue;
        status = FAILURE;
        fprintf(stderr, "ERROR LINE-%d mq_receive: %s\n", __LINE__,
                strerror(errno));
        break;
      }
    } while (bytes < 0);

    if (status == SUCCESS) break;
  }

  delwin(register_win);
  clear();
  refresh();
  if (mq_close(register_mq) < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_close: %s\n", __LINE__, strerror(errno));
  }

  return status;
}

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
    usleep(1000);
  }

  pthread_mutex_lock(&g_mutex_ncurses);
  delwin(user_win);
  pthread_mutex_unlock(&g_mutex_ncurses);

  return NULL;
}

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
  User *user = &user_list->users[user_list->len];
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
        usleep(100);
        continue;
      }
      fprintf(stderr, "ERROR LINE-%d mq_receive: %s\n", __LINE__,
              strerror(errno));
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
    int end = MIN(list_len, maxy + start);
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

void *MessageSender(void *argv) {
  MessagerController *controller = (MessagerController *)argv;

  mqd_t chat_mq = mq_open(CHAT_MQ, O_WRONLY);
  if (chat_mq < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_open: %s\n", __LINE__, strerror(errno));
    g_stop = 1;
    return NULL;
  }

  pthread_mutex_lock(&g_mutex_ncurses);
  WINDOW *input_win = newwin(3, COLS, LINES - 3, 0);
  box(input_win, 0, 0);
  wrefresh(input_win);
  pthread_mutex_unlock(&g_mutex_ncurses);

  char message_buf[MESSAGE_LEN_MAX] = {0};
  Message text = {0};
  strncpy(text.user, controller->user->name, USERNAME_MAX - 1);
  while (!g_stop) {
    pthread_mutex_lock(&g_mutex_ncurses);
    curs_set(TRUE);
    wmove(input_win, 1, 1);
    pthread_mutex_unlock(&g_mutex_ncurses);

    wgetnstr(input_win, message_buf, MESSAGE_LEN_MAX - 1);

    pthread_mutex_lock(&g_mutex_ncurses);
    curs_set(FALSE);
    wclear(input_win);
    box(input_win, 0, 0);
    wrefresh(input_win);
    pthread_mutex_unlock(&g_mutex_ncurses);

    if (strncmp(message_buf, "/exit", MESSAGE_LEN_MAX) == 0) {
      strncpy(text.text, "/exit", 6);
      mq_send(chat_mq, (char *)&text, sizeof(Message), 0);
      g_stop = 1;
      break;
    } else {
      strncpy(text.text, message_buf, MESSAGE_LEN_MAX);
      mq_send(chat_mq, (char *)&text, sizeof(Message), 0);
    }
  }
  if (mq_close(chat_mq) < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_close: %s\n", __LINE__, strerror(errno));
  }
  if (mq_unlink(controller->user->name) < 0) {
    fprintf(stderr, "ERROR LINE-%d mq_unlink: %s\n", __LINE__, strerror(errno));
  }
  pthread_mutex_lock(&g_mutex_ncurses);
  delwin(input_win);
  pthread_mutex_unlock(&g_mutex_ncurses);

  return NULL;
}

int main() {
  User user = {0};
  UserList user_list = {0};
  MessageList message_list = {0};
  MessagerController controller = {&user, &user_list, &message_list};

  pthread_mutex_init(&user_list.mutex, NULL);
  pthread_mutex_init(&message_list.mutex, NULL);

  pthread_t message_sender;
  pthread_t message_receiver;
  pthread_t user_receiver;

  initscr();
  cbreak();

  int status = Register(&user);
  if (status != FAILURE) {
    pthread_create(&message_sender, NULL, MessageSender, &controller);
    pthread_create(&message_receiver, NULL, MessageReceiver, &controller);
    pthread_create(&user_receiver, NULL, UserReceiver, &controller);

    pthread_join(message_sender, NULL);
    pthread_join(message_receiver, NULL);
    pthread_join(user_receiver, NULL);
  }
  pthread_mutex_destroy(&user_list.mutex);
  pthread_mutex_destroy(&message_list.mutex);
  endwin();
  exit(EXIT_SUCCESS);
}