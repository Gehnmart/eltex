#ifndef MESSAGER_GENERAL_RESOURCE_H
#define MESSAGER_GENERAL_RESOURCE_H

#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define REGISTER_MQ "/register"
#define CHAT_MQ "/chat"

#define USER_MAX 32
#define USERNAME_MAX 32
#define MESSAGE_MAX 64
#define MESSAGE_LEN_MAX 256
#define QUEUE_NAME_LEN_MAX 64

#define FAILURE 0
#define SUCCESS 1

#define NOTHING 0
#define DELETE_USER 1
#define APPEND_USER 2

#define BUF_MAX 1024
#define TIMEOUT 4

#define MIN(a, b) (a > b ? b : a)

typedef struct {
  unsigned last_receive_msg;
  unsigned id;
  mqd_t user_mq;
  char name[USERNAME_MAX];
} User;

typedef struct {
  pthread_mutex_t mutex;
  int len;
  User users[USER_MAX];
} UserList;

typedef union {
  char data_all [USERNAME_MAX + 1];
  struct {
    char type;
    char username[USERNAME_MAX];
  }data;
} Metadata;

typedef struct {
  int last_receive_message;
  char user[USERNAME_MAX];
  char text[MESSAGE_LEN_MAX];
  Metadata metadata;
} Message;

typedef struct {
  pthread_mutex_t mutex;
  int len;
  Message messages[MESSAGE_MAX];
} MessageList;

typedef struct {
  char name[USERNAME_MAX];
} UserCreateReq;

typedef struct {
  char status;
  char name[USERNAME_MAX];
} UserCreateRes;

#endif  // MESSAGER_GENERAL_RESOURCE_H