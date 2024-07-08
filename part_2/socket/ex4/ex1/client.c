#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  struct sockaddr_in client;
  socklen_t cl_size = sizeof(client);
  char buf[50] = {0};
  int sfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sfd == 1) {
    perror("socket():");
    exit(EXIT_FAILURE);
  }


  while(1) {
    recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *) &client, &cl_size);
    printf("%s\n", buf + 28);
  }

  close(sfd);
  exit(EXIT_SUCCESS);
}