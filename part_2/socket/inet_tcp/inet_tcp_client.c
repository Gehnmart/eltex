#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 2048

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int main() {
  char message[] = "Hi!\n";
  char buf[1024] = {0};

  struct sockaddr_in serv;

  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1)
    handle_error("socket");

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(SERVER_PORT);
  serv.sin_addr.s_addr = inet_addr("127.0.0.1");

  int status = connect(sfd, (struct sockaddr *)&serv, sizeof(serv));
  if (status == -1)
    handle_error("socket");
  
  send(sfd, message, sizeof(message), 0);
  recv(sfd, buf, sizeof(buf) - 1, 0);
  printf("%s\n", buf);

  close(sfd);
}