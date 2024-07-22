#ifndef EX1_GENERAL_H
#define EX1_GENERAL_H

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_MAX 50
#define PORT_SRV 2048
#define ADDR_SRV "127.0.0.1"

typedef struct {
  pthread_t thread;
  struct sockaddr_in client;
} Client;

typedef struct {
  Client    *cl_list;
  int       len;
  int       sfd;
  int       stop;
} ServerCtl;

#endif  // EX1_GENERAL_H