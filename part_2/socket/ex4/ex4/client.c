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
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>

#define PORT       2048
#define PORT_CL    2049
#define ADDR       "127.0.0.1"

#define BF_LEN     143
#define BF_LEN_S   "100"

#define IP_OFFSET  20
#define UDP_OFFSET 8
#define PCT_OFFSET 14
#define OFFSET     (IP_OFFSET + UDP_OFFSET + PCT_OFFSET)
#define MSG_LEN    (BF_LEN - OFFSET)

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

short GetChecksum(struct iphdr *iph) {
  int tmp, csum = 0;
  short *ptr = (short *)iph;

  for(int i = 0; i < 10; ++i) {
    csum += *ptr;
    ++ptr;
  }

  while((tmp = csum >> 16)) {
    csum = (csum & 0xFFFF) + tmp;
  }

  return ~csum;
}

int main() {
  int cfd, err, addr, if_index, sock_flag = 1;
  struct sockaddr_ll serv;
  socklen_t sock_size = sizeof(serv);
  char send_buf[BF_LEN] = {0};
  char recv_buf[BF_LEN] = {0};

  if_index = if_nametoindex("enp0s3");
  if(if_index == 0) {
    handle_error("if_nametoindex()");
  }

  inet_pton(AF_INET, ADDR, &addr);

  cfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (cfd == -1) {
    handle_error("socket():");
  }

  struct ether_header *pcthdr = (struct ether_header *)(send_buf);
  pcthdr->ether_shost[0] = 0x08;
  pcthdr->ether_shost[1] = 0x00;
  pcthdr->ether_shost[2] = 0x27;
  pcthdr->ether_shost[3] = 0x96;
  pcthdr->ether_shost[4] = 0xca;
  pcthdr->ether_shost[5] = 0xfe;

  pcthdr->ether_dhost[0] = 0x08;
  pcthdr->ether_dhost[1] = 0x00;
  pcthdr->ether_dhost[2] = 0x27;
  pcthdr->ether_dhost[3] = 0x96;
  pcthdr->ether_dhost[4] = 0xca;
  pcthdr->ether_dhost[5] = 0xfe;

  pcthdr->ether_type = htons(ETH_P_IP);


  struct iphdr *iph = (struct iphdr *)(send_buf + PCT_OFFSET);
  iph->version  = IPVERSION;
  iph->ihl      = 5;
  iph->tot_len  = sizeof(send_buf);
  iph->ttl      = MAXTTL;
  iph->protocol = IPPROTO_UDP;
  iph->saddr    = addr;
  iph->daddr    = addr;
  iph->check    = GetChecksum(iph);

  struct udphdr *udph = (struct udphdr *)(send_buf + IP_OFFSET + PCT_OFFSET);
  udph->source  = htons(PORT_CL);
  udph->dest    = htons(PORT);
  udph->len     = htons(sizeof(send_buf) - IP_OFFSET - PCT_OFFSET);

  memset(&serv, 0, sizeof(serv));
  serv.sll_family  = AF_PACKET;
  serv.sll_ifindex = if_index;
  serv.sll_halen   = ETH_ALEN;
  serv.sll_addr[0] = 0x08;
  serv.sll_addr[1] = 0x00;
  serv.sll_addr[2] = 0x27;
  serv.sll_addr[3] = 0x96;
  serv.sll_addr[4] = 0xca;
  serv.sll_addr[5] = 0xfe;

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
      struct udphdr *udph_recv = (struct udphdr *)(recv_buf + IP_OFFSET + PCT_OFFSET);
      if (udph_recv->source == htons(PORT)) {
        printf("returned message:\n  ->%s\n", recv_buf + OFFSET);
        break;
      }
    }
  }

  close(cfd);
  exit(EXIT_SUCCESS);
}