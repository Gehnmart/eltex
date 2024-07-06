#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 2048
#define ADDR "127.0.0.1"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int main() {
  struct sockaddr_in serv, client;
  int cfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (cfd == -1) handle_error("socket");

  int coking_addr = 0;
  inet_pton(AF_INET, ADDR, &coking_addr);
  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(PORT);
  serv.sin_addr.s_addr = coking_addr;

  int status = connect(cfd, (struct sockaddr *)&serv, sizeof(serv));
  if (status == -1) handle_error("connect");
  char buf[50] = {0};
  strncpy(buf, "Hi!", sizeof(buf));

  send(cfd, buf, sizeof(buf), 0);
  recv(cfd, buf, sizeof(buf), 0);
  printf("%s\n", buf);
  send(cfd, buf, sizeof(buf), 0);

  close(cfd);
  exit(EXIT_SUCCESS);
}