#include <fcntl.h> /* For O_* constants */
#include <linux/limits.h>
#include <mqueue.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h> /* For mode constants */
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "../message.h"
#include "../user.h"

MessageList g_message_list;

void sig_winch(int signo) {
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
  resizeterm(size.ws_row, size.ws_col);
}

static void InitNcurses() {
  initscr();
  cbreak();
  refresh();
}

void Register(UserCreateReq user_request, UserCreateRes *user_response) {
  struct mq_attr attr = {0, 10, BUF_MAX, 0};

  mqd_t register_mq = mq_open("/register", O_WRONLY);
  if (register_mq < 0) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  char request_mq[QUEUE_NAME_LEN_MAX] = {0};

  mq_send(register_mq, (char *)&user_request, BUF_MAX, 0);

  strncpy(request_mq, "/", USERNAME_MAX);
  strncat(request_mq, user_request.name, USERNAME_MAX);

  mqd_t mqdes_client = mq_open(request_mq, O_CREAT | O_RDWR, 0666, &attr);
  if (mqdes_client < 0) {
    perror("mq_open");
  }

  struct timespec timout;
  clock_gettime(CLOCK_REALTIME, &timout);
  timout.tv_sec += TIMEOUT;
  if (-1 == (mq_timedreceive(mqdes_client, (char *)user_response, BUF_MAX, NULL,
                             &timout))) {
    perror("mq_receive");
  }
  mq_close(mqdes_client);
  mq_close(register_mq);
  mq_unlink(request_mq);

  return;
}

void LoginWindow(UserCreateRes *user_response) {
  WINDOW *win =
      newwin(3, USERNAME_MAX + 2, LINES / 2 - 3, COLS / 2 - USERNAME_MAX / 2);
  while (1) {
    char user_name[USERNAME_MAX + 1];
    box(win, 0, 0);
    wrefresh(win);
    curs_set(TRUE);
    wmove(win, 0, 1);
    wprintw(win, "Enter the username:");
    wmove(win, 1, 1);
    wgetnstr(win, user_name, USERNAME_MAX - 1);
    curs_set(FALSE);
    wrefresh(win);

    UserCreateReq user_request = {0};
    strncpy(user_request.name, user_name, USERNAME_MAX);
    Register(user_request, user_response);

    if (user_response->status != STATUS_OK) {
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
}

void *Updater(void *argv) {
  UserCreateRes *user_response = (UserCreateRes *) argv;
  mqd_t chat_mq = mq_open(user_response->name, O_RDONLY);
  if (chat_mq < 0) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }
  while(1) {
    Message message = {0};
    ssize_t msg_len = mq_receive(chat_mq, (char *)&message, BUF_MAX, NULL);
    if (msg_len == -1) {
      perror("mq_receive");
      continue;
    }

    strncpy(g_message_list.messages[g_message_list.len].message, message.message, MESSAGE_LEN_MAX);
    strncpy(g_message_list.messages[g_message_list.len].user, message.user, USERNAME_MAX);
    g_message_list.messages[g_message_list.len].id = g_message_list.len;
    
    g_message_list.len++;
  }
}

void ChatWindow(UserCreateRes *user_response) {
  mqd_t chat_mq = mq_open("/chat", O_WRONLY);
  if (chat_mq < 0) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  WINDOW *chat_win = newwin(LINES - 3, COLS - USERNAME_MAX, 0, 0);
  WINDOW *user_win = newwin(LINES - 3, USERNAME_MAX, 0, COLS - USERNAME_MAX);
  WINDOW *input_win = newwin(3, COLS, LINES - 3, 0);
  while (1) {
    box(chat_win, 0, 0);
    box(user_win, 0, 0);
    box(input_win, 0, 0);
    wrefresh(chat_win);
    wrefresh(user_win);
    wrefresh(input_win);

    for(int i = 0; i < g_message_list.len; i++){
      wmove(chat_win, 1, i + 1);
      wprintw(chat_win, "%s: %s",g_message_list.messages[i].user, g_message_list.messages[i].message);
    }

    char msg[MESSAGE_LEN_MAX] = {0};
    curs_set(TRUE);
    wmove(input_win, 0, 1);
    wprintw(input_win, "Input field:");
    wmove(input_win, 1, 1);
    wgetnstr(input_win, msg, USERNAME_MAX - 1);
    curs_set(FALSE);
    wrefresh(input_win);

    Message message;
    strncpy(message.message, msg, MESSAGE_LEN_MAX);
    strncpy(message.user , user_response->name, USERNAME_MAX);
    mq_send(chat_mq, (char *)&message, BUF_MAX, 0);

    break;
  }
  delwin(chat_win);
  delwin(user_win);
  delwin(input_win);
}

int main() {
  pthread_t updater;
  InitNcurses();

  UserCreateRes user_response;
  LoginWindow(&user_response);
  pthread_create(&updater, NULL, Updater, &user_response);
  ChatWindow(&user_response);
  pthread_join(updater, NULL);

  endwin();
  exit(EXIT_SUCCESS);
}