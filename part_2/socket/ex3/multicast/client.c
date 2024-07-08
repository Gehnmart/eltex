#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERV "224.0.0.1"
#define PORT 2048
#define BUF_MAX 50

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int main() {
  int sfd, err;
  char buf[BUF_MAX];
  struct sockaddr_in endpoint;
  socklen_t endpnt_len = sizeof(endpoint);

  sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd == -1) {
    handle_error("socket():");
  }

  endpoint.sin_family = AF_INET;
  endpoint.sin_port = htons(PORT);
  endpoint.sin_addr.s_addr = INADDR_ANY;

  if (bind(sfd, (struct sockaddr *)&endpoint, sizeof(endpoint)) == -1) {
    handle_error("bind():");
  }

  struct ip_mreqn mreq;
  inet_pton(AF_INET, SERV, &mreq.imr_multiaddr.s_addr);
  mreq.imr_address.s_addr = htonl(INADDR_ANY);
  mreq.imr_ifindex = 0;

  err = setsockopt(sfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
  if(err == -1) {
    handle_error("setsockopt(): IP_ADD_MEMBERSHIP");
  }
  recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr *)&endpoint, &endpnt_len);
  printf("time - %s", buf);

  err = setsockopt(sfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
  if(err == -1) {
    handle_error("setsockopt(): IP_DROP_MEMBERSHIP");
  }

  close(sfd);
  exit(EXIT_SUCCESS);
}