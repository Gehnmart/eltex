#ifndef SERVER_client_H
#define SERVER_client_H

#include <ncurses.h>

#include "../general_resource.h"

typedef struct {
  User *user;
  UserList *user_list;
  MessageList *message_list;
  pthread_mutex_t ncurses_lock;
} MessagerController;

#endif  // SERVER_SERVER_H