#include "general.h"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

void ServWriter(struct sockaddr_in *serv, const char *addr, int port) {
  int coking_addr = 0;
  inet_pton(AF_INET, addr, &coking_addr);
  memset(serv, 0, sizeof(*serv));

  serv->sin_family = AF_INET;
  serv->sin_port = htons(port);
  serv->sin_addr.s_addr = coking_addr;
}

int main() {
  char buf[BUF_MAX] = {0};
  struct sockaddr_in serv;
  socklen_t serv_size = sizeof(serv);

  ServWriter(&serv, SERV_ADDR, SERV_PORT);

  int cfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (cfd == -1) handle_error("socket");

  sendto(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&serv, sizeof(serv));
  recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&serv, &serv_size);

  printf("%s\n", buf);
  close(cfd);
  exit(EXIT_SUCCESS);
}