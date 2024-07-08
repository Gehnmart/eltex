#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

#define PORT 2048
#define PORT_CL 2049
#define ADDR "127.0.0.1"

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

typedef union {
  char  data[64];
  struct {
    short src_port;
    short dst_port;
    short len;
    short checksum;
    char  payload[56];
  } part;
} UdpHeader;

int main() {
  struct sockaddr_in serv;
  socklen_t sock_size = sizeof(serv);
  char buf[50] = {0};
  int sfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sfd == 1) {
    handle_error("socket():");
  }

  UdpHeader header;
  memset(&header, 0, sizeof(header));
  header.part.src_port = htons(PORT_CL);
  header.part.dst_port = htons(PORT);
  header.part.len =      htons(sizeof(header));
  strncpy(header.part.payload, "Hello world!", sizeof(header.part.payload));

  serv.sin_family = AF_INET;
  serv.sin_addr.s_addr = inet_addr(ADDR);
  serv.sin_port = htons(PORT);

  sendto(sfd, &header, sizeof(header), 0, (struct sockaddr *) &serv, sock_size);
  while(1) {
    recvfrom(sfd, &header, sizeof(header), 0, (struct sockaddr *) &serv, &sock_size);
    if(header.part.src_port == htons(PORT)){
      printf("success\n");
    }
    usleep(100);
  }

  close(sfd);
  exit(EXIT_SUCCESS);
}