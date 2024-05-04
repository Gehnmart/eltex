#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "../general_resource.h"

typedef struct {
  UserList *user_list;
  MessageList *message_list;
} MessagerController;

#endif  // SERVER_SERVER_H