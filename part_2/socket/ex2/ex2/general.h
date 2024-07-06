#ifndef EX2_GENERAL_H
#define EX2_GENERAL_H

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define BUF_MAX    50
#define SERV_PORT  2048
#define SERV_ADDR  "127.0.0.1"
#define SERV_COUNT 10

typedef struct {
  struct sockaddr_in client;
  pthread_t          thread;
  int                sfd;
  int                port;
  char               is_bussy;
}Server;

typedef struct {
  Server server_list[SERV_COUNT];
  int    port;
}ServerCtl;

#endif  // EX2_GENERAL_H