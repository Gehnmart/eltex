#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define SERV "255.255.255.255"
#define PORT 2048
#define BUF_MAX 50

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int stop = 0;

void GetTime(char *buf, int n) {
  time_t rawtime;
  struct tm *timeinfo;
  memset(buf, 0, n);

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  asctime_r(timeinfo, buf);
}

int main() {
  int sfd, flag = 1;
  char buf[BUF_MAX];
  struct sockaddr_in endpoint;

  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1) {
    handle_error("socket error:");
  }

  if (setsockopt(sfd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)) == -1) {
    handle_error("setsockopt error:");
  }

  int inet_addr;
  inet_pton(AF_INET, SERV, &inet_addr);
  endpoint.sin_family = AF_INET;
  endpoint.sin_port = htons(PORT);
  endpoint.sin_addr.s_addr = inet_addr;

  while (!stop) {
    GetTime(buf, sizeof(buf));
    sendto(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&endpoint,
           sizeof(endpoint));
    sleep(1);
  }

  exit(EXIT_SUCCESS);
}