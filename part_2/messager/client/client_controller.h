#ifndef CLIENT_CLIENT_CONTROLLER_H
#define CLIENT_CLIENT_CONTROLLER_H

#include "../additional.h"
#include "helpers/helpers.h"

typedef struct {
  char client_stop;
  User *user;
  UserList *user_list;
  MessageList *message_list;
  pthread_mutex_t ncurses_mutex;
} ClientController;

int ControllerDestroy(ClientController *controller);
int ControllerInit(ClientController *controller);

#endif  // CLIENT_CLIENT_CONTROLLER_H