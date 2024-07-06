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
#define PORT 2048
#define ADDR "127.0.0.1"

typedef struct {
  struct sockaddr_in *clients;
  pthread_t *threads;
  int len;
  int sfd;
} ServerCtl;

#endif  // EX1_GENERAL_H