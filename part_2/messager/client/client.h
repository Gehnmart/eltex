#ifndef SERVER_client_H
#define SERVER_client_H

#include "../general_resource.h"

typedef struct {
  User *user;
  UserList *user_list;
  MessageList *message_list;
} MessagerController;

#endif  // SERVER_SERVER_H