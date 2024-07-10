#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define ETH_INTERFACE "enp0s8"

#define SRC_MAC_ADDR "08:00:27:d1:e9:19"
#define DST_MAC_ADDR "08:00:27:78:85:25"
#define SRC_ADDR "192.168.56.3"
#define DST_ADDR "192.168.56.2"
#define SRC_PORT 2049
#define DST_PORT 2048

#define BF_LEN 143
#define BF_LEN_S "100"

#define IP_OFFSET 20
#define UDP_OFFSET 8
#define ETH_OFFSET 14
#define OFFSET (IP_OFFSET + UDP_OFFSET + ETH_OFFSET)
#define MSG_LEN (BF_LEN - OFFSET)

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

short CalculateChecksum(struct iphdr *iph) {
  long tmp, csum = 0;
  unsigned short *ptr = (short *)iph;

  for (int i = 0; i < 10; ++i) {
    csum += *ptr;
    ++ptr;
  }

  while ((tmp = csum >> 16)) {
    csum = (csum & 0xFFFF) + tmp;
  }

  return ~csum;
}

int main() {
  int cfd, err, if_index, sock_flag = 1;
  struct sockaddr_ll serv, client;
  socklen_t sock_size = sizeof(serv);
  struct ether_header *pcthdr;
  struct iphdr *iph;
  struct udphdr *udph;
  struct udphdr *udph_recv;
  char send_buf[BF_LEN] = {0};
  char recv_buf[BF_LEN] = {0};

  if_index = if_nametoindex(ETH_INTERFACE);
  if (if_index == 0) {
    handle_error("if_nametoindex()");
  }

  cfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (cfd == -1) {
    handle_error("socket():");
  }

  pcthdr = (struct ether_header *)(send_buf);
  ether_aton_r(SRC_MAC_ADDR, (struct ether_addr *)pcthdr->ether_shost);
  ether_aton_r(DST_MAC_ADDR, (struct ether_addr *)pcthdr->ether_dhost);
  pcthdr->ether_type = htons(ETH_P_IP);

  iph = (struct iphdr *)(send_buf + ETH_OFFSET);
  iph->ihl = 5;
  iph->ttl = MAXTTL;
  iph->version = IPVERSION;
  iph->protocol = IPPROTO_UDP;
  inet_pton(AF_INET, SRC_ADDR, &iph->saddr);
  inet_pton(AF_INET, DST_ADDR, &iph->daddr);
  iph->tot_len = htons(sizeof(send_buf) - ETH_OFFSET);
  iph->check = CalculateChecksum(iph);

  udph = (struct udphdr *)(send_buf + IP_OFFSET + ETH_OFFSET);
  udph->dest = htons(DST_PORT);
  udph->source = htons(SRC_PORT);
  udph->len = htons(sizeof(send_buf) - IP_OFFSET - ETH_OFFSET);

  memset(&serv, 0, sizeof(serv));
  serv.sll_halen = ETH_ALEN;
  serv.sll_family = AF_PACKET;
  serv.sll_ifindex = if_index;
  ether_aton_r(DST_MAC_ADDR, (struct ether_addr *)serv.sll_addr);

  while (1) {
    printf("type your message or type exit:\n  ->");
    scanf("%" BF_LEN_S "s", send_buf + OFFSET);
    if (strncmp(send_buf + OFFSET, "exit", 5) == 0) {
      break;
    }
    sendto(cfd, &send_buf, sizeof(send_buf), 0, (struct sockaddr *)&serv,
           sock_size);
    while (1) {
      recvfrom(cfd, &recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&client,
               &sock_size);
      udph_recv = (struct udphdr *)(recv_buf + IP_OFFSET + ETH_OFFSET);
      if (udph_recv->source == htons(DST_PORT)) {
        printf("returned message:\n  ->%s\n", recv_buf + OFFSET);
        break;
      }
    }
  }

  close(cfd);
  exit(EXIT_SUCCESS);
}