#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 2048
#define PORT_CL 2049
#define ADDR "127.0.0.1"

#define BF_LEN 64
#define DATA_LEN (BF_LEN - 8)

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int main() {
  struct sockaddr_in serv;
  socklen_t sock_size = sizeof(serv);
  char send_buf[BF_LEN] = {0};
  char recv_buf[BF_LEN] = {0};
  int sfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sfd == -1) {
    handle_error("socket():");
  }

  struct udphdr udph;
  memset(&udph, 0, sizeof(udph));
  udph.source = htons(PORT_CL);
  udph.dest = htons(PORT);
  udph.len = htons(sizeof(send_buf));
  memcpy(send_buf, &udph, sizeof(udph));
  strncpy(send_buf + sizeof(udph), "HelloWorld!",
          sizeof(send_buf) - sizeof(udph));

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(PORT);
  inet_pton(AF_INET, ADDR, &serv.sin_addr.s_addr);

  while (1) {
    printf("type your message or type exit:\n");
    scanf("%55s", send_buf + sizeof(udph));
    if (strncmp(send_buf + sizeof(udph), "exit", 5) == 0) {
      break;
    }
    sendto(sfd, &send_buf, sizeof(send_buf), 0, (struct sockaddr *)&serv,
           sock_size);
    while (1) {
      recvfrom(sfd, &recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&serv,
               &sock_size);
      memcpy(&udph, recv_buf + sizeof(struct iphdr), sizeof(udph));
      if (udph.source == htons(PORT)) {
        printf("%s\n", recv_buf + (sizeof(udph) + sizeof(struct iphdr)));
        break;
      }
    }
  }

  close(sfd);
  exit(EXIT_SUCCESS);
}