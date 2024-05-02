#ifndef MESSAGER_USER_H
#define MESSAGER_USER_H

#include "general_resource.h"

typedef struct {
    unsigned id;
    char name[NAME_MAX];
} User;

typedef struct {
    char name[NAME_MAX];
} UserReq;

typedef struct {
    unsigned id;
    char name[NAME_MAX];
} UserRes;

#endif  // MESSAGER_USER_H