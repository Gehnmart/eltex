#ifndef MESSAGER_ADDITIONAL_H
#define MESSAGER_ADDITIONAL_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>

#define SEM_SERVER_NAME   "/sem_server" /*semaphore server posix name*/
#define SEM_CLIENT_NAME   "/sem_client" /*semaphore client posix name*/
#define SHARED_DATA_NAME  "/shared_data" /*shared data posix name*/

#define STAT_EXIT         1
#define STAT_JOIN         2
#define STAT_EXEC         3
#define FAILURE           0
#define SUCCESS           1
#define USER_MAX          16 /*max connected users (defines size user array)*/
#define USERNAME_MAX      32 /*max length username string*/
#define MESSAGE_MAX       32 /*max lingth message string*/
#define MESSAGE_LEN_MAX   64 /*max messages maybe old messages will be overwriten*/

/*print error in stderr format("line, msg, error") */
#define errExit(msg)                                               \
  do {                                                             \
    fprintf(stderr, "line-%d %s %s\n", __LINE__, msg, strerror(errno)); \
    exit(EXIT_FAILURE);                                            \
  } while (0)

/*user struct maybe neded add new feature*/
typedef struct {
  int  status;
  char username[USERNAME_MAX];
} User;

/*static list users struct size defined USER_MAX*/
typedef struct {
  int  len; /*current len user list or last user*/
  User list[USER_MAX];
} UserList;

typedef struct {
  char username[USERNAME_MAX]; /*sender name*/
  char text[MESSAGE_LEN_MAX]; /*message text*/
} Message;

typedef struct {
  int     len; /*current len message list or last message*/
  Message list[MESSAGE_MAX];
} MessageList;

/*data which will be located in shared memory*/
typedef struct {
  MessageList msg_list; /*list messages*/
  UserList    usr_list; /*list users*/
//  UserList    registration_req; /*pull registration query*/
} SharedData;

/*struct for server and client controller*/
typedef struct {
  int        th_stop; /*stoped all threads and end programm*/
  int        shmfd; /*shared memory id*/
  char       selfname[USERNAME_MAX]; /*selfname server default set 'server', client can choose any name*/
  sem_t      *sem_server; /*semaphore server*/
  sem_t      *sem_client; /*semaphore client*/
  SharedData *shared_data; /*shared data locatioin in shared memory and contain MessageList and UserList*/
} MessagerCtl;

#endif  // MESSAGER_ADDITIONAL_H