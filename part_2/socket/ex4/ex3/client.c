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

#define BF_LEN 129
#define BF_LEN_S "100"

#define IP_OFFSET 20
#define UDP_OFFSET 8
#define OFFSET (IP_OFFSET + UDP_OFFSET)
#define MSG_LEN (BF_LEN - OFFSET)

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

int main() {
  int cfd, err, addr, sock_flag = 1;
  struct sockaddr_in serv;
  socklen_t sock_size = sizeof(serv);
  char send_buf[BF_LEN] = {0};
  char recv_buf[BF_LEN] = {0};

  inet_pton(AF_INET, ADDR, &addr);

  cfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (cfd == -1) {
    handle_error("socket():");
  }

  err = setsockopt(cfd, IPPROTO_IP, IP_HDRINCL, &sock_flag, sizeof(sock_flag));
  if (err == -1) {
    handle_error("setsockopt(): IP_HDRINCL");
  }

  struct iphdr *iph = (struct iphdr *)(send_buf);
  iph->version  = IPVERSION;
  iph->ihl      = 5;
  iph->tot_len  = sizeof(send_buf);
  iph->ttl      = MAXTTL;
  iph->protocol = IPPROTO_UDP;
  iph->saddr    = addr;
  iph->daddr    = addr;

  struct udphdr *udph = (struct udphdr *)(send_buf + IP_OFFSET);
  udph->source  = htons(PORT_CL);
  udph->dest    = htons(PORT);
  udph->len     = htons(sizeof(send_buf) - IP_OFFSET);

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(PORT);
  inet_pton(AF_INET, ADDR, &serv.sin_addr.s_addr);

  while (1) {
    printf("type your message or type exit:\n  ->");
    scanf("%" BF_LEN_S "s", send_buf + OFFSET);
    if (strncmp(send_buf + OFFSET, "exit", 5) == 0) {
      break;
    }
    sendto(cfd, &send_buf, sizeof(send_buf), 0, (struct sockaddr *)&serv,
           sock_size);
    while (1) {
      recvfrom(cfd, &recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&serv,
               &sock_size);
      struct udphdr *udph_recv = (struct udphdr *)(recv_buf + IP_OFFSET);
      if (udph_recv->source == htons(PORT)) {
        printf("returned message:\n  ->%s\n", recv_buf + OFFSET);
        break;
      }
    }
  }

  close(cfd);
  exit(EXIT_SUCCESS);
}