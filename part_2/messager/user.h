#ifndef MESSAGER_USER_H
#define MESSAGER_USER_H

#include "general_resource.h"

#define STATUS_OK 1
#define STATUS_NO 0

typedef struct {
    unsigned id;
    mqd_t user_mq;
    char name[USERNAME_MAX];
} User;

typedef struct {
    char name[USERNAME_MAX];
} UserCreateReq;

typedef struct {
    unsigned id;
    char name[USERNAME_MAX];
    char status;
} UserCreateRes;

typedef struct {
    User users[USER_MAX];
} UserList;

#endif  // MESSAGER_USER_H