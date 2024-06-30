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
  struct sockaddr_in serv, client;
  char message[] = "Hello!\n";
  char buf[1024] = {0};

  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1)
    handle_error("socket");

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(SERVER_PORT);
  serv.sin_addr.s_addr = inet_addr("127.0.0.1");
  

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
}