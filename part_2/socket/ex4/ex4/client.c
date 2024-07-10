#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <unistd.h>


#define PORT 2048
#define PORT_CL 2049
#define ADDR "192.168.56.2"
#define ETH_INTERFACE "enp0s8"

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

void WriteEthHeader(struct ether_header *pcthdr) {
  pcthdr->ether_shost[0] = 0x08;
  pcthdr->ether_shost[1] = 0x00;
  pcthdr->ether_shost[2] = 0x27;
  pcthdr->ether_shost[3] = 0xd1;
  pcthdr->ether_shost[4] = 0xe9;
  pcthdr->ether_shost[5] = 0x19;

  pcthdr->ether_dhost[0] = 0x08;
  pcthdr->ether_dhost[1] = 0x00;
  pcthdr->ether_dhost[2] = 0x27;
  pcthdr->ether_dhost[3] = 0x78;
  pcthdr->ether_dhost[4] = 0x85;
  pcthdr->ether_dhost[5] = 0x25;

  pcthdr->ether_type = htons(ETH_P_IP);
}

int main() {
  int cfd, err, if_index, sock_flag = 1;
  struct sockaddr_ll serv, client;
  socklen_t sock_size = sizeof(serv);
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

  struct ether_header *pcthdr = (struct ether_header *)(send_buf);
  WriteEthHeader(pcthdr);

  struct iphdr *iph = (struct iphdr *)(send_buf + ETH_OFFSET);
  iph->version  = IPVERSION;
  iph->ihl      = 5;
  iph->tot_len  = htons(sizeof(send_buf) - ETH_OFFSET);
  iph->ttl      = MAXTTL;
  iph->protocol = IPPROTO_UDP;
  iph->saddr    = inet_addr("192.168.56.3");
  inet_pton(AF_INET, ADDR, &iph->daddr);
  iph->check    = CalculateChecksum(iph);

  struct udphdr *udph = (struct udphdr *)(send_buf + IP_OFFSET + ETH_OFFSET);
  udph->source = htons(PORT_CL);
  udph->dest = htons(PORT);
  udph->len = htons(sizeof(send_buf) - IP_OFFSET - ETH_OFFSET);

  memset(&serv, 0, sizeof(serv));
  serv.sll_family = AF_PACKET;
  serv.sll_ifindex = if_index;
  serv.sll_halen = ETH_ALEN;
  serv.sll_addr[0] = 0x08;
  serv.sll_addr[1] = 0x00;
  serv.sll_addr[2] = 0x27;
  serv.sll_addr[3] = 0x78;
  serv.sll_addr[4] = 0x85;
  serv.sll_addr[5] = 0x25;

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
      struct udphdr *udph_recv =
          (struct udphdr *)(recv_buf + IP_OFFSET + ETH_OFFSET);
      if (udph_recv->source == htons(PORT)) {
        printf("returned message:\n  ->%s\n", recv_buf + OFFSET);
        break;
      }
    }
  }

  close(cfd);
  exit(EXIT_SUCCESS);
}