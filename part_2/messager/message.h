#ifndef MESSAGER_MESSAGE_H
#define MESSAGER_MESSAGE_H

#include "general_resource.h"

typedef struct {
    unsigned id;
    char user[USERNAME_MAX];
    char message[MESSAGE_LEN_MAX];
} Message;

typedef struct {
    int len;
    Message messages[MESSAGE_MAX];
} MessageList;

#endif  // MESSAGER_MESSAGE_H
