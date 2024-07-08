#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
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

int main() {
  int sfd;
  char buf[BUF_MAX];
  struct sockaddr_in endpoint;
  socklen_t endpoint_len = sizeof(endpoint);

  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1) {
    handle_error("socket error:");
  }

  int inet_addr;
  inet_pton(AF_INET, SERV, &inet_addr);
  endpoint.sin_family = AF_INET;
  endpoint.sin_port = htons(PORT);
  endpoint.sin_addr.s_addr = inet_addr;

  if (bind(sfd, (struct sockaddr *)&endpoint, endpoint_len) == -1) {
    handle_error("bind error:");
  }

  recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&endpoint,
           &endpoint_len);
  printf("time - %s\n", buf);

  exit(EXIT_SUCCESS);
}