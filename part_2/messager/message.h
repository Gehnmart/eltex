#ifndef MESSAGER_MESSAGE_H
#define MESSAGER_MESSAGE_H

#include "general_resource.h"

typedef struct {
    unsigned id;
    char user[NAME_MAX];
    char message[MESSAGE_MAX];
} Message;

typedef struct {
    User user
} MessageReq;

#endif  // MESSAGER_MESSAGE_H
