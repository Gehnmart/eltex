#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "../additional.h"

typedef struct {
  char server_stop;
  UserList *user_list;
  MessageList *message_list;
} ServerController;

int ControllerDestroy(ServerController *controller);
int ControllerInit(ServerController *controller);

#endif  // SERVER_SERVER_H