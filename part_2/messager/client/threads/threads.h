#ifndef CLIENT_THREADS_H
#define CLIENT_THREADS_H

#include <ncurses.h>

#include "../../additional.h"
#include "../helpers/helpers.h"

typedef struct {
  User *user;
  UserList *user_list;
  MessageList *message_list;
  pthread_mutex_t ncurses_lock;
} MessagerController;

void *UserReceiver(void *argv);
void *MessageReceiver(void *argv);
void *MessageSender(void *argv);

#endif  // CLIENT_THREADS_H