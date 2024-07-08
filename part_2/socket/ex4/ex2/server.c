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
  int sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1) handle_error("socket");

  int coking_addr = 0;
  inet_pton(AF_INET, ADDR, &coking_addr);
  struct sockaddr_in serv, client;
  memset(&serv, 0, sizeof(serv));
  memset(&client, 0, sizeof(client));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(PORT);
  serv.sin_addr.s_addr = coking_addr;

  if (bind(sfd, (struct sockaddr *)&serv, sizeof(serv)) == -1)
    handle_error("bind");

  char buf[1024] = {0};
  socklen_t cl_size = sizeof(client);
  recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &cl_size);
  printf("packet %s\n", buf);
  strncpy(buf, "Hello!", sizeof(buf));
  sendto(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&client, cl_size);
  recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &cl_size);

  close(sfd);
  exit(EXIT_SUCCESS);
}