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
  char buf[1024] = {0};

  struct sockaddr_un serv;

  int sfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if (sfd == -1)
    handle_error("socket");

  memset(&serv, 0, sizeof(serv));
  serv.sun_family = AF_LOCAL;
  strncpy(serv.sun_path, SOCK_PATH, sizeof(serv.sun_path) - 1);

  int status = connect(sfd, (struct sockaddr *)&serv, sizeof(serv));
  if (status == -1)
    handle_error("connect");
  
  strncpy(buf, "Hi!", sizeof(buf));

  send(sfd, buf, sizeof(buf), 0);
  recv(sfd, buf, sizeof(buf), 0);
  printf("%s\n", buf);
  send(sfd, buf, sizeof(buf), 0);

  close(sfd);
}