#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define MAX_EVENTS 10
#define ADDR "127.0.0.1"
#define DEFAULT_PORT_TCP 2048
#define DEFAULT_PORT_UDP 2049

#define handle_error(msg) \
  do {                    \
    perror(msg);          \
    exit(EXIT_FAILURE);   \
  } while (0)

void GetTime(char *buf, int n) {
  time_t rawtime;
  struct tm *timeinfo;
  memset(buf, 0, n);

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  asctime_r(timeinfo, buf);
}

void SetSockAddr(struct sockaddr_in *serv, const char *addr, int port) {
  int coking_addr = 0;
  inet_pton(AF_INET, addr, &coking_addr);
  memset(serv, 0, sizeof(*serv));

  serv->sin_family = AF_INET;
  serv->sin_port = htons(port);
  serv->sin_addr.s_addr = coking_addr;
}

int CreateSockFD(int domain, int type) {
  int sfd = socket(domain, type, 0);
  if (sfd == -1){
    handle_error("socket error:");
  }
  return sfd;
}

void EpollAdd(int epollfd, int fd, int events) {
  struct epoll_event ev;
  ev.events = events;
  ev.data.fd = fd;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1){
    handle_error("epoll_ctl error:");
  }
}

int main() {
  char buf[50];
  struct epoll_event events[MAX_EVENTS];
  int udp_listen_sock, tcp_listen_sock, tcp_conn_sock, event_count, epollfd;
  struct sockaddr_in udp_sockaddr, tcp_sockaddr, addr;
  socklen_t addrlen = sizeof(addr);

  SetSockAddr(&tcp_sockaddr, ADDR, DEFAULT_PORT_TCP);
  SetSockAddr(&udp_sockaddr, ADDR, DEFAULT_PORT_UDP);

  tcp_listen_sock = CreateSockFD(AF_INET, SOCK_STREAM);
  udp_listen_sock = CreateSockFD(AF_INET, SOCK_DGRAM);

  if(bind(tcp_listen_sock, (struct sockaddr *) &tcp_sockaddr, sizeof(tcp_sockaddr)) == -1) {
    handle_error("bind error");
  }

  if(bind(udp_listen_sock, (struct sockaddr *) &udp_sockaddr, sizeof(udp_sockaddr)) == -1) {
    handle_error("bind error");
  }

  if (listen(tcp_listen_sock, 10) == -1){
    handle_error("listen error:");
  }

  epollfd = epoll_create1(0);
  if(epollfd == -1) {
    handle_error("epoll_create1 error:");
  }

  EpollAdd(epollfd, tcp_listen_sock, EPOLLIN);
  EpollAdd(epollfd, udp_listen_sock, EPOLLIN);

  for (;;) {
    event_count = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (event_count == -1) {
      handle_error("epoll_wait error:");
    }

    for (int n = 0; n < event_count; ++n) {
      if (events[n].data.fd == tcp_listen_sock) {
        tcp_conn_sock = accept(tcp_listen_sock, (struct sockaddr *) &addr, &addrlen);
        if(tcp_conn_sock == -1) {
          handle_error("accept error:");
        }
        GetTime(buf, sizeof(buf));
        send(tcp_conn_sock, buf, sizeof(buf), 0);
        recv(tcp_conn_sock, buf, sizeof(buf), 0);
        
        close(tcp_conn_sock);
      } else if (events[n].data.fd == udp_listen_sock) {
        recvfrom(udp_listen_sock, buf, sizeof(buf), 0, (struct sockaddr *) &addr, &addrlen);
        GetTime(buf, sizeof(buf));
        sendto(udp_listen_sock, buf, sizeof(buf), 0, (struct sockaddr *) &addr, addrlen);
      }
    }
  }

  exit(EXIT_SUCCESS);
}