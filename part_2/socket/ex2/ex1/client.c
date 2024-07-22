#include "general.h"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int main() {
  char buf[50] = {0};
  int sfd, port = 0;
  socklen_t addr_sz = 0;
  struct sockaddr_in serv, client;

  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1) {
    handle_error("socket():");
  }

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(PORT_SRV);
  inet_pton(AF_INET, ADDR_SRV, &serv.sin_addr.s_addr);

  sendto(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&serv, sizeof(serv));
  memset(&serv, 0, sizeof(serv));
  addr_sz = sizeof(serv);
  recvfrom(sfd, &port, sizeof(port), 0, (struct sockaddr *)&serv, &addr_sz);

  if (connect(sfd, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
    handle_error("connect():");
  }

  strncpy(buf, "time", sizeof(buf));
  send(sfd, buf, sizeof(buf), 0);
  recv(sfd, buf, sizeof(buf), 0);
  printf("now time = %s\n", buf);

  strncpy(buf, "exit", sizeof(buf));
  send(sfd, buf, sizeof(buf), 0);

  if (close(sfd) == -1) {
    handle_error("close():");
  }

  exit(EXIT_SUCCESS);
}