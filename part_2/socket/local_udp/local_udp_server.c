#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "my_socket"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int main() {
  struct sockaddr_un serv, client;
  char buf[1024] = {0};

  int sfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if (sfd == -1)
    handle_error("socket");

  memset(&serv, 0, sizeof(serv));
  serv.sun_family = AF_LOCAL;
  strncpy(serv.sun_path, SOCK_PATH, sizeof(serv.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *)&serv, sizeof(serv)) == -1)
    handle_error("bind");

  memset(&client, 0, sizeof(client));
  socklen_t client_addr_size = sizeof(client);
  recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *) &client, &client_addr_size);
  printf("%s\n", buf);
  strncpy(buf, "Hello!", sizeof(buf));
  sendto(sfd, buf, sizeof(buf), 0, (struct sockaddr *) &client, client_addr_size);
  recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *) &client, &client_addr_size);

  close(sfd);
  unlink(SOCK_PATH);
}