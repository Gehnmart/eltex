#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_SERVER "server"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int main() {
  int sfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if (sfd == -1) handle_error("socket");

  if (0 == access(SOCK_SERVER, F_OK)) {
    if (unlink(SOCK_SERVER)) {
      handle_error("unlink");
    }
  }

  struct sockaddr_un serv, client;
  memset(&serv, 0, sizeof(serv));
  memset(&client, 0, sizeof(client));
  serv.sun_family = AF_LOCAL;
  strncpy(serv.sun_path, SOCK_SERVER, sizeof(serv.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *)&serv, sizeof(serv)) == -1)
    handle_error("bind");

  char buf[50] = {0};
  socklen_t cl_size = sizeof(client);
  recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &cl_size);
  printf("%s\n", buf);
  strncpy(buf, "Hello!", sizeof(buf));
  sendto(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&client, cl_size);
  recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &cl_size);

  close(sfd);
  unlink(SOCK_SERVER);
  exit(EXIT_SUCCESS);
}