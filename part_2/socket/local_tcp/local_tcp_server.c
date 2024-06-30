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
  char message[] = "Hello!\n";
  char buf[1024] = {0};

  int sfd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (sfd == -1)
    handle_error("socket");

  memset(&serv, 0, sizeof(serv));
  serv.sun_family = AF_LOCAL;
  strncpy(serv.sun_path, SOCK_PATH, sizeof(serv.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *)&serv, sizeof(serv)) == -1)
    handle_error("bind");

  if (listen(sfd, 5) == -1)
    handle_error("listen");

  socklen_t client_addr_size = sizeof(client);
  int cfg = accept(sfd, (struct sockaddr *)&client, &client_addr_size);
  if (cfg == -1)
    handle_error("accept");

  recv(cfg, buf, sizeof(buf) - 1, 0);
  printf("%s", buf);
  send(cfg, message, sizeof(message), 0);

  close(cfg);
  close(sfd);
  unlink(SOCK_PATH);
}