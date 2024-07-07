#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define ADDR "127.0.0.1"
#define DEFAULT_PORT_TCP 2048
#define DEFAULT_PORT_UDP 2049

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
  char buf[50] = {0};
  struct sockaddr_in serv;
  socklen_t serv_size = sizeof(serv);

  ServWriter(&serv, ADDR, DEFAULT_PORT_TCP);

  int cfd = socket(AF_INET, SOCK_STREAM, 0);
  if (cfd == -1) handle_error("socket");
  if (connect(cfd, (struct sockaddr *) &serv, serv_size) == -1){
    handle_error("connect error:");
  }

  send(cfd, buf, sizeof(buf), 0);
  recv(cfd, buf, sizeof(buf), 0);

  printf("%s\n", buf);
  close(cfd);
  exit(EXIT_SUCCESS);
}