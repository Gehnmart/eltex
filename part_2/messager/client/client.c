#include "client.h"

#include <malloc.h>
#include <ncurses.h>

#include "../general_resource.h"

char g_exit = 1;

void InitNcurses() {
  initscr();
  cbreak();
  refresh();
}

int Register(User *user) {
  char status = SUCCESS;
  struct mq_attr attr = {0, 10, BUF_MAX, 0};

  mqd_t register_mq = mq_open(REGISTER_MQ, O_WRONLY);
  if (register_mq < 0) {
    perror("Register() mq_open:");
    return FAILURE;
  }

  WINDOW *win =
      newwin(3, USERNAME_MAX + 2, LINES / 2 - 3, COLS / 2 - USERNAME_MAX / 2);
  while (g_exit) {
    box(win, 0, 0);
    wrefresh(win);
    curs_set(TRUE);
    wmove(win, 0, 1);
    wprintw(win, "Enter the username:");
    wmove(win, 1, 1);
    wgetnstr(win, user->name, USERNAME_MAX - 1);
    curs_set(FALSE);
    wrefresh(win);

    if (strncmp(user->name, "/exit", strlen("/exit")) == 0) {
      status = FAILURE;
      break;
    }
    if (strchr(user->name, '/') != NULL) {
      wmove(win, 2, 1);
      wprintw(win, "Name doesnt include / sym!");
      wrefresh(win);
      getchar();
      wclear(win);
      wrefresh(win);
      continue;
    }

    char temp[BUF_MAX] = {0};
    temp[0] = '/';
    strcpy(temp + 1, user->name);
    strcpy(user->name, temp);
    printf("%s\n", user->name);
    user->user_mq = mq_open(user->name, O_CREAT | O_RDWR, 0666, &attr);
    if (user->user_mq < 0) {
      perror("Register() mq_open2:");
      status = FAILURE;
      break;
    }

    mq_send(register_mq, user->name, strlen(user->name), 0);

    int slen = mq_receive(user->user_mq, &status, BUF_MAX, 0);
    if (slen < 0) {
      status = FAILURE;
    }

    if (status != SUCCESS) {
      mq_close(user->user_mq);
      mq_unlink(user->name);
      status = FAILURE;
      wmove(win, 2, 1);
      wprintw(win, "Error username is exist!");
      wrefresh(win);
      getchar();
      wclear(win);
      wrefresh(win);
    } else {
      break;
    }
  }

  delwin(win);
  mq_close(register_mq);

  return status;
}

void *ChatWindow(void *argv) {
  MessagerController *controller = (MessagerController *)argv;

  mqd_t chat_mq = mq_open(CHAT_MQ, O_WRONLY);
  if (chat_mq < 0) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  WINDOW *chat_win = newwin(LINES - 3, COLS - USERNAME_MAX, 0, 0);
  WINDOW *user_win = newwin(LINES - 3, USERNAME_MAX, 0, COLS - USERNAME_MAX);
  WINDOW *input_win = newwin(3, COLS, LINES - 3, 0);
  while (g_exit) {
    box(chat_win, 0, 0);
    box(user_win, 0, 0);
    box(input_win, 0, 0);
    wrefresh(chat_win);
    wrefresh(user_win);
    wrefresh(input_win);

    pthread_mutex_lock(&controller->message_list->mutex);
    for (int i = 0; i < controller->message_list->len; i++) {
      wmove(chat_win, i + 1, 1);
      wprintw(chat_win, "%s: %s", controller->message_list->messages[i].user,
              controller->message_list->messages[i].message);
    }
    pthread_mutex_unlock(&controller->message_list->mutex);

    char msg[MESSAGE_LEN_MAX] = {0};
    curs_set(TRUE);
    wmove(input_win, 0, 1);
    wprintw(input_win, "Input field:");
    wmove(input_win, 1, 1);
    wgetnstr(input_win, msg, USERNAME_MAX - 1);
    curs_set(FALSE);
    wclear(input_win);
    wrefresh(input_win);

    if (strncmp(msg, "/exit", MESSAGE_LEN_MAX) == 0) {
      break;
    }

    Message message;
    strncpy(message.message, msg, MESSAGE_LEN_MAX);
    strncpy(message.user, controller->user->name, USERNAME_MAX);
    mq_send(chat_mq, (char *)&message, BUF_MAX, 0);
  }

  close(chat_mq);

  delwin(chat_win);
  delwin(user_win);
  delwin(input_win);
}

void *MessageReceiver(void *argv) {
  MessagerController *controller = (MessagerController *)argv;
  while (g_exit) {
    Message message;
    mq_receive(controller->user->user_mq, (char *)&message, BUF_MAX, 0);
    pthread_mutex_lock(&controller->message_list->mutex);
    memcpy((void *)&controller->message_list->messages[controller->message_list->len], (void *)&message, sizeof(Message));
    controller->message_list->len++;
    pthread_mutex_unlock(&controller->message_list->mutex);
  }
  exit(EXIT_SUCCESS);
}

int main() {
  User user = {0};
  UserList user_list = {0};
  MessageList message_list = {0};
  MessagerController controller = {&user, &user_list, &message_list};

  pthread_mutex_init(&user_list.mutex, NULL);
  pthread_mutex_init(&message_list.mutex, NULL);

  pthread_t pt_ui;
  // pthread_t pt_send;
  pthread_t pt_receive;

  InitNcurses();

  Register(&user);

  pthread_create(&pt_ui, NULL, ChatWindow, &controller);
  // pthread_create(pt_send, NULL);
  pthread_create(&pt_receive, NULL, MessageReceiver, &controller);

  pthread_join(pt_ui, NULL);
  // pthread_join(pt_send, NULL);
  pthread_join(pt_receive, NULL);

  getchar();
  endwin();
  exit(EXIT_SUCCESS);
}