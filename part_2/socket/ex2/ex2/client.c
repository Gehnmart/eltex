#include "general.h"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int main() {
  int port = 0;
  struct sockaddr_in serv, new_serv, client;
  int cfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (cfd == -1) handle_error("socket");

  int coking_addr = 0;
  inet_pton(AF_INET, SERV_ADDR, &coking_addr);
  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(SERV_PORT);
  serv.sin_addr.s_addr = coking_addr;
  char buf[BUF_MAX] = {0};
  sendto(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&serv, sizeof(serv));
  int serv_size = sizeof(serv);
  recvfrom(cfd, &port, sizeof(port), 0, (struct sockaddr *)&serv, &serv_size);

  memset(&new_serv, 0, sizeof(new_serv));
  new_serv.sin_family = AF_INET;
  new_serv.sin_port = htons(port);
  new_serv.sin_addr.s_addr = coking_addr;
  if( -1 == connect(cfd, (struct sockaddr *) &new_serv, sizeof(new_serv)))
    handle_error("connect error:");

  while (1) {
    scanf("%50s", buf);
    if (strncmp(buf, "exit", sizeof(buf)) == 0) {
      send(cfd, buf, sizeof(buf), 0);
      break;
    }
    if (strncmp(buf, "time", sizeof(buf)) == 0) {
      send(cfd, buf, sizeof(buf), 0);
      recv(cfd, buf, sizeof(buf), 0);
      printf("now time = %s\n", buf);
    }
  }

  close(cfd);
  exit(EXIT_SUCCESS);
}