#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_SERVER "server"
#define SOCK_CLIENT "client"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int main() {
  struct sockaddr_un serv, client;
  int cfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if (cfd == -1) handle_error("socket");

  if (0 == access(SOCK_CLIENT, F_OK)) {
    if (unlink(SOCK_CLIENT)) {
      handle_error("unlink");
    }
  }

  memset(&serv, 0, sizeof(serv));
  serv.sun_family = AF_LOCAL;
  strncpy(serv.sun_path, SOCK_SERVER, sizeof(serv.sun_path) - 1);

  memset(&client, 0, sizeof(client));
  client.sun_family = AF_LOCAL;
  strncpy(client.sun_path, SOCK_CLIENT, sizeof(client.sun_path));

  if (bind(cfd, (struct sockaddr *)&client, sizeof(client)) == -1) {
    handle_error("bind");
  }

  int status = connect(cfd, (struct sockaddr *)&serv, sizeof(serv));
  if (status == -1) handle_error("connect");
  char buf[50] = {0};
  strncpy(buf, "Hi!", sizeof(buf));

  send(cfd, buf, sizeof(buf), 0);
  recv(cfd, buf, sizeof(buf), 0);
  printf("%s\n", buf);
  send(cfd, buf, sizeof(buf), 0);

  close(cfd);
  unlink(SOCK_CLIENT);
  exit(EXIT_SUCCESS);
}